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

#include "MatrixChannelSpec.h"

TA_BASEFUNS_CTORS_DEFN(MatrixChannelSpec);


void MatrixChannelSpec::Initialize() {
  cell_geom.SetDims(1);
  uses_cell_names = false;
}

void MatrixChannelSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
  taBase::Own(cell_names, this);
}
 
void MatrixChannelSpec::CutLinks() {
  cell_names.CutLinks();
  cell_geom.CutLinks();
  inherited::CutLinks();
}

void MatrixChannelSpec::Copy_(const MatrixChannelSpec& cp) {
  cell_geom = cp.cell_geom;
  uses_cell_names = cp.uses_cell_names;
  cell_names = cp.cell_names;
}

void MatrixChannelSpec::UpdateAfterEdit() {
  InitCellNames();
  inherited::UpdateAfterEdit();
}

void MatrixChannelSpec::InitCellNames() {
  if (uses_cell_names) {
    cell_names.SetGeomN(cell_geom);
  } else { // no cell names
    cell_names.Reset();
  }
}

void MatrixChannelSpec::SetCellGeom(bool uses_names, int dims,
    int d0, int d1, int d2, int d3, int d4)
{
  uses_cell_names = uses_names;
  cell_geom.SetDims(dims);
  cell_geom.Set(0, d0);
  cell_geom.Set(1, d1);
  cell_geom.Set(2, d2);
  cell_geom.Set(3, d3);
  cell_geom.Set(4, d4);
  UpdateAfterEdit();
}

void MatrixChannelSpec::SetCellGeomN(bool uses_names, const MatrixGeom& geom) {
  uses_cell_names = uses_names;
  cell_geom = geom;
  UpdateAfterEdit();
}
