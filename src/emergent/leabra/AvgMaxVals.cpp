// Copyright 2017, Regents of the University of Colorado,
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

#include "AvgMaxVals.h"

TA_BASEFUNS_CTORS_DEFN(AvgMaxVals);

void AvgMaxVals::Initialize() {
  cmpt = true;
  avg = sum = max = 0.0f; n = 0; max_i = -1;
}

void AvgMaxVals::Copy_(const AvgMaxVals& cp) {
  cmpt = cp.cmpt;
  avg = cp.avg;
  max = cp.max;
  max_i = cp.max_i;
  sum = cp.sum;
  n = cp.n;
}

