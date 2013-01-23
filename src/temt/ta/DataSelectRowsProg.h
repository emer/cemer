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

#ifndef DataSelectRowsProg_h
#define DataSelectRowsProg_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <DataSelectSpec>

// declare all other types mentioned but not required to include:
class DataOpEl; // 


TypeDef_Of(DataSelectRowsProg);

class TA_API DataSelectRowsProg : public DataSrcDestProg { 
  // selects rows from src_data into dest_data according to select_spec
INHERITED(DataSrcDestProg)
public:
  DataSelectSpec	select_spec; // #SHOW_TREE data selection specification

  virtual DataOpEl* AddColumn(const String& col_name) { return select_spec.AddColumn(col_name, GetSrcData()); }
  // #CAT_Data #BUTTON add a new column to operate on
  virtual void	AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the select_spec list of ops columns 
  override void	UpdateSpecDataTable();

  override String GetDisplayName() const;
  override String	GetToolbarName() const { return "sel rows"; }

  TA_SIMPLE_BASEFUNS(DataSelectRowsProg);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataSelectRowsProg_h
