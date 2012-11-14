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

#ifndef SUBVERSION_H_
#define SUBVERSION_H_

#include "ta_def.h"
#include <apr.h>  // apr_off_t
#include <string>

// TODO: pimpl this so all of emergent doesn't have to depend on APR/SVN?
//#include <apr_pools.h>
extern "C" {
  typedef struct apr_pool_t apr_pool_t;
  typedef struct svn_client_ctx_t svn_client_ctx_t;
  typedef struct svn_wc_notify_t svn_wc_notify_t;
}

// TODO: inherit from generic version control abstract base class.
class TA_API Subversion
{
public:
  // TODO: how to handle authentication?
  Subversion(const char *working_copy_path);
  virtual ~Subversion();

  // Check if the working copy has already been checked out.
  bool IsWorkingCopy();

  // Checkout returns the revision checked out, or -1 on error.
  int Checkout(const char *url, int rev = -1);

  // Update the working copy and return the revision checked out, or -1 on error.
  int Update(int rev = -1);

  // TODO: need to decide what return types make sense for each API.
  int Add(const char *file_or_dir, bool recurse = true, bool add_parents = true);
  bool MakeDir(const char *new_dir, bool create_parents = true);
  bool MakeUrlDir(const char *url, bool create_parents = true);

  // Checkin 'files': a comma or newline separated list of files/dirs.
  // If empty, the whole working copy will be committed.
  int Checkin(const char *comment, const char *files = "");
  int Status(const char *files = "");

  // Call to cancel current operation in progress.
  void Cancel();

  // TODO?
  // Log();

private:
  void createContext();

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
};

#endif // SUBVERSION_H_

