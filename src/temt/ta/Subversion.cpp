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

#include "Subversion.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <QUrl>

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <apr_pools.h>
#include <svn_client.h>
#include <svn_path.h>
#include <svn_pools.h>
#include <svn_wc.h>
#include "ta_type.h" // taMisc::Error, etc.
#include "inetworkaccessmanager.h" // getUsernamePassword()

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
  taMisc::Info("Terminated APR");
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
  std::string
  replaceLineEndingsWithLF(const std::string &msg)
  {
    std::string lfMsg;
    lfMsg.reserve(msg.size());
    for (unsigned i = 0; i < msg.size(); ++i) {
      if (msg[i] == '\r') { // CR
        if (i + 1 < msg.size() && msg[i] == '\n') { // LF
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
      std::string msg = sub->getCommitMessage();
      if (!msg.empty()) {
        // TODO: Remove following line once testing is complete.
        // It's only here so we don't spam the dev email list.
        msg += " (quiet)";

        // Subversion requires that only line feeds be used, no CRLF or CR.
        msg = replaceLineEndingsWithLF(msg);
        *log_msg = apr_pstrdup(pool, msg.c_str());
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
  , m_pool(0) // initialized below
  , m_ctx(0)
  , m_cancelled(false)
  , m_commit_message(0)
{
  taMisc::Info("Creating SubversionClient object");

  // Set up APR and a pool.
  Apr::initializeOnce(); // must be done prior to any other APR calls.
  m_pool = svn_pool_create(0);

  // Set the error handler to one that doesn't abort!
  // We'll check for these errors and convert to exceptions.
  svn_error_set_malfunction_handler(svn_error_raise_on_malfunction);

  try {
    // Create a context object.  Throws on failure.
    m_ctx = createContext();
  }
  catch (const Exception &ex) {
    taMisc::Error("Could not construct SubversionClient.", ex.what());
    throw;
  }
}

SubversionClient::~SubversionClient()
{
  taMisc::Info("Destroying SubversionClient object");

  // No need to destroy the context since it was allocated from
  // the pool, which is about to be destroyed.
  m_ctx = 0;

  // Clean up the pool and APR context.
  svn_pool_destroy(m_pool);
}

void
SubversionClient::SetWorkingCopyPath(const char *working_copy_path)
{
  // Canonicalize the working copy path.
  m_wc_path = svn_path_canonicalize(working_copy_path, m_pool);
}

std::string
SubversionClient::GetWorkingCopyPath() const
{
  return m_wc_path;
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
SubversionClient::GetUsername(const char *url, UsernameSource source) const
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
          m_pool))
    {
      svn_error_clear(error);
      return "";
    }

    // There's gotta be an easier way to get the username for a given URL!!
    // Can't just lookup the file based on the realmString, since its format
    // may change based on the client and/or server in use!  This is pretty
    // brute force.
    QFileInfoList authFiles = getAllFilesUnder(userAuthDirPath);
    QString username =
      findUsernameInAuthFileMatchingRealm(authFiles, realmString);
    if (!username.isEmpty()) {
      return username.toStdString();
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
  if (svn_error_t *error = svn_client_create_context(&ctx, m_pool)) {
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
        m_pool))
  {
    throw Exception("Subversion error getting configuration directory", error);
  }

  // Initialize the context's configuration hash.
  ctx->config = 0;
  if (svn_error_t *error = svn_config_get_config(
        &ctx->config,
        userConfigDirPath,
        m_pool))
  {
    throw Exception("Subversion error getting configuration info", error);
  }

  if (!ctx->config) {
    throw Exception("Subversion did not populate ctx->config.");
  }

  // Initialize the authentication baton.
  apr_array_header_t *providers = createAuthProviders(ctx->config);
  ctx->auth_baton = 0;
  svn_auth_open(&ctx->auth_baton, providers, m_pool);

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
        m_pool))
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
    &provider, Glue::svn_auth_plaintext_prompt_func, 0, m_pool);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  svn_auth_get_username_provider(&provider, m_pool);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

    /* The server-cert, client-cert, and client-cert-password providers. */
  if (svn_error_t *error = svn_auth_get_platform_specific_provider(
        &provider,
        "windows",
        "ssl_server_trust",
        m_pool))
  {
    // Not fatal, just clear the error.
    svn_error_clear(error);
  }
  else if (provider) {
    // If the cal succeeded, then put the provider on the list.
    APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  }

  svn_auth_get_ssl_server_trust_file_provider(&provider, m_pool);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  svn_auth_get_ssl_client_cert_file_provider(&provider, m_pool);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;
  svn_auth_get_ssl_client_cert_pw_file_provider2(
    &provider, Glue::svn_auth_plaintext_prompt_func, 0, m_pool);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

  // Add our own simple username/password auth provider.
  svn_auth_get_simple_prompt_provider(
    &provider,
    Glue::svn_auth_simple_prompt_func,
    0, // baton
    1, // retries
    m_pool);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

  svn_auth_get_username_prompt_provider(
    &provider,
    Glue::svn_auth_username_prompt_func,
    0, // baton
    1, // retries
    m_pool);
  APR_ARRAY_PUSH(providers, svn_auth_provider_object_t *) = provider;

  return providers;
}

// Check if the working copy has already been checked out.
bool
SubversionClient::IsWorkingCopy()
{
  if (!m_url) return false;

#if 0
  // TODO: call some svn_wc_* function to determine if this is
  // a valid working copy.
  if (!fileExists(m_wc_path)) return false;
  return svn_wc_is_valid(m_wc_path); // ??
#else
  return false;
#endif
}

int
SubversionClient::Checkout(const char *url, bool recursive, int rev)
{
  m_cancelled = false;
  /* TODO probably no need to the below code
  if (IsWorkingCopy()) {
    if (0 == std::strcmp(m_url, url)) {
      // If user requested checkout of the same URL, just do an update.
      return Update(rev);
    }
    else {
      throw Exception(
        std::string("Working copy already exists with URL: ") + m_url
          + "; will not checkout new URL: " + url);
    }
  }
  */

  // Working copy doesn't exist yet, so create it by checking out the URL.

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

  svn_depth_t depth = svn_depth_infinity;
  if (!recursive) {
    svn_depth_t depth = svn_depth_empty;
  }

  // Set advanced options we don't care about.
  svn_boolean_t ignore_externals = false;
  svn_boolean_t allow_unver_obstructions = true;

  if (svn_error_t *error = svn_client_checkout3(
        &result_rev, // out param
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
    throw Exception(
      std::string("Subversion checkout error for URL ") + url, error);
  }

  return result_rev;
}

int
SubversionClient::Update(int rev)
{
  m_cancelled = false;

// TODO: rest of function copied from example dir, needs cleanup.

  // Out parameter -- the value of the revision checked out from the repository.
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
  {
    throw Exception("Subversion update error", error);
  }

  // Should only be one element updated since we just provided one path.
  taMisc::Info("Number of elements updated:",
    toString(result_revs->nelts).c_str());
  svn_revnum_t updateRev = APR_ARRAY_IDX(
    result_revs, result_revs->nelts, svn_revnum_t);
  return updateRev;
}

int
SubversionClient::Add(const char *file_or_dir, bool recurse, bool add_parents)
{
  m_cancelled = false;
  // TODO.
  return -1;
}

bool
SubversionClient::MakeDir(const char *new_dir, bool make_parents)
{
  m_cancelled = false;

  // won't be used unless we make an immediate commit after adding files (by setting revprop_table)
  svn_commit_info_t *commit_info_p = svn_create_commit_info(m_pool);

  // create an array containing a single path to be created
  apr_array_header_t *paths = apr_array_make(m_pool, 1, sizeof(const char *));
  APR_ARRAY_PUSH(paths, const char *) = new_dir;

  // TODO: we need to set revprop_table to a non-null if we wanna make an immediate commit after adding files
  // svn_client_propget3 can be used to create an apr_hash_t
  const apr_hash_t *revprop_table = NULL;

  // TODO: #ifdef this to use mkdir4 for svn >=1.7.
  if (svn_error_t *error = svn_client_mkdir3(
        &commit_info_p,
        paths,
        make_parents, // whether to create non-existent parent directories
        revprop_table,
        m_ctx,
        m_pool))
  {
    throw Exception("Subversion error", error);
  }

  return true;
}

bool
SubversionClient::MakeUrlDir(const char *url, const char *comment, bool make_parents)
{
  m_commit_message = comment;
  return MakeDir(url, make_parents);
}

int
SubversionClient::Checkin(const char *comment, const char *files)
{
  m_cancelled = false;
  m_commit_message = comment;

  // 'files' is a comma or newline separated list of files and/or directories.
  // If empty, the whole working copy will be committed.

// TODO: rest of function copied from example dir, needs cleanup.

  //svn_commit_info_t *commit_info_p = svn_create_commit_info(m_pool);
  svn_commit_info_t *commit_info_p = NULL;

  // create an array containing a single element which is the path path to be committed
  apr_array_header_t *paths = apr_array_make(m_pool, 1, sizeof(const char *));
  APR_ARRAY_PUSH(paths, const char *) = m_wc_path;

  // commit changes to the children of the paths
  svn_depth_t depth = svn_depth_infinity;

  // unlock paths in the repository after commit
  svn_boolean_t keep_locks = false;

  // no need to changelist filtering
  svn_boolean_t keep_changelists = false;
  const apr_array_header_t *changelists = apr_array_make(m_pool, 0, sizeof(const char *));

  // TODO: null?
  const apr_hash_t *revprop_table = NULL;

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
    throw Exception("Subversion error", error);
  }

  return 1; // TODO: return the new revision
}

int
SubversionClient::Status(const char *files)
{
  m_cancelled = false;
  // See comment in checkin() re: files param.
  // TODO.
  return -1;
}

void
SubversionClient::Cancel()
{
  m_cancelled = true;
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

std::string
SubversionClient::getCommitMessage()
{
  // TODO: if m_commit_message is null, prompt user using dialog.
  if (!m_commit_message) return "Automated checkin from emergent";

  return m_commit_message;
}
