// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#include "FlatTreeEl.h"
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(FlatTreeEl);

void FlatTreeEl::Initialize() {
  nest_level = 0;
  type = NULL;
  mdef = NULL;
  addr = NULL;
  parent_el = NULL;
  size = 0;
  tot_size = 0;
}

taHashVal FlatTreeEl::ComputeHashCode() const {
  // note: level is critical -- don't want to compare at diff levels
  return taHashEl::HashCode_String(name + "&" + value) + nest_level;
}

bool FlatTreeEl::MemberNoShow() const {
  if(!mdef || !obj) return false;
  return !mdef->GetCondOptTest("CONDSHOW", obj->GetTypeDef(), obj.ptr());
}

bool FlatTreeEl::MemberNoEdit() const {
  if(!mdef || !obj) return false;
  return !mdef->GetCondOptTest("CONDEDIT", obj->GetTypeDef(), obj.ptr());
}

String FlatTreeEl::GetTypeDecoKey() const {
  if(obj)
    return obj->GetTypeDecoKey();
  return _nilString;
}

String FlatTreeEl::GetDisplayName() const {
  return name;                  // for now..
}
