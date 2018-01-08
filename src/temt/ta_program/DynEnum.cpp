// Copyright 2013-2017, Regents of the University of Colorado,
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
#include <ProgVar>
#include <MemberDef>

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
  // doing this at progvar level now
  // CheckError(!enum_type, quiet, rval,
  //            "enum_type is not set for this dynamic enum value");
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

void  DynEnum::GetControlPanelLabel(MemberDef* mbr, String& label, const String& xtra_lbl, bool short_label) const {
  ProgVar* prog_var = (ProgVar*)GetOwner(&TA_ProgVar);
  if (prog_var) {
    prog_var->GetControlPanelLabel(mbr, label, xtra_lbl, short_label);
  }
  else {
    inherited::GetControlPanelLabel(mbr, label, xtra_lbl, short_label);
  }
}

void DynEnum::GetControlPanelDesc(MemberDef* mbr, String& eff_desc) const {
  ProgVar* prog_var = (ProgVar*)GetOwner(&TA_ProgVar);
  if (prog_var) {
    prog_var->GetControlPanelDesc(mbr, eff_desc);
  }
  else {
    inherited::GetControlPanelDesc(mbr, eff_desc);
  }
}

taBase::DumpQueryResult DynEnum::Dump_QuerySaveMember(MemberDef* md) {
  DumpQueryResult rval = DQR_SAVE; // only used for membs we match below
  if(md->name == "value") {
    if(owner && owner->InheritsFrom(&TA_ProgVar)) {
      ProgVar* own = (ProgVar*)owner;
      if(!own->HasVarFlag(ProgVar::SAVE_VAL))
        rval = DQR_NO_SAVE;
    }
  }
  return rval;
}
