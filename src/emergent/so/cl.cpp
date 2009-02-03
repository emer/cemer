// Copyright, 1995-2007, Regents of the University of Colorado,
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



#include "cl.h"

void ClLayerSpec::Initialize() {
  netin_type = MAX_NETIN_WINS;
}

void ClLayerSpec::Compute_Act_post(SoLayer* lay, SoNetwork* net) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act_post(lay, net);
    return;
  }
  if(lay->units.leaves == 0)
    return;
  
  SoUnit* win_u = FindWinner(lay);
  if(win_u == NULL) return;

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();

  float lvcnt = (float)lay->units.leaves;
  lay->avg_act = (uspec->act_range.max / lvcnt) +
    (((lvcnt - 1.0f) * uspec->act_range.min) / lvcnt);

  win_u->act = uspec->act_range.max;
  win_u->act_i = lay->avg_act;	// this is the rescaled value..
}

void SoftClUnitSpec::Initialize() {
  var = 1.0f;
  norm_const = 1.0f / sqrtf(2.0f * 3.1415926f * var);
  denom_const = 0.5f / var;
}

void SoftClUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  norm_const = 1.0f / sqrtf(2.0f * 3.1415926f * var);
  denom_const = 0.5f / var;
}

void SoftClUnitSpec::Compute_Netin(Unit* u, Network* net, int thread_no) {
  // do distance instead of net input
  if (u->ext_flag & Unit::EXT)
    u->net = u->ext;
  else {
    // do distance instead of net input
    u->net = 0.0f;
    for(int g=0; g<u->recv.size; g++) {
      SoRecvCons* recv_gp = (SoRecvCons*)u->recv.FastEl(g);
      if(!recv_gp->prjn->from->lesioned())
	u->net += recv_gp->Compute_Dist(u);
    }
  }
}

void SoftClUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  SoUnit* su = (SoUnit*)u;
  if(u->ext_flag & Unit::EXT)
    su->act = su->act_i = u->ext;
  else
    su->act = su->act_i = norm_const * expf(-denom_const * su->net);
}

void SoftClLayerSpec::Initialize() {
  netin_type = MIN_NETIN_WINS;	// not that this is actually used..
}

void SoftClLayerSpec::Compute_Act_post(SoLayer* lay, SoNetwork* net) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act_post(lay, net);
    return;
  }

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();

  float sum = 0.0f;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i) {
    // act has already been computed..
    sum += u->act;
  }

  if(sum > 0.0f) {
    FOR_ITR_EL(Unit, u, lay->units., i) {
      u->act = uspec->act_range.Project(u->act / sum);
      // normalize by sum, rescale to act range range
    }
  }

  Compute_AvgAct(lay, net);
}
