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

#ifndef VEHeightField_h
#define VEHeightField_h 1

// parent includes:
#include <VEStatic>

// member includes:
#include <taVector3f>
#include <DataTable>

// declare all other types mentioned but not required to include:


taTypeDef_Of(VEHeightField);

class TA_API VEHeightField : public VEStatic {
  // virtual environment height field -- 3d surface defined by a grid of height values
INHERITED(VEStatic)
public:

#ifdef __MAKETA__
  Shape         shape;          // #READ_ONLY #HIDDEN shape is always height field
  taVector3f    box;            // #READ_ONLY #HIDDEN not relevant
#endif

  // todo: lookup column in data table..

  DataTableRef  height_data;    // data table containing height field data
  String        data_col;       // column name within table that has the data -- IMPORTANT: must be a 2d float/double matrix column!
  int           row_num;        // row number containing height field data

  override void Init();

  TA_SIMPLE_BASEFUNS(VEHeightField);
private:
  void  Initialize();
  void  Destroy() { };
};

#endif // VEHeightField_h
