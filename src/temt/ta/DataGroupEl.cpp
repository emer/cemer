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
  name = col_name + " " + agg.GetAggName();
}

String DataGroupEl::GetName() const {
  return name;                  // need to use cached name for loading..
}

bool DataGroupEl::SetName(const String& nm) {
  if(nm.empty()) return false;
  String tnm = trim(nm);
  name = tnm;
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
    String new_name = taMisc::StringCVar(nm);
    if (col_name == new_name) return true;
    col_name = new_name;
  }
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
