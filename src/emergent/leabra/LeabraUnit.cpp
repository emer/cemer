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

#include "LeabraUnit.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(VChanBasis);

TA_BASEFUNS_CTORS_DEFN(LeabraUnitChans);

TA_BASEFUNS_CTORS_DEFN(LeabraUnit);

void LeabraUnitChans::Initialize() {
  l = i = h = a = 0.0f;
}

void LeabraUnitChans::Copy_(const LeabraUnitChans& cp) {
  l = cp.l;
  i = cp.i;
  h = cp.h;
  a = cp.a;
}

void LeabraUnit::Initialize() {
  bias.con_type = &TA_LeabraCon;

  act_eq = 0.0f;
  act_lrn = 0.0f;
  act_nd = 0.0f;
  act_avg = 0.15f;
  avg_ss = 0.15f;
  avg_s = 0.15f;
  avg_m = 0.15f;
  avg_l = 0.15f;
  thal = 0.0f;
  deep5b = 0.0f;
  deep5b_net = 0.0f;
  act_ctxt = 0.0f;
  net_ctxt = 0.0f;
  p_act_p = 0.0f;
  davg = 0.0f;
  act_p = act_m = act_dif = 0.0f;
  act_mid = 0.0f;
  da = 0.0f;
  I_net = 0.0f;
  v_m = 0.0f;
  noise = 0.0f;
  dav = 0.0f;
  sev = 0.0f;

  in_subgp = false;
  net_scale = 0.0f;
  bias_scale = 0.0f;
  ctxt_scale = 0.0f;

  act_sent = 0.0f;
  net_raw = 0.0f;
  net_delta = 0.0f;
  g_i_raw = 0.0f;
  g_i_delta = 0.0f;
  g_i_syn = 0.0f;
  g_i_self = 0.0f;

  i_thr = 0.0f;
  spk_amp = 1.0f;
  misc_1 = 0.0f;
  spk_t = -1;

  act_buf = NULL;
  spike_e_buf = NULL;
  spike_i_buf = NULL;
}

void LeabraUnit::InitLinks() {
  inherited::InitLinks();
  taBase::Own(vcb, this);
  taBase::Own(gc, this);
  GetInSubGp();
}

void LeabraUnit::CutLinks() {
  if(spike_e_buf) {
    taBase::unRefDone(spike_e_buf);
    spike_e_buf = NULL;
  }
  if(spike_i_buf) {
    taBase::unRefDone(spike_i_buf);
    spike_i_buf = NULL;
  }
  if(act_buf) {
    taBase::unRefDone(act_buf);
    act_buf = NULL;
  }

  inherited::CutLinks();
}

void LeabraUnit::BuildUnits() {
  inherited::BuildUnits();
  LeabraUnitSpec* us = (LeabraUnitSpec*)GetUnitSpec();
  if(us) {
    us->Init_SpikeBuff(this);
    us->Init_ActBuff(this);
  }
}

void LeabraUnit::GetInSubGp() {
  Unit_Group* ownr = (Unit_Group*)owner;
  if((ownr != NULL) && (ownr->owner != NULL) && ownr->owner->InheritsFrom(TA_taSubGroup))
    in_subgp = true;
  else
    in_subgp = false;
}

void LeabraUnit::Copy_(const LeabraUnit& cp) {
  act_eq = cp.act_eq;
  act_lrn = cp.act_lrn;
  act_nd = cp.act_nd;
  act_avg = cp.act_avg;
  avg_s = cp.avg_s;
  avg_ss = cp.avg_ss;
  avg_m = cp.avg_m;
  avg_l = cp.avg_l;
  thal = cp.thal;
  deep5b = cp.deep5b;
  deep5b_net = cp.deep5b_net;
  act_ctxt = cp.act_ctxt;
  net_ctxt = cp.net_ctxt;
  p_act_p = cp.p_act_p;
  davg = cp.davg;
  act_m = cp.act_m;
  act_p = cp.act_p;
  act_dif = cp.act_dif;
  act_mid = cp.act_mid;
  da = cp.da;
  vcb = cp.vcb;
  gc = cp.gc;
  I_net = cp.I_net;
  v_m = cp.v_m;
  noise = cp.noise;
  dav = cp.dav;
  sev = cp.sev;
  // not: in_subgp
  net_scale = cp.net_scale;
  bias_scale = cp.bias_scale;
  ctxt_scale = cp.ctxt_scale;
  act_sent = cp.act_sent;
  net_raw = cp.net_raw;
  net_delta = cp.net_delta;
  g_i_raw = cp.g_i_raw;
  g_i_delta = cp.g_i_delta;
  g_i_syn = cp.g_i_syn;
  g_i_self = cp.g_i_self;
  i_thr = cp.i_thr;
  spk_amp = cp.spk_amp;
  misc_1 = cp.misc_1;
  spk_t = cp.spk_t;
}
