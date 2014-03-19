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
  name = col_name + " ";
  if(order == ASCENDING)
    name += "up";
  else
    name += "dn";
}

String DataSortEl::GetName() const {
  return name;                  // need to use cached name for loading..
}

bool DataSortEl::SetName(const String& nm) {
  String tnm = trim(nm);
  name = tnm;
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
    String new_name = taMisc::StringCVar(nm);
    if (col_name == new_name) return true;
    col_name = new_name;
  }
  return true;
}

void DataSortEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(col_lookup) {
    CheckError(col_lookup->is_matrix, quiet, rval,"cannot use matrix column to sort");
  }
}
