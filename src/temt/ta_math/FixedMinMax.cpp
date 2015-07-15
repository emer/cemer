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

#include "FixedMinMax.h"
#include <MinMax>

TA_BASEFUNS_CTORS_LITE_DEFN(FixedMinMax);

void FixedMinMax::Initialize() {
  min = max = 0.0f;
  fix_min = fix_max = false;
}

void FixedMinMax::Copy_(const FixedMinMax& cp) {
  min = cp.min; max = cp.max;
  fix_min = cp.fix_min; fix_max = cp.fix_max;
}

void FixedMinMax::FixRange(MinMax& mm) {
  if(fix_min) mm.min = min; if(fix_max) mm.max = max;
}
