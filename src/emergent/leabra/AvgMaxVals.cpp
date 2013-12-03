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

#include "AvgMaxVals.h"

void AvgMaxVals::Initialize() {
  cmpt = true;
  avg = max = 0.0f; max_i = -1;
}

void AvgMaxVals::Copy_(const AvgMaxVals& cp) {
  cmpt = cp.cmpt;
  avg = cp.avg; max = cp.max; max_i = cp.max_i;
}

void AvgMaxVals::UpdtTimeAvg(const AvgMaxVals& cp, float dt) {
  if(max_i < 0) {
    Copy_(cp);
    max_i = 1;                  // first data point
  }
  else {
    avg += dt * (cp.avg - avg);
    max += dt * (cp.max - max);
    max_i++;                    // keep track of number of updates
  }
}
