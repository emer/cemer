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

Subversion::Subversion(const char *working_copy_path)
  : m_wc_path(working_copy_path)
  , m_url()
{
}

Subversion::~Subversion()
{
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

  // Working copy doesn't exist yet, call some svn_wc_* function to create it.
  // TODO.
  return -1;
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
