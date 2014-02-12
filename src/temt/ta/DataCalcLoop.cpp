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

#include "DataCalcLoop.h"
#include <Program>
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(DataCalcLoop);

taTypeDef_Of(DataCalcAddDestRow);
taTypeDef_Of(DataCalcSetDestRow);
taTypeDef_Of(float_Matrix);
taTypeDef_Of(double_Matrix);
taTypeDef_Of(int_Matrix);

void DataCalcLoop::Initialize() {
  src_row_var.name = "src_row";
  use_col_numbers = false;
}

void DataCalcLoop::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying() || prg->isDestroying()) return;
  src_cols.name = "src_cols";
  dest_cols.name = "dest_cols";
  UpdateSpecDataTable();
  for(int i=0;i<loop_code.size;i++) {
    ProgEl* pe = loop_code[i];
    if(pe->InheritsFrom(&TA_DataCalcAddDestRow) || pe->InheritsFrom(&TA_DataCalcSetDestRow))
      pe->UpdateAfterEdit();    // get the data tables!
  }
}

void DataCalcLoop::UpdateSpecDataTable() {
  src_cols.SetDataTable(GetSrcData());
  dest_cols.SetDataTable(GetDestData());
  UpdateColVars();
}

void DataCalcLoop::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  // null ok in dest_data just as long as you don't call set dest!
}


void DataCalcLoop::SetColProgVarFmData(ProgVar* pv, DataOpEl* ds) {
  pv->SetVarFlag(ProgVar::LOCAL_VAR);
  if(!ds->col_lookup) return;   // nothing to do
  ValType vt = ds->col_lookup->valType();
  if(ds->col_lookup->is_matrix) {
    pv->var_type = ProgVar::T_Object;
    pv->object_val = NULL;
    if(vt == VT_FLOAT)
      pv->object_type = &TA_float_Matrix;
    else if(vt == VT_DOUBLE)
      pv->object_type = &TA_double_Matrix;
    else if(vt == VT_INT)
      pv->object_type = &TA_int_Matrix;
    else if(vt == VT_STRING)
      pv->object_type = &TA_String_Matrix;
    else
      pv->object_type = &TA_taMatrix;
  }
  else {
    if(vt == VT_FLOAT || vt == VT_DOUBLE) {
      pv->SetReal(0.0f);
    }
    else if(vt == VT_INT) {
      pv->SetInt(0);
    }
    else if(vt == VT_STRING) {
      pv->SetString("");
    }
  }
}

void DataCalcLoop::UpdateColVars() {
  src_cols.GetColumns(GetSrcData());
  String srcp = "s_";
  int ti, i;
  for(i = src_col_vars.size - 1; i >= 0; --i) { // delete not used ones
    ProgVar* pv = src_col_vars.FastEl(i);
    ti = src_cols.FindNameIdx(pv->name.after(srcp));
    if (ti >= 0) {
      SetColProgVarFmData(pv, src_cols.FastEl(ti));
      //      pa->UpdateFromVar(*pv);
    } else {
      src_col_vars.RemoveIdx(i);
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < src_cols.size; ++ti) {
    DataOpEl* ds = src_cols.FastEl(ti);
    i = src_col_vars.FindNameIdx(srcp + ds->col_name);
    if(i < 0) {
      ProgVar* pv = new ProgVar();
      pv->name = srcp + ds->col_name;
      SetColProgVarFmData(pv, ds);
      src_col_vars.Insert(pv, ti);
    } else if (i != ti) {
      src_col_vars.MoveIdx(i, ti);
    }
  }
  src_cols.ClearColumns();

  dest_cols.GetColumns(GetDestData());
  srcp = "d_";
  for(i = dest_col_vars.size - 1; i >= 0; --i) { // delete not used ones
    ProgVar* pv = dest_col_vars.FastEl(i);
    ti = dest_cols.FindNameIdx(pv->name.after(srcp));
    if (ti >= 0) {
      SetColProgVarFmData(pv, dest_cols.FastEl(ti));
    } else {
      dest_col_vars.RemoveIdx(i);
    }
  }
  // add args in target not in us, and put in the right order
  for (ti = 0; ti < dest_cols.size; ++ti) {
    DataOpEl* ds = dest_cols.FastEl(ti);
    i = dest_col_vars.FindNameIdx(srcp + ds->col_name);
    if(i < 0) {
      ProgVar* pv = new ProgVar();
      pv->name = srcp + ds->col_name;
      SetColProgVarFmData(pv, ds);
      dest_col_vars.Insert(pv, ti);
    } else if (i != ti) {
      dest_col_vars.MoveIdx(i, ti);
    }
  }
  dest_cols.ClearColumns();
}


String DataCalcLoop::GetDisplayName() const {
  String rval = "Calc Loop ";
  if(src_data_var) {
    rval += " from: " + src_data_var->name;
  }
  else {
    rval += "ERR! src_data_var is NULL";
  }
  if(dest_data_var) {
    rval += " to: " + dest_data_var->name;
  }
  return rval;
}

void DataCalcLoop::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  UpdateColVars();
  if(GetSrcData()) {
    src_cols.GetColumns(GetSrcData());
    src_cols.CheckConfig(quiet, rval);
    src_cols.ClearColumns();
  }
  if(GetDestData()) {
    dest_cols.GetColumns(GetDestData());
    dest_cols.CheckConfig(quiet, rval);
    dest_cols.ClearColumns();
  }
  loop_code.CheckConfig(quiet, rval);
}

ProgVar* DataCalcLoop::FindVarName(const String& var_nm) const {
  if(var_nm == "src_row")
    return (ProgVar*)&src_row_var;
  ProgVar* pv = src_col_vars.FindName(var_nm);
  if(pv) return pv;
  pv = dest_col_vars.FindName(var_nm);
  if(pv) return pv;
  return loop_code.FindVarName(var_nm);
}

void DataCalcLoop::PreGenChildren_impl(int& item_id) {
  loop_code.PreGen(item_id);
}

void DataCalcLoop::GenCssPre_impl(Program* prog) {
  if(!GetSrcData()) {
    prog->AddLine(this, "// no src data!", ProgLine::MAIN_LINE);
    return;
  }
  src_cols.GetColumns(GetSrcData());
  prog->AddLine(this, "{ DataCalcLoop* dcl = this" + GetPath(NULL, program()) + ";",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  if(dest_data_var) {
    prog->AddLine(this, dest_data_var->name + ".ResetData(); // all data ops clear out old existing data");
    prog->AddLine(this, "DataOpList common_dest_cols; // pre-initialize, for CopyCommonCols");
    prog->AddLine(this, "DataOpList common_src_cols;");
    prog->AddLine(this, "DataOpList common_dest_cols_named; // only the cols named in dest_cols");
    prog->AddLine(this, "DataOpList common_src_cols_named;  // only the cols named in src_cols");
    prog->AddLine(this, String("taDataProc::GetCommonCols(") + dest_data_var->name + ", "
                  + src_data_var->name + ", common_dest_cols, common_src_cols);");
    prog->AddLine(this, "common_dest_cols_named = common_dest_cols; common_src_cols_named = common_src_cols;");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_dest_cols_named, dcl->dest_cols);");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_dest_cols_named, dcl->src_cols);");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_src_cols_named, dcl->dest_cols);");
    prog->AddLine(this, "taDataProc::GetColIntersection(common_src_cols_named, dcl->src_cols);");
    prog->AddLine(this, dest_data_var->name + "->StructUpdate(true);");
  }
  prog->AddLine(this, src_data_var->name + "->StructUpdate(true);");

  prog->AddLine(this, String("for(int src_row=0; src_row < ") + src_data_var->name + ".rows; src_row++) {");
  prog->IncIndent();
  prog->AddVerboseLine(this, false, "\"in for loop\"");

  for(int i=0;i<src_cols.size; i++) {
    DataOpEl* ds = src_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = GetSrcData()->data[ds->col_idx];
    String rval;
    if(da->is_matrix) {
      ValType vt = da->valType();
      String mat_type = "taMatrix";
      if(vt == VT_FLOAT)
        mat_type = "float_Matrix";
      else if(vt == VT_DOUBLE)
        mat_type = "double_Matrix";
      else if(vt == VT_INT)
        mat_type = "int_Matrix";
      else if(vt == VT_STRING)
        mat_type = "String_Matrix";
      rval += mat_type + "* s_" + ds->col_name + " = " + src_data_var->name + ".GetValAsMatrix(";
    }
    else {
      rval += "Variant s_" + ds->col_name + " = " + src_data_var->name + ".GetValAsVar(";
    }
    if(use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", src_row);";
    prog->AddLine(this, rval);
  }
  src_cols.ClearColumns();
  // dest cols are only activated by DataAddDestRow
}

void DataCalcLoop::GenCssBody_impl(Program* prog) {
  loop_code.GenCss(prog);
}

void DataCalcLoop::GenCssPost_impl(Program* prog) {
  if(!GetSrcData()) return;
  prog->DecIndent();
  prog->AddLine(this, "} // for loop");
  if(dest_data_var)
    prog->AddLine(this, dest_data_var->name + "->StructUpdate(false);");
  prog->AddLine(this, src_data_var->name + "->StructUpdate(false);");
  prog->DecIndent();
  prog->AddLine(this, "} // DataCalcLoop dcl");
}

const String DataCalcLoop::GenListing_children(int indent_level) {
  return loop_code.GenListing(indent_level + 1);
}

void DataCalcLoop::AddAllSrcColumns() {
  src_cols.AddAllColumns_gui(GetSrcData());
  UpdateColVars();
}
void DataCalcLoop::AddAllDestColumns() {
  dest_cols.AddAllColumns_gui(GetDestData());
  UpdateColVars();
}

DataOpEl* DataCalcLoop::AddSrcColumn(const String& col_name) {
  DataOpEl* rval = src_cols.AddColumn(col_name, GetSrcData());
  UpdateColVars();
  return rval;
}
DataOpEl* DataCalcLoop::AddDestColumn(const String& col_name) {
  DataOpEl* rval = dest_cols.AddColumn(col_name, GetDestData());
  UpdateColVars();
  return rval;
}

