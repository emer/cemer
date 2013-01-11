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

#include "dumpMisc.h"


/* Version 4.x Note on Error handling

  Version 3.x had a mechanism whereby any taMisc::Error call with a non-
  alphanum first character in the error msg would only print to cerr
  not raise an error dialog. In v4.x we have added a taMisc::Warning
  call for this purpose. So any call in the dump code that previously
  did not raise a gui window has been converted to a Warning.

*/
taBase_PtrList 	dumpMisc::update_after;
taBase_PtrList  dumpMisc::post_update_after;
DumpPathSubList	dumpMisc::path_subs;
DumpPathTokenList dumpMisc::path_tokens;
VPUList 	dumpMisc::vpus;
taBase*		dumpMisc::dump_root;
String		dumpMisc::dump_root_path;

void dumpMisc::PostUpdateAfter() {
  taMisc::is_post_loading++;
  for (int i=0; i < dumpMisc::post_update_after.size; i++) {
    taBase* tmp = dumpMisc::post_update_after.FastEl(i);
    tmp->Dump_Load_post();
  }
  dumpMisc::post_update_after.Reset();
  taMisc::is_post_loading--;
}  
