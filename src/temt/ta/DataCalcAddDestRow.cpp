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

#include "DataCalcAddDestRow.h"


void DataCalcAddDestRow::Initialize() {
}

String DataCalcAddDestRow::GetDisplayName() const {
  String rval = "Add Row to: ";
  if(dest_data_var) {
    rval += dest_data_var->name;
  }
  else {
    rval += "ERR! dest_data_var is NULL";
  }
  return rval;
}

void DataCalcAddDestRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcAddDestRow::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcAddDestRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}

void DataCalcAddDestRow::Copy_(const DataCalcAddDestRow& cp) {
  GetDataPtrsFmLoop();
}

void DataCalcAddDestRow::CheckThisConfig_impl(bool quiet, bool& rval) {
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

void DataCalcAddDestRow::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcAddDestRow Error -- DataCalcLoop not found!!",
                  ProgLine::MAIN_LINE);
    return;
  }
  DataTable* dd = dcl->GetDestData();
  if(!dd) {
    prog->AddLine(this, "// DataCalcAddDestRow Error -- dest_data_var not set!!",
                  ProgLine::MAIN_LINE);
    return;
  }

  dcl->dest_cols.GetColumns(dd);
  prog->AddLine(this, dcl->dest_data_var->name + "->AddBlankRow();", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);

  for(int i=0;i<dcl->dest_cols.size; i++) {
    DataOpEl* ds = dcl->dest_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = dd->data[ds->col_idx];
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
      rval += mat_type + "* d_" + ds->col_name + " = " + dcl->dest_data_var->name + ".GetValAsMatrix(";
    }
    else {
      rval += "Variant d_" + ds->col_name + " = " + dcl->dest_data_var->name + ".GetValAsVar(";
    }
    if(dcl->use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", -1); // -1 = last row";
    prog->AddLine(this, rval);
  }
  dcl->dest_cols.ClearColumns();
}
