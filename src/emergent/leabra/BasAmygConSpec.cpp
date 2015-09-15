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

#include "BasAmygConSpec.h"

TA_BASEFUNS_CTORS_DEFN(BasAmygConSpec);

void BasAmygConSpec::Initialize() {
  ba_type = ACQ;
  dip_da_gain = 1.0f;
  invert_da = false;
}

void BasAmygConSpec::Defaults_init() {
}
