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

#include "DataSortEl.h"

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataSortEl);


void DataSortEl::Initialize() {
  order = ASCENDING;
}

void DataSortEl::UpdateName() {
  String nname = col_name + "_";
  if(order == ASCENDING)
    nname += "up";
  else
    nname += "dn";
  nname += GetListIdxSuffix();
  SetName(nname);
}

bool DataSortEl::SetName(const String& nm) {
  // Ensure name is a legal C-language identifier.
  String new_name = taMisc::StringCVar(nm);
  if (name == new_name) return true;
  name = new_name;
  if (!taMisc::is_changing_type)
    MakeNameUnique();
  return true;
}

String DataSortEl::GetDisplayName() const {
  String rval = col_name + " ";
  if(order == ASCENDING)
    rval += "up";
  else
    rval += "dn";
  return rval;
}

bool DataSortEl::BrowserEditSet(const String& new_val_str, int move_after) {
  if(new_val_str.empty()) return false;
  String tnm = trim(new_val_str);
  if(tnm.contains(" ")) {
    col_name = taMisc::StringCVar(tnm.before(" "));
    String opnm = trim(tnm.after(" "));
    opnm.downcase();
    if(opnm == "up" || opnm == "ascending") {
      order = ASCENDING;
    }
    else {
      order = DESCENDING;
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

void DataSortEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(col_lookup) {
    CheckError(col_lookup->is_matrix, quiet, rval,"cannot use matrix column to sort");
  }
}
