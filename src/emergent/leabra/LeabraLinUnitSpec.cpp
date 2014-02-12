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

#include "LeabraLinUnitSpec.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(LeabraLinUnitSpec);


void LeabraLinUnitSpec::Initialize() {
  SetUnique("act_fun", true);
  SetUnique("act_range", true);
  SetUnique("clamp_range", true);
  SetUnique("act", true);
  act_fun = LINEAR;
  act_range.max = 20;
  act_range.min = 0;
  act_range.UpdateAfterEdit_NoGui();
  clamp_range.max = 1.0f;
  clamp_range.UpdateAfterEdit_NoGui();
  act.gain = 2;
}

void LeabraLinUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net) {
  float new_act = u->net * act.gain; // use linear netin as act

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += Compute_Noise(u, net);
  }
  u->act = u->act_nd = u->act_eq = act_range.Clip(new_act);
  // note: keeping act_lrn based on sigmoidal function.. 
  Compute_ActLrnFmVm_rate(u, net);
}
