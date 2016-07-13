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
#include <Program>

#include <QDir>
#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(ProgLib);
TA_BASEFUNS_CTORS_DEFN(ProgLib_List);


void ProgLib::Initialize() {
  init = false;
}

taBase* ProgLib::NewProgram(ProgLibEl* prog_type, Program_Group* new_owner) {
  if(prog_type == NULL) return NULL;
  return prog_type->NewProgram(new_owner);
}

taBase* ProgLib::NewProgramFmName(const String& prog_nm, Program_Group* new_owner) {
  ProgLibEl* el = FindName(prog_nm);
  if(!el) {
    taMisc::Error("NewProgramFmName: could not find program of given name in library:",
                  prog_nm);
    return NULL;
  }
  return NewProgram(el, new_owner);
}

bool ProgLib::UpdateProgramFmName(const String& prog_nm, Program* prog) {
  ProgLibEl* el = FindName(prog_nm);
  if(!el) {
    taMisc::Error("UpdateProgramFmName: could not find program of given name in library:",
                  prog_nm);
    return false;
  }
  prog->UpdateFromProgLib(el);
  return true;
}
