// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



#include "cl.h"

void ClLayerSpec::Initialize() {
  netin_type = MAX_NETIN_WINS;
}

void ClLayerSpec::Compute_Act(SoLayer* lay) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act(lay);
    return;
  }
  if(lay->units.leaves == 0)
    return;
  
  SoUnit* win_u = FindWinner(lay);
  if(win_u == NULL) return;

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;

  float lvcnt = (float)lay->units.leaves;
  lay->avg_act = (uspec->act_range.max / lvcnt) +
    (((lvcnt - 1.0f) * uspec->act_range.min) / lvcnt);

  win_u->act = uspec->act_range.max;
  win_u->act_i = lay->avg_act;	// this is the rescaled value..
}

void SoftClUnitSpec::Initialize() {
  var = 1.0f;
  norm_const = 1.0f / sqrtf(2.0f * 3.14159265358979323846 * var);
  denom_const = 0.5f / var;
}

void SoftClUnitSpec::UpdateAfterEdit() {
  SoUnitSpec::UpdateAfterEdit();
  norm_const = 1.0f / sqrtf(2.0f * 3.14159265358979323846 * var);
  denom_const = 0.5f / var;
}

void SoftClUnitSpec::Compute_Netin(Unit* u) {
  // do distance instead of net input
  u->net = 0.0f;
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g)
    u->net += recv_gp->Compute_Dist(u);
}

void SoftClUnitSpec::Compute_Act(Unit* u) {
  SoUnit* su = (SoUnit*)u;
  if(u->ext_flag & Unit::EXT)
    su->act = su->act_i = u->ext;
  else
    su->act = su->act_i = norm_const * expf(-denom_const * su->net);
}

void SoftClLayerSpec::Initialize() {
  netin_type = MIN_NETIN_WINS;	// not that this is actually used..
}

void SoftClLayerSpec::Compute_Act(SoLayer* lay) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act(lay);
    return;
  }

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;

  float sum = 0.0f;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->Compute_Act();
    sum += u->act;
  }

  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->act = uspec->act_range.Project(u->act / sum);
    // normalize by sum, rescale to act range range
  }

  Compute_AvgAct(lay);
}
