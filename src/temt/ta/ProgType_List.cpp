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

#include "ProgType_List.h"
#include <Program>
#include <DynEnumType>

void ProgType_List::Initialize() {
  SetBaseType(&TA_ProgType);
  setUseStale(true);
}

void ProgType_List::El_SetIndex_(void* it_, int idx) {
  ProgType* it = (ProgType*)it_;
  if (it->name.empty()) {
    it->name = "Type_" + (String)idx;
  }
}

DynEnumType* ProgType_List::NewDynEnum() {
  return (DynEnumType*)New_gui(1, &TA_DynEnumType); // gui op
}

taBase* ProgType_List::FindTypeName(const String& nm)  const {
  for (int i = 0; i < size; ++i) {
    ProgType* it = FastEl(i);
    taBase* ptr = it->FindTypeName(nm);
    if(ptr) return ptr;
  }
  return NULL;
}

void ProgType_List::GenCss(Program* prog) const {
  for (int i = 0; i < size; ++i) {
    ProgType* it = FastEl(i);
    it->GenCss(prog);
  }
}

const String ProgType_List::GenListing(int indent_level) const {
  String rval(0, 40 * size, '\0'); // buffer with typical-ish room
  for (int i = 0; i < size; ++i) {
    ProgType* it = FastEl(i);
    rval += it->GenListing(indent_level);
  }
  return rval;
}

void ProgType_List::setStale() {
  inherited::setStale();
  // note: there are no vars just in programs anymore
  // if we are in a program group, dirty all progs
  // note: we have to test if in a prog first, otherwise we'll always get a group
//   Program* prog = GET_MY_OWNER(Program);
//   if (!prog) {
//     Program_Group* grp = GET_MY_OWNER(Program_Group);
//     if (grp)
//       grp->SetProgsStale();
//   }
}

bool ProgType_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgType_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgType_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
