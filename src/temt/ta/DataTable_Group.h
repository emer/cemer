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

#ifndef DataTable_Group_h
#define DataTable_Group_h 1

// parent includes:
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; //

taTypeDef_Of(DataTable);

taTypeDef_Of(DataTable_Group);

class TA_API DataTable_Group : public taGroup<DataTable> {
  // ##CAT_Data ##EXPAND_DEF_1 group of data objects
INHERITED(taGroup<DataTable>)
public:
  bool          save_tables;    // #DEF_true save the data tables in this group -- turn this off for groups of data tables that are all temporary and should not be saved -- saves space for headers etc in dump file

  String       GetTypeDecoKey() const override { return "DataTable"; }

  bool         Dump_QuerySaveChildren() override { return save_tables; }

  TA_BASEFUNS(DataTable_Group);
private:
  SIMPLE_COPY(DataTable_Group);
  void  Initialize()            { SetBaseType(&TA_DataTable); save_tables = true; }
  void  Destroy()               { };
};

TA_SMART_PTRS(DataTable_Group);

#endif // DataTable_Group_h
