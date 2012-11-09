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

#ifndef SUBVERSION_H_
#define SUBVERSION_H_

#include "ta_def.h"

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
  int MakeDir(const char *new_dir, bool create_parents = true);
  int MakeUrlDir(const char *url, bool create_parents = true);

  // Checkin 'files': a comma or newline separated list of files/dirs.
  // If empty, the whole working copy will be committed.
  int Checkin(const char *comment, const char *files = "");
  int Status(const char *files = "");

private:
  const char *m_wc_path;
  const char *m_url;
};

#endif // SUBVERSION_H_

