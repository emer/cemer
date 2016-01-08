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

#ifndef DataTableCell_h
#define DataTableCell_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class DataCol; //

taTypeDef_Of(DataTableCell);

class TA_API DataTableCell : public taOBase {
  // ##INLINE ##NO_TOKENS represents a single cell within a data column, for interfacing with control panels 
INHERITED(taOBase)
public:
  DataCol*            row_column;       // the value in this column/row will be used in the label
  DataCol*            value_column;     // the column with the editable value
  int                 row;              // the row as the user sees it
  int                 row_indexes_row;  // #HIDDEN the row in the underlying matrix that holds all rows, visible and hidden
  String              value;            // content of the table cell
  
  void                GetControlPanelText(MemberDef* mbr, const String& xtra_lbl, String& full_lbl, String& desc) const override;
  // #IGNORE set a default label

  TA_BASEFUNS_NOCOPY(DataTableCell);
  
protected:

private:
  void Initialize();
  void Destroy()     { };
};

#endif // DataTableCell_h
