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
#include <DataTableRef>
#include <DataTableCols>

// declare all other types mentioned but not required to include:
class DataCol; // 


taTypeDef_Of(DataOpEl);

class TA_API DataOpEl : public taOBase {
  // #STEM_BASE ##NO_TOKENS ##INLINE ##CAT_Data base class for data operations spec element
  INHERITED(taOBase)
public:
  DataTableRef		data_table;
  // #READ_ONLY #HIDDEN #NO_SAVE data table -- gets set dynamically
  DataCol*		col_lookup;
  // #NULL_OK #NO_SAVE #FROM_LIST_data_table.data #NO_EDIT #NO_UPDATE_POINTER lookup column in data table to operate on -- sets col_name field (which is what is actually used) and returns to NULL after selection is applied
  String		col_name;	// name of column in data table to operate on (either enter directly or lookup from col_lookup)
  int			col_idx;	// #READ_ONLY #NO_SAVE column idx (from GetColumns)

  virtual void 	SetDataTable(DataTable* dt);
  // #CAT_DataOp set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt);
  // #CAT_DataOp get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns();
  // #CAT_DataOp clear column pointers (don't keep these guys hanging around)

  virtual void  SetColName(const String& nm);

  bool          HasName() const override { return true; }
  String        GetName() const override;
  bool          SetName(const String& nm) override;
  void          MakeNameUnique() override;
  virtual void  UpdateName() { };
  // #IGNORE define this for any subclasses that do more with name than just col_name, called in SetColName and UAE
  virtual String GetListIdxSuffix();
  // get owner index based suffix to append to name -- make them always unique in case of overlap
  String        GetDisplayName() const override;
  String        GetTypeDecoKey() const override { return "ProgArg"; }

  bool          BrowserEditEnable() const override { return true; }
  bool          BrowserEditSet(const String& new_val_str, int move_after = 0) override;

  TA_SIMPLE_BASEFUNS(DataOpEl);
protected:
  void	UpdateAfterEdit_impl() override;	// set col_name from col_lookup
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void 	Destroy()		{ };
};

#endif // DataOpEl_h
