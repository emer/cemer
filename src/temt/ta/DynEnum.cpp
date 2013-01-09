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

#include "DynEnum.h"

void DynEnum::Initialize() {
  value = 0;
}

void DynEnum::Destroy() {
  CutLinks();
}

String DynEnum::GetDisplayName() const {
  if((bool)enum_type)
    return enum_type->name + " " + NameVal();
  else
    return "(no dyn enum type!)";
}

void DynEnum::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!enum_type, quiet, rval,
             "enum_type is not set for this dynamic enum value");
}

const String DynEnum::NameVal() const {
  if(!enum_type) return _nilString;
  if(enum_type->bits) {
    String rval;
    for(int i=0;i<enum_type->enums.size;i++) {
      DynEnumItem* it = enum_type->enums.FastEl(i);
      if(value & it->value) {
        if(!rval.empty()) rval += "|";
        rval += it->name;
      }
    }
    return rval;
  }
  else {
    for(int i=0;i<enum_type->enums.size;i++) {
      DynEnumItem* it = enum_type->enums.FastEl(i);
      if(value == it->value) {
        return it->name;
      }
    }
  }
  return _nilString;
}

bool DynEnum::SetNameVal(const String& nm) {
  if(!enum_type) return false;
  DynEnumItem* it = enum_type->enums.FindName(nm);
  if(TestError(!it, "SetNameVal", "value label:", nm, "not found!"))
    return false;
  value = it->value;
  return true;
}

