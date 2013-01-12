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

#include "DataCalcSetSrcRow.h"
#include <DataCalcLoop>



void DataCalcSetSrcRow::Initialize() {
}

String DataCalcSetSrcRow::GetDisplayName() const {
  String rval = "Set Row in: ";
  if(src_data_var) {
    rval += src_data_var->name;
  }
  else {
    rval += "ERR! src_data_var is NULL";
  }
  return rval;
}

void DataCalcSetSrcRow::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcSetSrcRow::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcSetSrcRow::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}

void DataCalcSetSrcRow::Copy_(const DataCalcSetSrcRow& cp) {
  GetDataPtrsFmLoop();
}

void DataCalcSetSrcRow::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  CheckError(!dcl, quiet, rval, "parent DataCalcLoop not found");
}

void DataCalcSetSrcRow::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcSetSrcRow Error -- DataCalcLoop not found!!",
                  ProgLine::MAIN_LINE);
    return;
  }
  DataTable* sd = dcl->GetSrcData();
  if(!sd) {
    prog->AddLine(this, "// DataCalcSetSrcRow Error -- src_data_var not set!!",
                  ProgLine::MAIN_LINE);
    return;
  }

  dcl->src_cols.GetColumns(sd);
  for(int i=0;i<dcl->src_cols.size; i++) {
    DataOpEl* ds = dcl->src_cols[i];
    if(ds->col_idx < 0) continue;
    DataCol* da = sd->data[ds->col_idx];
    String rval;
    if(da->is_matrix)
      rval += dcl->src_data_var->name + ".SetValAsMatrix(";
    else
      rval += dcl->src_data_var->name + ".SetValAsVar(";
    rval += "s_" + ds->col_name + ", ";
    if(dcl->use_col_numbers)
      rval += String(ds->col_idx);
    else
      rval += String("\"") + da->name + String("\"");
    rval += ", src_row);";
    prog->AddLine(this, rval);
  }
  prog->AddLine(this, dcl->src_data_var->name + ".WriteClose();");
  dcl->dest_cols.ClearColumns();
}
