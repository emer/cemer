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

#include "DataVarProgMatrix.h"
#include <DataCol>
#include <DataTable>
#include <Program>

TA_BASEFUNS_CTORS_DEFN(DataVarProgMatrix);



void DataVarProgMatrix::Initialize() {
}

bool DataVarProgMatrix::GenCss_OneVar(Program* prog, ProgVar* var, const String& idnm, int var_no) {
  if(!var) return false;
  DataCol* da = NULL;
  String col_nm = var->name.before('_', -1);
  if(TestError(col_nm.empty(), "GenCss_OneVar", "if your column is scalar use DataVarProg not DataVarProgMatrix, if your column contains matrix data your variable name must contain a '_' with part before that being name of column in data table to get/set value")) {
    return false;
  }
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(col_nm);
    if(da && da->isString())
      string_cvt = "(String)";  // cast variant value to a string for setting!
  }
  if(row_spec == CUR_ROW) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetMatrixCellData(" + string_cvt + var->name
                    + ", \"" + col_nm +"\", " + String(var_no) + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetMatrixCellData(\"" + col_nm
                    + "\", " + String(var_no) + ");");
  }
  else if(row_spec == ROW_NUM) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetMatrixFlatValColName(" + string_cvt + var->name
                    + ", \"" + col_nm +"\", " + row_var->name + ", " + String(var_no) + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetMatrixFlatValColName(\"" + col_nm + "\", "
                    + row_var->name + ", " + String(var_no) + ");");
  }
  else if(row_spec == ROW_VAL) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetMatrixFlatValColRowName(" + string_cvt + var->name
                    + ", \"" + col_nm + "\", \"" + row_var->name + "\", " + row_var->name
                    + ", " + String(var_no) + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetMatrixFlatValColRowName(\"" + col_nm +"\", \""
                    + row_var->name + "\", " + row_var->name + ", " + String(var_no) + ");");
  }
  return true;
}
