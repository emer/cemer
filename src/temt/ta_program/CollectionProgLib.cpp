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

#include "CollectionProgLib.h"
#include "ProgLib.h"
#include <FileProgLib>
#include <WikiProgLib>
#include <NameVar>
#include <taMisc>
#include <Program>
#include <Variant>

#include <QDir>
#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(CollectionProgLib);

void CollectionProgLib::Initialize() {
  init = false;
}

void CollectionProgLib::CreateSubLibs() {
  if(sub_libs.size >= WEB_USER_LIB) return;
  sub_libs.Reset();
  // NOTE: make this in same order as ProgLibs enum so can just use index addressing
  sub_libs.Add
    (new FileProgLib(taMisc::prog_lib_paths.GetVal("UserLib").toString(), "UserLib"));
  sub_libs.Add
    (new FileProgLib(taMisc::prog_lib_paths.GetVal("SystemLib").toString(), "SystemLib"));
  sub_libs.Add
    (new WikiProgLib(taMisc::plib_app_wiki, "WebAppLib"));
  sub_libs.Add
    (new WikiProgLib(taMisc::plib_sci_wiki, "WebSciLib"));
  if(taMisc::plib_user_wiki.nonempty()) {
    sub_libs.Add
      (new WikiProgLib(taMisc::plib_user_wiki, "WebUserLib"));
  }
}

ProgLib* CollectionProgLib::GetSubLib(ProgLibs library) {
  if(library >= SEARCH_LIBS) return NULL;
  CreateSubLibs();
  return sub_libs.SafeEl(library);
}

void CollectionProgLib::FindPrograms() {
  taMisc::Busy();
  taMisc::Info("loading program library -- can take a few moments depending..");
  CreateSubLibs();
  Reset();  // clear existing
  
  // Iterate over all program libraries in the collection and call their respective FindPrograms
  for (int i = 0; i < sub_libs.size; i++) {
    ProgLib* lib = sub_libs[i];
    lib->FindPrograms();
    for (int pi = 0; pi < lib->size; pi++) {
      Add(lib->FastEl(pi));
    }
  }
  init = true;
  taMisc::DoneBusy();
}

bool CollectionProgLib::SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving!");
    return false;
  }
  ProgLib* lib = GetSubLib(library);
  if(!lib) return false;
  lib->SaveProgGrpToProgLib(prg_grp, library);
  // lib->FindPrograms();          // local save already does update!
  return true;
}

bool CollectionProgLib::SaveProgToProgLib(Program* prg, ProgLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving!");
    return false;
  }
  ProgLib* lib = GetSubLib(library);
  if(!lib) return false;
  lib->SaveProgToProgLib(prg, library);
  // lib->FindPrograms();          // local save already does update!
  return true;
}
