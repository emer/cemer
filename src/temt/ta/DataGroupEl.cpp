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


void DataGroupEl::Initialize() {
}

String DataGroupEl::GetDisplayName() const {
  return col_name + " " + agg.GetAggName();
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
