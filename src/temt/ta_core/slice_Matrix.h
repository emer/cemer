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

#ifndef slice_Matrix_h
#define slice_Matrix_h 1

// parent includes:
#include <int_Matrix>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(slice_Matrix);

class TA_API slice_Matrix: public int_Matrix {
  // #INSTANCE an int matrix that is used to hold slicing information for indexing -- type is just a marker to unambiguously indicate this form of indexing
INHERITED(int_Matrix)
public:

  virtual int_Matrix*   Expand(int implicit_end = 100);
  // #CAT_Slice expand the slice range spec to a matrix of all the individual indexes implied in the range -- defaults for unspecified start, end, step are 0:implicit_end:1

  TA_MATRIX_FUNS_DERIVED(slice_Matrix, int);
private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // slice_Matrix_h
