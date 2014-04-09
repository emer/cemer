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

#include "DataVarProg.h"
#include <DataCol>
#include <DataTable>
#include <Program>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataVarProg);

void DataVarProg::Initialize() {
  set_data = false;
  all_matches = false;
 }

void DataVarProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(row_spec != CUR_ROW && !row_var, quiet, rval, "row_var is NULL but is required!");
}

String DataVarProg::GetDisplayName() const {
  String row_var_name;
  if((bool)row_var)
    row_var_name = row_var->name;

  String rval;
  if(set_data)
    rval = "Data To: ";
  else
    rval = "Data From: ";

  if(data_var)
    rval += " table=" + data_var->name + " ";
  else
    rval += " table=? ";

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

  if(set_data)
    rval += " set=to ";
  else
    rval += " set=from ";

  if(all_matches)
    rval += " all_matches=true ";
  else
    rval += " all_matches=false ";

  if (!all_matches) {
    if(var_1)
      rval += " var_1=" + var_1->name + " ";
    else
      rval += " var_1=? ";

    if(var_2)
      rval += " var_2=" + var_2->name + " ";
    else
      rval += " var_2=? ";

    if(var_3)
      rval += " var_3=" + var_3->name + " ";
    else
      rval += " var_3=? ";

    if(var_4)
      rval += " var_4=" + var_4->name + " ";
    else
      rval += " var_4=? ";
  }

  return rval;
}

bool DataVarProg::GenCss_OneVar(Program* prog, ProgVar* var, const String& idnm, int var_no) {
  if (!var) return false;
  // if the var is a matrix, then delegate to our Mat handler
  if ((var->var_type == ProgVar::T_Object) &&
    var->object_type->InheritsFrom(&TA_taMatrix))
    return GenCss_OneVarMat(prog, var, idnm, var_no);

  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";

  if(dt && (var->var_type == ProgVar::T_HardEnum || var->var_type == ProgVar::T_DynEnum)) {
    da = dt->FindColName(var->name);
    if(da->isMatrix()) {
      return GenCss_OneVarMatEnum(prog, var, idnm, var_no);
    }
  }

  // todo: could do some verbose logging here..

  if(dt) {
    da = dt->FindColName(var->name);
    if(da && da->isString())
      string_cvt = "(String)";  // cast variant value to a string for setting!
  }
  if(row_spec == CUR_ROW) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetDataByName(" + string_cvt + var->name + ", \"" + var->name +"\");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetDataByName(\"" + var->name + "\");");
  }
  else if(row_spec == ROW_NUM) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetValColName(" + string_cvt + var->name + ", \"" + var->name +"\", "
                    + row_var->name + ", " + String(quiet)  + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetValColName(\"" + var->name + "\", "
                    + row_var->name + ", " + String(quiet) + ");");
  }
  else if(row_spec == ROW_VAL) {
    if(set_data)
      prog->AddLine(this, idnm + ".SetValColRowName(" + string_cvt + var->name + ", \"" + var->name+ "\", \""
                    + row_var->name + "\", " + row_var->name + ", " + String(quiet) + ");");
    else
      prog->AddLine(this, var->name + " = " + idnm + ".GetValColRowName(\"" + var->name +"\", \""
                    + row_var->name + "\", " + row_var->name + ", " + String(quiet) + ");");
  }
  return true;
}

  bool DataVarProg::GenCss_OneVarMat(Program* prog, ProgVar* var, const String& idnm, int var_no) {
  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(var->name);
    if(da && da->isString())
      string_cvt = "(String)";  // cast variant value to a string for setting!
  }
  // in all cases, we need a temp var that is ref counted, to hold the mat slice for the col
  prog->AddLine(this, "{taMatrix* __tmp_mat;");
  prog->IncIndent();
  // first, get the mat slice
  if (row_spec == CUR_ROW) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetMatrixDataByName(\"" + var->name + "\");");
  }
  else if (row_spec == ROW_NUM) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColName(\"" + var->name + "\", "
                  + row_var->name + ", " + String(quiet) + ");");
  }
  else if (row_spec == ROW_VAL) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColRowName(\"" + var->name +"\", \""
                  + row_var->name + "\", " + row_var->name + ", " + String(quiet) + ");");
  }
  if(set_data) {
    prog->AddLine(this, String("__tmp_mat.CopyFrom(") + var->name + ");");
  }
  else {
    prog->AddLine(this, var->name + ".CopyFrom(__tmp_mat);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  return true;
}

  bool DataVarProg::GenCss_OneVarMatEnum(Program* prog, ProgVar* var, const String& idnm, int var_no) {
  DataCol* da = NULL;
  DataTable* dt = GetData();
  String string_cvt = "";
  if(dt) {
    da = dt->FindColName(var->name);
    if(da && da->isString())
      string_cvt = "(String)";  // cast variant value to a string for setting!
  }
  // in all cases, we need a temp var that is ref counted, to hold the mat slice for the col
  prog->AddLine(this, "{taMatrix* __tmp_mat;");
  prog->IncIndent();
  // first, get the mat slice
  if (row_spec == CUR_ROW) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetMatrixDataByName(\"" + var->name + "\");");
  }
  else if (row_spec == ROW_NUM) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColName(\"" + var->name + "\", "
                  + row_var->name + ", " + String(quiet) + ");");
  }
  else if (row_spec == ROW_VAL) {
    prog->AddLine(this, String("__tmp_mat = ") + idnm + ".GetValAsMatrixColRowName(\"" + var->name +"\", \""
                  + row_var->name + "\", " + row_var->name + ", " + String(quiet) + ");");
  }
  if(set_data) {
    prog->AddLine(this, String("__tmp_mat.InitValsFmVar(0);"));
    prog->AddLine(this, String("__tmp_mat.Set_Flat(1, ") + var->name + ");");
  }
  else {
    prog->AddLine(this, var->name + " = __tmp_mat.FindVal_Flat(1);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  return true;
}

void DataVarProg::GenCssBody_impl(Program* prog) {
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
  if (!all_matches) {
    GenCss_OneVar(prog, var_1, idnm, 0);
    GenCss_OneVar(prog, var_2, idnm, 1);
    GenCss_OneVar(prog, var_3, idnm, 2);
    GenCss_OneVar(prog, var_4, idnm, 3);
  }
  else {
    DataTable* dt = GetData();
    if (!dt)
      return;
    ProgVar_List all_vars = program()->vars;
    for (int i = 0; i < all_vars.size; i++) {
      String var_name = all_vars.SafeEl(i)->name;
      if (!row_var || var_name != row_var->name) {  // don't try to set the row variable itself - this is just the key to which row to use
        int idx = dt->FindColNameIdx(var_name);
        if (idx >= 0) {
          ProgVar* pvar = all_vars.SafeEl(i);
          GenCss_OneVar(prog, pvar, idnm, 0);
        }
      }
    }
  }
}

bool DataVarProg::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  if(dc.startsWith("data from") || dc.startsWith("data to")) return true;
  return false;
}

bool DataVarProg::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true; // nothing we can do

  if(dc.startsWith("data to"))
    set_data = true;
  else
    set_data = false;

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
    else if (name == "var_1") {
      var_1 = FindVarNameInScope(value, false); // don't make
    }
    else if (name =="var_2") {
      var_2 = FindVarNameInScope(value, false); // don't make
    }
    else if (name == "var_3") {
      var_3 = FindVarNameInScope(value, false); // don't make
    }
    else if (name == "var_4") {
      var_4 = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("row_s")) {
      row_spec = StringToRowType(value);
    }
    else if (name.startsWith("row_v")) {
      row_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("set")) {
      if (value == "to" || value == "true")
        set_data = true;
      else if (value == "from" || value == "false")
        set_data = false;
      else
        set_data = false;
    }
    else if (name.startsWith("all")) {
      if (value == "true")
        all_matches = true;
      else
        all_matches = false;
    }
  }
  SigEmitUpdated();
  return true;
}
