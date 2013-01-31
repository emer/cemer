// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef taCodeUtils_h
#define taCodeUtils_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:

TypeDef_Of(taCodeUtils);

class TA_API taCodeUtils {
  // #NO_TOKENS #INSTANCE code for operating on the ta codebase itself
friend class InitProcRegistrar;
public:

  static bool	CreateNewSrcFiles(const String& type_nm, const String& top_path,
                                  const String& src_dir);
  // #CAT_File create new .h header and .cpp source file for type name as top_path/src_dir/<type_nm>.h|.cpp, and create header include stubs in top_path/include/<type_nm>|.h -- top_path must be full path to source top (e.g., $HOME/emergent) -- if files already exist, a _new suffix is added, and return value is false (else true) -- also does svn add using shell to add to svn -- files have src_dir/COPYRIGHT.txt appended at top if avail, and .cpp file automatically includes header

  static bool   RenameFileSVN(const String& old_filename, const String& new_filename);
  // #CAT_File rename file from old to new name in current working directory (or absolute path) -- returns success -- uses shell command to "svn mv" -- requires svn commandline to be avail
  static bool   RemoveFileSVN(const String& filename);
  // #CAT_File remove file in current working directory (or absolute path) -- returns success -- uses shell command to "svn rm" -- requires svn commandline to be avail

  static bool	RenameType(const String& type_nm, const String& new_nm,
                           const String& top_path, const String& src_dir);
  // #CAT_File rename type name to new name, replacing string in all files in given directory (WARNING: just does a very basic gsub string replacement, so type name better be unique!), and doing an svn mv on the file name for that type (does this before replace) -- recommend svn commit prior to doing this!

  static bool	RemoveType(const String& type_nm, 
                           const String& top_path, const String& src_dir);
  // #CAT_File removes a given type -- svn rm files that define the type (assumes one type per file!) including the include files, and reports on all the files that reference this type in the rest of the code

  static bool	ListAllInherits(const String& type_nm);
  // #CAT_File List all of the types that inherit from the given type -- goes beyond the direct parentage lists to see subclasses of subclasses

  static String TypeIncludes(TypeDef* td);
  // get the include files for give type, based on existing ta info

  static bool	CreateNewSrcFilesExisting(const String& type_nm, const String& top_path,
					  const String& src_dir);
  // #CAT_File create all new source files for an existing type -- just calls TypeDef version of this
  static void	CreateAllNewSrcFiles();
  // #CAT_File create all new source files!!  this is a one-time function that will be removed!

};

#endif // taCodeUtils_h
