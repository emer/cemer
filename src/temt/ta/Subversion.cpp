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

#include "SubversionClient.h"

String
Subversion::GetUsername(const String &url, bool prompt)
{
  // Get the cached username for the given URL.  If no username cached and
  // prompt=true, prompt the user with a dialog.  Otherwise returns empty
  // string.
  try {
    SubversionClient client;
    std::string username = client.GetUsername(
      url.chars(),
      prompt ? SubversionClient::CHECK_CACHE_THEN_PROMPT_USER
             : SubversionClient::CHECK_CACHE_ONLY
    );
    return String(username.c_str());
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Could not get username.\n", ex.what());
    return "";
  }
}

int
Subversion::Checkout(const String &working_copy_path, const String &url, int rev, bool recurse)
{
  // Checkout a working copy and return the revision checked out.
  // Returns -1 on error.
  try {
    SubversionClient client;
    client.SetWorkingCopyPath(working_copy_path.chars());
    return client.Checkout(url.chars(), rev, recurse);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Could not complete checkout.\n", ex.what());
    return -1;
  }
}

int
Subversion::Update(const String &working_copy_path, int rev)
{
  // Update the working copy and return the revision.  Returns -1 on error.
  try {
    SubversionClient client;
    client.SetWorkingCopyPath(working_copy_path.chars());
    return client.Update(rev);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Could not update working copy.\n", ex.what());
    return -1;
  }
}

bool
Subversion::Add(const String &file_or_dir, bool recurse, bool add_parents)
{
  // Add files or directories to the working copy and schedule for future
  // commit.  Returns false on error.
  try {
    SubversionClient client;
    client.Add(file_or_dir.chars(), recurse, add_parents);
    return true;
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Could not add files to working copy.\n", ex.what());
    return false;
  }
}

bool
Subversion::MakeDir(const String &new_dir, bool make_parents)
{
  // Create a directory in the working copy.  Returns true if the directory
  // was created or already exists.
  try {
    SubversionClient client;
    return client.MakeDir(new_dir.chars(), make_parents);
  }
  catch (const SubversionClient::Exception &ex) {
    if (ex.GetErrorCode() == SubversionClient::EMER_ERR_ENTRY_EXISTS ||
        ex.GetErrorCode() == SubversionClient::EMER_ERR_FS_ALREADY_EXISTS)
    {
      return true; // OK, already exists.
    }
    taMisc::Error("Could not create directory in working copy.\n", ex.what());
    return false;
  }
}

bool
Subversion::MakeUrlDir(const String &url, const String &comment, bool make_parents)
{
  // Create a directory in the repository.  Returns true if the directory was
  // created or already exists.
  try {
    SubversionClient client;
    return client.MakeUrlDir(url.chars(), comment.chars(), make_parents);
  }
  catch (const SubversionClient::Exception &ex) {
    if (ex.GetErrorCode() == SubversionClient::EMER_ERR_ENTRY_EXISTS ||
        ex.GetErrorCode() == SubversionClient::EMER_ERR_FS_ALREADY_EXISTS)
    {
      return true; // OK, already exists.
    }
    taMisc::Error("Could not create directory in repository.\n", ex.what());
    return false;
  }
}

int
Subversion::Checkin(const String &paths, const String &comment)
{
  // Checkin files under the given paths (comma or newline separated list of
  // files/dirs).  Returns the new revision number or -1 if nothing to commit.
  // Returns -2 on error.
  try {
    SubversionClient client;
    return client.Checkin(comment.chars(), paths.chars());
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Could not check in files.\n", ex.what());
    return -2;
  }
}

int
Subversion::GetLastChangedRevision(const String &path)
{
  // Get the last revision in which the passed path was changed, according
  // to the working copy.  Returns -1 on error.
  try {
    SubversionClient client;
    return client.GetLastChangedRevision(path.chars());
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Could not get revision info.\n", ex.what());
    return -1;
  }
}
