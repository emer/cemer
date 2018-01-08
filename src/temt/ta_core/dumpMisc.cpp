// Co2018ght 2013-2017, Regents of the University of Colorado,
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
#include <taProject>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taVersion>

taBase_PtrList 	dumpMisc::update_after;
taBase_PtrList  dumpMisc::post_update_after;
DumpPathSubList	dumpMisc::path_subs;
DumpPathTokenList dumpMisc::path_tokens;
VPUList 	dumpMisc::vpus;
taBase*		dumpMisc::dump_root = NULL;
taProject*      dumpMisc::dump_proj = NULL;
String		dumpMisc::dump_root_path;

static taVersion v809(8, 0, 9);

void dumpMisc::PostUpdateAfter() {
  taMisc::is_post_loading++;
  for (int i=0; i < dumpMisc::post_update_after.size; i++) {
    taBase* tmp = dumpMisc::post_update_after.FastEl(i);
    tmp->Dump_Load_post();
  }
  dumpMisc::post_update_after.Reset();
  taMisc::is_post_loading--;
}  

String dumpMisc::GetDumpPath(taBase* obj) {
  if(taMisc::is_loading && taMisc::loading_version < v809) {
    return obj->GetPath();
  }
  else {
    if(dump_root->IsParentOf(obj)) {
      return obj->GetPath(dump_root); // inside object, use numbers
    }
    else if(dump_proj && dump_proj->IsParentOf(obj)) {
      return obj->GetPathNames(dump_proj); // outside, use names
    }
    else {
      return obj->GetPathNames();    // way outside, definitely use names
    }
  }
}

taBase* dumpMisc::FindFromDumpPath(const String& path, MemberDef*& md) {
  taBase* rval = NULL;
  if(taMisc::is_loading && taMisc::loading_version < v809) {
    rval = tabMisc::root->FindFromPath(path, md);
  }
  else {
    rval = dump_root->FindFromPath(path, md);
    if(!rval && dump_proj) {
      rval = dump_proj->FindFromPath(path, md);
    }
    if(!rval) {
      rval = tabMisc::root->FindFromPath(path, md);
    }
  }
  return rval;
}
