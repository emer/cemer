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

#ifndef DataOpBaseSpec_h
#define DataOpBaseSpec_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <DataOpList>

// declare all other types mentioned but not required to include:
class DataTable; // 
class DataOpEl; // 


taTypeDef_Of(DataOpBaseSpec);

class TA_API DataOpBaseSpec : public taNBase {
  // #STEM_BASE ##CAT_Data ##DEF_CHILD_ops ##DEF_CHILDNAME_Operators a datatable operation specification -- contains a list of operation elements associated with different data columns
INHERITED(taNBase)
public:
  DataOpList	ops;		// the list of operation elements, associated with different data columns

  virtual void 	SetDataTable(DataTable* dt) { ops.SetDataTable(dt); }
  // #CAT_DataOp set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt) { ops.GetColumns(dt); }
  // #CAT_DataOp get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns() { ops.ClearColumns(); }
  // #CAT_DataOp clear column pointers (don't keep these guys hanging around)

  virtual DataOpEl* AddColumn(const String& col_name, DataTable* dt)
  { return ops.AddColumn(col_name, dt); }
  // #CAT_DataOp #BUTTON add a new column to operate on from given data table
  virtual void	AddAllColumns(DataTable* dt, bool exclude_strings = false)
  { ops.AddAllColumns(dt, exclude_strings); }
  // #CAT_DataOp add all columns from given data table -- optionally excluding string-valued columns -- use this one in Programs (gui one selects new columns in browser)
  virtual void	AddAllColumns_gui(DataTable* dt, bool exclude_strings = false)
  { ops.AddAllColumns_gui(dt, exclude_strings); }
  // #CAT_DataOp #BUTTON #LABEL_AddAllColumns add all columns from given data table -- optionally excluding string-valued columns

  taList_impl*	children_() override {return &ops;}	
  Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const override
  { return ops.Elem(idx, mode); }
  String 	GetTypeDecoKey() const override { return "DataTable"; }
  TA_SIMPLE_BASEFUNS(DataOpBaseSpec);
protected:
  void	CheckChildConfig_impl(bool quiet, bool& rval) override;
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // DataOpBaseSpec_h
