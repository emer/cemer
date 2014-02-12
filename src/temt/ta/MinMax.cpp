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

#include "MinMax.h"
#include <float_Matrix>

TA_BASEFUNS_CTORS_LITE_DEFN(MinMax);

void MinMax::SetRange(float_Matrix& mat) {
  if (mat.size == 0) {
    Init(0.0f);
  } else {
    Init(mat.el[0]);
    for (int i = 1; i < mat.size; ++i)
      UpdateRange(mat.el[i]);
  }
} 

void MinMax::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(max < min) max = min + 1.0f; // fix so max is *always* > min
}

