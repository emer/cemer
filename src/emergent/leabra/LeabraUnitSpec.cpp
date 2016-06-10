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
#include <DeepCtxtConSpec>
#include <SendDeepRawConSpec>
#include <SendDeepModConSpec>
#include <taProject>
#include <taMath_double>
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraActFunSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActMiscSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeFunSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeMiscSpec);
#ifdef SUGP_NETIN
TA_BASEFUNS_CTORS_DEFN(LeabraNetinSpec);
#endif // SUGP_NETIN
TA_BASEFUNS_CTORS_DEFN(OptThreshSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraInitSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraDtSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActAvgSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraAvgLSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraAvgL2Spec);
TA_BASEFUNS_CTORS_DEFN(AdaptLeakSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraChannels);
TA_BASEFUNS_CTORS_DEFN(ActAdaptSpec);
TA_BASEFUNS_CTORS_DEFN(ShortPlastSpec);
TA_BASEFUNS_CTORS_DEFN(SynDelaySpec);
TA_BASEFUNS_CTORS_DEFN(RLrateSpec);
TA_BASEFUNS_CTORS_DEFN(DeepSpec);
TA_BASEFUNS_CTORS_DEFN(DaModSpec);
TA_BASEFUNS_CTORS_DEFN(NoiseAdaptSpec);

TA_BASEFUNS_CTORS_DEFN(LeabraUnitSpec);

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
  net_gain = 1.0f;
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

#ifdef SUGP_NETIN
void LeabraNetinSpec::Initialize() {
  max_on = false;
  max_ff = false;
  Defaults_init();
}

void LeabraNetinSpec::Defaults_init() {
  max_mix = 0.2f;
  max_gain = 0.4f;
  sum_mix = 1.0f - max_mix;
  max_mult = max_mix * max_gain;
}

void LeabraNetinSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sum_mix = 1.0f - max_mix;
  max_mult = max_mix * max_gain;
}
#endif // SUGP_NETIN


void OptThreshSpec::Initialize() {
  send = .1f;
  delta = 0.005f;
  xcal_lrn = 0.01f;
}

void LeabraInitSpec::Initialize() {
  Defaults_init();
}

void LeabraInitSpec::Defaults_init() {
  act = 0.0f;
  v_m = 0.4f;
  netin = 0.0f;
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
  ss_tau = 2.0f;
  s_tau = 2.0f;
  m_tau = 10.0f;
  m_in_s = 0.1f;

  ss_dt = 1.0f / ss_tau;
  s_dt = 1.0f / s_tau;
  m_dt = 1.0f / m_tau;
  s_in_s = 1.0f - m_in_s;
}


void LeabraActAvgSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ss_dt = 1.0f / ss_tau;
  s_dt = 1.0f / s_tau;
  m_dt = 1.0f / m_tau;
  s_in_s = 1.0f - m_in_s;
}

void LeabraAvgLSpec::Initialize() {
  Defaults_init();
}

void LeabraAvgLSpec::Defaults_init() {
  init = 0.4f;
  max = 1.5f;
  min = 0.2f;
  tau = 10.0f;
  lrn_max = 0.05f;
  lrn_min = 0.005f;
  
  dt = 1.0f / tau;
  lrn_fact = (lrn_max - lrn_min) / (max - min);
}

void LeabraAvgLSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt = 1.0f / tau;
  lrn_fact = (lrn_max - lrn_min) / (max - min);
}


void LeabraAvgL2Spec::Initialize() {
  Defaults_init();
}

void LeabraAvgL2Spec::Defaults_init() {
  err_mod = true;
  err_min = 0.01f;
  act_thr = 0.2f;
  lay_act_thr = 0.01f;
}

void AdaptLeakSpec::Initialize() {
  on = false;
  Defaults_init();
}

void AdaptLeakSpec::Defaults_init() {
  tau = 100.0f;
  hi_thr = 1.2f;
  lo_thr = 0.4f;
  min_bwt = 0.0f;
  max_bwt = 0.2f;
  
  dt = 1.0f / tau;
  mid_thr = 0.5f * (hi_thr + lo_thr);
}

void AdaptLeakSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt = 1.0f / tau;
  mid_thr = 0.5f * (hi_thr + lo_thr);
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

void RLrateSpec::Initialize() {
  on = false;
  Defaults_init();
}

void RLrateSpec::Defaults_init() {
  base = 0.5f;
  act_thr = 0.2f;
}

void DeepSpec::Initialize() {
  on = false;
  role = SUPER;
  Defaults_init();
}

void DeepSpec::Defaults_init() {
  raw_thr_rel = 0.1f;
  raw_thr_abs = 0.1f;
  mod_min = 0.8f;
  trc_p_only_m = false;
  trc_thal_gate = false;
  trc_trace = false;
  mod_range = 1.0f - mod_min;
}

void DeepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  mod_range = 1.0f - mod_min;
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
  drop_thr = 0.1f;
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

  deep_raw_qtr = Q4;

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
  deep.UpdateAfterEdit_NoGui();
  CreateNXX1Fun(act, nxx1_fun, noise_conv);

  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);

  // if(deep.on && deep_raw_qtr == QNULL) { // doesn't make sense to not have any deep raw..
  //   deep_raw_qtr = Q4;
  // }
}

void LeabraUnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(net) {
    // if(dt.integ != 1000.0f * net->times.time_inc) {
    //   taMisc::Warning("unit time integration constant dt.integ of:", (String)dt.integ,
    //                   "does not match network phases.time_inc increment of:",
    //                   (String)net->times.time_inc, "time_inc should be 0.001 * dt.integ");
    // }
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

  u->bias_fwt = 0.0f;
  u->bias_swt = 0.0f;
  u->ext_orig = 0.0f;
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
  u->avg_s_eff = 0.15f;
  u->avg_m = 0.15f;
  u->avg_l = avg_l.init;
  u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  u->r_lrate = 1.0f;
  u->act_avg = 0.15f;
  u->act_raw = 0.0f;
  u->deep_raw = 0.0f;
  u->deep_raw_prv = 0.0f;
  u->deep_mod = 1.0f;
  u->deep_lrn = 1.0f;
  u->deep_ctxt = 0.0f;
  u->deep_mod_net = 0.0f;
  u->deep_raw_net = 0.0f;
  u->thal = 0.0f;
  u->thal_cnt = -1.0f;
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
  u->da_p = 0.0f;
  u->da_n = 0.0f;
  u->sev = 0.0f;
  u->ach = 0.0f;
  u->misc_1 = 0.0f;
  u->misc_2 = 0.0f;
  u->spk_t = -1;

  u->bias_scale = 0.0f;
  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->gi_raw = 0.0f;
  u->deep_raw_sent = 0.0f;

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
  u->misc_2 = 0.0f;

  Init_ActAvg(u, net, thr_no);

  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    cs->Init_Weights_rcgp(recv_gp, net, thr_no);
  }
}

void LeabraUnitSpec::LoadBiasWtVal(float bwt, UnitVars* uv, Network* net) {
  LeabraUnitVars* u = (LeabraUnitVars*)uv;
  u->bias_wt = bwt;
  u->bias_fwt = bwt;
  u->bias_swt = bwt;
}

void LeabraUnitSpec::Init_ActAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  u->act_avg = act_misc.avg_init;
  u->avg_l = act_misc.avg_init;
  u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
}

void LeabraUnitSpec::Init_Netins(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->gi_raw = 0.0f;
  // u->gi_syn = 0.0f;
  // u->net = 0.0f;

  u->deep_raw_net = 0.0f;
  u->deep_mod_net = 0.0f;
  u->deep_raw_sent = 0.0f;

  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    recv_gp->net = 0.0f;
    recv_gp->net_raw = 0.0f;
#ifdef SUGP_NETIN
    if(netin.max_on) {
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      cs->Init_SUGpNetin(recv_gp, net, thr_no);
    }
#endif // SUGP_NETIN
  }
}

void LeabraUnitSpec::Init_Acts(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Acts(u, net, thr_no);

  Init_Netins(u, net, thr_no);

  u->act = init.act;
  u->net = init.netin;

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
  u->avg_s_eff = u->avg_s;
  u->avg_m = act_misc.avg_init;
  u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  u->r_lrate = 1.0f;
  // not avg_l
  // not act_avg
  u->act_raw = 0.0f;
  u->deep_raw = 0.0f;
  u->deep_raw_prv = 0.0f;
  u->deep_mod = 1.0f;
  u->deep_lrn = 1.0f;
  u->deep_ctxt = 0.0f;

  u->thal = 0.0f;
  u->thal_cnt = -1.0f;
  u->gc_i = 0.0f;
  u->I_net = 0.0f;
  u->v_m = init.v_m;
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
  u->da_p = 0.0f;
  u->da_n = 0.0f;
  u->ach = 0.0f;
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
    u->act -= decay * (u->act - init.act);
    u->net -= decay * (u->net - init.netin);
    u->act_eq -= decay * (u->act_eq - init.act);
    u->act_nd -= decay * (u->act_nd - init.act);
    u->act_raw -= decay * (u->act_raw - init.act);
    u->gc_i -= decay * u->gc_i;
    u->v_m -= decay * (u->v_m - init.v_m);
    u->v_m_eq -= decay * (u->v_m_eq - init.v_m);
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
  if(deep.on) {
    net->deep.on = true;
    net->deep.raw_qtr = (LeabraNetDeep::Quarters)(net->deep.raw_qtr | deep_raw_qtr);
  }
  if(bias_spec) {
    ((LeabraConSpec*)bias_spec.SPtr())->Trial_Init_Specs(net);
  }
  
#ifdef SUGP_NETIN
  if(netin.max_on) {
    TestWarning(!net->net_misc.sugp_netin, "Trial_Init_Specs",
                "netin.max_on requires Network net_misc.sugp_net = true -- I will set this now, but you will need to rebuild the network for this to take effect!");
    TestWarning(!net->NetinPerPrjn(), "Trial_Init_Specs",
                "netin.max_on requires Network NETIN_PER_PRJN flag to be set -- I will set this now, but you will need to rebuild the network for this to take effect!");
    net->SetNetFlag(Network::NETIN_PER_PRJN);
  }
#endif // SUGP_NETIN
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
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  
  if(lay->acts_p.avg >= avg_l_2.lay_act_thr) {
    float lval = u->avg_m;
    if(lval > avg_l_2.act_thr) { // above threshold, raise it up
      u->avg_l += avg_l.dt * (avg_l.max - u->avg_l);
    }
    else {
      u->avg_l += avg_l.dt * (avg_l.min - u->avg_l);
    }
  }
  u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  if(avg_l_2.err_mod) {
    float eff_err = MAX(lay->cos_diff_avg_lrn, avg_l_2.err_min);
    u->avg_l_lrn *= eff_err;
  }
  if(lay->layer_type != Layer::HIDDEN || deep.IsTRC()) {
    u->avg_l_lrn = 0.0f;        // no self organizing in non-hidden layers!
  }

  if(adapt_leak.on) {
    adapt_leak.AdaptLeak(u->bias_wt, u->bias_dwt, u->avg_l);
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
  u->da_p = 0.0f;
  u->da_n = 0.0f;
  u->ach = 0.0f;
  u->thal = 0.0f;
  //  u->sev = 0.0f; // longer time-course
  // reset all the time vars so it isn't ambiguous as these update
  if(taMisc::gui_active) {
    u->act_q1 = u->act_q2 = u->act_q3 = u->act_q4 = 0.0f;
    u->act_m = u->act_p = u->act_dif = 0.0f;
  }
}

void LeabraUnitSpec::Trial_NoiseInit(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(noise_type != NO_NOISE && noise_adapt.trial_fixed &&
     (noise.type != Random::NONE)) {
    u->noise = noise.Gen(thr_no);
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
    u->ext_orig = u->ext;
    u->ClearExtFlag(UnitVars::EXT);
  }
  else {
    u->ext = u->targ;
    u->ext_orig = u->ext;
    u->SetExtFlag(UnitVars::EXT);
  }
}

void LeabraUnitSpec::Quarter_Init_PrvVals(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no) {
  if(deep.on && (deep_raw_qtr & Q2)) {
    // if using beta rhythm, this happens at that interval
    if(Quarter_DeepRawPrevQtr(net->quarter)) {
      if(net->quarter == 0)
        u->net_prv_q = u->net_prv_trl; // net was cleared
      else
        u->net_prv_q = u->net;
    }
  }
  else {
    if(net->quarter == 0)
      u->net_prv_q = u->net_prv_trl; // net was cleared
    else
      u->net_prv_q = u->net;
  }
}

void LeabraUnitSpec::Compute_NetinScale(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // this is all receiver-based and done only at beginning of each quarter
  bool plus_phase = (net->phase == LeabraNetwork::PLUS_PHASE);
  Unit* un = u->Un(net, thr_no);
  float net_scale = 0.0f;
  float inhib_net_scale = 0.0f;
  float deep_raw_scale = 0.0f;
  float deep_mod_scale = 0.0f;
  
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
    float rel_scale = cs->wt_scale.rel;
    
    if(cs->inhib) {
      inhib_net_scale += rel_scale;
    }
    else if(cs->IsDeepRawCon()) {
      deep_raw_scale += rel_scale;
    }
    else if(cs->IsDeepModCon()) {
      deep_mod_scale += rel_scale;
    }
    else {                      // DeepCtxtCon gets included in overall netin!
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
    else if(cs->IsDeepRawCon()) {
      if(deep_raw_scale > 0.0f)
        recv_gp->scale_eff /= deep_raw_scale;
    }
    else if(cs->IsDeepModCon()) {
      if(deep_mod_scale > 0.0f)
        recv_gp->scale_eff /= deep_mod_scale;
    }
    else {
      if(net_scale > 0.0f)
        recv_gp->scale_eff /= net_scale;
    }
  }
}

void LeabraUnitSpec::Send_DeepCtxtNetin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  float act_ts = u->deep_raw;
  if(act_ts > opt_thresh.send) {
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      // LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      // if(tol->hard_clamped)      continue;
      LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
      if(cs->IsDeepCtxtCon()) {
        DeepCtxtConSpec* sp = (DeepCtxtConSpec*)cs;
        sp->Send_DeepCtxtNetin(send_gp, net, thr_no, act_ts);
      }
    }
  }
}

void LeabraUnitSpec::Compute_DeepCtxt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;
  // note: this is OK to only integrate selectively b/c not using delta-based netin

  int flat_idx = u->flat_idx;
  int nt = net->n_thrs_built;
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float net_sum = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendDeepRawNetTmp(j)[flat_idx];
    net_sum += ndval;
  }
  u->deep_ctxt = net_sum;
}

void LeabraUnitSpec::Compute_DeepStateUpdt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  u->deep_raw_prv = u->deep_raw; // keep track of what we sent here, for context learning
}

void LeabraUnitSpec::Init_InputData(UnitVars* u, Network* net, int thr_no) {
  inherited::Init_InputData(u, net, thr_no);
  ((LeabraUnitVars*)u)->ext_orig = 0.0f;
}

void LeabraUnitSpec::ApplyInputData_post(LeabraUnitVars* u) {
  if(!u->HasExtFlag(UnitVars::EXT))
    return;
  u->ext_orig = u->ext;
}

void LeabraUnitSpec::Compute_HardClamp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!u->HasExtFlag(UnitVars::EXT))
    return;
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  if(!(ls->clamp.hard && lay->HasExtFlag(UnitVars::EXT))) {
    return;
  }
  float ext_in = u->ext;
  u->act_raw = ext_in;
  if(net->cycle > 0 && deep.ApplyDeepMod()) {
    ext_in *= u->deep_mod;
  }
  u->net = u->thal = ext_in;
  ext_in = clamp_range.Clip(ext_in);
  u->act_eq = u->act_nd = u->act = ext_in;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + ext_in / act.gain;
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
  float ext_in = u->ext;
  u->act_raw = ext_in;
  if(net->cycle > 0 && deep.ApplyDeepMod()) {
    ext_in *= u->deep_mod;
  }
  u->net = u->thal = ext_in;
  // ext_in = clamp_range.Clip(ext_in);
  u->act_eq = u->act_nd = u->act = ext_in;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + ext_in / act.gain;
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
  u->targ = u->ext_orig;        // orig is safer
  u->ext = 0.0f;
}

void LeabraUnitSpec::TargExtToComp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!u->HasExtFlag(UnitVars::TARG_EXT))
    return;
  if(u->HasExtFlag(UnitVars::EXT))
    u->targ = u->ext_orig;      // orig is safer
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
  // if(syn_delay.on) { // todo!
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
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsDeepModCon()) {
          SendDeepModConSpec* sp = (SendDeepModConSpec*)cs;
          sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
        }
        if(!cs->DoesStdNetin()) continue;
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(tol->hard_clamped)      continue;
        if(cs->wt_scale.no_plus_net && net->quarter == 3) {
          // netin typically reset at start of plus phase, so we need to send minus phase
          // activation for first cycle and then stop sending after that
          const int cyc_per_qtr = net->times.quarter;
          int qtr_cyc = net->cycle;
          if(net->cycle > cyc_per_qtr)             // just in case cycle being reset
            qtr_cyc -= net->quarter * cyc_per_qtr; // quarters into this cyc
          if(qtr_cyc > 0) continue;
          float actm_delta = u->act_m - u->act_sent;
          ((LeabraConSpec*)send_gp->con_spec)->Send_NetinDelta(send_gp, net, thr_no,
                                                               actm_delta);
          continue;
        }
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
      LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
      if(cs->IsDeepModCon()) {
        SendDeepModConSpec* sp = (SendDeepModConSpec*)cs;
        sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
      }
      if(!cs->DoesStdNetin()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->hard_clamped)        continue;
      if(cs->wt_scale.no_plus_net && net->quarter == 3) {
        // netin typically reset at start of plus phase, so we need to send minus phase
        // activation for first cycle and then stop sending after that
        const int cyc_per_qtr = net->times.quarter;
        int qtr_cyc = net->cycle;
        if(net->cycle > cyc_per_qtr)
          qtr_cyc -= net->quarter * cyc_per_qtr; // quarters into this cyc
        if(qtr_cyc > 0) continue;
        if(u->act_m > opt_thresh.send) continue; // not actually off!
      }
      ((LeabraConSpec*)send_gp->con_spec)->Send_NetinDelta(send_gp, net, thr_no,
                                                           act_delta);
    }
    u->act_sent = 0.0f;         // now it effectively sent a 0..
  }
}

void LeabraUnitSpec::Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // this integrates from SendDelta into net_raw and gi_syn
  int nt = net->n_thrs_built;
  const int flat_idx = u->flat_idx;
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float net_delta = 0.0f;
  float gi_delta = 0.0f;
#ifdef SUGP_NETIN
  // note: commenting this out to avoid potential confusion -- uncomment if SUGP_NETIN
  // is ever re-activated..
  
  // if(net->net_misc.sugp_netin) {
  //   const int nrg = u->NRecvConGps(net, thr_no);
  //   if(netin.max_on) {
  //     float net_raw = 0.0f;
  //     for(int g=0; g < nrg; g++) {
  //       LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
  //       if(recv_gp->NotActive()) continue;
  //       LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
  //       const int nsgp = MAX(((LeabraPrjn*)recv_gp->prjn)->n_sugps, 1);
  //       int max_idx = 0;
  //       float max_raw = 0.0f;
  //       float sum_raw = 0.0f;
  //       for(int sg=0; sg < nsgp; sg++) {
  //         float sg_net_delta = 0.0f;
  //         for(int j=0;j<nt;j++) {
  //           float& ndval = net->ThrSendNetinTmpPerSugp(j, g, sg)[flat_idx]; 
  //           sg_net_delta += ndval;
  //         }
  //         if(cs->inhib) {
  //           recv_gp->net_raw += sg_net_delta;
  //           gi_delta += sg_net_delta;
  //         }
  //         else if(nsgp > 0 && recv_gp->sugp_net) {
  //           recv_gp->sugp_net[sg] += sg_net_delta; // increment
  //           if(recv_gp->sugp_net[sg] > max_raw) {
  //             max_raw = recv_gp->sugp_net[sg];
  //             max_idx = sg;
  //           }
  //           sum_raw += recv_gp->sugp_net[sg];
  //         }
  //         else {
  //           recv_gp->net_raw += sg_net_delta; // use recv_gp
  //           max_raw = recv_gp->net_raw;
  //         }
  //       }
  //       if(!cs->inhib) {
  //         if(nsgp > 0 && recv_gp->sugp_net) {
  //           if(!netin.max_ff || recv_gp->prjn->direction == Projection::FM_INPUT) {
  //             recv_gp->net_raw = netin.max_mult * max_raw * (float)nsgp +
  //               netin.sum_mix * sum_raw;
  //           }
  //           else {
  //             recv_gp->net_raw = sum_raw;
  //           }
  //         }
  //         net_raw += recv_gp->net_raw;
  //       }
  //     }
  //     u->net_raw = net_raw;
  //   }
  //   else {                      // non max_netin but still sugp_netin
  //     for(int g=0; g < nrg; g++) {
  //       LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
  //       if(recv_gp->NotActive()) continue;
  //       LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
  //       const int nsgp = MAX(((LeabraPrjn*)recv_gp->prjn)->n_sugps, 1);
  //       for(int sg=0; sg < nsgp; sg++) {
  //         float sg_net_delta = 0.0f;
  //         for(int j=0;j<nt;j++) {
  //           float& ndval = net->ThrSendNetinTmpPerSugp(j, g, sg)[flat_idx]; 
  //           sg_net_delta += ndval;
  //         }
  //         recv_gp->net_raw += sg_net_delta;
  //         if(cs->inhib) {
  //           gi_delta += sg_net_delta;
  //         }
  //         else {
  //           net_delta += sg_net_delta;
  //         }
  //       }
  //     }
  //     u->net_raw += net_delta;
  //   }
  // }
  // else
#endif // SUGP_NETIN
  if(net->NetinPerPrjn()) {
    const int nrg = u->NRecvConGps(net, thr_no); 
    for(int g=0; g< nrg; g++) {
      LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
      if(recv_gp->NotActive()) continue;
      float g_net_delta = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
	g_net_delta += ndval;
      }
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      recv_gp->net_raw += g_net_delta; // note: direct assignment to raw, no time integ
      if(cs->inhib) {
        gi_delta += g_net_delta;
      }
      else {
        net_delta += g_net_delta;
      }
    }
    u->net_raw += net_delta;
  }
  else {
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmp(j)[flat_idx];
      net_delta += ndval;
    }
    u->net_raw += net_delta;
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
}

void LeabraUnitSpec::DeepModNetin_Integ(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  int flat_idx = u->flat_idx;
  int nt = net->n_thrs_built;
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float net_delta = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendDeepModNetTmp(j)[flat_idx];
    net_delta += ndval;
  }
  u->deep_mod_net += net_delta;
}

void LeabraUnitSpec::Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->deep.mod_net) {       // if anyone's doing it, we need to integrate!
    DeepModNetin_Integ(u, net, thr_no);
  }
  
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(lay->hard_clamped) {
    // TestWrite(u->thal, 0.0f);     // reset here before thalamic writing
    return;
  }

  Compute_NetinRaw(u, net, thr_no);
  // u->net_raw and u->gi_syn now have proper values integrated from deltas

  float net_syn = act_misc.net_gain * u->net_raw;
  float net_ex = 0.0f;
  if(deep.IsTRC() && Quarter_DeepRawNow(net->quarter)) {
    LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
    if(lay->am_deep_raw_net.max > 0.1f) { // have to get some input to clamp
      if(deep.trc_p_only_m) {
        LeabraUnGpData* ugd = lay->UnGpDataUn(un);
        if(ugd->acts_prvq.max > 0.1f) {
          // only activate if we got prior and current activation
          net_syn = u->deep_raw_net; // only gets from deep!  and no extras!
        }
        else {
          net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);
        }
      }
      else {                       // always do it
        net_syn = u->deep_raw_net; // only gets from deep!  and no extras!
      }
    }
    else {
      net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);
    }
  }
  else {
    net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);  // this could modify net_syn if it wants..
  }
  
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
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE)) {
    u->net += Compute_Noise(u, net, thr_no);
  }
  else if((noise_type == NET_MULT_NOISE) && (noise.type != Random::NONE)) {
    u->net *= Compute_Noise(u, net, thr_no);
  }

  // TestWrite(u->thal, 0.0f);     // reset here before thalamic writing
}

float LeabraUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  float net_ex = init.netin;
  if(bias_spec) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UnitVars::EXT)) {
    net_ex += u->ext * ls->clamp.gain;
  }
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  if(da_mod.on) {
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      net_ex += da_mod.plus * u->da_p * net_syn;
    }
    else {                      // MINUS_PHASE
      net_ex += da_mod.minus * u->da_p * net_syn;
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

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: activation -- rate code

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

void LeabraUnitSpec::Compute_DeepMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(deep.SendDeepMod()) {
    u->deep_lrn = u->deep_mod = u->act;      // record what we send!
  }
  else if(deep.IsTRC()) {
    u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
    if(deep.trc_thal_gate) {
      u->net *= u->thal;
    }
  }
  // must be SUPER units at this point
  else if(lay->am_deep_mod_net.max < 0.1f) { // not enough yet 
    u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100%
  }
  else {
    u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max; // todo: could not normalize this..
    u->deep_mod = deep.mod_min + deep.mod_range * u->deep_lrn;
  }
}

void LeabraUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

  if(deep.on) {
    Compute_DeepMod(u, net, thr_no);
  }

  if((net->cycle >= 0) && lay->hard_clamped) {
    // Compute_HardClamp happens before deep_mod is available due to timing of updates
    if(deep.ApplyDeepMod() && net->cycle == 0) {
      // sync this with Compute_HardClamp:
      float ext_in = u->ext * u->deep_mod;
      u->net = u->thal = ext_in;
      ext_in = clamp_range.Clip(ext_in);
      u->act_eq = u->act_nd = u->act = ext_in;
    }
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

  u->act_raw = new_act;
  if(deep.ApplyDeepMod()) { // apply attention directly to act
    new_act *= u->deep_mod;
  }
  if(deep.IsTRC() && Quarter_DeepRawNow(net->quarter)) {
    if(deep.trc_trace) {
      new_act = MAX(u->act_q0, new_act);
    }
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

  if(deep.on) {
    Compute_DeepMod(u, net, thr_no);
  }

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
    if(Random::Poisson(spike_p, thr_no) > 0.0f) fire_now = true;
    break;
  case SpikeMiscSpec::UNIFORM:
    fire_now = Random::BoolProb(spike_p, thr_no);
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
    if(adapt_leak.on)
      gc_l += u->bias_wt;
    float gc_i = u->gc_i * g_bar.i;

    if(updt_spk_vm) {
      // first compute v_m, using midpoint method:
      float v_m_eff = u->v_m;
      // midpoint method: take a half-step:
      float I_net_1 =
        (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff)) +
        (gc_i * (E_i - v_m_eff)) - u->adapt;
      v_m_eff += .5f * dt.integ * dt.vm_dt * I_net_1; // go half way
      float I_net = (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff))
        + (gc_i * (E_i - v_m_eff)) - u->adapt;
      // add spike current if relevant
      if(spike_misc.ex) {
        I_net += gc_l * spike_misc.exp_slope *
          taMath_float::exp_fast((v_m_eff - act.thr) / spike_misc.exp_slope);
      }
      u->v_m += dt.integ * dt.vm_dt * I_net;
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
    rval = noise.Gen(thr_no);
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
  Compute_SRAvg(u, net, thr_no);
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

  u->avg_s_eff = act_avg.s_in_s * u->avg_s + act_avg.m_in_s * u->avg_m;
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats


///////////////////////////////////////////////////////////////////////
//      Deep layer updating

void LeabraUnitSpec::Compute_DeepRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;

  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  // must use act_raw to compute deep_raw because deep_raw is then the input to deep_norm
  // -- if we use act which is already modulated by deep_norm then we get a compounding
  // effect which doesn't work well at all in practice -- does not allow for dynamic
  // deep_mod updating -- just gets stuck in its own positive feedback cycle.

  // todo: revisit this!!
  
  float thr_cmp = lay->acts_raw.avg +
    deep.raw_thr_rel * (lay->acts_raw.max - lay->acts_raw.avg);
  thr_cmp = MAX(thr_cmp, deep.raw_thr_abs);
  float draw = 0.0f;
  if(u->act_raw >= thr_cmp) {
    draw = u->act_raw;
  }
  u->deep_raw = draw;
}

void LeabraUnitSpec::Send_DeepRawNetin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;

  float act_ts = u->deep_raw; // note: no delay for deep

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->deep_raw_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      const int nsg = u->NSendConGps(net, thr_no); 
      for(int g=0; g< nsg; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
        if(send_gp->NotActive()) continue;
        // LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        // if(tol->hard_clamped)      continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsDeepRawCon()) {
          SendDeepRawConSpec* sp = (SendDeepRawConSpec*)cs;
          sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
        }
      }
      u->deep_raw_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->deep_raw_sent > opt_thresh.send) {
    float act_delta = - u->deep_raw_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      // LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      // if(tol->hard_clamped)        continue;
      LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
      if(cs->IsDeepRawCon()) {
        SendDeepRawConSpec* sp = (SendDeepRawConSpec*)cs;
        sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
      }
    }
    u->deep_raw_sent = 0.0f;         // now it effectively sent a 0..
  }
}

void LeabraUnitSpec::DeepRawNetin_Integ(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // note: no tests here -- ALWAYS run this (network already filters on general need)
  // so that delta can always be tracked!
  int flat_idx = u->flat_idx;
  int nt = net->n_thrs_built;
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float net_delta = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendDeepRawNetTmp(j)[flat_idx];
    net_delta += ndval;
  }
  u->deep_raw_net += net_delta;
}

void LeabraUnitSpec::ClearDeepActs(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  u->deep_raw = 0.0f;
  u->deep_raw_prv = 0.0f;
  u->deep_ctxt = 0.0f;
  u->deep_mod = 1.0f;
  u->deep_lrn = 1.0f;
  u->deep_raw_net = 0.0f;
  u->deep_mod_net = 0.0f;
  u->deep_raw_sent = 0.0f;
}

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
    if(r_lrate.on) {
      if(u->act_p >= r_lrate.act_thr && u->act_q0 >= r_lrate.act_thr) {
        u->r_lrate = 1.0f;
      }
      else {
        u->r_lrate = r_lrate.base;
      }
    }
    Compute_ActTimeAvg(u, net, thr_no);
    break;
  }
}

void LeabraUnitSpec::Compute_ActTimeAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(act_misc.avg_dt <= 0.0f) return;
  u->act_avg += act_misc.avg_dt * (u->act_nd - u->act_avg);
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

  init.v_m = e_rev.l;
  vm_range.min = 0.0f;
  vm_range.max = 2.0f;

  if(act_fun != SPIKE) {
    dt.vm_tau = 3.3f;
    act.gain = 100;
  }

  UpdateAfterEdit();
}

void LeabraUnitSpec::GraphVmFun(DataTable* graph_data, float g_i, float min, float max, float incr) {
  taProject* proj = GetMyProj();
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
  taProject* proj = GetMyProj();
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
  taProject* proj = GetMyProj();
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
//   taProject* proj = GetMyProj();
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


