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

#include "CElAmygUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(CElAmygDaMod);
TA_BASEFUNS_CTORS_DEFN(CElAmygUnitSpec);

void CElAmygDaMod::Initialize() {
  burst_da_gain = 0.0f;
  dip_da_gain = 0.1f;
  acq_deep_mod = true;
  us_clamp_avg = 0.2f;
  Defaults_init();
}

void CElAmygDaMod::Defaults_init() {
}

void CElAmygUnitSpec::Initialize() {
  acq_ext = ACQ;
  valence = APPETITIVE;
  dar = D1R;
  Defaults_init();
}

void CElAmygUnitSpec::Defaults_init() {
  SetUnique("deep", true);
  deep.on = true;
  deep.role = DeepSpec::DEEP;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f;
  deep.mod_thr = 0.01f;         // default .1

  // these are params to produce low-level baseline tonic activation
  SetUnique("init", true);
  init.v_m = 0.55f;
  SetUnique("g_bar", true);
  g_bar.l = 0.6f;               // todo: maybe higher?
  SetUnique("e_rev", true);
  e_rev.l = 0.55f;
  e_rev.i = 0.4f;
}

void CElAmygUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(acq_ext == ACQ) {
    if(valence == APPETITIVE) {
      dar = D1R;
    }
    else {     // AVERSIVE
      dar = D2R;
    }
  }
  else {      // EXT
    if(valence == APPETITIVE) {
      dar = D2R;                // reversed!
    }
    else {     // AVERSIVE
      dar = D1R;
    }
  }
}

void CElAmygUnitSpec::Compute_DeepMod(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
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
  else if(u->deep_mod_net <= deep.mod_thr) { // not enough yet
    u->deep_lrn = 0.0f;    // default is 0!
    if(deep_mod_zero) {
      u->deep_mod = 0.0f;
    }
    else {
      u->deep_mod = 1.0f;
    }
  }
  else {
    u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max;
    u->deep_mod = 1.0f;                               // do not modulate with deep_mod!
  }
}

float CElAmygUnitSpec::Compute_DaModNetin(LeabraUnitState_cpp* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  float da_val = u->da_p;
  if(da_val > 0.0f) {
    da_val *= cel_da_mod.burst_da_gain;
  }
  else {
    da_val *= cel_da_mod.dip_da_gain;
  }
  
  float mod_val = net_syn;

  if(acq_ext == ACQ) {
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
  else {                        // extinction -- gets from Acq/Go inhib
    if(cel_da_mod.acq_deep_mod) {
      da_val *= fmaxf(u->deep_mod_net, mod_val);
    }
    else {
      da_val *= fmaxf(u->gi_syn, mod_val);
    }
    if(dar == D2R)
      da_val = -da_val;           // flip the sign
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      return da_mod.plus * da_val;
    }
    else {                      // MINUS_PHASE
      return da_mod.minus * da_val;
    }
  }
}

float CElAmygUnitSpec::Compute_NetinExtras(LeabraUnitState_cpp* u, LeabraNetwork* net,
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
  // if not using act_eq for learning, use deep_raw_net (i.e., US-input) to modulate net_syn
  //  if it exists!
  if(cel_da_mod.lrn_mod_act) {
    
  }
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  if(cel_da_mod.lrn_mod_act) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
    // also need to modulate net_syn for US (PV) inputs, if present
    if(u->deep_raw_net > 0.1f) {
      net_syn = cel_da_mod.us_clamp_avg * u->deep_raw_net + (1.0f - cel_da_mod.us_clamp_avg) * net_syn;
    }
  }
  return net_ex;
}

void CElAmygUnitSpec::Compute_ActFun_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_ActFun_Rate(u, net, thr_no);
  // default is to use act_eq for later use by C_Compute__dWt_CEl_Delta() to effect learning *as if* phasic dopamine modulates activations - but without actually doing it!
  if(!cel_da_mod.lrn_mod_act) {
    float da_val = u->da_p;
    if(da_val > 0.0f) {
      da_val *= cel_da_mod.burst_da_gain;
    }
    else {
      da_val *= cel_da_mod.dip_da_gain;
    }
    if(dar == D2R) { da_val = -da_val; } // flip the sign
    u->act_eq *= (1.0f + da_val); // co-opt act_eq variable for wt changes
    // similarly, clamp act_eq to US (PV) inputs (deep_raw_net), if present
    if(u->deep_raw_net > 0.01f) {
      u->act_eq = u->deep_raw_net;
    }
  }
  // ELSE: actually *DOES* modulate actual activations - CAUTION - very brittle!
}

void CElAmygUnitSpec::Quarter_Final_RecVals(LeabraUnitState_cpp* u, LeabraNetwork* net,
                                            int thr_no) {
  inherited::Quarter_Final_RecVals(u, net, thr_no);
  if(net->quarter == 3) {
    u->act_dif = u->act_p - u->act_q0; // prior trial -- this is learning delta
  }
}
