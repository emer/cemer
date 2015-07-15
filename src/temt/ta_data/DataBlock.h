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

#ifndef DataBlock_h
#define DataBlock_h 1

// this is useful for seeing who is including this and why..
// #warning "datatable included"

#include <DataTable>

// todo: at some point these can be removed (obsolete starting with 7.0.0, 5/2014)

taTypeDef_Of(DataBlock);

class DataBlock : public DataTable {
  // #OBSOLETE obsolete -- do not use -- this was previously a base class of DataTable
  INHERITED(DataTable)
public:

  TA_BASEFUNS_NOCOPY(DataBlock);
private:
  void Initialize() { };
  void Destroy() { };
};

taTypeDef_Of(DataBlock_Idx);

class DataBlock_Idx : public DataBlock {
  // #OBSOLETE obsolete -- do not use -- this was previously a base class of DataTable
  INHERITED(DataBlock)
public:

  TA_BASEFUNS_NOCOPY(DataBlock_Idx);
private:
  void Initialize() { };
  void Destroy() { };
};

#endif //DataBlock_h
