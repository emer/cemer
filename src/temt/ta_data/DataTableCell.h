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
  DataCol*            row_column;   // the values in this column will be displayed so the user can choose a row without referring to the row by number
  DataCol*            value_column; //
  int                 row;          //
  String              value;        //
  
  void                GetControlPanelText(MemberDef* mbr, const String& xtra_lbl, String& full_lbl, String& desc) const override;
  // #IGNORE set a default label

  TA_BASEFUNS_NOCOPY(DataTableCell);
  
protected:

private:
  void Initialize();
  void Destroy()     { };
};

#endif // DataTableCell_h
