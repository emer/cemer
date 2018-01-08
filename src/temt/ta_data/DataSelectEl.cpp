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

#include "DataSelectEl.h"
#include <MemberDef>
#include <ProgEl>
#include <taProject>
#include <taMisc>


TA_BASEFUNS_CTORS_DEFN(DataSelectEl);


void DataSelectEl::Initialize() {
  on = true;
  rel = Relation::EQUAL;
  use_var = false;
  cmp = 0;
  act_enabled = true;
}

void DataSelectEl::UpdateName() {
  String nname = col_name + "_" +
    TA_Relation.GetEnumString("Relations", rel) + "_";
  if(use_var && (bool)var)
    nname += var->name;
  else
    nname += taMisc::StringCVar(cmp.toString());
  nname += GetListIdxSuffix();
  SetName(nname);
}

bool DataSelectEl::SetName(const String& nm) {
  // Ensure name is a legal C-language identifier.
  String new_name = taMisc::StringCVar(nm);
  if (name == new_name) return true;
  name = new_name;
  if (!taMisc::is_changing_type)
    MakeNameUnique();
  return true;
}

String DataSelectEl::GetDisplayName() const {
  String rval = col_name + " " +
    TA_Relation.GetEnumLabel("Relations", rel)+ " ";
  if(use_var && (bool)var)
    rval += var->name;
  else
    rval += cmp.toString();
  return rval;
}

bool DataSelectEl::BrowserEditSet(const String& new_val_str, int move_after) {
  if(new_val_str.empty()) return false;
  if(move_after != -11) {
    taProject* proj = GetMyProj();
    if(proj) {
      proj->undo_mgr.SaveUndo(this, "BrowserEditSet", this);
    }
  }
  String tnm = trim(new_val_str);
  if(tnm.contains(" ")) {
    col_name = taMisc::StringCVar(tnm.before(" "));
    String opnm = trim(tnm.after(" "));
    String valstr;
    if(opnm.contains(" ")) {
      valstr = trim(opnm.after(" "));
      opnm = opnm.before(" ");
    }
    MemberDef* opmd = GetTypeDef()->members.FindName("rel");
    if(opmd) {
      opmd->SetValStr(opnm, (void*)this);
    }
    if(valstr.nonempty()) {
      if(use_var) {
        ProgEl* pel = GET_MY_OWNER(ProgEl);
        if(pel) {
          var = pel->FindVarNameInScope(valstr, true);
        }
      }
      else {
        cmp.updateFromString(valstr);
      }
    }
  }
  else {
    // Ensure name is a legal C-language identifier.
    String new_name = taMisc::StringCVar(new_val_str);
    if (col_name == new_name) return true;
    col_name = new_name;
  }
  UpdateAfterEdit();
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
