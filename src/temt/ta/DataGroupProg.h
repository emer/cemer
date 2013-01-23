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

#ifndef DataGroupProg_h
#define DataGroupProg_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <DataGroupSpec>

// declare all other types mentioned but not required to include:
class DataOpEl; // 


TypeDef_Of(DataGroupProg);

class TA_API DataGroupProg : public DataSrcDestProg { 
  // groups src_data into dest_data according to group_spec
INHERITED(DataSrcDestProg)
public:
  DataGroupSpec		group_spec; // #SHOW_TREE data grouping specification

  virtual DataOpEl* AddColumn(const String& col_name) { return group_spec.AddColumn(col_name, GetSrcData()); }
  // #CAT_Data #BUTTON add a new column to operate on
  virtual void	AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the group_spec list of ops columns 
  override void	UpdateSpecDataTable();

  override String GetDisplayName() const;
  override String	GetToolbarName() const { return "group"; }

  TA_SIMPLE_BASEFUNS(DataGroupProg);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataGroupProg_h
