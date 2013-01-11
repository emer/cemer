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

#include "taVector3i.h"
#include <taMatrix>
#include <taVector3f>
#include <MatrixIndex>

taVector3i::taVector3i(const taVector3f& cp) {
  Register(); Initialize();
  x = (int)cp.x; y = (int)cp.y; z = (int)cp.z;
}

taVector3i& taVector3i::operator=(const taVector3f& cp) {
  x = (int)cp.x; y = (int)cp.y;	z = (int)cp.z;
  return *this;
}

void taVector3i::CopyToMatrixGeom(MatrixGeom& geom) {
  geom.SetDims(3);
  geom.Set(0, x);
  geom.Set(1, y);
  geom.Set(2, z);
}

void taVector3i::CopyToMatrixIndex(MatrixIndex& idx) {
  idx.SetDims(3);
  idx.Set(0, x);
  idx.Set(1, y);
  idx.Set(2, z);
}

void taVector3i::ToMatrix(taMatrix& mat) const {
  mat.SetGeom(1,3); mat.SetFmVar(x,0);  mat.SetFmVar(y,1); mat.SetFmVar(z,2);
}

void taVector3i::FromMatrix(taMatrix& mat) {
  x = mat.SafeElAsVar(0).toInt();  y = mat.SafeElAsVar(1).toInt();
  z = mat.SafeElAsVar(2).toInt();
}

bool taVector3i::FitNinXY(int n) {
  return FitN(n);
}

