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

TA_BASEFUNS_CTORS_DEFN(LeabraUnit);

void LeabraUnit::Initialize() {
  bias.con_type = &TA_LeabraCon;

  act_eq = 0.0f;
  act_nd = 0.0f;
  act_lrn = 0.0f;
  spike = 0.0f;
  act_p = act_m = act_dif = 0.0f;
  net_prv_phs = 0.0f;
  net_prv_trl = 0.0f;
  act_mid = 0.0f;
  da = 0.0f;
  avg_ss = 0.15f;
  avg_s = 0.15f;
  avg_m = 0.15f;
  avg_l = 0.15f;
  act_avg = 0.15f;
  thal = 0.0f;
  deep5b = 0.0f;
  lrnmod = 0.0f;
  deep5b_net = 0.0f;
  act_ctxt = 0.0f;
  net_ctxt = 0.0f;
  p_act_p = 0.0f;
  gc_i = 0.0f;
  gc_l = 0.0f;
  I_net = 0.0f;
  v_m = 0.0f;
  v_m_eq = 0.0f;
  adapt = 0.0f;
  gi_syn = 0.0f;
  gi_self = 0.0f;
  gi_ex = 0.0f;
  E_i = 0.25f;
  syn_tr = 1.0f;
  syn_nr = 1.0f;
  syn_pr = 0.2f;
  syn_kre = 0.0f;
  noise = 0.0f;
  dav = 0.0f;
  sev = 0.0f;

  net_scale = 0.0f;
  bias_scale = 0.0f;
  ctxt_scale = 0.0f;

  act_sent = 0.0f;
  net_raw = 0.0f;
  net_delta = 0.0f;
  gi_raw = 0.0f;
  gi_delta = 0.0f;

  misc_1 = 0.0f;
  spk_t = -1;

  act_buf = NULL;
  spike_e_buf = NULL;
  spike_i_buf = NULL;
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

void LeabraUnit::Copy_(const LeabraUnit& cp) {
  act_eq = cp.act_eq;
  act_nd = cp.act_nd;
  act_lrn = cp.act_lrn;
  spike = cp.spike;
  act_m = cp.act_m;
  act_p = cp.act_p;
  act_dif = cp.act_dif;
  net_prv_phs = cp.net_prv_phs;
  net_prv_trl = cp.net_prv_trl;
  act_mid = cp.act_mid;
  da = cp.da;
  avg_ss = cp.avg_ss;
  avg_s = cp.avg_s;
  avg_m = cp.avg_m;
  avg_l = cp.avg_l;
  act_avg = cp.act_avg;
  thal = cp.thal;
  deep5b = cp.deep5b;
  lrnmod = cp.lrnmod;
  deep5b_net = cp.deep5b_net;
  act_ctxt = cp.act_ctxt;
  net_ctxt = cp.net_ctxt;
  p_act_p = cp.p_act_p;
  gc_i = cp.gc_i;
  gc_l = cp.gc_l;
  I_net = cp.I_net;
  v_m = cp.v_m;
  v_m_eq = cp.v_m_eq;
  adapt = cp.adapt;
  gi_syn = cp.gi_syn;
  gi_self = cp.gi_self;
  gi_ex = cp.gi_ex;
  E_i = cp.E_i;
  syn_tr = cp.syn_tr;
  syn_nr = cp.syn_nr;
  syn_pr = cp.syn_pr;
  syn_kre = cp.syn_kre;
  noise = cp.noise;
  dav = cp.dav;
  sev = cp.sev;

  net_scale = cp.net_scale;
  bias_scale = cp.bias_scale;
  ctxt_scale = cp.ctxt_scale;
  act_sent = cp.act_sent;
  net_raw = cp.net_raw;
  net_delta = cp.net_delta;
  gi_raw = cp.gi_raw;
  gi_delta = cp.gi_delta;

  misc_1 = cp.misc_1;
  spk_t = cp.spk_t;
}
