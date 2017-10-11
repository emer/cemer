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

#include "cl.h"

TA_BASEFUNS_CTORS_DEFN(ClConSpec);
TA_BASEFUNS_CTORS_DEFN(SoftClConSpec);
TA_BASEFUNS_CTORS_DEFN(ClLayerSpec);
TA_BASEFUNS_CTORS_DEFN(SoftClLayerSpec);
TA_BASEFUNS_CTORS_DEFN(SoftClUnitSpec);

void ClLayerSpec::Initialize() {
  netin_type = MAX_NETIN_WINS;
}

void ClLayerSpec::Compute_Act_post(SoLayer* lay, SoNetwork* net) {
  if(lay->ext_flag & UnitState::EXT) { // input layer
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

  win_u->act() = uspec->act_range.max;
  win_u->act_i() = lay->avg_act;  // this is the rescaled value..
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

void SoftClUnitSpec::Compute_Netin(UnitState* u, Network* net, int thr_no) {
  // do distance instead of net input
  if (u->ext_flag & UnitState::EXT) {
    u->net = u->ext;
  }
  else {
    // do distance instead of net input
    u->net = 0.0f;
    const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
    for(int g=0; g<nrcg; g++) {
      SoConState* rgp = (SoConState*)net->ThrUnRecvConState(thr_no, u->thr_un_idx, g);
      if(rgp->NotActive()) continue;
      u->net += rgp->con_spec->Compute_Dist(rgp, net, thr_no);
    }
  }
}

void SoftClUnitSpec::Compute_Act(UnitState* u, Network* net, int thr_no) {
  SoUnitState* su = (SoUnitState*)u;
  if(su->ext_flag & UnitState::EXT) {
    su->act = su->act_i = u->ext;
  }
  else {
    su->act = su->act_i = norm_const * expf(-denom_const * su->net);
  }
}

void SoftClLayerSpec::Initialize() {
  netin_type = MIN_NETIN_WINS;  // not that this is actually used..
}

void SoftClLayerSpec::Compute_Act_post(SoLayer* lay, SoNetwork* net) {
  if(lay->ext_flag & UnitState::EXT) { // input layer
    SoLayerSpec::Compute_Act_post(lay, net);
    return;
  }

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();

  float sum = 0.0f;
  FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
    // act has already been computed..
    sum += u->act();
  }

  if(sum > 0.0f) {
    float norm = 1.0f / sum;
    FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
      u->act() = uspec->act_range.Project(u->act() * norm);
      // normalize by sum, rescale to act range range
    }
  }

  Compute_AvgAct(lay, net);
}
