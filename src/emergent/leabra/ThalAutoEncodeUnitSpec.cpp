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

#include "ThalAutoEncodeUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(AutoEncodeSpecs);
TA_BASEFUNS_CTORS_DEFN(ThalAutoEncodeUnitSpec);

void AutoEncodeSpecs::Initialize() {
  binarize = false;
  thr = 0.5f;
}

void AutoEncodeSpecs::Defaults_init() {
  
}

void ThalAutoEncodeUnitSpec::Initialize() {
  Defaults_init();
}

void ThalAutoEncodeUnitSpec::Defaults_init() {
  SetUnique("act_misc", true);
  act_misc.rec_nd = false;
  act_misc.avg_nd = false;
  SetUnique("deep", true);
  deep.on = true;
  SetUnique("deep_qtr", true);
  deep_qtr = Q4;
  SetUnique("deep_norm", true);
  deep_norm.on = true;
  deep_norm.raw_val = DeepNormSpec::NORM_NET;
}

float ThalAutoEncodeUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                                  int thr_no, float& net_syn) {
  if(Quarter_DeepNow(net->quarter)) {
    net_syn = u->deep_raw_net;          // only gets from deep!
  }
  return inherited::Compute_NetinExtras(u, net, thr_no, net_syn);
}

void ThalAutoEncodeUnitSpec::Compute_ActFun_Rate(LeabraUnitVars* u, LeabraNetwork* net,
                                                 int thr_no) {
  inherited::Compute_ActFun_Rate(u, net, thr_no);
  if(Quarter_DeepNow(net->quarter)) {
    if(auto_enc.binarize) {
      if(u->act_eq >= auto_enc.thr) {
        u->act_eq = u->act = clamp_range.max;
      }
      else {
        u->act_eq = u->act = clamp_range.min;
      }
    }
  }
}

void ThalAutoEncodeUnitSpec::Trial_Init_SRAvg(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  inherited::Trial_Init_SRAvg(u, net, thr_no);
  u->avg_l_lrn = 0.0f;        // no self organizing in clamped layers!
}

void ThalAutoEncodeUnitSpec::Compute_DeepNorm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!Compute_DeepTest(u, net, thr_no))
    return;
  if(deep_norm.raw_val == DeepNormSpec::THAL) {
    if(net->quarter >= 1)
      u->deep_norm = u->deep_raw_norm;
    else
      u->deep_norm = 0.0f;      // restart at start..
  }
  else {
    LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
    if(lay->am_deep_raw_norm.max > 0.0f)
      u->deep_norm = u->deep_raw_norm;
    else
      u->deep_norm = 1.0f;
  }
}

