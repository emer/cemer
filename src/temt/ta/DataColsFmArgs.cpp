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
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataColsFmArgs);


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
  String rval = "Data Cols Fm Args: ";
  
  if(data_var)
    rval += " table=" + data_var->name + " ";
  else
    rval += " table=? ";
  
  String row_var_name;
  if((bool)row_var)
    row_var_name = row_var->name;
  
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

bool DataColsFmArgs::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  if(dc.startsWith("datacol") || dc.startsWith("data col")) return true;
  return false;
}

bool DataColsFmArgs::CvtFmCode(const String& code) {
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
    else if (name.startsWith("row_s")) {
      row_spec = StringToRowType(value);
    }
    else if (name.startsWith("row_v")) {
      row_var = FindVarNameInScope(value, false); // don't make
    }
  }
  
  SigEmitUpdated();
  return true;
}

// copied from DataVarBase because this is the only other class using this
// and I didn't want to move the code to a common location
DataColsFmArgs::RowType DataColsFmArgs::StringToRowType(const String& row_type) {
  if (row_type == "row_num" || row_type == "ROW_NUM")
    return DataColsFmArgs::ROW_NUM;
  else if (row_type == "row_val" || row_type == "ROW_VAL")
    return DataColsFmArgs::ROW_VAL;
  else
    return DataColsFmArgs::CUR_ROW;
}

