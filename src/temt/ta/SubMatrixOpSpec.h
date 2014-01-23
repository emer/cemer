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

#ifndef SubMatrixOpSpec_h
#define SubMatrixOpSpec_h 1

// parent includes:
#include <DataOpBaseSpec>

// member includes:
#include <DataTable>

// declare all other types mentioned but not required to include:


taTypeDef_Of(SubMatrixOpSpec);

class TA_API SubMatrixOpSpec : public DataOpBaseSpec {
  // sub matrix operations spec -- contains a list of columns that contain sub matricies, and their offsets within a larger matrix
INHERITED(DataOpBaseSpec)
public:
  DataTableRef		sub_matrix_table;
  // pointer to a sub matrix data table, for looking up the column names in the spec (different tables can be used for actual processing as long as they have the same structure)

  TA_SIMPLE_BASEFUNS(SubMatrixOpSpec);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;

private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // SubMatrixOpSpec_h
