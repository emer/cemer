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

#include "XYNGeom.h"

TA_BASEFUNS_CTORS_LITE_DEFN(XYNGeom);

void XYNGeom::Initialize() {
  x = 1;
  y = 1;
  n_not_xy = false;
  n = 1;
  z = 0;
}

void XYNGeom::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(n_not_xy && n > x*y) { // only if not fitting, expand
    FitN(n);
  }
  if(n_not_xy) {
    if(x * y == n) n_not_xy = false; // no need for flag
  }
  else {
    n = x * y;			// always keep n up-to-date
  }
}

void XYNGeom::operator=(const taVector2i& cp) {
  x = cp.x; y = cp.y; UpdateAfterEdit_NoGui();
}

