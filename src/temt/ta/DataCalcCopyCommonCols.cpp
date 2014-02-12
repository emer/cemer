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

#include "DataCalcCopyCommonCols.h"
#include <DataCalcLoop>
#include <Program>
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(DataCalcCopyCommonCols);



void DataCalcCopyCommonCols::Initialize() {
  only_named_cols = false;
}

String DataCalcCopyCommonCols::GetDisplayName() const {
  String rval = "Copy Common Cols from: ";
  if(src_data_var)
    rval += src_data_var->name;
  else
    rval += "ERR! src_data_var is NULL";
  rval += " to: ";
  if(dest_data_var)
    rval += dest_data_var->name;
  else
    rval += "ERR! dest_data_var is NULL";
  return rval;
}

void DataCalcCopyCommonCols::GetDataPtrsFmLoop() {
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl || dcl->isDestroying()) return;
  src_data_var = dcl->src_data_var;
  dest_data_var = dcl->dest_data_var;
}

void DataCalcCopyCommonCols::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  GetDataPtrsFmLoop();
}

void DataCalcCopyCommonCols::InitLinks() {
  inherited::InitLinks();
  GetDataPtrsFmLoop();
}

void DataCalcCopyCommonCols::Copy_(const DataCalcCopyCommonCols& cp) {
  CopyInner_(cp);
  GetDataPtrsFmLoop();
}

void DataCalcCopyCommonCols::CheckThisConfig_impl(bool quiet, bool& rval) {
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

void DataCalcCopyCommonCols::GenCssBody_impl(Program* prog) {
  // can assume that the dcl variable has already been declared!!
  DataCalcLoop* dcl = GET_MY_OWNER(DataCalcLoop);
  if(!dcl) {
    prog->AddLine(this, "// DataCalcCopyCommonCols Error -- DataCalcLoop not found!!",
                  ProgLine::MAIN_LINE);
    return;
  }
  if(!dcl->dest_data_var) {
    prog->AddLine(this, "// DataCalcCopyCommonCols Error -- dest_data_var null in DataCalcLoop!!", ProgLine::MAIN_LINE);
    return;
  }

  prog->AddLine(this, String("if(") + dcl->dest_data_var->name
                + ".rows == 0) { taMisc::Error(\"Dest Rows == 0 -- forgot AddDestRow??\"); break; }", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  if(only_named_cols)
    prog->AddLine(this, "taDataProc::CopyCommonColsRow_impl(" + dcl->dest_data_var->name + ", " +
          dcl->src_data_var->name + ", common_dest_cols_named, common_src_cols_named, -1, src_row);");
  else
    prog->AddLine(this, "taDataProc::CopyCommonColsRow_impl(" + dcl->dest_data_var->name + ", " +
                  dcl->src_data_var->name + ", common_dest_cols, common_src_cols, -1, src_row);");
}
