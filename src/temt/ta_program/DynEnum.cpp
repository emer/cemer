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
#include <ProgEl>

TA_BASEFUNS_CTORS_DEFN(DynEnum);

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

String DynEnum::NameVal() const {
  return NumberToName(value);
}

bool DynEnum::SetNameVal(const String& nm) {
  if(!enum_type) return false;
  value = NameToNumber(nm);
  return true;
}

String DynEnum::NumberToName(int val) const {
  if(!enum_type) return _nilString;
  return enum_type->NumberToName(val);
}

int    DynEnum::NameToNumber(const String& nm) const {
  if(!enum_type) return 0;
  return enum_type->NameToNumber(nm);
}

void  DynEnum::NumberToName_Array(String_Array& names, const int_Array& vals) const {
  if(!enum_type) return;
  enum_type->NumberToName_Array(names, vals);
}

void  DynEnum::NameToNumber_Array(int_Array& vals, const String_Array& names) const {
  if(!enum_type) return;
  enum_type->NameToNumber_Array(vals, names);
}

void  DynEnum::NumberToName_Matrix(String_Matrix& names, const int_Matrix& vals) const {
  if(!enum_type) return;
  enum_type->NumberToName_Matrix(names, vals);
}

void  DynEnum::NameToNumber_Matrix(int_Matrix& vals, const String_Matrix& names) const {
  if(!enum_type) return;
  enum_type->NameToNumber_Matrix(vals, names);
}

void DynEnum::MakeTemplate_fmtype(DynEnum* an_enum, TypeDef* td) {
  taBase* tok = (taBase*)td->GetInstance();
  if(tok) {
    taBase* o = tok->MakeToken();
    o->SetName("New" + td->name);
  }
//  for(int i=0;i<td->children.size;i++) {
//    TypeDef* chld = td->children[i];
//    MakeTemplate_fmtype(an_enum, chld);
//  }
}

DynEnum* DynEnum::MakeTemplate() {
  DynEnum* an_enum = new DynEnum;
  
  MakeTemplate_fmtype(an_enum, &TA_ProgEl);
  return an_enum;
}

