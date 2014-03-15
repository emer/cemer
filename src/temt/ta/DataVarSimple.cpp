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

#include "DataVarSimple.h"
#include <DataCol>
#include <DataTable>
#include <Program>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataVarSimple);

void DataVarSimple::Initialize() {
  writeToDataTable = false;    // child classes MUST SET
}

void DataVarSimple::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(row_spec != CUR_ROW && !row_var, quiet, rval, "row_var is NULL but is required!");
}

bool DataVarSimple::GenCss_OneVar(Program* prog, ProgVar* var, const String& idnm, int var_no) {
  if (!var) return false;
  // if the var is a matrix, then delegate to our Mat handler
  if ((var->var_type == ProgVar::T_Object) &&
    var->object_type->InheritsFrom(&TA_taMatrix))
    return GenCss_OneVarMat(prog, var, idnm, var_no);

  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";

  if(dt && (var->var_type == ProgVar::T_HardEnum || var->var_type == ProgVar::T_DynEnum)) {
    da = dt->FindColName(column_name);
    if(da->isMatrix()) {
      return GenCss_OneVarMatEnum(prog, var, idnm, var_no);
    }
  }

  if(dt) {
    da = dt->FindColName(column_name);
    if(da && da->isString())
      string_cvt = "(String)";  // cast variant value to a string for setting!
  }
  if(row_spec == CUR_ROW) {
    if(writeToDataTable)
      prog->AddLine(this, idnm + ".SetDataByName(" + string_cvt + var->name + ", \"" + column_name +"\");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetDataByName(\"" + column_name + "\");");
  }
  else if(row_spec == ROW_NUM) {
    if(writeToDataTable)
      prog->AddLine(this, idnm + ".SetValColName(" + string_cvt + var->name + ", \"" + column_name +"\", "
                    + row_var->name + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetValColName(\"" + column_name + "\", "
                    + row_var->name + ");");
  }
  else if(row_spec == ROW_VAL) {
    if(writeToDataTable)
      prog->AddLine(this, idnm + ".SetValColRowName(" + string_cvt + var->name + ", \"" + column_name + "\", \""
                    + row_var->name + "\", " + row_var->name + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetValColRowName(\"" + column_name +"\", \""
                    + row_var->name + "\", " + row_var->name + ");");
  }
  return true;
}

  bool DataVarSimple::GenCss_OneVarMat(Program* prog, ProgVar* var, const String& idnm, int var_no) {
  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(column_name);
    if(da && da->isString())
      string_cvt = "(String)";  // cast variant value to a string for setting!
  }
  // in all cases, we need a temp var that is ref counted, to hold the mat slice for the col
  prog->AddLine(this, "{taMatrix* __tmp_mat;");
  prog->IncIndent();
  // first, get the mat slice
  if (row_spec == CUR_ROW) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetMatrixDataByName(\"" + column_name + "\");");
  }
  else if (row_spec == ROW_NUM) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColName(\"" + column_name + "\", "
                  + row_var->name + ");");
  }
  else if (row_spec == ROW_VAL) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColRowName(\"" + column_name +"\", \""
                  + row_var->name + "\", " + row_var->name + ");");
  }
  if(writeToDataTable) {
    prog->AddLine(this, String("__tmp_mat.CopyFrom(") + column_name + ");");
  }
  else {
    prog->AddLine(this, column_name + ".CopyFrom(__tmp_mat);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  return true;
}

  bool DataVarSimple::GenCss_OneVarMatEnum(Program* prog, ProgVar* var, const String& idnm, int var_no) {
  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(column_name);
    if(da && da->isString())
      string_cvt = "(String)";  // cast variant value to a string for setting!
  }
  // in all cases, we need a temp var that is ref counted, to hold the mat slice for the col
  prog->AddLine(this, "{taMatrix* __tmp_mat;");
  prog->IncIndent();
  // first, get the mat slice
  if (row_spec == CUR_ROW) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetMatrixDataByName(\"" + column_name + "\");");
  }
  else if (row_spec == ROW_NUM) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColName(\"" + column_name + "\", "
                  + row_var->name + ");");
  }
  else if (row_spec == ROW_VAL) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColRowName(\"" + column_name +"\", \""
                  + row_var->name + "\", " + row_var->name + ");");
  }
  if(writeToDataTable) {
    prog->AddLine(this, String("__tmp_mat.InitValsFmVar(0);"));
    prog->AddLine(this, String("__tmp_mat.Set_Flat(1, ") + column_name + ");");
  }
  else {
    prog->AddLine(this, column_name + " = __tmp_mat.FindVal_Flat(1);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  return true;
}

void DataVarSimple::GenCssBody_impl(Program* prog) {
  if(!data_var) {
    prog->AddLine(this, "// data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  if(row_spec != CUR_ROW && !row_var) {
    prog->AddLine(this, "// row_var not set but needed!", ProgLine::MAIN_LINE);
    return;
  }
  String idnm = data_var->name;
  prog->AddLine(this, "// " + GetDisplayName(), ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
    GenCss_OneVar(prog, var, idnm, 0);
}

bool DataVarSimple::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String remainder = code.after(":");
  if(remainder.empty()) return true;

  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);

  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();

    if (name.startsWith("tab")) {
      data_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("var")) {
      var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("row_s")) {
      row_spec = StringToRowType(value);
    }
    else if (name.startsWith("row_v")) {
      row_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("col")) {
      column_name = value;
    }
  }

  SigEmitUpdated();
  return true;
}

