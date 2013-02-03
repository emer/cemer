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

#include "LeabraTdUnitSpec.h"
#include <LeabraTdUnit>

void LeabraTdUnitSpec::Initialize() {
  min_obj_type = &TA_LeabraTdUnit;
  lambda = 0.0f;
}

void LeabraTdUnitSpec::Init_Acts(Unit* u, Network* net) {
  inherited::Init_Acts(u, net);
  LeabraTdUnit* lu = (LeabraTdUnit*)u;
  lu->p_act_m = -.01f;
  lu->p_act_p = -.01f;
}

void LeabraTdUnitSpec::Init_Weights(Unit* u, Network* net) {
  inherited::Init_Weights(u, net);
  ((LeabraTdUnit*)u)->trace = 0.0f;
}

void LeabraTdUnitSpec::EncodeState(LeabraUnit* u, LeabraNetwork* net) {
  inherited::EncodeState(u, net);
  LeabraTdUnit* lu = (LeabraTdUnit*)u;
  lu->p_act_p = lu->act_p;
  lu->p_act_m = lu->act_m;
  lu->trace = lambda * lu->trace + lu->p_act_p;
}
