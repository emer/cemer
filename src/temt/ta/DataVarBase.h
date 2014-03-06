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

#ifndef DataVarBase_h
#define DataVarBase_h 1

// parent includes:
#include <DataOneProg>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataVarBase);

class TA_API DataVarBase : public DataOneProg {
  // #VIRT_BASE A program element for exchanging information between program variables and data table values in columns with the same names as the variables -- scalar var/col and matrix var/col supported, and enum to/from Matrix localist code also supported (see DataVarBaseMatrix for accessing individual matrix cells)
INHERITED(DataOneProg)
public:
  enum RowType {
    CUR_ROW,			// use the current row (i.e., the last one added or specifically set by Read or Write operation)
    ROW_NUM,			// set row_var to a variable that contains the row number to operate on
    ROW_VAL,      // set row_var to a variable that contains a value that is used to find the row number by searching within data table column with the same name as the row_var variable
  };

  RowType     row_spec;	  // how the row number within data table is specified
  ProgVarRef  row_var;    // #CONDSHOW_OFF_row_spec:CUR_ROW #ITEM_FILTER_StdProgVarFilter program variable containing information about which row to operate on (depends on row_spec for what this information is)

  void                        UpdateAfterEdit_impl() override;
  DataVarBase::RowType        StringToRowType(const String& row_type);

  PROGEL_SIMPLE_BASEFUNS(DataVarBase);
protected:

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataVarBase_h
