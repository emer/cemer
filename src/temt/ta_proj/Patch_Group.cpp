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

#include "Patch_Group.h"
#include <Patch>
#include <taProject>
#include <taDateTime>

TA_BASEFUNS_CTORS_DEFN(Patch_Group);

void Patch_Group::Initialize() {
  SetBaseType(&TA_Patch);
}

void Patch_Group::LoadPatch() {
  CallFun("Load");              // args etc
}

void Patch_Group::FindPatches() {
  // todo: same code as program library??  browser locations..
}

Patch* Patch_Group::NewPatch() {
  Patch* patch = (Patch*)NewEl(1);
  taProject* own = GET_MY_OWNER(taProject);
  if(own) {
    patch->author = own->author;
    patch->email = own->email;
    patch->date = taDateTime::CurrentDateTimeStampString();
  }
  return patch;
}
