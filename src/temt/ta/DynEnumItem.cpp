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

#include "DynEnumItem.h"
#include <Program>

void DynEnumItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(Program::IsForbiddenName(name)) {
    name = "My" + name;
  }
  DynEnumType* typ = GET_MY_OWNER(DynEnumType);
  if(typ) {
    if(typ->bits && value <= 0) {
      value = 1;
      typ->enums.OrderItems();
    }
  }
}

void DynEnumItem::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(Program::IsForbiddenName(name, false), quiet, rval,
	     "Name:",name,"is a css reserved name used for something else -- please choose another name");
}

String DynEnumItem::GetDisplayName() const {
  String rval = name + "=";
  DynEnumType* typ = GET_MY_OWNER(DynEnumType);
  if(typ && typ->bits)
    return rval + String(value, "%x");
  return rval + String(value);
}

bool DynEnumItem::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool DynEnumItem::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool DynEnumItem::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
