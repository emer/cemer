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

#include "BLAmygUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(BLAmygDaMod);
TA_BASEFUNS_CTORS_DEFN(BLAmygAChMod);
TA_BASEFUNS_CTORS_DEFN(BLAmygUnitSpec);

void BLAmygDaMod::Initialize() {
  Defaults_init();
}

void BLAmygDaMod::Defaults_init() {
  pct_act = 1.0f;
  burst_da_gain = 0.1f;
  dip_da_gain = 0.1f;
  us_clamp_avg = 0.2f;
}

void BLAmygAChMod::Initialize() {
  Defaults_init();
}

void BLAmygAChMod::Defaults_init() {
  on = true;
  mod_min = 0.8f;
  mod_min_c = 1.0f - mod_min;
}

void BLAmygAChMod::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  mod_min_c = 1.0f - mod_min;
}

void BLAmygUnitSpec::Initialize() {
  dar = D1R;
  Defaults_init();
}

void BLAmygUnitSpec::Defaults_init() {
  SetUnique("deep", true);
  deep.on = true;
  deep.role = DeepSpec::DEEP;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f;
  deep.mod_thr = 0.1f;          // note: was .01 in hard code
}

void BLAmygUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void BLAmygUnitSpec::Compute_DeepMod(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(deep.SendDeepMod()) {
    u->deep_lrn = u->deep_mod = u->act;      // record what we send!
  }
  else if(deep.IsTRC()) {
    u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
    if(trc.thal_gate) {
      u->net *= u->thal;
    }
  }
  // must be SUPER units at this point
  // else if(lay->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet
  else if(u->deep_mod_net <= deep.mod_thr) { // per-unit, NOT layer
    u->deep_lrn = 0.0f;    // default is 0!
    if(deep_mod_zero) {
      u->deep_mod = 0.0f; // makes activity dependent on deep_mod_net
    }
    else {
      u->deep_mod = 1.0f;
    }
  }
  else {
    u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max;
    u->deep_mod = 1.0f;
  }
  
  if(bla_ach_mod.on) {          // use deep mod to reflect ach modulation
    u->deep_mod = bla_ach_mod.mod_min + bla_ach_mod.mod_min_c * u->ach;
  }
}

float BLAmygUnitSpec::Compute_DaModNetin(LeabraUnitState_cpp* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  float da_val = u->da_p;
  if(da_val > 0.0f) {
    da_val *= bla_da_mod.burst_da_gain;
  }
  else {
    da_val *= bla_da_mod.dip_da_gain;
  }

  float mod_val = u->act_eq;
  if(bla_da_mod.lrn_mod_act) {
    mod_val = bla_da_mod.pct_act * u->act_eq + (1.0f - bla_da_mod.pct_act) * net_syn;
  }
    
  da_val *= mod_val;
  if(dar == D2R)
    da_val = -da_val;           // flip the sign
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    return da_mod.plus * da_val;
  }
  else {                      // MINUS_PHASE
    return da_mod.minus * da_val;
  }
}

float BLAmygUnitSpec::Compute_NetinExtras(LeabraUnitState_cpp* u, LeabraNetwork* net,
                                           int thr_no, float& net_syn) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  float net_ex = init.netin;
  if(bias_spec) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UnitState::EXT)) {
    if(ls->clamp.avg)
      net_syn = ls->clamp.ClampAvgNetin(u->ext, net_syn);
    else
      net_ex += u->ext * ls->clamp.gain;
  }
  
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  
  if(bla_da_mod.lrn_mod_act) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
    // similarly for US (PV) inputs (deep_raw_net), if present
    if(u->deep_raw_net > 0.1f) {
      net_syn = bla_da_mod.us_clamp_avg * u->deep_raw_net + (1.0f - bla_da_mod.us_clamp_avg) * net_syn;
    }
  }
  return net_ex;
}

void BLAmygUnitSpec::Compute_ActFun_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_ActFun_Rate(u, net, thr_no);
  
  // default is to use act_eq for later use by C_Compute__dWt_CEl_Delta() to effect learning *as if* phasic dopamine modulates activations - but without actually doing it!
  if(!bla_da_mod.lrn_mod_act) {
    float da_val = u->da_p;
    if(da_val > 0.0f) {
      da_val *= bla_da_mod.burst_da_gain;
    }
    else {
      da_val *= bla_da_mod.dip_da_gain;
    }
    if(dar == D2R) { da_val = -da_val; } // flip the sign
    u->act_eq *= (1.0f + da_val);
  }
}

void BLAmygUnitSpec::Quarter_Final_RecVals(LeabraUnitState_cpp* u, LeabraNetwork* net,
                                        int thr_no) {
  inherited::Quarter_Final_RecVals(u, net, thr_no);
  if(net->quarter == 3) {
    u->act_dif = u->act_p - u->act_q0; // prior trial -- this is learning delta
  }
}
