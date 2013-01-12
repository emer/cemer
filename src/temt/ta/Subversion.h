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

#include <taNBase>


// This class provides emergent programs with static functions that wrap
// the SubversionClient API.
class TA_API Subversion : public taNBase
{
  // ##INLINE ##NO_TOKENS Subversion interface -- static functions for working with Subversion repositories.
  INHERITED(taNBase)

public:
  static String GetUsername(const String &url, bool prompt = true);
  // #CAT_Subversion Get the cached username for the given URL.  If no username cached and prompt=true, prompt the user with a dialog.  Otherwise returns empty string.

  static int Checkout(const String &working_copy_path, const String &url, int rev = -1, bool recurse = true);
  // #CAT_Subversion Checkout a working copy and return the revision checked out.  Returns -1 on error.

  static int Update(const String &working_copy_path, int rev = -1);
  // #CAT_Subversion Update the working copy and return the revision.  Returns -1 on error.

  static bool Add(const String &file_or_dir, bool recurse = true, bool add_parents = true);
  // #CAT_Subversion Add files or directories to the working copy and schedule for future commit.  Returns false on error.

  static bool MakeDir(const String &new_dir, bool make_parents = true);
  // #CAT_Subversion Create a directory in the working copy.  Returns true if the directory was created or already exists.

  static bool MakeUrlDir(const String &url, const String &comment, bool make_parents = true);
  // #CAT_Subversion Create a directory in the repository.  Returns true if the directory was created or already exists.

  static int Checkin(const String &paths, const String &comment);
  // #CAT_Subversion Checkin files under the given path(s) (comma or newline separated list of files/dirs).  Returns the new revision number or -1 if nothing to commit.  Returns -2 on error.

  static int GetLastChangedRevision(const String &path);
  // #CAT_Subversion Get the last revision in which the passed path was changed, according to the working copy.  Returns -1 on error.

protected:
  TA_BASEFUNS_NOCOPY(Subversion);

private:
  void Initialize() { }
  void Destroy() { }
};

#endif // SUBVERSION_H_
