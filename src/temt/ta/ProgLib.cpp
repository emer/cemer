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

#include "ProgLib.h"
#include <NameVar>
#include <taMisc>

#include <QDir>
#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(ProgLib);


void ProgLib::Initialize() {
  not_init = true;
}

void ProgLib::FindPrograms() {
  Reset();                      // clear existing
  for(int pi=0; pi< taMisc::prog_lib_paths.size; pi++) {
    NameVar pathvar = taMisc::prog_lib_paths[pi];
    String path = pathvar.value.toString();
    String lib_name = pathvar.name;
    QDir dir(path);
    QStringList files = dir.entryList();
    for(int i=0;i<files.size();i++) {
      String fl = files[i];
      if(!fl.contains(".prog")) continue;
      ProgLibEl* pe = new ProgLibEl;
      pe->lib_name = lib_name;
      if(pe->ParseProgFile(fl, path))
        Add(pe);
      else
        delete pe;
    }
  }
  not_init = false;
}

taBase* ProgLib::NewProgram(ProgLibEl* prog_type, Program_Group* new_owner) {
  if(prog_type == NULL) return NULL;
  return prog_type->NewProgram(new_owner);
}

taBase* ProgLib::NewProgramFmName(const String& prog_nm, Program_Group* new_owner) {
  return NewProgram(FindName(prog_nm), new_owner);
}
