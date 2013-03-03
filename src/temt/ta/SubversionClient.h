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

#ifndef SUBVERSION_CLIENT_H_
#define SUBVERSION_CLIENT_H_

#include "ta_def.h"
#include <apr.h>  // apr_off_t
#include <stdexcept>
#include <string>

class String_PArray; //

// TODO: pimpl this so all of emergent doesn't have to depend on APR/SVN?
//#include <apr_pools.h>
extern "C" {
  typedef struct apr_array_header_t apr_array_header_t;
  typedef struct apr_hash_t apr_hash_t;
  typedef struct apr_pool_t apr_pool_t;
  typedef struct svn_client_ctx_t svn_client_ctx_t;
  typedef struct svn_error_t svn_error_t;
  typedef struct svn_wc_notify_t svn_wc_notify_t;
}

// TODO: inherit from generic version control abstract base class.
class TA_API SubversionClient
{
public:
  // Some error codes translated from the <svn_error_codes.h> values.
  enum ErrorCode
  {
    EMER_GENERAL_SVN_ERROR, // Check GetSvnErrorCode() for specific SVN error.
    EMER_OPERATION_CANCELLED,
    EMER_ERR_RA_DAV_REQUEST_FAILED,
    EMER_FORBIDDEN,
    // adding a dir when add_parents is true and none of the dir's parents are versioned
    EMER_ERR_CLIENT_NO_VERSIONED_PARENT,
    // adding a duplicate dir to a working copy
    EMER_ERR_ENTRY_EXISTS,
    // adding a duplicate dir to a repository
    EMER_ERR_FS_ALREADY_EXISTS,
    // TBD.
  };

  // Code using SubversionClient should catch exceptions of this type.
  class Exception : public std::runtime_error
  {
  public:
    // Exception based on svn_error_t object.  An additional message may
    // be provided; it will be prepended to the svn_error_t's message,
    // which may be null.  In no case may svn_error itself be null!
    explicit Exception(svn_error_t *svn_error);
    Exception(const std::string &additional_msg, svn_error_t *svn_error);

    // Exception with specified message and error code; svn error code is
    // optional and defaults to "OK".  Intended use is for exceptions thrown
    // that are not the result of an svn_error_t.
    explicit Exception(
      const std::string &msg,
      ErrorCode error_code = EMER_GENERAL_SVN_ERROR,
      int svn_error_code = 0);

    ErrorCode GetErrorCode() const;
    int GetSvnErrorCode() const;

  private:
    static ErrorCode toEmerErrorCode(svn_error_t *svn_error);

    ErrorCode m_error_code;
    int m_svn_error_code;
  };

  // There are three ways to get the username.
  enum UsernameSource
  {
    CHECK_CACHE_THEN_PROMPT_USER,
    CHECK_CACHE_ONLY,
    PROMPT_USER_ONLY,
  };

  SubversionClient();
  virtual ~SubversionClient();

  void SetWorkingCopyPath(const char *working_copy_path);
  std::string GetWorkingCopyPath() const;
  std::string GetUsername(const char *url, UsernameSource source) const;

  // Checkout a working copy and return the revision checked out.
  int Checkout(const char *url, int rev = -1, bool recurse = true);

  // Update the working copy and return the revision.
  int Update(int rev = -1);

  // Add files to the working copy and schedule for future commit.
  // Does not throw if file is already versioned.
  void Add(const char *file_or_dir, bool recurse = true, bool add_parents = true);
  // Delete listed files (or directories) from the repository, with force and keep_local options
  void Delete(const String_PArray& files, bool force, bool keep_local);

  // Create a directory in the working copy or in the repository.
  // The "Try" versions politely ignore cases where the directory
  // already exists.  Other errors will throw exceptions.
  // All functions return true if a new directory was created.
  bool MakeDir(const char *new_dir, bool make_parents = true);
  bool TryMakeDir(const char *new_dir, bool make_parents = true); // returns no error if the dir already exists.
  bool MakeUrlDir(const char *url, const char *comment = 0, bool make_parents = true);
  bool TryMakeUrlDir(const char *url, const char *comment = 0, bool make_parents = true); // returns no error if the dir already exists.

  // Checkin 'files': a comma or newline separated list of files/dirs.
  // If empty or null, the whole working copy will be committed.
  // Returns the new revision number or -1 if nothing to commit.
  int Checkin(const char *comment = 0, const char *files = 0);
  int GetLastChangedRevision(const char *path);

  // Call to cancel current operation in progress.
  void Cancel();

private:
  svn_client_ctx_t * createContext();
  apr_array_header_t * createAuthProviders(apr_hash_t *config);

  // Callbacks.
  struct Glue; // connects C-style callbacks with these methods.
  bool isCancelled();
  void notify(const svn_wc_notify_t *notify);
  void notifyProgress(apr_off_t progress, apr_off_t total);
  std::string getCommitMessage();

  const char *m_wc_path;
  const char *m_url;
  apr_pool_t *m_pool;
  svn_client_ctx_t *m_ctx;
  bool m_cancelled;
  const char *m_commit_message;
  int m_last_changed_revision;
};

#endif // SUBVERSION_CLIENT_H_

