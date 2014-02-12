// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ScaleRange.h"
#include <ColorScale>

TA_BASEFUNS_CTORS_DEFN(ScaleRange);

void ScaleRange::SetFromScale(ColorScale& cs) {
  auto_scale = cs.auto_scale;
  min = cs.min;
  max = cs.max;
}

void ScaleRange::SetFromScaleRange(ColorScale& cs) {
  cs.auto_scale = auto_scale;
  cs.SetMinMax(min, max);
}

void ScaleRange::UpdateAfterEdit_impl() {
  taOBase::UpdateAfterEdit_impl(); // skip over taNbase to avoid c_name thing!
}

