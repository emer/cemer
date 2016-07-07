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
  not_init = true;
}

void CollectionProgLib::setupSubLibs() {
  subProgLibs.Reset();
  ProgLib* subLib;
  subLib = new FileProgLib(taMisc::prog_lib_paths.GetVal("SystemLib").toString(), "SystemLib");
  subProgLibs.AddUnique(NameVar("SystemLib",(Variant)subLib));
  subLib = new FileProgLib(taMisc::prog_lib_paths.GetVal("UserLib").toString(), "UserLib");
  subProgLibs.AddUnique(NameVar("UserLib",(Variant)subLib));
  subLib = new WikiProgLib("test", "WebLib");
  subProgLibs.AddUnique(NameVar("WebLib",(Variant)subLib));
}

void CollectionProgLib::FindPrograms() {
  if (subProgLibs.size == 0)
    setupSubLibs();

  Reset();  // clear existing
  
  
  ProgLib* subLib;
  //Iterate over all program libraries in the collection and call their respective FindPrograms
  for (int i = 0; i < subProgLibs.size; i++) {
    subLib = (ProgLib*)subProgLibs[i].value.toPtr();
    subLib->FindPrograms();
    for (int pi = 0; pi < subLib->size; pi++) {
      Add(subLib->FastEl(pi));
    }
  }

  not_init = false;
}

taBase* CollectionProgLib::NewProgram(ProgLibEl* prog_type, Program_Group* new_owner) {
  if(prog_type == NULL) return NULL;
  return prog_type->NewProgram(new_owner);
}

taBase* CollectionProgLib::NewProgramFmName(const String& prog_nm, Program_Group* new_owner) {
  return NewProgram(FindName(prog_nm), new_owner);
}

bool CollectionProgLib::SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) {
  return false;
}

bool CollectionProgLib::SaveProgToProgLib(Program* prg, ProgLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving -- program saved in local directory!");
    return false;
  }
  ProgLib* subLib;
  String path = "./";
  if(library == USER_LIB)
    subLib = (ProgLib*)subProgLibs.GetVal("UserLib").toPtr();
  else if(library == SYSTEM_LIB) {
    subLib = (ProgLib*)subProgLibs.GetVal("SystemLib").toPtr();
  }
  else if(library == WEB_LIB) {
    subLib = (ProgLib*)subProgLibs.GetVal("WebLib").toPtr();
  } else {
    return false;
  }
  subLib->SaveProgToProgLib(prg, library);
  FindPrograms();
  return false;
}
