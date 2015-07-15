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

#include "taVector3f.h"

#include <taMatrix>
#include <taVector3i>
#include <MatrixIndex>

TA_BASEFUNS_CTORS_LITE_DEFN(taVector3f);

taVector3f::taVector3f(const taVector3i& cp) {
  Register(); Initialize();
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
}

taVector3f& taVector3f::operator=(const taVector3i& cp) {
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
  return *this;
}

void taVector3f::ToMatrix(taMatrix& mat) const {
  mat.SetGeom(1,3); mat.SetFmVar(x,0);  mat.SetFmVar(y,1); mat.SetFmVar(z,2);
}

void taVector3f::FromMatrix(taMatrix& mat) {
  x = mat.SafeElAsVar(0).toFloat();  y = mat.SafeElAsVar(1).toFloat();
  z = mat.SafeElAsVar(2).toFloat();
}
