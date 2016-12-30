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

#include "BLAmygUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(BLAmygDaMod);
TA_BASEFUNS_CTORS_DEFN(BLAmygUnitSpec);

void BLAmygDaMod::Initialize() {
  burst_da_gain = 1.0f;
  dip_da_gain = 1.0f;
  acq_deep_mod = true;
  us_clamp_avg = 0.2f;
  Defaults_init();
}

void BLAmygDaMod::Defaults_init() {
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
}

void BLAmygUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void BLAmygUnitSpec::Compute_DeepMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(deep.SendDeepMod()) {
    u->deep_lrn = u->deep_mod = u->act;      // record what we send!
  }
  else if(deep.IsTRC()) {
    u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
  }
  // must be SUPER units at this point
  else if(lay->am_deep_mod_net.max < 0.01f) { // not enough yet // was .1f
    u->deep_lrn = 0.0f;    // default is 0!
    u->deep_mod = 1.0f;
  }
  else {
    u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max; // todo: could not normalize this..
    u->deep_mod = 1.0f;                               // do not modulate with deep_mod!
  }
}

float BLAmygUnitSpec::Compute_DaModNetin(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  float da_val = u->da_p;
  if(da_val > .0f) {
    da_val *= bla_da_mod.burst_da_gain;
  }
  else {
    da_val *= bla_da_mod.dip_da_gain;
  }
  
  da_val *= u->act; // net_syn;
  if(dar == D2R)
    da_val = -da_val;           // flip the sign
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    return da_mod.plus * da_val;
  }
  else {                      // MINUS_PHASE
    return da_mod.minus * da_val;
  }
}

float BLAmygUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no, float& net_syn) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  float net_ex = init.netin;
  if(bias_spec) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UnitVars::EXT)) {
    if(ls->clamp.avg)
      net_syn = ls->clamp.ClampAvgNetin(u->ext, net_syn);
    else
      net_ex += u->ext * ls->clamp.gain;
  }
  // new part: use deep_raw_net if it exists!
  if(u->deep_raw_net > 0.1f) {
    net_syn = bla_da_mod.us_clamp_avg * u->deep_raw_net + (1.0f - bla_da_mod.us_clamp_avg) * net_syn;
  }
  
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  if(da_mod.on) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
  }
  return net_ex;
}

