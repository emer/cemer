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

TA_BASEFUNS_CTORS_DEFN(DataTableCell_List);

DataTableCell*          DataTableCell_List::FindCell(DataCol* value_column, int row) {
  for(int i = 0; i < size; ++i) {
    DataTableCell* cell = FastEl(i);
    if (cell->value_column == value_column && cell->row == row) {
      return cell;
    }
  }
  return NULL;
}