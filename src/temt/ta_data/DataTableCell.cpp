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

#include "DataTableCell.h"
#include <DataCol>
#include <DataTable>

#include <taMisc>
#include <taiMisc>

TA_BASEFUNS_CTORS_DEFN(DataTableCell);

void  DataTableCell::Initialize() {
  view_row = -1;             // start with invalid row
  index_row = -1;             // start with invalid row
  value = "";
  value_column = NULL;
  row_lookup_col = NULL;
  control_panel = NULL;
  enabled = true;
  dtc_is_column_type = false;
  row_lookup_value = "";
}

void DataTableCell::GetControlPanelText(MemberDef* mbr, const String& extra_label, String& full_lbl, String& desc) const {
  if (!value_column) return;

  GetControlPanelLabel(mbr, full_lbl);
}

void DataTableCell::GetControlPanelLabel(MemberDef* mbr, String& label) const {
  if (!value_column) return;
  
  String table_str = value_column->dataTable()->GetName().CamelToSnake().elidedTo(taiMisc::CP_ITEM_ELIDE_LENGTH_LONG);
  String column_str = value_column->GetName().CamelToSnake().elidedTo(taiMisc::CP_ITEM_ELIDE_LENGTH_SHORT);
  if (dtc_is_column_type) {
    label = "_ " + table_str + "_" + column_str;
  }
  else if (row_lookup_col) {
    label = table_str + "_" + column_str + "_" + row_lookup_col->GetValAsString(view_row);
  }
  else {
    if (view_row != -1) {
      label = table_str + "_" + column_str + "_" + "row_" + String(view_row);
    }
    else {
      label = table_str + "_" + column_str + "_" + "hidden_row";
    }
  }
}

void DataTableCell::SetControlPanelEnabled(bool do_enable) {
  enabled = do_enable;
  if (!do_enable) {
    SetBaseFlag(taBase::BF_GUI_READ_ONLY);
  }
  else {
    ClearBaseFlag(taBase::BF_GUI_READ_ONLY);
  }
}

void DataTableCell::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  
  if (!value_column) {
    taMisc::Error("Choose a data table column - DataTableCell::UpdateAfterEdit_impl() - NULL not allowed");
    return;
  }
  // user may have changed view_row or value_column by editing from the property panel
  DataTable* dt = value_column->dataTable();
  index_row = dt->GetIndexRow(view_row);
  value = value_column->GetValAsString(view_row);
  
  dt->NotifyControlPanel(this);
}
