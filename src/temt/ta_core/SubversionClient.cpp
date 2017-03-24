// Copyright, 2012, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "SubversionClient.h"
#include <iNetworkAccessManager>

#include <int_PArray>
#include <taMisc>
#include <taiMisc>

#include <taGuiDialog>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <QUrl>
#include <iTextEdit>
#include <QBoxLayout>

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <apr_pools.h>
#include <svn_client.h>
#include <svn_path.h>
#include <svn_pools.h>
#include <svn_props.h>
#include <svn_time.h>
#include <svn_wc.h>
#include <svn_sorts.h>
#include <apr_xlate.h> // for APR_*_CHARSET
#include <apr_hash.h>
namespace {
  template<typename T>
  std::string
  toString(const T &val)
  {
    std::ostringstream oss;
    oss << val;
    return oss.str();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Apr
///////////////////////////////////////////////////////////////////////////////
class Apr
{
public:
  static void initializeOnce();
private:
  Apr();
  ~Apr();
};

void Apr::initializeOnce()
{
  static Apr apr;
}

Apr::Apr()
{
  apr_status_t status = apr_initialize();
  if (status != APR_SUCCESS) {
    throw SubversionClient::Exception(
      "Failed to initialize APR: " + toString(status));
  }
}

Apr::~Apr()
{
  apr_terminate();
  // this happens in full program global destructors -- cannot send such a message then!
  // taMisc::Info("Terminated APR");
}

///////////////////////////////////////////////////////////////////////////////
// SubversionClient::Exception
///////////////////////////////////////////////////////////////////////////////

namespace {
  std::string
  createMessage(const std::string &additional_msg, svn_error_t *svn_error)
  {
    std::string msg;
    msg.reserve(500);
    msg += additional_msg;
    if (svn_error) {
      msg += "\nError #";
      msg += toString(svn_error->apr_err);
      if (svn_error->message) {
        msg += ": ";
        msg += svn_error->message;
      }
    }
    return msg;
  }
}

SubversionClient::Exception::Exception(svn_error_t *svn_error)
  : std::runtime_error(svn_error->message ? svn_error->message : "Error")
  , m_error_code(toEmerErrorCode(svn_error))
  , m_svn_error_code(svn_error->apr_err)
{
  if(taiMisc::busy_count > 0)   // if we excepted
    taMisc::DoneBusy();
  svn_error_clear(svn_error);
}

SubversionClient::Exception::Exception(
  const std::string &additional_msg,
  svn_error_t *svn_error
)
  : std::runtime_error(createMessage(additional_msg, svn_error))
  , m_error_code(toEmerErrorCode(svn_error))
  , m_svn_error_code(svn_error->apr_err)
{
  // if(taiMisc::busy_count > 0)   // if we excepted
  //   taMisc::DoneBusy();
  svn_error_clear(svn_error);
}

SubversionClient::Exception::Exception(
  const std::string &msg,
  ErrorCode error_code,
  int svn_error_code
)
  : std::runtime_error(msg)
  , m_error_code(error_code)
  , m_svn_error_code(svn_error_code)
{
  if(taiMisc::busy_count > 0)   // if we excepted
    taMisc::DoneBusy();
}

SubversionClient::ErrorCode
SubversionClient::Exception::GetErrorCode() const
{
  return m_error_code;
}

int
SubversionClient::Exception::GetSvnErrorCode() const
{
  return m_svn_error_code;
}

// Convert certain SVN error codes to emergent error codes; the idea is that
// client code shouldn't need to #include <svn_error_codes.h>, it can just
// use the codes defined by the SubversionClient::ErrorCode enumeration.
SubversionClient::ErrorCode
SubversionClient::Exception::toEmerErrorCode(svn_error_t *svn_error)
{
  switch (svn_error->apr_err) {
  default:                    return EMER_GENERAL_SVN_ERROR;
  case SVN_ERR_CANCELLED:     return EMER_OPERATION_CANCELLED;
  case SVN_ERR_ENTRY_EXISTS:  return EMER_ERR_ENTRY_EXISTS;
  case SVN_ERR_CLIENT_NO_VERSIONED_PARENT: return EMER_ERR_CLIENT_NO_VERSIONED_PARENT;
  case SVN_ERR_FS_ALREADY_EXISTS: return EMER_ERR_FS_ALREADY_EXISTS;

  // On Ubuntu 10.4, if the user cancels authentication, it causes a 170001
  // error (SVN_ERR_RA_NOT_AUTHORIZED).  We could translate that into
  // EMER_OPERATION_CANCELLED, but it seems likely that something as
  // generic as "not authorized" could also occur in other cirumstances.

  case SVN_ERR_RA_DAV_REQUEST_FAILED:
    if (svn_error->message) {
      if (std::strstr(svn_error->message, "403 Forbidden")) {
        return EMER_FORBIDDEN;
      }
      if (std::strstr(svn_error->message, "405 Method Not Allowed")) {
        // Probably...
        return EMER_ERR_ENTRY_EXISTS;
      }
    }
    return EMER_GENERAL_SVN_ERROR;
  }
}

///////////////////////////////////////////////////////////////////////////////
// SubversionClient::Glue
///////////////////////////////////////////////////////////////////////////////

// Note: Should never throw exceptions in callbacks, since the C code
// won't know what to do with them.

struct SubversionClient::Glue
{
  // Glue code to connect C-style callbacks with C++ object methods.
  static
  svn_error_t *
  svn_cancel_func(void *cancel_baton)
  {
    SubversionClient *sub = reinterpret_cast<SubversionClient *>(cancel_baton);
    if (sub && sub->isCancelled()) {
      return svn_error_create(SVN_ERR_CANCELLED, 0, "Operation cancelled");
    }
    else {
      // Docs for svn_cancel_func_t say to return SVN_NO_ERROR, but
      // looking at the SVN_ERR macro suggests this makes more sense.
      // (SVN_NO_ERROR happens to be #define 0... lovely.)
      return 0;
    }
  }

  static
  void
  svn_wc_notify_func2(
    void *baton,
    const svn_wc_notify_t *notify,
    apr_pool_t *pool)
  {
    if (SubversionClient *sub = reinterpret_cast<SubversionClient *>(baton)) {
      sub->notify(notify);
    }
  }

  static
  void
  svn_ra_progress_notify_func(
    apr_off_t progress,
    apr_off_t total,
    void *baton,
    apr_pool_t *pool)
  {
    if (SubversionClient *sub = reinterpret_cast<SubversionClient *>(baton)) {
      // progress is the number of bytes already transferred, total is
      // the total number of bytes to transfer or -1 if it's not known.
      sub->notifyProgress(progress, total);
    }
  }

  static
  String
  replaceLineEndingsWithLF(const String&msg)
  {
    String lfMsg;
    for (unsigned i = 0; i < msg.length(); ++i) {
      if (msg[i] == '\r') { // CR
        if (i + 1 < msg.length() && msg[i] == '\n') { // LF
          // Skip the CR, next char is LF.
        }
        else {
          lfMsg += '\r'; // Replace CR with LF.
        }
      }
      else {
        lfMsg += msg[i]; // Copy all other characters.
      }
    }
    return lfMsg;
  }

  static
  svn_error_t *
  svn_client_get_commit_log3(
    const char **log_msg,
    const char **tmp_file,
    const apr_array_header_t *commit_items,
    void *baton,
    apr_pool_t *pool)
  {
    *tmp_file = 0;
    *log_msg = 0; // Should cause the operation to be cancelled if not set.
    if (SubversionClient *sub = reinterpret_cast<SubversionClient *>(baton)) {

      String com_itm_str;
      for (int i = 0; i < commit_items->nelts; i++) {
        svn_client_commit_item3_t *item
          = APR_ARRAY_IDX(commit_items, i, svn_client_commit_item3_t *);
        String path;
        char text_mod = '_', prop_mod = ' ';

        if(item->path)
          path = item->path;
        else if (item->url)
          path = item->url;
        else 
          path = ".";

        if ((item->state_flags & SVN_CLIENT_COMMIT_ITEM_DELETE)
            && (item->state_flags & SVN_CLIENT_COMMIT_ITEM_ADD))
          text_mod = 'R';
        else if (item->state_flags & SVN_CLIENT_COMMIT_ITEM_ADD)
          text_mod = 'A';
        else if (item->state_flags & SVN_CLIENT_COMMIT_ITEM_DELETE)
          text_mod = 'D';
        else if (item->state_flags & SVN_CLIENT_COMMIT_ITEM_TEXT_MODS)
          text_mod = 'M';

        if (item->state_flags & SVN_CLIENT_COMMIT_ITEM_PROP_MODS)
          prop_mod = 'M';

        path += String(" ") + String(text_mod) + String(prop_mod);
        if (item->state_flags & SVN_CLIENT_COMMIT_ITEM_IS_COPY)
          path += "+ ";
        else
          path += " ";
        com_itm_str += path + "\n";
      }
      String msg = sub->getCommitMessage(com_itm_str);
      if (!msg.empty()) {
        // TODO: Remove following line once testing is complete.
        // It's only here so we don't spam the dev email list.
        //        msg += " (quiet)";

        // Subversion requires that only line feeds be used, no CRLF or CR.
        msg = replaceLineEndingsWithLF(msg);
        *log_msg = apr_pstrdup(pool, msg);
      }
    }
    return 0;
  }

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  static
  svn_error_t *
  svn_wc_conflict_resolver_func(
    svn_wc_conflict_result_t **result,
    const svn_wc_conflict_description_t *description,
    void *baton,
    apr_pool_t *pool)
  {
    // TODO: basically unimplemented.
    *result = svn_wc_create_conflict_result(
      svn_wc_conflict_choose_postpone, 0, pool);
    return 0;
  }

#else
  static
  svn_error_t *
  svn_wc_conflict_resolver_func2(
    svn_wc_conflict_result_t **result,
    const svn_wc_conflict_description2_t *description,
    void *baton,
    apr_pool_t *result_pool,
    apr_pool_t *scratch_pool)
  {
    // TODO: basically unimplemented.
    *result = svn_wc_create_conflict_result(
      svn_wc_conflict_choose_postpone, 0, result_pool);
    return 0;
  }
#endif

  static
  svn_error_t *
  svn_info_receiver(
    void *baton,
    const char *path,
    const svn_info_t *info,
    apr_pool_t *pool)
  {
    if (SubversionClient *sub = reinterpret_cast<SubversionClient *>(baton)) {
      sub->m_last_changed_revision = -1;
      if (info) {
        // Could extract a lot more out of this, but this suffices for now.
        sub->m_last_changed_revision = info->last_changed_rev;
      }
    }
    return 0;
  }

  //////////////////// Authentication functions ////////////////////

  static
  svn_error_t *
  svn_auth_simple_prompt_func(
    svn_auth_cred_simple_t **ppCred,
    void *, // baton,
    const char *realm,
    const char *username,
    svn_boolean_t may_save,
    apr_pool_t *pool)
  {
    QString qUser(username);
    QString qPass;
    QString qMessage("Enter Subversion credentials.");
    if (realm) {
      qMessage.append("\n").append(realm);
    }

    // Only prompt the user to save their credentials if the may_save
    // parameter indicates we're allowed to.
    bool saveFlag = false;
    bool *pSaveFlag = may_save ? &saveFlag : 0;

    // GUI prompt for credentials.
    if (getUsernamePassword(qUser, qPass, qMessage, pSaveFlag)) {
      // User hit OK; build credential struct.
      svn_auth_cred_simple_t *pCred = (svn_auth_cred_simple_t *)
        apr_pcalloc(pool, sizeof(svn_auth_cred_simple_t));
      *ppCred = pCred;
      pCred->username = apr_pstrdup(pool, qPrintable(qUser));
      pCred->password = apr_pstrdup(pool, qPrintable(qPass));
      pCred->may_save = saveFlag;
      return 0;
    }

    // User cancelled credentials dialog.
    return svn_error_create(SVN_ERR_CANCELLED, 0, "Operation cancelled");
  }

  static
  svn_error_t *
  svn_auth_username_prompt_func(
    svn_auth_cred_username_t **ppCred,
    void *, // baton,
    const char *realm,
    svn_boolean_t may_save,
    apr_pool_t *pool)
  {
    QString qUser;
    QString qPass;
    QString qMessage("Enter Subversion username.");
    if (realm) {
      qMessage.append("\n").append(realm);
    }

    // Only prompt the user to save their credentials if the may_save
    // parameter indicates we're allowed to.
    bool saveFlag = false;
    bool *pSaveFlag = may_save ? &saveFlag : 0;

    // GUI prompt for credentials.
    if (getUsernamePassword(qUser, qPass, qMessage, pSaveFlag, 0, true)) {
      // User hit OK; build credential struct.
      svn_auth_cred_username_t *pCred = (svn_auth_cred_username_t *)
        apr_pcalloc(pool, sizeof(svn_auth_cred_username_t));
      *ppCred = pCred;
      pCred->username = apr_pstrdup(pool, qPrintable(qUser));
      pCred->may_save = saveFlag;
      return 0;
    }

    // User cancelled credentials dialog.
    return svn_error_create(SVN_ERR_CANCELLED, 0, "Operation cancelled");
  }

  static
  svn_error_t *
  svn_auth_plaintext_prompt_func(
    svn_boolean_t *may_save_plaintext,
    const char *, // realmstring,
    void *, // baton,
    apr_pool_t *) // pool)
  {
    // TODO: prompt user for permission to cache their password in the clear.
    *may_save_plaintext = true;
    return 0;
  }
};

///////////////////////////////////////////////////////////////////////////////
// SubversionClient
///////////////////////////////////////////////////////////////////////////////

SubversionClient::SubversionClient()
  : m_wc_path(0)
  , m_url(0)
  , m_pool_perm(0) // initialized below
  , m_ctx(0)
  , m_cancelled(false)
  , m_commit_message(0)
  , m_last_changed_revision(-1)
{
  // Set up APR and a pool.
  Apr::initializeOnce(); // must be done prior to any other APR calls.
  m_pool_perm = svn_pool_create(0);
  
  svn_operation = new QMutex();

  // Set the error handler to one that doesn't abort!
  // We'll check for these errors and convert to exceptions.
#if SVN_VER_MINOR >= 6
  svn_error_set_malfunction_handler(svn_error_raise_on_malfunction);
#endif

  // Create a context object.  Throws on failure.
  m_ctx = createContext();
}

SubversionClient::~SubversionClient()
{
  // No need to destroy the context since it was allocated from
  // the pool, which is about to be destroyed.
  m_ctx = 0;

  // Clean up the pool and APR context.
  svn_pool_destroy(m_pool_perm);
}

void
SubversionClient::SetWorkingCopyPath(const String& working_copy_path)
{
  // Canonicalize the working copy path.
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  m_wc_path = svn_path_canonicalize(working_copy_path, m_pool_perm);
#else
  m_wc_path = svn_dirent_canonicalize(working_copy_path, m_pool_perm);
#endif
}

std::string
SubversionClient::GetWorkingCopyPath() const
{
  return m_wc_path.chars();
}

namespace { // anonymous, helpers for GetUsername().
  QString
  urlToRealmString(const QString &url)
  {
#if (QT_VERSION >= 0x040600)
    QUrl qurl(QUrl::fromUserInput(url));
#else
    QUrl qurl(url);
#endif

    QString scheme = qurl.scheme();
    int defaultPort = 80;
    if (scheme == "https") {
      defaultPort = 443;
    }
    else if (scheme == "svn") {
      defaultPort = 3690;
    }

    int port = qurl.port(defaultPort);
    QString host = qurl.host();
    QString realm = QString("<%1://%2:%3>").arg(scheme).arg(host).arg(port);
    return realm;
  }

  QFileInfoList
  getAllFilesUnder(const QString &directoryPath)
  {
    QFileInfoList files;
    QFileInfoList dirs;
    dirs << QFileInfo(directoryPath);
    while (!dirs.isEmpty()) {
      QDir d(dirs.first().absoluteFilePath());
      dirs.removeFirst();

      // Get files in this dir.
      d.setFilter(QDir::Files);
      files.append(d.entryInfoList());

      // Get subdirs.
      d.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
      dirs.append(d.entryInfoList());
    }
    return files;
  }

  QString
  findUsernameInAuthFileMatchingRealm(
    const QFileInfoList &authFiles,
    const QString &realmString)
  {
    foreach (const QFileInfo &fi, authFiles) {
      QFile file(fi.absoluteFilePath());
      if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream text(&file);
        QString username;
        bool realmMatches = false;
        while (true) { // Read all lines of the file.
          QString line = text.readLine();
          if (line.isNull()) break; // EOF
          if (line == SVN_CONFIG_REALMSTRING_KEY) { // "svn:realmstring"
            line = text.readLine();
            line = text.readLine();
            realmMatches = line.contains(realmString, Qt::CaseInsensitive);
          }
          else if (line == SVN_CONFIG_OPTION_USERNAME) { // "username"
            line = text.readLine();
            line = text.readLine();
            username = line;
          }

          if (realmMatches && !username.isEmpty()) {
            return username;
          }
        }
      }
    }
    return "";
  }
}

std::string
SubversionClient::GetUsername(const String& url, UsernameSource source) const
{
  QString realmString = urlToRealmString(url);

  // Check the cache first.
  if (source == CHECK_CACHE_THEN_PROMPT_USER ||
      source == CHECK_CACHE_ONLY)
  {
    const char *userAuthDirPath = 0;
    if (svn_error_t *error = svn_config_get_user_config_path(
          &userAuthDirPath,
          0, // config_dir
          SVN_CONFIG_SECTION_AUTH,
          m_pool_perm))
    {
      // Couldn't get config dir, so no way to check the cache.
      // Ignore this error and skip down to the PROMPT logic.
      svn_error_clear(error);
    }
    else if (userAuthDirPath && *userAuthDirPath) {
      // There's gotta be an easier way to get the username for a given URL!!
      // Can't just lookup the file based on the realmString, since its format
      // may change based on the client and/or server in use!  This is pretty
      // brute force.
      QFileInfoList authFiles = getAllFilesUnder(userAuthDirPath);
      QString username =
        findUsernameInAuthFileMatchingRealm(authFiles, realmString);
      if (!username.isEmpty()) {
        // Got a non-empty username so return it.
        return username.toStdString();
      }
    }
  }

  // Then prompt the user.
  if (source == CHECK_CACHE_THEN_PROMPT_USER ||
      source == PROMPT_USER_ONLY)
  {
    QString qUser;
    QString qPass;
    QString qMessage("Enter Subversion username.");
    qMessage.append("\n").append(realmString);
    if (getUsernamePassword(qUser, qPass, qMessage, 0, 0, true)) {
      return qUser.toStdString();
    }
  }

  // Oops.
  return "";
}

svn_client_ctx_t *
SubversionClient::createContext()
{
  // Allocate a new context object from the pool.
  svn_client_ctx_t *ctx = 0;
  if (svn_error_t *error = svn_client_create_context(&ctx, m_pool_perm)) {
    // The documentation says this call won't error in current implementation,
    // but in case a future implementation does error, we should handle it.
    // Don't want calling code to have an uninitialized SubversionClient
    // object.
    throw Exception("Subversion error creating context", error);
  }

  // Initialize the context with batons and callbacks and such.
  ctx->client_name = "emergent";

  // Get the user configuration directory (e.g., ~/.subversion/).
  const char *userConfigDirPath = 0;
  if (svn_error_t *error = svn_config_get_user_config_path(
        &userConfigDirPath,
        0, // config_dir
        0, // set to SVN_CONFIG_CATEGORY_CONFIG to get the config file.
        m_pool_perm))
  {
    throw Exception("Subversion error getting configuration directory", error);
  }

  // Initialize the context's configuration hash.
  ctx->config = 0;
  if (svn_error_t *error = svn_config_get_config(
        &ctx->config,
        userConfigDirPath,
        m_pool_perm))
  {
    throw Exception("Subversion error getting configuration info", error);
  }

  if (!ctx->config) {
    throw Exception("Subversion did not populate ctx->config.");
  }

  // Initialize the authentication baton.
  apr_array_header_t *providers = createAuthProviders(ctx->config);
  ctx->auth_baton = 0;
  svn_auth_open(&ctx->auth_baton, providers, m_pool_perm);

  // Tell the auth_baton where to cache credentials.
  svn_auth_set_parameter(
    ctx->auth_baton,
    SVN_AUTH_PARAM_CONFIG_DIR,
    userConfigDirPath);

  ctx->mimetypes_map = 0; // TODO: do I need to implement this somehow?

  ctx->cancel_func = Glue::svn_cancel_func;
  ctx->cancel_baton = this;

  ctx->notify_func = 0; // Deprecated in 1.2.
  ctx->notify_baton = 0;
  ctx->notify_func2 = Glue::svn_wc_notify_func2;
  ctx->notify_baton2 = this;

  ctx->progress_func = Glue::svn_ra_progress_notify_func;
  ctx->progress_baton = this;

  ctx->log_msg_func = 0; // Deprecated in 1.3.
  ctx->log_msg_baton = 0;
  ctx->log_msg_func2 = 0; // Deprecated in 1.5.
  ctx->log_msg_baton2 = 0;
  ctx->log_msg_func3 = Glue::svn_client_get_commit_log3;
  ctx->log_msg_baton3 = this;

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  ctx->conflict_func = Glue::svn_wc_conflict_resolver_func;
  ctx->conflict_baton = this;
#else
  ctx->conflict_func = 0; // Deprecated in 1.7.
  ctx->conflict_baton = 0;
  ctx->conflict_func2 = Glue::svn_wc_conflict_resolver_func2;
  ctx->conflict_baton2 = this;
#endif

  return ctx;
}

apr_array_header_t *
SubversionClient::createAuthProviders(apr_hash_t *config)
{
  // Based on code in
  // subversion-1.7.7/subversion/bindings/javahl/native/ClientContext.cpp

  // Get the svn_config_t configuration data object from the hash.
  svn_config_t *configData = reinterpret_cast<svn_config_t *>(apr_hash_get(
    config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING));

  // Get the list of authentication providers.
  apr_array_header_t *providers = 0;
  if (svn_error_t *error = svn_auth_get_platform_specific_client_providers(
        &providers,
        configData,
        m_pool_perm))
  {
    throw Exception("Subversion error getting authentication providers", error);
  }

  if (!providers) {
    throw Exception("Subversion did not provide authentication providers.");
  }

  // Add auth providers that can check the user's ~/.subversion directory
  // for cached credentials.
  svn_auth_provider_object_t *provider = 0;
  svn_auth_get_simple_provider2(
    &provider, Glue::svn_auth_plaintext_prompt_func, 0, m_pool_perm);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  svn_auth_get_username_provider(&provider, m_pool_perm);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

    /* The server-cert, client-cert, and client-cert-password providers. */
  if (svn_error_t *error = svn_auth_get_platform_specific_provider(
        &provider,
        "windows",
        "ssl_server_trust",
        m_pool_perm))
  {
    // Not fatal, just clear the error.
    svn_error_clear(error);
  }
  else if (provider) {
    // If the cal succeeded, then put the provider on the list.
    APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  }

  svn_auth_get_ssl_server_trust_file_provider(&provider, m_pool_perm);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  svn_auth_get_ssl_client_cert_file_provider(&provider, m_pool_perm);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  svn_auth_get_ssl_client_cert_pw_file_provider2(
    &provider, Glue::svn_auth_plaintext_prompt_func, 0, m_pool_perm);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

  // Add our own simple username/password auth provider.
  svn_auth_get_simple_prompt_provider(
    &provider,
    Glue::svn_auth_simple_prompt_func,
    0, // baton
    1, // retries
    m_pool_perm);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

  svn_auth_get_username_prompt_provider(
    &provider,
    Glue::svn_auth_username_prompt_func,
    0, // baton
    1, // retries
    m_pool_perm);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

  return providers;
}

int
SubversionClient::Checkout(const String& url_in, const String& to_wc, int rev, bool recurse)
{
  m_cancelled = false;
  QMutexLocker qml(svn_operation);

  // Working copy doesn't exist yet, so create it by checking out the URL.

  if(to_wc.nonempty()) {
    SetWorkingCopyPath(to_wc);
  }

  apr_pool_t* m_pool = svn_pool_create(0);

  String url = url_in;
  
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  url = svn_uri_canonicalize(url, m_pool);
#endif

  // Out parameter -- the value of the revision checked out from the
  // repository.
  svn_revnum_t result_rev = 0;

  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_unspecified;

  // Set the revision number, if provided.  Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  if (rev < 0) {
    revision.kind = svn_opt_revision_head;
  }
  else {
    revision.kind = svn_opt_revision_number;
    revision.value.number = rev;
  }

  // Set the depth of subdirectories to be checked out.
  svn_depth_t depth = recurse ? svn_depth_infinity : svn_depth_empty;

  // Set advanced options we don't care about.
  svn_boolean_t ignore_externals = false;
  svn_boolean_t allow_unver_obstructions = true;

  if (svn_error_t *error = svn_client_checkout3
      (&result_rev, // out param
       url,
       m_wc_path,
       &peg_revision,
       &revision,
       depth,
       ignore_externals,
       allow_unver_obstructions,
       m_ctx,
       m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception
      (std::string("Subversion checkout error for URL ") + url.chars(), error);
  }
  svn_pool_destroy(m_pool);
  return result_rev;
}

class SvnFileInfoPtrs {
public:
  String_PArray* file_names;
  String_PArray* file_paths;
  int_PArray*    file_sizes;
  int_PArray*    file_revs;
  int_PArray*    file_times;
  int_PArray*    file_kinds;
  String_PArray* file_authors;
};

static svn_error_t* mysvn_list_callback(void *baton, const char *path,
                                        const svn_dirent_t *dirent,
                                        const svn_lock_t *lock, const char *abs_path,
                                        apr_pool_t *pool) {
  if(dirent == NULL)
    return NULL;
  SvnFileInfoPtrs* fi = (SvnFileInfoPtrs*)baton;
  fi->file_names->Add(path);
  fi->file_paths->Add(abs_path);
  fi->file_sizes->Add(dirent->size);
  fi->file_revs->Add(dirent->created_rev);
  fi->file_times->Add(dirent->time / 1000000); // microseconds -> seconds
  fi->file_kinds->Add(dirent->kind);
  fi->file_authors->Add(dirent->last_author);
  return NULL;
}

void
SubversionClient::List(String_PArray& file_names, String_PArray& file_paths,
                       int_PArray& file_sizes,
                       int_PArray& file_revs, int_PArray& file_times,
                       int_PArray& file_kinds, String_PArray& file_authors,
                       const String& url_in, int rev, bool recurse) {

  apr_pool_t* m_pool = svn_pool_create(0);

  String url = url_in;
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  url = svn_uri_canonicalize(url, m_pool);
#endif
  
  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;

  // Set the revision number, if provided.  Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  if (rev < 0) {
    revision.kind = svn_opt_revision_head;
    peg_revision.kind = svn_opt_revision_unspecified;
  }
  else {
    revision.kind = svn_opt_revision_number;
    revision.value.number = rev;
    peg_revision.kind = svn_opt_revision_number;
    peg_revision.value.number = rev;
  }

  // Set the depth of subdirectories to be checked out.
  svn_depth_t depth = recurse ? svn_depth_infinity : svn_depth_immediates;

  SvnFileInfoPtrs svn_fi_baton;
  svn_fi_baton.file_names = &file_names;
  svn_fi_baton.file_paths = &file_paths;
  svn_fi_baton.file_sizes = &file_sizes;
  svn_fi_baton.file_revs = &file_revs;
  svn_fi_baton.file_times = &file_times;
  svn_fi_baton.file_kinds = &file_kinds;
  svn_fi_baton.file_authors = &file_authors;

  if (svn_error_t *error = svn_client_list2
      (url,
       &peg_revision,
       &revision,
       depth,
       SVN_DIRENT_ALL,
       false,                   // don't fetch locks
       mysvn_list_callback,
       (void*)&svn_fi_baton,
       m_ctx,
       m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion list error", error);
    }
  svn_pool_destroy(m_pool);
}

class SvnGetInfoPtrs {
public:
  int*    rev;
  int*    kind;
  String* root_url;
  int*    last_changed_rev;
  int*    last_changed_date;
  String* last_changed_author;
  int64_t* size;
};

static svn_error_t* mysvn_info_callback(void *baton, const char *path,
                                        const svn_info_t *info,
                                        apr_pool_t *pool) {
  if(info == NULL)
    return NULL;
  SvnGetInfoPtrs* fi = (SvnGetInfoPtrs*)baton;
  *(fi->rev) = info->rev;
  *(fi->kind) = info->kind;
  *(fi->root_url) = info->URL;
  *(fi->last_changed_rev) = info->last_changed_rev;
  *(fi->last_changed_date) = info->last_changed_date / 1000000; // microseconds -> seconds
  *(fi->last_changed_author) = info->last_changed_author;
  *(fi->size) = info->size;
  return NULL;
}

bool
SubversionClient::GetInfo(const String& file_or_dir_or_url, int& rev, int& kind,
                          String& root_url, int& last_changed_rev, int& last_changed_date,
                          String& last_changed_author, int64_t& size) {
  apr_pool_t* m_pool = svn_pool_create(0);
  QMutexLocker qml(svn_operation);

  String path_in = file_or_dir_or_url;
  bool is_url = false;
  if(path_in.contains("http"))
    is_url = true;
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  if(is_url) {
    path_in = svn_uri_canonicalize(path_in, m_pool);
  }
#endif
  
  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_unspecified;
  // get HEAD revision.
  svn_opt_revision_t revision;
  if(is_url) {
    revision.kind = svn_opt_revision_head;
  }
  else {
    revision.kind = svn_opt_revision_unspecified; // just uses local info -- doesn't go out over network
  }

  // Set the depth of subdirectories to be checked out.
  svn_depth_t depth = svn_depth_empty;

  SvnGetInfoPtrs svn_fi_baton;
  svn_fi_baton.rev = &rev;
  svn_fi_baton.kind = &kind;
  svn_fi_baton.root_url = &root_url;
  svn_fi_baton.last_changed_rev = &last_changed_rev;
  svn_fi_baton.last_changed_date = &last_changed_date;
  svn_fi_baton.last_changed_author = &last_changed_author;
  svn_fi_baton.size = &size;

  if (svn_error_t *error = svn_client_info2
      (path_in,
       &peg_revision,
       &revision,
       mysvn_info_callback,
       (void*)&svn_fi_baton,
       depth,
       NULL,
       m_ctx,
       m_pool))
    {
      svn_pool_destroy(m_pool);
      return false;
    }
  svn_pool_destroy(m_pool);
  return true;
}


bool
SubversionClient::UrlExists(const String& url, int& rev) {
  int kind, last_changed_rev, last_changed_date;
  String root_url, last_changed_author;
  int64_t sz;
  rev = 0;
  bool rval = GetInfo(url, rev, kind, root_url, last_changed_rev, last_changed_date,
                      last_changed_author, sz);
  if(!rval || rev <= 0) return false;
  return true;
}


bool
SubversionClient::IsWCRevSameAsHead(const String& file_or_dir, int& wc_rev, int& head_rev) {
  wc_rev = 0;
  head_rev = 0;
  int kind, last_changed_rev, last_changed_date;
  String wc_url, root_url, last_changed_author;
  int64_t sz;
  QFileInfo fi(file_or_dir.toQString());
  if(!fi.exists()) {
    return false;
  }
  GetInfo(file_or_dir, wc_rev, kind, wc_url, last_changed_rev, last_changed_date,
          last_changed_author, sz);
  GetInfo(wc_url, head_rev, kind, root_url, last_changed_rev, last_changed_date,
          last_changed_author, sz);
  return (wc_rev == head_rev);
}  


void 
SubversionClient::GetFile(const String& from_url, String& to_str, int rev) {

  apr_pool_t* m_pool = svn_pool_create(0);
  QMutexLocker qml(svn_operation);

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  from_url = svn_uri_canonicalize(from_url, m_pool);
#endif

  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;

  // Set the revision number, if provided.  Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  if (rev < 0) {
    revision.kind = svn_opt_revision_head;
    peg_revision.kind = svn_opt_revision_unspecified;
  }
  else {
    revision.kind = svn_opt_revision_number;
    revision.value.number = rev;
    peg_revision.kind = svn_opt_revision_number;
    peg_revision.value.number = rev;
  }

  apr_pool_t* strpool = svn_pool_create(0);

  svn_stringbuf_t* stringbuf = svn_stringbuf_create_ensure(1024, strpool);
  svn_stream_t *out_strm = svn_stream_from_stringbuf(stringbuf, strpool);

  if (svn_error_t *error = svn_client_cat2
      (out_strm,
       from_url,
       &peg_revision,
       &revision,
       m_ctx,
       m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion SaveFile cat2 error", error);
    }

  svn_stream_close(out_strm);
  to_str = stringbuf->data;
  svn_pool_destroy(strpool);
  svn_pool_destroy(m_pool);
}

void 
SubversionClient::SaveFile(const String& from_url, const String& to_path, int rev) {

  apr_pool_t* m_pool = svn_pool_create(0);
  QMutexLocker qml(svn_operation);

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  from_url = svn_uri_canonicalize(from_url, m_pool);
#endif

  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;

  // Set the revision number, if provided.  Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  if (rev < 0) {
    revision.kind = svn_opt_revision_head;
    peg_revision.kind = svn_opt_revision_unspecified;
  }
  else {
    revision.kind = svn_opt_revision_number;
    revision.value.number = rev;
    peg_revision.kind = svn_opt_revision_number;
    peg_revision.value.number = rev;
  }

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  const char* to_path_cln = svn_path_canonicalize(to_path, m_pool);
#else
  const char* to_path_cln = svn_dirent_canonicalize(to_path, m_pool);
#endif

  svn_stream_t* out_strm;

  if(svn_error_t* error = svn_stream_open_writable
     (&out_strm,
      to_path_cln,
      m_pool,
      m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion SaveFile error -- couldn't write file", error);
    }

  if (svn_error_t *error = svn_client_cat2
      (out_strm,
       from_url,
       &peg_revision,
       &revision,
       m_ctx,
       m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion SaveFile cat2 error", error);
    }

  svn_stream_close(out_strm);
  svn_pool_destroy(m_pool);
}

void 
SubversionClient::GetDiffToPrev(const String& from_url, String& to_str, int rev) {
  apr_pool_t* m_pool = svn_pool_create(0);

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  from_url = svn_uri_canonicalize(from_url, m_pool);
#endif

  if(rev < 0) {
    taMisc::Error("GetDiffToPrev: must specify revision to diff to explicitly -- can't use -1");
    return;
  }

  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_number;
  peg_revision.value.number = rev;

  // Set the revision number, if provided.  Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  revision.kind = svn_opt_revision_number;
  revision.value.number = rev;

  svn_opt_revision_t prv_revision;
  prv_revision.kind = svn_opt_revision_number;
  prv_revision.value.number = rev-1;

  apr_file_t* outfile;
  apr_file_t* errfile;
  apr_status_t status;
  
  const char* tmp_dir;
  apr_temp_dir_get(&tmp_dir, m_pool);

  String tmp_fnm = String(tmp_dir) + "/svn_diff_tmp_out_XXXXXX";
  if((status = apr_file_mktemp(&outfile, (char*)tmp_fnm.chars(),
                               APR_CREATE | APR_WRITE,
                               m_pool))) {
    svn_pool_destroy(m_pool);
    taMisc::Error("cannot open diff tmp out at:", tmp_fnm);
    return;
  }

  if((status = apr_file_open_stderr(&errfile, m_pool))) {
    svn_pool_destroy(m_pool);
    taMisc::Error("cannot open stderr");
    return;
  }

  svn_depth_t depth = svn_depth_infinity;

  if(svn_error_t *error = svn_client_diff4
     (NULL, // const apr_array_header_t *diff_options,
      from_url,                 // const char *	path1,
      &prv_revision,
      from_url,                 // const char *	path2,
      &revision,
      NULL,
      depth,
      true, // svn_boolean_t 	ignore_ancestry,
      true, // svn_boolean_t 	no_diff_deleted,
      false, // svn_boolean_t 	ignore_content_type,
      APR_LOCALE_CHARSET, // const char * header_encoding,
      outfile,
      errfile,
      NULL, // const apr_array_header_t * changelists,
      m_ctx,
      m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion SaveFile diff4 fm prev error", error);
    }

  apr_file_close(outfile);
  to_str.LoadFromFile(tmp_fnm);
  taMisc::RemoveFile(tmp_fnm);
  svn_pool_destroy(m_pool);
}

void 
SubversionClient::GetDiffWc(const String& from_url, String& to_str) {
  apr_pool_t* m_pool = svn_pool_create(0);
  QMutexLocker qml(svn_operation);

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  from_url = svn_uri_canonicalize(from_url, m_pool);
#endif

  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_base;

  // Set the revision numbers to compare wc against base checkout version
  svn_opt_revision_t revision;
  revision.kind = svn_opt_revision_working;

  svn_opt_revision_t prv_revision;
  prv_revision.kind = svn_opt_revision_base;

  apr_file_t* outfile;
  apr_file_t* errfile;
  apr_status_t status;
  char tmp_fnm[] = "svn_diff_tmp_out_XXXXXX";
  if((status = apr_file_mktemp(&outfile, tmp_fnm,
                               APR_CREATE | APR_WRITE,
                               m_pool))) {
    svn_pool_destroy(m_pool);
    taMisc::Error("cannot open diff tmp out");
    return;
  }

  if((status = apr_file_open_stderr(&errfile, m_pool))) {
    svn_pool_destroy(m_pool);
    taMisc::Error("cannot open stderr");
    return;
  }

  svn_depth_t depth = svn_depth_infinity;

  if(svn_error_t *error = svn_client_diff4
     (NULL, // const apr_array_header_t *diff_options,
      from_url,                 // const char *	path1,
      &prv_revision,
      from_url,                 // const char *	path2,
      &revision,
      NULL,
      depth,
      true, // svn_boolean_t 	ignore_ancestry,
      true, // svn_boolean_t 	no_diff_deleted,
      false, // svn_boolean_t 	ignore_content_type,
      APR_LOCALE_CHARSET, // const char * header_encoding,
      outfile,
      errfile,
      NULL, // const apr_array_header_t * changelists,
      m_ctx,
      m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion SaveFile diff4 wc error", error);
    }

  apr_file_close(outfile);
  to_str.LoadFromFile(tmp_fnm);
  taMisc::RemoveFile(tmp_fnm);
  svn_pool_destroy(m_pool);

}

class SvnLogInfoPtrs {
public:
  int_PArray* revs;
  String_PArray* commit_msgs;
  String_PArray* authors;
  int_PArray* times;
  int_PArray* files_start_idx;
  int_PArray* files_n;
  String_PArray* files;
  String_PArray* actions;
};

static svn_error_t* mysvn_log_callback(void *baton, svn_log_entry_t* log_entry,
                                       apr_pool_t *pool) {
  if (log_entry == NULL)
    return NULL;
  SvnLogInfoPtrs* li = (SvnLogInfoPtrs*)baton;
  li->revs->Add(log_entry->revision);

  static const String svnLog(SVN_PROP_REVISION_LOG);
  static const String svnDate(SVN_PROP_REVISION_DATE);
  static const String svnAuthor(SVN_PROP_REVISION_AUTHOR);

  bool got_msg = false;
  bool got_auth = false;
  bool got_date = false;
  if(log_entry->revprops) {
    for(apr_hash_index_t *index = apr_hash_first (pool, log_entry->revprops);
        index != NULL;
        index = apr_hash_next(index))
      {
        // extract next entry from hash
        const char* key = NULL;
        ptrdiff_t keyLen;
        const char** val = NULL;

        apr_hash_this(index, reinterpret_cast<const void**>(&key),
                      &keyLen, reinterpret_cast<void**>(&val));

        String name = key;
        String value = *val;

        if(name == svnLog && !got_msg) {
          li->commit_msgs->Add(value);
          got_msg = true;
        }
        else if(name == svnAuthor && !got_auth) {
          li->authors->Add(value);
          got_auth = true;
        }
        else if(name == svnDate && !got_date) {
          apr_time_t ts;
          svn_time_from_cstring(&ts, *val, pool);
          li->times->Add(ts / 1000000); // microseconds -> seconds
          got_date = true;
        }
      }
  }
  if(!got_msg)
    li->commit_msgs->Add("");
  if(!got_auth)
    li->authors->Add("");
  if(!got_date)
    li->times->Add(0);

  int files_start = li->files->size;
  li->files_start_idx->Add(files_start); // starting idx

  // changed_paths2 is for 1.6 and above -- that is all we support
  if (log_entry->changed_paths2 != NULL) {
    char * path;
    apr_ssize_t klen;
    svn_log_changed_path2_t* log_item;
    for (apr_hash_index_t * itr = apr_hash_first(pool, log_entry->changed_paths2);
        itr; itr = apr_hash_next(itr)) {

      apr_hash_this(itr, (const void **) &path, &klen, (void **) &log_item);

      li->files->Add(path);
      String action = log_item->action; // char A, D, R, M
      li->actions->Add(action);
    }
  }

  li->files_n->Add(li->files->size - files_start); // how many we added
  return NULL;
}

void
SubversionClient::GetLogs(int_PArray& revs, String_PArray& commit_msgs,
                          String_PArray& authors,
                          int_PArray& times, int_PArray& files_start_idx,
                          int_PArray& files_n, String_PArray& files,
                          String_PArray& actions,
                          const String& url, int end_rev, int n_entries) {

  apr_pool_t* m_pool = svn_pool_create(0);
  QMutexLocker qml(svn_operation);

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 7)
  url = svn_uri_canonicalize(url, m_pool);
#endif

  n_entries = MAX(1, n_entries);

  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_unspecified;

  // create an array containing a single element which is the url target
  apr_array_header_t *targets = apr_array_make(m_pool, 1, sizeof(const char *));
  APR_ARRAY_PUSH(targets, const char *) = url;


  svn_opt_revision_range_t revision_range;
  if(end_rev < 0) {
    // we can't support head just yet unfortunately, and  this was deadly
    revision_range.end.kind = svn_opt_revision_number;
    revision_range.end.value.number = n_entries+1;

    revision_range.start.kind = svn_opt_revision_number;
    revision_range.start.value.number = 1; // just go all the way back, and let limit settle it..
  }
  else {
    revision_range.end.kind = svn_opt_revision_number;
    revision_range.end.value.number = end_rev;

    revision_range.start.kind = svn_opt_revision_number;
    revision_range.start.value.number = MAX(1, end_rev-n_entries+1);
  }

  apr_array_header_t* revision_ranges
    = apr_array_make (m_pool, 1, sizeof(svn_opt_revision_range_t*));
  APR_ARRAY_PUSH(revision_ranges, svn_opt_revision_range_t*) = &revision_range;


  SvnLogInfoPtrs svn_li_baton;
  svn_li_baton.revs = &revs;
  svn_li_baton.commit_msgs = &commit_msgs;
  svn_li_baton.authors = &authors;
  svn_li_baton.times = &times;
  svn_li_baton.files_start_idx = &files_start_idx;
  svn_li_baton.files_n = &files_n;
  svn_li_baton.files = &files;
  svn_li_baton.actions = &actions;

  if(svn_error_t *error = svn_client_log5
    (targets,
    &peg_revision,
    revision_ranges,
    n_entries,
    true, // discover_changed_paths
    false, // strict_node_history,
    false, // include_merged_revisions -- this causes a crash!!!
    NULL, // revprops -- get all
    mysvn_log_callback,
    (void*)&svn_li_baton,
    m_ctx,
    m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion log error", error);
    }

  svn_pool_destroy(m_pool);
}

int
SubversionClient::Update(int rev)
{
  QMutexLocker qml(svn_operation);
  m_cancelled = false;

  apr_pool_t* m_pool = svn_pool_create(0);

  // The value of the revision(s) checked out from the repository will be
  // populated into this array.
  apr_array_header_t *result_revs = 0;

  // create an array containing a single element which is the input path to be updated
  apr_array_header_t *paths = apr_array_make(m_pool, 1, sizeof(const char *));
  APR_ARRAY_PUSH(paths, const char *) = m_wc_path;

  // Set the revision number, if provided. Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  if (rev < 0) {
    revision.kind = svn_opt_revision_head;
  }
  else {
    revision.kind = svn_opt_revision_number;
    revision.value.number = rev;
  }

  // Get all files.
  svn_depth_t depth = svn_depth_infinity;

  // Set advanced options we don't care about.
  svn_boolean_t ignore_externals = false;
  svn_boolean_t allow_unver_obstructions = true;
  svn_boolean_t depth_is_sticky = true;

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  if (svn_error_t *error = svn_client_update3(
        &result_revs,
        paths,
        &revision,
        depth,
        depth_is_sticky,
        ignore_externals,
        allow_unver_obstructions,
        m_ctx,
        m_pool))
#else
  svn_boolean_t adds_as_modification = true;
  svn_boolean_t make_parents = false;
    
  if (svn_error_t *error = svn_client_update4(
        &result_revs,
        paths,
        &revision,
        depth,
        depth_is_sticky,
        ignore_externals,
        allow_unver_obstructions,
        adds_as_modification,
        make_parents,
        m_ctx,
        m_pool))
#endif
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion update error", error);
  }

  // Should only be one element updated since we just provided one path.
  // taMisc::Info("Number of elements updated:", String(result_revs->nelts));
  svn_revnum_t updateRev = APR_ARRAY_IDX(result_revs, 0, svn_revnum_t);
  svn_pool_destroy(m_pool);
  return updateRev;
}

int
SubversionClient::UpdateFiles(const String_PArray& files, int rev) {
  m_cancelled = false;

  if(files.size == 0) {
    return Update(rev);
  }
  
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  // The value of the revision(s) checked out from the repository will be
  // populated into this array.
  apr_array_header_t *result_revs = 0;

  // create an array containing a single element which is the input path to be updated
  apr_array_header_t *paths = apr_array_make(m_pool, files.size, sizeof(const char *));
  for(int i=0; i< files.size; i++) {
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
    APR_ARRAY_PUSH(paths, const char *) = svn_path_canonicalize(files[i], m_pool);
#else
    APR_ARRAY_PUSH(paths, const char *) = svn_dirent_canonicalize(files[i], m_pool);
#endif
  }

  // Set the revision number, if provided. Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  if (rev < 0) {
    revision.kind = svn_opt_revision_head;
  }
  else {
    revision.kind = svn_opt_revision_number;
    revision.value.number = rev;
  }

  // Get all files.
  svn_depth_t depth = svn_depth_infinity;

  // Set advanced options we don't care about.
  svn_boolean_t ignore_externals = false;
  svn_boolean_t allow_unver_obstructions = true;
  svn_boolean_t depth_is_sticky = true;

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  if (svn_error_t *error = svn_client_update3(
        &result_revs,
        paths,
        &revision,
        depth,
        depth_is_sticky,
        ignore_externals,
        allow_unver_obstructions,
        m_ctx,
        m_pool))
#else
  svn_boolean_t adds_as_modification = true;
  svn_boolean_t make_parents = false;

  if (svn_error_t *error = svn_client_update4(
        &result_revs,
        paths,
        &revision,
        depth,
        depth_is_sticky,
        ignore_externals,
        allow_unver_obstructions,
        adds_as_modification,
        make_parents,
        m_ctx,
        m_pool))
#endif
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion update error", error);
  }

  // Should only be one element updated since we just provided one path.
  // taMisc::Info("Number of elements updated:", String(result_revs->nelts));
  svn_revnum_t updateRev = APR_ARRAY_IDX(result_revs, 0, svn_revnum_t);
  svn_pool_destroy(m_pool);
  return updateRev;
}

void
SubversionClient::RevertFiles(const String_PArray& files) {
  m_cancelled = false;

  if(files.size == 0) {
    return;
  }
  QMutexLocker qml(svn_operation);
  apr_pool_t* m_pool = svn_pool_create(0);

  // create an array containing a single element which is the input path to be updated
  apr_array_header_t *paths = apr_array_make(m_pool, files.size, sizeof(const char *));
  for(int i=0; i< files.size; i++) {
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
    APR_ARRAY_PUSH(paths, const char *) = svn_path_canonicalize(files[i], m_pool);
#else
    APR_ARRAY_PUSH(paths, const char *) = svn_dirent_canonicalize(files[i], m_pool);
#endif
  }

  // Get all files.
  svn_depth_t depth = svn_depth_infinity;

  if (svn_error_t *error = svn_client_revert2(
        paths,
        depth,
        NULL,                   // changelists.. nope
        m_ctx,
        m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion revert files error", error);
  }

  svn_pool_destroy(m_pool);
}

void
SubversionClient::Add(const String& f_or_d, bool recurse, bool add_parents)
{
  m_cancelled = false;
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  // canonicalize the path
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  const char* file_or_dir = svn_path_canonicalize(f_or_d, m_pool);
#else
  const char* file_or_dir = svn_dirent_canonicalize(f_or_d, m_pool);
#endif

  // if adding dir, the depth of subdirectories to be added
  svn_depth_t depth = recurse ? svn_depth_infinity : svn_depth_empty;

  // do not error on already-versioned items
  svn_boolean_t force = true;

  // don not add files or dirs that match ignore patterns
  svn_boolean_t no_ignore = false;

  if (svn_error_t *error = svn_client_add4(
        file_or_dir,
        depth,
        force,
        no_ignore,
        add_parents,  // whether or not to create non-versioned parent directories
        m_ctx,
        m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error adding files", error);
  }
  svn_pool_destroy(m_pool);
}

void
SubversionClient::Delete(const String_PArray& files, bool force, bool keep_local)
{
  m_cancelled = false;
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  // create an array containing a single path to be created
  apr_array_header_t *paths = apr_array_make(m_pool, files.size, sizeof(const char *));
  for(int i=0; i< files.size; i++) {
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
    APR_ARRAY_PUSH(paths, const char *) = svn_path_canonicalize(files[i], m_pool);
#else
    APR_ARRAY_PUSH(paths, const char *) = svn_dirent_canonicalize(files[i], m_pool);
#endif
  }

  svn_commit_info_t *commit_info_p = svn_create_commit_info(m_pool);

  svn_boolean_t svn_force = force;
  svn_boolean_t svn_keep_local = keep_local;

  // don not add files or dirs that match ignore patterns
  svn_boolean_t no_ignore = false;

  // We don't need to set any custom revision properties, so null.
  const apr_hash_t *revprop_table = 0;

  // Not implementing the 1.7 API -- see rationale in Checkin().
  if (svn_error_t *error = svn_client_delete3
      (
       &commit_info_p,
       paths,
       svn_force,
       svn_keep_local,
       revprop_table,
       m_ctx,
       m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error removing files", error);
  }
  svn_pool_destroy(m_pool);
}


void
SubversionClient::MoveFile(const String_PArray& from_nms, String& to_nm, bool force) {
  m_cancelled = false;
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  // create an array containing a single path to be created
  apr_array_header_t *paths = apr_array_make(m_pool, from_nms.size, sizeof(const char *));
  for(int i=0; i< from_nms.size; i++) {
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
    APR_ARRAY_PUSH(paths, const char *) = svn_path_canonicalize(from_nms[i], m_pool);
#else
    APR_ARRAY_PUSH(paths, const char *) = svn_dirent_canonicalize(from_nms[i], m_pool);
#endif
  }

  // canonicalize the path
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  const char* to_can = svn_path_canonicalize(to_nm, m_pool);
#else
  const char* to_can = svn_dirent_canonicalize(to_nm, m_pool);
#endif

  svn_commit_info_t *commit_info_p = svn_create_commit_info(m_pool);


  svn_boolean_t svn_force = force;
  svn_boolean_t svn_move_as_child = (from_nms.size > 1);
  svn_boolean_t svn_make_parents = true;

  // We don't need to set any custom revision properties, so null.
  const apr_hash_t *revprop_table = 0;

  // Not implementing the 1.7 API -- see rationale in Checkin().
  if (svn_error_t *error = svn_client_move5
      (
       &commit_info_p,
       paths,
       to_can,
       svn_force,
       svn_move_as_child,
       svn_make_parents,
       revprop_table,
       m_ctx,
       m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error moving files", error);
  }
  svn_pool_destroy(m_pool);
}

void
SubversionClient::CopyFile(const String_PArray& from_nms, String& to_nm) {
  m_cancelled = false;

  taMisc::Confirm("Sorry, as of now there is an intractible bug in our use of svn_client_copy in subversion -- we're trying to fix it -- please use the command line in the meantime.");
  return;
  
  apr_pool_t* m_pool = svn_pool_create(0);

  // create an array containing a single path to be created
  apr_array_header_t *paths = apr_array_make(m_pool, from_nms.size, sizeof(const char *));
  for(int i=0; i< from_nms.size; i++) {
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
    APR_ARRAY_PUSH(paths, const char *) = svn_path_canonicalize(from_nms[i], m_pool);
#else
    APR_ARRAY_PUSH(paths, const char *) = svn_dirent_canonicalize(from_nms[i], m_pool);
#endif
  }

  // canonicalize the path
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  const char* to_can = svn_path_canonicalize(to_nm, m_pool);
#else
  const char* to_can = svn_dirent_canonicalize(to_nm, m_pool);
#endif

  svn_commit_info_t *commit_info_p = svn_create_commit_info(m_pool);

  svn_boolean_t svn_copy_as_child = (from_nms.size > 1);
  svn_boolean_t svn_make_parents = true;
  svn_boolean_t svn_ignore_externals = true;

  // We don't need to set any custom revision properties, so null.
  const apr_hash_t *revprop_table = 0;

  // Not implementing the 1.7 API -- see rationale in Checkin().
  if (svn_error_t *error = svn_client_copy5
      (
       &commit_info_p,
       paths,
       to_can,
       svn_copy_as_child,
       svn_make_parents,
       svn_ignore_externals,
       revprop_table,
       m_ctx,
       m_pool))
    {
      svn_pool_destroy(m_pool);
      throw Exception("Subversion error copying files", error);
    }

  // get the exact same crash here:
  // if (svn_error_t *error = svn_client_copy6
  //     (
  //      paths,
  //      to_can,
  //      svn_copy_as_child,
  //      svn_make_parents,
  //      svn_ignore_externals,
  //      revprop_table,
  //      NULL,                    // no commit callback
  //      NULL,                    // no commit baton
  //      m_ctx,
  //      m_pool))
  // {
  //   svn_pool_destroy(m_pool);
  //   throw Exception("Subversion error copying files", error);
  // }

  svn_pool_destroy(m_pool);
}

bool
SubversionClient::MakeDir(const String& new_dir, bool make_parents)
{
  m_cancelled = false;

  apr_pool_t* m_pool = svn_pool_create(0);
  QMutexLocker qml(svn_operation);

  // won't be used unless we make an immediate commit after adding files (by setting revprop_table)
  svn_commit_info_t *commit_info_p = svn_create_commit_info(m_pool);

  // create an array containing a single path to be created
  apr_array_header_t *paths = apr_array_make(m_pool, 1, sizeof(const char *));
  APR_ARRAY_PUSH(paths, const char *) = new_dir;

  // We don't need to set any custom revision properties, so null.
  const apr_hash_t *revprop_table = 0;

  // Not implementing the 1.7 API -- see rationale in Checkin().
  if (svn_error_t *error = svn_client_mkdir3(
        &commit_info_p,
        paths,
        make_parents, // whether or not to create non-versioned parent directories
        revprop_table,
        m_ctx,
        m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error making directory", error);
  }
  svn_pool_destroy(m_pool);
  return true;
}

bool
SubversionClient::TryMakeDir(const String& new_dir, bool make_parents)
{
  m_cancelled = false;

  try {
    return MakeDir(new_dir, make_parents);
  }
  catch (const Exception &ex) {
    if (ex.GetErrorCode() == EMER_ERR_ENTRY_EXISTS ||
        ex.GetErrorCode() == EMER_ERR_FS_ALREADY_EXISTS)
    {
      return false; // duplicate dir
    }

    // No need to log here; it will be re-caught and logged later.
    throw;
  }
}

bool
SubversionClient::MakeUrlDir(const String& url, const String& comment, bool make_parents)
{
  m_commit_message = comment;
  return MakeDir(url, make_parents);
}

bool
SubversionClient::TryMakeUrlDir(const String& url, const String& comment, bool make_parents)
{
  m_commit_message = comment;
  return TryMakeDir(url, make_parents);
}

// Commits files and returns the new revision number.
// Throws on error; returns -1 if there was nothing to commit.
int
SubversionClient::Checkin(const String& comment) {
  m_cancelled = false;
  m_commit_message = comment;
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  // 'files' is a comma or newline separated list of files and/or directories.
  // If empty, the whole working copy will be committed.
  apr_array_header_t *paths = apr_array_make(m_pool, 1, sizeof(const char *));
  // create an array containing a single element which is the path path to be committed
  APR_ARRAY_PUSH(paths, const char *) = m_wc_path;

  // commit changes to the children of the paths
  svn_depth_t depth = svn_depth_infinity;

  // unlock paths in the repository after commit
  svn_boolean_t keep_locks = false;

  // no need to changelist filtering
  svn_boolean_t keep_changelists = false;
  const apr_array_header_t *changelists =
    apr_array_make(m_pool, 0, sizeof(const char *));

  // We don't need to set any custom revision properties, so null.
  const apr_hash_t *revprop_table = 0;

  // Subversion 1.7 has a function svn_client_commit5(), but for now the
  // 1.6 API svn_client_commit4() works just fine for us.  Implementing
  // in terms of the 1.7 API would mean duplicating the logic in the 1.7
  // version of svn_client_commit4(), which is just a wrapper around the
  // new svn_client_commit5().
  svn_commit_info_t *commit_info_p = 0; // out param.
  if (svn_error_t *error = svn_client_commit4(
        &commit_info_p,
        paths,
        depth,
        keep_locks,
        keep_changelists,
        changelists,
        revprop_table,
        m_ctx,
        m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error committing files", error);
  }

  int rval = commit_info_p ? commit_info_p->revision : SVN_INVALID_REVNUM;
  svn_pool_destroy(m_pool);
  return rval;
}

int
SubversionClient::CheckinFiles(const String_PArray& files, const String& comment) {
  m_cancelled = false;
  m_commit_message = comment;

  if(files.size == 0) {
    return Checkin(comment);
  }
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  apr_array_header_t *paths = apr_array_make(m_pool, files.size, sizeof(const char *));
  for(int i=0; i< files.size; i++) {
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
    APR_ARRAY_PUSH(paths, const char *) = svn_path_canonicalize(files[i], m_pool);
#else
    APR_ARRAY_PUSH(paths, const char *) = svn_dirent_canonicalize(files[i], m_pool);
#endif
  }

  // commit changes to the children of the paths
  svn_depth_t depth = svn_depth_infinity;

  // unlock paths in the repository after commit
  svn_boolean_t keep_locks = false;

  // no need to changelist filtering
  svn_boolean_t keep_changelists = false;
  const apr_array_header_t *changelists =
    apr_array_make(m_pool, 0, sizeof(const char *));

  // We don't need to set any custom revision properties, so null.
  const apr_hash_t *revprop_table = 0;

  // Subversion 1.7 has a function svn_client_commit5(), but for now the
  // 1.6 API svn_client_commit4() works just fine for us.  Implementing
  // in terms of the 1.7 API would mean duplicating the logic in the 1.7
  // version of svn_client_commit4(), which is just a wrapper around the
  // new svn_client_commit5().
  svn_commit_info_t *commit_info_p = 0; // out param.
  if (svn_error_t *error = svn_client_commit4(
        &commit_info_p,
        paths,
        depth,
        keep_locks,
        keep_changelists,
        changelists,
        revprop_table,
        m_ctx,
        m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error committing files", error);
  }

  int rval = commit_info_p ? commit_info_p->revision : SVN_INVALID_REVNUM;
  svn_pool_destroy(m_pool);
  return rval;
}

int
SubversionClient::GetLastChangedRevision(const String& path)
{
  m_cancelled = false;
  m_last_changed_revision = -1;
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  // This call makes a callback to svn_info_receiver(), which
  // sets our m_last_changed_revision member variable.
  if (svn_error_t *error = svn_client_info2(
        path,
        0, // Set peg_revision and revision to
        0, //   null to pull from working copy.
        Glue::svn_info_receiver,
        this,
        svn_depth_empty, // Just want info on 'path'.
        0, // No changelists filtering.
        m_ctx,
        m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error getting revision info", error);
  }

  svn_pool_destroy(m_pool);
  return m_last_changed_revision;
}

void
SubversionClient::GetUrlFromPath(String& url, const String& path) {
  const char* url_str;

  apr_pool_t* m_pool = svn_pool_create(0);

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  if(svn_error_t *error = svn_client_url_from_path
     (&url_str,
      path,
      m_pool))
#else
  if(svn_error_t *error = svn_client_url_from_path2
    (&url_str,
     path,
     m_ctx,
     m_pool,
     m_pool))
#endif
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error getting path", error);
  }
  url = url_str;
  svn_pool_destroy(m_pool);
}

void
SubversionClient::GetRootUrlFromPath(String& url, const String& path) {
  const char* url_str;
  apr_pool_t* m_pool = svn_pool_create(0);

  if(svn_error_t *error = svn_client_root_url_from_path
    (&url_str,
     path,
     m_ctx,
     m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion error getting root path", error);
  }
  url = url_str;
  svn_pool_destroy(m_pool);
}


void
SubversionClient::Cancel()
{
  m_cancelled = true;
}

void
SubversionClient::Cleanup()
{
  m_cancelled = false;
  QMutexLocker qml(svn_operation);

  apr_pool_t* m_pool = svn_pool_create(0);

  if (svn_error_t *error = svn_client_cleanup(
        m_wc_path,
        m_ctx,
        m_pool))
  {
    svn_pool_destroy(m_pool);
    throw Exception("Subversion cleanup error", error);
  }

  svn_pool_destroy(m_pool);
}

// Note: Should never throw exceptions in callbacks, since the C code
// won't know what to do with them.

void
SubversionClient::notify(const svn_wc_notify_t *notify)
{
  // TODO.
}

bool
SubversionClient::isCancelled()
{
  return m_cancelled;
}

void
SubversionClient::notifyProgress(apr_off_t progress, apr_off_t total)
{
  // TODO.
}

String SubversionClient::getCommitMessage(const String& com_itm_str) {
  if(m_commit_message.nonempty())
    return m_commit_message;
  
  taGuiDialog dlg;
  taBase::Ref(dlg);   // no need to UnRef - will be deleted at end of method
  
  dlg.win_title = "Svn Commit Message";
  dlg.width = taiMisc::resizeByMainFont(400);
  dlg.height = taiMisc::resizeByMainFont(200);
  
  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);
  
  String row("");
  int space = 5;
  
  row = "prompt";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("prmpt_lbl", widget, row, "label=Provide Commit Message:;");
  
  row = "itms";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("itms_lbl", widget, row, "label=" + com_itm_str +";");
  
  row = "msg";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  // use a text field not a single line edit box
  iTextEdit* text_edit = new iTextEdit();
  // Get the hbox for this row so we can add our combobox to it.
  taGuiLayout *hboxEmer = dlg.FindLayout(row);
  if (!hboxEmer)
    return false;
  QBoxLayout *hbox = hboxEmer->layout;
  if (!hbox)
    return false;
  hbox->addWidget(text_edit);
  
  int drval = dlg.PostDialog(true);
  if(drval) {
    return String(text_edit->toPlainText());  // convert QString
  }
  return "";
}
