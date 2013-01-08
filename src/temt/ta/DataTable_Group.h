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
#include <DataTable>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API DataTable_Group : public taGroup<DataTable> {
  // ##CAT_Data ##EXPAND_DEF_1 group of data objects
INHERITED(taGroup<DataTable>)
public:
  override String       GetTypeDecoKey() const { return "DataTable"; }

  TA_BASEFUNS(DataTable_Group);
private:
  NOCOPY(DataTable_Group)
  void  Initialize()            { SetBaseType(&TA_DataTable); }
  void  Destroy()               { };
};

#endif // DataTable_Group_h
