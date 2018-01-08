// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef dumpMisc_h
#define dumpMisc_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taBase_PtrList>
#include <DumpPathSubList>
#include <DumpPathTokenList>
#include <VPUList>

// declare all other types mentioned but not required to include:
class taProject; //

class TA_API dumpMisc {
  // #NO_TOKENS ##NO_CSS ##NO_MEMBERS miscellaneous stuff for dump files
public:
  static taBase_PtrList		update_after; // objects to update after loading (still in is_loading context)
  static taBase_PtrList		post_update_after; // objects to update after loading, dispatched from event loop 
  static DumpPathSubList 	path_subs;     // path substitutions
  static DumpPathTokenList	path_tokens;  // path tokens
  static VPUList 		vpus;	      // pointers that couldn't get cached out
  static taBase*		dump_root;    // top-level object for load or save
  static taProject*             dump_proj;    // project scope for load or save
  static String			dump_root_path; // path of top-level object for load or save 

  static String                 GetDumpPath(taBase* obj);
  // special path function optimized for dump file saving
  static taBase*                FindFromDumpPath(const String& path, MemberDef*& md);
  // special path function optimized for dump file saving
  
  static void 		 	PostUpdateAfter(); // called by taiMiscCore when there were guys on pua list
};

#endif // dumpMisc_h
