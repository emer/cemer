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

#include "DumpPathSub.h"
#include <DumpPathSubList>
#include <taBase>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>


DumpPathSub::DumpPathSub(TypeDef* td, taBase* par, const String& o, const String& n) {
  type = td; parent = par;
  old_path = o; new_path = n;
}

void DumpPathSubList::AddPath(TypeDef* td, taBase* par, String& o, String& n) {
  int o_i = o.length()-1;
  int n_i = n.length()-1;
  int o_last_sep = -1;		// position of last separator character
  int n_last_sep = -1;
  // remove any trailing parts of name that are common to both..
  // often only the leading part of name is new, so no need to
  // replicate the same fix over and over..
  while ((o_i >= 0) && (n_i >= 0) && (o[o_i] == n[n_i])) {
    if(o[o_i] == '.') {
      o_last_sep = o_i;
      n_last_sep = n_i;
    }
    o_i--; n_i--;
  }
  String* op = (String*)&o;
  String* np = (String*)&n;
  String ot, nt;
  if(o_last_sep > 0) {
    op = &ot;
    np = &nt;
    ot = o.before(o_last_sep);
    nt = n.before(n_last_sep);
  }
  if(taMisc::verbose_load >= taMisc::MESSAGES) {
    String ppath = par->GetPathNames();
    taMisc::Warning("---> New Path Fix, old:",*op,"new:",*np,"in:",ppath);
  }
  DumpPathSub* nwsb = new DumpPathSub(td, par, *op, *np);
  Add(nwsb);
  if(par != tabMisc::root) {	// if local path, then add a global path fix too
    String ppath = par->GetPath();
    unFixPath(td, tabMisc::root, ppath); // un fix the parent if necessary
    String long_o = ppath + *op;
    String long_n = ppath + *np;
    nwsb = new DumpPathSub(td, tabMisc::root, long_o, long_n);
    Add(nwsb);
//     if(taMisc::verbose_load >= taMisc::MESSAGES) {
      taMisc::Warning("---> New Global Path Fix, old:",long_o,"new:",long_n);
//     }
  }
}

void DumpPathSubList::FixPath(TypeDef*, taBase* par, String& path) {
  // search backwards to get latest fix
  int i;
  for(i=size-1; i>=0; i--) {
    DumpPathSub* dp = FastEl(i);
    if(dp->parent != par) // scope by parent
      continue;
    if(path.contains(dp->old_path)) {
      path.gsub(dp->old_path, dp->new_path);
      return;			// done!
    }
  }
}

void DumpPathSubList::unFixPath(TypeDef*, taBase* par, String& path) {
  // search backwards to get latest fix
  int i;
  for(i=size-1; i>=0; i--) {
    DumpPathSub* dp = FastEl(i);
    if(dp->parent != par) // scope by parent
      continue;
    if(path.contains(dp->new_path)) {
      path.gsub(dp->new_path, dp->old_path);
      return;			// done!
    }
  }
}

