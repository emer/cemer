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

#include "DataTableCell_List.h"

#include <DataTable>
#include <SigLinkSignal>

TA_BASEFUNS_CTORS_DEFN(DataTableCell_List);

DataTableCell* DataTableCell_List::FindCell(DataCol* value_column, int row) {
  for(int i = 0; i < size; ++i) {
    DataTableCell* cell = FastEl(i);
    if (!value_column && cell->view_row == row) {
      return cell;
    }
    else if (cell->value_column == value_column && cell->view_row == row) {
      return cell;
    }
  }
  return NULL;
}

DataTableCell* DataTableCell_List::FindCellEnabled(DataCol* value_column, int row) {
  for(int i = 0; i < size; ++i) {
    DataTableCell* cell = FastEl(i);
    if (!value_column && cell->view_row == row && cell->enabled) {
      return cell;
    }
    else if (cell->value_column == value_column && cell->view_row == row && cell->enabled) {
      return cell;
    }
  }
  return NULL;
}

DataTableCell* DataTableCell_List::FindCellIndexRow(DataCol* value_column, int index_row) {
  for(int i = 0; i < size; ++i) {
    DataTableCell* cell = FastEl(i);
    if (!value_column && cell->index_row == index_row) {
      return cell;
    }
    else if (cell->value_column == value_column && cell->index_row == index_row) {
      return cell;
    }
  }
  return NULL;
}

DataTableCell* DataTableCell_List::FindColumnTypeDTC(DataCol* value_column) {
  for(int i = 0; i < size; ++i) {
    DataTableCell* cell = FastEl(i);
    if (cell->value_column == value_column && cell->column_type_dtc) {
      return cell;
    }
  }
  return NULL;
}

void DataTableCell_List::UpdateViewRows() {
  DataTable* dt = (DataTable*)this->GetOwner();
  for (int i=0; i<size; i++) {
    DataTableCell* dtc = FastEl(i);
    dtc->view_row = dt->GetViewRow(dtc->index_row);
    
    // set enabled state
    if (dtc->view_row == -1) {
      dtc->SetControlPanelEnabled(false);
    }
    else {
      dtc->SetControlPanelEnabled(true);
    }
  }
}

void DataTableCell_List::SigEmit(int sls, void* op1, void* op2) {
  if (sls == SLS_ITEM_DELETING) {
    this->RemoveEl_(op1);
  }
  inherited::SigEmit(sls, op1, op2);
}
