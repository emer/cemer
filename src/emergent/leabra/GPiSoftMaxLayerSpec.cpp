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

#include "GPiSoftMaxLayerSpec.h"

#include <LeabraNetwork>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(GPiSoftMaxSpec);
TA_BASEFUNS_CTORS_DEFN(GPiSoftMaxLayerSpec);

void GPiSoftMaxSpec::Initialize() {
  temp = 1.0f;
  rnd_p = 0.02f;
  Defaults_init();
}

void GPiSoftMaxSpec::Defaults_init() {
}

void GPiSoftMaxLayerSpec::Initialize() {
  
}

void GPiSoftMaxLayerSpec::Compute_SoftMax(LeabraLayer* lay, LeabraNetwork* net) {
  float sum = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    float eval = taMath_float::exp(uv->net / soft_max.temp);
    uv->misc_1 = eval;
    sum += eval;
  }

  if(sum > 0.0f) {
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      if(u->lesioned()) continue;
      LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
      uv->misc_1 /= sum;
    }
  }

  int chosen_i = 0;
  if(sum == 0.0f || Random::BoolProb(soft_max.rnd_p)) {
    chosen_i = Random::IntZeroN(lay->units.leaves, taMisc::dmem_proc);
  }
  else {
    float rndval = Random::ZeroOne(taMisc::dmem_proc);
    sum = 0.0f;
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      if(u->lesioned()) {
        chosen_i++;
        continue;
      }
      LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
      sum += uv->misc_1;
      if(sum >= rndval)
        break;
      chosen_i++;
    }
  }

  int leaf = 0;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) {
      leaf++;
      continue;
    }
    LeabraUnitVars* uv = (LeabraUnitVars*)u->GetUnitVars();
    float act = 0.0f;
    if(leaf == chosen_i) {
      act = 1.0f;
    }
    uv->act = uv->act_eq = act;
    leaf++;
  }
}

void GPiSoftMaxLayerSpec::Compute_CycleStats_Pre(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats_Pre(lay, net);
  Compute_SoftMax(lay, net);
}

