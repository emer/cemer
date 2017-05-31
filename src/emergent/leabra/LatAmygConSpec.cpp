// Copyright 2016, Regents of the University of Colorado,
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

#include "LatAmygConSpec.h"

TA_BASEFUNS_CTORS_DEFN(LatAmygGains);
TA_BASEFUNS_CTORS_DEFN(LatAmygConSpec);

void LatAmygGains::Initialize() {
  Defaults_init();
  dar = D1D2R;                  // neutral..
}

void LatAmygGains::Defaults_init() {
  burst_da_gain = 1.0f;
  dip_da_gain= 1.0f;
  wt_decay_rate = 0.001f;
  wt_decay_floor = 0.5f;
  neg_lrate = 0.001f;
}

void LatAmygConSpec::Initialize() {
}

void LatAmygConSpec::Defaults_init() {
}
