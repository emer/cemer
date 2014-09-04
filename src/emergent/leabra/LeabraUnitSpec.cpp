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

#include "LeabraUnitSpec.h"
#include <LeabraNetwork>
#include <LeabraBiasSpec>
#include <LeabraTICtxtConSpec>
#include <Deep5bConSpec>
#include <taProject>
#include <taMath_double>
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraChannels);
TA_BASEFUNS_CTORS_DEFN(LeabraUnitSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActFunSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActFunExSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeFunSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeMiscSpec);
TA_BASEFUNS_CTORS_DEFN(ActAdaptSpec);
TA_BASEFUNS_CTORS_DEFN(DepressSpec);
TA_BASEFUNS_CTORS_DEFN(SynDelaySpec);
TA_BASEFUNS_CTORS_DEFN(OptThreshSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraDtSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActAvgSpec);
TA_BASEFUNS_CTORS_DEFN(VChanSpec);
TA_BASEFUNS_CTORS_DEFN(MaxDaSpec);
TA_BASEFUNS_CTORS_DEFN(CIFERSpec);
TA_BASEFUNS_CTORS_DEFN(DaModSpec);
TA_BASEFUNS_CTORS_DEFN(NoiseAdaptSpec);
SMARTREF_OF_CPP(LeabraUnitSpec);


void LeabraActFunSpec::Initialize() {
  Defaults_init();
}

void LeabraActFunSpec::Defaults_init() {
  gelin = true;
  if(taMisc::is_loading) {
    taVersion v511(5, 1, 1);
    if(taMisc::loading_version < v511) { // default prior to 511 is non-gelin
      gelin = false;
    }
  }
  if(gelin) {
    thr = .5f;
    gain = 100.0f;
    nvar = .005f;
  }
  else {
    thr = .25f;
    gain = 600.0f;
    nvar = .005f;
  }
}

void LeabraActFunSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(owner) owner->UpdateAfterEdit(); // update our unitspec so it can recompute lookup function!
}


void LeabraActFunExSpec::Initialize() {
  i_thr = STD;
  Defaults_init();
}

void LeabraActFunExSpec::Defaults_init() {
  avg_dt = .005f;
  avg_init = 0.15f;
  rescale_ctxt = true;
}

void SpikeFunSpec::Initialize() {
  g_gain = 9.0f;
  rise = 0.0f;
  decay = 5.0f;
  window = 3;
  eq_gain = 8.0f;
  eq_dt = 0.02f;
  // vm_dt of .1 should also be used; vm_noise var .002???

  gg_decay = g_gain / decay;
  gg_decay_sq = g_gain / (decay * decay);
  gg_decay_rise = g_gain / (decay - rise);

  oneo_decay = 1.0f / decay;
  if(rise > 0.0f)
    oneo_rise = 1.0f / rise;
  else
    oneo_rise = 1.0f;
}

void SpikeFunSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(window <= 0) window = 1;
  if(decay > 0.0f) {
    gg_decay = g_gain / decay;
    gg_decay_sq = g_gain / (decay * decay);
    if(decay != rise)
      gg_decay_rise = g_gain / (decay - rise);

    oneo_decay = 1.0f / decay;
    if(rise > 0.0f)
      oneo_rise = 1.0f / rise;
    else
      oneo_rise = 1.0f;
  }
}

void SpikeMiscSpec::Initialize() {
  exp_slope = 0.02f;
  spk_thr = 1.2f;
  vm_r = 0.30f;
  t_r = 6;
  vm_dend = 0.3f;
  vm_dend_dt = 0.16f;
  vm_dend_time = 1.0f / vm_dend_dt;
  clamp_max_p = 0.12f;
  clamp_type = REGULAR;
}

void SpikeMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  vm_dend_time = 1.0f / vm_dend_dt;
}

void ActAdaptSpec::Initialize() {
  on = false;
  Defaults_init();
}

void ActAdaptSpec::Defaults_init() {
  dt = 0.007f;
  vm_gain = 0.04f;
  spike_gain = 0.00805f;
  interval = 10;
  trials = false;

  dt_time = 1.0f / dt;
}

void ActAdaptSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt_time = 1.0f / dt;
}

void DepressSpec::Initialize() {
  on = false;
  phase_act = ACT_ND;
  trials = false;
  Defaults_init();
}

void DepressSpec::Defaults_init() {
  rec = .2f;
  asymp_act = .5f;
  delay = 0;
  interval = 10;
  max_amp = 1.0f;

  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
}

void DepressSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(rec < .00001f) rec = .00001f;
  if(asymp_act < .00001f) asymp_act = .00001f;
  if(asymp_act > 1.0f) asymp_act = 1.0f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  depl = MAX(depl, 0.0f);
}

void SynDelaySpec::Initialize() {
  on = false;
  delay = 4;
}

void OptThreshSpec::Initialize() {
  send = .1f;
  delta = 0.005f;
  phase_dif = 0.0f;             // .8 also useful
  xcal_lrn = 0.01f;
}

void LeabraDtSpec::Initialize() {
  vm_eq_cyc = 0;
  Defaults_init();
}

void LeabraDtSpec::Defaults_init() {
  integ = 1.0f;
  vm = 0.3f;                    // best for gelin -- todo: may need to change
  net = 0.7f;
  d_vm_max = 100.0f;		// todo: may need to change
  midpoint = false;
  vm_eq_dt = 1.0f;
  integ_time = 1.0f / integ;
  vm_time = 1.0f / vm;
  net_time = 1.0f / net;
}

void LeabraDtSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  integ_time = 1.0f / integ;
  vm_time = 1.0f / vm;
  net_time = 1.0f / net;
}

void LeabraActAvgSpec::Initialize() {
  l_up_add = true;
  l_dn_pct = true;
  l_up_dt = 0.2f;
  l_dn_dt = 0.4f;
  m_dt = 0.1f;
  s_dt = 0.2f;
  ss_dt = 1.0f;
  use_nd = false;

  if(taMisc::is_loading) {
    taVersion v634(6, 3, 4);
    if(taMisc::loading_version < v634) { // default prior to 634 is off
      l_up_add = false;
      l_dn_pct = false;
      l_up_dt = 0.6f;
      l_dn_dt = 0.05f;
    }
  }

  l_time = 1.0f / l_dn_dt;
  m_time = 1.0f / m_dt;
  s_time = 1.0f / s_dt;
  ss_time = 1.0f / ss_dt;
}

void LeabraActAvgSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  l_time = 1.0f / l_dn_dt;
  m_time = 1.0f / m_dt;
  s_time = 1.0f / s_dt;
  ss_time = 1.0f / ss_dt;
}

void LeabraChannels::Initialize() {
  e = l = i = h = a = 0.0f;
}

void LeabraChannels::Copy_(const LeabraChannels& cp) {
  e = cp.e;
  l = cp.l;
  i = cp.i;
  h = cp.h;
  a = cp.a;
}

void VChanSpec::Initialize() {
  on = false;
  b_inc_dt = .01f;
  b_dec_dt = .01f;
  a_thr = .5f;
  d_thr = .1f;
  g_dt = .1f;
  init = false;
  trl = false;
}

void VChanBasis::Initialize() {
  hyst = acc = 0.0f;
  hyst_on = acc_on = false;
  g_h = g_a = 0.0f;
}

void VChanBasis::Copy_(const VChanBasis& cp) {
  hyst = cp.hyst;
  acc = cp.acc;
  hyst_on = cp.hyst_on;
  acc_on = cp.acc_on;
  g_h = cp.g_h;
  g_a = cp.g_a;
}

void CIFERSpec::Initialize() {
  on = false;
  phase_updt = false;
  super_gain = .05f;
  thal_5b_thr = 0.5f;
  act_5b_thr = 0.5f;
  binary5b = false;
  ti_5b = 0.5f;
  bg_lrate = 1.0f;
  fg_lrate = 0.0f;
  Defaults_init();
}

void CIFERSpec::Defaults_init() {
}

void DaModSpec::Initialize() {
  on = false;
  gain = .1f;
  Defaults_init();
}

void DaModSpec::Defaults_init() {
  mod = PLUS_CONT;
}

void NoiseAdaptSpec::Initialize() {
  trial_fixed = true;
  k_pos_noise = false;
  mode = FIXED_NOISE;
  Defaults_init();
}

void NoiseAdaptSpec::Defaults_init() {
  min_pct = 0.5f;
  min_pct_c = 1.0f - min_pct;
}

void NoiseAdaptSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  min_pct_c = 1.0f - min_pct;
}

void MaxDaSpec::Initialize() {
  val = INET_DA;
  inet_scale = 1.0;
  lay_avg_thr = 0.01f;
}

void LeabraUnitSpec::Initialize() {
  min_obj_type = &TA_LeabraUnit;
  bias_con_type = &TA_LeabraCon;
  bias_spec.SetBaseType(&TA_LeabraBiasSpec);

  act_fun = NOISY_XX1;

  act_lrn_on = false;

  noise_type = NO_NOISE;
  noise.type = Random::GAUSSIAN;
  noise.var = .001f;

  Defaults_init();

  CreateNXX1Fun(act, nxx1_fun, noise_conv);
  CreateNXX1Fun(act_lrn, lrn_nxx1_fun, lrn_noise_conv);
}

void LeabraUnitSpec::Defaults_init() {
  // todo: shouldn't I call Defaults_init on all the sub-guys, or is this automatic?
  sse_tol = .5f;
  clamp_range.min = .0f;
  clamp_range.max = .95f;
  clamp_range.UpdateAfterEdit_NoGui();

  vm_range.max = 2.0f;
  vm_range.min = 0.0f;
  vm_range.UpdateAfterEdit_NoGui();

  g_bar.e = 1.0f;
  g_bar.l = 0.1f;
  g_bar.i = 1.0f;
  g_bar.h = 0.01f;
  g_bar.a = 0.03f;
  e_rev.e = 1.0f;
  e_rev.l = 0.3f;
  e_rev.i = 0.25f;
  e_rev.h = 1.0f;
  e_rev.a = 0.0f;

  v_m_init.type = Random::NONE;
  v_m_init.mean = e_rev.l;
  v_m_init.var = 0.0f;

  act_init.type = Random::NONE;
  act_init.mean = 0.0f;
  act_init.var = 0.0f;

  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  e_rev_sub_thr.h = e_rev.h - act.thr;
  e_rev_sub_thr.a = e_rev.a - act.thr;
  //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);

  e_rev_sub_thr_lrn.e = e_rev.e - act_lrn.thr;
  e_rev_sub_thr_lrn.l = e_rev.l - act_lrn.thr;
  e_rev_sub_thr_lrn.i = e_rev.i - act_lrn.thr;
  e_rev_sub_thr_lrn.h = e_rev.h - act_lrn.thr;
  e_rev_sub_thr_lrn.a = e_rev.a - act_lrn.thr;

  lrn_thr_sub_e_rev_e = (act_lrn.thr - e_rev.e);

  hyst.b_inc_dt = .05f;
  hyst.b_dec_dt = .05f;
  hyst.a_thr = .8f;
  hyst.d_thr = .7f;
}

void LeabraUnitSpec::InitLinks() {
  //  bias_spec.type = &TA_LeabraBiasSpec;
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LeabraUnitSpec);
  children.SetBaseType(&TA_LeabraUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void LeabraUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(act_fun == SPIKE) {
    dt.midpoint = true;         // must use midpoint for spiking
  }

  clamp_range.UpdateAfterEdit_NoGui();
  vm_range.UpdateAfterEdit_NoGui();
  depress.UpdateAfterEdit_NoGui();
  noise_sched.UpdateAfterEdit_NoGui();
  spike.UpdateAfterEdit_NoGui();
  spike_misc.UpdateAfterEdit_NoGui();
  adapt.UpdateAfterEdit_NoGui();
  dt.UpdateAfterEdit_NoGui();
  act_avg.UpdateAfterEdit_NoGui();
  noise_adapt.UpdateAfterEdit_NoGui();
  CreateNXX1Fun(act, nxx1_fun, noise_conv);
  CreateNXX1Fun(act_lrn, lrn_nxx1_fun, lrn_noise_conv);
  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  e_rev_sub_thr.h = e_rev.h - act.thr;
  e_rev_sub_thr.a = e_rev.a - act.thr;
  //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);

  e_rev_sub_thr_lrn.e = e_rev.e - act_lrn.thr;
  e_rev_sub_thr_lrn.l = e_rev.l - act_lrn.thr;
  e_rev_sub_thr_lrn.i = e_rev.i - act_lrn.thr;
  e_rev_sub_thr_lrn.h = e_rev.h - act_lrn.thr;
  e_rev_sub_thr_lrn.a = e_rev.a - act_lrn.thr;
  lrn_thr_sub_e_rev_e = (act_lrn.thr - e_rev.e);
}

void LeabraUnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(da_mod.on && act.i_thr != LeabraActFunExSpec::NO_AH, quiet, rval,
                "da_mod is on but act.i_thr != NO_AH -- this is generally required for da modulation to work properly as it operates through the a & h currents, and including them in i_thr computation leads to less clean modulation effects -- I set this for you in spec:", name)) {
    SetUnique("act", true);
    act.i_thr = LeabraActFunExSpec::NO_AH; // key for dopamine effects
  }
//  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  // if(net) {
  // }
}

bool LeabraUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  //  Network* net = GET_MY_OWNER(Network);
  bool rval = true;

  return rval;
}

void LeabraUnitSpec::CreateNXX1Fun(LeabraActFunSpec& act_spec, FunLookup& nxx1_fl, 
                                   FunLookup& noise_fl) {
  // first create the gaussian noise convolver
  if(act_spec.gelin) {
    nxx1_fl.x_range.min = -.1f;
    nxx1_fl.x_range.max = 1.0f;
    nxx1_fl.res = .001f;       // needs same fine res to get the noise transitions
    nxx1_fl.UpdateAfterEdit_NoGui();
  }
  else {
    nxx1_fl.x_range.min = -.03f;
    nxx1_fl.x_range.max = .20f;
    nxx1_fl.res = .001f;
    nxx1_fl.UpdateAfterEdit_NoGui();
  }

  float ns_rng = 3.0f * act_spec.nvar;       // range factor based on noise level -- 3 sd
  ns_rng = MAX(ns_rng, nxx1_fl.res);
  nxx1_fl.x_range.min = -ns_rng;

  noise_fl.x_range.min = -ns_rng;
  noise_fl.x_range.max = ns_rng;
  noise_fl.res = nxx1_fl.res;
  noise_fl.UpdateAfterEdit_NoGui();

  noise_fl.AllocForRange();
  int i;
  float eff_nvar = MAX(act_spec.nvar, 1.0e-6f); // just too lazy to do proper conditional for 0..
  float var = eff_nvar * eff_nvar;
  for(i=0; i < noise_fl.size; i++) {
    float x = noise_fl.Xval(i);
    noise_fl[i] = expf(-((x * x) / var));
  }

  // normalize it
  float sum = 0.0f;
  for(i=0; i < noise_fl.size; i++)
    sum += noise_fl[i];
  for(i=0; i < noise_fl.size; i++)
    noise_fl[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fl.x_range.min + noise_fl.x_range.min;
  fun.x_range.max = nxx1_fl.x_range.max + noise_fl.x_range.max;
  fun.res = nxx1_fl.res;
  fun.UpdateAfterEdit_NoGui();
  fun.AllocForRange();

  if(act_fun == LeabraUnitSpec::NOISY_LINEAR) {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
        val = act_spec.gain * x;
      fun[i] = val;
    }
  }
  else {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
        val = (act_spec.gain * x) / ((act_spec.gain * x) + 1.0f);
      fun[i] = val;
    }
  }

  nxx1_fl.Convolve(fun, noise_fl); // does alloc
}

void LeabraUnitSpec::SetLearnRule(LeabraNetwork* net) {
  if(bias_spec.SPtr())
    ((LeabraConSpec*)bias_spec.SPtr())->SetLearnRule(net);
}

int LeabraUnitSpec::CountCons(LeabraUnit* u, LeabraNetwork* net) {
  int rval = u->Unit::CountCons(net);
  return rval;
}

void LeabraUnitSpec::Init_dWt(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;

  // do this sender based!
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    send_gp->Init_dWt(u, (LeabraNetwork*)net);
  }

  if(u->bias.size > 0) {
    bias_spec->B_Init_dWt(&u->bias, u, net);
  }
}

void LeabraUnitSpec::Init_Weights(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  // do this sender based!
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    send_gp->Init_Weights(u, (LeabraNetwork*)net);
  }

  if(u->bias.size > 0) {
    bias_spec->B_Init_Weights(&u->bias, u, net);
  }

  u->act_avg = act.avg_init;
  u->misc_1 = 0.0f;
  u->spk_amp = depress.max_amp;
  u->vcb.hyst = u->vcb.g_h = 0.0f;
  u->vcb.hyst_on = false;
  u->vcb.acc = u->vcb.g_a = 0.0f;
  u->vcb.acc_on = false;

  u->ClearLearnFlag();

  Init_ActAvg(u, net);
}

void LeabraUnitSpec::Init_Weights_sym(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;

  // do this sender based!
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    Projection* prjn = send_gp->prjn;
    if(prjn->layer->units_flat_idx < prjn->from->units_flat_idx)
      continue;                 // higher copies from lower, so if we're lower, bail..
    send_gp->Init_Weights_sym(u, (LeabraNetwork*)net);
  }
}

void LeabraUnitSpec::Init_Weights_post(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;

  // do this sender based!
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    send_gp->Init_Weights_post(u, (LeabraNetwork*)net);
  }

  if(u->bias.size > 0) {
    bias_spec->B_Init_Weights_post(&u->bias, u, net);
  }
}


void LeabraUnitSpec::Init_ActAvg(LeabraUnit* u, LeabraNetwork* net) {
  u->act_avg = act.avg_init;
  u->avg_l = act.avg_init;
}

void LeabraUnitSpec::Init_Netins(LeabraUnit* u, LeabraNetwork*) {
  // too drastic
  // u->net_scale = 0.0f;
  // u->bias_scale = 0.0f;
  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->net_delta = 0.0f;
  u->g_i_raw = 0.0f;
  u->g_i_delta = 0.0f;
  // u->g_i_syn = 0.0f;
  //  u->net_ctxt = 0.0f;

  // u->net = 0.0f;
  // u->gc.i = 0.0f;

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    recv_gp->net = 0.0f;
    recv_gp->net_raw = 0.0f;
  }
}

void LeabraUnitSpec::Init_Acts(Unit* ru, Network* rnet) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Acts(u, net);

  Init_Netins(u, net);

  u->net = 0.0f;               // these are not done in netins -- need to nuke
  u->g_i_syn = 0.0f;
  u->g_i_self = 0.0f;
  u->gc.i = 0.0f;

  if(hyst.init) {
    u->vcb.hyst = u->vcb.g_h = 0.0f;
    u->vcb.hyst_on = false;
  }
  if(acc.init) {
    u->vcb.acc = u->vcb.g_a = 0.0f;
    u->vcb.acc_on = false;
  }
  u->gc.l = 0.0f;
  u->gc.i = 0.0f;
  u->gc.h = 0.0f;
  u->gc.a = 0.0f;
  u->I_net = 0.0f;
  u->v_m = v_m_init.Gen();
  u->vm_dend = 0.0f;
  u->adapt = 0.0f;
  u->da = 0.0f;
  u->act = act_init.Gen();
  u->act_eq = u->act;
  u->act_nd = u->act_eq;
  u->act_p = u->act_m = u->act_dif = 0.0f;
  u->act_mid = 0.0f;
  u->avg_ss = act.avg_init;
  u->avg_s = act.avg_init;
  u->avg_m = act.avg_init;
  u->thal = 0.0f;
  u->deep5b = 0.0f;
  u->deep5b_net = 0.0f;
  u->act_ctxt = 0.0f;
  u->net_ctxt = 0.0f;
  u->p_act_p = 0.0f;
  u->davg = 0.0f;
  u->dav = 0.0f;
  u->sev = 0.0f;
  u->noise = 0.0f;

  u->i_thr = 0.0f;
  if(depress.on)
    u->spk_amp = depress.max_amp;
  u->spk_t = -1;

  Init_SpikeBuff(u);
  if(act_fun == SPIKE) {
    u->spike_e_buf->Reset();
    u->spike_i_buf->Reset();
  }

  Init_ActBuff(u);
  if(syn_delay.on) {
    u->act_buf->Reset();
  }
}


void LeabraUnitSpec::DecayState(LeabraUnit* u, LeabraNetwork* net, float decay) {
  // todo: turn this on after next round of testing
  if(decay > 0.0f) {            // no need to reset netin if not decaying at all
    Init_Netins(u, net);
  }

  u->v_m -= decay * (u->v_m - v_m_init.mean);
  u->vm_dend -= decay * u->vm_dend;
  u->adapt -= decay * u->adapt;
  u->act -= decay * (u->act - act_init.mean);
  u->act_nd -= decay * (u->act_nd - act_init.mean);
  u->act_eq -= decay * (u->act_eq - act_init.mean);
  u->p_act_p -= decay * (u->p_act_p - act_init.mean); // reset this for ti guys
  // note: this is causing a problem in learning with xcal:
//   u->avg_ss -= decay * (u->avg_ss - act.avg_init);
//   u->avg_s -= decay * (u->avg_s - act.avg_init);
//   u->avg_m -= decay * (u->avg_m - act.avg_init);
  u->net -= decay * u->net;
  u->g_i_syn -= decay * u->g_i_syn;
  u->g_i_self -= decay * u->g_i_self;
  u->gc.i -= decay * u->gc.i;   // not really needed but visually..

  if(hyst.on && !hyst.trl)
    u->vcb.hyst -= decay * u->vcb.hyst;
  if(acc.on && !acc.trl)
    u->vcb.acc -= decay * u->vcb.acc;
  if(depress.on)
    u->spk_amp += (depress.max_amp - u->spk_amp) * decay;

  u->da = u->I_net = 0.0f;

  if(decay == 1.0f) {
    Init_SpikeBuff(u);
    if(act_fun == SPIKE) {
      u->spike_e_buf->Reset();
      u->spike_i_buf->Reset();
    }

    Init_ActBuff(u);
    if(syn_delay.on) {
      u->act_buf->Reset();
    }
  }
}

void LeabraUnitSpec::Init_SpikeBuff(LeabraUnit* u) {
  if(act_fun == SPIKE) {
    if(!u->spike_e_buf) {
      u->spike_e_buf = new float_CircBuffer;
      taBase::Own(u->spike_e_buf, u);
    }
    if(!u->spike_i_buf) {
      u->spike_i_buf = new float_CircBuffer;
      taBase::Own(u->spike_i_buf, u);
    }
  }
  else {
    if(u->spike_e_buf) {
      taBase::unRefDone(u->spike_e_buf);
      u->spike_e_buf = NULL;
    }
    if(u->spike_i_buf) {
      taBase::unRefDone(u->spike_i_buf);
      u->spike_i_buf = NULL;
    }
  }
}

void LeabraUnitSpec::Init_ActBuff(LeabraUnit* u) {
  if(syn_delay.on) {
    if(!u->act_buf) {
      u->act_buf = new float_CircBuffer;
      taBase::Own(u->act_buf, u);
    }
  }
  else {
    if(u->act_buf) {
      taBase::unRefDone(u->act_buf);
      u->act_buf = NULL;
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      TrialInit functions

void LeabraUnitSpec::Trial_Init_Specs(LeabraNetwork* net) {
  if(bias_spec.SPtr())
    ((LeabraConSpec*)bias_spec.SPtr())->Trial_Init_Specs(net);
}

void LeabraUnitSpec::Trial_Init_Unit(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  Trial_DecayState(u, net);
  Trial_NoiseInit(u, net);
  Trial_Init_SRAvg(u, net);
}

void LeabraUnitSpec::Trial_DecayState(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* lay = u->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  DecayState(u, net, ls->decay.event);
  u->dav = 0.0f;
  u->sev = 0.0f;
  u->ClearLearnFlag();
}

void LeabraUnitSpec::Trial_NoiseInit(LeabraUnit* u, LeabraNetwork* net) {
  if(noise_type != NO_NOISE && noise_adapt.trial_fixed && !noise_adapt.k_pos_noise &&
     (noise.type != Random::NONE)) {
    u->noise = noise.Gen();
  }
}

void LeabraUnitSpec::Trial_Init_SRAvg(LeabraUnit* u, LeabraNetwork* net) {
//  LeabraLayer* lay = u->own_lay();

  float lval = u->avg_m;
  if(act_avg.l_up_add) {
    if(lval > opt_thresh.send) {          // active, even just a bit
      u->avg_l += lval * act_avg.l_up_dt; // additive up
    }
    else {
      float eff_dt = act_avg.l_dn_dt;
      if(act_avg.l_dn_pct)
        eff_dt *= u->own_lay()->kwta.pct;
      u->avg_l += eff_dt * (lval - u->avg_l); // mult down
    }
  }
  else {
    if(lval > u->avg_l) {
      u->avg_l += act_avg.l_up_dt * (lval - u->avg_l);
    }
    else {
      float eff_dt = act_avg.l_dn_dt;
      if(act_avg.l_dn_pct)
        eff_dt *= u->own_lay()->kwta.pct;
      u->avg_l += eff_dt * (lval - u->avg_l);
    }
  }

  if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL || net->ct_sravg.force_con)  {
    for(int g = 0; g < u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->NotActive()) continue;
      send_gp->Trial_Init_SRAvg(u,net);
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      SettleInit functions

void LeabraUnitSpec::Settle_Init_Unit(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  Settle_Init_TargFlags(u, net);
  Settle_DecayState(u, net);
  Compute_NetinScale(u, net);
}

void LeabraUnitSpec::Settle_Init_TargFlags(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* lay = u->own_lay();
  if(!u->HasExtFlag(Unit::TARG))
    return;

  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    if(!lay->HasExtFlag(Unit::TARG)) {  // layer isn't a target but unit is..
      u->targ = u->ext;
    }
    u->ext = 0.0f;
    u->UnSetExtFlag(Unit::EXT);
  }
  else {
    u->ext = u->targ;
    u->SetExtFlag(Unit::EXT);
  }
}

void LeabraUnitSpec::Settle_DecayState(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* lay = u->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  float dkval = 0.0f;
  if(net->phase_no == 1)
    dkval = ls->decay.phase;
  DecayState(u, net, dkval);
}

void LeabraUnitSpec::Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net) {
  // this is all receiver-based and done only at beginning of each settle
  u->net_scale = 0.0f;  // total of scale values for this unit's inputs
  float inhib_net_scale = 0.0f;
  int n_active_cons = 0;        // track this for bias weight scaling!
  bool plus_phase = (net->phase == LeabraNetwork::PLUS_PHASE);
  // possible dependence on recv_gp->size is why this cannot be computed in Projection
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->NotActive()) continue;
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    cs->Compute_NetinScale(recv_gp, from, plus_phase); // sets recv_gp->scale_eff
    float rel_scale = 0.0f;
    if(plus_phase && cs->diff_scale_p)
      rel_scale = cs->wt_scale_p.rel;
    else
      rel_scale = cs->wt_scale.rel;
    if(cs->inhib) {
      inhib_net_scale += rel_scale;
    }
    else {
      n_active_cons++;
      u->net_scale += rel_scale;
    }
  }
  // add the bias weight into the netinput, scaled by 1/n
  if(u->bias.size) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.SPtr();
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(u->n_recv_cons > 0)
      u->bias_scale /= (float)u->n_recv_cons; // one over n scaling for bias!
  }

  float ctxt_rel_scale = 0.0f;
  // now renormalize, each one separately..
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->NotActive()) continue;
    Projection* prjn = (Projection*) recv_gp->prjn;
    LeabraLayer* from = (LeabraLayer*) prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->inhib) {
      if(inhib_net_scale > 0.0f)
        recv_gp->scale_eff /= inhib_net_scale;
    }
    else {
      if(u->net_scale > 0.0f) {
        recv_gp->scale_eff /= u->net_scale;
        if(cs->IsTICtxtCon()) {
          ctxt_rel_scale += recv_gp->scale_eff;
        }
      }
    }
  }

  if(act.rescale_ctxt && u->ctxt_scale > 0.0f && ctxt_rel_scale > 0.0f) {
    // context is computed in plus phase, with plus phase scaling.  thus we need to do it 
    // both ways -- up and down -- in either case, always going toward current
    u->act_ctxt *= (ctxt_rel_scale / u->ctxt_scale);
  }
  u->ctxt_scale = ctxt_rel_scale;
}

void LeabraUnitSpec::Compute_NetinScale_Senders(LeabraUnit* u, LeabraNetwork* net) {
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) {
      send_gp->scale_eff = 0.0f;
    }
    else {
      Unit* ru = send_gp->Un(0,net);
      send_gp->scale_eff =
        ((LeabraRecvCons*)ru->recv.FastEl(send_gp->recv_idx()))->scale_eff;
    }
  }
}

void LeabraUnitSpec::Compute_HardClamp(LeabraUnit* u, LeabraNetwork*) {
  u->net = u->ext;
  u->thal = u->ext;             // thalamus is external input
  u->act_eq = clamp_range.Clip(u->ext);
  u->act_nd = u->act_eq;
  u->act_lrn = u->act_eq;
  u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;

  if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
  u->AddToActBuf(syn_delay);
}

// NOTE: these two functions should always be the same modulo the clamp_range.Clip

void LeabraUnitSpec::Compute_HardClampNoClip(LeabraUnit* u, LeabraNetwork*) {
  u->net = u->ext;
  //  u->act_eq = clamp_range.Clip(u->ext);
  u->act_eq = u->ext;
  u->act_nd = u->act_eq;
  u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;

  if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
  u->AddToActBuf(syn_delay);
}

void LeabraUnitSpec::ExtToComp(LeabraUnit* u, LeabraNetwork*) {
  if(!u->HasExtFlag(Unit::EXT))
    return;
  u->ext_flag = Unit::COMP;
  u->targ = u->ext;
  u->ext = 0.0f;
}

void LeabraUnitSpec::TargExtToComp(LeabraUnit* u, LeabraNetwork*) {
  if(!u->HasExtFlag(Unit::TARG_EXT))
    return;
  if(u->HasExtFlag(Unit::EXT))
    u->targ = u->ext;
  u->ext = 0.0f;
  u->ext_flag = Unit::COMP;
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 1: netinput

void LeabraUnitSpec::Send_NetinDelta(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  int thno_arg = thread_no;
  if(thread_no < 0) {
    net->send_pct_tot++;        // only safe for non-thread case
    thno_arg = 0;		// pass 0 as arg -- need to store in tmp vec
  }
  float act_ts = u->act;
  if(syn_delay.on) {
    if(!u->act_buf)
      Init_ActBuff(u);
    act_ts = u->act_buf->CircSafeEl(0); // get first logical element..
  }

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->act_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      if(thread_no < 0)
        net->send_pct_n++;
      for(int g=0; g<u->send.size; g++) {
        LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(tol->hard_clamped)      continue;
        send_gp->Send_NetinDelta(net, thno_arg, act_delta);
      }
      u->act_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    if(thread_no < 0)
      net->send_pct_n++;
    float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->hard_clamped)        continue;
      send_gp->Send_NetinDelta(net, thno_arg, act_delta);
    }
    u->act_sent = 0.0f;         // now it effectively sent a 0..
  }
}

void LeabraUnitSpec::Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  // this integrates from SendDelta and then does further integration
  int nt = net->lthreads.n_threads_act;
  float nw_nt = 0.0f;

  LeabraLayer* lay = u->own_lay();
  if(lay->hard_clamped) return;
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  if(net->NetinPerPrjn()) {
    float nw_inhb = 0.0f;
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->NotActive()) continue;
      float g_nw_nt = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ndval = net->send_netin_tmp.FastEl3d(u->flat_idx, g, j); 
	g_nw_nt += ndval;
        ndval = 0.0f;           // zero immediately upon use -- for threads
        // note: if another thread in async mode is still in SendDelta, it could
        // theoretically be writing to this value at the same time we're zeroing
        // causing data corruption -- these are the risks..
      }
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      // todo: incorporate finer-grained inhib here..
      if(cs->inhib)
	nw_inhb += g_nw_nt;
      else
	nw_nt += g_nw_nt;

      recv_gp->net_raw += g_nw_nt;
      // todo: not clear if we need this at this point..
      // u->net += dt.net * (tot_net - u->net);
      // u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
    }
    u->net_delta = nw_nt;
    u->g_i_delta = nw_inhb;
  }
  else {
    for(int j=0;j<nt;j++) {
      float& ndval = net->send_netin_tmp.FastEl2d(u->flat_idx, j);
      nw_nt += ndval;
      ndval = 0.0f;           // zero immediately upon use -- for threads
    }
    u->net_delta = nw_nt;
  }

  if(net->inhib_cons_used) {
    u->g_i_raw += u->g_i_delta;
    if(act_fun == SPIKE) {
      u->g_i_syn = MAX(u->g_i_syn, 0.0f);
      Compute_NetinInteg_Spike_i(u, net);
    }
    else {
      u->g_i_syn += dt.net * (u->g_i_raw - u->g_i_syn);
      u->g_i_syn = MAX(u->g_i_syn, 0.0f); // negative netin doesn't make any sense
    }
  }
  else {
    // clear so automatic inhibition can add to these values!
    u->g_i_syn = 0.0f;
    u->g_i_raw = 0.0f;
  }
  
  u->net_raw += u->net_delta;
  float tot_net = (u->bias_scale * u->bias.OwnCn(0,LeabraConSpec::WT)) + u->net_raw;

  if(u->HasExtFlag(Unit::EXT)) {
    tot_net += u->ext * ls->clamp.gain;
  }
  else if(u->HasExtFlag(Unit::TARG)) {
    tot_net += u->targ * ls->clamp.minus_targ_gain;
  }

  if(net->ti_mode) {
    tot_net += u->act_ctxt + u->deep5b_net;
  }

  if(cifer.on) {
    tot_net += cifer.super_gain * u->thal * tot_net;
  }

  u->net_delta = 0.0f;  // clear for next use
  u->g_i_delta = 0.0f;  // clear for next use

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = MAX(tot_net, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u, net);
  }
  else {
    u->net += dt.net * (tot_net - u->net);
    u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
  }

  // add just before computing i_thr -- after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += Compute_Noise(u, net);
  }

  if(net->net_misc.kwta_used) {
    u->i_thr = Compute_IThresh(u, net);
  }
}

void LeabraUnitSpec::Compute_NetinInteg_Spike_e(LeabraUnit* u, LeabraNetwork* net) {
  // netin gets added at the end of the spike_buf -- 0 time is the end
  Init_SpikeBuff(u);
  u->spike_e_buf->CircAddLimit(u->net, spike.window); // add current net to buffer
  int mx = MAX(spike.window, u->spike_e_buf->length);
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net
    for(int t=0;t<mx;t++) {
      sum += u->spike_e_buf->CircSafeEl(t);
    }
    sum /= (float)spike.window; // normalize over window
    u->net += spike.gg_decay * sum - (u->net * spike.oneo_decay);
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = u->spike_e_buf->CircSafeEl(t);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    // from compute_netinavg
    u->net += dt.net * (sum - u->net);
  }
  u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
}

void LeabraUnitSpec::Compute_NetinInteg_Spike_i(LeabraUnit* u, LeabraNetwork* net) {
  // netin gets added at the end of the spike_i_buf -- 0 time is the end
  Init_SpikeBuff(u);
  u->spike_i_buf->CircAddLimit(u->gc.i, spike.window); // add current net to buffer
  int mx = MAX(spike.window, u->spike_i_buf->length);
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net
    for(int t=0;t<mx;t++) {
      sum += u->spike_i_buf->CircSafeEl(t);
    }
    sum /= (float)spike.window; // normalize over window
    u->g_i_syn += spike.gg_decay * sum - (u->g_i_syn * spike.oneo_decay);
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = u->spike_i_buf->CircSafeEl(t);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    u->g_i_syn += dt.net * (sum - u->g_i_syn);
  }
  u->g_i_syn = MAX(u->g_i_syn, 0.0f); // negative netin doesn't make any sense
}

float LeabraUnitSpec::Compute_IThresh(LeabraUnit* u, LeabraNetwork* net) {
  switch(act.i_thr) {
  case LeabraActFunExSpec::STD:
    return Compute_IThreshStd(u, net);
  case LeabraActFunExSpec::NO_A:
    return Compute_IThreshNoA(u, net);
  case LeabraActFunExSpec::NO_H:
    return Compute_IThreshNoH(u, net);
  case LeabraActFunExSpec::NO_AH:
    return Compute_IThreshNoAH(u, net);
  case LeabraActFunExSpec::ALL:
    return Compute_IThreshAll(u, net);
  }
  return 0.0f;
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: inhibition


///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: activation

void LeabraUnitSpec::Compute_Act(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  LeabraLayer* lay = u->own_lay();

  if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

  if((net->cycle >= 0) && lay->hard_clamped) {
    if(lay->hard_clamped && act_fun == SPIKE) {
      Compute_ClampSpike(u, net, u->ext * spike_misc.clamp_max_p);
      u->AddToActBuf(syn_delay);
    }
    Compute_SRAvg(u, net, thread_no); // unit level -- needed even for clamped
    return; // don't re-compute
  }

  // first, apply inhibition 
  LeabraInhib* thr = GetInhib(u);
  if(thr) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    Compute_ApplyInhib(u, ls, net, thr->i_val.g_i);
  }

  Compute_Conduct(u, net);
  Compute_Vm(u, net);
  Compute_ActFmVm(u, net);
  Compute_SelfReg_Cycle(u, net);

  u->AddToActBuf(syn_delay);

  Compute_SRAvg(u, net, thread_no); // unit level only, not cons (must be separate)
}

void LeabraUnitSpec::Compute_ClampSpike(LeabraUnit* u, LeabraNetwork* net, float spike_p) {
  bool fire_now = false;
  switch(spike_misc.clamp_type) {
  case SpikeMiscSpec::POISSON:
    if(Random::Poisson(spike_p) > 0.0f) fire_now = true;
    break;
  case SpikeMiscSpec::UNIFORM:
    fire_now = Random::BoolProb(spike_p);
    break;
  case SpikeMiscSpec::REGULAR: {
    if(spike_p > 0.0f) {
      int cyc_int = (int)((1.0f / spike_p) + 0.5f);
      fire_now = (net->ct_cycle % cyc_int == 0);
    }
    break;
  }
  case SpikeMiscSpec::CLAMPED:
    return;                     // do nothing further
  }
  if(fire_now) {
    u->v_m = spike_misc.spk_thr + 0.1f; // make it fire
  }
  else {
    u->v_m = e_rev.l;           // make it not fire
  }

  Compute_ActFmVm_spike(u, net); // then do normal spiking computation
}

void LeabraUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* lay = u->own_lay();

  if(da_mod.on && (da_mod.mod == DaModSpec::PLUS_CONT)) {
    Compute_DaMod_PlusCont(u,net);
  }

  // total conductances
  float g_bar_i_val = g_bar.i;
  float g_bar_l_val = g_bar.l;
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->spec.SPtr();
  u->gc.i *= g_bar_i_val;
  //  u->net *= g_bar_e_val; // do NOT do this here -- keep in original units for dnet stuff -- g_bar.e is multiplied later when net is actually used..
  u->gc.l = g_bar_l_val;
  u->gc.h = g_bar.h * u->vcb.g_h;
  u->gc.a = g_bar.a * u->vcb.g_a;
}

void LeabraUnitSpec::Compute_DaMod_PlusCont(LeabraUnit* u, LeabraNetwork* net) {
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    if(u->dav > 0.0f) {
      u->vcb.g_a = 0.0f;
      u->vcb.g_h = da_mod.gain * u->dav * u->act_m; // increase in proportion to participation in minus phase
    }
    else {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = -da_mod.gain * u->dav * u->act_m; // decrease in proportion to participation in minus phase
    }
  }
  else {
    u->vcb.g_h = 0.0f;
    u->vcb.g_a = 0.0f;  // clear in minus phase!
  }
}

void LeabraUnitSpec::Compute_Vm(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun == SPIKE && spike_misc.t_r > 0 && u->spk_t > 0) {
    int spkdel = net->ct_cycle - u->spk_t;
    if(spkdel >= 0 && spkdel <= spike_misc.t_r)
      return;                   // just bail
  }

  if(net->cycle < dt.vm_eq_cyc) {
    // directly go to equilibrium value
    float new_v_m = Compute_EqVm(u);
    u->I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
    u->v_m += dt.vm_eq_dt * u->I_net;
  }
  else {
    float v_m_eff = u->v_m;
    float net_eff = u->net * g_bar.e;
    if(dt.midpoint) {
      float I_net_1 =
        (net_eff * (e_rev.e - v_m_eff)) + (u->gc.l * (e_rev.l - v_m_eff)) +
        (u->gc.i * (e_rev.i - v_m_eff)) + (u->gc.h * (e_rev.h - v_m_eff)) +
        (u->gc.a * (e_rev.a - v_m_eff));
      v_m_eff += .5f * dt.vm * I_net_1; // go half way
    }
    u->I_net =
      (net_eff * (e_rev.e - v_m_eff)) + (u->gc.l * (e_rev.l - v_m_eff)) +
      (u->gc.i * (e_rev.i - v_m_eff)) + (u->gc.h * (e_rev.h - v_m_eff)) +
      (u->gc.a * (e_rev.a - v_m_eff));

    // add spike current if relevant
    if(act_fun == SPIKE && spike_misc.exp_slope > 0.0f) {
      u->I_net += g_bar.l * spike_misc.exp_slope *
        expf((v_m_eff - act.thr) / spike_misc.exp_slope);
    }

    float dvm = dt.vm * (u->I_net - u->adapt);
    if(dvm > dt.d_vm_max) dvm = dt.d_vm_max;
    else if(dvm < -dt.d_vm_max) dvm = -dt.d_vm_max;
    u->v_m += dt.integ * dvm;
  }

  if((noise_type == VM_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->v_m += Compute_Noise(u, net);
  }

  u->v_m = vm_range.Clip(u->v_m);

  if(act_fun == SPIKE) {
    // decay back to zero
    u->vm_dend -= spike_misc.vm_dend_dt * u->vm_dend;
  }
}

void LeabraUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun == SPIKE) {
    Compute_ActFmVm_spike(u, net);
    Compute_ActLrnFmVm_rate(u, net); // todo: use rate-coded learning here???
  }
  else {
    Compute_ActFmVm_rate(u, net);
    Compute_ActLrnFmVm_rate(u, net);
  }
}

float LeabraUnitSpec::Compute_ActValFmVmVal_rate_impl(float val_sub_thr,
                                 LeabraActFunSpec& act_spec, FunLookup& nxx1_fl) {
  float new_act = 0.0f;
  switch(act_fun) {
  case NOISY_XX1: {
    if(val_sub_thr <= nxx1_fl.x_range.min)
      new_act = 0.0f;
    else if(val_sub_thr >= nxx1_fl.x_range.max) {
      val_sub_thr *= act_spec.gain;
      new_act = val_sub_thr / (val_sub_thr + 1.0f);
    }
    else {
      new_act = nxx1_fl.Eval(val_sub_thr);
    }
    break;
  }
  case SPIKE:
    break;                      // compiler food
  case NOISY_LINEAR: {
    if(val_sub_thr <= nxx1_fl.x_range.min)
      new_act = 0.0f;
    else if(val_sub_thr >= nxx1_fl.x_range.max) {
      new_act = val_sub_thr * act_spec.gain;
    }
    else {
      new_act = nxx1_fl.Eval(val_sub_thr);
    }
    break;
  }
  case XX1: {
    if(val_sub_thr < 0.0f)
      new_act = 0.0f;
    else {
      val_sub_thr *= act_spec.gain;
      new_act = val_sub_thr / (val_sub_thr + 1.0f);
    }
    break;
  }
  case LINEAR: {
    if(val_sub_thr < 0.0f)
      new_act = 0.0f;
    else
      new_act = val_sub_thr * act_spec.gain;
  }
  break;
  }
  return new_act;
}

void LeabraUnitSpec::Compute_ActFmVm_rate(LeabraUnit* u, LeabraNetwork* net) {
  float new_act;
  if(act.gelin) {
    if(u->v_m <= act.thr) {
      new_act = Compute_ActValFmVmVal_rate(u->v_m - act.thr);
    }
    else {
      float g_e_thr = Compute_EThresh(u);
      new_act = Compute_ActValFmVmVal_rate(u->net * g_bar.e - g_e_thr);
    }
    if(net->cycle < dt.vm_eq_cyc) {
      new_act = u->act_nd + dt.vm_eq_dt * (new_act - u->act_nd); // eq dt
    }
    else {
      new_act = u->act_nd + dt.vm * (new_act - u->act_nd); // time integral with dt.vm  -- use nd to avoid synd problems
    }
  }
  else {
    new_act = Compute_ActValFmVmVal_rate(u->v_m - act.thr);
  }
  if(depress.on) {                   // synaptic depression
    u->act_nd = act_range.Clip(new_act); // nd is non-discounted activation!!! solves tons of probs
    new_act *= MIN(u->spk_amp, 1.0f);
  }

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += Compute_Noise(u, net);
  }
  u->act = act_range.Clip(new_act);
  u->act_eq = u->act;
  if(!depress.on)
    u->act_nd = u->act_eq;
}

void LeabraUnitSpec::Compute_ActLrnFmVm_rate(LeabraUnit* u, LeabraNetwork* net) {
  if(!act_lrn_on) {
    u->act_lrn = u->act_eq;
    return;
  }
  float new_act_lrn;
  if(act_lrn.gelin) {
    if(u->v_m <= act_lrn.thr) {
      new_act_lrn = Compute_ActValFmVmVal_rate_impl(u->v_m - act_lrn.thr, act_lrn,
                                                    lrn_nxx1_fun);
    }
    else {
      float g_e_thr = Compute_EThreshLrn(u);
      new_act_lrn = Compute_ActValFmVmVal_rate_impl(u->net * g_bar.e - g_e_thr, act_lrn, 
                                                    lrn_nxx1_fun);
    }
    if(net->cycle < dt.vm_eq_cyc) {
      new_act_lrn = u->act_lrn + dt.vm_eq_dt * (new_act_lrn - u->act_lrn); // eq dt
    }
    else {
      new_act_lrn = u->act_lrn + dt.vm * (new_act_lrn - u->act_lrn);
    }
  }
  else {
    new_act_lrn = Compute_ActValFmVmVal_rate_impl(u->v_m - act_lrn.thr, act_lrn,
                                                  lrn_nxx1_fun);
  }
  u->act_lrn = act_range.Clip(new_act_lrn);
}

void LeabraUnitSpec::Compute_ActFmVm_spike(LeabraUnit* u, LeabraNetwork* net) {
  if(u->v_m > spike_misc.spk_thr) {
    u->act = 1.0f;
    u->v_m = spike_misc.vm_r;
    u->vm_dend += spike_misc.vm_dend;
    u->spk_t = net->ct_cycle;
  }
  else {
    u->act = 0.0f;
  }

  float old_nd = u->act_nd / spike.eq_gain;
  float new_nd;
  if(spike.eq_dt > 0.0f) {
    new_nd = act_range.Clip(spike.eq_gain * ((1.0f - spike.eq_dt) * old_nd + spike.eq_dt * u->act));
  }
  else {                        // increment by phase
    if(net->cycle > 0)
      old_nd *= (float)net->cycle;
    new_nd = act_range.Clip(spike.eq_gain * (old_nd + u->act) / (float)(net->cycle+1));
  }
  u->da = new_nd - u->act_nd;   // da is on equilibrium activation
  u->act_nd = new_nd;

  if(depress.on) {
    u->act *= MIN(u->spk_amp, 1.0f);    // after eq
    u->act_eq = u->spk_amp * u->act_nd; // act_eq is depressed rate code, nd is non-depressed!
  }
  else {
    u->act_eq = u->act_nd;      // eq = nd
  }
}

////////////////////////////////////////////////////////////////////
//              Self reg / adapt / depress

void LeabraUnitSpec::Compute_ActAdapt_Cycle_spike(LeabraUnit* u, LeabraNetwork* net) {
  if(!adapt.on) {
    u->adapt = 0.0f;
  }
  else {
    float dad = dt.integ * adapt.Compute_dAdapt(u->v_m, e_rev.l, u->adapt);
    if(u->act > 0.0f) {                                               // spiked
      dad += adapt.spike_gain;
    }
    u->adapt += dad;
  }
}

void LeabraUnitSpec::Compute_ActAdapt_Cycle_rate(LeabraUnit* u, LeabraNetwork* net) {
  if(!adapt.on) {
    u->adapt = 0.0f;
  }
  else {
    float dad = dt.integ * adapt.Compute_dAdapt(0.0f, u->act, u->adapt);
    // vm_gain is now driving based on (0 - act) -- the more active, the longer it is in refractory, and thus the longer the adapt goes down..
    if(net->ct_cycle % adapt.interval == 0) {
      dad += u->act * adapt.spike_gain; // rate code version of spiking
    }
    u->adapt += dad;
  }
}

void LeabraUnitSpec::Compute_ActAdapt_Trial_rate(LeabraUnit* u, LeabraNetwork* net) {
  if(!adapt.on) {
    u->adapt = 0.0f;
  }
  else {
    float dad = dt.integ * adapt.Compute_dAdapt(0.0f, u->act, u->adapt);
    // vm_gain is now driving based on (0 - act) -- the more active, the longer it is in refractory, and thus the longer the adapt goes down..
    if(net->trial % adapt.interval == 0) {
      dad += u->act * adapt.spike_gain; // rate code version of spiking
    }
  }
}

void LeabraUnitSpec::Compute_Depress_Cycle(LeabraUnit* u, LeabraNetwork* net) {
  if(!depress.on) {
    u->spk_amp = depress.max_amp;
  }
  else {
    if(net->ct_cycle < depress.delay) {
      if((net->ct_cycle+1) % depress.interval == 0) {
        u->spk_amp += (depress.max_amp - u->spk_amp) * depress.rec; // recover only
      }
    }
    else {
      if(((net->ct_cycle-depress.delay)+1) % depress.interval == 0) {
        u->spk_amp += -u->act * depress.depl +
          (depress.max_amp - u->spk_amp) * depress.rec;
      }
    }
    if(u->spk_amp < 0.0f)                       u->spk_amp = 0.0f;
    else if(u->spk_amp > depress.max_amp)       u->spk_amp = depress.max_amp;
  }
}

void LeabraUnitSpec::Compute_Depress_Trial(LeabraUnit* u, LeabraNetwork* net) {
  if(!depress.on) {
    u->spk_amp = depress.max_amp;
  }
  else {
    if((net->trial+1) % depress.interval == 0) {
      u->spk_amp += -u->act * depress.depl + (depress.max_amp - u->spk_amp) * depress.rec;
    }
    if(u->spk_amp < 0.0f)                       u->spk_amp = 0.0f;
    else if(u->spk_amp > depress.max_amp)       u->spk_amp = depress.max_amp;
  }
}

void LeabraUnitSpec::Compute_SelfReg_Cycle(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun == SPIKE) {
    Compute_ActAdapt_Cycle_spike(u, net);
    Compute_Depress_Cycle(u, net);
  }
  else {
    if(!adapt.trials) {
      Compute_ActAdapt_Cycle_rate(u, net);
    }
    if(!depress.trials) {
      Compute_Depress_Cycle(u, net);
    }
  }

  // fast time scale updated every cycle
  if(hyst.on && !hyst.trl)
    hyst.UpdateBasis(u->vcb.hyst, u->vcb.hyst_on, u->vcb.g_h, u->act_eq);
  if(acc.on && !acc.trl)
    acc.UpdateBasis(u->vcb.acc, u->vcb.acc_on, u->vcb.g_a, u->act_eq);
}

void LeabraUnitSpec::Compute_SelfReg_Trial(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun != SPIKE) {
    if(adapt.trials) {
      Compute_ActAdapt_Trial_rate(u, net);
    }
    if(depress.trials) {
      Compute_Depress_Trial(u, net);
    }
  }

  // slow time scale updated at end of trial
  if(hyst.on && hyst.trl)
    hyst.UpdateBasis(u->vcb.hyst, u->vcb.hyst_on, u->vcb.g_h, u->act_eq);
  if(acc.on && acc.trl)
    acc.UpdateBasis(u->vcb.acc, u->vcb.acc_on, u->vcb.g_a, u->act_eq);
}

float LeabraUnitSpec::Compute_Noise(LeabraUnit* u, LeabraNetwork* net) {
  float rval = 0.0f;
  if(noise_adapt.trial_fixed) {
    rval = u->noise; // u->noise is trial-level generated value
  }
  else {
    rval = noise.Gen();
    u->noise = rval;
  }

  if(noise_adapt.mode == NoiseAdaptSpec::SCHED_CYCLES) {
    rval *= noise_sched.GetVal(net->cycle);
  }
  else if(noise_adapt.mode == NoiseAdaptSpec::SCHED_EPOCHS) {
    rval *= noise_sched.GetVal(net->epoch);
  }
  else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_PVI) {
    rval *= (1.0f - (noise_adapt.min_pct_c * net->pvlv_pvi));
  }
  else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_LVE) {
    rval *= (1.0f - (noise_adapt.min_pct_c * net->pvlv_lve));
  }
  else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_MIN) {
    float pvlv_val = MIN(net->pvlv_pvi, net->pvlv_lve);
    rval *= (1.0f - (noise_adapt.min_pct_c * pvlv_val));
  }

  return rval;
}

void LeabraUnitSpec::Compute_SRAvg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  if(!lay) return;
  if(lay->sravg_vals.state == CtSRAvgVals::NO_SRAVG) return; // don't

  float ru_act; // activation to use for updating averages
  if(act_avg.use_nd || net->learn_rule == LeabraNetwork::CTLEABRA_CAL || 
     net->learn_rule == LeabraNetwork::LEABRA_CHL) {
    ru_act = u->act_nd;
  }
  else {
    ru_act = u->act_lrn;        // note: using lrn here..
    LeabraUnitSpec* rus = (LeabraUnitSpec*)u->GetUnitSpec();
    if(rus->act_fun == LeabraUnitSpec::SPIKE)
      ru_act *= rus->spike.eq_gain;
  }

  bool do_s = false;

  // if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL_C) {
  //   // note: this is cascade version -- each builds upon the other
  //   u->avg_ss += act_avg.ss_dt * (ru_act - u->avg_ss);
  //   float ds = act_avg.s_dt * (u->avg_ss - u->avg_s);
  //   u->avg_s += ds;
  //   u->davg += net->ct_lrn_trig.davg_dt * (ds - u->davg);

  //   u->avg_m += act_avg.m_dt * (u->avg_s - u->avg_m);

  //   float lval = u->avg_m; // note this is *avg_m* not act_m..
  //   if(lval > u->avg_l)
  //     u->avg_l += act_avg.l_up_dt * (lval - u->avg_l);
  //   else
  //     u->avg_l += act_avg.l_dn_dt * (lval - u->avg_l);
  // }
  // else {

  // use continuous updating so these are always current -- no need for post-average step
  if(lay->sravg_vals.state == CtSRAvgVals::SRAVG_M ||
     lay->sravg_vals.state == CtSRAvgVals::SRAVG_SM) {
    if(lay->sravg_vals.m_sum == 1.0f) {
      u->avg_m = ru_act;
    }
    else {
      u->avg_m = (ru_act + u->avg_m * (lay->sravg_vals.m_sum - 1.0f)) /
        lay->sravg_vals.m_sum;
    }
  }

  if(lay->sravg_vals.state == CtSRAvgVals::SRAVG_S ||
     lay->sravg_vals.state == CtSRAvgVals::SRAVG_SM) {
    do_s = true;
    if(lay->sravg_vals.s_sum == 1.0f) {
      u->avg_s = ru_act;
    }
    else {
      u->avg_s = (ru_act + u->avg_s * (lay->sravg_vals.s_sum - 1.0f)) /
        lay->sravg_vals.s_sum;
    }
  }
}

void LeabraUnitSpec::Compute_SRAvg_Cons(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  bool do_s = false;
  if(lay->sravg_vals.state == CtSRAvgVals::SRAVG_S ||
     lay->sravg_vals.state == CtSRAvgVals::SRAVG_SM) {
    do_s = true;
  }
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    if(!rlay->Compute_SRAvg_Test(net)) continue;
    send_gp->Compute_SRAvg(u, net, do_s);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

float LeabraUnitSpec::Compute_MaxDa(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* lay = u->own_lay();
  float fda = 0.0f;
  switch(maxda.val) {
  case MaxDaSpec::NO_MAX_DA:
    break;
  case MaxDaSpec::DA_ONLY:
    fda = fabsf(u->da);
    break;
  case MaxDaSpec::INET_ONLY:
    fda = fabsf(maxda.inet_scale * u->I_net);
    break;
  case MaxDaSpec::INET_DA:
    if(lay->acts.avg <= maxda.lay_avg_thr)
      fda = fabsf(maxda.inet_scale * u->I_net);
    else
      fda = fabsf(u->da);
    break;
  }
  return fda;
}

///////////////////////////////////////////////////////////////////////
//      Cycle Optional Misc

void LeabraUnitSpec::Compute_MidMinus(LeabraUnit* u, LeabraNetwork* net) {
  u->act_mid = u->act_eq;
}

void LeabraUnitSpec::Compute_CycSynDep(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    send_gp->Compute_CycSynDep(u,net);
  }
}


///////////////////////////////////////////////////////////////////////
//      Phase and Trial Activation Updating

void LeabraUnitSpec::PostSettle(LeabraUnit* u, LeabraNetwork* net) {
  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  float use_act;
  if(depress.phase_act == DepressSpec::ACT_ND)
    use_act = u->act_nd;
  else if(depress.phase_act == DepressSpec::ACT_EQ)
    use_act = u->act_eq;
  else                          // ACT_LRN
    use_act = u->act_lrn;
    

  switch(net->phase_order) {
  case LeabraNetwork::MINUS_PLUS:
    if(no_plus_testing) {
      u->net_ctxt = 0.0f;
      u->deep5b_net = 0.0f;
      u->p_act_p = u->act_p;
      u->act_m = u->act_p = use_act;
      u->act_dif = 0.0f;
      Compute_ActTimeAvg(u, net);
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE)
        u->act_m = use_act;
      else {
        u->net_ctxt = 0.0f;
        u->deep5b_net = 0.0f;
        u->p_act_p = u->act_p;
        u->act_p = use_act;
        u->act_dif = u->act_p - u->act_m;
        Compute_DaMod_PlusPost(u, net);
        Compute_ActTimeAvg(u, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_ONLY:
    u->net_ctxt = 0.0f;
    u->deep5b_net = 0.0f;
    u->p_act_p = u->act_p;
    u->act_m = u->act_p = use_act;
    u->act_dif = 0.0f;
    Compute_ActTimeAvg(u, net);
    break;
  }
}

void LeabraUnitSpec::Compute_ActTimeAvg(LeabraUnit* u, LeabraNetwork* net) {
  if(act.avg_dt <= 0.0f) return;
  u->act_avg += act.avg_dt * (u->act_eq - u->act_avg);
}

void LeabraUnitSpec::Compute_DaMod_PlusPost(LeabraUnit* u, LeabraNetwork* net) {
  if(!da_mod.on || (da_mod.mod != DaModSpec::PLUS_POST)) return;
  float dact = da_mod.gain * u->dav * u->act_m; // delta activation
  if(dact > 0.0f) {
    dact *= 1.0f - u->act_p;
  }
  else {
    dact *= u->act_p;
  }
  u->act_p = act_range.Clip(u->act_p + dact);
}

/////////////////////////////////////////////////
//              Leabra TI

void LeabraUnitSpec::TI_Compute_Deep5bAct(LeabraUnit* u, LeabraNetwork* net) {
  if(cifer.on) {
    if(!cifer.phase_updt && net->phase_no == 0) return;
    if(u->thal >= cifer.thal_5b_thr && u->act_eq >= cifer.act_5b_thr) {
      if(cifer.binary5b) {
        u->deep5b = 1.0f;
      }
      else {
        u->deep5b = u->thal * u->act_eq;
      }
    }
    else {
      u->deep5b = 0.0f;
    }
  }
  else {
    if(net->phase_no == 0) return; // never in minus for regular TI
    u->deep5b = u->act_eq;         // compatible with std TI
  }
}

void LeabraUnitSpec::TI_Send_Deep5bNetin(LeabraUnit* u, LeabraNetwork* net,
                                         int thread_no) {
  float act_ts = u->deep5b;

  if(act_ts > opt_thresh.send) {
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(!((LeabraConSpec*)send_gp->GetConSpec())->IsDeep5bCon()) continue;
      Deep5bConSpec* sp = (Deep5bConSpec*)send_gp->GetConSpec();
      // if(!tol->TI_UpdateContextTest(net)) continue;
      sp->Send_Deep5bNetin(send_gp, net, thread_no, act_ts);
    }
  }
}

void LeabraUnitSpec::TI_Send_Deep5bNetin_Post(LeabraUnit* u, LeabraNetwork* net,
                                         int thread_no) {
  int nt = net->lthreads.n_threads_act;
  float nw_nt = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->send_netin_tmp.FastEl2d(u->flat_idx, j);
    nw_nt += ndval;
    ndval = 0.0f;             // zero immediately..
  }
  u->deep5b_net = nw_nt;
}

void LeabraUnitSpec::TI_Send_CtxtNetin(LeabraUnit* u, LeabraNetwork* net,
                                       int thread_no) {
  float act_ts = u->act_eq;
  if(cifer.on) {
    act_ts *= (1.0f - cifer.ti_5b);
    act_ts += cifer.ti_5b * u->deep5b;
  }

  if(act_ts > opt_thresh.send) {
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(!((LeabraConSpec*)send_gp->GetConSpec())->IsTICtxtCon()) continue;
      LeabraTICtxtConSpec* sp = (LeabraTICtxtConSpec*)send_gp->GetConSpec();
      if(!tol->TI_UpdateContextTest(net)) continue;
      sp->Send_CtxtNetin(send_gp, net, thread_no, act_ts);
    }
  }
}

void LeabraUnitSpec::TI_Send_CtxtNetin_Post(LeabraUnit* u, LeabraNetwork* net,
                                            int thread_no) {
  int nt = net->lthreads.n_threads_act;
  float nw_nt = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->send_netin_tmp.FastEl2d(u->flat_idx, j);
    nw_nt += ndval;
    ndval = 0.0f;             // zero immediately..
  }
  u->net_ctxt = nw_nt;
}

void LeabraUnitSpec::TI_Compute_CtxtAct(LeabraUnit* u, LeabraNetwork* net) {
  u->act_ctxt = u->net_ctxt;  // straight pass-through -- gating will need to modulate this
}

void LeabraUnitSpec::TI_ClearContext(LeabraUnit* u, LeabraNetwork* net) {
  u->deep5b = 0.0f;
  u->deep5b_net = 0.0f;
  u->act_ctxt = 0.0f;
  u->net_ctxt = 0.0f;
  u->p_act_p = 0.0f;
}


//////////////////////////////////////////
//      Stage 6: Learning               //
//////////////////////////////////////////

void LeabraUnitSpec::Compute_dWt(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  for(int g = 0; g < u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    if(!rlay->Compute_dWt_Test(net))
      continue;
    send_gp->Compute_dWt(u,net);
  }

  LeabraLayer* lay = u->own_lay();
  if(!lay->Compute_dWt_Test(net)) return; // applies to bias weights now

  LeabraConSpec* bspc = ((LeabraConSpec*)bias_spec.SPtr());
  bspc->B_Compute_dWt(&u->bias, u, lay);
}

void LeabraUnitSpec::Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return;
  for(int g = 0; g < u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->NotActive()) continue;
    recv_gp->Compute_dWt_Norm(u,net);
  }
}

void LeabraUnitSpec::Compute_Weights(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;

  for(int g = 0; g < u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    send_gp->Compute_Weights(u, net);
  }
  LeabraConSpec* bspc = ((LeabraConSpec*)bias_spec.SPtr());
  bspc->B_Compute_Weights(&u->bias, u);
}

void LeabraUnitSpec::Compute_StableWeights(LeabraUnit* u, LeabraNetwork* net,
                                           int thread_no) {
  LeabraLayer* olay = u->own_lay();
  if(olay->lesioned()) return;
  for(int g = 0; g < u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
    cs->Compute_StableWeights(send_gp, u, net);
  }

  LeabraConSpec* bspc = ((LeabraConSpec*)bias_spec.SPtr());
  bspc->C_Compute_StableWeights(u->bias.OwnCn(0,LeabraConSpec::WT),
                                u->bias.OwnCn(0,LeabraConSpec::SWT),
                                u->bias.OwnCn(0,LeabraConSpec::LWT));
}


///////////////////////////////////////////////////////////////////////
//      Stats

float LeabraUnitSpec::Compute_SSE(Unit* ru, Network* rnet, bool& has_targ) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  // just replaces act_m for act in original
  float sse = 0.0f;
  has_targ = false;
  if(u->HasExtFlag(Unit::TARG | Unit::COMP)) {
    has_targ = true;
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) >= sse_tol)
      sse = uerr * uerr;
  }
  return sse;
}

bool LeabraUnitSpec::Compute_PRerr(Unit* ru, Network* rnet, float& true_pos,
                                   float& false_pos, float& false_neg, float& true_neg) {
  // just replaces act_m for act in original
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f;
  bool has_targ = false;
  if(u->HasExtFlag(Unit::TARG | Unit::COMP)) {
    has_targ = true;
    // float uerr = u->targ - u->act_m;
    // if(fabsf(uerr) < sse_tol) {
    //   true_pos = u->targ;
    // }
    // else {
      if(u->targ > u->act_m) {
        true_pos = u->act_m;
	true_neg = 1.0 - u->targ;
        false_neg = u->targ - u->act_m;
      }
      else {
        true_pos = u->targ;
        false_pos = u->act_m - u->targ;
	true_neg = 1.0 - u->act_m;
      }
    // }
  }
  return has_targ;
}

float LeabraUnitSpec::Compute_NormErr(LeabraUnit* u, LeabraNetwork* net) {
  if(!u->HasExtFlag(Unit::TARG | Unit::COMP)) return 0.0f;

  if(net->on_errs) {
    if(u->act_m > 0.5f && u->targ < 0.5f) return 1.0f;
  }
  if(net->off_errs) {
    if(u->act_m < 0.5f && u->targ > 0.5f) return 1.0f;
  }
  return 0.0f;
}

//////////////////////////////////////////
//       Misc Functions                 //
//////////////////////////////////////////

void LeabraUnitSpec::BioParams(bool gelin, float norm_sec, float norm_volt, float volt_off, float norm_amp,
          float C_pF, float gbar_l_nS, float gbar_e_nS, float gbar_i_nS,
          float erev_l_mV, float erev_e_mV, float erev_i_mV,
          float act_thr_mV, float spk_thr_mV, float exp_slope_mV,
       float adapt_dt_time_ms, float adapt_vm_gain_nS, float adapt_spk_gain_nA)
{
  // derived units
  float norm_siemens = norm_amp / norm_volt;
  float norm_farad = (norm_sec * norm_amp) / norm_volt;

  dt.vm = 1.0f / ((C_pF * 1.0e-12f) / norm_farad);

  g_bar.l = (gbar_l_nS * 1.0e-9f) / norm_siemens;
  g_bar.e = (gbar_e_nS * 1.0e-9f) / norm_siemens;
  g_bar.i = (gbar_i_nS * 1.0e-9f) / norm_siemens;

  e_rev.l = ((erev_l_mV * 1.0e-3f) - volt_off) / norm_volt;
  e_rev.e = ((erev_e_mV * 1.0e-3f) - volt_off) / norm_volt;
  e_rev.i = ((erev_i_mV * 1.0e-3f) - volt_off) / norm_volt;

  act.thr = ((act_thr_mV* 1.0e-3f) - volt_off) / norm_volt;
  spike_misc.spk_thr = ((spk_thr_mV * 1.0e-3f) - volt_off) / norm_volt;
  spike_misc.exp_slope = ((exp_slope_mV * 1.0e-3f)) / norm_volt; // no off!
  spike_misc.vm_r = e_rev.l;                                     // go back to leak

  adapt.dt = 1.0f / ((adapt_dt_time_ms * 1.0e-3f) / norm_sec);
  adapt.vm_gain = (adapt_vm_gain_nS * 1.0e-9f) / norm_siemens;
  adapt.spike_gain = (adapt_spk_gain_nA * 1.0e-9f) / norm_amp;

  v_m_init.mean = e_rev.l;
  vm_range.min = 0.0f;
  vm_range.max = 2.0f;
  dt.d_vm_max = 100.0f;         // no max

  if(gelin) {
    dt.vm = 0.3f;
    act.gain = 100;
    act.gelin = true;
  }

  UpdateAfterEdit();
}

void LeabraUnitSpec::GraphVmFun(DataTable* graph_data, float g_i, float min, float max, float incr) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_VmFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Vm", idx, VT_FLOAT);
  
  incr = MAX(0.001f, incr);	// must be pos

  for(float x = min; x <= max; x += incr) {
    float y = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
      ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(y, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::GraphActFmVmFun(DataTable* graph_data, float min, float max, float incr) {
  if(TestError(act.gelin, "GraphActFmVmFun", "cannot graph act from vm when act.gelin = true, because act does not depend on vm in this case!  Use GraphActFmNetFun instead."))
    return;

  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFmVmFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Vm", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Act", idx, VT_FLOAT);

  LeabraUnit un;
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  incr = MAX(0.001f, incr);	// must be pos
  float x;
  for(x = min; x <= max; x += incr) {
    un.v_m = x;
    Compute_ActFmVm(&un, net);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::GraphActFmNetFun(DataTable* graph_data, float g_i, float min, float max,
                                      float incr, float g_e_thr, float lin_gain) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFmNetFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* av = graph_data->FindMakeColName("Act", idx, VT_FLOAT);
  DataCol* lin = graph_data->FindMakeColName("Linear", idx, VT_FLOAT);

  av->SetUserData("PLOT_1", true);
  av->SetUserData("MIN", 0.0f);
  av->SetUserData("MAX", 1.0f);

  lin->SetUserData("PLOT_2", true);

//  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  incr = MAX(0.001f, incr);	// must be pos
  float x;
  for(x = min; x <= max; x += incr) {
    float aval;
    if(act.gelin) {
      aval = Compute_ActValFmVmVal_rate(x - g_e_thr);
    }
    else {
      float v_m = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
        ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
      aval = Compute_ActValFmVmVal_rate(v_m - act.thr);
    }
    float ln = x - g_e_thr;
    if(ln < 0.0f) ln = 0.0f;
    ln *= lin_gain;
    if(ln > 1.0f) ln = 1.0f;
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    av->SetValAsFloat(aval, -1);
    lin->SetValAsFloat(ln, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::TimeExp(int mode, int nreps) {

  const char* modes[6] = {"double sum", "double exp", "double exp_fast",
                          "float sum", "float exp", "float exp_fast"};

  TimeUsedHR tu;
  tu.StartTimer(true);          // start, w/reset
  double dsum = 0.0;
  float  fsum = 0.0f;
  switch (mode) {
  case 0: {
    for(int i=0;i<nreps; i++) {
      dsum += Random::UniformMinMax(-50.0, 50.0) - 0.99 * dsum;
    }
    break;
  }
  case 1: {
    for(int i=0;i<nreps; i++) {
      dsum += taMath_double::exp(Random::UniformMinMax(-50.0, 50.0)) - 0.99 * dsum;
    }
    break;
  }
  case 2: {
    for(int i=0;i<nreps; i++) {
      dsum += taMath_double::exp_fast(Random::UniformMinMax(-50.0, 50.0)) - 0.99 * dsum;
    }
    break;
  }
  case 3: {
    for(int i=0;i<nreps; i++) {
      fsum += Random::UniformMinMax(-50.0, 50.0) - 0.99f * fsum;
    }
    break;
  }
  case 4: {
    for(int i=0;i<nreps; i++) {
      fsum += taMath_float::exp(Random::UniformMinMax(-50.0, 50.0)) - 0.99f * fsum;
    }
    break;
  }
  case 5: {
    for(int i=0;i<nreps; i++) {
      fsum += taMath_float::exp_fast(Random::UniformMinMax(-50.0, 50.0)) - 0.99f * fsum;
    }
    break;
  }
  }
  tu.EndTimer();
  String msg;
  msg << "mode: " << mode << " " << modes[mode] << " seconds used: " << tu.s_used
      << " dsum: " << dsum << " fsum: " << fsum;
  taMisc::Info(msg);
}

void LeabraUnitSpec::GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_SpikeAlphFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* t = graph_data->FindMakeColName("time_fm_spike", idx, VT_FLOAT);
  DataCol* g = graph_data->FindMakeColName("conductance", idx, VT_FLOAT);

  float tmax = MAX(spike.window, 2.0f);

  float sumg = 0.0f;
  float x;
  if(!force_alpha && spike.rise == 0.0f && spike.decay > 0.0f) {
    float net = 0.0f;
    float input;
    tmax = 20.0f;
    for(x = 0.0f; x <= tmax; x += 1.0f) {
      if(x < spike.window)
        input = spike.gg_decay / (float)spike.window;
      else
        input = 0.0f;
      net = net + input - net / spike.decay;
      graph_data->AddBlankRow();
      t->SetValAsFloat(x, -1);
      g->SetValAsFloat(net, -1);
      sumg += net;
    }
  }
  else {
    for(x = 0.0f; x <= tmax; x += 1.0f) {
      float y = spike.ComputeAlpha(x);
      graph_data->AddBlankRow();
      t->SetValAsFloat(x, -1);
      g->SetValAsFloat(y, -1);
      sumg += y;
    }
  }
  graph_data->AddBlankRow();
  t->SetValAsFloat(x, -1);
  g->SetValAsFloat(0.0f, -1);
  graph_data->AddBlankRow();
  t->SetValAsFloat(x+1.0f, -1);
  g->SetValAsFloat(sumg, -1);
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

// void LeabraUnitSpec::GraphSLNoiseAdaptFun(DataTable* graph_data, float incr) {
//   taProject* proj = GET_MY_OWNER(taProject);
//   if(!graph_data) {
//     graph_data = proj->GetNewAnalysisDataTable(name + "_SLNoiseAdaptFun", true);
//   }
//   int idx;
//   graph_data->StructUpdate(true);
//   graph_data->ResetData();
//   DataCol* lt = graph_data->FindMakeColName("LongTerm", idx, VT_FLOAT);
//   lt->SetUserData("X_AXIS", true);
//   DataCol* st = graph_data->FindMakeColName("ShortTerm", idx, VT_FLOAT);
//   st->SetUserData("Z_AXIS", true);
//   DataCol* nv = graph_data->FindMakeColName("NoiseVarMult", idx, VT_FLOAT);
//   nv->SetUserData("PLOT_1", true);
//   nv->SetUserData("MIN", 0.0f);
//   nv->SetUserData("MAX", 1.0f);

//   LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

//   for(float ltv = 0.0f; ltv <= 1.0f; ltv += incr) {
//     for(float stv = 0.0f; stv <= 1.0f; stv += incr) {
//       float nvar = noise_adapt.SLNoiseFun(stv, ltv);
//       graph_data->AddBlankRow();
//       lt->SetValAsFloat(ltv, -1);
//       st->SetValAsFloat(stv, -1);
//       nv->SetValAsFloat(nvar, -1);
//     }
//   }
//   taDataAnal::Matrix3DGraph(graph_data, "LongTerm", "ShortTerm");
//   graph_data->SetUserData("NEG_DRAW", false);
//   graph_data->SetUserData("NEG_DRAW_Z", false);
//   graph_data->StructUpdate(false);
//   graph_data->FindMakeGraphView();
// }


