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
}

void LeabraUnit::CutLinks() {
  // if(spike_e_buf) {
  //   taBase::unRefDone(spike_e_buf);
  //   spike_e_buf = NULL;
  // }
  // if(spike_i_buf) {
  //   taBase::unRefDone(spike_i_buf);
  //   spike_i_buf = NULL;
  // }
  // if(act_buf) {
  //   taBase::unRefDone(act_buf);
  //   act_buf = NULL;
  // }

  inherited::CutLinks();
}

void LeabraUnit::BuildUnits() {
  inherited::BuildUnits();
  // LeabraUnitSpec* us = (LeabraUnitSpec*)GetUnitSpec();
  // if(us) {
  //   us->Init_SpikeBuff(this);
  //   us->Init_ActBuff(this);
  // }
}

void LeabraUnit::Copy_(const LeabraUnit& cp) {
  // act_eq = cp.act_eq;
  // act_nd = cp.act_nd;
  // spike = cp.spike;
  // act_q0 = cp.act_q0;
  // act_q1 = cp.act_q1;
  // act_q2 = cp.act_q2;
  // act_q3 = cp.act_q3;
  // act_q4 = cp.act_q4;
  // act_m = cp.act_m;
  // act_p = cp.act_p;
  // act_dif = cp.act_dif;
  // net_prv_q = cp.net_prv_q;
  // net_prv_trl = cp.net_prv_trl;
  // da = cp.da;
  // avg_ss = cp.avg_ss;
  // avg_s = cp.avg_s;
  // avg_m = cp.avg_m;
  // avg_l = cp.avg_l;
  // act_avg = cp.act_avg;
  // thal = cp.thal;
  // thal_prv = cp.thal_prv;
  // deep5b = cp.deep5b;
  // d5b_net = cp.d5b_net;
  // ti_ctxt = cp.ti_ctxt;
  // lrnmod = cp.lrnmod;
  // gc_i = cp.gc_i;
  // I_net = cp.I_net;
  // v_m = cp.v_m;
  // v_m_eq = cp.v_m_eq;
  // adapt = cp.adapt;
  // gi_syn = cp.gi_syn;
  // gi_self = cp.gi_self;
  // gi_ex = cp.gi_ex;
  // E_i = cp.E_i;
  // syn_tr = cp.syn_tr;
  // syn_nr = cp.syn_nr;
  // syn_pr = cp.syn_pr;
  // syn_kre = cp.syn_kre;
  // noise = cp.noise;
  // dav = cp.dav;
  // sev = cp.sev;

  // net_scale = cp.net_scale;
  // bias_scale = cp.bias_scale;
  // ctxt_scale = cp.ctxt_scale;
  // act_sent = cp.act_sent;
  // net_raw = cp.net_raw;
  // gi_raw = cp.gi_raw;
  // d5b_sent = cp.d5b_sent;

  // misc_1 = cp.misc_1;
  // spk_t = cp.spk_t;
}
