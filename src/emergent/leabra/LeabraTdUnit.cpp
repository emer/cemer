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

#include "LeabraTdUnit.h"

void LeabraTdUnit::Initialize() {
  p_act_m = -.01f;
  p_act_p = -.01f;
  trace = 0.0f;
}

void LeabraTdUnit::Copy_(const LeabraTdUnit& cp) {
  p_act_p = cp.p_act_p;
  p_act_m = cp.p_act_m;
  trace = cp.trace;
}

