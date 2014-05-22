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

#include "DataGroupEl.h"

#include <MemberDef>
#include <taMisc>


TA_BASEFUNS_CTORS_DEFN(DataGroupEl);


void DataGroupEl::Initialize() {
}

void DataGroupEl::UpdateName() {
  String nname = col_name + "_" + agg.GetAggName();
  nname += GetListIdxSuffix();
  SetName(nname);
}

bool DataGroupEl::SetName(const String& nm) {
  // Ensure name is a legal C-language identifier.
  String new_name = taMisc::StringCVar(nm);
  if (name == new_name) return true;
  name = new_name;
  if (!taMisc::is_changing_type)
    MakeNameUnique();
  return true;
}

String DataGroupEl::GetDisplayName() const {
  return col_name + " " + agg.GetAggName();
}

bool DataGroupEl::BrowserEditSet(const String& new_val_str, int move_after) {
  if(new_val_str.empty()) return false;
  String tnm = trim(new_val_str);
  if(tnm.contains(" ")) {
    col_name = taMisc::StringCVar(tnm.before(" "));
    String opnm = trim(tnm.after(" "));
    MemberDef* opmd = TA_Aggregate.members.FindName("op");
    if(opmd) {
      opmd->SetValStr(opnm, (void*)&agg);
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

void DataGroupEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(col_lookup) {
    CheckError((agg.op == Aggregate::GROUP) && col_lookup->isMatrix(), quiet, rval,
               "cannot use matrix column to GROUP");
    ValType mvt = agg.MinValType();
    CheckError((mvt == Aggregate::VT_INT) && !col_lookup->isNumeric(), quiet, rval,
               "aggregation operator:", agg.GetAggName(), "requires numeric data to operate on, but column named:", col_name, "is not numeric");
  }
}
