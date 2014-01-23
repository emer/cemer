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

#ifndef DataSelectColsProg_h
#define DataSelectColsProg_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <DataOpList>

// declare all other types mentioned but not required to include:
class DataOpEl; // 


taTypeDef_Of(DataSelectColsProg);

class TA_API DataSelectColsProg : public DataSrcDestProg { 
  // ##DEF_CHILD_select_spec ##DEF_CHILDNAME_Select_Spec selects rows from src_data into dest_data according to select_spec
INHERITED(DataSrcDestProg)
public:
  DataOpList		select_spec; // #SHOW_TREE columns to select

  virtual DataOpEl* AddColumn(const String& col_name) { return select_spec.AddColumn(col_name, GetSrcData()); }
  // #CAT_Data #BUTTON add a new column to operate on
  virtual void	AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the select_spec list of ops columns 
  void	UpdateSpecDataTable() CPP11_OVERRIDE;

  taList_impl*	children_() CPP11_OVERRIDE {return &select_spec; }	
  Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const CPP11_OVERRIDE
  { return select_spec.Elem(idx, mode); }
  String GetDisplayName() const CPP11_OVERRIDE;
  String	GetToolbarName() const CPP11_OVERRIDE { return "sel cols"; }

  TA_SIMPLE_BASEFUNS(DataSelectColsProg);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE; 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataSelectColsProg_h
