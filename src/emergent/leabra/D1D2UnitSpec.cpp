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

#include "D1D2UnitSpec.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(D1D2UnitSpec);

void D1D2UnitSpec::Initialize() {
  Defaults_init();
}

void D1D2UnitSpec::Defaults_init() {
  deep.mod_thr = 0.1f;         // default is .1
}

