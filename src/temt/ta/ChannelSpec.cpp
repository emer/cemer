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

#include "ChannelSpec.h"


void ChannelSpec::Initialize() {
  chan_num = -1; // for standalone, means "at end", otherwise, when in list, is set to index number
  val_type = VT_FLOAT; // most common type
}

void ChannelSpec::Copy_(const ChannelSpec& cp) {
  chan_num = cp.chan_num;
  val_type = cp.val_type; 
}

const MatrixGeom& ChannelSpec::cellGeom() const {
  static MatrixGeom no_geom;
  return no_geom;
}

const String_Matrix& ChannelSpec::cellNames() const {
  static String_Matrix no_names;
  return no_names;
}

/*obs??? String ChannelSpec::GetColText(int col, int) {
  switch (col) {
  case 0: return chan_num;
  case 1: return name;
  case 2: return ValTypeToStr(val_type);
  case 3: return isMatrix();
  case 4: return isMatrix() ? cellGeom().GeomToString() : _nilString;
  case 5: return isMatrix() ? String(usesCellNames()) : _nilString;
  default: return _nilString; // compiler food
  }
}*/
