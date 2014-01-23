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

#ifndef DataOpList_h
#define DataOpList_h 1

// parent includes:
#include <DataOpEl>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; // 


taTypeDef_Of(DataOpList);

class TA_API DataOpList : public taList<DataOpEl> {
  // ##CAT_Data a list of data table operations
INHERITED(taList<DataOpEl>)
public:

  virtual void 	SetDataTable(DataTable* dt);
  // #CAT_DataOp set the data table to enable looking up columns

  virtual void 	GetColumns(DataTable* dt);
  // #CAT_DataOp get the column pointers for given data table (looking up by name)
  virtual void 	ClearColumns();
  // #CAT_DataOp clear column pointers (don't keep these guys hanging around)

  virtual DataOpEl* AddColumn(const String& col_name, DataTable* dt);
  // #CAT_DataOp #BUTTON add a new column to operate on from given data table
  virtual void	AddAllColumns(DataTable* dt);
  // #CAT_DataOp add all columns from given data table
  virtual void	AddAllColumns_gui(DataTable* dt);
  // #CAT_DataOp #BUTTON #LABEL_AddAllColumns add all columns from given data table

  void	SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);

  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "ProgArg"; }

  TA_BASEFUNS_NOCOPY(DataOpList);
private:
  void	Initialize() 		{ SetBaseType(&TA_DataOpEl); }
  void 	Destroy()		{ };
};

#endif // DataOpList_h
