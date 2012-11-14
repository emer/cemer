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

#include <cstring>
#include <iostream>

#include <apr_pools.h>
#include <svn_client.h>
#include <svn_path.h>
#include <svn_pools.h>
#include <svn_wc.h>

///////////////////////////////////////////////////////////////////////////////
// Subversion::Glue
///////////////////////////////////////////////////////////////////////////////

struct Subversion::Glue
{
  // Glue code to connect C-style callbacks with C++ object methods.
  static
  svn_error_t *
  svn_cancel_func(void *cancel_baton)
  {
    Subversion *sub = reinterpret_cast<Subversion *>(cancel_baton);
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
    if (Subversion *sub = reinterpret_cast<Subversion *>(baton)) {
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
    if (Subversion *sub = reinterpret_cast<Subversion *>(baton)) {
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
    tmp_file = 0;
    log_msg = 0; // Should cause the operation to be cancelled if not set.
    if (Subversion *sub = reinterpret_cast<Subversion *>(baton)) {
      std::string msg = sub->getCommitMessage();
      if (!msg.empty()) {
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
};

///////////////////////////////////////////////////////////////////////////////
// Subversion
///////////////////////////////////////////////////////////////////////////////

Subversion::Subversion(const char *working_copy_path)
  : m_wc_path(0)
  , m_url(0)
  , m_pool(0) // initialized below
  , m_ctx(0)
  , m_cancelled(false)
{
  // Set up APR and a pool.
  // TODO: move this somewhere more global so we don't setup and teardown
  // the APR with every Subversion object that gets created.  Also, this
  // probably won't work as-is if there are more than two objects created
  // at the same time, since APR probably expects to be initialized only
  // once.
  apr_initialize();
  m_pool = svn_pool_create(0);

  // Canonicalize the path.
  m_wc_path = svn_path_canonicalize(working_copy_path, m_pool);

  // Create a context object.  May throw on failure.
  createContext();
}

Subversion::~Subversion()
{
  // Shouldn't need to destroy the context since it was allocated from
  // the pool, which is about to be destroyed.
  m_ctx = 0;

  // Clean up the pool and APR context.
  // TODO: this should be more global.
  svn_pool_destroy(m_pool);
  apr_terminate();
}

void
Subversion::createContext()
{
  // Allocate a new context object from the pool.
  svn_error_t *error = svn_client_create_context(&m_ctx, m_pool);
  if (error) {
    // TODO: Hook this into taMisc::Warning()
    std::cout << "Subversion error creating context: "
      << error->message << std::endl;
    svn_error_clear(error);
    m_ctx = 0;
    // TODO: throw?  docs say tihs call won't error in current implementation,
    // but in case a future implementation does error, we should handle it.
    // Don't want calling code to have an uninitialized Subversion object.
    return;
  }

  // Initialize the context with batons and callbacks and such.
  m_ctx->client_name = "emergent";

  // Get the user configuration directory (e.g., ~/.subversion/).
  const char *userConfigDirPath = 0;

  // Actually, there's no need to get the config dir -- the library will find
  // it for us if we pass null to svn_config_get_config().
  #if 0
    error = svn_config_get_user_config_path(
      &userConfigDirPath,
      0, // config_dir
      0, // set to SVN_CONFIG_CATEGORY_CONFIG to get the config file.
      m_pool);
  #endif

  // Initialize the context's configuration hash.
  m_ctx->config = 0;
  error = svn_config_get_config(&m_ctx->config, userConfigDirPath, m_pool);
  if (error) {
    std::cout << "Subversion error getting configuration info: "
      << error->message << std::endl;
    svn_error_clear(error);
    m_ctx = 0;
    return;
  }
  if (!m_ctx->config) {
    std::cout << "Subversion did not populate m_ctx->config" << std::endl;
    m_ctx = 0;
    return;
  }

  // Get the svn_config_t configuration data object from the hash.
  svn_config_t *configData = reinterpret_cast<svn_config_t *>(apr_hash_get(
    m_ctx->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING));

  // Get the list of authentication providers.
  apr_array_header_t *providers = 0;
  error = svn_auth_get_platform_specific_client_providers(
    &providers,
    configData,
    m_pool);
  if (error) {
    std::cout << "Subversion error getting authentication providers: "
      << error->message << std::endl;
    svn_error_clear(error);
    m_ctx = 0;
    return;
  }
  if (!providers) {
    std::cout << "Subversion did not provide authentication providers."
      << std::endl;
    m_ctx = 0;
    return;
  }

  // Initialize the authentication baton.
  // TODO: is this sufficient?
  m_ctx->auth_baton = 0;
  svn_auth_open(&m_ctx->auth_baton, providers, m_pool);

  m_ctx->mimetypes_map = 0; // TODO: do I need to implement this somehow?

  m_ctx->cancel_func = Subversion::Glue::svn_cancel_func;
  m_ctx->cancel_baton = this;

  m_ctx->notify_func = 0; // Deprecated in 1.2.
  m_ctx->notify_baton = 0;
  m_ctx->notify_func2 = Subversion::Glue::svn_wc_notify_func2;
  m_ctx->notify_baton2 = this;

  m_ctx->progress_func = Subversion::Glue::svn_ra_progress_notify_func;
  m_ctx->progress_baton = this;

  m_ctx->log_msg_func = 0; // Deprecated in 1.3.
  m_ctx->log_msg_baton = 0;
  m_ctx->log_msg_func2 = 0; // Deprecated in 1.5.
  m_ctx->log_msg_baton2 = 0;
  m_ctx->log_msg_func3 = Subversion::Glue::svn_client_get_commit_log3;
  m_ctx->log_msg_baton3 = this;

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR < 7)
  m_ctx->conflict_func = Subversion::Glue::svn_wc_conflict_resolver_func;
  m_ctx->conflict_baton = this;
#else
  m_ctx->conflict_func = 0; // Deprecated in 1.7.
  m_ctx->conflict_baton = 0;
  m_ctx->conflict_func2 = Subversion::Glue::svn_wc_conflict_resolver_func2;
  m_ctx->conflict_baton2 = this;
#endif
}

// Check if the working copy has already been checked out.
bool
Subversion::IsWorkingCopy()
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
Subversion::Checkout(const char *url, int rev)
{
  m_cancelled = false;
  if (IsWorkingCopy()) {
    if (0 == std::strcmp(m_url, url)) {
      // If user requested checkout of the same URL, just do an update.
      return Update(rev);
    }
    else {
      // Otherwise error.
      std::cout << "Error: Working copy already exists with URL: " << m_url
        << "\nWill not checkout new URL: " << url << std::endl;
      return -1;
    }
  }

  // Working copy doesn't exist yet, so create it by checking out the URL.

  // Out parameter -- the value of the revision checked out from the
  // repository.
  svn_revnum_t result_rev;

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

  // Set depth to get all files.
  svn_depth_t depth = svn_depth_infinity;

  // Set advanced options we don't care about.
  svn_boolean_t ignore_externals = false;
  svn_boolean_t allow_unver_obstructions = true;

  svn_error_t *err =
    svn_client_checkout3(
      &result_rev, // out param
      url,
      m_wc_path,
      &peg_revision,
      &revision,
      depth,
      ignore_externals,
      allow_unver_obstructions,
      m_ctx,
      m_pool);

  // Check for error.
  if (err) {
    // TODO: Hook this into taMisc::Warning()
    std::cout << "Subversion error: " << err->message << std::endl;
    svn_error_clear(err);
    return -1;
  }

  return result_rev;
}

int
Subversion::Update(int rev)
{
  m_cancelled = false;
  // TODO.
  return -1;
}

int
Subversion::Add(const char *file_or_dir, bool recurse, bool add_parents)
{
  m_cancelled = false;
  // TODO.
  return -1;
}

bool
Subversion::MakeDir(const char *new_dir, bool create_parents)
{
  m_cancelled = false;
  // TODO.
  return false;
}

bool
Subversion::MakeUrlDir(const char *url, bool create_parents)
{
  m_cancelled = false;
  // TODO.
  return false;
}

int
Subversion::Checkin(const char *comment, const char *files)
{
  m_cancelled = false;
  // 'files' is a comma or newline separated list of files and/or directories.
  // If empty, the whole working copy will be committed.
  // TODO.
  return -1;
}

int
Subversion::Status(const char *files)
{
  m_cancelled = false;
  // See comment in checkin() re: files param.
  // TODO.
  return -1;
}

void
Subversion::Cancel()
{
  m_cancelled = true;
}

void
Subversion::notify(const svn_wc_notify_t *notify)
{
  // TODO.
}

bool
Subversion::isCancelled()
{
  return m_cancelled;
}

void
Subversion::notifyProgress(apr_off_t progress, apr_off_t total)
{
  // TODO.
}

std::string
Subversion::getCommitMessage()
{
  // TODO: prompt user using dialog or command line.
  return "because";
}
