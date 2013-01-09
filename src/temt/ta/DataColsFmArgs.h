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

#ifndef DataColsFmArgs_h
#define DataColsFmArgs_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:
class DataTable; // 


class TA_API DataColsFmArgs: public ProgEl { 
  // sets column value(s) in a data table based on startup arguments of the same name as the column -- row in data table to set values in is specified by a program variable (which can itself be previously set by an argument)
INHERITED(ProgEl)
public:
  enum RowType {
    CUR_ROW,			// use the current row (i.e., the last one added or specifically set by Read or Write operation)
    ROW_NUM,			// row_var variable contains the row number to operate on
    ROW_VAL,			// row_var variable contains a value that is used to find the row number by searching within data table column with the same name as the row_var variable
  };

  ProgVarRef	data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to data table with columns that are to be set from startup args
  RowType	row_spec;	// how the row number within data table is specified
  ProgVarRef	row_var;	// #CONDEDIT_OFF_row_spec:CUR_ROW #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser program variable containing information about which row to operate on (depends on row_spec for what this information is)

  virtual DataTable* GetData() const;
  // get actual data table pointer from variable

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "DataTable"; }
  override String	GetToolbarName() const { return "data=args"; }

  void	GenRegArgs(Program* prog);
  // #IGNORE generate RegisterArgs code

  PROGEL_SIMPLE_BASEFUNS(DataColsFmArgs);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckThisConfig_impl(bool quiet, bool& rval);
  override void	GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // DataColsFmArgs_h
