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

#include "DataVarRead.h"
#include <DataCol>
#include <DataTable>
#include <Program>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(DataVarRead);

void DataVarRead::Initialize() {
  writeToDataTable = false;   // MUST SET
}

String DataVarRead::GetDisplayName() const {
  String row_var_name;
  if((bool)row_var)
    row_var_name = row_var->name;

  String rval = "From table: ";

  if(data_var)
    rval += " table=" + data_var->name + " ";
  else
    rval += " table=? ";

  if (!column_name.empty())
    rval += " column=" + column_name + " ";
  else
    rval += " column=? ";

  if(row_spec  == CUR_ROW)
    rval += " row_spec=cur_row ";
  else if(row_spec == ROW_NUM)
    rval += " row_spec=row_num ";
  else if(row_spec == ROW_VAL)
    rval += " row_spec=row_val ";
  else
    rval += " row_spec=cur_row ";

  // only display row_var if needed
  if(row_spec == ROW_NUM || row_spec == ROW_VAL) {
    if (row_var)
      rval += " row_var=" + row_var_name + " ";
    else
      rval += " row_var=? ";
  }

  if(var)
    rval += " var=" + var->name + " ";
  else
    rval += " var=? ";

  return rval;
}

bool DataVarRead::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  if(dc.startsWith("from table:")) return true;
  return false;
}
