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

TA_BASEFUNS_CTORS_DEFN(DataTableCell);

void  DataTableCell::Initialize() {
  row = -1;             // start with invalid row
  value = "";
  value_column = NULL;
  row_column = NULL;
}

void DataTableCell::GetControlPanelText(MemberDef* mbr, const String& extra_label, String& full_lbl, String& desc) const {
  if (!value_column) return;
  if (row_column) {
    full_lbl = value_column->dataTable()->GetName() + "__" + value_column->GetName() + "__" + row_column->GetValAsString(row);
  }
  else {
    full_lbl = value_column->dataTable()->GetName() + "__" + value_column->GetName() + "__" + "row_" + String(row);
  }

//  for (int i=0; i<value_column->rows(); i++) {
//    String str_1 = value_column->GetValAsString(i);
////    taMisc::DebugInfo(str_1);
//    value_column->UnSetMatrixViewMode();
//    String str_2 = value_column->AR()->SafeElAsStr(i);
////    taMisc::DebugInfo(str_2);
//    value_column->SetMatrixViewMode();
//  }
}
