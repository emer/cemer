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

#ifndef DataVarProg_h
#define DataVarProg_h 1

// parent includes:
#include <DataOneProg>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataVarProg);

class TA_API DataVarProg : public DataOneProg { 
  // A program element for exchanging information between program variables and data table values in columns with the same names as the variables -- scalar var/col and matrix var/col supported, and enum to/from Matrix localist code also supported (see DataVarProgMatrix for accessing individual matrix cells)
INHERITED(DataOneProg)
public:
  enum RowType {
    CUR_ROW,			// use the current row (i.e., the last one added or specifically set by Read or Write operation)
    ROW_NUM,			// row_var variable contains the row number to operate on
    ROW_VAL,			// row_var variable contains a value that is used to find the row number by searching within data table column with the same name as the row_var variable
  };

  bool		    set_data;	  // if true, values in data table are set according to current variable values, otherwise, it gets data from the data table into the variables
  RowType	    row_spec;	  // how the row number within data table is specified
  ProgVarRef	row_var;	  // #CONDEDIT_OFF_row_spec:CUR_ROW #ITEM_FILTER_StdProgVarFilter program variable containing information about which row to operate on (depends on row_spec for what this information is)
  bool		    quiet;		  // #CONDSHOW_OFF_row_spec:CUR_ROW do not generate an error if the row_var value is not found (either row num beyond bounds, or row_val not found -- just don't set anything)
  bool        all_matches;// if true will set any variable whose name matches a column name of specified data table

  ProgVarRef	var_1;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_2;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_3;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_4;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!

  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "data=vars"; }

  PROGEL_SIMPLE_BASEFUNS(DataVarProg);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);

  override void	GenCssBody_impl(Program* prog);
  virtual bool  GenCss_OneVar(Program* prog, ProgVar* var, const String& idnm, int var_no);
  virtual bool  GenCss_OneVarMat(Program* prog, ProgVar* mat_var, const String& idnm, int var_no);
  virtual bool  GenCss_OneVarMatEnum(Program* prog, ProgVar* mat_var, const String& idnm, int var_no);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataVarProg_h
