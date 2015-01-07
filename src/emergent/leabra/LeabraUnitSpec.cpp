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
TA_BASEFUNS_CTORS_DEFN(CIFERThalSpec);
TA_BASEFUNS_CTORS_DEFN(CIFERDeep5bSpec);
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
  Defaults_init();
}

void LeabraActMiscSpec::Defaults_init() {
  rec_nd = true;
  avg_nd = true;
  act_max_hz = 100.0f;
  avg_tau = 200.0f;
  avg_init = 0.15f;

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
  Defaults_init();
}

void SpikeMiscSpec::Defaults_init() {
  ex = false;
  exp_slope = 0.02f;
  spk_thr = 1.2f;
  clamp_max_p = 0.12f;
  clamp_type = REGULAR;
  vm_r = 0.30f;
  t_r = 3;

  eff_spk_thr = 0.5f;           // ex = off
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

  vm_dt = 1.0f / vm_tau;
  net_dt = 1.0f / net_tau;
}

void LeabraDtSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  vm_dt = 1.0f / vm_tau;
  net_dt = 1.0f / net_tau;
}

void LeabraActAvgSpec::Initialize() {
  Defaults_init();
}

void LeabraActAvgSpec::Defaults_init() {
  l_up_inc = 0.1f;
  l_dn_tau = 2.5f;
  ss_tau = 2.0f;
  s_tau = 2.0f;
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
  Ei_dyn = true;                // use when adapt is on
  Ei_gain = 0.001f;
  Ei_tau = 50.0f;

  dt = 1.0f / tau;
  Ei_dt = 1.0f / Ei_tau;
}

void ActAdaptSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt = 1.0f / tau;
  Ei_dt = 1.0f / Ei_tau;
}

void ShortPlastSpec::Initialize() {
  on = false;
  f_r_ratio = 0.02f;
  kre = 0.002f;
  Defaults_init();
}

void ShortPlastSpec::Defaults_init() {
  p0 = 0.2f;
  p0_norm = 0.2f;
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

void CIFERThalSpec::Initialize() {
  on = false;
  thal_thr = 0.2f;
  thal_bin = true;
  auto_thal = false;
  thal_to_super = 0.0f;
  Defaults_init();
}

void CIFERThalSpec::Defaults_init() {
}

void CIFERDeep5bSpec::Initialize() {
  on = false;
  ti_rescale = true;
  ti_resc_all = false;
  ti_scale_mult = 100.0f;
  act5b_thr = 0.2f;
  d5b_to_super = 0.0f;
  ti_5b = 0.0f;
  ti_5b_c = 1.0f - ti_5b;
  Defaults_init();
}

void CIFERDeep5bSpec::Defaults_init() {
  burst = true;
}

void CIFERDeep5bSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ti_5b_c = 1.0f - ti_5b;
}

void DaModSpec::Initialize() {
  on = false;
  minus = 0.0f;
  plus = 0.01f;
  Defaults_init();
}

void DaModSpec::Defaults_init() {
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
  bias_spec.SetBaseType(&TA_LeabraBiasSpec);

  act_fun = NOISY_XX1;

  deep5b_qtr = QNULL;
  ti_ctxt_qtr = QNULL;

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

  if(noise_type == VM_NOISE && act_fun != SPIKE) {
    taMisc::Warning("Cannot use noise_type = VM_NOISE with rate-code (non-spiking) activation function -- changing noise_type to NETIN_NOISE");
    noise_type = NETIN_NOISE;
  }

  if(spike_misc.ex)
    spike_misc.eff_spk_thr = spike_misc.spk_thr;
  else
    spike_misc.eff_spk_thr = act.thr;

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
    if(dt.integ != 1000.0f * net->times.time_inc) {
      taMisc::Warning("unit time integration constant dt.integ of:", (String)dt.integ,
                      "does not match network phases.time_inc increment of:",
                      (String)net->times.time_inc, "time_inc should be 0.001 * dt.integ");
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

  for(i=0; i<fun.size; i++) {
    float x = fun.Xval(i);
    float val = 0.0f;
    if(x > 0.0f)
      val = (act_spec.gain * x) / ((act_spec.gain * x) + 1.0f);
    fun[i] = val;
  }

  nxx1_fl.Convolve(fun, noise_fl); // does alloc
}

void LeabraUnitSpec::Init_Vars(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Vars(u, net, thr_no);

  u->act_eq = 0.0f;
  u->act_nd = 0.0f;
  u->spike = 0.0f;
  u->act_q0 = 0.0f;
  u->act_q1 = 0.0f;
  u->act_q2 = 0.0f;
  u->act_q3 = 0.0f;
  u->act_q4 = 0.0f;
  u->act_m = 0.0f;
  u->act_p = 0.0f;
  u->act_dif = 0.0f;
  u->net_prv_q = 0.0f;
  u->net_prv_trl = 0.0f;
  u->da = 0.0f;
  u->avg_ss = 0.15f;
  u->avg_s = 0.15f;
  u->avg_m = 0.15f;
  u->avg_l = 0.15f;
  u->act_avg = 0.15f;
  u->thal = 0.0f;
  u->thal_prv = 0.0f;
  u->deep5b = 0.0f;
  u->d5b_net = 0.0f;
  u->ti_ctxt = 0.0f;
  u->lrnmod = 0.0f;
  u->gc_i = 0.0f;
  u->I_net = 0.0f;
  u->v_m = 0.0f;
  u->v_m_eq = 0.0f;
  u->adapt = 0.0f;
  u->gi_syn = 0.0f;
  u->gi_self = 0.0f;
  u->gi_ex = 0.0f;
  u->E_i = 0.25f;
  u->syn_tr = 1.0f;
  u->syn_nr = 1.0f;
  u->syn_pr = 0.2f;
  u->syn_kre = 0.0f;
  u->noise = 0.0f;
  u->dav = 0.0f;
  u->sev = 0.0f;

  u->bias_scale = 0.0f;

  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->gi_raw = 0.0f;
  u->d5b_sent = 0.0f;

  u->misc_1 = 0.0f;
  u->spk_t = -1;

  // act_buf = NULL;
  // spike_e_buf = NULL;
  // spike_i_buf = NULL;
}

void LeabraUnitSpec::Init_Weights(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;

  if(bias_spec) {
    bias_spec->B_Init_Weights(u, net, thr_no);
  }

  u->net_prv_q = 0.0f;
  u->net_prv_trl = 0.0f;
  u->act_avg = act_misc.avg_init;
  u->misc_1 = 0.0f;

  Init_ActAvg(u, net, thr_no);
}

void LeabraUnitSpec::Init_ActAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  u->act_avg = act_misc.avg_init;
  u->avg_l = act_misc.avg_init;
}

void LeabraUnitSpec::Init_Netins(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->gi_raw = 0.0f;
  u->d5b_sent = 0.0f;
  u->d5b_net = 0.0f;
  // u->gi_syn = 0.0f;
  // u->ti_ctxt = 0.0f;

  // u->net = 0.0f;

  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    recv_gp->net = 0.0f;
    recv_gp->net_raw = 0.0f;
  }
}

void LeabraUnitSpec::Init_Acts(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Acts(u, net, thr_no);

  Init_Netins(u, net, thr_no);

  u->act = act_init.Gen();
  u->net = 0.0f;               // these are not done in netins -- need to nuke

  u->act_eq = u->act;
  u->act_nd = u->act_eq;
  u->spike = 0.0f;
  u->act_q0 = 0.0f;
  u->act_q1 = 0.0f;
  u->act_q2 = 0.0f;
  u->act_q3 = 0.0f;
  u->act_q4 = 0.0f;
  u->act_m = 0.0f;
  u->act_p = 0.0f;
  u->act_dif = 0.0f;
  u->net_prv_q = 0.0f;        // note: init acts clears this kind of history..
  u->net_prv_trl = 0.0f;

  u->da = 0.0f;
  u->avg_ss = act_misc.avg_init;
  u->avg_s = act_misc.avg_init;
  u->avg_m = act_misc.avg_init;
  // not avg_l
  // not act_avg
  u->thal = 0.0f;
  u->thal_prv = 0.0f;
  u->deep5b = 0.0f;
  u->lrnmod = 0.0f;
  u->d5b_net = 0.0f;
  u->ti_ctxt = 0.0f;
  u->gc_i = 0.0f;
  u->I_net = 0.0f;
  u->v_m = v_m_init.Gen();
  u->v_m_eq = u->v_m;
  u->adapt = 0.0f;
  u->gi_syn = 0.0f;
  u->gi_self = 0.0f;
  u->gi_ex = 0.0f;
  u->E_i = e_rev.i;
  u->syn_tr = 1.0f;
  u->syn_nr = 1.0f;
  u->syn_pr = stp.p0;
  u->syn_kre = 0.0f;
  u->noise = 0.0f;
  u->dav = 0.0f;
  // u->sev = 0.0f; // longer time-course

  // not the scales
  // init netin gets act_sent, net_raw, etc

  u->spk_t = -1;

  Init_SpikeBuff(u);
  // if(act_fun == SPIKE) {
  //   u->spike_e_buf->Reset();
  //   u->spike_i_buf->Reset();
  // }

  Init_ActBuff(u);
  // if(syn_delay.on) {
  //   u->act_buf->Reset();
  // }
}


void LeabraUnitSpec::DecayState(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                                float decay) {
  if(decay > 0.0f) {            // no need to reset netin if not decaying at all
    u->act -= decay * (u->act - act_init.mean);
    u->net -= decay * u->net;
    u->act_eq -= decay * (u->act_eq - act_init.mean);
    u->act_nd -= decay * (u->act_nd - act_init.mean);
    u->gc_i -= decay * u->gc_i;
    u->v_m -= decay * (u->v_m - v_m_init.mean);
    u->v_m_eq -= decay * (u->v_m_eq - v_m_init.mean);
    if(adapt.on) {
      u->adapt -= decay * u->adapt;
    }

    u->gi_syn -= decay * u->gi_syn;
    u->gi_self -= decay * u->gi_self;
    u->gi_ex -= decay * u->gi_ex;
    u->E_i -= decay * (u->E_i - e_rev.i);

    if(stp.on) {
      u->syn_tr -= decay * (u->syn_tr - 1.0f);
      u->syn_nr -= decay * (u->syn_nr - 1.0f);
      u->syn_pr -= decay * (u->syn_tr - stp.p0);
      u->syn_kre -= decay * u->syn_kre;
    }
  }

  if(taMisc::gui_active) {
    u->da = 0.0f;
    u->I_net = 0.0f;
  }

  if(decay == 1.0f) {
    Init_SpikeBuff(u);
    // if(act_fun == SPIKE) {
    //   u->spike_e_buf->Reset();
    //   u->spike_i_buf->Reset();
    // }

    Init_ActBuff(u);
    // if(syn_delay.on) {
    //   u->act_buf->Reset();
    // }
  }
}

void LeabraUnitSpec::Init_SpikeBuff(LeabraUnitVars* u) {
  // todo:
  // if(act_fun == SPIKE) {
  //   if(!u->spike_e_buf) {
  //     u->spike_e_buf = new float_CircBuffer;
  //     taBase::Own(u->spike_e_buf, u);
  //   }
  //   if(!u->spike_i_buf) {
  //     u->spike_i_buf = new float_CircBuffer;
  //     taBase::Own(u->spike_i_buf, u);
  //   }
  // }
  // else {
  //   if(u->spike_e_buf) {
  //     taBase::unRefDone(u->spike_e_buf);
  //     u->spike_e_buf = NULL;
  //   }
  //   if(u->spike_i_buf) {
  //     taBase::unRefDone(u->spike_i_buf);
  //     u->spike_i_buf = NULL;
  //   }
  // }
}

void LeabraUnitSpec::Init_ActBuff(LeabraUnitVars* u) {
  // if(syn_delay.on) {
  //   if(!u->act_buf) {
  //     u->act_buf = new float_CircBuffer;
  //     taBase::Own(u->act_buf, u);
  //   }
  // }
  // else {
  //   if(u->act_buf) {
  //     taBase::unRefDone(u->act_buf);
  //     u->act_buf = NULL;
  //   }
  // }
}

///////////////////////////////////////////////////////////////////////
//      TrialInit functions

void LeabraUnitSpec::Trial_Init_Specs(LeabraNetwork* net) {
  if(act_fun == SPIKE) {
    net->net_misc.spike = true;
  }
  else {
    TestWarning(net->net_misc.spike, "Trial_Init_Specs",
                "detected a mix of SPIKE and NOISY_XX1 activation functions -- due to code optimizations, must all be either one or the other!");
  }
  if(bias_spec)
    ((LeabraConSpec*)bias_spec.SPtr())->Trial_Init_Specs(net);
}

void LeabraUnitSpec::Trial_Init_Unit(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Trial_Init_PrvVals(u, net, thr_no);   // do this b4 decay, so vals are intact
  Trial_Init_SRAvg(u, net, thr_no);     // do this b4 decay..
  Trial_DecayState(u, net, thr_no);
  Trial_NoiseInit(u, net, thr_no);
}

void LeabraUnitSpec::Trial_Init_PrvVals(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  u->net_prv_trl = u->net; 
  u->act_q0 = u->act_q4;
}

void LeabraUnitSpec::Trial_Init_SRAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float lval = u->avg_m;
  if(lval > opt_thresh.send) {          // active, even just a bit
    u->avg_l += lval * act_avg.l_up_inc; // additive up
  }
  else {
    LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
    float eff_dt = act_avg.l_dn_dt * lay->acts_p_avg_eff;
    u->avg_l += eff_dt * (lval - u->avg_l); // mult down
  }
}

void LeabraUnitSpec::Trial_DecayState(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  DecayState(u, net, thr_no, ls->decay.trial);
  // note: theoretically you could avoid doing Init_Netins if there is no decay between
  // trials, and save some compute time, but the delta-based netin has enough
  // error accumulation that this does NOT work well in practice, so we always do it here
  Init_Netins(u, net, thr_no); 
  u->dav = 0.0f;
  //  u->sev = 0.0f; // longer time-course
  u->lrnmod = 0.0f;
  // reset all the time vars so it isn't ambiguous as these update
  if(taMisc::gui_active) {
    u->act_q1 = u->act_q2 = u->act_q3 = u->act_q4 = 0.0f;
    u->act_m = u->act_p = u->act_dif = 0.0f;
  }
}

void LeabraUnitSpec::Trial_NoiseInit(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(noise_type != NO_NOISE && noise_adapt.trial_fixed &&
     (noise.type != Random::NONE)) {
    u->noise = noise.Gen();
  }
}

///////////////////////////////////////////////////////////////////////
//      QuarterInit functions

void LeabraUnitSpec::Quarter_Init_Unit(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Quarter_Init_TargFlags(u, net, thr_no);
  Quarter_Init_PrvVals(u, net, thr_no);
  Compute_NetinScale(u, net, thr_no);
  Compute_HardClamp(u, net, thr_no);
}

void LeabraUnitSpec::Quarter_Init_TargFlags(LeabraUnitVars* u, LeabraNetwork* net,
                                            int thr_no) {
  if(!u->HasExtFlag(UnitVars::TARG))
    return;

  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    // LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
    // if(!lay->HasExtFlag(UnitVars::TARG)) {  // layer isn't a target but unit is..
    //   u->targ = u->ext;
    // }
    u->ext = 0.0f;
    u->ClearExtFlag(UnitVars::EXT);
  }
  else {
    u->ext = u->targ;
    u->SetExtFlag(UnitVars::EXT);
  }
}

void LeabraUnitSpec::Quarter_Init_PrvVals(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no) {
  u->net_prv_q = u->net;
  if(Quarter_Deep5bNow(net->quarter)) {
    u->thal_prv = u->thal;        // only grab prv at start of quarter where deep5b is updating
  }
}

void LeabraUnitSpec::Compute_NetinScale(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // this is all receiver-based and done only at beginning of each quarter
  Unit* un = u->Un(net, thr_no);
  bool plus_phase = (net->phase == LeabraNetwork::PLUS_PHASE);
  bool init_netin = false;
  // NOTE: *everyone* has to init netins when scales change across quarters, because any existing netin has already been weighted at the previous scaled -- no way to rescale that aggregate -- just have to start over..
  bool exclude_d5b = false;
  bool d5b_turned_on = false;   // deep5b just turned on
  bool d5b_turned_off = false;   // deep5b just turned off
  if((plus_phase && net->net_misc.diff_scale_p) ||
     (net->quarter == 1 && net->net_misc.diff_scale_q1)) {
    init_netin = true;
  }
  if(cifer_d5b.on) {
    d5b_turned_on = (net->quarter >= 1 && Quarter_Deep5bNow(net->quarter) &&
                     !Quarter_Deep5bNow(net->quarter-1));
    d5b_turned_off = (net->quarter >= 1 && !Quarter_Deep5bNow(net->quarter) &&
                      Quarter_Deep5bNow(net->quarter-1));
    if(cifer_d5b.ti_rescale) {
      exclude_d5b = true;        // always exclude!
    }
  }

  if(init_netin) {
    Init_Netins(u, net, thr_no);
  }

  float net_scale = 0.0f;
  float inhib_net_scale = 0.0f;
  int n_active_cons = 0;        // track this for bias weight scaling!
  
  // important: count all projections so it is uniform across all units
  // in the layer!  if a unit does not have a connection in a given projection,
  // then it counts as a zero, but it counts in overall normalization!
  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    cs->Compute_NetinScale(recv_gp, from, plus_phase); // sets recv_gp->scale_eff
    if(exclude_d5b && cs->IsDeep5bCon())               // exclude from rel rescaling
      continue;
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
      net_scale += rel_scale;
    }
  }
  
  // add the bias weight into the netinput, scaled by 1/n
  if(bias_spec) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.SPtr();
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(un->n_recv_cons > 0)
      u->bias_scale /= (float)un->n_recv_cons; // one over n scaling for bias!
  }

  float d5b_rel_scale = 0.0f;
  // float ctxt_rel_scale = 0.0f;
  // now renormalize, each one separately..
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    Projection* prjn = (Projection*) recv_gp->prjn;
    LeabraLayer* from = (LeabraLayer*) prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->inhib) {
      if(inhib_net_scale > 0.0f)
        recv_gp->scale_eff /= inhib_net_scale;
    }
    else {
      if(net_scale > 0.0f) {
        recv_gp->scale_eff /= net_scale;
        if(cs->IsDeep5bCon()) {
          d5b_rel_scale += recv_gp->scale_eff;
        }
      }
    }
  }

  // finally: renorm ti scale
  if(cifer_d5b.on && cifer_d5b.ti_rescale) {
    float sc_fact;
    if(cifer_d5b.ti_resc_all) {
      sc_fact = 0.0001f;
    }
    else {
      sc_fact = (1.0f - cifer_d5b.ti_scale_mult * d5b_rel_scale);
      if(sc_fact < 0.0001f) sc_fact = 0.0001f; // keep it something so we can recover original
    }
    if(d5b_turned_on)
      u->ti_ctxt *= sc_fact;    // downscale
    else if(d5b_turned_off)
      u->ti_ctxt /= sc_fact;    // upscale
  }
}

void LeabraUnitSpec::Compute_HardClamp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!u->HasExtFlag(UnitVars::EXT))
    return;
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  if(!(ls->clamp.hard && lay->HasExtFlag(UnitVars::EXT))) {
    return;
  }
  u->net = u->ext;
  u->thal = u->ext;             // thalamus is external input
  u->act_eq = clamp_range.Clip(u->ext);
  u->act_nd = u->act_eq;
  u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->v_m_eq = u->v_m;
  u->da = u->I_net = 0.0f;

  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
  // u->AddToActBuf(syn_delay);
}

// NOTE: these two functions should always be the same modulo the clamp_range.Clip

void LeabraUnitSpec::Compute_HardClampNoClip(LeabraUnitVars* u, LeabraNetwork* net,
                                             int thr_no) {
  if(!u->HasExtFlag(UnitVars::EXT))
    return;
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  if(!(ls->clamp.hard && lay->HasExtFlag(UnitVars::EXT))) {
    return;
  }
  u->net = u->ext;
  u->thal = u->ext;             // thalamus is external input
  //  u->act_eq = clamp_range.Clip(u->ext);
  u->act_eq = u->ext;
  u->act_nd = u->act_eq;
  u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->v_m_eq = u->v_m;
  u->da = u->I_net = 0.0f;

  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
  // u->AddToActBuf(syn_delay);
}

void LeabraUnitSpec::ExtToComp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!u->HasExtFlag(UnitVars::EXT))
    return;
  u->ClearExtFlag(UnitVars::EXT);
  u->SetExtFlag(UnitVars::COMP);
  u->targ = u->ext;
  u->ext = 0.0f;
}

void LeabraUnitSpec::TargExtToComp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!u->HasExtFlag(UnitVars::TARG_EXT))
    return;
  if(u->HasExtFlag(UnitVars::EXT))
    u->targ = u->ext;
  u->ext = 0.0f;
  u->ClearExtFlag(UnitVars::TARG_EXT);
  u->SetExtFlag(UnitVars::COMP);
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 1: netinput

void LeabraUnitSpec::Send_NetinDelta(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->n_thrs_built == 1) {
    net->send_pct_tot++;        // only safe for non-thread case
  }
  float act_ts = u->act;
  // if(syn_delay.on) {
  //   if(!u->act_buf)
  //     Init_ActBuff(u);
  //   act_ts = u->act_buf->CircSafeEl(0); // get first logical element..
  // }

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->act_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      if(net->n_thrs_built == 1) {
        net->send_pct_n++;
      }
      const int nsg = u->NSendConGps(net, thr_no); 
      for(int g=0; g< nsg; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
        if(send_gp->NotActive()) continue;
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(tol->hard_clamped)      continue;
        ((LeabraConSpec*)send_gp->con_spec)->Send_NetinDelta(send_gp, net, thr_no,
                                                             act_delta);
      }
      u->act_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    if(net->n_thrs_built == 1) {
      net->send_pct_n++;
    }
    float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->hard_clamped)        continue;
      ((LeabraConSpec*)send_gp->con_spec)->Send_NetinDelta(send_gp, net, thr_no,
                                                           act_delta);
    }
    u->act_sent = 0.0f;         // now it effectively sent a 0..
  }

  if(Quarter_Deep5bNow(net->quarter)) {
    Send_Deep5bNetin(u, net, thr_no);
  }    
}

void LeabraUnitSpec::Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // this integrates from SendDelta into net_raw and gi_syn
  int nt = net->n_thrs_built;
  int flat_idx = u->UnFlatIdx(net, thr_no);
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float net_delta = 0.0f;
  float gi_delta = 0.0f;
  if(net->NetinPerPrjn()) {
    const int nrg = u->NRecvConGps(net, thr_no); 
    for(int g=0; g< nrg; g++) {
      LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
      if(recv_gp->NotActive()) continue;
      float g_net_delta = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
	g_net_delta += ndval;
#ifndef CUDA_COMPILE
        ndval = 0.0f;           // zero immediately upon use -- for threads
#endif
      }
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      // todo? incorporate finer-grained inhib here?
      if(cs->inhib)
	gi_delta += g_net_delta;
      else
	net_delta += g_net_delta;

      recv_gp->net_raw += g_net_delta; // note: direct assignment to raw, no time integ
    }
  }
  else {
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmp(j)[flat_idx];
      net_delta += ndval;
#ifndef CUDA_COMPILE
      ndval = 0.0f;           // zero immediately upon use -- for threads
#endif
    }
  }

  if(net->net_misc.inhib_cons) {
    u->gi_raw += gi_delta;
    if(act_fun == SPIKE) {
      u->gi_syn = MAX(u->gi_syn, 0.0f);
      Compute_NetinInteg_Spike_i(u, net, thr_no);
    }
    else {
      u->gi_syn += dt.integ * dt.net_dt * (u->gi_raw - u->gi_syn);
      u->gi_syn = MAX(u->gi_syn, 0.0f); // negative netin doesn't make any sense
    }
  }
  else {
    // clear so automatic inhibition can add to these values!
    // this is an unnec memory write!
    // u->gi_syn = 0.0f;
    // u->gi_raw = 0.0f;
  }
  
  u->net_raw += net_delta;
}

void LeabraUnitSpec::Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(lay->hard_clamped) return;

  Compute_NetinRaw(u, net, thr_no);
  // u->net_raw and u->gi_syn now have proper values integrated from deltas

  float net_syn = u->net_raw;
  float net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);
  // this could modify net_syn if it wants..
  float net_tot = net_syn + net_ex;

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = MAX(net_tot, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u, net, thr_no);
  }
  else {
    u->net += dt.integ * dt.net_dt * (net_tot - u->net);
    if(u->net < 0.0f) u->net = 0.0f; // negative netin doesn't make any sense
  }

  // add after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += Compute_Noise(u, net, thr_no);
  }

  if(Quarter_Deep5bNow(net->quarter)) {
    Send_Deep5bNetin_Post(u, net, thr_no);
  }    
}

float LeabraUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  LeabraLayerSpec* ls = (LeabraLayerSpec*)u->Un(net, thr_no)->own_lay()->GetLayerSpec();

  float net_ex = 0.0f;
  if(bias_spec) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UnitVars::EXT)) {
    net_ex += u->ext * ls->clamp.gain;
  }
  if(net->net_misc.ti) {
    net_ex += u->ti_ctxt;
  }
  if(cifer_thal.on) {
    net_ex += cifer_thal.thal_to_super * u->thal * net_syn;
  }
  if(cifer_d5b.on) {
    net_ex += u->d5b_net + cifer_d5b.d5b_to_super * u->deep5b; // not * net_syn
  }
  if(da_mod.on) {
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      net_ex += da_mod.plus * u->dav * net_syn;
    }
    else {                      // MINUS_PHASE
      net_ex += da_mod.minus * u->dav * net_syn;
    }
  }
  return net_ex;
}


void LeabraUnitSpec::Compute_NetinInteg_Spike_e(LeabraUnitVars* u, LeabraNetwork* net,
                                                int thr_no) {
  // todo!
  // netin gets added at the end of the spike_buf -- 0 time is the end
  // Init_SpikeBuff(u);
  // // u->spike_e_buf->CircAddLimit(u->net, spike.window); // add current net to buffer
  // int mx = MAX(spike.window, u->spike_e_buf->length);
  // float sum = 0.0f;
  // if(spike.rise == 0.0f && spike.decay > 0.0f) {
  //   // optimized fast recursive exp decay: note: does NOT use dt.net_dt
  //   for(int t=0;t<mx;t++) {
  //     sum += u->spike_e_buf->CircSafeEl(t);
  //   }
  //   sum /= (float)spike.window; // normalize over window
  //   u->net += dt.integ * (spike.gg_decay * sum - (u->net * spike.oneo_decay));
  // }
  // else {
  //   for(int t=0;t<mx;t++) {
  //     float spkin = u->spike_e_buf->CircSafeEl(t);
  //     if(spkin > 0.0f) {
  //       sum += spkin * spike.ComputeAlpha(mx-t-1);
  //     }
  //   }
  //   // from compute_netinavg
  //   u->net += dt.integ * dt.net_dt * (sum - u->net);
  // }
  // u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
}

void LeabraUnitSpec::Compute_NetinInteg_Spike_i(LeabraUnitVars* u, LeabraNetwork* net,
                                                int thr_no) {
  // netin gets added at the end of the spike_i_buf -- 0 time is the end
  // Init_SpikeBuff(u);
  // u->spike_i_buf->CircAddLimit(u->gc_i, spike.window); // add current net to buffer
  // int mx = MAX(spike.window, u->spike_i_buf->length);
  // float sum = 0.0f;
  // if(spike.rise == 0.0f && spike.decay > 0.0f) {
  //   // optimized fast recursive exp decay: note: does NOT use dt.net_dt
  //   for(int t=0;t<mx;t++) {
  //     sum += u->spike_i_buf->CircSafeEl(t);
  //   }
  //   sum /= (float)spike.window; // normalize over window
  //   u->gi_syn += dt.integ * (spike.gg_decay * sum - (u->gi_syn * spike.oneo_decay));
  // }
  // else {
  //   for(int t=0;t<mx;t++) {
  //     float spkin = u->spike_i_buf->CircSafeEl(t);
  //     if(spkin > 0.0f) {
  //       sum += spkin * spike.ComputeAlpha(mx-t-1);
  //     }
  //   }
  //   u->gi_syn += dt.integ * dt.net_dt * (sum - u->gi_syn);
  // }
  // u->gi_syn = MAX(u->gi_syn, 0.0f); // negative netin doesn't make any sense
}

void LeabraUnitSpec::Send_Deep5bNetin(LeabraUnitVars* u, LeabraNetwork* net,
                                         int thr_no) {
  float act_ts = u->deep5b;
  // note: no delay for 5b
  // if(syn_delay.on) {
  //   if(!u->act_buf)
  //     Init_ActBuff(u);
  //   act_ts = u->act_buf->CircSafeEl(0); // get first logical element..
  // }

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->d5b_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      const int nsg = u->NSendConGps(net, thr_no); 
      for(int g=0; g< nsg; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
        if(send_gp->NotActive()) continue;
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(tol->hard_clamped)      continue;
        if(!((LeabraConSpec*)send_gp->GetConSpec())->IsDeep5bCon()) continue;
        Deep5bConSpec* sp = (Deep5bConSpec*)send_gp->GetConSpec();
        sp->Send_D5bNetDelta(send_gp, net, thr_no, act_delta);
      }
      u->d5b_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->d5b_sent > opt_thresh.send) {
    float act_delta = - u->d5b_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->hard_clamped)        continue;
      if(!((LeabraConSpec*)send_gp->GetConSpec())->IsDeep5bCon()) continue;
      Deep5bConSpec* sp = (Deep5bConSpec*)send_gp->GetConSpec();
      sp->Send_D5bNetDelta(send_gp, net, thr_no, act_delta);
    }
    u->d5b_sent = 0.0f;         // now it effectively sent a 0..
  }
}

void LeabraUnitSpec::Send_Deep5bNetin_Post(LeabraUnitVars* u, LeabraNetwork* net,
                                         int thr_no) {
  int nt = net->n_thrs_built;
  int flat_idx = u->UnFlatIdx(net, thr_no);
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float net_delta = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendD5bNetTmp(j)[flat_idx];
    net_delta += ndval;
#ifndef CUDA_COMPILE
    ndval = 0.0f;             // zero immediately..
#endif
  }
  u->d5b_net += net_delta;
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: inhibition

void LeabraUnitSpec::Compute_ApplyInhib
(LeabraUnitVars* u, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
 LeabraInhib* thr, float ival) {
  Compute_SelfInhib(u, net, thr_no, lspec);
  float gi_ex = 0.0f;
  if(lspec->del_inhib.on) {
    gi_ex = lspec->del_inhib.prv_trl * u->net_prv_trl + 
      lspec->del_inhib.prv_q * u->net_prv_q;
  }
  u->gc_i = ival + u->gi_syn + u->gi_self + gi_ex;
  if(taMisc::gui_active) {
    u->gi_ex = gi_ex;
  }
}


///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: activation -- rate code

void LeabraUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

  if((net->cycle >= 0) && lay->hard_clamped) {
    return; // don't re-compute
  }

  // first, apply inhibition 
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraInhib* thr = ((LeabraUnitSpec*)u->unit_spec)->GetInhib(un);
  if(thr) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    Compute_ApplyInhib(u, net, thr_no, ls, thr, thr->i_val.g_i);
  }

  Compute_Vm(u, net, thr_no);
  Compute_ActFun_Rate(u, net, thr_no);
  Compute_SelfReg_Cycle(u, net, thr_no);

  // u->AddToActBuf(syn_delay);
}

float LeabraUnitSpec::Compute_ActFun_Rate_impl(float val_sub_thr) {
  float new_act = 0.0f;
  if(val_sub_thr >= nxx1_fun.x_range.max) {
    val_sub_thr *= act.gain;
    new_act = val_sub_thr / (val_sub_thr + 1.0f);
  }
  else if(val_sub_thr > nxx1_fun.x_range.min) {
    new_act = nxx1_fun.Eval(val_sub_thr);
  }
  return new_act;
}

void LeabraUnitSpec::Compute_ActFun_Rate(LeabraUnitVars* u, LeabraNetwork* net,
                                         int thr_no) {
  float new_act;
  if(u->v_m_eq <= act.thr) {
    // note: this is quite important -- if you directly use the gelin
    // the whole time, then units are active right away -- need v_m_eq dynamics to
    // drive subthreshold activation behavior
    new_act = Compute_ActFun_Rate_impl(u->v_m_eq - act.thr);
  }
  else {
    float g_e_thr = Compute_EThresh(u);
    new_act = Compute_ActFun_Rate_impl((u->net * g_bar.e) - g_e_thr);
  }
  if(net->cycle >= dt.fast_cyc) {
    new_act = u->act_nd + dt.integ * dt.vm_dt * (new_act - u->act_nd); // time integral with dt.vm_dt  -- use nd to avoid synd problems
  }

  if(taMisc::gui_active) {
    u->da = new_act - u->act_nd;
  }
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += Compute_Noise(u, net, thr_no);
  }
  u->act_nd = act_range.Clip(new_act);

  if(stp.on) {                   // short term plasticity, depression
    u->act = u->act_nd * u->syn_tr; // overall probability of transmission
  }
  else {
    u->act = u->act_nd;
  }
  u->act_eq = u->act;           // for rate code, eq == act

  // we now use the exact same vm-based dynamics as in SPIKE model, for full consistency!
  // note that v_m_eq is NOT reset here:
  if(u->v_m > spike_misc.eff_spk_thr) {
    u->spike = 1.0f;
    u->v_m = spike_misc.vm_r;
    u->spk_t = net->tot_cycle;
  }
  else {
    TestWrite(u->spike, 0.0f);
  }
}

void LeabraUnitSpec::Compute_RateCodeSpike(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  // use act_nd here so it isn't a self-fulfilling function!
  // note: this is only used for clamped layers -- dynamic layers use SPIKE-based mechanisms
  u->spike = 0.0f;
  if(u->act_nd <= opt_thresh.send) { // no spiking below threshold..
    u->spk_t = -1;
    return;
  }
  if(u->spk_t < 0) {            // start counting from first time above threshold
    u->spk_t = net->tot_cycle;
    return;
  }
  int interval = act_misc.ActToInterval(net->times.time_inc, dt.integ, u->act_nd);
  if((net->tot_cycle - u->spk_t) >= interval) {
    u->spike = 1.0f;
    u->spk_t = net->tot_cycle;
    u->v_m = spike_misc.vm_r;   // reset vm when we spike -- now we can use it just like spiking!
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: activation -- spiking

void LeabraUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

  if((net->cycle >= 0) && lay->hard_clamped) {
    Compute_ClampSpike(u, net, thr_no, u->ext * spike_misc.clamp_max_p);
      // u->AddToActBuf(syn_delay);
    return; // don't re-compute
  }

  // first, apply inhibition 
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraInhib* thr = ((LeabraUnitSpec*)u->unit_spec)->GetInhib(un);
  if(thr) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    Compute_ApplyInhib(u, net, thr_no, ls, thr, thr->i_val.g_i);
  }

  Compute_Vm(u, net, thr_no);
  Compute_ActFun_Spike(u, net, thr_no);
  Compute_SelfReg_Cycle(u, net, thr_no);

  // u->AddToActBuf(syn_delay);
}

void LeabraUnitSpec::Compute_ActFun_Spike(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no) {
  if(u->v_m > spike_misc.eff_spk_thr) {
    u->act = 1.0f;
    u->spike = 1.0f;
    u->v_m = spike_misc.vm_r;
    u->spk_t = net->tot_cycle;
  }
  else {
    TestWrite(u->act, 0.0f);
    TestWrite(u->spike, 0.0f);
  }

  float act_nd = u->act_nd / spike.eq_gain;
  if(spike.eq_dt > 0.0f) {
    act_nd += dt.integ * spike.eq_dt * (u->act - act_nd);
  }
  else {                        // increment by phase
    if(net->cycle > 0)
      act_nd *= (float)net->cycle;
    act_nd = (act_nd + u->act) / (float)(net->cycle+1);
  }
  act_nd = act_range.Clip(spike.eq_gain * act_nd);
  if(taMisc::gui_active) {
    u->da = act_nd - u->act_nd;   // da is on equilibrium activation
  }
  u->act_nd = act_nd;

  if(stp.on) {
    u->act *= u->syn_tr;
    u->act_eq = u->syn_tr * u->act_nd; // act_eq is depressed rate code
  }
  else {
    u->act_eq = u->act_nd;      // eq = nd
  }
}

void LeabraUnitSpec::Compute_ClampSpike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                                        float spike_p) {
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
      fire_now = (net->cycle % cyc_int == 0);
    }
    break;
  }
  case SpikeMiscSpec::CLAMPED:
    return;                     // do nothing further
  }
  if(fire_now) {
    u->v_m = spike_misc.eff_spk_thr + 0.1f; // make it fire
  }
  else {
    u->v_m = e_rev.l;           // make it not fire
  }

  Compute_ActFun_Spike(u, net, thr_no); // then do normal spiking computation
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3.2: membrane potential

void LeabraUnitSpec::Compute_Vm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  bool updt_spk_vm = true;
  if(spike_misc.t_r > 0 && u->spk_t > 0) {
    int spkdel = net->tot_cycle - u->spk_t;
    if(spkdel >= 0 && spkdel <= spike_misc.t_r)
      updt_spk_vm = false;    // don't update the spiking vm during refract
  }

  if(net->cycle < dt.fast_cyc) {
    // directly go to equilibrium value
    float new_v_m = Compute_EqVm(u);
    float I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
    u->v_m += I_net;
    u->v_m_eq = u->v_m;
    if(taMisc::gui_active) {
      u->I_net = I_net;
    }
  }
  else {
    float net_eff = u->net * g_bar.e;
    float E_i;
    if(adapt.on && adapt.Ei_dyn) {
      // update the E_i reversal potential as function of inhibitory current
      // key to assume that this is driven by backpropagating AP's
      E_i = u->E_i;
      u->E_i += adapt.Ei_gain * u->act_eq + adapt.Ei_dt * (e_rev.i - u->E_i);
    }
    else {
      E_i = e_rev.i;
    }

    float gc_l = g_bar.l;
    float gc_i = u->gc_i * g_bar.i;

    if(updt_spk_vm) {
      // first compute v_m, using midpoint method:
      float v_m_eff = u->v_m;
      // midpoint method: take a half-step:
      float I_net_1 =
        (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff)) +
        (gc_i * (E_i - v_m_eff));
      v_m_eff += .5f * dt.integ * dt.vm_dt * I_net_1; // go half way
      float I_net = (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff))
        + (gc_i * (E_i - v_m_eff));
      // add spike current if relevant
      if(spike_misc.ex) {
        I_net += g_bar.l * spike_misc.exp_slope *
          taMath_float::exp_fast((v_m_eff - act.thr) / spike_misc.exp_slope);
      }
      u->v_m += dt.integ * dt.vm_dt * (I_net - u->adapt);
      if(taMisc::gui_active) {
        u->I_net = I_net;
      }
    }

    // always compute v_m_eq with simple integration -- used for rate code subthreshold
    float I_net_r = (net_eff * (e_rev.e - u->v_m_eq)) 
      + (gc_l * (e_rev.l - u->v_m_eq)) +  (gc_i * (E_i - u->v_m_eq));
    u->v_m_eq += dt.integ * dt.vm_dt * (I_net_r - u->adapt);
  }

  if((noise_type == VM_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    float ns = Compute_Noise(u, net, thr_no); 
    u->v_m += ns;
    u->v_m_eq += ns;
  }

  if(u->v_m < vm_range.min) u->v_m = vm_range.min;
  if(u->v_m > vm_range.max) u->v_m = vm_range.max;
  if(u->v_m_eq < vm_range.min) u->v_m_eq = vm_range.min;
  if(u->v_m_eq > vm_range.max) u->v_m_eq = vm_range.max;
}

////////////////////////////////////////////////////////////////////
//              Self reg / adapt / depress

void LeabraUnitSpec::Compute_ActAdapt_Cycle(LeabraUnitVars* u, LeabraNetwork* net,
                                            int thr_no) {
  if(!adapt.on) {
    TestWrite(u->adapt, 0.0f);
  }
  else {
    float dad = dt.integ * (adapt.Compute_dAdapt(u->v_m, e_rev.l, u->adapt) +
                            u->spike * adapt.spike_gain);
    u->adapt += dad;
  }
}

void LeabraUnitSpec::Compute_ShortPlast_Cycle(LeabraUnitVars* u, LeabraNetwork* net,
                                              int thr_no) {
  if(!stp.on) {
    if(u->syn_tr != 1.0f) {
      u->syn_tr = 1.0f;
      u->syn_nr = 1.0f;
      u->syn_pr = stp.p0;
      u->syn_kre = 0.0f;
    }
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
      if(u->syn_tr > 1.0f) u->syn_tr = 1.0f;                  // max out at 1.0
    }
  }
}

void LeabraUnitSpec::Compute_SelfReg_Cycle(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  Compute_ActAdapt_Cycle(u, net, thr_no);
  Compute_ShortPlast_Cycle(u, net, thr_no);
}

float LeabraUnitSpec::Compute_Noise(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
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

void LeabraUnitSpec::Compute_Act_Post(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_ThalDeep5b(u, net, thr_no);
  Compute_SRAvg(u, net, thr_no);
}

void LeabraUnitSpec::Compute_Act_ThalDeep5b(LeabraUnitVars* u, LeabraNetwork* net,
                                            int thr_no) {
  if(cifer_thal.on) {
    if(cifer_thal.auto_thal) {
      u->thal = u->act_eq;
    }
    if(u->thal < cifer_thal.thal_thr) {
      TestWrite(u->thal, 0.0f);
    }
    if(cifer_thal.thal_bin && u->thal > 0.0f) {
      TestWrite(u->thal, 1.0f);
    }
  }

  if(!cifer_d5b.on) return;
  
  if(Quarter_Deep5bNow(net->quarter)) {
    float act5b = u->act_eq;
    if(act5b < cifer_d5b.act5b_thr) {
      act5b = 0.0f;
    }
    u->deep5b = u->thal * act5b;  // thal is thresholded
  }
  else {
    if(cifer_d5b.burst) {
      TestWrite(u->deep5b, 0.0f); // turn it off
    }
  }
}

void LeabraUnitSpec::Compute_SRAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float ru_act;
  if(act_misc.avg_nd) {
    ru_act = u->act_nd;
  }
  else {
    ru_act = u->act_eq;
  }

  u->avg_ss += dt.integ * act_avg.ss_dt * (ru_act - u->avg_ss);
  u->avg_s += dt.integ * act_avg.s_dt * (u->avg_ss - u->avg_s);
  u->avg_m += dt.integ * act_avg.m_dt * (u->avg_s - u->avg_m);
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats


///////////////////////////////////////////////////////////////////////
//      Phase and Trial Activation Updating

void LeabraUnitSpec::Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Quarter_Final_RecVals(u, net, thr_no);
}

void LeabraUnitSpec::Quarter_Final_RecVals(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  float use_act;
  if(act_misc.rec_nd) {
    use_act = u->act_nd;
  }
  else {
    use_act = u->act_eq;
  }

  switch(net->quarter) {        // this has not advanced yet -- still 0-3
  case 0:
    u->act_q1 = use_act;
    break;
  case 1:
    u->act_q2 = use_act;
    break;
  case 2:
    u->act_q3 = use_act;
    u->act_m = use_act;
    break;
  case 3:
    u->act_q4 = use_act;
    u->act_p = use_act;
    u->act_dif = u->act_p - u->act_m;
    Compute_ActTimeAvg(u, net, thr_no);
    break;
  }

  if(Quarter_Deep5bNow(net->quarter)) {
    // just ending a quarter where deep5b was updating -- save this thal as previous thal
    u->thal_prv = u->thal;
  }
}

void LeabraUnitSpec::Compute_ActTimeAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(act_misc.avg_dt <= 0.0f) return;
  u->act_avg += act_misc.avg_dt * (u->act_nd - u->act_avg);
}


/////////////////////////////////////////////////
//              Leabra TI

void LeabraUnitSpec::Send_TICtxtNetin(LeabraUnitVars* u, LeabraNetwork* net,
                                      int thr_no) {

  if(!Quarter_SendTICtxtNow(net->quarter)) return;

  float act_ts = u->act_eq;
  if(cifer_d5b.on) {
    act_ts *= cifer_d5b.ti_5b_c;
    act_ts += cifer_d5b.ti_5b * u->deep5b;
  }

  if(act_ts > opt_thresh.send) {
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(!((LeabraConSpec*)send_gp->GetConSpec())->IsTICtxtCon()) continue;
      LeabraTICtxtConSpec* sp = (LeabraTICtxtConSpec*)send_gp->GetConSpec();
      sp->Send_TICtxtNetin(send_gp, net, thr_no, act_ts);
    }
  }
}

void LeabraUnitSpec::Send_TICtxtNetin_Post(LeabraUnitVars* u, LeabraNetwork* net,
                                            int thr_no) {
  if(!Quarter_SendTICtxtNow(net->quarter)) return;

  int flat_idx = u->UnFlatIdx(net, thr_no);
  int nt = net->n_thrs_built;
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float nw_nt = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendNetinTmp(j)[flat_idx];
    nw_nt += ndval;
#ifndef CUDA_COMPILE
    ndval = 0.0f;             // zero immediately..
#endif
  }
  u->ti_ctxt = nw_nt;
}

void LeabraUnitSpec::ClearTICtxt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  u->deep5b = 0.0f;
  u->d5b_net = 0.0f;
  u->ti_ctxt = 0.0f;
}


///////////////////////////////////////////////////////////////////////
//      Stats

float LeabraUnitSpec::Compute_SSE(UnitVars* ru, Network* rnet, int thr_no, bool& has_targ) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  // just replaces act_m for act in original
  float sse = 0.0f;
  has_targ = false;
  if(u->HasExtFlag(UnitVars::COMP_TARG)) {
    has_targ = true;
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) >= sse_tol)
      sse = uerr * uerr;
  }
  return sse;
}

bool LeabraUnitSpec::Compute_PRerr
(UnitVars* ru, Network* rnet, int thr_no,
 float& true_pos, float& false_pos, float& false_neg, float& true_neg) {
  // just replaces act_m for act in original
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f; true_neg = 0.0f;
  bool has_targ = false;
  if(u->HasExtFlag(UnitVars::COMP_TARG)) {
    has_targ = true;
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
  }
  return has_targ;
}

float LeabraUnitSpec::Compute_NormErr(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                                      bool& targ_active) {
  targ_active = false;
  if(!u->HasExtFlag(UnitVars::COMP_TARG)) return 0.0f;

  targ_active = (u->targ > 0.5f);   // use this for counting expected activity level
    
  if(net->lstats.on_errs) {
    if(u->act_m > 0.5f && u->targ < 0.5f) return 1.0f;
  }
  if(net->lstats.off_errs) {
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
    aval = Compute_ActFun_Rate_impl(x - g_e_thr);
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


