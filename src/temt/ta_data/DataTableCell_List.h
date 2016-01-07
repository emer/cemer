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

#ifndef DataTableCell_List_h
#define DataTableCell_List_h 1

// parent includes:
#include <DataTableCell>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(DataTableCell_List);

class TA_API DataTableCell_List : public taList<DataTableCell> {
  // a list of DataTableCell objects which are used to represent a table cell for linking with a control panel
INHERITED(taList<DataTableCell>)
public:
  DataTableCell*          FindCell(DataCol* value_column, int row);

  TA_BASEFUNS_NOCOPY(DataTableCell_List);
private:
  void Initialize()  { SetBaseType(&TA_DataTableCell); };
  void Destroy()     { };
};

#endif // DataTableCell_List_h
