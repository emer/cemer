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

#ifndef DataOpEl_h
#define DataOpEl_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <DataTable>
#include <DataTableCols>

// declare all other types mentioned but not required to include:
class DataCol; // 
class DataTable; // 


TypeDef_Of(DataOpEl);

class TA_API DataOpEl : public taOBase {
  // #STEM_BASE ##NO_TOKENS ##NO_UPDATE_AFTER ##INLINE ##CAT_Data base class for data operations spec element
  INHERITED(taOBase)
public:
  DataTableRef		data_table;
  // #READ_ONLY #HIDDEN #NO_SAVE data table -- gets set dynamically
  DataTableColsRef	data_cols;
  // #READ_ONLY #HIDDEN #NO_SAVE data table columns -- gets set dynamically -- just to lookup column
  DataCol*		col_lookup;
  // #NULL_OK #NO_SAVE #FROM_GROUP_data_cols #NO_EDIT #NO_UPDATE_POINTER lookup column in data table to operate on -- sets col_name field (which is what is actually used) and returns to NULL after selection is applied
  String		col_name;	// name of column in data table to operate on (either enter directly or lookup from col_lookup)
  int			col_idx;	// #READ_ONLY #NO_SAVE column idx (from GetColumns)

  virtual void 	SetDataTable(DataTable* dt);
  // #CAT_DataOp set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt);
  // #CAT_DataOp get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns();
  // #CAT_DataOp clear column pointers (don't keep these guys hanging around)

  override String GetName() const;
  override String GetDisplayName() const;
  override String GetTypeDecoKey() const { return "ProgArg"; }
  TA_SIMPLE_BASEFUNS(DataOpEl);
protected:
  override void	UpdateAfterEdit_impl();	// set col_name from col_lookup
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void 	Destroy()		{ };
};

#endif // DataOpEl_h
