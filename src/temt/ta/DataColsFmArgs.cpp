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

#include "DataColsFmArgs.h"
#include <Program>
#include <ProgVar>
#include <DataTable>
#include <taMisc>


void DataColsFmArgs::Initialize() {
  row_spec = CUR_ROW;
}

void DataColsFmArgs::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void DataColsFmArgs::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!data_var, quiet, rval, "data_var is NULL")) return; // fatal
  // should be done by var, not us
  //  CheckError(!data_var->object_val, quiet, rval, "data_var variable is NULL");
  CheckError(data_var->object_type != &TA_DataTable, quiet, rval,
             "data_var variable does not point to a DataTable object");
  CheckError(row_spec != CUR_ROW && !row_var, quiet, rval, "row_var is NULL but is required!");
}

String DataColsFmArgs::GetDisplayName() const {
  String rval = "Data Cols Fm Args";
  DataTable* dt = GetData();
  if(dt) {
    rval += " To: " + dt->name;
  }
  String row_var_name = "(ERR: not set!)";
  if((bool)row_var)
    row_var_name = row_var->name;
  if(row_spec  == CUR_ROW)
    rval += " cur_row";
  else if(row_spec == ROW_NUM)
    rval += " row_num: " + row_var_name;
  else
    rval += " row_val: " + row_var_name;
  return rval;
}

DataTable* DataColsFmArgs::GetData() const {
  if(!data_var) return NULL;
  if(data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)data_var->object_val.ptr();
}

void DataColsFmArgs::GenCssBody_impl(Program* prog) {
  DataTable* dt = GetData();
  if(!dt) {
    prog->AddLine(this, "// DataColsFmArgs: data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, "{ // DataColsFmArgs fm: " + dt->name, ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
  prog->IncIndent();
  prog->AddLine(this, "String dcfma_colnm, dcfma_argval;");
  prog->AddLine(this, "for(int j=0;j<" + data_var->name + ".cols();j++) {");
  prog->IncIndent();
  prog->AddLine(this, "dcfma_colnm = " + data_var->name + ".data[j].name;");
  prog->AddLine(this, "dcfma_argval = taMisc::FindArgByName(dcfma_colnm);");
  prog->AddLine(this, "if(dcfma_argval.empty()) continue;");
  if(row_spec == CUR_ROW) {
    prog->AddLine(this, data_var->name + ".SetDataByName(dcfma_argval, dcfma_colnm);");
  }
  else if(row_spec == ROW_NUM) {
    prog->AddLine(this, data_var->name + ".SetValColName(dcfma_argval, dcfma_colnm, "
                  + row_var->name + ");");
  }
  else if(row_spec == ROW_VAL) {
    prog->AddLine(this, data_var->name + ".SetValColRowName(dcfma_argval, dcfma_colnm, \""
                  + row_var->name + "\", " + row_var->name + ");");
  }
  if(taMisc::dmem_proc == 0) {
    prog->AddLine(this, String("taMisc::Info(\"Set column: \",dcfma_colnm,\"in data table:\",\"") +
                  dt->name + "\",\"to val:\",dcfma_argval);");
  }
  prog->DecIndent();
  prog->AddLine(this, "}");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void DataColsFmArgs::GenRegArgs(Program* prog) {
  DataTable* dt = GetData();
  if(dt) {
    for(int j=0;j<dt->cols();j++) {
      DataCol* dc = dt->data[j];
      prog->AddLine(this, "taMisc::AddEqualsArgName(\"" + dc->name + "\");");
      prog->AddLine(this, "taMisc::AddArgNameDesc(\"" + dc->name
                    + "\", \"DataColsFmArgs: data_table = " + dt->name + "\");");
    }
  }
}
