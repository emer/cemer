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

#include "slice_Matrix.h"

int_Matrix* slice_Matrix::Expand(int implicit_end) {
  if(TestError(size < 3, "Expand", "slice matrix does not contain at least 3 elements, as it must"))
    return NULL;
  // todo: could try to handle multi-dimensional case, but it is tricky due to
  // uneven sizes of internal dimensions..
  int_Matrix* rval = new int_Matrix;
  int start = FastEl_Flat(0);
  int end = FastEl_Flat(1);
  int step = FastEl_Flat(2);
  if(step == 0) step = 1;
  int sz = implicit_end;                // default size
  if(end > 0)                   // if end set, then it is size
    sz = end;
  if(!FixSliceValsFromSize(start, end, sz)) {
    return NULL;
  }
  int n_vals = (end-start) / ABS(step);
  rval->SetGeom(1, n_vals);
  if(step > 0) {
    int cnt=0;
    for(int i = start; i < end; i += step, cnt++) {
      rval->FastEl_Flat(cnt) = i;
    }
  }
  else {
    int cnt=0;
    for(int i = end-1; i >= start; i += step, cnt++) {
      rval->FastEl_Flat(cnt) = i;
    }
  }
  return rval;
}

