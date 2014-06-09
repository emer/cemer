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

#include "DataCalcSetDestRow.h"
#include <DataCalcLoop>
#include <Program>
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(DataCalcSetDestRow);



void DataCalcSetDestRow::Initialize() {
}

String DataCalcSetDestRow::GetDisplayName() const {
  String rval = "Set Dest Row in: ";
  if(dest_data_var) {
    rval += dest_data_var->name;
  }
  else {
    rval += "?";
  }
  return rval;
}

void DataCalcSetDestRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcSetDestRow::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcSetDestRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}

void DataCalcSetDestRow::Copy_(const DataCalcSetDestRow& cp) {
  GetDataPtrsFmLoop();
}

void DataCalcSetDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(CheckError(!dcl, quiet, rval,"parent DataCalcLoop not found")) return;
  if(CheckError(!dcl->dest_data_var, quiet, rval,
                "DataCalcLoop::dest_data_var is NULL, but is needed")) return;
  // should be done by var, not us
//   CheckError(!dcl->dest_data_var->object_val, quiet, rval, "DataCalcLoop::dest_data_var variable NULL");
  CheckError(dcl->dest_data_var->object_type != &TA_DataTable, quiet, rval,
             "DataCalcLoop::dest_data_var variable does not point to a DataTable object");
}

void DataCalcSetDestRow::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcSetDestRow Error -- DataCalcLoop not found!!",
                  ProgLine::MAIN_LINE);
    return;
  }
  DataTable* dd = dcl->GetDestData();
  if(!dd) {
    prog->AddLine(this, "// DataCalcSetDestRow Error -- dest_data_var not set!!",
                  ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, String("if(") + dcl->dest_data_var->name
        + ".rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }",
                ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  dcl->dest_cols.GetColumns(dd);
  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = dd->data[ds->col_idx];
    String rval;
    if(da->is_matrix)
      rval += dcl->dest_data_var->name + ".SetValAsMatrix(";
    else
      rval += dcl->dest_data_var->name + ".SetValAsVar(";
    rval += "d_" + ds->col_name + ", ";
    if(dcl->use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", -1); // -1 = last row";
    prog->AddLine(this, rval);
  }
  prog->AddLine(this, dcl->dest_data_var->name + ".WriteClose();");
  dcl->dest_cols.ClearColumns();
}

bool DataCalcSetDestRow::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  if(dc.startsWith("set dest row"))
    return true;
  String dn = trim(GetDisplayName().before(":"));
  if (code.startsWith(dn))
    return true;
  return false;
}

bool DataCalcSetDestRow::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String remainder = code.after(":");
  if(remainder.empty()) return true;
  
  SigEmitUpdated();
  return true;
}