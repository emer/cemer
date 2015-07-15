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

#include "DataColSpec.h"
#include <MatrixGeom>
#include <String_Matrix>

TA_BASEFUNS_CTORS_DEFN(DataColSpec);


void DataColSpec::Initialize() {
  col_num = -1; // for standalone, means "at end", otherwise, when in list, is set to index number
  val_type = VT_FLOAT; // most common type
  is_matrix = false;
}

void DataColSpec::SetCellGeom(int dims,
                              int d0, int d1, int d2, int d3, int d4)
{
  is_matrix = true;
  cell_geom.SetDims(dims);
  cell_geom.Set(0, d0);
  cell_geom.Set(1, d1);
  cell_geom.Set(2, d2);
  cell_geom.Set(3, d3);
  cell_geom.Set(4, d4);
}

void DataColSpec::SetCellGeomN(const MatrixGeom& geom) {
  is_matrix = true;
  cell_geom = geom;
}
