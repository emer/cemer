// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include <apr.h>
#include <apr_pools.h>
#include <svn_client.h>
#include <svn_path.h>
#include <svn_pools.h>

Subversion::Subversion(const char *working_copy_path)
  : m_wc_path(0)
  , m_url(0)
  , m_pool(0) // initialized below
{
  // Set up APR and a pool -- eventually we'll move this somewhere
  // more global.
  apr_initialize();
  m_pool = svn_pool_create(0);

  // Canonicalize the path.
  m_wc_path = svn_path_canonicalize(working_copy_path, m_pool);
}

Subversion::~Subversion()
{
  // Clean up the pool and APR context.
  // TODO: this should be more global.
  svn_pool_destroy(m_pool);
  apr_terminate();
}

// Check if the working copy has already been checked out.
bool Subversion::IsWorkingCopy()
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

int Subversion::Checkout(const char *url, int rev)
{
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

  // TODO: Not using kdesvncpp stuff so gotta roll our own context.
  svn_client_ctx_t *ctx = 0;

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
      ctx,
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

int Subversion::Update(int rev)
{
  // TODO.
  return -1;
}

int Subversion::Add(const char *file_or_dir, bool recurse, bool add_parents)
{
  // TODO.
  return -1;
}

int Subversion::MakeDir(const char *new_dir, bool create_parents)
{
  // TODO.
  return -1;
}

int Subversion::MakeUrlDir(const char *url, bool create_parents)
{
  // TODO.
  return -1;
}

int Subversion::Checkin(const char *comment, const char *files)
{
  // 'files' is a comma or newline separated list of files and/or directories.
  // If empty, the whole working copy will be committed.
  // TODO.
  return -1;
}

int Subversion::Status(const char *files)
{
  // See comment in checkin() re: files param.
  // TODO.
  return -1;
}
