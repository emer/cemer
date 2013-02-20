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

#include "LeabraTICtxtConSpec.h"

void LeabraTICtxtConSpec::Initialize() {
  ti_learn_pred = true;
  SetUnique("wt_scale", true);
  wt_scale.rel = 0.0f;          // very important for not interfering with anything else
  wt_scale.abs = 1.0f;
}

void LeabraTICtxtConSpec::Compute_NetinScale(LeabraRecvCons* recv_gp, LeabraLayer* from) {
  float savg = from->kwta.pct;
  float from_sz = (float)from->units.leaves;
  float n_cons = (float)recv_gp->size;
  recv_gp->scale_eff = wt_scale.abs * wt_scale.SLayActScale(savg, from_sz, n_cons);
}

