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
#include <DataVarBase>

// member includes:
//#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataVarProg);

class TA_API DataVarProg : public DataVarBase {
  // A program element for exchanging information between program variables and data table values in columns with the same names as the variables -- scalar var/col and matrix var/col supported, and enum to/from Matrix localist code also supported (see DataVarProgMatrix for accessing individual matrix cells)
INHERITED(DataVarBase)
public:

  bool        set_data;	  // if true, values in data table are set according to current variable values, otherwise, it gets data from the data table into the variables
  bool        all_matches; // if true will set or read from any variable whose name matches a column name of specified data table

  ProgVarRef	var_1;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_2;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_3;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!
  ProgVarRef	var_4;		// #CONDSHOW_OFF_all_matches #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!

  String	GetDisplayName() const override;
  String  GetToolbarName() const override { return "data=vars"; }

  PROGEL_SIMPLE_BASEFUNS(DataVarProg);
protected:
  void	GenCssBody_impl(Program* prog) override;
  virtual bool  GenCss_OneVar(Program* prog, ProgVar* var, const String& idnm, int var_no);
  virtual bool  GenCss_OneVarMat(Program* prog, ProgVar* mat_var, const String& idnm, int var_no);
  virtual bool  GenCss_OneVarMatEnum(Program* prog, ProgVar* mat_var, const String& idnm, int var_no);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataVarProg_h
