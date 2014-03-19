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

#include "DataSelectEl.h"
#include <MemberDef>
#include <taMisc>


TA_BASEFUNS_CTORS_DEFN(DataSelectEl);


void DataSelectEl::Initialize() {
  on = true;
  rel = Relation::EQUAL;
  use_var = false;
  cmp = 0;
  act_enabled = true;
}

void DataSelectEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  name = col_name + " " +
    GetTypeDef()->GetEnumString("Relations", rel)+ " ";
  if(use_var && (bool)var)
    name += var->name;
  else
    name += cmp.toString();
}

String DataSelectEl::GetName() const {
  return name;                  // need to use cached name for loading..
}

bool DataSelectEl::SetName(const String& nm) {
  String tnm = trim(nm);
  name = tnm;
  if(tnm.contains(" ")) {
    col_name = taMisc::StringCVar(tnm.before(" "));
    String opnm = trim(tnm.after(" "));
    if(opnm.contains(" "))      // todo: do further parsing!
      opnm = opnm.before(" ");
    MemberDef* opmd = GetTypeDef()->members.FindName("rel");
    if(opmd) {
      opmd->SetValStr(opnm, (void*)this);
    }
  }
  else {
    // Ensure name is a legal C-language identifier.
    String new_name = taMisc::StringCVar(nm);
    if (col_name == new_name) return true;
    col_name = new_name;
  }
  return true;
}

bool DataSelectEl::Eval(const Variant& val) {
  if(use_var && (bool)var) {
    cmp = var->GetVar();        // get current val
  }
  switch(rel) {
  case Relation::EQUAL:
    return val == cmp;
  case Relation::NOTEQUAL:
    return val != cmp;
  case Relation::LESSTHAN:
    return val < cmp;
  case Relation::GREATERTHAN:
    return val > cmp;
  case Relation::LESSTHANOREQUAL:
    return val <= cmp;
  case Relation::GREATERTHANOREQUAL:
    return val >= cmp;
  case Relation::CONTAINS:
    return val.toString().contains(cmp.toString());
  case Relation::NOT_CONTAINS:
    return !val.toString().contains(cmp.toString());
  }
  return false;
}

void DataSelectEl::UpdateEnabled() {
  act_enabled = on;
  if((bool)enable_var) {
    act_enabled = on && enable_var->GetVar().toBool();
  }
}

void DataSelectEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(col_lookup) {
    CheckError(col_lookup->is_matrix && col_lookup->cell_size() > 1 &&
               !(rel == Relation::EQUAL || rel == Relation::NOTEQUAL), quiet, rval,
               "matrix column selection only uses EQUAL or NOTEQUAL and checks for a 1 value at matrix cell given by cmp index");
  }
  CheckError(use_var && !var, quiet, rval, "use_var is selected but no var variable is set!");
  if(use_var && (bool)var) {
    CheckError(var->HasVarFlag(ProgVar::LOCAL_VAR), quiet, rval, "var for use_var is a local variable -- must be a global var in .args or .vars");
  }
}
