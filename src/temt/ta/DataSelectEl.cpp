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


void DataSelectEl::Initialize() {
  on = true;
  rel = EQUAL;
  use_var = false;
  cmp = 0;
  act_enabled = true;
}

String DataSelectEl::GetDisplayName() const {
  String rval = col_name + " " +
    GetTypeDef()->GetEnumString("Relations", rel)+ " ";
  if(use_var && (bool)var)
    rval += var->name;
  else
    rval += cmp.toString();
  return rval;
}

bool DataSelectEl::Eval(const Variant& val) {
  if(use_var && (bool)var) {
    cmp = var->GetVar();        // get current val
  }
  switch(rel) {
  case EQUAL:
    return val == cmp;
  case NOTEQUAL:
    return val != cmp;
  case LESSTHAN:
    return val < cmp;
  case GREATERTHAN:
    return val > cmp;
  case LESSTHANOREQUAL:
    return val <= cmp;
  case GREATERTHANOREQUAL:
    return val >= cmp;
  case CONTAINS:
    return val.toString().contains(cmp.toString());
  case NOT_CONTAINS:
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
               !(rel == EQUAL || rel == NOTEQUAL), quiet, rval,
               "matrix column selection only uses EQUAL or NOTEQUAL and checks for a 1 value at matrix cell given by cmp index");
  }
  CheckError(use_var && !var, quiet, rval, "use_var is selected but no var variable is set!");
  if(use_var && (bool)var) {
    CheckError(var->HasVarFlag(ProgVar::LOCAL_VAR), quiet, rval, "var for use_var is a local variable -- must be a global var in .args or .vars");
  }
}
