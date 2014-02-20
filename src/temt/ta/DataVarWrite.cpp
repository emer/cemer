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

#include "DataVarWrite.h"
#include <DataCol>
#include <DataTable>
#include <Program>

TA_BASEFUNS_CTORS_DEFN(DataVarWrite);

void DataVarWrite::Initialize() {
  writeToDataTable = true;    // MUST SET
}

String DataVarWrite::GetDisplayName() const {
  String rval;
  String row_var_name = "(ERROR: not set!)";
  if((bool)row_var)
    row_var_name = row_var->name;

  rval = "To table: ";

  if(data_var)
    rval += data_var->name;
  else
    rval += "(SELECT: data table),";

  if (!column_name.empty())
    rval += " column: " + column_name + ",";
  else
    rval += "(ENTER: column name),";

  if(row_spec  == CUR_ROW)
    rval += " cur_row";
  else if(row_spec == ROW_NUM)
    rval += " row_num: " + row_var_name + ",";
  else
    rval += " row_val: " + row_var_name + ",";

    rval += " From variable: ";

  if(var)
    rval += var->name + " ";

  return rval;
}
