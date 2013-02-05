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

#include "LeabraBiasSpec.h"
#include <LeabraNetwork>

void LeabraBiasSpec::Initialize() {
  //   min_obj_type = &TA_RecvCons; // don't bother..
  SetUnique("rnd", true);
  SetUnique("wt_limits", true);
  SetUnique("wt_scale", true);
  SetUnique("wt_scale_init", true);

  Defaults_init();
}

void LeabraBiasSpec::Defaults_init() {
  rnd.mean = 0.0f;
  rnd.var = 0.0f;
  wt_limits.min = -1.0f;
  wt_limits.max = 5.0f;
  wt_limits.sym = false;
  wt_limits.type = WeightLimits::NONE;
  dwt_thresh = .1f;
}

bool LeabraBiasSpec::CheckObjectType_impl(taBase* obj) {
  // don't allow anything to point to a biasspec except the unitspec!
  if(!obj->InheritsFrom(TA_BaseSpec) &&
     !obj->InheritsFrom(TA_LeabraCon)) return false;
  return true;
}

// todo: note that this is very wasteful replication of code for just one last line..

void LeabraBiasSpec::B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  if(fabsf(err) >= dwt_thresh)
    cn->dwt += cur_lrate * err;
}

void LeabraBiasSpec::B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
                                                LeabraLayer* rlay) {
  // cal only for bias weights: only err is useful contributor to this learning
  float dw = ru->avg_s - ru->avg_m;
  if(fabsf(dw) >= dwt_thresh)
    cn->dwt += cur_lrate * dw;
}

void LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
                                               LeabraLayer* rlay) {
  float dw = ru->avg_s - ru->avg_m;
  if(fabsf(dw) >= dwt_thresh)
    cn->dwt += cur_lrate * dw;
}
