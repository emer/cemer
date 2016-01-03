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
//#include <DynEnumTableCol>

// member includes:

// declare all other types mentioned but not required to include:
class DataCol; //

taTypeDef_Of(DataTableCell);

class TA_API DataTableCell : public taOBase {
  // ##INLINE ##NO_TOKENS  A set of key bindings for shortcuts on menu items and for keyPressEvent interpretation
INHERITED(taOBase)
public:
//  DynEnumTableCol     row_values;     // values of the row_column
  DataCol*            row_column;  // #HIDDEN the values in this column will be displayed so the user can choose a row without referring to the row by number
  int                 row;         //
  String              value;       //
  
  String              name;        //  #HIDDEN - hide the objects name - is there a better way?

  TA_BASEFUNS_NOCOPY(DataTableCell);
  
protected:
  void    UpdateAfterEdit_impl() override;

private:
  void Initialize();
  void Destroy()     { };
};

#endif // DataTableCell_h
