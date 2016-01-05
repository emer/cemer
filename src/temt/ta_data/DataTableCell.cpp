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
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(DataTableCell);

void  DataTableCell::Initialize() {
  row_column = NULL;
  current_row = -1;             // start with invalid row
  value = "";
}

void DataTableCell::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  
  DataCol* dc = (DataCol*)GetOwner();
  if (dc) {
    // todo: almost certainly don't want to ALWAYS have the value setting here..
    // can we get away with not caching the value at all, and instead always
    // dynamically reading / writing cell value when needed -- otherwise it
    // creates a huge sync'ing problem!
    // also, important to check valid range -- starts invalid so not setting to null
    if(current_row < dc->rows() && current_row >= 0) {
      dc->SetValAsVar(value, current_row);
    }
  }
}
