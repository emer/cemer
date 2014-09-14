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
TA_BASEFUNS_CTORS_DEFN(LeabraActMiscSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeFunSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeMiscSpec);
TA_BASEFUNS_CTORS_DEFN(OptThreshSpec);
TA_BASEFUNS_CTORS_DEFN(ActAdaptSpec);
TA_BASEFUNS_CTORS_DEFN(ShortPlastSpec);
TA_BASEFUNS_CTORS_DEFN(SynDelaySpec);
TA_BASEFUNS_CTORS_DEFN(LeabraDtSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActAvgSpec);
TA_BASEFUNS_CTORS_DEFN(CIFERSpec);
TA_BASEFUNS_CTORS_DEFN(DaModSpec);
TA_BASEFUNS_CTORS_DEFN(NoiseAdaptSpec);

SMARTREF_OF_CPP(LeabraUnitSpec);


void LeabraActFunSpec::Initialize() {
  Defaults_init();
}

void LeabraActFunSpec::Defaults_init() {
  thr = 0.5f;
  gain = 100.0f;
  nvar = 0.005f;
}

void LeabraActFunSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(owner) owner->UpdateAfterEdit(); // update our unitspec so it can recompute lookup function!
}


void LeabraActMiscSpec::Initialize() {
  act_lrn = ACT_EQ;
  Defaults_init();
}

void LeabraActMiscSpec::Defaults_init() {
  act_max_hz = 100.0f;
  avg_tau = 200.0f;
  avg_init = 0.15f;
  rescale_ctxt = true;

  avg_dt = 1.0f / avg_tau;
}

void LeabraActMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  avg_dt = 1.0f / avg_tau;
}  

void SpikeFunSpec::Initialize() {
  g_gain = 9.0f;
  rise = 0.0f;
  decay = 5.0f;
  window = 3;
  eq_gain = 8.0f;
  eq_tau = 50.0f;

  gg_decay = g_gain / decay;
  gg_decay_sq = g_gain / (decay * decay);
  gg_decay_rise = g_gain / (decay - rise);

  oneo_decay = 1.0f / decay;
  if(rise > 0.0f)
    oneo_rise = 1.0f / rise;
  else
    oneo_rise = 1.0f;

  if(eq_tau > 0.0f)
    eq_dt = 1.0f / eq_tau;
  else
    eq_dt = 0.0f;
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
  if(eq_tau > 0.0f)
    eq_dt = 1.0f / eq_tau;
  else
    eq_dt = 0.0f;
}

void SpikeMiscSpec::Initialize() {
  exp_slope = 0.02f;
  spk_thr = 1.2f;
  clamp_max_p = 0.12f;
  clamp_type = REGULAR;
  vm_r = 0.30f;
  t_r = 6;
}

void SpikeMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void OptThreshSpec::Initialize() {
  send = .1f;
  delta = 0.005f;
  xcal_lrn = 0.01f;
}

void LeabraDtSpec::Initialize() {
  fast_cyc = 0;
  Defaults_init();
}

void LeabraDtSpec::Defaults_init() {
  integ = 1.0f;
  vm_tau = 3.3f;
  net_tau = 1.4f;
  midpoint = false;

  vm_dt = 1.0f / vm_tau;
  net_dt = 1.0f / net_tau;
}

void LeabraDtSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  vm_dt = 1.0f / vm_tau;
  net_dt = 1.0f / net_tau;
}

void LeabraActAvgSpec::Initialize() {
  l_up_inc = 0.2f;
  l_dn_tau = 2.5f;
  ss_tau = 1.0f;
  s_tau = 5.0f;
  m_tau = 10.0f;
  
  l_dn_dt = 1.0f / l_dn_tau;
  ss_dt = 1.0f / ss_tau;
  s_dt = 1.0f / s_tau;
  m_dt = 1.0f / m_tau;
}

void LeabraActAvgSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  l_dn_dt = 1.0f / l_dn_tau;
  ss_dt = 1.0f / ss_tau;
  s_dt = 1.0f / s_tau;
  m_dt = 1.0f / m_tau;
}

void LeabraChannels::Initialize() {
  e = l = i = 0.0f;
}

void LeabraChannels::Copy_(const LeabraChannels& cp) {
  e = cp.e;
  l = cp.l;
  i = cp.i;
}

void ActAdaptSpec::Initialize() {
  on = false;
  Defaults_init();
}

void ActAdaptSpec::Defaults_init() {
  tau = 144;
  vm_gain = 0.04f;
  spike_gain = 0.00805f;

  dt = 1.0f / tau;
}

void ActAdaptSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt = 1.0f / tau;
}

void ShortPlastSpec::Initialize() {
  on = false;
  f_r_ratio = 0.02f;
  p0 = 0.3f;
  p0_norm = 0.4f;
  kre = 0.002f;
  Defaults_init();
}

void ShortPlastSpec::Defaults_init() {
  rec_tau = 200.0f;
  fac = 0.3f;
  kre_tau = 100.0f;

  fac_tau = f_r_ratio * rec_tau;
  rec_dt = 1.0f / rec_tau;
  fac_dt = 1.0f / fac_tau;
  kre_dt = 1.0f / kre_tau;
  oneo_p0_norm = 1.0f / p0_norm;
}

void ShortPlastSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  fac_tau = f_r_ratio * rec_tau;
  rec_dt = 1.0f / rec_tau;
  fac_dt = 1.0f / fac_tau;
  kre_dt = 1.0f / kre_tau;
  oneo_p0_norm = 1.0f / p0_norm;
}

void SynDelaySpec::Initialize() {
  on = false;
  delay = 4;
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
  mode = FIXED_NOISE;
  Defaults_init();
}

void NoiseAdaptSpec::Defaults_init() {
}

void NoiseAdaptSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void LeabraUnitSpec::Initialize() {
  min_obj_type = &TA_LeabraUnit;
  bias_con_type = &TA_LeabraCon;
  bias_spec.SetBaseType(&TA_LeabraBiasSpec);

  act_fun = NOISY_XX1;

  noise_type = NO_NOISE;
  noise.type = Random::GAUSSIAN;
  noise.var = .001f;

  Defaults_init();

  CreateNXX1Fun(act, nxx1_fun, noise_conv);
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
  e_rev.e = 1.0f;
  e_rev.l = 0.3f;
  e_rev.i = 0.25f;

  v_m_init.type = Random::NONE;
  v_m_init.mean = 0.4f; // e_rev.l;  // 0.4f works sig better in many cases..
  v_m_init.var = 0.0f;

  act_init.type = Random::NONE;
  act_init.mean = 0.0f;
  act_init.var = 0.0f;

  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);
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

  act_misc.UpdateAfterEdit_NoGui();
  spike.UpdateAfterEdit_NoGui();
  spike_misc.UpdateAfterEdit_NoGui();
  clamp_range.UpdateAfterEdit_NoGui();
  vm_range.UpdateAfterEdit_NoGui();
  dt.UpdateAfterEdit_NoGui();
  act_avg.UpdateAfterEdit_NoGui();
  adapt.UpdateAfterEdit_NoGui();
  stp.UpdateAfterEdit_NoGui();
  noise_sched.UpdateAfterEdit_NoGui();
  noise_adapt.UpdateAfterEdit_NoGui();
  CreateNXX1Fun(act, nxx1_fun, noise_conv);

  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);
}

void LeabraUnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(net) {
    if(dt.integ != 1000.0f * net->time_inc) {
      taMisc::Warning("unit time integration constant dt.integ of:", (String)dt.integ,
                      "does not match network time_inc increment of:",
                      (String)net->time_inc, "time_inc should be 0.001 * dt.integ");
    }
  }
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
  nxx1_fl.x_range.min = -.1f;
  nxx1_fl.x_range.max = 1.0f;
  nxx1_fl.res = .001f;       // needs same fine res to get the noise transitions
  nxx1_fl.UpdateAfterEdit_NoGui();

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

  u->act_avg = act_misc.avg_init;
  u->misc_1 = 0.0f;

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
  u->act_avg = act_misc.avg_init;
  u->avg_l = act_misc.avg_init;
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
  // u->gc_i = 0.0f;

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

  u->act = act_init.Gen();
  u->net = 0.0f;               // these are not done in netins -- need to nuke

  u->act_eq = u->act;
  u->act_nd = u->act_eq;
  u->act_lrn = u->act_eq;
  u->spike = 0.0f;
  u->act_p = u->act_m = u->act_dif = 0.0f;
  u->act_mid = 0.0f;

  u->da = 0.0f;
  u->avg_ss = act_misc.avg_init;
  u->avg_s = act_misc.avg_init;
  u->avg_m = act_misc.avg_init;
  // not avg_l
  // not act_avg
  u->thal = 0.0f;
  u->deep5b = 0.0f;
  u->deep5b_net = 0.0f;
  u->act_ctxt = 0.0f;
  u->net_ctxt = 0.0f;
  u->p_act_p = 0.0f;
  u->gc_i = 0.0f;
  u->gc_l = 0.0f;
  u->I_net = 0.0f;
  u->v_m = v_m_init.Gen();
  u->adapt = 0.0f;
  u->syn_tr = 1.0f;
  u->syn_nr = 1.0f;
  u->syn_pr = stp.p0;
  u->syn_kre = 0.0f;
  u->noise = 0.0f;
  u->dav = 0.0f;
  u->sev = 0.0f;

  // not the scales
  // init netin gets act_sent, net_raw, net_delta etc
  u->g_i_syn = 0.0f;
  u->g_i_self = 0.0f;

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
  if(decay > 0.0f) {            // no need to reset netin if not decaying at all
    Init_Netins(u, net);
    u->act -= decay * (u->act - act_init.mean);
    u->net -= decay * u->net;
    u->act_eq -= decay * (u->act_eq - act_init.mean);
    u->act_nd -= decay * (u->act_nd - act_init.mean);
    u->act_lrn -= decay * (u->act_lrn - act_init.mean);
    u->p_act_p -= decay * (u->p_act_p - act_init.mean); // reset this for ti guys
    u->gc_i -= decay * u->gc_i;
    u->v_m -= decay * (u->v_m - v_m_init.mean);
    if(adapt.on) {
      u->adapt -= decay * u->adapt;
    }
    if(stp.on) {
      u->syn_tr -= decay * (u->syn_tr - 1.0f);
      u->syn_nr -= decay * (u->syn_nr - 1.0f);
      u->syn_pr -= decay * (u->syn_tr - stp.p0);
      u->syn_kre -= decay * u->syn_kre;
    }

    u->g_i_syn -= decay * u->g_i_syn;
    u->g_i_self -= decay * u->g_i_self;

    // note: this is causing a problem in learning with xcal:
    //   u->avg_ss -= decay * (u->avg_ss - act_misc.avg_init);
    //   u->avg_s -= decay * (u->avg_s - act_misc.avg_init);
    //   u->avg_m -= decay * (u->avg_m - act_misc.avg_init);
    // not avg_l, act_avg, thal, deep5b*, act_ctxt* 
  }

  u->da = 0.0f;
  u->I_net = 0.0f;

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
  if(noise_type != NO_NOISE && noise_adapt.trial_fixed &&
     (noise.type != Random::NONE)) {
    u->noise = noise.Gen();
  }
}

void LeabraUnitSpec::Trial_Init_SRAvg(LeabraUnit* u, LeabraNetwork* net) {
  float lval = u->avg_m;
  if(lval > opt_thresh.send) {          // active, even just a bit
    u->avg_l += lval * act_avg.l_up_inc; // additive up
  }
  else {
    float eff_dt = act_avg.l_dn_dt * u->own_lay()->acts_m_avg;
    u->avg_l += eff_dt * (lval - u->avg_l); // mult down
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

  if(act_misc.rescale_ctxt && u->ctxt_scale > 0.0f && ctxt_rel_scale > 0.0f) {
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
      // u->net += dt.integ * dt.net_dt * (tot_net - u->net);
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
      u->g_i_syn += dt.integ * dt.net_dt * (u->g_i_raw - u->g_i_syn);
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
    u->net += dt.integ * dt.net_dt * (tot_net - u->net);
    u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
  }

  // add after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += Compute_Noise(u, net);
  }
}

void LeabraUnitSpec::Compute_NetinInteg_Spike_e(LeabraUnit* u, LeabraNetwork* net) {
  // netin gets added at the end of the spike_buf -- 0 time is the end
  Init_SpikeBuff(u);
  u->spike_e_buf->CircAddLimit(u->net, spike.window); // add current net to buffer
  int mx = MAX(spike.window, u->spike_e_buf->length);
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      sum += u->spike_e_buf->CircSafeEl(t);
    }
    sum /= (float)spike.window; // normalize over window
    u->net += dt.integ * (spike.gg_decay * sum - (u->net * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = u->spike_e_buf->CircSafeEl(t);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    // from compute_netinavg
    u->net += dt.integ * dt.net_dt * (sum - u->net);
  }
  u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
}

void LeabraUnitSpec::Compute_NetinInteg_Spike_i(LeabraUnit* u, LeabraNetwork* net) {
  // netin gets added at the end of the spike_i_buf -- 0 time is the end
  Init_SpikeBuff(u);
  u->spike_i_buf->CircAddLimit(u->gc_i, spike.window); // add current net to buffer
  int mx = MAX(spike.window, u->spike_i_buf->length);
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      sum += u->spike_i_buf->CircSafeEl(t);
    }
    sum /= (float)spike.window; // normalize over window
    u->g_i_syn += dt.integ * (spike.gg_decay * sum - (u->g_i_syn * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = u->spike_i_buf->CircSafeEl(t);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    u->g_i_syn += dt.integ * dt.net_dt * (sum - u->g_i_syn);
  }
  u->g_i_syn = MAX(u->g_i_syn, 0.0f); // negative netin doesn't make any sense
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
    if(act_fun == SPIKE) {
      Compute_ClampSpike(u, net, u->ext * spike_misc.clamp_max_p);
      u->AddToActBuf(syn_delay);
    }
    else {
      Compute_RateCodeSpike(u, net);
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
  Compute_ActFun(u, net);
  Compute_SelfReg_Cycle(u, net);

  u->AddToActBuf(syn_delay);

  Compute_SRAvg(u, net, thread_no); // unit level only, not cons (must be separate)
}

void LeabraUnitSpec::Compute_Vm(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun == SPIKE && spike_misc.t_r > 0 && u->spk_t > 0) {
    int spkdel = net->tot_cycle - u->spk_t;
    if(spkdel >= 0 && spkdel <= spike_misc.t_r)
      return;                   // just bail -- leave vm as it is!
  }

  if(net->cycle < dt.fast_cyc) {
    // directly go to equilibrium value
    float new_v_m = Compute_EqVm(u);
    u->I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
    u->v_m += u->I_net;
  }
  else {
    float v_m_eff = u->v_m;
    float net_eff = u->net * g_bar.e;
    if(dt.midpoint) {
      float I_net_1 =
        (net_eff * (e_rev.e - v_m_eff)) + (u->gc_l * (e_rev.l - v_m_eff)) +
        (u->gc_i * (e_rev.i - v_m_eff));
      v_m_eff += .5f * dt.integ * dt.vm_dt * I_net_1; // go half way
    }
    u->I_net =
      (net_eff * (e_rev.e - v_m_eff)) + (u->gc_l * (e_rev.l - v_m_eff)) +
      (u->gc_i * (e_rev.i - v_m_eff));

    // add spike current if relevant
    if(act_fun == SPIKE && spike_misc.exp_slope > 0.0f) {
      u->I_net += g_bar.l * spike_misc.exp_slope *
        expf((v_m_eff - act.thr) / spike_misc.exp_slope);
    }

    u->v_m += dt.integ * dt.vm_dt * (u->I_net - u->adapt);
  }

  if((noise_type == VM_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->v_m += Compute_Noise(u, net);
  }

  u->v_m = vm_range.Clip(u->v_m);
}

void LeabraUnitSpec::Compute_ActFun(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun == SPIKE) {
    Compute_ActFun_spike(u, net);
  }
  else {
    Compute_ActFun_rate(u, net);
  }
}

float LeabraUnitSpec::Compute_ActFun_rate_impl(float val_sub_thr) {
  float new_act = 0.0f;
  switch(act_fun) {
  case NOISY_XX1: {
    if(val_sub_thr <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(val_sub_thr >= nxx1_fun.x_range.max) {
      val_sub_thr *= act.gain;
      new_act = val_sub_thr / (val_sub_thr + 1.0f);
    }
    else {
      new_act = nxx1_fun.Eval(val_sub_thr);
    }
    break;
  }
  case SPIKE:
    break;                      // compiler food
  case NOISY_LINEAR: {
    if(val_sub_thr <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(val_sub_thr >= nxx1_fun.x_range.max) {
      new_act = val_sub_thr * act.gain;
    }
    else {
      new_act = nxx1_fun.Eval(val_sub_thr);
    }
    break;
  }
  case XX1: {
    if(val_sub_thr < 0.0f)
      new_act = 0.0f;
    else {
      val_sub_thr *= act.gain;
      new_act = val_sub_thr / (val_sub_thr + 1.0f);
    }
    break;
  }
  case LINEAR: {
    if(val_sub_thr < 0.0f)
      new_act = 0.0f;
    else
      new_act = val_sub_thr * act.gain;
  }
  break;
  }
  return new_act;
}

void LeabraUnitSpec::Compute_ActFun_rate(LeabraUnit* u, LeabraNetwork* net) {
  float new_act;
  if(u->v_m <= act.thr) {
    // note: this is actually quite important -- if you directly use the gelin
    // the whole time, then units are active right away, and that is problematic
    new_act = Compute_ActFun_rate_impl(u->v_m - act.thr);
  }
  else {
    float g_e_thr = Compute_EThresh(u);
    new_act = Compute_ActFun_rate_impl((u->net * g_bar.e) - g_e_thr);
  }
  if(net->cycle >= dt.fast_cyc) {
    new_act = u->act_nd + dt.integ * dt.vm_dt * (new_act - u->act_nd); // time integral with dt.vm_dt  -- use nd to avoid synd problems
  }
  if(stp.on) {                   // short term plasticity
    u->act_nd = act_range.Clip(new_act); // nd is non-discounted activation!!! solves tons of probs
    new_act *= u->syn_tr;
  }

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += Compute_Noise(u, net);
  }
  u->act = act_range.Clip(new_act);
  u->act_eq = u->act;
  if(!stp.on)
    u->act_nd = u->act_eq;

  if(act_misc.act_lrn == LeabraActMiscSpec::ACT_EQ) {
    u->act_lrn = u->act_eq;
  }
  else {                        // ACT_ND
    u->act_lrn = u->act_nd;
  }
  Compute_RateCodeSpike(u, net);
}

void LeabraUnitSpec::Compute_ActFun_spike(LeabraUnit* u, LeabraNetwork* net) {
  if(u->v_m > spike_misc.spk_thr) {
    u->act = 1.0f;
    u->spike = 1.0f;
    u->v_m = spike_misc.vm_r;
    u->spk_t = net->tot_cycle;
  }
  else {
    u->act = 0.0f;
    u->spike = 0.0f;
  }

  float old_nd = u->act_nd / spike.eq_gain;
  float new_nd;
  if(spike.eq_dt > 0.0f) {
    new_nd = (1.0f - dt.integ * spike.eq_dt) * old_nd + dt.integ * spike.eq_dt * u->act;
    new_nd = act_range.Clip(spike.eq_gain * new_nd);
  }
  else {                        // increment by phase
    if(net->cycle > 0)
      old_nd *= (float)net->cycle;
    new_nd = act_range.Clip(spike.eq_gain * (old_nd + u->act) / (float)(net->cycle+1));
  }
  u->da = new_nd - u->act_nd;   // da is on equilibrium activation
  u->act_nd = new_nd;

  if(stp.on) {
    u->act *= u->syn_tr;
    u->act_eq = u->syn_tr * u->act_nd; // act_eq is depressed rate code, nd is non-depressed!
  }
  else {
    u->act_eq = u->act_nd;      // eq = nd
  }
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

  Compute_ActFun_spike(u, net); // then do normal spiking computation
}

void LeabraUnitSpec::Compute_RateCodeSpike(LeabraUnit* u, LeabraNetwork* net) {
  // use act_nd here so it isn't a self-fulfilling function!
  u->spike = 0.0f;
  if(u->act_nd <= opt_thresh.send) { // no spiking below threshold..
    u->spk_t = -1;
    return;
  }
  if(u->spk_t < 0) {            // start counting from first time above threshold
    u->spk_t = net->tot_cycle;
    return;
  }
  int interval = act_misc.ActToInterval(net->time_inc, dt.integ, u->act_nd);
  if((net->tot_cycle - u->spk_t) >= interval) {
    u->spike = 1.0f;
    u->spk_t = net->tot_cycle;
  }
}

////////////////////////////////////////////////////////////////////
//              Self reg / adapt / depress

void LeabraUnitSpec::Compute_ActAdapt_Cycle(LeabraUnit* u, LeabraNetwork* net) {
  if(!adapt.on) {
    u->adapt = 0.0f;
  }
  else {
    float dad;
    if(act_fun == SPIKE) {
      dad = dt.integ * adapt.Compute_dAdapt(u->v_m, e_rev.l, u->adapt);
    }
    else {
      dad = dt.integ * adapt.Compute_dAdapt(0.0f, u->act, u->adapt);
      // vm_gain drives based on (0 - act) -- the more active, the longer it is in refractory, and thus the longer the adapt goes down..
    }
    if(u->spike > 0.0f) {       // spiked
      dad += adapt.spike_gain;
    }
    u->adapt += dad;
  }
}

void LeabraUnitSpec::Compute_ShortPlast_Cycle(LeabraUnit* u, LeabraNetwork* net) {
  if(!stp.on) {
    u->syn_tr = 1.0f;
    u->syn_nr = 1.0f;
    u->syn_pr = stp.p0;
    u->syn_kre = 0.0f;
  }
  else {
    float dnr = (dt.integ * stp.rec_dt + u->syn_kre) * (1.0f - u->syn_nr)
      - u->syn_pr * u->syn_nr * u->spike;
    float dpr = dt.integ * stp.fac_dt * (stp.p0 - u->syn_pr)
      + stp.fac * (1.0f - u->syn_pr) * u->spike;
    float dkre = -dt.integ * stp.kre_dt * u->syn_kre
      + stp.kre * (1.0f - u->syn_kre) * u->spike;
    u->syn_nr += dnr;
    u->syn_pr += dpr;
    u->syn_kre += dkre;
    if(u->spike > 0.0f) {                                     // only update at spike
      u->syn_tr = stp.oneo_p0_norm * (u->syn_nr * u->syn_pr); // normalize pr by p0_norm
      u->syn_tr = MIN(1.0f, u->syn_tr);                       // max out at 1.0
    }
  }
}

void LeabraUnitSpec::Compute_SelfReg_Cycle(LeabraUnit* u, LeabraNetwork* net) {
  Compute_ActAdapt_Cycle(u, net);
  Compute_ShortPlast_Cycle(u, net);
}

void LeabraUnitSpec::Compute_SelfReg_Trial(LeabraUnit* u, LeabraNetwork* net) {
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

  return rval;
}

void LeabraUnitSpec::Compute_SRAvg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  if(!lay) return;

  float ru_act = u->act_lrn; // activation to use for updating averages
  if(act_fun == LeabraUnitSpec::SPIKE)
    ru_act *= spike.eq_gain;

  u->avg_ss += dt.integ * act_avg.ss_dt * (ru_act - u->avg_ss);
  u->avg_s += dt.integ * act_avg.s_dt * (u->avg_ss - u->avg_s);
  u->avg_m += dt.integ * act_avg.m_dt * (u->avg_s - u->avg_m);
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

///////////////////////////////////////////////////////////////////////
//      Cycle Optional Misc

void LeabraUnitSpec::Compute_MidMinus(LeabraUnit* u, LeabraNetwork* net) {
  u->act_mid = u->act_eq;
}


///////////////////////////////////////////////////////////////////////
//      Phase and Trial Activation Updating

void LeabraUnitSpec::PostSettle(LeabraUnit* u, LeabraNetwork* net) {
  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  float use_act = u->act_lrn;

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
  if(act_misc.avg_dt <= 0.0f) return;
  u->act_avg += act_misc.avg_dt * (u->act_eq - u->act_avg);
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
  // int thno_arg = thread_no;
  // if(thread_no < 0) {
  //   thno_arg = 0;		// pass 0 as arg -- need to store in tmp vec
  // }
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

void LeabraUnitSpec::BioParams(float norm_sec, float norm_volt, float volt_off, float norm_amp,
          float C_pF, float gbar_l_nS, float gbar_e_nS, float gbar_i_nS,
          float erev_l_mV, float erev_e_mV, float erev_i_mV,
          float act_thr_mV, float spk_thr_mV, float exp_slope_mV,
       float adapt_tau_ms, float adapt_vm_gain_nS, float adapt_spk_gain_nA)
{
  // derived units
  float norm_siemens = norm_amp / norm_volt;
  float norm_farad = (norm_sec * norm_amp) / norm_volt;

  dt.vm_tau = ((C_pF * 1.0e-12f) / norm_farad);

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

  adapt.tau = (adapt_tau_ms * 1.0e-3f) / norm_sec;
  adapt.vm_gain = (adapt_vm_gain_nS * 1.0e-9f) / norm_siemens;
  adapt.spike_gain = (adapt_spk_gain_nA * 1.0e-9f) / norm_amp;

  v_m_init.mean = e_rev.l;
  vm_range.min = 0.0f;
  vm_range.max = 2.0f;

  if(act_fun != SPIKE) {
    dt.vm_tau = 3.3f;
    act.gain = 100;
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
    aval = Compute_ActFun_rate_impl(x - g_e_thr);
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


