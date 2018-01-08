// Co2018ght 2012-2017, Regents of the University of Colorado,
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
#include <taString>
#include <apr.h>  // apr_off_t
#include <stdexcept>
#include <string>
#include <QMutex>

#ifdef CopyFile
#undef CopyFile
#endif

class String_PArray; //
class int_PArray; //

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

  void  SetWorkingCopyPath(const String& working_copy_path);
  std::string GetWorkingCopyPath() const;
  std::string GetUsername(const String& url, UsernameSource source) const;

  int   Checkout(const String& url, const String& to_wc_path, 
                 int rev = -1, bool recurse = true);
  // Checkout a working copy and return the revision checked out.

  void  List(String_PArray& file_names, String_PArray& file_paths, int_PArray& file_sizes,
            int_PArray& file_revs, int_PArray& file_times, int_PArray& file_kinds,
            String_PArray& file_authors,
            const String& url, int rev = -1, bool recurse = false);
  // list files in the repository url at given url -- fills in the arrays with coordinated values for each file -- times are seconds since 1970 standard time, kind = svn_node_kind_t -- svn_node_file or svn_node_dir, sizes are in bytes -- rev -1 = head (current), recurse = get all the subdirectories under url too (expensive)

  bool  GetInfo(const String& file_or_dir_or_url, int& rev, int& kind, String& root_url,
                int& last_changed_rev, int& last_changed_date,
                String& last_changed_author, int64_t& size);
  // Get info about a file / dir / url -- times are seconds since 1970 standard time, kind = svn_node_kind_t -- svn_node_file or svn_node_dir, sizes are in bytes -- for local working copy file / dir, it does NOT access the network and is fast.. returns false for any kind of error, typically a file or path that does not exist (does not throw exceptions) -- thus can be used as a quick and easy check for url path existence

  bool  UrlExists(const String& url, int& rev);
  // does the given url exist??
  
  bool  IsWCRevSameAsHead(const String& file_or_dir, int& wc_rev, int& head_rev);
  // Checks if the working copy revision of given file or directory is the same as the head revision on the repository version of that file/dir -- returns true if so, and fills in revisions in either case - this is a very fast way to determine whether to do a full update, for pull-only repositories -- if file_or_dir does not exist locally, then returns false, and both rev's = 0
  
  void  SaveFile(const String& from_url, const String& to_path, int rev = -1);
  // copy a file from given fully-specified url to a file at given to_path, using given revision (-1 = head)

  void  GetFile(const String& from_url, String& to_str, int rev = -1);
  // get file from given fully-specified url to a string at given to_path, using given revision (-1 = head)

  void  GetDiffToPrev(const String& from_url, String& to_str, int rev);
  // get diff to previous revision for given url at given rev -- must specify the revision explicitly (don't use -1), formatted diff output goes into given string
  void  GetDiffWc(const String& from_url, String& to_str);
  // get diff of working copy file relative to base in repository

  void  GetLogs(int_PArray& revs, String_PArray& commit_msgs, String_PArray& authors,
               int_PArray& times, int_PArray& files_start_idx,
               int_PArray& files_n, String_PArray& files, String_PArray& actions,
               const String& url, int end_rev = -1, int n_entries = 50);
  // get logs of commits made to repository, with coordinated info in each array, and files listed (full paths from url) at files_start_idx into files array, with files_n per each one -- end at given revision (-1 = head or current) and go back given number of log entries

  int   Update(int rev = -1);
  // Update the working copy and return the revision.
  int   UpdateFiles(const String_PArray& files, int rev = -1);
  // Update specific list of files and return the revision -- if files empty updates the entire working copy
  void  RevertFiles(const String_PArray& files);
  // revert specific list of files -- remove any local modifications, restore to prior updated state -- good to do if you've made non-needed local mods, prior to updating to a new version, to prevent conflicts

  void  Add(const String& file_or_dir, bool recurse = true, bool add_parents = true);
  // Add files to the working copy and schedule for future commit -- Does not throw if file is already versioned.

  void  Delete(const String_PArray& files, bool force, bool keep_local);
  // Delete listed files (or directories) from the repository, with force and keep_local options

  void  CopyFile(const String_PArray& from_nms, String& to_nm);
  // copy file from given name(s) to given name (if multiple from names, to must be a dir) -- force = overwrite existing etc

  void  MoveFile(const String_PArray& from_nms, String& to_nm, bool force = false);
  // move files from given name(s) to given name  (if multiple from names, to must be a dir) -- force = overwrite existing etc
  
  // All functions return true if a new directory was created.
  bool  MakeDir(const String& new_dir, bool make_parents = true);
  // Create a directory in the working copy -- returns true if directory created
  bool  TryMakeDir(const String& new_dir, bool make_parents = true);
  // Create a directory in the working copy -- returns true if directory created -- no error if the dir already exists
  bool  MakeUrlDir(const String& url, const String& comment=NULLStr, bool make_parents = true);
  // Create a directory in the repository -- returns true if directory created
  bool  TryMakeUrlDir(const String& url, const String& comment=NULLStr, bool make_parents = true);
  // Create a directory in the repository -- returns true if directory created -- no error if the dir already exists.

  int   Checkin(const String& comment=NULLStr);
  // Checkin (commit) working copy -- returns the new revision number or -1 if nothing to commit.
  int   CheckinFiles(const String_PArray& files, const String& comment=NULLStr);
  // Checkin specific list of files in given array -- if empty, the whole working copy will be committed -- returns the new revision number or -1 if nothing to commit.
  int   GetLastChangedRevision(const String& path);

  void  GetUrlFromPath(String& url, const String& path);
  // get the server url from path to local file
  void  GetRootUrlFromPath(String& url, const String& path);
  // get the server root url from path to local file

  void Cancel();
  // Call to cancel current operation in progress.

  void Cleanup();
  // cleanup working copy directory, cleaning locks and generally allowing an interrupted or broken action to be undone -- call this whenever stuff isn't working

  
private:
  svn_client_ctx_t * createContext();
  apr_array_header_t * createAuthProviders(apr_hash_t *config);

  // Callbacks.
  struct Glue; // connects C-style callbacks with these methods.
  bool isCancelled();
  void notify(const svn_wc_notify_t *notify);
  void notifyProgress(apr_off_t progress, apr_off_t total);
  String getCommitMessage(const String& com_itm_str);

  String  m_wc_path;
  String  m_url;
  apr_pool_t *m_pool_perm;      // permanent pool for persistent things
  svn_client_ctx_t *m_ctx;
  bool m_cancelled;
  String      m_commit_message;
  int m_last_changed_revision;
  QMutex *svn_operation;
};

#endif // SUBVERSION_CLIENT_H_

