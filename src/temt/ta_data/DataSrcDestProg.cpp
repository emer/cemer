// Copyright 2017, Regents of the University of Colorado,
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

#include "DataSrcDestProg.h"

#include <taMisc>
#include <DataTable>
#include <taGuiDialog>

TA_BASEFUNS_CTORS_DEFN(DataSrcDestProg);

taTypeDef_Of(DataTable);

void DataSrcDestProg::Initialize() {
}

void DataSrcDestProg::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(!src_data_var, quiet, rval, "src_data_var is NULL")) return; // fatal
  // should be done by var, not us
//   CheckError(!src_data_var->object_val, quiet, rval, "src_data_var variable NULL");
  CheckError(src_data_var->object_type != &TA_DataTable, quiet, rval,
             "src_data_var variable does not point to a DataTable object");
  // NULL OK in dest_data_var!
}

DataTable* DataSrcDestProg::GetSrcData() {
  if(!src_data_var) return NULL;
  if(src_data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)src_data_var->object_val.ptr();
}

DataTable* DataSrcDestProg::GetDestData() {
  if(!dest_data_var) return NULL;
  if(dest_data_var->object_type != &TA_DataTable) return NULL;
  return (DataTable*)dest_data_var->object_val.ptr();
}

String DataSrcDestProg::AddColumnDialog() {
  taGuiDialog dlg;
  String column_name;
  dlg.Reset();
  dlg.prompt = "Choose a column to add to select specification ";
  dlg.win_title = "Choose column for Select method";
  dlg.AddWidget("main", "", "");
  dlg.AddVBoxLayout("mainv","","main","");
  String curow = "lbl";
  dlg.AddHBoxLayout(curow, "mainv","","");
  dlg.AddLabel("full_lbl_lbl", "main", curow, "label=Column: ;");
  MemberDef* md = FindMemberName("src_table");
  String tool_tip = "tooltip=enter a column name; Conrol + space will show all columns in the source data table.";
  dlg.AddStringFieldForLookup(md, this, &column_name, "full_lbl", "main", curow, tool_tip);  // must pass member def and base object (this) to get completion to work
  
  int drval = dlg.PostDialog(true);
  
  return column_name;
}

