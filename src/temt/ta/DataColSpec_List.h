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

#ifndef DataColSpec_List_h
#define DataColSpec_List_h 1

// parent includes:
#include <DataColSpec>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; // 


taTypeDef_Of(DataColSpec_List);

class TA_API DataColSpec_List: public taList<DataColSpec> {
  // ##CAT_Data a list of channel specifications
INHERITED(taList<DataColSpec>)
public:

  virtual bool UpdateDataTableCols(DataTable* dt, bool remove_orphans = true);
  // update data table columns to match the specs in this list, exactly, including the ordering of the columns -- if remove_orphans is true, then any existing columns that don't match are removed (otherwise they are just left at the end)

  int          NumListCols() const override {return 5;}
  const        KeyString GetListColKey(int col) const override;
  String       GetColHeading(const KeyString& key) const override;
  String       GetColHeadingIdx(int col) const;

  TA_BASEFUNS_NOCOPY(DataColSpec_List);
private:
  void          Initialize() {SetBaseType(&TA_DataColSpec);}
  void          Destroy() {}
};

#endif // DataColSpec_List_h
