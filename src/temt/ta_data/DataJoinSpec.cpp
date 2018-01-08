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

#include "DataJoinSpec.h"

TA_BASEFUNS_CTORS_DEFN(DataJoinSpec);


void DataJoinSpec::Initialize() {
  type = INNER;
  nomatch_warn = true;
}

void DataJoinSpec::SetDataTable(DataTable* dt_a, DataTable* dt_b) {
  col_a.SetDataTable(dt_a);
  col_b.SetDataTable(dt_b);
}

void DataJoinSpec::GetColumns(DataTable* dt_a, DataTable* dt_b) {
  col_a.GetColumns(dt_a);
  col_b.GetColumns(dt_b);
}

void DataJoinSpec::ClearColumns() {
  col_a.ClearColumns();
  col_b.ClearColumns();
}

String DataJoinSpec::GetDisplayName() const {
//  return "join: col_a:" + col_a.GetDisplayName() + " to col_b: " + col_b.GetDisplayName();
  
  String rval = "join spec:";
  if (!col_a.col_name.empty())
    rval += " col_a = " + col_a.col_name;
  else
    rval += " col_a = ? ";
  
  if (!col_b.col_name.empty())
    rval += " col_b = " + col_b.col_name;
  else
    rval += " col_b = ? ";
  
  if(type  == INNER)
    rval += " join type = Inner ";
  else
    rval += " join type = Left ";

  return rval;
}

void DataJoinSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  col_a.CheckConfig(quiet, rval);
  col_b.CheckConfig(quiet, rval);
}
