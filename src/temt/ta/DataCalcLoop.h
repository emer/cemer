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

#ifndef DataCalcLoop_h
#define DataCalcLoop_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <DataOpList>
#include <ProgEl_List>
#include <ProgVar_List>
#include <ProgVar>

// declare all other types mentioned but not required to include:
class DataOpEl; // 
class ProgEl; // 
class TypeDef; // 


taTypeDef_Of(DataCalcLoop);

class TA_API DataCalcLoop : public DataSrcDestProg { 
  // enables arbitrary calculations and operations on data by looping row-by-row through the src_data table; can either just operate on src_data (using SetSrcRow) or generate new dest_data (using AddDestRow and SetDestRow)
INHERITED(DataSrcDestProg)
public:
  DataOpList		src_cols;
  // source columns to operate on (variables are labeled as s_xxx where xxx is col_name)
  DataOpList		dest_cols;
  // destination columns to operate on (variables are labeled as d_xxx where xxx is col_name)
  ProgEl_List		loop_code; // #SHOW_TREE the items to execute in the loop
  bool			use_col_numbers; // #DEF_false use column numbers instead of names to access the column data within the loop -- this is significantly faster but also much more brittle -- if the columns change at all after the program is compiled, difficult-to-debug errors can occur -- use with caution!
  ProgVar_List		src_col_vars;  // #READ_ONLY source column variables
  ProgVar_List		dest_col_vars;  // #READ_ONLY dest column variables
  ProgVar		src_row_var;	// #HIDDEN #READ_ONLY #NO_SAVE variable for FindVarName rval for src_row loop variable

  int 		ProgElChildrenCount() const override { return loop_code.size; }

  virtual DataOpEl* AddSrcColumn(const String& col_name);
  // #CAT_DataOp #BUTTON add a new source column to operate on
  virtual DataOpEl* AddDestColumn(const String& col_name);
  // #CAT_DataOp #BUTTON add a new dest column to operate on
  virtual void	AddAllSrcColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the src_cols list of columns 
  virtual void	AddAllDestColumns();
  // #BUTTON #CAT_Data add all columns from dest_data to the dest_cols list of columns 
  void	UpdateSpecDataTable() override;

  virtual ProgEl*	AddLoopCode(TypeDef* el_type)	{ return (ProgEl*)loop_code.New(1, el_type); }
  // #BUTTON #TYPE_ProgEl add a new loop code element

  ProgVar*	FindVarName(const String& var_nm) const override;

  String GetDisplayName() const override;
  String	GetToolbarName() const override { return "calc loop"; }

  PROGEL_SIMPLE_BASEFUNS(DataCalcLoop);
protected:
  void UpdateAfterEdit_impl() override;
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;
  void CheckChildConfig_impl(bool quiet, bool& rval) override;
  void	PreGenChildren_impl(int& item_id) override;

  virtual void	SetColProgVarFmData(ProgVar* pv, DataOpEl* ds);
  virtual void	UpdateColVars();
  // sync col vars from cols

  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssBody_impl(Program* prog) override; 
  void		GenCssPost_impl(Program* prog) override; 
  const String	GenListing_children(int indent_level) override;

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataCalcLoop_h
