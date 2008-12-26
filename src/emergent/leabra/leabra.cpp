// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra.h"

#include "ta_platform.h"
#include "netstru_extra.h"

// todo: are these really necessary anymore!?
#include <math.h>
#include <limits.h>
#include <float.h>

static void leabra_converter_init() {
  DumpFileCvt* cvt = new DumpFileCvt("Leabra", "LeabraUnit");
  cvt->repl_strs.Add(NameVar("_MGroup", "_Group"));
  cvt->repl_strs.Add(NameVar("Project", "V3LeabraProject"));
  cvt->repl_strs.Add(NameVar("V3LeabraProject_Group", "Project_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("V3LeabraProjection", "LeabraPrjn"));       // new prjn type!
  cvt->repl_strs.Add(NameVar("LeabraPrjn_Group", "Projection_Group"));
  cvt->repl_strs.Add(NameVar("Network", "LeabraNetwork"));
  cvt->repl_strs.Add(NameVar("LeabraNetwork_Group", "Network_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("LeabraWiz", "LeabraWizard"));
  // obsolete types get replaced with taBase..
  cvt->repl_strs.Add(NameVar("WinView_Group", "taBase_Group"));
  cvt->repl_strs.Add(NameVar("ProjViewState_List", "taBase_List"));
  cvt->repl_strs.Add(NameVar("NetView", "taNBase"));
  cvt->repl_strs.Add(NameVar("DataTable", "taNBase"));
  cvt->repl_strs.Add(NameVar("EnviroView", "taNBase"));
  cvt->repl_strs.Add(NameVar("Xform", "taBase"));
  cvt->repl_strs.Add(NameVar("ImageEnv", "ScriptEnv"));
  cvt->repl_strs.Add(NameVar("unique/w=", "unique"));
  taMisc::file_converters.Add(cvt);
}

void leabra_module_init() {
  ta_Init_leabra();		// initialize types 
  leabra_converter_init();	// configure converter
}

// module initialization
InitProcRegistrar mod_init_leabra(leabra_module_init);

//////////////////////////
//  	Con, Spec	//
//////////////////////////

void WtScaleSpec::Initialize() {
  rel = 1.0f;
  abs = 1.0f;
}

void WtScaleSpecInit::Initialize() {
  init = false;
  rel = 1.0f;
  abs = 1.0f;
}

void WtSigSpec::Initialize() {
  gain = 6.0f;
  off = 1.25f;
}

void WtSigSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(owner) owner->UpdateAfterEdit(); // update our conspec so it can recompute lookup function!
}

void LearnMixSpec::Initialize() {
  hebb = .001f;
  err = 1.0f - hebb;
  err_sb = true;
}

void LearnMixSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  err = 1.0f - hebb;
}

void XCalLearnSpec::Initialize() {
  lrn_var = XCAL;

  dwt_norm = false;

  mvl_mix = 0.002f;

  s_mix = 0.90f;

  l_dt = 0.005f;
  l_gain = 1.8f;

  ml_dt = 0.4f;

  d_gain = 1.0f;
  d_rev = 0.15f;

  svm_mix = 1.0f - mvl_mix;
  m_mix = 1.0f - s_mix;
  d_rev_ratio = (1.0f - d_rev) / d_rev;
}

void XCalLearnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  svm_mix = 1.0f - mvl_mix;
  m_mix = 1.0f - s_mix;
  d_rev_ratio = (1.0f - d_rev) / d_rev;
  if(d_rev > 0.0f)
    d_rev_ratio = (1.0f - d_rev) / d_rev;
  else
    d_rev_ratio = 1.0f;
}

void XCalMiscSpec::Initialize() {
  avg_updt = TRIAL;
  ml_mix = 0.0f;
  m_dt = 0.03f;
  s_dt = 0.1f;

//   lrn_thr = 0.2f;
//   lrn_delay = 200;

  use_nd = false;

  avg_init = 0.15f;
  rnd_min_avg = -1.0f;		// turn off by default
  rnd_var = 0.1f;

  sm_mix = 1.0f - ml_mix;
}

void XCalMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sm_mix = 1.0f - ml_mix;
}

void SAvgCorSpec::Initialize() {
  cor = .4f;
  thresh = .001f;
  norm_con_n = false;
}

void AdaptRelNetinSpec::Initialize() {
  on = false;
  trg_fm_input = .85f;
  trg_fm_output = .15f;
  trg_lateral = 0.0f;
  trg_sum = 1.0f;
  tol_lg = 0.05f;
  tol_sm = 0.2f;
  rel_lrate = .2f;
}

void AdaptRelNetinSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  trg_sum = trg_fm_input + trg_fm_output + trg_lateral;
  TestWarning(fabsf(trg_sum - 1.0f) > .01, "UAE", "target values do not sum to 1");
}

bool AdaptRelNetinSpec::CheckInTolerance(float trg, float val) {
  float tol;
  if(trg > .25f)
    tol = tol_lg * trg;
  else
    tol = tol_sm * trg;
  if(fabsf(trg - val) <= tol) return true;
  return false;
}


void LeabraConSpec::Initialize() {
  min_obj_type = &TA_LeabraCon;
  wt_limits.min = 0.0f;
  wt_limits.max = 1.0f;
  wt_limits.sym = true;
  wt_limits.type = WeightLimits::MIN_MAX;

  learn_rule = LEABRA_CHL;
  inhib = false;

  wt_sig_fun.x_range.min = 0.0f;
  wt_sig_fun.x_range.max = 1.0f;
  wt_sig_fun.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun.UpdateAfterEdit();

  wt_sig_fun_inv.x_range.min = 0.0f;
  wt_sig_fun_inv.x_range.max = 1.0f;
  wt_sig_fun_inv.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun_inv.UpdateAfterEdit();

  wt_sig_fun_lst.off = -1;   wt_sig_fun_lst.gain = -1; // trigger an update
  wt_sig_fun_res = -1.0;

  rnd.mean = .5f;
  rnd.var = .25f;
  lrate = .01f;
  cur_lrate = .01f;
  lrs_value = EPOCH;
  lrate_sched.interpolate = false;
}

void LeabraConSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_LeabraConSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(wt_scale, this);
  taBase::Own(wt_scale_init, this);
  taBase::Own(wt_sig, this);
  taBase::Own(lrate_sched, this);
  taBase::Own(lmix, this);
  taBase::Own(xcal, this);
  taBase::Own(xcalm, this);
  taBase::Own(savg_cor, this);
  taBase::Own(rel_net_adapt, this);
  taBase::Own(wt_sig_fun, this);
  taBase::Own(wt_sig_fun_inv, this);
  taBase::Own(wt_sig_fun_lst, this);
  CreateWtSigFun();
  if(taMisc::is_loading || taMisc::is_duplicating) return;
  LeabraNetwork* mynet = GET_MY_OWNER(LeabraNetwork);
  if(mynet) {
    SetLearnRule(mynet);		// get current learning rule.
  }
}

void LeabraConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lrate_sched.UpdateAfterEdit();
  CreateWtSigFun();
  lmix.UpdateAfterEdit();
  xcal.UpdateAfterEdit();
  xcalm.UpdateAfterEdit();
}

void LeabraConSpec::Defaults() {
  wt_scale.Defaults();
  wt_scale_init.Defaults();
  wt_sig.Defaults();
  lmix.Defaults();
  savg_cor.Defaults();
  Initialize();
}

void LeabraConSpec::SetLearnRule(LeabraNetwork* net) {
  if((int)net->learn_rule == (int)learn_rule) return;
  learn_rule = (LeabraConSpec::LearnRule)net->learn_rule;
  // todo: could set come conflicting params..
}

void LeabraConSpec::SetCurLrate(LeabraNetwork* net, int epoch) {
  cur_lrate = lrate;		// as a backup..
  if(lrs_value == NO_LRS) return;

  if(lrs_value == EXT_REW_AVG) {
    LeabraLayer* er_lay = LeabraLayerSpec::FindLayerFmSpecNet(net, &TA_ExtRewLayerSpec);
    if(er_lay != NULL) {
      LeabraUnit* un = (LeabraUnit*)er_lay->units.Leaf(0);
      float avg_rew = un->act_avg;
      int ar_pct = (int)(100.0f * avg_rew);
      cur_lrate = lrate * lrate_sched.GetVal(ar_pct);
      return;
    }
    else {
      TestWarning(true, "SetCurLrate", "appropriate ExtRew layer not found for EXT_REW_AVG, reverting to EPOCH!");
      SetUnique("lrs_value", true);
      lrs_value = EPOCH;
      UpdateAfterEdit();
    }
  }
  if(lrs_value == EXT_REW_STAT) {
    int arval = 0;
    if(net->epoch < 1) {
      arval = lrate_sched.last_ctr;
    }
    else {
      arval = (int)(100.0f * net->avg_ext_rew);
    }
    cur_lrate = lrate * lrate_sched.GetVal(arval);
  }

  // this is no longer relevant
//   if(lrs_value == SE_STAT) {
//     int seval = 0;
//     if(net->epoch < 1) {
//       seval = lrate_sched.last_ctr;
//     }
//     else {
//       seval = (int);
//     }
//     cur_lrate = lrate * lrate_sched.GetVal(seval);
//   }

  if(lrs_value == EPOCH) {
    cur_lrate = lrate * lrate_sched.GetVal(epoch);
  }
}

void LeabraConSpec::CreateWtSigFun() {
  if((wt_sig_fun_lst.gain == wt_sig.gain) && (wt_sig_fun_lst.off == wt_sig.off)
     && (wt_sig_fun_res == wt_sig_fun.res))
    return;
  wt_sig_fun.AllocForRange();
  int i;
  for(i=0; i<wt_sig_fun.size; i++) {
    float w = wt_sig_fun.Xval(i);
    wt_sig_fun[i] = wt_sig.SigFmLinWt(w);
  }
  wt_sig_fun_inv.AllocForRange();
  for(i=0; i<wt_sig_fun_inv.size; i++) {
    float w = wt_sig_fun_inv.Xval(i);
    wt_sig_fun_inv[i] = wt_sig.LinFmSigWt(w);
  }
  // prevent needless recomputation of this lookup table..
  wt_sig_fun_lst.gain = wt_sig.gain; wt_sig_fun_lst.off = wt_sig.off;
  wt_sig_fun_res = wt_sig_fun.res;
}

void LeabraConSpec::GraphWtSigFun(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_WtSigFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* lnwt = graph_data->FindMakeColName("Wt", idx, VT_FLOAT);
  DataCol* sigwt = graph_data->FindMakeColName("SigWt", idx, VT_FLOAT);
  DataCol* invwt = graph_data->FindMakeColName("InvWt", idx, VT_FLOAT);
  lnwt->SetUserData("MIN", 0.0f);
  lnwt->SetUserData("MAX", 1.0f);
  sigwt->SetUserData("MIN", 0.0f);
  sigwt->SetUserData("MAX", 1.0f);
  invwt->SetUserData("MIN", 0.0f);
  invwt->SetUserData("MAX", 1.0f);

  float x;
  for(x = 0.0f; x <= 1.0f; x += .01f) {
    float sig = wt_sig.SigFmLinWt(x);
    float inv = wt_sig.LinFmSigWt(x);
    graph_data->AddBlankRow();
    lnwt->SetValAsFloat(x, -1);
    sigwt->SetValAsFloat(sig, -1);
    invwt->SetValAsFloat(inv, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraConSpec::GraphXCaldWtFun(DataTable* graph_data, float thr_p) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_XCalFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* sravg = graph_data->FindMakeColName("SRAvg", idx, VT_FLOAT);
  DataCol* dwt = graph_data->FindMakeColName("dWt", idx, VT_FLOAT);
  sravg->SetUserData("MIN", 0.0f);
  sravg->SetUserData("MAX", 1.0f);
  dwt->SetUserData("MIN", -1.0f);
  dwt->SetUserData("MAX", 1.0f);

  float x;
  for(x = 0.0f; x <= 1.0f; x += .01f) {
    float dw = xcal.dWtFun(x, thr_p);
    graph_data->AddBlankRow();
    sravg->SetValAsFloat(x, -1);
    dwt->SetValAsFloat(dw, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraConSpec::GraphXCalSoftBoundFun(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_XCalSoftBoundFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* wt = graph_data->FindMakeColName("Wt", idx, VT_FLOAT);
  DataCol* dwt = graph_data->FindMakeColName("dWt", idx, VT_FLOAT);
  wt->SetUserData("MIN", 0.0f);
  wt->SetUserData("MAX", 1.0f);
  dwt->SetUserData("MIN", 0.0f);
  dwt->SetUserData("MAX", 0.5f);

  float x;
  for(x = 0.0f; x <= 1.0f; x += .01f) {
    float dw = xcal.SymSbFun(x);
    graph_data->AddBlankRow();
    wt->SetValAsFloat(x, -1);
    dwt->SetValAsFloat(dw, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraRecvCons::Initialize() {
  SetConType(&TA_LeabraCon);
  scale_eff = 0.0f;
  savg_cor = 1.0f;
  net = 0.0;
}

void LeabraRecvCons::Copy_(const LeabraRecvCons& cp) {
  scale_eff = cp.scale_eff;
  savg_cor = cp.savg_cor;
  net = cp.net;
}

void LeabraSendCons::Initialize() {
  SetConType(&TA_LeabraCon);
}

void LeabraBiasSpec::Initialize() {
  //   min_obj_type = &TA_RecvCons; // don't bother..
  SetUnique("rnd", true);
  SetUnique("wt_limits", true);
  SetUnique("wt_scale", true);
  SetUnique("wt_scale_init", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;
  wt_limits.min = -1.0f;
  wt_limits.max = 5.0f;
  wt_limits.sym = false;
  wt_limits.type = WeightLimits::NONE;
  dwt_thresh = .1f;
}

bool LeabraBiasSpec::CheckObjectType_impl(TAPtr obj) {
  // don't allow anything to point to a biasspec except the unitspec!
  if(!obj->InheritsFrom(TA_BaseSpec) &&
     !obj->InheritsFrom(TA_LeabraCon)) return false;
  return true;
}

void LeabraBiasSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

//////////////////////////
//  	Unit, Spec	//
//////////////////////////

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

void ActFunSpec::Initialize() {
  thr = .25f;
  gain = 600.0f;
  nvar = .005f;
  avg_dt = .005f;
  avg_init = 0.15f;
  i_thr = STD;
}

void ActFunSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(owner) owner->UpdateAfterEdit(); // update our unitspec so it can recompute lookup function!
}

void SpikeFunSpec::Initialize() {
  g_gain = 5.0f;
  rise = 0.0f;
  decay = 5.0f;
  window = 3;
  v_m_r = 0.0f;
  eq_gain = 10.0f;
  eq_dt = 0.02f;
  hard_gain = .2f;
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

void DepressSpec::Initialize() {
  on = false;
  rec = .2f;
  asymp_act = .5f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  max_amp = 1.0f;
  clamp_norm_max_amp = (.95f * depl + rec) / rec;
}

void DepressSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(rec < .00001f) rec = .00001f;
  if(asymp_act < .00001f) asymp_act = .00001f;
  if(asymp_act > 1.0f) asymp_act = 1.0f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  depl = MAX(depl, 0.0f);
  clamp_norm_max_amp = (.95f * depl + rec) / rec;
}

void SynDelaySpec::Initialize() {
  on = false;
  delay = 4;
}

void OptThreshSpec::Initialize() {
  send = .1f;
  delta = 0.005f;
  learn = 0.01f;
  phase_dif = 0.0f;		// .8 also useful
}

void DtSpec::Initialize() {
  vm = 0.3f;
  net = 0.7f;
  d_vm_max = 0.025f;
  midpoint = false;
  vm_eq_cyc = 0;
  vm_eq_dt = 1.0f;
}

void DaModSpec::Initialize() {
  on = false;
  mod = PLUS_CONT;
  gain = .1f;
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

  sse_tol = .5f;

  act_fun = NOISY_XX1;

  clamp_range.min = .0f;
  clamp_range.max = .95f;
  clamp_range.UpdateAfterEdit();

  vm_range.max = 1.0f;
  vm_range.min = 0.0f;
  vm_range.UpdateAfterEdit();

  v_m_init.type = Random::UNIFORM;
  v_m_init.mean = .15f;
  v_m_init.var = 0.0f;

  g_bar.e = 1.0f;
  g_bar.l = 0.1f;
  g_bar.i = 1.0f;
  g_bar.h = 0.01f;
  g_bar.a = 0.03f;
  e_rev.e = 1.0f;
  e_rev.l = 0.15f;
  e_rev.i = 0.15f;
  e_rev.h = 1.0f;
  e_rev.a = 0.0f;

  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  e_rev_sub_thr.h = e_rev.h - act.thr;
  e_rev_sub_thr.a = e_rev.a - act.thr;

  hyst.b_inc_dt = .05f;
  hyst.b_dec_dt = .05f;
  hyst.a_thr = .8f;
  hyst.d_thr = .7f;

  noise_type = NO_NOISE;
  noise.type = Random::GAUSSIAN;
  noise.var = .001f;

  noise_conv.x_range.min = -.05f;
  noise_conv.x_range.max = .05f;
  noise_conv.res = .001f;
  noise_conv.UpdateAfterEdit();

  nxx1_fun.x_range.min = -.03f;
  nxx1_fun.x_range.max = .20f;
  nxx1_fun.res = .001f;
  nxx1_fun.UpdateAfterEdit();

  CreateNXX1Fun();
}

void LeabraUnitSpec::Defaults() {
  act.Defaults();
  spike.Defaults();
  depress.Defaults();
  syn_delay.Defaults();
  opt_thresh.Defaults();
  dt.Defaults();
  Initialize();
  //  bias_spec.SetSpec(bias_spec.SPtr());
}

void LeabraUnitSpec::InitLinks() {
  //  bias_spec.type = &TA_LeabraBiasSpec;
  inherited::InitLinks();
  children.SetBaseType(&TA_LeabraUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(act, this);
  taBase::Own(spike, this);
  taBase::Own(depress, this);
  taBase::Own(syn_delay, this);
  taBase::Own(opt_thresh, this);
  taBase::Own(clamp_range, this);
  taBase::Own(vm_range, this);
  taBase::Own(v_m_init, this);
  taBase::Own(dt, this);
  taBase::Own(g_bar, this);
  taBase::Own(e_rev, this);
  taBase::Own(e_rev_sub_thr, this);
  taBase::Own(hyst, this);
  taBase::Own(acc, this);
  taBase::Own(maxda, this);
  taBase::Own(noise, this);
  taBase::Own(noise_sched, this);
  taBase::Own(nxx1_fun, this);
  taBase::Own(noise_conv, this);
}

void LeabraUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  clamp_range.UpdateAfterEdit();
  vm_range.UpdateAfterEdit();
  depress.UpdateAfterEdit();
  noise_sched.UpdateAfterEdit();
  spike.UpdateAfterEdit();
  CreateNXX1Fun();
  if(depress.on)
    act_range.max = depress.max_amp;
  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  e_rev_sub_thr.h = e_rev.h - act.thr;
  e_rev_sub_thr.a = e_rev.a - act.thr;
}

void LeabraUnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(da_mod.on && act.i_thr != ActFunSpec::NO_AH, quiet, rval,
		"da_mod is on but act.i_thr != NO_AH -- this is generally required for da modulation to work properly as it operates through the a & h currents, and including them in i_thr computation leads to less clean modulation effects -- I set this for you in spec:", name)) {
    SetUnique("act", true);
    act.i_thr = ActFunSpec::NO_AH; // key for dopamine effects
  }
}

bool LeabraUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  //  Network* net = GET_MY_OWNER(Network);
  bool rval = true;
 
  for(int g=0; g<un->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)un->send.FastEl(g);
    if(send_gp->cons.size < 2) continue;
    Unit* ru = send_gp->Un(0);
    float first_sc = ((LeabraRecvCons*)ru->recv.FastEl(send_gp->recv_idx))->scale_eff;
    for(int j=1; j<send_gp->cons.size; j++) {
      float sc = ((LeabraRecvCons*)ru->recv.FastEl(send_gp->recv_idx))->scale_eff;
      if(un->CheckError((sc != first_sc), quiet, rval,
		    "the effective weight scales for different sending connections within a group are not all the same!  Sending Layer:",
		    send_gp->prjn->from->name, ", Rev Layer:", send_gp->prjn->layer->name,
		    ", first_sc: ", String(first_sc), ", sc: ", String(sc)))
	return false;
    }
  }

  return true;
}

void LeabraUnitSpec::CreateNXX1Fun() {
  // first create the gaussian noise convolver
  noise_conv.AllocForRange();
  int i;
  float var = act.nvar * act.nvar;
  for(i=0; i < noise_conv.size; i++) {
    float x = noise_conv.Xval(i);
    noise_conv[i] = expf(-((x * x) / var));
  }

  // normalize it
  float sum = 0.0f;
  for(i=0; i < noise_conv.size; i++)
    sum += noise_conv[i];
  for(i=0; i < noise_conv.size; i++)
    noise_conv[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fun.x_range.min + noise_conv.x_range.min;
  fun.x_range.max = nxx1_fun.x_range.max + noise_conv.x_range.max;
  fun.res = nxx1_fun.res;
  fun.UpdateAfterEdit();
  fun.AllocForRange();

  if(act_fun == LeabraUnitSpec::NOISY_LINEAR) {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
	val = act.gain * x;
      fun[i] = val;
    }
  }
  else {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
	val = (act.gain * x) / ((act.gain * x) + 1.0f);
      fun[i] = val;
    }
  }

  nxx1_fun.Convolve(fun, noise_conv);
}

void LeabraUnitSpec::Init_Weights(Unit* u) {
  inherited::Init_Weights(u);
  LeabraUnit* lu = (LeabraUnit*)u;
  lu->act_avg = act.avg_init;
  lu->misc_1 = 0.0f;
  lu->misc_2 = 0.0f;
  lu->misc_3 = 0.0f;
  lu->spk_amp = depress.max_amp;
  lu->vcb.hyst = lu->vcb.g_h = 0.0f;
  lu->vcb.hyst_on = false;
  lu->vcb.acc = lu->vcb.g_a = 0.0f;
  lu->vcb.acc_on = false;

  ((LeabraConSpec*)bias_spec.SPtr())->B_Init_RAvg_l((LeabraCon*)u->bias.Cn(0), lu);
}

void LeabraUnitSpec::Init_ActAvg(LeabraUnit* u) {
  u->act_avg = act.avg_init;
  ((LeabraConSpec*)bias_spec.SPtr())->B_Init_RAvg_l((LeabraCon*)u->bias.Cn(0), u);
}  

void LeabraUnitSpec::SetCurLrate(LeabraNetwork* net, int epoch) {
  if(bias_spec.SPtr())
    ((LeabraConSpec*)bias_spec.SPtr())->SetCurLrate(net, epoch);
}

void LeabraUnitSpec::SetLearnRule(LeabraNetwork* net) {
  if(bias_spec.SPtr())
    ((LeabraConSpec*)bias_spec.SPtr())->SetLearnRule(net);
  if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL) {
    opt_thresh.learn = -1;	// some very tiny value could be used instead!
  }
}

////////////////////////////////////////////
//	Stage 0: at start of settling	  // 
////////////////////////////////////////////

/*was void LeabraUnitSpec::Init_Netin(LeabraUnit* u) {
  if(act.send_delta) {
    u->net_delta = 0.0f;
    u->g_i_delta = 0.0f;
    u->net = 0.0f;		// important for soft-clamped layers
    u->gc.i = 0.0f;
  }
  else {
    u->gc.i = 0.0f;
    u->net = u->clmp_net;
  }

  // not initialized by default.
//   LeabraRecvCons* recv_gp;
//   int g;
//   FOR_ITR_GP(LeabraRecvCons, recv_gp, u->recv., g) {
//     recv_gp->net = 0.0f;
//   } 
} */

void LeabraUnitSpec::Init_Acts(LeabraUnit* ru, LeabraLayer*) {
  inherited::Init_Acts(ru);
  //  ru->clmp_net = 0.0f;
  ru->net_scale = 0.0f;
  ru->bias_scale = 0.0f;
  ru->prv_net = 0.0f;
  ru->prv_g_i = 0.0f;
  if(hyst.init) {
    ru->vcb.hyst = ru->vcb.g_h = 0.0f;
    ru->vcb.hyst_on = false;
  }
  if(acc.init) {
    ru->vcb.acc = ru->vcb.g_a = 0.0f;
    ru->vcb.acc_on = false;
  }
  ru->gc.l = 0.0f;
  ru->gc.i = 0.0f;
  ru->gc.h = 0.0f;
  ru->gc.a = 0.0f;
  ru->I_net = 0.0f;
  ru->v_m = v_m_init.Gen();
  ru->da = 0.0f;
  ru->act = 0.0f;
  ru->act_eq = 0.0f;
  ru->act_nd = 0.0f;
  ru->act_p = ru->act_m = ru->act_dif = 0.0f;
  ru->act_m2 = ru->act_p2 = ru->act_dif2 = 0.0f;
  ru->dav = 0.0f;
  ru->maint_h = 0.0f;

  ru->act_sent = 0.0f;
  ru->net_raw = 0.0f;
  ru->net_delta = 0.0f;
  ru->g_i_delta = 0.0f;
  ru->g_i_raw = 0.0f;

  ru->i_thr = 0.0f;
  if(depress.on)
    ru->spk_amp = act_range.max;
  ru->act_buf.Reset();
  ru->spike_buf.Reset();
}

void LeabraUnitSpec::Compute_NetinScale(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  // this is all receiver-based and done only at beginning of settling
  u->net_scale = 0.0f;	// total of scale values for this unit's inputs

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from.ptr();
    if(lay->lesioned() || !recv_gp->cons.size)	continue;
     // this is the normalization value: takes into account target activity of layer
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    WtScaleSpec& wt_scale = cs->wt_scale;
    float savg = lay->kwta.pct;
    if(cs->savg_cor.norm_con_n)	// sometimes it makes sense to just do it by the group n
      recv_gp->scale_eff = wt_scale.NetScale() * (1.0f / ((float)recv_gp->cons.size * savg));
    else
      recv_gp->scale_eff = wt_scale.NetScale() * (1.0f / ((float)lay->units.leaves * savg));
    u->net_scale += wt_scale.rel;
  }
  // add the bias weight into the netinput, scaled by 1/n
  if(u->bias.cons.size) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.SPtr();
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(u->n_recv_cons > 0)
      u->bias_scale /= (float)u->n_recv_cons; // one over n scaling for bias!
  }
  // now renormalize
  if(u->net_scale == 0.0f) return;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from.ptr();
    if(lay->lesioned() || !recv_gp->cons.size)	continue;
    recv_gp->scale_eff /= u->net_scale; // normalize by total connection scale
  }
}

void LeabraUnitSpec::Compute_NetinRescale(LeabraUnit* u, LeabraLayer*, LeabraNetwork*, float new_scale) {
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from.ptr();
    if(lay->lesioned() || !recv_gp->cons.size)	continue;
    recv_gp->scale_eff *= new_scale;
  }
  // rescale existing netins so that the delta is not even noticed!
  u->net *= new_scale;
  u->clmp_net *= new_scale;
  u->net_raw *= new_scale;
}

void LeabraUnitSpec::Init_ClampNet(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  u->clmp_net = 0.0f;
  if(u->bias.cons.size) {		// add bias weight
    u->clmp_net += u->bias_scale * u->bias.Cn(0)->wt;
  }
}

void LeabraUnitSpec::Send_ClampNet(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(u->act > opt_thresh.send) {
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->prjn->layer->lesioned() || !send_gp->cons.size) continue;
      if(TestWarning(((LeabraConSpec*)send_gp->GetConSpec())->inhib, "Send_ClampNet",
		     "cannot send inhibition from a hard-clamped layer!  Set layerspec clamp.hard off!")) {
	continue;
      }
      send_gp->Send_ClampNet(u->act);
    }
  }
}

////////////////////////////////////
//	Stage 1: netinput 	  //
////////////////////////////////////

void LeabraUnitSpec::Send_Netin(LeabraUnit* u, LeabraLayer*, LeabraNetwork* net) {
  // sender-based (and this fun not called on hard_clamped EXT layers)
  net->send_pct_tot++;
  float act_ts = u->act;
  if(syn_delay.on) {
    act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
  }
  if(act_ts > opt_thresh.send) {
    net->send_pct_n++;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size) continue;
      send_gp->Send_Netin(act_ts);
    }
  }
}

void LeabraUnitSpec::Send_NetinDelta(LeabraUnit* u, LeabraLayer*, LeabraNetwork* net) {
  net->send_pct_tot++;
  float act_ts = u->act;
  if(syn_delay.on) {
    act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
  }

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->act_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      net->send_pct_n++;
      for(int g=0; g<u->send.size; g++) {
	LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size)	continue;
	send_gp->Send_NetinDelta(act_delta);
      }
      u->act_sent = act_ts;	// cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    net->send_pct_n++;
    float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size)	continue;
      send_gp->Send_NetinDelta(act_delta);
    }
    u->act_sent = 0.0f;		// now it effectively sent a 0..
  }
}

//////////////////////////////////////////////////////////////////
//	Stage 2: netinput averages and clamping (if necc)	//
//////////////////////////////////////////////////////////////////

void LeabraUnitSpec::Compute_HardClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  u->net = u->prv_net = u->ext * lay->stm_gain;
  u->act_eq = clamp_range.Clip(u->ext);
  u->act_nd = u->act_eq;
  if(act_fun == SPIKE)
    u->act = spike.hard_gain * u->act_eq;
  else
    u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
  u->AddToActBuf(syn_delay);
}

// NOTE: these two functions should always be the same modulo the clamp_range.Clip

void LeabraUnitSpec::Compute_HardClampNoClip(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  u->net = u->prv_net = u->ext * lay->stm_gain;
  //  u->act_eq = clamp_range.Clip(u->ext);
  u->act_eq = u->ext;
  u->act_nd = u->act_eq;
  if(act_fun == SPIKE)
    u->act = spike.hard_gain * u->act_eq;
  else
    u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
  u->AddToActBuf(syn_delay);
}

bool LeabraUnitSpec::Compute_SoftClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  bool inc_gain = false;
  if(u->ext_flag & Unit::EXT) {
    if((u->ext > .5f) && (u->act < .5f))
      inc_gain = true;	// must increase gain because targ unit is < .5..

    u->net += u->ext * lay->stm_gain;
  }
  return inc_gain;
}

//////////////////////////////////////////
//	Stage 3: inhibition		//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_Netin_Spike(LeabraUnit* u, LeabraLayer* lay, LeabraInhib*,
					 LeabraNetwork* net) {
  // netin gets added at the end of the spike_buf -- 0 time is the end
  u->spike_buf.CircAddLimit(u->net, spike.window); // add current net to buffer
  int mx = MAX(spike.window, u->spike_buf.length);
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net
    for(int t=0;t<mx;t++) {
      sum += u->spike_buf.CircSafeEl(t);
    }
    sum /= (float)spike.window;	// normalize over window
    u->net = u->prv_net + spike.gg_decay * sum - (u->prv_net * spike.oneo_decay);
    u->prv_net = u->net;
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = u->spike_buf.CircSafeEl(t);
      if(spkin > 0.0f) {
	sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    u->net = sum;
    // from compute_netinavg
    u->net = u->prv_net + dt.net * (u->net - u->prv_net);
    u->prv_net = u->net;
  }
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += noise_sched.GetVal(net->cycle) * noise.Gen();
  }
  u->i_thr = Compute_IThresh(u, lay, net);
}

float LeabraUnitSpec::Compute_IThreshStd(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // including the ga and gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a + u->gc.h * e_rev_sub_thr.h) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoA(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // NOT including the ga term
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.h * e_rev_sub_thr.h) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoH(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // NOT including the gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoAH(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // NOT including the ga and gh terms
  return ((non_bias_net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshAll(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  // including the ga and gh terms and bias weights
  return ((u->net * e_rev_sub_thr.e + u->gc.l * e_rev_sub_thr.l
	   + u->gc.a * e_rev_sub_thr.a + u->gc.h * e_rev_sub_thr.h) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThresh(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  switch(act.i_thr) {
  case ActFunSpec::STD:
    return Compute_IThreshStd(u, lay, net);
  case ActFunSpec::NO_A:
    return Compute_IThreshNoA(u, lay, net);
  case ActFunSpec::NO_H:
    return Compute_IThreshNoH(u, lay, net);
  case ActFunSpec::NO_AH:
    return Compute_IThreshNoAH(u, lay, net);
  case ActFunSpec::ALL:
    return Compute_IThreshAll(u, lay, net);
  }
  return 0.0f;
} 

//////////////////////////////////////////
//	Stage 4: the final activation 	//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_Act(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net) {
  Compute_Conduct(u, lay, thr, net);
  Compute_Vm(u, lay, thr, net);
  Compute_ActFmVm(u, lay, thr, net);
  Compute_SelfReg_Cycle(u, lay, thr, net);
  u->AddToActBuf(syn_delay);
}

void LeabraUnitSpec::Compute_DaMod_PlusCont(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net) {
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    if(u->dav > 0.0f) {
      u->vcb.g_a = 0.0f;
      u->vcb.g_h = u->maint_h + da_mod.gain * u->dav * u->act_m; // increase in proportion to participation in minus phase
    }
    else {
      u->vcb.g_h = u->maint_h;
      u->vcb.g_a = -da_mod.gain * u->dav * u->act_m; // decrease in proportion to participation in minus phase
    }
  }
  else {
    u->vcb.g_h = u->maint_h;
    u->vcb.g_a = 0.0f;	// clear in minus phase!
  }
}

void LeabraUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net) {
  if(da_mod.on && (da_mod.mod == DaModSpec::PLUS_CONT)) {
    Compute_DaMod_PlusCont(u,lay,thr,net);
  }

  // total conductances
  float g_bar_i_val = g_bar.i;
  float	g_bar_e_val = g_bar.e;
  float	g_bar_l_val = g_bar.l;
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->spec.SPtr();
  if((ls->adapt_i.type == AdaptISpec::G_BAR_I) || (ls->adapt_i.type == AdaptISpec::G_BAR_IL))
    g_bar_i_val = lay->adapt_i.g_bar_i; // adapting value..
  if(ls->adapt_i.type == AdaptISpec::G_BAR_IL)
    g_bar_l_val = lay->adapt_i.g_bar_l; // adapting value..
  u->gc.i *= g_bar_i_val;
  u->net *= g_bar_e_val;
  u->gc.l = g_bar_l_val;
  u->gc.h = g_bar.h * u->vcb.g_h;
  u->gc.a = g_bar.a * u->vcb.g_a;
}

void LeabraUnitSpec::Compute_Vm(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork* net) {
  if(net->cycle < dt.vm_eq_cyc) {
    // directly go to equilibrium value
    float new_v_m= (((u->net * e_rev.e) + (u->gc.l * e_rev.l) + (u->gc.i * e_rev.i) +
		     (u->gc.h * e_rev.h) + (u->gc.a * e_rev.a)) / 
		    (u->net + u->gc.l + u->gc.i + u->gc.h + u->gc.a));
    u->I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
    u->v_m += dt.vm_eq_dt * u->I_net;
  }
  else {
    float v_m_eff = u->v_m;
    if(dt.midpoint) {
      float I_net_1 = 
	(u->net * (e_rev.e - v_m_eff)) + (u->gc.l * (e_rev.l - v_m_eff)) + 
	(u->gc.i * (e_rev.i - v_m_eff)) + (u->gc.h * (e_rev.h - v_m_eff)) +
	(u->gc.a * (e_rev.a - v_m_eff));
      v_m_eff += .5f * dt.vm * I_net_1; // go half way
    }
    u->I_net = 
      (u->net * (e_rev.e - v_m_eff)) + (u->gc.l * (e_rev.l - v_m_eff)) + 
      (u->gc.i * (e_rev.i - v_m_eff)) + (u->gc.h * (e_rev.h - v_m_eff)) +
      (u->gc.a * (e_rev.a - v_m_eff));
    float dvm = dt.vm * u->I_net;
    if(dvm > dt.d_vm_max) dvm = dt.d_vm_max;
    else if(dvm < -dt.d_vm_max) dvm = -dt.d_vm_max;
    u->v_m += dvm;
  }

  if((noise_type == VM_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->v_m += noise_sched.GetVal(net->cycle) * noise.Gen();
  }

  u->v_m = vm_range.Clip(u->v_m);
}

void LeabraUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
				     LeabraNetwork* net) {
  if(act_fun == SPIKE) {
    Compute_ActFmVm_spike(u, lay, thr, net); 
  }
  else {
    Compute_ActFmVm_rate(u, lay, thr, net); 
  }
}

void LeabraUnitSpec::Compute_ActFmVm_rate(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
					  LeabraNetwork* net) {

  float thr_vm = u->v_m - act.thr; // thresholded vm
  float new_act = 0.0f;
  switch(act_fun) {
  case NOISY_XX1: {
    if(thr_vm <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(thr_vm >= nxx1_fun.x_range.max) {
      thr_vm *= act.gain;
      new_act = thr_vm / (thr_vm + 1.0f);
    }
    else {
      new_act = nxx1_fun.Eval(thr_vm);
    }
  }
  break;
  case XX1: {
    if(thr_vm < 0.0f)
      new_act = 0.0f;
    else {
      thr_vm *= act.gain;
      new_act = thr_vm / (thr_vm + 1.0f);
    }
  }
  break;
  case NOISY_LINEAR: {
    if(thr_vm <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(thr_vm >= nxx1_fun.x_range.max) {
      new_act = thr_vm * act.gain;
    }
    else {
      new_act = nxx1_fun.Eval(thr_vm);
    }
  }
  break;
  case LINEAR: {
    if(thr_vm < 0.0f)
      new_act = 0.0f;
    else
      new_act = thr_vm * act.gain;
  }
  break;
  case SPIKE:
    break;			// compiler food
  }

  if(depress.on) {		     // synaptic depression
    u->act_nd = act_range.Clip(new_act); // nd is non-discounted activation!!! solves tons of probs
    new_act *= u->spk_amp;
    u->spk_amp += -new_act * depress.depl + (depress.max_amp - u->spk_amp) * depress.rec;
    if(u->spk_amp < 0.0f) 			u->spk_amp = 0.0f;
    else if(u->spk_amp > depress.max_amp)	u->spk_amp = depress.max_amp;
  }

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += noise_sched.GetVal(net->cycle) * noise.Gen();
  }
  u->act = act_range.Clip(new_act);
  u->act_eq =u->act;
  if(!depress.on)
    u->act_nd = u->act_eq;
}

void LeabraUnitSpec::Compute_ActFmVm_spike(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
					   LeabraNetwork* net) {
  if(u->v_m > act.thr) {
    u->act = 1.0f;
    u->v_m = spike.v_m_r;
  }
  else {
    u->act = 0.0f;
  }

  float old_nd = u->act_nd / spike.eq_gain;
  float new_nd;
  if(spike.eq_dt > 0.0f) {
    new_nd = act_range.Clip(spike.eq_gain * ((1.0f - spike.eq_dt) * old_nd + spike.eq_dt * u->act));
  }
  else {			// increment by phase
    if(net->cycle > 0)
      old_nd *= (float)net->cycle;
    new_nd = act_range.Clip(spike.eq_gain * (old_nd + u->act) / (float)(net->cycle+1));
  }
  u->da = new_nd - u->act_nd;	// da is on equilibrium activation
  u->act_nd = new_nd;

  if(depress.on) {
    u->act *= u->spk_amp;	// after eq
    u->spk_amp += -u->act * depress.depl + (depress.max_amp - u->spk_amp) * depress.rec;
    if(u->spk_amp < 0.0f) 			u->spk_amp = 0.0f;
    else if(u->spk_amp > depress.max_amp)	u->spk_amp = depress.max_amp;
    u->act_eq = u->spk_amp * u->act_nd; // act_eq is depressed rate code, nd is non-depressed!
  }
  else {
    u->act_eq = u->act_nd;	// eq = nd
  }
}

void LeabraUnitSpec::Compute_SelfReg_Cycle(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork*) {
  // fast time scale updated every cycle
  if(hyst.on && !hyst.trl)
    hyst.UpdateBasis(u->vcb.hyst, u->vcb.hyst_on, u->vcb.g_h, u->act_eq);
  if(acc.on && !acc.trl)
    acc.UpdateBasis(u->vcb.acc, u->vcb.acc_on, u->vcb.g_a, u->act_eq);
}

void LeabraUnitSpec::Compute_SelfReg_Trial(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  // slow time scale updated at end of trial
  if(hyst.on && hyst.trl)
    hyst.UpdateBasis(u->vcb.hyst, u->vcb.hyst_on, u->vcb.g_h, u->act_eq);
  if(acc.on && acc.trl)
    acc.UpdateBasis(u->vcb.acc, u->vcb.acc_on, u->vcb.g_a, u->act_eq);
}

void LeabraUnitSpec::Compute_MaxDa(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
				   LeabraNetwork* net) {
  float fda;
  if(maxda.val == MaxDaSpec::DA_ONLY)
    fda = fabsf(u->da);
  else if(maxda.val == MaxDaSpec::INET_ONLY)
    fda = fabsf(maxda.inet_scale * u->I_net);
  else {
    if(lay->acts.avg <= maxda.lay_avg_thr)
      fda = fabsf(maxda.inet_scale * u->I_net);
    else
      fda = fabsf(u->da);
  }
  lay->maxda = MAX(fda, lay->maxda);
  thr->maxda = MAX(fda, thr->maxda);
  net->maxda = MAX(fda, net->maxda);
}

void LeabraUnitSpec::Compute_CycSynDep(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_CycSynDep(u);
  }
}


//////////////////////////////////////////
//	Stage 5: Between Events 	//
//////////////////////////////////////////

void LeabraUnitSpec::PhaseInit(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(!(u->ext_flag & Unit::TARG))
    return;

  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    if(!(lay->ext_flag & Unit::TARG)) {	// layer isn't a target but unit is..
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

void LeabraUnitSpec::DecayPhase(LeabraUnit* u, LeabraLayer*, LeabraNetwork*, float decay) {
  u->v_m -= decay * (u->v_m - v_m_init.mean);
  u->act -= decay * u->act;
  u->act_nd -= decay * u->act_nd;
  u->act_eq -= decay * u->act_eq;
  u->prv_net -= decay * u->prv_net;
  u->prv_g_i -= decay * u->prv_g_i;
  if(hyst.on && !hyst.trl)
    u->vcb.hyst -= decay * u->vcb.hyst;
  if(acc.on && !acc.trl)
    u->vcb.acc -= decay * u->vcb.acc;
  if(depress.on)
    u->spk_amp += (act_range.max - u->spk_amp) * decay;

  // reset the rest of this stuff just for clarity
  u->act_sent = 0.0f;
  u->net_raw = 0.0f;
  u->net_delta = 0.0f;
  u->g_i_raw = 0.0f;
  u->g_i_delta = 0.0f;
  
  u->net = 0.0f;
  u->net_scale = u->bias_scale = 0.0f;
  u->da = u->I_net = 0.0f;

  if(decay == 1.0f) {
    u->act_buf.Reset();
    u->spike_buf.Reset();
  }
}

void LeabraUnitSpec::DecayEvent(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay) {
  LeabraUnitSpec::DecayPhase(u, lay, net, decay);
  u->dav = 0.0f;
}

void LeabraUnitSpec::ExtToComp(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(!(u->ext_flag & Unit::EXT))
    return;
  u->ext_flag = Unit::COMP;
  u->targ = u->ext;
  u->ext = 0.0f;
}

void LeabraUnitSpec::TargExtToComp(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(!(u->ext_flag & Unit::TARG_EXT))
    return;
  if(u->ext_flag & Unit::EXT)
    u->targ = u->ext;
  u->ext = 0.0f;
  u->ext_flag = Unit::COMP;
}

void LeabraUnitSpec::Compute_ActTimeAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
					LeabraNetwork* net)
{
  if(act.avg_dt <= 0.0f) return;
  u->act_avg += act.avg_dt * (u->act_eq - u->act_avg);
}

void LeabraUnitSpec::Compute_DaMod_PlusPost(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
					    LeabraNetwork* net)
{
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

void LeabraUnitSpec::PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
				LeabraNetwork* net)
{
  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  switch(net->phase_order) {
  case LeabraNetwork::MINUS_PLUS:
    if(no_plus_testing) {
      u->act_m = u->act_p = u->act_nd;
      u->act_dif = 0.0f;
      Compute_ActTimeAvg(u, lay, thr, net);
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE)
	u->act_m = u->act_nd;
      else {
	u->act_p = u->act_nd;
	u->act_dif = u->act_p - u->act_m;
	Compute_DaMod_PlusPost(u,lay,thr,net);
	Compute_ActTimeAvg(u, lay, thr, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_MINUS:
    if(no_plus_testing) {
      u->act_m = u->act_p = u->act_nd;
      u->act_dif = 0.0f;
      Compute_ActTimeAvg(u, lay, thr, net);
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE) {
	u->act_m = u->act_nd;
	u->act_dif = u->act_p - u->act_m;
      }
      else {
	u->act_p = u->act_nd;
	Compute_ActTimeAvg(u, lay, thr, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_ONLY:
    u->act_m = u->act_p = u->act_nd;
    u->act_dif = 0.0f;
    Compute_ActTimeAvg(u, lay, thr, net);
    break;
  case LeabraNetwork::MINUS_PLUS_NOTHING:
  case LeabraNetwork::MINUS_PLUS_MINUS:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      u->act_m = u->act_nd;
    }
    else if(net->phase_no == 1) {
      u->act_p = u->act_nd;
      u->act_dif = u->act_p - u->act_m;
      if(no_plus_testing) {
	u->act_m = u->act_nd;	// update act_m because it is actually another test case!
      }
      Compute_DaMod_PlusPost(u,lay,thr,net);
      Compute_ActTimeAvg(u, lay, thr, net);
    }
    else {
      u->act_m2 = u->act_nd;
      u->act_dif2 = u->act_p - u->act_m2;
    }
    break;
  case LeabraNetwork::PLUS_NOTHING:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      u->act_p = u->act_nd;
      Compute_ActTimeAvg(u, lay, thr, net);
    }
    else {
      u->act_m = u->act_nd;
      u->act_dif = u->act_p - u->act_m;
    }
    break;
  case LeabraNetwork::MINUS_PLUS_PLUS:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      u->act_m = u->act_nd;
    }
    else if(net->phase_no == 1) {
      u->act_p = u->act_nd;
      Compute_DaMod_PlusPost(u,lay,thr,net);
      Compute_ActTimeAvg(u, lay, thr, net);
      u->act_dif = u->act_p - u->act_m;
    }
    else {
      u->act_p2 = u->act_nd;
      u->act_dif2 = u->act_p2 - u->act_p;
    }
    break;
  case LeabraNetwork::MINUS_PLUS_PLUS_NOTHING:
  case LeabraNetwork::MINUS_PLUS_PLUS_MINUS:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      u->act_m = u->act_nd;
    }
    else if(net->phase_no == 1) {
      u->act_p = u->act_nd;
      u->act_dif = u->act_p - u->act_m;
      Compute_DaMod_PlusPost(u,lay,thr,net);
      Compute_ActTimeAvg(u, lay, thr, net);
    }
    else if(net->phase_no == 2) {
      u->act_p2 = u->act_nd;
      u->act_dif2 = u->act_p2 - u->act_p;
    }
    else {
      u->act_m2 = u->act_nd;
      u->act_dif2 = u->act_p2 - u->act_m2;
    }
    break;
  }
}

//////////////////////////////////////////
//	Stage 6: Learning 		//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_SRAvg(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net,
				   bool do_s) {
  // always do bias b/c it is cheap, might be useful..
  LeabraConSpec* bias_sp = (LeabraConSpec*)bias_spec.SPtr();
  bias_sp->B_Compute_SRAvg((LeabraCon*)u->bias.Cn(0), u, do_s);

  if(net->train_mode != LeabraNetwork::TEST) {	// expensive con-level only for training
    if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL) {
      for(int g=0; g<u->recv.size; g++) {
	LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
	if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
	recv_gp->Compute_SRAvg(u, do_s);
      }
    }
    else if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL) {
      for(int g=0; g<u->recv.size; g++) {
	LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
	if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
	if((cs->xcal.lrn_var == XCalLearnSpec::XCAL) ||
	   (cs->xcal.lrn_var == XCalLearnSpec::XCAL_CHL)) continue;
	recv_gp->Compute_SRAvg(u, do_s);
      }
    }
  }
}

void LeabraUnitSpec::Init_SRAvg(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  ((LeabraConSpec*)bias_spec.SPtr())->B_Init_SRAvg((LeabraCon*)u->bias.Cn(0), u, lay);

  if(net->learn_rule >= LeabraNetwork::CTLEABRA_CAL) {
    for(int g = 0; g < u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Init_SRAvg(u);
    }
  }
}

void LeabraUnitSpec::Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule >= LeabraNetwork::CTLEABRA_CAL) {
    LeabraConSpec* bsp = (LeabraConSpec*)bias_spec.SPtr();
    if(bsp->xcalm.avg_updt == XCalMiscSpec::TRIAL) {
      if(lay->sravg_m_nrm * ((LeabraCon*)u->bias.Cn(0))->sravg_m < opt_thresh.learn) return;
    }
    else {
      if(((LeabraCon*)u->bias.Cn(0))->sravg_m < opt_thresh.learn) return;
    }
  }
  else {
    if((u->act_p <= opt_thresh.learn) && (u->act_m <= opt_thresh.learn))
      return;
    if(lay->phase_dif_ratio < opt_thresh.phase_dif)
      return;
  }
  Compute_dWt_impl(u, lay, net);
}

void LeabraUnitSpec::Compute_dWt_impl(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::LEABRA_CHL) {
    for(int g = 0; g < u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Compute_dWt_LeabraCHL(u);
    }
    ((LeabraConSpec*)bias_spec.SPtr())->B_Compute_dWt_LeabraCHL((LeabraCon*)u->bias.Cn(0), u);
  }
  else if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL) {
    for(int g = 0; g < u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Compute_dWt_CtLeabraCAL(u);
    }
    ((LeabraConSpec*)bias_spec.SPtr())->B_Compute_dWt_CtLeabraCAL((LeabraCon*)u->bias.Cn(0),
								  u, lay);
  }
  else if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL) {
    if(net->epoch < net->ct_time.n_avg_only_epcs) { // no learning while gathering data!
      return;
    }
    for(int g = 0; g < u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Compute_dWt_CtLeabraXCAL(u);
    }
    ((LeabraConSpec*)bias_spec.SPtr())->B_Compute_dWt_CtLeabraXCAL((LeabraCon*)u->bias.Cn(0),
								   u, lay);
  }
}

void LeabraUnitSpec::Compute_Weights(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  ((LeabraConSpec*)bias_spec.SPtr())->B_Compute_Weights((LeabraCon*)u->bias.Cn(0), u);

  if(net->learn_rule == LeabraNetwork::LEABRA_CHL) {
    for(int g = 0; g < u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Compute_Weights_LeabraCHL(u);
    }
  }
  else if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL) {
    for(int g = 0; g < u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Compute_Weights_CtLeabraCAL(u);
    }
  }
  else if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL) {
    if(net->epoch < net->ct_time.n_avg_only_epcs) { // no learning while gathering data!
      return;
    }
    for(int g = 0; g < u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Compute_Weights_CtLeabraXCAL(u);
    }
  }
}

float LeabraUnitSpec::Compute_SSE(bool& has_targ, Unit* u) {
  has_targ = false;
  LeabraUnit* lu = (LeabraUnit*)u;
  if(lu->ext_flag & (Unit::TARG | Unit::COMP)) {
    has_targ = true;
    float uerr = lu->targ - lu->act_m;
    if(fabsf(uerr) < sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  else
    return 0.0f;
}

float LeabraUnitSpec::Compute_NormErr(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(!(u->ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;

  if(net->on_errs) {
    if(u->act_m > 0.5f && u->targ < 0.5f) return 1.0f;
  }
  if(net->off_errs) {
    if(u->act_m < 0.5f && u->targ > 0.5f) return 1.0f;
  }
  return 0.0f;
}

//////////////////////////////////////////
//	 Misc Functions 		//
//////////////////////////////////////////

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

  float x;
  for(x = min; x <= max; x += incr) {
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

  float x;
  for(x = min; x <= max; x += incr) {
    un.v_m = x;
    Compute_ActFmVm(&un, NULL, NULL, net);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::GraphActFmNetFun(DataTable* graph_data, float g_i, float min, float max, float incr) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFmNetFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Act", idx, VT_FLOAT);

  LeabraUnit un;
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  float x;
  for(x = min; x <= max; x += incr) {
    un.v_m = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
      ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
    Compute_ActFmVm(&un, NULL, NULL, net);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::TimeExp(int mode, int nreps) {

  const char* modes[6] = {"double sum", "double exp", "double exp_fast",
			  "float sum", "float exp", "float exp_fast"};

  TimeUsedHR tu;
  tu.StartTimer(true);		// start, w/reset
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
  cerr << "mode: " << mode << " " << modes[mode] << " seconds used: " << tu.s_used
       << " dsum: " << dsum << " fsum: " << fsum << endl;
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

//////////////////////////
//  	Unit 		//
//////////////////////////


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
  act_nd = 0.0f;
  act_avg = 0.15f;
  ravg_l = 0.15f;
  ravg_ml = 0.15f;
  act_p = act_m = act_dif = 0.0f;
  act_m2 = act_p2 = act_dif2 = 0.0f;
  da = 0.0f;
  I_net = 0.0f;
  v_m = 0.0f;
  dav = 0.0f;
  maint_h = 0.0f;

  in_subgp = false;
  clmp_net = 0.0f;
  net_scale = 0.0f;
  bias_scale = 0.0f;
  prv_net = 0.0f;
  prv_g_i = 0.0f;

  act_sent = 0.0f;
  net_raw = 0.0f;
  net_delta = 0.0f;
  g_i_delta = 0.0f;
  g_i_raw = 0.0f;

  i_thr = 0.0f;
  spk_amp = 1.0f;
  misc_1 = 0.0f;
  misc_2 = 0.0f;
  misc_3 = 0.0f;
}

void LeabraUnit::InitLinks() {
  inherited::InitLinks();
  taBase::Own(vcb, this);
  taBase::Own(gc, this);
  taBase::Own(act_buf, this);
  taBase::Own(spike_buf, this);
  GetInSubGp();
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
  act_nd = cp.act_nd;
  act_avg = cp.act_avg;
  ravg_l = cp.ravg_l;
  ravg_ml = cp.ravg_ml;
  act_m = cp.act_m;
  act_p = cp.act_p;
  act_dif = cp.act_dif;
  act_m2 = cp.act_m2;
  act_p2 = cp.act_p2;
  act_dif2 = cp.act_dif2;
  da = cp.da;
  vcb = cp.vcb;
  gc = cp.gc;
  I_net = cp.I_net;
  v_m = cp.v_m;
  dav = cp.dav;
  maint_h = cp.maint_h;
  // not: in_subgp
  clmp_net = cp.clmp_net;
  net_scale = cp.net_scale;
  bias_scale = cp.bias_scale;
  prv_net = cp.prv_net;
  prv_g_i = cp.prv_g_i;
  act_sent = cp.act_sent;
  net_raw = cp.net_raw;
  net_delta = cp.net_delta;
  g_i_raw = cp.g_i_raw;
  g_i_delta = cp.g_i_delta;
  i_thr = cp.i_thr;
  spk_amp = cp.spk_amp;
  misc_1 = cp.misc_1;
  misc_2 = cp.misc_2;
  misc_3 = cp.misc_3;
  act_buf = cp.act_buf;
  spike_buf = cp.spike_buf;
}

void LeabraUnit::Init_Netin() {
  gc.i = 0.0f;
  net = clmp_net;
}

void LeabraUnit::Init_NetinDelta() {
  net_delta = 0.0f;
  g_i_delta = 0.0f;
  net = 0.0f;		// important for soft-clamped layers
  gc.i = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
//			Projection Level Code

void LeabraPrjn::Initialize() {
  netin_avg = 0.0f;
  netin_rel = 0.0f;

  avg_netin_avg = 0.0f;
  avg_netin_avg_sum = 0.0f;
  avg_netin_rel = 0.0f;
  avg_netin_rel_sum = 0.0f;
  avg_netin_n = 0;

  trg_netin_rel = -1.0f;		// indicates not set
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
  DMem_InitAggs();
#endif
}

void LeabraPrjn::Destroy() {
}

void LeabraPrjn::Copy_(const LeabraPrjn& cp) {
  netin_avg = cp.netin_avg;
  netin_rel = cp.netin_rel;

  avg_netin_avg = cp.avg_netin_avg;
  avg_netin_avg_sum = cp.avg_netin_avg_sum;
  avg_netin_rel = cp.avg_netin_rel;
  avg_netin_rel_sum = cp.avg_netin_rel_sum;
  avg_netin_n = cp.avg_netin_n;

  trg_netin_rel = cp.trg_netin_rel;
}

void LeabraPrjn::SetCurLrate(LeabraNetwork* net, int epoch) {
  if(con_spec.SPtr())
    ((LeabraConSpec*)con_spec.SPtr())->SetCurLrate(net, epoch);
}

void LeabraPrjn::SetLearnRule(LeabraNetwork* net) {
  if(con_spec.SPtr())
    ((LeabraConSpec*)con_spec.SPtr())->SetLearnRule(net);
}

void LeabraPrjn::Init_Stats() {
  netin_avg = 0.0f;
  netin_rel = 0.0f;

  avg_netin_avg = 0.0f;
  avg_netin_avg_sum = 0.0f;
  avg_netin_rel = 0.0f;
  avg_netin_rel_sum = 0.0f;
  avg_netin_n = 0;
}

#ifdef DMEM_COMPILE
void LeabraPrjn::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraPrjn::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif

//////////////////////////////////////////////////////////////////////////
//			Layer Level Code

//////////////////////////
//  	Layer, Spec	//
//////////////////////////

void LeabraInhibSpec::Initialize() {
  type = KWTA_KV2K;
  kwta_pt = .25f;
  min_i = 0.0f;
  comp_thr = .5f;
  comp_gain = 2.0f;
  gp_pt = .2f;
}

void KWTASpec::Initialize() {
  k_from = USE_PCT;
  k = 12;
  pct = .25f;
  pat_q = .5f;
  diff_act_pct = false;
  act_pct = .1f;
  gp_i = false;
  gp_g = .5f;
}

void AdaptISpec::Initialize() {
  type = NONE;
  tol = .02f;			// allow to be off by this amount before taking action
  p_dt = .1f;			// take reasonably quick action..
  mx_d = .9f;			// move this far in either direction
  l = .2f;			// proportion to assign to leak..
  a_dt = .005f;			// time averaging
}

void KwtaTieBreak::Initialize() {
  on = false;
  k_thr = 1.0f;
  diff_thr = 0.2f;
}

void ClampSpec::Initialize() {
  hard = true;
  gain = .2f;
  d_gain = 0.0f;
}

void DecaySpec::Initialize() {
  event = 1.0f;
  phase = 1.0f;
  phase2 = 0.0f;
  clamp_phase2 = false;
}

void CtLayerInhibMod::Initialize() {
  use_sin = false;
  burst_i = 0.02f;
  trough_i = 0.02f;
  use_fin = false;
  inhib_i = 0.0f;
}

void LayNetRescaleSpec::Initialize() {
  on = false;
  max_net = 0.6f;
  net_extra = 0.2f;
}

void LayAbsNetAdaptSpec::Initialize() {
  on = false;
  trg_net = .5f;
  tol = .1f;
  abs_lrate = .2f;
}

void LeabraLayerSpec::Initialize() {
  min_obj_type = &TA_LeabraLayer;
  inhib_group = ENTIRE_LAYER;

  old_compute_i = KWTA_INHIB;
  old_i_kwta_pt = -1.0f;
  old_gp_i_pt = -1.0f;
}

void LeabraLayerSpec::Defaults() {
  adapt_i.Defaults();
  clamp.Defaults();
  decay.Defaults();
  net_rescale.Defaults();
  Initialize();
}

void LeabraLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(old_gp_i_pt != -1.0f) {	// convert from old
    if(GetUnique("compute_i") || GetUnique("i_kwta_pt") || GetUnique("gp_i_pt")) {
      SetUnique("inhib", true);
    }
    inhib.type = (LeabraInhibSpec::InhibType)old_compute_i;
    inhib.kwta_pt = old_i_kwta_pt;
    inhib.gp_pt = old_gp_i_pt;
    old_gp_i_pt = -1.0f;	// negatroid
  }
}

void LeabraLayerSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_LeabraLayerSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(kwta, this);
  taBase::Own(gp_kwta, this);
  taBase::Own(tie_brk, this);
  taBase::Own(adapt_i, this);
  taBase::Own(clamp, this);
  taBase::Own(decay, this);
  taBase::Own(ct_inhib_mod, this);
  taBase::Own(net_rescale, this);
  taBase::Own(abs_net_adapt, this);
}

bool LeabraLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = true;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  if(net && net->learn_rule == LeabraNetwork::CTLEABRA_XCAL) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->opt_thresh.learn > 0.0f, quiet, rval,
		       "LeabraUnitSpec opt_thresh.learn must be -1 for CTLEABRA_XCAL -- I just set it for you in spec:", us->name)) {
      us->SetUnique("opt_thresh", true);
      us->opt_thresh.learn = -1.0f;
    }
  }

  if(net && net->learn_rule >= LeabraNetwork::CTLEABRA_CAL) {
    if(lay->CheckError(decay.phase == 1.0f, quiet, rval,
		       "LeabraLayerSpec decay.phase should be 0 or small for for CTLEABRA_X/CAL -- I just set it to 0 for you in spec:", name)) {
      SetUnique("decay", true);
      decay.phase = 0.0f;
    }
  }

  if(lay->CheckError(!lay->projections.el_base->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
		"does not have LeabraPrjn projection base type!",
		"project must be updated and projections remade"))
    return false;
  bool has_rel_net_conspec = false;
  for(int i=0;i<lay->projections.size;i++) {
    Projection* prjn = (Projection*)lay->projections[i];
    lay->CheckError(!prjn->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
	       "does not have LeabraPrjn projection base type!",
	       "Projection must be re-made");
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.GetSpec();
    if(cs && cs->rel_net_adapt.on) has_rel_net_conspec = true;
  }
  if(has_rel_net_conspec) {
    // check for total trg_netin_rel
    float sum_trg_netin_rel = 0.0f;
    for(int i=0;i<lay->projections.size;i++) {
      LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
      sum_trg_netin_rel += prjn->trg_netin_rel;
    }
    lay->CheckError((fabsf(sum_trg_netin_rel - 1.0f) > .001f), quiet, rval,
		    "sum of trg_netin_rel values for layer:",String(sum_trg_netin_rel),
		    "!= 1.0 -- must fix!");
  }

  lay->CheckError((inhib_group == UNIT_GROUPS && lay->units.gp.size == 0), quiet, rval,
		  "inhib_group is UNIT_GROUPS but layer does not have any unit groups!  will not work.");
		     
  return rval;
}

void LeabraLayerSpec::HelpConfig() {
  String help = "LeabraLayerSpec Configuration:\n\
The layer spec sets the target level of activity, k, for each layer.  \
Therefore, you must have a different layer spec with an appropriate activity \
level for layers that have different activity levels.  Note that if you set \
the activity level by percent this will work for different sized layers that \
have the same percent activity level.";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

void LeabraLayerSpec::Init_Weights(LeabraLayer* lay) {
  Compute_Active_K(lay);	// need kwta.pct for init
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_Weights();
  if(lay->units.gp.size > 0) {
    int gi;
    for(gi=0;gi<lay->units.gp.size;gi++) {
      LeabraUnit_Group* gp = (LeabraUnit_Group*)lay->units.gp[gi];
      gp->misc_state = gp->misc_state1 = gp->misc_state2 = 0;
    }
  }
  Init_Inhib(lay);		// initialize inhibition at start..
  Init_Stats(lay);
  lay->sravg_s_sum = 0.0f;
  lay->sravg_s_nrm = 0.0f;
  lay->sravg_m_sum = 0.0f;
  lay->sravg_m_nrm = 0.0f;
  lay->ravg_l_avg = 0.15f;
}

void LeabraLayerSpec::Init_Stats(LeabraLayer* lay) {
  lay->avg_netin.avg = 0.0f;
  lay->avg_netin.max = 0.0f;

  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;
  lay->avg_netin_n = 0;

  lay->norm_err = 0.0f;

  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    prjn->Init_Stats();
  }
}

void LeabraLayerSpec::Init_ActAvg(LeabraLayer* lay) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_ActAvg();
}

void LeabraLayerSpec::SetCurLrate(LeabraLayer* lay, LeabraNetwork* net, int epoch) {
  if(lay->unit_spec.SPtr()) {
    ((LeabraUnitSpec*)lay->unit_spec.SPtr())->SetCurLrate(net, epoch);
  }
  LeabraPrjn* p;
  taLeafItr pi;
  FOR_ITR_EL(LeabraPrjn, p, lay->projections., pi) {
    p->SetCurLrate(net, epoch);
  }
}

void LeabraLayerSpec::SetLearnRule(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::LEABRA_CHL) {
    decay.phase = 1.0f;		// all phase decay
  }
  else {
    decay.phase = 0.0f;		// no phase decay -- these are not even called
    decay.phase2 = 0.0f;

    if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL) {
      net->ct_sravg.interval = 1;
    }
  }

  if(lay->unit_spec.SPtr()) {
    ((LeabraUnitSpec*)lay->unit_spec.SPtr())->SetLearnRule(net);
  }
  LeabraPrjn* p;
  taLeafItr pi;
  FOR_ITR_EL(LeabraPrjn, p, lay->projections., pi) {
    p->SetLearnRule(net);
  }
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  Projection* p;
  taLeafItr pi;
  FOR_ITR_EL(Projection, p, lay->projections., pi) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
    if(fmlay->spec.SPtr()->InheritsFrom(layer_spec)) {	// inherits - not excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  Projection* p;
  taLeafItr pi;
  FOR_ITR_EL(Projection, p, lay->projections., pi) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
    if(fmlay->spec.SPtr()->GetTypeDef() == layer_spec) {	// not inherits - excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecNet(Network* net, TypeDef* layer_spec) {
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    if(lay->spec.SPtr()->InheritsFrom(layer_spec)) {	// inherits - not excact match!
      return lay;
    }
  }
  return NULL;
}

//////////////////////////////////////////
//	Stage 0: at start of settling	// 
//////////////////////////////////////////

void LeabraLayerSpec::Compute_Active_K(LeabraLayer* lay) {
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int totk = 0;
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_Active_K_impl(lay, rugp, (LeabraInhib*)rugp, gp_kwta);
      totk += rugp->kwta.k;
    }
    if(inhib_group == UNIT_GROUPS) {
      if(lay->kwta.k != totk)
	lay->Inhib_ResetSortBuf();
      lay->kwta.k = totk;
      lay->kwta.Compute_Pct(lay->units.leaves);
      if(gp_kwta.diff_act_pct)
	lay->kwta.pct = gp_kwta.act_pct;	// override!!
    }
  }
  if(inhib_group != UNIT_GROUPS) {
    Compute_Active_K_impl(lay, &(lay->units), (LeabraInhib*)lay, kwta);
    if(kwta.diff_act_pct)
      lay->kwta.pct = kwta.act_pct;	// override!!
  }
}

void LeabraLayerSpec::Compute_Active_K_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					    KWTASpec& kwtspec)
{
  int new_k = 0;
  if(kwtspec.k_from == KWTASpec::USE_PCT)
    new_k = (int)(kwtspec.pct * (float)ug->leaves);
  else if((kwtspec.k_from == KWTASpec::USE_PAT_K) && 
	  ((lay->ext_flag & (Unit::TARG | Unit::COMP)) || (lay->ext_flag & Unit::EXT)))
    new_k = Compute_Pat_K(lay, ug, thr);
  else
    new_k = kwtspec.k;

  if(inhib.type == LeabraInhibSpec::KWTA_INHIB)
    new_k = MIN(ug->leaves - 1, new_k);
  else
    new_k = MIN(ug->leaves, new_k);
  new_k = MAX(1, new_k);

  if(thr->kwta.k != new_k)
    thr->Inhib_ResetSortBuf();

  thr->kwta.k = new_k;
  thr->kwta.Compute_Pct(ug->leaves);
}

int LeabraLayerSpec::Compute_Pat_K(LeabraLayer* lay, Unit_Group* ug, LeabraInhib*) {
  bool use_comp = false;
  if(lay->ext_flag & Unit::COMP) // only use comparison vals if entire lay is COMP!
    use_comp = true;
  int pat_k = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    // use either EXT or TARG information...
    if(u->ext_flag & Unit::EXT) {
      if(u->ext >= kwta.pat_q)
	pat_k++;
    }
    else if(u->ext_flag & Unit::TARG) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }
    else if(use_comp && (u->ext_flag | Unit::COMP)) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }	      
  }
  return pat_k;
}

void LeabraLayerSpec::Init_Acts(LeabraLayer* lay) {
  lay->ext_flag = Unit::NO_EXTERNAL;
  lay->stm_gain = clamp.gain;
  lay->hard_clamped = false;
  lay->net_rescale = 1.0f;
  lay->ResetSortBuf();
  Compute_Active_K(lay);	// need kwta.pct for init
  lay->Inhib_Init_Acts(this);
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->Inhib_Init_Acts(this);
    }
  }
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_Acts(lay);
}

void LeabraLayerSpec::Compute_HardClampPhase2(LeabraLayer* lay, LeabraNetwork* net) {
  // special case to speed up processing by clamping 2nd plus phase to prior + phase
  lay->SetExtFlag(Unit::EXT);
  lay->hard_clamped = true;
  lay->Inhib_SetVals(inhib.kwta_pt);		// assume 0 - 1 clamped inputs

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->SetExtFlag(Unit::EXT);
    u->ext = u->act_p;
    u->Compute_HardClampNoClip(lay, net); // important: uses no clip here: not really external values!
  }
  Compute_ActAvg(lay, net);
}

void LeabraLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(decay.clamp_phase2 && (net->phase != LeabraNetwork::MINUS_PHASE) && (net->phase_no >= 2)) {
    Compute_HardClampPhase2(lay, net);
    return;
  }
  if(!(clamp.hard && (lay->ext_flag & Unit::EXT))) {
    lay->hard_clamped = false;
    return;
  }
  lay->hard_clamped = true;	// cache this flag
  lay->Inhib_SetVals(inhib.kwta_pt);		// assume 0 - 1 clamped inputs

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_HardClamp(lay, net);
  }
  Compute_ActAvg(lay, net);
  Compute_OutputName(lay, net);
}

void LeabraLayerSpec::Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_NetinScale(lay, net);
  }
}

void LeabraLayerSpec::Init_ClampNet(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_ClampNet(lay, net);
}

void LeabraLayerSpec::Send_ClampNet(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_ClampNet(lay, net);
}

//////////////////////////////////
//	Stage 1: netinput 	//
//////////////////////////////////

void LeabraLayerSpec::Send_Netin(LeabraLayer* lay, LeabraNetwork* net) {
  // hard-clamped input layers are already computed in the clmp_net value
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_Netin(lay, net);
}

void LeabraLayerSpec::Send_NetinDelta(LeabraLayer* lay, LeabraNetwork* net) {
  // hard-clamped input layers are already computed in the clmp_net value
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_NetinDelta(lay, net);
}

//////////////////////////////////////////////////////////////////
//	Stage 2: netinput averages and clamping (if necc)	//
//////////////////////////////////////////////////////////////////

void LeabraLayerSpec::Compute_Clamp_NetAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  if(lay->ext_flag & Unit::EXT)
    Compute_SoftClamp(lay, net);

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    lay->netin.avg = 0.0f; lay->netin.max = -FLT_MAX; lay->netin.max_i = -1;
    lay->i_thrs.avg = 0.0f; lay->i_thrs.max = -FLT_MAX; lay->i_thrs.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_NetinAvg(lay, rugp, (LeabraInhib*)rugp, net);
      // keep maximums for linking purposes
      lay->netin.avg = MAX(lay->netin.avg, rugp->netin.avg);
      if(rugp->netin.max > lay->netin.max) {
	lay->netin.max = rugp->netin.max; lay->netin.max_i = g;
      }
      lay->i_thrs.avg = MAX(lay->i_thrs.avg, rugp->i_thrs.avg);
      if(rugp->i_thrs.max > lay->i_thrs.max) {
	lay->i_thrs.max = rugp->i_thrs.max; lay->i_thrs.max_i = g;
      }
    }
  }
  else {
    Compute_NetinAvg(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_NetinAvg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  thr->netin.avg = 0.0f; thr->netin.max = -FLT_MAX; thr->netin.max_i = -1;
  thr->i_thrs.avg = 0.0f; thr->i_thrs.max = -FLT_MAX; thr->i_thrs.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_NetinAvg(lay, thr, net);
    thr->netin.avg += u->net;
    if(u->net > thr->netin.max) {
      thr->netin.max = u->net;  thr->netin.max_i = lf;
    }
    thr->i_thrs.avg += u->i_thr;
    if(u->i_thr > thr->i_thrs.max) {
      thr->i_thrs.max = u->i_thr;  thr->i_thrs.max_i = lf;
    }
    lf++;
  }

  if(ug->leaves > 0) {
    thr->netin.avg /= (float)ug->leaves;	// turn it into an average
    thr->i_thrs.avg /= (float)ug->leaves;	// turn it into an average
  }
}

void LeabraLayerSpec::Compute_SoftClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(clamp.d_gain == 0.0f) {
    lay->stm_gain = clamp.gain;	// always set directly to spec val so changes are "live"
  }
  LeabraUnit* u;
  taLeafItr i;
  bool inc_gain = false;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    if(!inc_gain)
      inc_gain = u->Compute_SoftClamp(lay, net);
    else
      u->Compute_SoftClamp(lay, net);
  }
  if(inc_gain && (net->cycle > 1)) // only increment after it has a chance to activate
    lay->stm_gain += clamp.d_gain;
}


//////////////////////////////////////////
//	Stage 3: inhibition		//
//////////////////////////////////////////

void LeabraSort::FastInsertLink(void* it, int where) {
  if((where >= size) || (where < 0)) {
    AddOnly_(it);
    return;
  }
  if(size > 0)
    AddOnly_(NULL);
  int i;
  for(i=size-1; i>where; i--)
    el[i] = el[i-1];
  el[where] = it;
}

int LeabraSort::FindNewNetPos(float nw_net) {
  int st = 0;
  int n = size;
  while(true) {
    if(n <= 2) {
      if(FastEl(st)->i_thr < nw_net)
	return st;
      else if((st+1 < size) && (FastEl(st+1)->i_thr < nw_net))
	return st + 1;
      else if((st+2 < size) && (FastEl(st+2)->i_thr < nw_net))
	return st + 2;
      return st;
    }
    int n2 = n / 2;
    if(FastEl(st + n2)->i_thr < nw_net)
      n = n2;			// search in upper 1/2 of list
    else {
      st += n2; n -= n2;	// search in lower 1/2 of list
    }
  }
}

void LeabraLayerSpec::Init_Inhib(LeabraLayer* lay) {
  lay->adapt_i.avg_avg = lay->kwta.pct;
  lay->adapt_i.i_kwta_pt = inhib.kwta_pt;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  lay->adapt_i.g_bar_i = us->g_bar.i;
  lay->adapt_i.g_bar_l = us->g_bar.l;
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->adapt_i.avg_avg = rugp->kwta.pct;
      if(((inhib.type == LeabraInhibSpec::AVG_MAX_PT_INHIB)
	  || (inhib.type == LeabraInhibSpec::MAX_INHIB))
	 && (inhib_group != ENTIRE_LAYER))
	rugp->adapt_i.i_kwta_pt = inhib.gp_pt;
      else
	rugp->adapt_i.i_kwta_pt = inhib.kwta_pt;
      rugp->adapt_i.g_bar_i = us->g_bar.i;
      rugp->adapt_i.g_bar_l = us->g_bar.l;
    }
  }
}

void LeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped)	return;	// say no more..

  if(inhib_group != UNIT_GROUPS) {
    Compute_Inhib_impl(lay, &(lay->units), (LeabraInhib*)lay, net, inhib);
  }
  if(lay->units.gp.size > 0) {
    if(inhib_group == UNIT_GROUPS) {
      lay->Inhib_SetVals(0.0f);
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	Compute_Inhib_impl(lay, rugp, (LeabraInhib*)rugp, net, inhib);
	float gp_g_i = rugp->i_val.g_i;
	if(gp_kwta.gp_i)
	  gp_g_i *= gp_kwta.gp_g;
	lay->i_val.g_i = MAX(lay->i_val.g_i, gp_g_i);
      }
    }
    else if(inhib_group == LAY_AND_GPS) {
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	Compute_Inhib_impl(lay, rugp, (LeabraInhib*)rugp, net, inhib);
      }
    }
    Compute_LayInhibToGps(lay, net);
  }

  Compute_CtDynamicInhib(lay, net);
}

void LeabraLayerSpec::Compute_Inhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					 LeabraNetwork* net, LeabraInhibSpec& ispec) {
  if(ispec.type == LeabraInhibSpec::UNIT_INHIB) {
    thr->i_val.g_i = 0.0f;	// make sure it's zero, cuz this gets added to units.. 
  }
  else {
    if(ispec.type == LeabraInhibSpec::KWTA_INHIB)
      Compute_Inhib_kWTA(lay, ug, thr, net, ispec);
    else if(ispec.type == LeabraInhibSpec::KWTA_AVG_INHIB)
      Compute_Inhib_kWTA_Avg(lay, ug, thr, net, ispec);
    else if(ispec.type == LeabraInhibSpec::KWTA_KV2K)
      Compute_Inhib_kWTA_kv2k(lay, ug, thr, net, ispec);
    else if(ispec.type == LeabraInhibSpec::KWTA_COMP_COST)
      Compute_Inhib_kWTA_CompCost(lay, ug, thr, net, ispec);
    else if(ispec.type == LeabraInhibSpec::AVG_MAX_PT_INHIB)
      Compute_Inhib_AvgMaxPt(lay, ug, thr, net, ispec);
    else if(ispec.type == LeabraInhibSpec::MAX_INHIB)
      Compute_Inhib_Max(lay, ug, thr, net, ispec);
    thr->i_val.g_i = thr->i_val.kwta;
  }

  thr->i_val.g_i_orig = thr->i_val.g_i;	// retain original values..
}

// basic sorting function:

void LeabraLayerSpec::Compute_Inhib_kWTA_Sort(Unit_Group* ug, LeabraInhib* thr,
					      LeabraSort& act_buf, LeabraSort& inact_buf,
					      int k_eff, float& k_net, int& k_idx) {
  LeabraUnit* u;
  taLeafItr i;
  int j;
  if(act_buf.size != k_eff) { // need to fill the sort buf..
    act_buf.size = 0;
    j = 0;
    for(u = (LeabraUnit*)ug->FirstEl(i); u && (j < k_eff);
	u = (LeabraUnit*)ug->NextEl(i), j++)
    {
      act_buf.Add(u);		// add unit to the list
      if(u->i_thr < k_net) {
	k_net = u->i_thr;	k_idx = j;
      }
    }
    inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; u; u = (LeabraUnit*)ug->NextEl(i)) {
      if(u->i_thr <=  k_net) {	// not bigger than smallest one in sort buffer
	inact_buf.Add(u);
	continue;
      }
      inact_buf.Add(act_buf[k_idx]); // now inactive
      act_buf.ReplaceIdx(k_idx, u);// replace the smallest with it
      k_net = u->i_thr;		// assume its the smallest
      for(j=0; j < k_eff; j++) { 	// and recompute the actual smallest
	float tmp = act_buf[j]->i_thr;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find k_net
    for(j=0; j < k_eff; j++) { 	// these should be the top ones, very fast!!
      float tmp = act_buf[j]->i_thr;
      if(tmp < k_net) {
	k_net = tmp;		k_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < inact_buf.size; j++) {
      u = inact_buf[j];
      if(u->i_thr <=  k_net)		// not bigger than smallest one in sort buffer
	continue;
      inact_buf.ReplaceIdx(j, act_buf[k_idx]);	// now inactive
      act_buf.ReplaceIdx(k_idx, u);// replace the smallest with it
      k_net = u->i_thr;		// assume its the smallest
      int i;
      for(i=0; i < k_eff; i++) { 	// and recompute the actual smallest
	float tmp = act_buf[i]->i_thr;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = i;
	}
      }
    }
  }
}

void LeabraLayerSpec::Compute_Inhib_BreakTie(LeabraInhib* thr) {
  thr->kwta.ithr_diff = (thr->kwta.k_ithr - thr->kwta.k1_ithr) / thr->kwta.k_ithr;
  thr->kwta.tie_brk = 0;
  if(tie_brk.on && (thr->kwta.k_ithr > tie_brk.k_thr)) {
    if(thr->kwta.ithr_diff < tie_brk.diff_thr) {
      // we now have an official tie: break it by reducing firing of "others"
      thr->kwta.k1_ithr = (1.0f - tie_brk.diff_thr) * thr->kwta.k_ithr;
      thr->kwta.tie_brk = 1;
    }
  }
}

// actual kwta impls:

void LeabraLayerSpec::Compute_Inhib_kWTA(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr,
					 LeabraNetwork*, LeabraInhibSpec& ispec) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_plus_1 = thr->kwta.k + 1;	// expand cutoff to include N+1th one
  k_plus_1 = MIN(ug->leaves,k_plus_1);
  float k1_net = FLT_MAX;
  int k1_idx = 0;

  Compute_Inhib_kWTA_Sort(ug, thr, thr->active_buf, thr->inact_buf, k_plus_1, k1_net, k1_idx);

  // active_buf now has k+1 most active units, get the next-highest one
  int k_idx = -1;
  float net_k = FLT_MAX;
  for(int j=0; j < k_plus_1; j++) {
    float tmp = thr->active_buf[j]->i_thr;
    if((tmp < net_k) && (j != k1_idx)) {
      net_k = tmp;		k_idx = j;
    }
  }
  if(k_idx == -1) {		// we didn't find the next one
    k_idx = k1_idx;
    net_k = k1_net;
  }

  LeabraUnit* k1_u = (LeabraUnit*)thr->active_buf[k1_idx];
  LeabraUnit* k_u = (LeabraUnit*)thr->active_buf[k_idx];

  float k1_i = k1_u->i_thr;
  float k_i = k_u->i_thr;
  thr->kwta.k_ithr = k_i;
  thr->kwta.k1_ithr = k1_i;

  Compute_Inhib_BreakTie(thr);

  // place kwta inhibition between k and k+1
  float nw_gi = thr->kwta.k1_ithr + ispec.kwta_pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Avg(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr,
					     LeabraNetwork*, LeabraInhibSpec& ispec) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_eff = thr->kwta.k;	// keep cutoff at k
  float k_net = FLT_MAX;
  int k_idx = 0;

  Compute_Inhib_kWTA_Sort(ug, thr, thr->active_buf, thr->inact_buf, k_eff, k_net, k_idx);

  // active_buf now has k most active units, get averages of both groups
  int j;
  float k_avg = 0.0f;
  for(j=0; j < k_eff; j++)
    k_avg += thr->active_buf[j]->i_thr;
  k_avg /= (float)k_eff;

  float oth_avg = 0.0f;
  for(j=0; j < thr->inact_buf.size; j++)
    oth_avg += thr->inact_buf[j]->i_thr;
  if(thr->inact_buf.size > 0)
    oth_avg /= (float)thr->inact_buf.size;

  // place kwta inhibition between two averages
  // this uses the adapting point!
  float pt = ispec.kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;

  Compute_Inhib_BreakTie(thr);

  float nw_gi = thr->kwta.k1_ithr + pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_kv2k(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr,
					      LeabraNetwork*, LeabraInhibSpec& ispec) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_eff = thr->kwta.k;
  float k_net = FLT_MAX;
  int k_idx = 0;

  Compute_Inhib_kWTA_Sort(ug, thr, thr->active_buf, thr->inact_buf, k_eff, k_net, k_idx);

  // active_buf now has k most active units, get average from act buf
  int j;
  float oth_avg = 0.0f;
  float k_avg = 0.0f;
  for(j=0; j < k_eff; j++)
    k_avg += thr->active_buf[j]->i_thr;
  k_avg /= (float)k_eff;

  int k2_eff = 2 * thr->kwta.k;
  if(k2_eff >= ug->leaves) {
    // just use inact buf: same as kwta_avg in this case..
    for(j=0; j < thr->inact_buf.size; j++)
      oth_avg += thr->inact_buf[j]->i_thr;
    if(thr->inact_buf.size > 0)
      oth_avg /= (float)thr->inact_buf.size;
  }
  else {
    // find 2k guys
    float k2_net = FLT_MAX;
    int k2_idx = 0;
    Compute_Inhib_kWTA_Sort(ug, thr, thr->active_2k_buf, thr->inact_2k_buf, k2_eff, k2_net, k2_idx);

    for(j=0; j < thr->active_2k_buf.size; j++)
      oth_avg += thr->active_2k_buf[j]->i_thr;
    if(thr->active_2k_buf.size > 0)
      oth_avg /= (float)thr->active_2k_buf.size;
  }

  // place kwta inhibition between two averages
  // this uses the adapting point!
  float pt = ispec.kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;

  Compute_Inhib_BreakTie(thr);

  float nw_gi = thr->kwta.k1_ithr + pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_CompCost(LeabraLayer*, Unit_Group* ug,
						  LeabraInhib* thr, LeabraNetwork*,
						  LeabraInhibSpec& ispec) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_eff = thr->kwta.k;	// keep cutoff at k
  float k_net = FLT_MAX;
  int k_idx = 0;

  Compute_Inhib_kWTA_Sort(ug, thr, thr->active_buf, thr->inact_buf, k_eff, k_net, k_idx);

  LeabraUnit* k_u = (LeabraUnit*)thr->active_buf[k_idx];
  float k_ithr = k_u->i_thr;

  float oth_sum = 0.0f;
  for(int j=0; j < thr->inact_buf.size; j++) {
    float oth_nrm = thr->inact_buf[j]->i_thr / k_ithr;
    if(oth_nrm >= ispec.comp_thr)
      oth_sum += oth_nrm;
  }
  if(thr->inact_buf.size > 0)
    oth_sum /= (float)thr->inact_buf.size;

  float pt = ispec.kwta_pt;
//   if(adapt_i.type == AdaptISpec::KWTA_PT)  // this is not correct: adapt goes the wrong way!
//     pt = thr->adapt_i.i_kwta_pt;

  float nw_gi = k_ithr - pt * (1.0f - ispec.comp_gain * oth_sum);

  thr->kwta.k_ithr = k_ithr;
  thr->kwta.k1_ithr = nw_gi;

//   Compute_Inhib_BreakTie(thr); // not applicable!

  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Gps(LeabraLayer* lay, LeabraNetwork* net,
					     LeabraInhibSpec& ispec) {
  // computing the top *groups*, not units here!
  int k_eff = lay->kwta.k;	// only get top k

  float k_net = FLT_MAX;
  int k_idx = 0;
  LeabraUnit_Group* u;
  int i;
  int j;
  if(lay->active_buf.size != k_eff) { // need to fill the sort buf..
    lay->active_buf.size = 0;
    for(i = 0; i < k_eff; i++) {
      u = (LeabraUnit_Group*)lay->units.gp[i], 
      lay->active_buf.Add((LeabraUnit*)u);		// add unit to the list
      if(u->i_val.g_i < k_net) {
	k_net = u->i_val.g_i;	k_idx = i;
      }
    }
    lay->inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; i<lay->units.gp.size; i++) {
      u = (LeabraUnit_Group*)lay->units.gp[i];
      if(u->i_val.g_i <=  k_net) {	// not bigger than smallest one in sort buffer
	lay->inact_buf.Add((LeabraUnit*)u);
	continue;
      }
      lay->inact_buf.Add(lay->active_buf[k_idx]); // now inactive
      lay->active_buf.ReplaceIdx(k_idx, (LeabraUnit*)u);// replace the smallest with it
      k_net = u->i_val.g_i;		// assume its the smallest
      for(j=0; j < k_eff; j++) { 	// and recompute the actual smallest
	float tmp = ((LeabraUnit_Group*)lay->active_buf[j])->i_val.g_i;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find k_net
    for(j=0; j < k_eff; j++) { 	// these should be the top ones, very fast!!
      float tmp = ((LeabraUnit_Group*)lay->active_buf[j])->i_val.g_i;
      if(tmp < k_net) {
	k_net = tmp;		k_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < lay->inact_buf.size; j++) {
      u = (LeabraUnit_Group*)lay->inact_buf[j];
      if(u->i_val.g_i <=  k_net)		// not bigger than smallest one in sort buffer
	continue;
      lay->inact_buf.ReplaceIdx(j, lay->active_buf[k_idx]);	// now inactive
      lay->active_buf.ReplaceIdx(k_idx, (LeabraUnit*)u);// replace the smallest with it
      k_net = u->i_val.g_i;		// assume its the smallest
      int i;
      for(i=0; i < k_eff; i++) { 	// and recompute the actual smallest
	float tmp = ((LeabraUnit_Group*)lay->active_buf[i])->i_val.g_i;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = i;
	}
      }
    }
  }

  // active_buf now has k most active groups - go through groups and set large inhib in 
  // inactive ones!
  for(j=0; j < lay->inact_buf.size; j++) {
    u = (LeabraUnit_Group*)lay->inact_buf[j];
    // set inhib to more than what is needed to keep *max* unit below threshold..
    LeabraUnit* un = (LeabraUnit*)u->Leaf(0);// get first unit 
    float tnet = un->net;
    un->net = u->netin.max;	// set to max
    u->i_val.g_i = 1.2f * un->Compute_IThresh(lay, net);
    un->net = tnet;
  }
}

void LeabraLayerSpec::Compute_Inhib_AvgMaxPt(LeabraLayer* lay, Unit_Group*, LeabraInhib* thr,
					     LeabraNetwork*, LeabraInhibSpec& ispec) {
  float pt = ispec.kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  else if((inhib_group != ENTIRE_LAYER) && ((LeabraInhib*)lay != thr))
    pt = ispec.gp_pt;		// use sub-group version for sub-groups..
  
  float oth_avg = thr->i_thrs.avg; // put between the avg
  float k_avg = thr->i_thrs.max; // and the max..

  float nw_gi = oth_avg + pt * (k_avg - oth_avg);
  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_Max(LeabraLayer* lay, Unit_Group*, LeabraInhib* thr,
					LeabraNetwork*, LeabraInhibSpec& ispec) {
  float pt = ispec.kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  else if((inhib_group != ENTIRE_LAYER) && ((LeabraInhib*)lay != thr))
    pt = ispec.gp_pt;		// use sub-group version for sub-groups..
  
  float k_avg = thr->i_thrs.max; // and the max..
  float nw_gi = k_avg - pt;
  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_CtDynamicInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::LEABRA_CHL) return;

  float bi = net->ct_sin_i.burst_i;
  float ti = net->ct_sin_i.trough_i;
  if(ct_inhib_mod.use_sin) {
    bi = ct_inhib_mod.burst_i;
    ti = ct_inhib_mod.trough_i;
  }
  float ii = net->ct_fin_i.inhib_i;
  if(ct_inhib_mod.use_fin) {
    ii = ct_inhib_mod.inhib_i;
  }
  float imod = net->ct_sin_i.GetInhibMod(net->ct_cycle, bi, ti) +
    net->ct_fin_i.GetInhibMod(net->ct_cycle - net->ct_time.inhib_start, ii);

  // only one is going to be in effect at a time..
  lay->i_val.g_i += imod * lay->i_val.g_i;

  if(inhib_group != ENTIRE_LAYER) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i += imod * rugp->i_val.g_i;
    }
  }
}

//////////////////////////////////////////
//	Stage 3.25: LayInhibToGps	//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork*) {
  if(lay->units.gp.size == 0) return;

  if(inhib_group == ENTIRE_LAYER) {
    // propagate g_i to all subgroups even if doing ENTIRE_LAYER
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i = lay->i_val.g_i;
    }
  }
  else if(inhib_group == UNIT_GROUPS) {
    if(gp_kwta.gp_i) {	// linking groups: get max from layer
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	rugp->i_val.gp_g_i = lay->i_val.g_i;
	rugp->i_val.g_i = MAX(rugp->i_val.g_i, lay->i_val.g_i);
      }
    }
  }
  else if(inhib_group == LAY_AND_GPS) {
    // actual inhibition is max of layer and group
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i = MAX(rugp->i_val.g_i, lay->i_val.g_i);
    }
  }
}

//////////////////////////////////////////
//	Stage 3.5: Apply Inhib 		//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  if(lay->units.gp.size > 0) {	// even if ENTIRE_LAYER, do it by sub-group to get stats..
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ApplyInhib_impl(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_ApplyInhib_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ApplyInhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_ApplyInhib(lay, thr, net, thr->i_val.g_i);
  }
}

//////////////////////////////////////////
//	Stage 3.75: Inhib Avg	 	//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  if(lay->units.gp.size > 0) {	// even if ENTIRE_LAYER, do it by sub-group to get stats..
    lay->un_g_i.avg = 0.0f;
    lay->un_g_i.max = -FLT_MAX;
    lay->un_g_i.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_InhibAvg_impl(lay, rugp, (LeabraInhib*)rugp, net);
      lay->un_g_i.avg += rugp->un_g_i.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->un_g_i.max > lay->un_g_i.max) {
	lay->un_g_i.max = rugp->un_g_i.max;
	lay->un_g_i.max_i = g;
      }
    }
  }
  else {
    Compute_InhibAvg_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_InhibAvg_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  // now actually go through and set the gc.i for each unit
  thr->un_g_i.avg = 0.0f;
  thr->un_g_i.max = -FLT_MAX;
  thr->un_g_i.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(inhib.type == LeabraInhibSpec::UNIT_INHIB)
      u->Compute_InhibAvg(lay, thr, net);
    thr->un_g_i.avg += u->gc.i;
    if(u->gc.i > thr->un_g_i.max) {
      thr->un_g_i.max = u->gc.i;
      thr->un_g_i.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0)
    thr->un_g_i.avg /= (float)ug->leaves;	// turn it into an average
}

//////////////////////////////////////////
//	Stage 4: the final activation 	//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_ActAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					 LeabraNetwork* net) {
  thr->acts.avg = 0.0f;
  thr->acts.max = -FLT_MAX;
  thr->acts.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts.avg += u->act_eq;
    if(u->act_eq > thr->acts.max) {
      thr->acts.max = u->act_eq;  thr->acts.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts.avg = 0.0f;
    lay->acts.max = -FLT_MAX;
    lay->acts.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts.avg += rugp->acts.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts.max > lay->acts.max) {
	lay->acts.max = rugp->acts.max; lay->acts.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_MaxDa_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					 LeabraNetwork* net) {
  thr->maxda = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_MaxDa(lay, thr, net);
  }
}

void LeabraLayerSpec::Compute_MaxDa(LeabraLayer* lay, LeabraNetwork* net) {
  lay->maxda = 0.0f;
  if(lay->units.gp.size > 0) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_MaxDa_ugp(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_MaxDa_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr,
					  LeabraNetwork*) {
  thr->acts_m.avg = 0.0f;
  thr->acts_m.max = -FLT_MAX;
  thr->acts_m.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts_m.avg += u->act_m;
    if(u->act_m > thr->acts_m.max) {
      thr->acts_m.max = u->act_m;  thr->acts_m.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_m.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActMAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts_m.avg = 0.0f;
    lay->acts_m.max = -FLT_MAX;
    lay->acts_m.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActMAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts_m.avg += rugp->acts_m.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts_m.max > lay->acts_m.max) {
	lay->acts_m.max = rugp->acts_m.max; lay->acts_m.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts_m.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActMAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_p.avg = 0.0f;
  thr->acts_p.max = -FLT_MAX;
  thr->acts_p.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts_p.avg += u->act_p;
    if(u->act_p > thr->acts_p.max) {
      thr->acts_p.max = u->act_p;  thr->acts_p.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_p.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActPAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts_p.avg = 0.0f;
    lay->acts_p.max = -FLT_MAX;
    lay->acts_p.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActPAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts_p.avg += rugp->acts_p.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts_p.max > lay->acts_p.max) {
	lay->acts_p.max = rugp->acts_p.max; lay->acts_p.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts_p.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActPAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped) {
    Compute_OutputName(lay, net); // need to keep doing this because network clears it
    return;			// don't do this during normal processing
  }

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_Act_impl(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_Act_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }

  Compute_ActAvg(lay, net);
  Compute_MaxDa(lay, net);
  Compute_NetinRescale(lay, net);
  if(lay->ext_flag & Unit::TARG) {
    net->trg_max_act = MAX(net->trg_max_act, lay->acts.max);
  }
  Compute_OutputName(lay, net);
  if(lay->Iconified()) {
    lay->icon_value = lay->acts.avg;
  }
}

void LeabraLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_Act(lay,thr,net);
  }
}

void LeabraLayerSpec::Compute_NetinRescale(LeabraLayer* lay, LeabraNetwork* net) {
  if(!net_rescale.on) return;
  if(lay->netin.max <= net_rescale.max_net) return;
  float old_scale = lay->net_rescale;
  lay->net_rescale *= net_rescale.max_net / (lay->netin.max + net_rescale.net_extra);
  float new_scale = lay->net_rescale / old_scale;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_NetinRescale(lay, net, new_scale);
  }
}

void LeabraLayerSpec::Compute_OutputName_ugp(LeabraLayer* lay, Unit_Group* ug,
					     LeabraInhib* thr, LeabraNetwork* net) {
  String* onm;
  if(lay->unit_groups)
    onm = &(ug->output_name);
  else
    onm = &(lay->output_name);

  if(thr->acts.max_i < 0) {
    *onm = "n/a";
    return;
  }
  LeabraUnit* u = (LeabraUnit*)ug->Leaf(thr->acts.max_i);
  if(!u) {
    *onm = "n/a";
    return;
  }
  // for target/output layers, if we set something, set network name!
  if(u->name.empty()) return;
  *onm = u->name;	// if it is something..

  if(lay->unit_groups) {	// also aggregate the layer name
    if(lay->output_name.nonempty())
      lay->output_name += "_";
    lay->output_name += u->name;
  }

  if((lay->layer_type != Layer::OUTPUT) && (lay->layer_type != Layer::TARGET)) return;
  if(net->output_name.nonempty())
    net->output_name += "_";
  net->output_name += u->name;
}

void LeabraLayerSpec::Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->output_name = "";
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_OutputName_ugp(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_OutputName_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

float LeabraLayerSpec::Compute_TopKAvgAct_ugp(LeabraLayer* lay, Unit_Group* ug,
					      LeabraInhib* thr, LeabraNetwork*) {
  int k_eff = thr->kwta.k;	// keep cutoff at k

  if(TestError(k_eff <= 0 || thr->active_buf.size != k_eff, "Compute_TopKAvgAct_ugp",
	       "Only usable when using a kwta function!")) {
    return -1;
  }

  float k_avg = 0.0f;
  for(int j=0; j < k_eff; j++)
    k_avg += thr->active_buf[j]->act_eq;
  k_avg /= (float)k_eff;

  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgAct(LeabraLayer* lay, LeabraNetwork* net) {
  float k_avg = 0.0f;
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      k_avg += Compute_TopKAvgAct_ugp(lay, rugp, (LeabraInhib*)rugp, net);
    }
    if(lay->units.gp.size > 0)
      k_avg /= (float)lay->units.gp.size;
  }
  else {
    k_avg = Compute_TopKAvgAct_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgNetin_ugp(LeabraLayer* lay, Unit_Group* ug,
					      LeabraInhib* thr, LeabraNetwork*) {
  int k_eff = thr->kwta.k;	// keep cutoff at k

  if(TestError(k_eff <= 0 || thr->active_buf.size != k_eff, "Compute_TopKAvgNetin_ugp",
	       "Only usable when using a kwta function!")) {
    return -1;
  }

  float k_avg = 0.0f;
  for(int j=0; j < k_eff; j++)
    k_avg += thr->active_buf[j]->net;
  k_avg /= (float)k_eff;

  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgNetin(LeabraLayer* lay, LeabraNetwork* net) {
  float k_avg = 0.0f;
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      k_avg += Compute_TopKAvgNetin_ugp(lay, rugp, (LeabraInhib*)rugp, net);
    }
    if(lay->units.gp.size > 0)
      k_avg /= (float)lay->units.gp.size;
  }
  else {
    k_avg = Compute_TopKAvgNetin_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
  return k_avg;
}

void LeabraLayerSpec::Compute_CycSynDep(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_CycSynDep(lay, net);
  }
}

//////////////////////////////////////////
//	Stage 5: Between Events 	//
//////////////////////////////////////////

void LeabraLayerSpec::PhaseInit(LeabraLayer* lay, LeabraNetwork* net) {
  lay->net_rescale = 1.0f;
  if(lay->ext_flag & Unit::TARG) {	// only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE)
      lay->SetExtFlag(Unit::EXT);
  }
  else {
    if(clamp.hard) return;	// not target and hard-clamped -- bail
  }
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->PhaseInit(lay, net);
  }
}

void LeabraLayerSpec::DecayEvent(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayEvent(lay, net, decay.event);
  }
}
  
void LeabraLayerSpec::DecayPhase(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayPhase(lay, net, decay.phase);
  }
}

void LeabraLayerSpec::DecayPhase2(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayPhase(lay, net, decay.phase2);
  }
}
  
void LeabraLayerSpec::ExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(lay->ext_flag & Unit::EXT))	// only process ext
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->ExtToComp(lay, net);
}

void LeabraLayerSpec::TargExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(lay->ext_flag & Unit::TARG_EXT))	// only process w/ external input
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->TargExtToComp(lay, net);
}

void LeabraLayerSpec::AdaptGBarI(LeabraLayer* lay, LeabraNetwork*) {
  float diff = lay->kwta.pct - lay->acts.avg;
  if(fabsf(diff) > adapt_i.tol) {
    float p_i = 1.0f;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      p_i = 1.0f - adapt_i.l;
    }
    lay->adapt_i.g_bar_i -= p_i * adapt_i.p_dt * diff;
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    float minv = us->g_bar.i * (1.0 - adapt_i.mx_d);
    float maxv = us->g_bar.i * (1.0 + adapt_i.mx_d);
    if(lay->adapt_i.g_bar_i < minv) lay->adapt_i.g_bar_i = minv;
    if(lay->adapt_i.g_bar_i > maxv) lay->adapt_i.g_bar_i = maxv;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      lay->adapt_i.g_bar_l -= adapt_i.l * adapt_i.p_dt * diff;
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
      float minv = us->g_bar.l * (1.0 - adapt_i.mx_d);
      float maxv = us->g_bar.l * (1.0 + adapt_i.mx_d);
      if(lay->adapt_i.g_bar_l < minv) lay->adapt_i.g_bar_l = minv;
      if(lay->adapt_i.g_bar_l > maxv) lay->adapt_i.g_bar_l = maxv;
    }
  }
}

void LeabraLayerSpec::PostSettle_GetMinus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m = lay->acts;
  if(lay->units.gp.size > 0) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->acts_m = rugp->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetPlus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_p = lay->acts;
  if(lay->units.gp.size > 0) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->acts_p = rugp->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetMinus2(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m2 = lay->acts;
  if(lay->units.gp.size > 0) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->acts_m2 = rugp->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetPlus2(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_p2 = lay->acts;
  if(lay->units.gp.size > 0) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->acts_p2 = rugp->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetPhaseDifRatio(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->acts_p.avg > 0.0f)
    lay->phase_dif_ratio = lay->acts_m.avg / lay->acts_p.avg;
  else
    lay->phase_dif_ratio = 1.0f;
  if(lay->units.gp.size > 0) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      if(rugp->acts_p.avg > 0.0f)
	rugp->phase_dif_ratio = rugp->acts_m.avg / rugp->acts_p.avg;
      else
	rugp->phase_dif_ratio = 1.0f;
    }
  }
}


void LeabraLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  switch(net->phase_order) {
  case LeabraNetwork::MINUS_PLUS:
    if(no_plus_testing) {
      PostSettle_GetMinus(lay, net);
      PostSettle_GetPlus(lay, net);
      lay->phase_dif_ratio = 1.0f;
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE)
	PostSettle_GetMinus(lay, net);
      else {
	PostSettle_GetPlus(lay, net);
	PostSettle_GetPhaseDifRatio(lay, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_MINUS:
    if(no_plus_testing) {
      PostSettle_GetMinus(lay, net);
      PostSettle_GetPlus(lay, net);
      lay->phase_dif_ratio = 1.0f;
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE) {
	PostSettle_GetMinus(lay, net);
	PostSettle_GetPhaseDifRatio(lay, net);
      }
      else {
	PostSettle_GetPlus(lay, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_ONLY:
    PostSettle_GetMinus(lay, net);
    PostSettle_GetPlus(lay, net);
    lay->phase_dif_ratio = 1.0f;
    break;
  case LeabraNetwork::MINUS_PLUS_NOTHING:
  case LeabraNetwork::MINUS_PLUS_MINUS:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      PostSettle_GetMinus(lay, net);
    }
    else if(net->phase_no == 1) {
      PostSettle_GetPlus(lay, net);
      PostSettle_GetPhaseDifRatio(lay, net);
    }
    else {
      PostSettle_GetPlus2(lay, net);
    }
    break;
  case LeabraNetwork::PLUS_NOTHING:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      PostSettle_GetPlus(lay, net);
    }
    else {
      PostSettle_GetMinus(lay, net);
      PostSettle_GetPhaseDifRatio(lay, net);
    }
    break;
  case LeabraNetwork::MINUS_PLUS_PLUS:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      PostSettle_GetMinus(lay, net);
    }
    else if(net->phase_no == 1) {
      PostSettle_GetPlus(lay, net);
      PostSettle_GetPhaseDifRatio(lay, net);
    }
    else {
      PostSettle_GetPlus2(lay, net);
    }
    break;
  case LeabraNetwork::MINUS_PLUS_PLUS_NOTHING:
  case LeabraNetwork::MINUS_PLUS_PLUS_MINUS:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      PostSettle_GetMinus(lay, net);
    }
    else if(net->phase_no == 1) {
      PostSettle_GetPlus(lay, net);
      PostSettle_GetPhaseDifRatio(lay, net);
    }
    else if(net->phase_no == 2) {
      PostSettle_GetPlus2(lay, net);
    }
    else {
      PostSettle_GetMinus2(lay, net);
    }
    break;
  }

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      LeabraUnit* u;
      taLeafItr i;
      FOR_ITR_EL(LeabraUnit, u, rugp->, i)
	u->PostSettle(lay, (LeabraInhib*)rugp, net);
    }
  }
  else {
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i)
      u->PostSettle(lay, (LeabraInhib*)lay, net);
  }

  if((adapt_i.type == AdaptISpec::G_BAR_I) || (adapt_i.type == AdaptISpec::G_BAR_IL)) {
    AdaptGBarI(lay, net);
  }
}

void LeabraLayerSpec::EncodeState(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->EncodeState(lay, net);
}

void LeabraLayerSpec::Compute_SelfReg_Trial(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_SelfReg_Trial(lay, net);
}

float LeabraLayerSpec::Compute_SSE(LeabraLayer* lay, int& n_vals, bool unit_avg, bool sqrt) {
  return lay->Layer::Compute_SSE(n_vals, unit_avg, sqrt);
}


float LeabraLayerSpec::Compute_NormErr_ugp(LeabraLayer* lay, Unit_Group* ug,
					   LeabraInhib* thr, LeabraNetwork* net) {
  float nerr = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    nerr += u->Compute_NormErr(lay, net);
  }
  return nerr;
}

float LeabraLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;					 // assume not contributing
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  int ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      nerr += Compute_NormErr_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      if(net->on_errs && net->off_errs)
	ntot += 2 * rugp->kwta.k;
      else
	ntot += rugp->kwta.k;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
    if(net->on_errs && net->off_errs)
      ntot += 2 * lay->kwta.k;
    else
      ntot += lay->kwta.k;
  }
  if(ntot == 0) return -1.0f;

  lay->norm_err = nerr / (float)ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;		// no contributarse

  return lay->norm_err;
}

void LeabraLayerSpec::Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  lay->avg_netin_sum.avg += lay->netin.avg;
  lay->avg_netin_sum.max += lay->netin.max;
  lay->avg_netin_n++;

  float sum_net = 0.0f;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    prjn->netin_avg = 0.0f;
    int netin_avg_n = 0;
    LeabraUnit* u;
    taLeafItr ui;
    FOR_ITR_EL(LeabraUnit, u, lay->units., ui) {
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      if(u->act_eq < us->opt_thresh.send) continue; // ignore if not above sending thr
      LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(prjn->recv_idx);
      if(!cg) continue;
      float net = cg->Compute_Netin(u);
      cg->net = net;
      prjn->netin_avg += net;
      netin_avg_n++;
    }
    if(netin_avg_n > 0)
      prjn->netin_avg /= (float)netin_avg_n;
    sum_net += prjn->netin_avg;
  }

  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(sum_net > 0.0f)
      prjn->netin_rel = prjn->netin_avg / sum_net;
    // increment epoch-level
    prjn->avg_netin_avg_sum += prjn->netin_avg;
    prjn->avg_netin_rel_sum += prjn->netin_rel;
    prjn->avg_netin_n++;
  }
}

void LeabraLayerSpec::Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
#ifdef DMEM_COMPILE
  lay->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
  if(lay->avg_netin_n > 0) {
    lay->avg_netin.avg = lay->avg_netin_sum.avg / (float)lay->avg_netin_n;
    lay->avg_netin.max = lay->avg_netin_sum.max / (float)lay->avg_netin_n;
  }
  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;
  lay->avg_netin_n = 0;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
#ifdef DMEM_COMPILE
    prjn->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
    if(prjn->avg_netin_n > 0) {
      prjn->avg_netin_avg = prjn->avg_netin_avg_sum / (float)prjn->avg_netin_n;
      prjn->avg_netin_rel = prjn->avg_netin_rel_sum / (float)prjn->avg_netin_n;
    }
    prjn->avg_netin_n = 0;
    prjn->avg_netin_avg_sum = 0.0f;
    prjn->avg_netin_rel_sum = 0.0f;
  }
}

void LeabraLayerSpec::Compute_TrgRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  int n_in = 0;
  int n_out = 0;
  int n_lat = 0;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) {
      prjn->trg_netin_rel = 0.0f;
      continue;
    }
    if(prjn->con_spec->InheritsFrom(&TA_MarkerConSpec)) { // fix these guys
      prjn->trg_netin_rel = 0.0f;
      prjn->direction = Projection::DIR_UNKNOWN;
      continue;
    }
    if(prjn->direction == Projection::FM_INPUT) n_in++;
    else if(prjn->direction == Projection::FM_OUTPUT) n_out++;
    else if(prjn->direction == Projection::LATERAL) n_lat++;
  }
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    float in_trg = cs->rel_net_adapt.trg_fm_input;
    float out_trg = cs->rel_net_adapt.trg_fm_output;
    float lat_trg = cs->rel_net_adapt.trg_lateral;
    if(prjn->direction == Projection::FM_INPUT) {
      if(n_out == 0 && n_lat == 0) in_trg = 1.0;
      else if(n_out == 0) in_trg = in_trg / (in_trg + lat_trg);
      else if(n_lat == 0) in_trg = in_trg / (in_trg + out_trg);
      prjn->trg_netin_rel = in_trg / (float)n_in;
    }
    else if(prjn->direction == Projection::FM_OUTPUT) {
      if(n_in == 0 && n_lat == 0) out_trg = 1.0;
      else if(n_in == 0) out_trg = out_trg / (out_trg + lat_trg);
      else if(n_lat == 0) out_trg = out_trg / (out_trg + in_trg);
      prjn->trg_netin_rel = out_trg / (float)n_out;
    }
    else if(prjn->direction == Projection::LATERAL) {
      if(n_in == 0 && n_out == 0) lat_trg = 1.0;
      else if(n_in == 0) lat_trg = lat_trg / (lat_trg + out_trg);
      else if(n_out == 0) lat_trg = lat_trg / (lat_trg + in_trg);
      prjn->trg_netin_rel = lat_trg / (float)n_lat;
    }
  }
}

void LeabraLayerSpec::Compute_AdaptRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    if(prjn->trg_netin_rel <= 0.0f) continue; // not set
    if(!cs->rel_net_adapt.on) continue;
    if(cs->rel_net_adapt.CheckInTolerance(prjn->trg_netin_rel, prjn->avg_netin_rel))
      continue;
    cs->SetUnique("wt_scale", true);
    cs->wt_scale.rel += cs->rel_net_adapt.rel_lrate * 
      (prjn->trg_netin_rel - prjn->avg_netin_rel);
    if(cs->wt_scale.rel <= 0.0f) cs->wt_scale.rel = 0.0f;
  }
}

void LeabraLayerSpec::Compute_AdaptAbsNetin(LeabraLayer* lay, LeabraNetwork*) {
  if(!abs_net_adapt.on) return;
  float dst = abs_net_adapt.trg_net - lay->avg_netin.max;
  if(fabsf(dst) < abs_net_adapt.tol) return;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    cs->SetUnique("wt_scale", true);
    cs->wt_scale.abs += abs_net_adapt.abs_lrate * dst;
  }
}


//////////////////////////////////////////
//	Stage 6: Learning 		//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_SRAvg(LeabraLayer* lay, LeabraNetwork* net) {
  int eff_int = net->ct_sravg.interval;
  if(net->phase == LeabraNetwork::PLUS_PHASE && net->cycle >= net->ct_sravg.plus_s_st) {
    if((net->ct_time.plus - net->ct_sravg.plus_s_st) < eff_int) {
      eff_int = 1;		// make sure you get short-time/plus phase info!
    }
  }

  if((net->ct_cycle >= net->ct_sravg.start) &&
     (net->ct_cycle < (net->ct_time.inhib_start + net->ct_sravg.end)) &&
     ((net->ct_cycle - net->ct_sravg.start) % eff_int == 0)) {
    bool do_s = false;
    if(net->phase == LeabraNetwork::PLUS_PHASE && net->cycle >= net->ct_sravg.plus_s_st)
      do_s = true;

    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
      u->Compute_SRAvg(lay, net, do_s);
    }
    lay->sravg_m_sum += 1.0f;	// normal weighting
    if(do_s)
      lay->sravg_s_sum += 1.0f;
  }
}

void LeabraLayerSpec::Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net) {
  float ravg_l_sum = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Init_SRAvg(lay, net);
    ravg_l_sum += u->ravg_l;
  }
  if(lay->units.leaves > 0)
    lay->ravg_l_avg = ravg_l_sum / (float)lay->units.leaves;
  else
    lay->ravg_l_avg = 0.15f;

  // note: critical that these are *after* unit/con level one, which requires sravg_m_sum/nrm
  lay->sravg_s_sum = 0.0f;	// clear it!
  lay->sravg_m_sum = 0.0f;	// clear it!
}

void LeabraLayerSpec::AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork*) {
  if((lay->ext_flag & Unit::EXT) && !(lay->ext_flag & Unit::TARG))
    return;			// don't adapt points for input-only layers
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      // use minus phase activations to adapt
      rugp->adapt_i.avg_avg += adapt_i.a_dt * (rugp->acts_m.avg - rugp->adapt_i.avg_avg);
      float dif = rugp->adapt_i.avg_avg - rugp->kwta.pct;
      if(dif < -adapt_i.tol) {	// average is less than target
	// so reduce the point towards lower value
	rugp->adapt_i.i_kwta_pt += adapt_i.p_dt * 
	  ((inhib.kwta_pt - adapt_i.mx_d) - rugp->adapt_i.i_kwta_pt);
      }
      else if(dif > adapt_i.tol) {	// average is more than target
	// so increase point towards higher value
	rugp->adapt_i.i_kwta_pt += adapt_i.p_dt * 
	  ((inhib.kwta_pt + adapt_i.mx_d) - rugp->adapt_i.i_kwta_pt);
      }
    }
  }
  lay->adapt_i.avg_avg += adapt_i.a_dt * (lay->acts_m.avg - lay->adapt_i.avg_avg);
  float dif = lay->adapt_i.avg_avg - lay->kwta.pct;
  if(dif < -adapt_i.tol) {	// average is less than target
    // so reduce the point towards lower value
    lay->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((inhib.kwta_pt - adapt_i.mx_d) - lay->adapt_i.i_kwta_pt);
  }
  else if(dif > adapt_i.tol) {	// average is more than target
    // so increase point towards higher value
    lay->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((inhib.kwta_pt + adapt_i.mx_d) - lay->adapt_i.i_kwta_pt);
  }
}

void LeabraLayerSpec::Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule != LeabraNetwork::LEABRA_CHL) {
    if(lay->sravg_m_sum == 0.0f) return; // if nothing, nothing!
    lay->sravg_m_nrm = 1.0f / lay->sravg_m_sum;
    if(lay->sravg_s_sum > 0.0f) 
      lay->sravg_s_nrm = 1.0f / lay->sravg_s_sum;
    else
      lay->sravg_s_nrm = 1.0f;	// whatever
  }
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_dWt(lay, net);
  AdaptKWTAPt(lay, net);
}

void LeabraLayerSpec::Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->phase_order == LeabraNetwork::MINUS_PLUS_NOTHING ||
      net->phase_order == LeabraNetwork::MINUS_PLUS_MINUS) && 
     net->learn_rule != LeabraNetwork::LEABRA_CHL) {
    return;			// we don't learn here -- only after nothing!
  }
  Compute_dWt_impl(lay, net);
}

void LeabraLayerSpec::Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->learn_rule != LeabraNetwork::LEABRA_CHL) && (net->phase_order == LeabraNetwork::MINUS_PLUS_PLUS)) {
    Compute_dWt_impl(lay, net);	// go for it..
  }
  return;			// standard layers never learn here
}

void LeabraLayerSpec::Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net) {
  // all types learn here..
  Compute_dWt_impl(lay, net);
}


void LeabraLayerSpec::Compute_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_Weights(lay, net);
}

void LeabraLayer::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!spec.CheckSpec(GetTypeDef())) {
    rval = false; // fatal
    return;
  }
  if(!spec->CheckConfig_Layer(this, quiet)) {
    rval = false;
  }
}

//////////////////////////
// 	LeabraLayer	//
//////////////////////////
  
void AvgMaxVals::Initialize() {
  avg = max = 0.0f; max_i = -1;
}

void AvgMaxVals::Copy_(const AvgMaxVals& cp) {
  avg = cp.avg; max = cp.max; max_i = cp.max_i;
}

void KWTAVals::Initialize() {
  k = 12;
  pct = .25f;
  pct_c = .75f;
  adth_k = 1;
  k_ithr = 0.0f;
  k1_ithr = 0.0f;
  ithr_r = 0.0f;
  ithr_diff = 0.0f;
  tie_brk = 0;
}

void KWTAVals::Copy_(const KWTAVals& cp) {
  k = cp.k;
  pct = cp.pct;
  pct_c = cp.pct_c;
  adth_k = cp.adth_k;
  k_ithr = cp.k_ithr;
  k1_ithr = cp.k1_ithr;
  ithr_r = cp.ithr_r;
  ithr_diff = cp.ithr_diff;
  tie_brk = cp.tie_brk;
}

void KWTAVals::Compute_Pct(int n_units) {
  if(n_units > 0)
    pct = (float)k / (float)n_units;
  else
    pct = 0.0f;
  pct_c = 1.0f - pct;
}

void KWTAVals::Compute_IThrR() {
  if(k1_ithr <= 0.0f)
    ithr_r = 0.0f;
  else 
    ithr_r = logf(k_ithr / k1_ithr);
}

void AdaptIVals::Initialize() {
  avg_avg = 0.0f;
  i_kwta_pt = 0.0f;
  g_bar_i = 1.0f;
  g_bar_l = .1f;
}

void AdaptIVals::Copy_(const AdaptIVals& cp) {
  avg_avg = cp.avg_avg;
  i_kwta_pt = cp.i_kwta_pt;
  g_bar_i = cp.g_bar_i;
  g_bar_l = cp.g_bar_l;
}

void InhibVals::Initialize() {
  kwta = 0.0f;
  g_i = 0.0f;
  gp_g_i = 0.0f;
  g_i_orig = 0.0f;
}

void InhibVals::Copy_(const InhibVals& cp) {
  kwta = cp.kwta;
  g_i = cp.g_i;
  gp_g_i = cp.gp_g_i;
  g_i_orig = cp.g_i_orig;
}

void LeabraInhib::Inhib_Initialize() {
  kwta.k = 1;
  kwta.pct = .25;
  kwta.pct_c = .75;
  i_val.Defaults();
  phase_dif_ratio = 1.0f;
  maxda = 0.0f;
}

void LeabraInhib::Inhib_Init_Acts(LeabraLayerSpec*) {
  i_val.Defaults();
  netin.Defaults();
  i_thrs.Defaults();
  acts.Defaults();
  un_g_i.Defaults();
  maxda = 0.0f;
}

void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  projections.SetBaseType(&TA_LeabraPrjn);
  send_prjns.SetBaseType(&TA_LeabraPrjn);
  units.SetBaseType(&TA_LeabraUnit);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Inhib_Initialize();
  stm_gain = .5f;
  hard_clamped = false;
  dav = 0.0f;
  sravg_s_sum = 0.0f;
  sravg_s_nrm = 0.0f;
  sravg_m_sum = 0.0f;
  sravg_m_nrm = 0.0f;
  ravg_l_avg = 0.15f;
  norm_err = 0.0f;
  da_updt = false;
  net_rescale = 1.0f;

  avg_netin_n = 0;
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}  

void LeabraLayer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);
  taBase::Own(acts_p2, this);
  taBase::Own(acts_m2, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);

  taBase::Own(avg_netin, this);
  taBase::Own(avg_netin_sum, this);

  taBase::Own(misc_iar, this);

#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
#endif
  spec.SetDefaultSpec(this);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
}

void LeabraLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();
  misc_iar.CutLinks();
}

void LeabraInhib::Inhib_Copy_(const LeabraInhib& cp) {
  netin = cp.netin;
  i_thrs = cp.i_thrs;
  acts = cp.acts;
  acts_p = cp.acts_p;
  acts_m = cp.acts_m;
  acts_p2 = cp.acts_p2;
  acts_m2 = cp.acts_m2;
  phase_dif_ratio = cp.phase_dif_ratio;
  kwta = cp.kwta;
  i_val = cp.i_val;
  un_g_i = cp.un_g_i;
}

void LeabraLayer::Copy_(const LeabraLayer& cp) {
  Inhib_Copy_(cp);
  spec = cp.spec;
  stm_gain = cp.stm_gain;
  hard_clamped = cp.hard_clamped;
  misc_iar = cp.misc_iar;
  dav = cp.dav;
  sravg_s_sum = cp.sravg_s_sum;
  sravg_s_nrm = cp.sravg_s_nrm;
  sravg_m_sum = cp.sravg_m_sum;
  sravg_m_nrm = cp.sravg_m_nrm;
  ravg_l_avg = cp.ravg_l_avg;
  norm_err = cp.norm_err;

  // this will update spec pointer to new network if we are copied from other guy
  // only if the network is not otherwise already copying too!!
  // (other pointers are already dealt with in Layer)
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Network);
}

void LeabraLayer::ResetSortBuf() {
  Inhib_ResetSortBuf();		// reset sort buf after any edit..
  if(units.gp.size > 0) {
    for(int g=0; g<units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)units.gp[g];
      rugp->Inhib_ResetSortBuf();
    }
  }
}

void LeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ResetSortBuf();
}

bool LeabraLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)	return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((LeabraLayerSpec*)sp);
  else
    return false;
  return true;
} 

void LeabraLayer::BuildUnits() {
  ResetSortBuf();
  inherited::BuildUnits();
}

#ifdef DMEM_COMPILE
void LeabraLayer::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraLayer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif

void LeabraUnit_Group::Initialize() {
  Inhib_Initialize();
  misc_state = 0;
  misc_state1 = 0;
  misc_state2 = 0;
}

void LeabraUnit_Group::InitLinks() {
  inherited::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);
  taBase::Own(acts_p2, this);
  taBase::Own(acts_m2, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);
}

void LeabraUnit_Group::Copy_(const LeabraUnit_Group& cp) {
  Inhib_Copy_(cp);
  misc_state = cp.misc_state;
  misc_state1 = cp.misc_state1;
  misc_state2 = cp.misc_state2;
}


//////////////////////////
//  	Network		//
//////////////////////////

void LeabraNetMisc::Initialize() {
  cyc_syn_dep = false;
  syn_dep_int = 20;
}

void CtTrialTiming::Initialize() {
  minus = 50;
  plus = 20;
  inhib = 1;
  n_avg_only_epcs = 1;

  total_cycles = minus + plus + inhib;
  inhib_start = minus + plus;
}

void CtTrialTiming::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  total_cycles = minus + plus + inhib;
  inhib_start = minus + plus;
}

void CtSRAvgSpec::Initialize() {
  start = 30;
  end = 1;
  interval = 1;
  plus_s_st = 19;
}

void CtSineInhibMod::Initialize() {
  start = 30;
  duration = 20;
  n_pi = 2.0f;
  burst_i = 0.02f;
  trough_i = 0.02f;
}

void CtFinalInhibMod::Initialize() {
  start = 20;
  end = 25;
  inhib_i = 0.0f;
}

void LeabraNetwork::GraphInhibMod(bool flip_sign, DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_InhibMod", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* cyc_col = graph_data->FindMakeColName("ct_cycle", idx, VT_INT);
  DataCol* imod_col = graph_data->FindMakeColName("inhib_mod", idx, VT_FLOAT);
//   imod->SetUserData("MIN", 0.0f);
//   imod->SetUserData("MAX", 1.0f);

  float bi = ct_sin_i.burst_i;
  float ti = ct_sin_i.trough_i;
  float ii = ct_fin_i.inhib_i;

  for(int cyc = 0; cyc < ct_time.total_cycles; cyc++) {
    float imod = ct_sin_i.GetInhibMod(cyc, bi, ti) +
      ct_fin_i.GetInhibMod(cyc - ct_time.inhib_start, ii);
    if(flip_sign) imod *= -1.0f;
    graph_data->AddBlankRow();
    cyc_col->SetValAsInt(cyc, -1);
    imod_col->SetValAsFloat(imod, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}


void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);
  min_engine = &TA_LeabraEngine;

  learn_rule = LEABRA_CHL;
  prv_learn_rule = -1;
  phase_order = MINUS_PLUS;
  no_plus_test = true;
  trial_init = DECAY_STATE;
  sequence_init = DO_NOTHING;
  phase = MINUS_PHASE;
  nothing_phase = false;
  phase_no = 0;
  phase_max = 2;

  cycle_max = 60;
  min_cycles = 15;
  min_cycles_phase2 = 35;

  minus_cycles = 0.0f;
  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;

  netin_mod = 1;
  //  send_delta = false;
  send_delta = true;		// so much faster -- should always be on by default

  send_pct = 0.0f;
  send_pct_n = send_pct_tot = 0;
  avg_send_pct = 0.0f;
  avg_send_pct_sum = 0.0f;
  avg_send_pct_n = 0;

  maxda_stopcrit = .005f;
  maxda = 0.0f;
  
  trg_max_act_stopcrit = 1.0f;	// disabled
  trg_max_act = 0.0f;

  ext_rew = 0.0f;
  avg_ext_rew = 0.0f;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;

  on_errs = true;
  off_errs = true;

  norm_err = 0.0f;
  avg_norm_err = 0.0f;
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;
}

void LeabraNetwork::Init_Counters() {
  inherited::Init_Counters();
  phase = MINUS_PHASE;
  nothing_phase = false;
  phase_no = 0;
}

void LeabraNetwork::Init_Stats() {
  inherited::Init_Stats();
  maxda = 0.0f;
  trg_max_act = 0.0f;

  minus_cycles = 0.0f;
  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;

  minus_output_name = "";

  send_pct_n = send_pct_tot = 0;
  send_pct = 0.0f;
  avg_send_pct = 0.0f;
  avg_send_pct_sum = 0.0f;
  avg_send_pct_n = 0;

  ext_rew = 0.0f;
  avg_ext_rew = 0.0f;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;

  norm_err = 0.0f;
  avg_norm_err = 0.0f;
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;
}

void LeabraNetwork::Init_Sequence() {
  inherited::Init_Sequence();
  if(sequence_init == INIT_STATE) {
    Init_Acts();
  }
  else if(sequence_init == DECAY_STATE) {
    DecayState();
  }
}


void LeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_LeabraCon;
  prjn->recvcons_type = &TA_LeabraRecvCons;
  prjn->sendcons_type = &TA_LeabraSendCons;
  prjn->con_spec.SetBaseType(&TA_LeabraConSpec);
}

void LeabraNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ct_time.UpdateAfterEdit();

  if(TestWarning(ct_sravg.plus_s_st >= ct_time.plus, "UAE",
	       "ct_sravg.plus_s_st is higher than ct_time.plus (# of cycles in plus phase)"
	       "just set it to plus-2")) {
    ct_sravg.plus_s_st = ct_time.plus -2;
  }

  if(TestWarning(!off_errs && !on_errs, "UAE", "can't have both off_errs and on_errs be off (no err would be computed at all) -- turned both back on")) {
    on_errs = true;
    off_errs = true;
  }
  
  if(prv_learn_rule == -1) {
    prv_learn_rule = learn_rule;
  }
  else if(prv_learn_rule != learn_rule) {
    SetLearnRule();
    prv_learn_rule = learn_rule;
  }
}

void LeabraNetwork::SetLearnRule_ConSpecs(BaseSpec_Group* spgp) {
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, spgp->, i) {
    if(bs->InheritsFrom(&TA_LeabraConSpec)) {
      ((LeabraConSpec*)bs)->SetLearnRule(this);
    }
    SetLearnRule_ConSpecs(&bs->children); // recurse
  }
}

void LeabraNetwork::SetLearnRule() {
  if(learn_rule == LEABRA_CHL) {
    if(phase_order == MINUS_PLUS_NOTHING) {
      phase_order = MINUS_PLUS;
    }
    maxda_stopcrit = 0.005f;
    min_cycles = 15;
    min_cycles_phase2 = 35;
    cycle_max = 60;
  }
  else {
    if(phase_order == MINUS_PLUS) {
      phase_order = MINUS_PLUS_NOTHING;
    }
    maxda_stopcrit = -1;
    min_cycles = 0;
    min_cycles_phase2 = 0;
  }

  SetLearnRule_ConSpecs(&specs);
  // set all my specs -- otherwise it looks weird in hierarchy for unused parent specs

  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->SetLearnRule(this);
  }
}

//////////////////////////////////
// 	Cycle-Level Functions	//
//////////////////////////////////

void LeabraNetwork::Compute_Netin() {
  send_pct_n = send_pct_tot = 0;
  if (send_delta) {
    if (!(net_inst.ptr() &&
      ((LeabraEngineInst*)(net_inst.ptr()))->OnSend_NetinDelta())) 
    {
      Init_NetinDelta();	// this is done first because of sender-based net
      LeabraLayer* l;
      taLeafItr i;
      FOR_ITR_EL(LeabraLayer, l, layers., i) {
        if(!l->lesioned())
          l->Send_NetinDelta(this);
      }
    }
#ifdef DMEM_COMPILE
    dmem_share_units.Sync(3);
#endif
  }
  else {
    if (!(net_inst.ptr() &&
      ((LeabraEngineInst*)(net_inst.ptr()))->OnSend_Netin())) 
    {
      Init_Netin();		// this is done first because of sender-based net
      LeabraLayer* l;
      taLeafItr i;
      FOR_ITR_EL(LeabraLayer, l, layers., i) {
        if(!l->lesioned())
          l->Send_Netin(this);
    }
    }
#ifdef DMEM_COMPILE
    DMem_SyncNet();
#endif
  }
  if(send_pct_tot > 0) {
    send_pct = (float)send_pct_n / (float)send_pct_tot;
    avg_send_pct_sum += send_pct;
    avg_send_pct_n++;
  }
}

void LeabraNetwork::Compute_Clamp_NetAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Clamp_NetAvg(this);
  }
}

void LeabraNetwork::Compute_Inhib() {
  bool do_lay_gp = false;
  if(layers.gp.size == 0) do_lay_gp = true; // falsely set to true to prevent further checking
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Inhib(this);
    if(!do_lay_gp) {
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      if(laysp->kwta.gp_i) do_lay_gp = true;
    }
  }
  if(layers.gp.size == 0) do_lay_gp = false; // now override anything
  if(do_lay_gp) {
    for(int lgi = 0; lgi < layers.gp.size; lgi++) {
      Layer_Group* lg = (Layer_Group*)layers.gp[lgi];
      float lay_gp_g_i = 0.0f;
      for(int li = 0; li < lg->size; li++) {
	lay = (LeabraLayer*)lg->FastEl(li);
	LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
	if(lay->lesioned() || !laysp->kwta.gp_i) continue;
	float lay_val = laysp->kwta.gp_g * lay->i_val.g_i;
	lay_gp_g_i = MAX(lay_val, lay_gp_g_i);
      }
      if(lay_gp_g_i > 0.0f) {	// actually got something
	for(int li = 0; li < lg->size; li++) {
	  lay = (LeabraLayer*)lg->FastEl(li);
	  LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
	  if(lay->lesioned() || !laysp->kwta.gp_i) continue;
	  lay->i_val.gp_g_i = lay_gp_g_i;
	  if(laysp->kwta.gp_g > 1.0)
	    lay->i_val.gp_g_i /= laysp->kwta.gp_g; // if > 1, need to renorm otherwise its own inhib will cancel itself out!!
	  lay->i_val.g_i = MAX(lay->i_val.gp_g_i, lay->i_val.g_i);

	  if(lay->units.gp.size > 0) {
	    lay->Compute_LayInhibToGps(this);
	  }
	}
      }
    }
  }
}

void LeabraNetwork::Compute_ApplyInhib() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ApplyInhib(this);
  }
}

void LeabraNetwork::Compute_InhibAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_InhibAvg(this);
  }
}

void LeabraNetwork::Compute_Act() {
  output_name = "";		// this will be updated by layer compute_act
  maxda = 0.0f;		// initialize
  trg_max_act = 0.0f;
  inherited::Compute_Act();
}

void LeabraNetwork::Compute_CycSynDep() {
  if(!net_misc.cyc_syn_dep) return;
  if(ct_cycle % net_misc.syn_dep_int != 0) return;

  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_CycSynDep(this);
  }
}
  
void LeabraNetwork::Compute_SRAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_SRAvg(this);
  }
}
  
void LeabraNetwork::Init_SRAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Init_SRAvg(this);
  }
}
  
void LeabraNetwork::Cycle_Run() {
  if((cycle % netin_mod) == 0) {
    Compute_Netin();
    Compute_Clamp_NetAvg();
    Compute_Inhib();
    Compute_ApplyInhib();
    Compute_InhibAvg();
  }
  Compute_Act();
  Compute_CycSynDep();

  // ct_cycle is pretty useful anyway
  if(phase_no == 0 && cycle == 0) // detect start of trial
    ct_cycle = 0;
  Compute_SRAvg();		// note: only ctleabra variants do con-level compute here
  ct_cycle++;
}

//////////////////////////////////
// 	Settle-Level Functions	//
//////////////////////////////////

void LeabraNetwork::Compute_Active_K() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Active_K();	// this gets done at the outset..
  }
}
  
void LeabraNetwork::DecayEvent() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayEvent(this);
  }
}

void LeabraNetwork::DecayPhase() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayPhase(this);
  }
}

void LeabraNetwork::DecayPhase2() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayPhase2(this);
  }
}

void LeabraNetwork::PhaseInit() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->PhaseInit(this);
  }
}

void LeabraNetwork::ExtToComp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->ExtToComp(this);
  }
}

void LeabraNetwork::TargExtToComp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->TargExtToComp(this);
  }
}

void LeabraNetwork::Compute_HardClamp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_HardClamp(this);
  }
}

void LeabraNetwork::Compute_NetinScale() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_NetinScale(this);
  }
}

void LeabraNetwork::Send_ClampNet() {
  LeabraLayer* lay;
  taLeafItr l;
  // first init
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Init_ClampNet(this);
  }
  // then send
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Send_ClampNet(this);
  }
#ifdef DMEM_COMPILE
  if(send_delta)
    dmem_share_units.Sync(4);
#endif
}

void LeabraNetwork::PostSettle() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->PostSettle(this);
  }
}

void LeabraNetwork::Settle_Init_Decay() {
  if(phase_no == 1) {
    DecayPhase();		// prepare for next phase
  }
  else {
    DecayPhase2();
  }

  if(nothing_phase) {
    if(phase_order != MINUS_PLUS_MINUS && phase_order != MINUS_PLUS_PLUS_MINUS) {
      TargExtToComp();		// all external input is now 'comparison' = nothing!
    }
  }
}

void LeabraNetwork::Settle_Init_CtTimes() {
  if(learn_rule == LEABRA_CHL) return;

  if(phase_order >= MINUS_PLUS_PLUS_NOTHING) {
    switch(phase_no) {
    case 0:
      cycle_max = ct_time.minus;
      break;
    case 1:
      cycle_max = ct_time.plus;
      break;
    case 2:
      cycle_max = ct_time.plus;
      break;
    case 3:
      cycle_max = ct_time.inhib;
      break;
    }
  }
  else {			// for all other cases
    switch(phase_no) {
    case 0:
      cycle_max = ct_time.minus;
      break;
    case 1:
      cycle_max = ct_time.plus;
      break;
    case 2:
      cycle_max = ct_time.inhib;
      break;
    }
  }
}

void LeabraNetwork::Settle_Init() {
  int tmp_cycle = cycle;
  cycle = -2;			// special signal for settle init

  Settle_Init_Decay();

  Settle_Init_CtTimes();

  Compute_Active_K();		// compute here because could depend on pat_n
  PhaseInit();
  
  Compute_HardClamp();		// first clamp all hard-clamped input acts
  Compute_NetinScale();		// and then compute net scaling
  Send_ClampNet();		// and send net from clamped inputs
  cycle = tmp_cycle;
}	

void LeabraNetwork::Settle_Final() {
  // all weight changes take place here for consistency!
  PostSettle();
  if(train_mode != TEST) {
    switch(phase_order) {
    case MINUS_PLUS:
    case PLUS_MINUS:
    case PLUS_NOTHING:
      if(phase_no == 1)
	Compute_dWt_FirstPlus();
      break;
    case PLUS_ONLY:
      Compute_dWt_FirstPlus();
      break;
    case MINUS_PLUS_NOTHING:
    case MINUS_PLUS_MINUS:
      if(phase_no == 1)
	Compute_dWt_FirstPlus();
      else if(phase_no == 2)
	Compute_dWt_Nothing();
      break;
    case MINUS_PLUS_PLUS:
      if(phase_no == 1)
	Compute_dWt_FirstPlus();
      else if(phase_no == 2)
	Compute_dWt_SecondPlus();
      break;
    case MINUS_PLUS_PLUS_NOTHING:
    case MINUS_PLUS_PLUS_MINUS:
      if(phase_no == 1)
	Compute_dWt_FirstPlus();
      else if(phase_no == 2)
	Compute_dWt_SecondPlus();
      else if(phase_no == 3)
	Compute_dWt_Nothing();
      break;
    }
  }
}
  
//////////////////////////////////
// 	Trial-Level Functions	//
//////////////////////////////////

void LeabraNetwork::SetCurLrate() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->SetCurLrate(this, epoch);
  }
}

void LeabraNetwork::DecayState() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayEvent(this);
  }
}

void LeabraNetwork::EncodeState() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->EncodeState(this);
  }
}

void LeabraNetwork::Compute_SelfReg_Trial() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_SelfReg_Trial(this);
  }
}

void LeabraNetwork::Compute_dWt_FirstPlus() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_dWt_FirstPlus(this);
  }
}

void LeabraNetwork::Compute_dWt_SecondPlus() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_dWt_SecondPlus(this);
  }
}

void LeabraNetwork::Compute_dWt_Nothing() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_dWt_Nothing(this);
  }
}

void LeabraNetwork::Trial_Init() {
  SetCurLrate();

  cycle = -1;
  phase = MINUS_PHASE;
  nothing_phase = false;

  bool no_plus_testing = false;
  if(no_plus_test && (train_mode == TEST)) {
    no_plus_testing = true;
  }

  switch(phase_order) {
  case MINUS_PLUS:
    if(no_plus_testing)
      phase_max=1;
    else
      phase_max=2;
    break;
  case PLUS_MINUS:
    if(no_plus_testing)
      phase_max=1;		// just do minus
    else {
      phase_max=2;
      phase = PLUS_PHASE;
    }
    break;
  case PLUS_ONLY:
    phase_max=1;
    phase = PLUS_PHASE;
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    phase_max=3;	// ignore no_plus_test here -- just turn PLUS into extra MINUS
    break;
  case PLUS_NOTHING:
    phase_max=2;	// ignore no_plus_test here -- just turn PLUS into MINUS
    break;
  case MINUS_PLUS_PLUS:
    phase_max=3;	// ignore no_plus_test here -- just turn PLUS into extra MINUS
    break;
  case MINUS_PLUS_PLUS_NOTHING:
  case MINUS_PLUS_PLUS_MINUS:
    phase_max=4;	// ignore no_plus_test here -- just turn PLUS into extra MINUS
    break;
  }

  if(trial_init == INIT_STATE)
    Init_Acts();
  else if(trial_init == DECAY_STATE)
    DecayState();
  Init_SRAvg();			// 
}

void LeabraNetwork::Trial_UpdatePhase() {
  if(phase_no == phase_max) return; // done!

  // this assumes that phase_no > 0 -- called after updating phase_no
  nothing_phase = false;
  bool no_plus_testing = false;
  if(no_plus_test && (train_mode == TEST)) {
    no_plus_testing = true;
  }

  switch(phase_order) {
  case MINUS_PLUS:
    phase = PLUS_PHASE;
    break;
  case PLUS_MINUS:
    phase = MINUS_PHASE;
    break;
  case PLUS_ONLY:
    // nop
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    // diff between these two is in Settle_Init_Decay: TargExtToComp()
    if(phase_no == 1) {
      if(no_plus_testing)
	phase = MINUS_PHASE;	// another minus
      else
	phase = PLUS_PHASE;
    }
    else {
      phase = MINUS_PHASE;
      nothing_phase = true;
    }
    break;
  case PLUS_NOTHING:
    phase = MINUS_PHASE;
    nothing_phase = true;
    break;
  case MINUS_PLUS_PLUS:
    if(no_plus_testing)
      phase = MINUS_PHASE;	// another minus
    else
      phase = PLUS_PHASE;
    break;
  case MINUS_PLUS_PLUS_NOTHING:
  case MINUS_PLUS_PLUS_MINUS:
    // diff between these two is in Settle_Init_Decay: TargExtToComp()
    if(phase_no <= 2) {
      if(no_plus_testing)
	phase = MINUS_PHASE;	// another minus
      else
	phase = PLUS_PHASE;
    }
    else {
      phase = MINUS_PHASE;
      nothing_phase = true;
    }
    break;
  }
}

void LeabraNetwork::Trial_Final() {
  EncodeState();
  Compute_SelfReg_Trial();
}

void LeabraNetwork::Compute_ExtRew() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    if(lay->spec.SPtr()->GetTypeDef() != &TA_ExtRewLayerSpec) continue;
    LeabraUnit* eru = (LeabraUnit*)lay->units.Leaf(0);
    if(eru->misc_1 == 0.0f) { // indication of no reward available
      ext_rew = -1.1f;
    }
    else {
      ext_rew = eru->act_eq;	// just set it!
      avg_ext_rew_sum += ext_rew;
      avg_ext_rew_n++;
    }
  }
}

void LeabraNetwork::Compute_NormErr() {
  float nerr_sum = 0.0f;
  float nerr_avail = 0.0f;
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    float nerr = lay->Compute_NormErr(this);
    if(nerr >= 0.0f) {
      nerr_avail += 1.0f;
      nerr_sum += nerr;
    }
  }
  if(nerr_avail > 0.0f) {
    norm_err = nerr_sum / nerr_avail; // normalize contribution across layers
  }
  else {
    norm_err = 0.0f;
  }

  avg_norm_err_sum += norm_err;
  avg_norm_err_n++;
}

void LeabraNetwork::Compute_MinusCycles() {
  minus_cycles = cycle;
  avg_cycles_sum += minus_cycles;
  avg_cycles_n++;
}

bool LeabraNetwork::Compute_TrialStats_Test() {
  bool is_time = false;

  bool no_plus_testing = false;
  if(no_plus_test && (train_mode == TEST)) {
    no_plus_testing = true;
  }

  switch(phase_order) {
  case MINUS_PLUS:
    if(phase_no == 0) is_time = true;
    break;
  case PLUS_MINUS:
    if(no_plus_testing)
      is_time = true;
    else {
      if(phase_no == 1) is_time = true;
    }
    break;
  case PLUS_ONLY:
    is_time = true;
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    if(no_plus_testing) {
      if(phase_no == 1) is_time = true;
    }
    else {
      if(phase_no == 0) is_time = true;
    }
    break;
  case PLUS_NOTHING:
    if(phase_no == 1) is_time = true;
    break;
  case MINUS_PLUS_PLUS:
    if(phase_no == 0) is_time = true;
    break;
  case MINUS_PLUS_PLUS_NOTHING:
  case MINUS_PLUS_PLUS_MINUS:
    if(phase_no == 0) is_time = true;
    break;
  }

  return is_time;
}


void LeabraNetwork::Compute_TrialStats() {
  inherited::Compute_TrialStats();
  Compute_NormErr();
  Compute_MinusCycles();
  minus_output_name = output_name; // grab and hold..
}

void LeabraNetwork::Compute_AbsRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AbsRelNetin(this);
  }
}

void LeabraNetwork::Compute_AvgAbsRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AvgAbsRelNetin(this);
  }
}

void LeabraNetwork::Compute_TrgRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_TrgRelNetin(this);
  }
}

void LeabraNetwork::Compute_AdaptRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AdaptRelNetin(this);
  }
}

void LeabraNetwork::Compute_AdaptAbsNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AdaptAbsNetin(this);
  }
}

void LeabraNetwork::Compute_AvgCycles() {
  if(avg_cycles_n > 0) {
    avg_cycles = avg_cycles_sum / (float)avg_cycles_n;
  }
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;
}

void LeabraNetwork::Compute_AvgSendPct() {
  if(avg_send_pct_n > 0) {
    avg_send_pct = avg_send_pct_sum / (float)avg_send_pct_n;
  }
  avg_send_pct_sum = 0.0f;
  avg_send_pct_n = 0;
}

void LeabraNetwork::Compute_AvgExtRew() {
  if(avg_ext_rew_n > 0) {
    avg_ext_rew = avg_ext_rew_sum / (float)avg_ext_rew_n;
  }
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::Compute_AvgNormErr() {
  if(avg_norm_err_n > 0) {
    avg_norm_err = avg_norm_err_sum / (float)avg_norm_err_n;
  }
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;
}

void LeabraNetwork::Compute_EpochStats() {
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
  Compute_AvgExtRew();
  Compute_AvgNormErr();
  Compute_AvgSendPct();
}

///////////////////////////////////////////////////////////////////////////////////////
// 	LeabraProject

void LeabraProject::Initialize() {
  networks.SetBaseType(&TA_LeabraNetwork);
  wizards.SetBaseType(&TA_LeabraWizard);
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

void LeabraWizard::Initialize() {
  connectivity = BIDIRECTIONAL;
  default_net_type = &TA_LeabraNetwork;
}

bool LeabraWizard::StdNetwork(TypeDef* net_type, Network* net) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = proj->GetNewNetwork(net_type);
    if(TestError(!net, "StdNetwork", "network is NULL and could not make a new one -- aborting!")) return false;
  }
  if(!inherited::StdNetwork(net_type, net)) return false;
  return StdLayerSpecs((LeabraNetwork*)net);
}

bool LeabraWizard::StdLayerSpecs(LeabraNetwork* net) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "StdLayerSpecs", "network is NULL and could not make a new one -- aborting!")) return false;
  }
  LeabraLayerSpec* hid;
//   if(net->InheritsFrom(&TA_CtLeabraNetwork))
//     hid = (LeabraLayerSpec*)net->FindMakeSpec(NULL, &TA_CtLeabraLayerSpec);
//   else
    hid = (LeabraLayerSpec*)net->FindMakeSpec(NULL, &TA_LeabraLayerSpec);
  hid->name = "HiddenLayer";
  LeabraLayerSpec* inout;
//   if(net->InheritsFrom(&TA_CtLeabraNetwork))
//     inout = (LeabraLayerSpec*)hid->children.FindMakeSpec("Input_Output", &TA_CtLeabraLayerSpec);
//   else
    inout = (LeabraLayerSpec*)hid->children.FindMakeSpec("Input_Output", &TA_LeabraLayerSpec);
  hid->inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  hid->inhib.kwta_pt = .6f;
  inout->SetUnique("inhib", true);
  inout->SetUnique("kwta", true);
  inout->inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inout->inhib.kwta_pt = .25f;
  inout->kwta.k_from = KWTASpec::USE_PAT_K;

  int i;
  if(net->layers.size == layer_cfg.size) {	// likely to be using specs
    for(i=0;i<layer_cfg.size;i++) {
      LayerWizEl* el = (LayerWizEl*)layer_cfg[i];
      Layer* lay = (Layer*)net->layers.FindName(el->name);
      if(lay != NULL) {
	if(el->io_type == LayerWizEl::HIDDEN)
	  lay->SetLayerSpec(hid);
	else
	  lay->SetLayerSpec(inout);
      }
    }
  }
  else {
    for(i=0;i<net->layers.size;i++) {
      Layer* lay = (Layer*)net->layers[i];
      if(lay->layer_type == Layer::HIDDEN)
	lay->SetLayerSpec(hid);
      else
	lay->SetLayerSpec(inout);
    }
  }

  // move the bias spec under the con spec
  LeabraBiasSpec* bs;
//   if(net->InheritsFrom(&TA_CtLeabraNetwork))
//     bs = (LeabraBiasSpec*)net->specs.FindType(&TA_CtLeabraBiasSpec);
//   else
    bs = (LeabraBiasSpec*)net->specs.FindType(&TA_LeabraBiasSpec);
  if(bs != NULL) {
    LeabraConSpec* ps = (LeabraConSpec*)bs->FindParent();
    if(ps != NULL) return false;
//     if(net->InheritsFrom(&TA_CtLeabraNetwork))
//       ps = (LeabraConSpec*)net->specs.FindSpecTypeNotMe(&TA_CtLeabraConSpec, bs);
//     else
      ps = (LeabraConSpec*)net->specs.FindSpecTypeNotMe(&TA_LeabraConSpec, bs);
    if(ps != NULL) {
      ps->children.Transfer(bs);
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////
//			Unit Inhib
///////////////////////////////////////////////////////////////

bool LeabraWizard::UnitInhib(LeabraNetwork* net, int n_inhib_units) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "UnitInhib", "network is NULL and could not make a new one -- aborting!")) return false;
  }

  net->RemoveUnits();
  
  LeabraUnitSpec* basic_us = (LeabraUnitSpec*)net->FindSpecType(&TA_LeabraUnitSpec);
  if(TestError(!basic_us, "UnitInhib", "basic LeabraUnitSpec not found, bailing!")) {
    return false;
  }
  LeabraUnitSpec* inhib_us = (LeabraUnitSpec*)basic_us->children.FindMakeSpec("InhibUnits", &TA_LeabraUnitSpec);

  LeabraConSpec* basic_cs = (LeabraConSpec*)net->FindSpecType(&TA_LeabraConSpec);
  if(TestError(!basic_cs, "UnitInhib", "basic LeabraConSpec not found, bailing!")) {
    return false;
  }
  LeabraConSpec* inhib_cs = (LeabraConSpec*)basic_cs->children.FindMakeSpec("InhibCons", &TA_LeabraConSpec);

  LeabraConSpec* fb_inhib_cs = (LeabraConSpec*)basic_cs->children.FindMakeSpec("FBtoInhib", &TA_LeabraConSpec);
  LeabraConSpec* ff_inhib_cs = (LeabraConSpec*)fb_inhib_cs->children.FindMakeSpec("FFtoInhib", &TA_LeabraConSpec);

  LeabraLayerSpec* basic_ls = (LeabraLayerSpec*)net->FindSpecType(&TA_LeabraLayerSpec);
  if(TestError(!basic_ls, "UnitInhib", "basic LeabraLayerSpec not found, bailing!")) {
    return false;
  }
  LeabraLayerSpec* inhib_ls = (LeabraLayerSpec*)basic_ls->children.FindMakeSpec("InhibLayers", &TA_LeabraLayerSpec);

  FullPrjnSpec* fullprjn = (FullPrjnSpec*)net->FindSpecType(&TA_FullPrjnSpec);
  if(TestError(!fullprjn, "UnitInhib", "basic FullPrjnSpec not found, bailing!")) {
    return false;
  }

  // todo: optimize these params..
  basic_us->dt.vm = .04f;
  basic_us->g_bar.i = 10.0f;
  inhib_us->SetUnique("dt", true);
  inhib_us->dt.vm = .07f;

  inhib_cs->SetUnique("rnd", true);
  inhib_cs->rnd.mean = 1.0f;  inhib_cs->rnd.var = 0.0f;
  inhib_cs->SetUnique("wt_limits", true);
  inhib_cs->wt_limits.sym = false;
  inhib_cs->SetUnique("inhib", true);
  inhib_cs->inhib = true;

  fb_inhib_cs->SetUnique("wt_limits", true);
  fb_inhib_cs->wt_limits.sym = false;
  fb_inhib_cs->SetUnique("rnd", true);
  fb_inhib_cs->rnd.mean = .5f;  fb_inhib_cs->rnd.var = 0.05f;
  fb_inhib_cs->SetUnique("lrate", true);
  fb_inhib_cs->lrate = 0.0f;

  // todo: optimize
  ff_inhib_cs->SetUnique("wt_scale", true);
  ff_inhib_cs->wt_scale.abs = .4f;

  basic_ls->inhib.type = LeabraInhibSpec::UNIT_INHIB;
  basic_ls->adapt_i.type = AdaptISpec::G_BAR_I;
  basic_ls->adapt_i.tol = .02f;	// these params sometimes get off..
  basic_ls->adapt_i.p_dt = .1f;

  inhib_ls->SetUnique("kwta", true);
  inhib_ls->kwta.k_from = KWTASpec::USE_PCT;
  inhib_ls->kwta.pct = .34f;

  basic_us->UpdateAfterEdit();
  basic_cs->UpdateAfterEdit();
  basic_ls->UpdateAfterEdit();

  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    if(lay->layer_type == Layer::INPUT) continue;
    String nm = lay->name;
    nm.downcase();
    if(nm.contains("_inhib")) continue;

    String inm = lay->name + "_Inhib";
    LeabraLayer* ilay = (LeabraLayer*)net->layers.FindName(inm);
    if(ilay == NULL) {
      ilay = (LeabraLayer*)net->layers.NewEl(1);
      ilay->name = inm;
      ilay->pos.z = lay->pos.z;
      ilay->pos.x = lay->pos.x + lay->act_geom.x + 1;
      ilay->un_geom.n = n_inhib_units;
      if(n_inhib_units <= 20) {
	ilay->un_geom.x = 2; ilay->un_geom.y = n_inhib_units / 2;
	while(ilay->un_geom.x * ilay->un_geom.y < n_inhib_units) ilay->un_geom.y++;
      }
      else if(n_inhib_units <= 40) {
	ilay->un_geom.x = 4; ilay->un_geom.y = n_inhib_units / 4;
	while(ilay->un_geom.x * ilay->un_geom.y < n_inhib_units) ilay->un_geom.y++;
      }
    }
    ilay->SetLayerSpec(inhib_ls);
    ilay->SetUnitSpec(inhib_us);

    int j;
    for(j=0;j<lay->projections.size;j++) {
      LeabraLayer* fmlay = (LeabraLayer*)((Projection*)lay->projections[j])->from.ptr();
      if(fmlay->name.contains("_Inhib")) continue;
      if(fmlay == lay) continue;
      net->FindMakePrjn(ilay, fmlay, fullprjn, ff_inhib_cs);
    }
    net->FindMakePrjn(ilay, lay, fullprjn, fb_inhib_cs);
    net->FindMakePrjn(lay, ilay, fullprjn, inhib_cs);
    net->FindMakePrjn(ilay, ilay, fullprjn, inhib_cs);
  }

  net->UpdateAfterEdit();
  // todo:
//   net->InitAllViews();

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);

  // set settle cycles to 300
  net->cycle_max = 300;
  net->min_cycles = 150;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("UnitInhib");
  if(edit != NULL) {
    basic_us->SelectForEditNm("dt", edit, "excite");
    inhib_us->SelectForEditNm("dt", edit, "inhib");
    basic_us->SelectForEditNm("g_bar", edit, "excite");
    ff_inhib_cs->SelectForEditNm("wt_scale", edit, "ff_inhib");
    fb_inhib_cs->SelectForEditNm("rnd", edit, "to_inhib");
    inhib_cs->SelectForEditNm("rnd", edit, "fm_inhib");
    basic_ls->SelectForEditNm("adapt_i", edit, "layers");

    net->SelectForEditNm("cycle_max", edit, net->name);
    net->SelectForEditNm("min_cycles", edit, net->name);
  }
  return true;
}

bool LeabraWizard::StdProgs() {
  // todo: could check for subgroups and do LeabraAll_GpData instead
  //Program_Group* pg = 
  if(!StdProgs_impl("LeabraAll_Std")) return false;
  // todo: could do something more here..
  return true;
}

bool LeabraWizard::TestProgs(Program* call_test_from, bool call_in_loop, int call_modulus) {
  if(!TestProgs_impl("LeabraAll_Test", call_test_from, call_in_loop, call_modulus))
    return false;
  return true;
}


//////////////////////////////////
//  LeabraEngineInst		//
//////////////////////////////////

void LeabraEngineInst::Initialize() {
  n_tasks = 1;
}

void LeabraEngineInst::Destroy() {
}

void LeabraEngineInst::AssertScratchDims(int tasks, int units) {
  // set units mod4 so we can do SSE rollup efficiently
  units = (units + 3) & (~4);
  excit.SetGeom(2, units, tasks);
  inhib.SetGeom(2, units, tasks);
}

void LeabraEngineInst::RollupWritebackScratch_Netin() {
  const int size = unitSize(); 
  const int stride = excit.dim(0); // same for both
  // arrays, as constant addr's
  float* ea = (float*)excit.data();
  float* ia = (float*)inhib.data();
  float ex;
  float in; 
  for (int i = 0; i < size; ++i) {
    ex = ea[i];
    in = ia[i];
    for (int t = 1; t < n_tasks; ++t) {
      const int ut = (stride*t) + i;
      ex += ea[ut]; 
      in += ia[ut]; 
    }
    LeabraUnit* un = (LeabraUnit*)units[i];
    un->net = ex;
    un->gc.i = in;
  }
}


void LeabraEngineInst::RollupWritebackScratch_NetinDelta() {
  const int size = unitSize(); 
  const int stride = excit.dim(0); // same for both
  // arrays, as constant addr's
  float* ea = (float*)excit.data();
  float* ia = (float*)inhib.data();
  float ex;
  float in; 
  for (int i = 0; i < size; ++i) {
    ex = ea[i];
    in = ia[i];
    for (int t = 1; t < n_tasks; ++t) {
      const int ut = (stride*t) + i;
      ex += ea[ut]; 
      in += ia[ut]; 
    }
    LeabraUnit* un = (LeabraUnit*)units[i];
    un->net = 0.0f;	// important for soft-clamped layers
    un->gc.i = 0.0f;
    un->net_delta = ex;
    un->g_i_delta = in;
  }
}


//////////////////////////////////
//  LeabraEngine		//
//////////////////////////////////

void LeabraEngine::Initialize() {
}

void LeabraEngine::Destroy() {
}

taEngineInst* LeabraEngine::NewEngineInst_impl() const {
  return new LeabraEngineInst;
}


//////////////////////////////////
//  LeabraTask			//
//////////////////////////////////

void LeabraTask::Initialize() {
}

void LeabraTask::Destroy() {
}

void LeabraTask::run() {
  switch (proc_id) {
  case P_Compute_Act: DoCompute_Act(); break;
  case P_Compute_dWt: DoCompute_dWt(); break;
  case P_Compute_Weights: DoCompute_Weights(); break;
  case P_Send_Netin: DoSend_Netin(); break;
  case P_Send_NetinDelta_flat: DoSend_NetinDelta_flat(); break;
  case P_Send_NetinDelta_list: DoSend_NetinDelta_list(); break;
  default: inherited::run(); break;
  }
}

#ifdef TA_USE_THREADS

//////////////////////////////////
//  LeabraThreadEngine		//
//////////////////////////////////

void LeabraThreadEngine::Initialize() {
  n_threads = taMisc::cpus;
  nibble = true;
}

void LeabraThreadEngine::Destroy() {
}

taEngineInst* LeabraThreadEngine::NewEngineInst_impl() const {
  return new LeabraThreadEngineInst;
}


//////////////////////////////////
//  LeabraThreadEngineInst	//
//////////////////////////////////

void LeabraThreadEngineInst::Initialize() {
  tasks.SetBaseType(&TA_LeabraThreadEngineTask);
  memset(threads, 0, sizeof(threads));
  nibble = true;
  n_items = 0;
#ifdef DEBUG
  n_items_done = 0;
#endif
  col_n_units = NULL;
  col_n_tasks = NULL;
  col_tm_send_units = NULL;
  col_tm_make_threads = NULL;
  col_tm_release = NULL;
  col_tm_run = NULL;
  col_tm_nibble = NULL;
  col_tm_sync = NULL;

}

void LeabraThreadEngineInst::Destroy() {
  setTaskCount(0);
}

void LeabraThreadEngineInst::DoUnitProc(int proc_id) {
  if (use_log) tm_make_threads.StartTimer(); // reset; stopped in DoUnitProc
  // number of tasks will either be =threads, or less, if fewer units
  n_tasks = ((n_items + (UNIT_CHUNK_SIZE - 1)) & (~UNIT_CHUNK_SIZE)) / UNIT_CHUNK_SIZE;
  if (n_tasks < 1) n_tasks = 1;
  if (n_tasks > taskCount()) n_tasks = taskCount();
  //NOTE: don't bail even if items==0 because task(0) is used to init net values
  
#ifdef DEBUG
  n_items_done = 0;
#endif
  // start all the other threads first...
  // have to suspend then resume in case not finished from last time
  // init tasks and start all >0 tasks
  LeabraThreadEngineTask* tsk = NULL;
  taTaskThread* tt = NULL;
  for (int t = 0; t < n_tasks; ++t) {
    // note: task0 done in main thread, so no thread for it
    //NOTE: threads must have been idle or previously sync'ed!!!
    tsk = task(t);
    tsk->g_i = t * UNIT_CHUNK_SIZE;
    tsk->init_done = false;
    tsk->proc_id = proc_id;
    if (t > 0) {
      tt = threads[t];
      tt->release();
    }
  }
  if (use_log) tm_make_threads.EndTimer();
  
  // then do my part
  if (use_log) tm_run0.StartTimer(); 
  tsk = task(0);
  tsk->run();
  if (use_log) tm_run0.EndTimer(); 
  
  // then lend a "helping hand" (if enabled)
  if (nibble) {
    if (use_log) tm_nibble.StartTimer(); 
    for (int t = 1; t < n_tasks; ++t) {
      tsk = task(t);
      // note: its ok if tsk finishes between our test and calling run
      if ((tsk->g_i < n_items) && (tsk->init_done))
        tsk->run();
    }
    if (use_log) tm_nibble.EndTimer(); 
  }
  // always need to sync regardless of nibbling, since thread can be finishing
  // its chunks
  if (use_log) tm_sync.StartTimer(); 
  for (int t = 1; t < n_tasks; ++t) {
    tt = threads[t];
    tt->sync(); // suspending is syncing with completion of loop
  }
  if (use_log) tm_sync.EndTimer(); 
#ifdef DEBUG
  if (n_items != n_items_done)
    taMisc::Error("LeabraThreadEngineInst::DoUnitProc: n_items != n_items_done (exp/done: ",
    String(n_items), "/", String(n_items_done));
#endif
  if (use_log) tm_roll_write.StartTimer(); 
}


void LeabraThreadEngineInst::DoLayerProc(int proc_id) {
//  if (use_log) tm_make_threads.StartTimer(); // reset; stopped in DoUnitProc
  // number of tasks will either be =threads, or less, if fewer layers
  n_items = layerSize();
  n_tasks = n_items;
  if (n_tasks > taskCount()) n_tasks = taskCount();
  if (n_tasks < 1) return;
#ifdef DEBUG
  n_items_done = 0;
#endif
  // start all the other threads first...
  // have to suspend then resume in case not finished from last time
  // init tasks and start all >0 tasks
  LeabraThreadEngineTask* tsk = NULL;
  taTaskThread* tt = NULL;
  LeabraThreadEngineTask::g_l = 0;
  for (int t = 0; t < n_tasks; ++t) {
    // note: task0 done in main thread, so no thread for it
    //NOTE: threads must have been idle or previously sync'ed!!!
    tsk = task(t);
    tsk->init_done = false;
    tsk->proc_id = proc_id;
    if (t > 0) {
      tt = threads[t];
      tt->release();
    }
  }
//  if (use_log) tm_make_threads.EndTimer();
  
  // then do my part -- nibbling is intrinsic
//  if (use_log) tm_run0.StartTimer(); 
  tsk = task(0);
  tsk->run();
  // always need to sync regardless of nibbling, since thread can be finishing
  // its chunks
//  if (use_log) tm_sync.StartTimer(); 
  for (int t = 1; t < n_tasks; ++t) {
    tt = threads[t];
    tt->sync(); // suspending is syncing with completion of loop
  }
//  if (use_log) tm_sync.EndTimer(); 
#ifdef DEBUG
  if (n_items != n_items_done)
    taMisc::Error("LeabraThreadEngineInst::DoUnitProc: n_items != n_items_done (exp/done: ",
    String(n_items), "/", String(n_items_done));
#endif
  if (use_log) tm_roll_write.StartTimer(); 
}

void LeabraThreadEngineInst::OnBuild_impl() {
  inherited::OnBuild_impl();
  // alloc enough space for all units in send, because
  // we use the list like a fully allocated array, to avoid
  // unnecessary writes in non-delta (since most cycles will have same sends) 
  send_units.Alloc(unitSize());
  LeabraThreadEngine* engine = this->engine();
  setTaskCount(engine->n_threads);
  nibble = engine->nibble;
  // assert scratchpad guys (note: task threads do the clear)
  AssertScratchDims(taskCount(), unitSize());
  // setup tasks
  int scr_units = excit.dim(0); // exact mod4 value
  for (int i = 0; i < taskCount(); ++i) {
    LeabraThreadEngineTask* tsk = task(i);
    int fm = scr_units * i;
    tsk->scr_units = scr_units;
    tsk->excit = (float*)excit.FastEl_Flat_(fm);
    tsk->inhib = (float*)inhib.FastEl_Flat_(fm);
  }
  // assert data cols if logging enabled
  if (!(use_log && (bool)log_table)) return;
  // note: for thread guys, we just alloc #cpus, even if not all used
  col_n_units = log_table->FindMakeCol("n_units", VT_INT);
  col_n_units->desc = "number of units in this cycle";
  col_n_tasks = log_table->FindMakeCol("n_tasks", VT_INT);
  col_n_tasks->desc = "number of tasks used in this cycle";
  col_tm_tot = log_table->FindMakeCol("tm_tot", VT_DOUBLE);
  col_tm_tot->desc = "time spent (us/un) for entire process";
  col_tm_send_units = log_table->FindMakeCol("tm_send_units", VT_DOUBLE);
  col_tm_send_units->desc = "time spent (us/un) in t0 to build the send_units list";
  col_tm_make_threads = log_table->FindMakeCol("tm_make_threads", VT_DOUBLE);
  col_tm_make_threads->desc = "time spent (us/un) in t0 making the other threads, and starting them";
  col_tm_release = log_table->FindMakeColMatrix("tm_release", VT_DOUBLE, 1, taMisc::cpus);
  col_tm_release->desc = "time spent (us/un) in t1-tN between release() call and when it runs";
  col_tm_run = log_table->FindMakeColMatrix("tm_run", VT_DOUBLE, 1, taMisc::cpus);
  col_tm_run->desc = "time spent (us/un) in each thread running its own data";
  col_tm_nibble = log_table->FindMakeCol("tm_nibble", VT_DOUBLE);
  col_tm_nibble->desc = "time spent (us/un) in t0 nibbling other threads";
  col_tm_sync = log_table->FindMakeCol("tm_sync", VT_DOUBLE);
  col_tm_sync->desc = "time spent (us/un) in t0 syncing to other threads";
  col_tm_roll_write = log_table->FindMakeCol("tm_roll_write", VT_DOUBLE);
  col_tm_roll_write->desc = "time spent (us/un) in t0 rolling up excit/inhib and writing back to net";
}

bool LeabraThreadEngineInst::OnCompute_Act() {
  DoLayerProc(LeabraTask::P_Compute_Act);
  return true;
}

bool LeabraThreadEngineInst::OnCompute_dWt() {
  DoLayerProc(LeabraTask::P_Compute_dWt);
  return true;
}

bool LeabraThreadEngineInst::OnCompute_Weights() {
  DoLayerProc(LeabraTask::P_Compute_Weights);
  return true;
}

bool LeabraThreadEngineInst::OnSend_Netin() {
  if (use_log) {
    tm_tot.StartTimer();
    tm_send_units.StartTimer(); // reset; stopped in DoUnitProc
  }
  LeabraNetwork* net = this->net();
  // build list of send units -- we treat the list like an array
  // which is kinda sleazy, but it works fine (because we alloc'ed for all on build)
  int cnt = 0;
//  send_units.Reset();
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    LeabraUnit* un;
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec.ptr();
    taLeafItr ui;
    FOR_ITR_EL(LeabraUnit, un, lay->units., ui) {
      if (!(lay->lesioned() || lay->hard_clamped)) {
        net->send_pct_tot++;
	float act_ts = un->act;
	if(us->syn_delay.on) {
	  act_ts = un->act_buf.CircSafeEl(0); // get first logical element..
	}
        if (act_ts > us->opt_thresh.send) {
          net->send_pct_n++;
          if (send_units.el[cnt] != un)
            send_units.el[cnt] = un;
          ++cnt;
        }
      }
    }
  }
  send_units.size = cnt;
  if (use_log) tm_send_units.EndTimer(); // 
  //note: could very well be 0 send units...
  
  // dispatch the tasks
  n_items = cnt;
  DoUnitProc(LeabraTask::P_Send_Netin);
  
  // rollup the scratch and write back
  RollupWritebackScratch_Netin();
  if (use_log) {
    tm_roll_write.EndTimer(); 
    tm_tot.EndTimer();
  }
  WriteLogRecord();

  return true;
}

bool LeabraThreadEngineInst::OnSend_NetinDelta() {
  return OnSend_NetinDelta_list();
}

bool LeabraThreadEngineInst::OnSend_NetinDelta_flat() {
  if (use_log) {
    tm_tot.StartTimer();
  }
  // dispatch the tasks
  n_items = unitSize();
  DoUnitProc(LeabraTask::P_Send_NetinDelta_flat);
  
  // rollup the scratch and write back
  RollupWritebackScratch_NetinDelta();
  if (use_log) {
    tm_roll_write.EndTimer(); 
    tm_tot.EndTimer();
  }
  WriteLogRecord();
  return true;
}

bool LeabraThreadEngineInst::OnSend_NetinDelta_list() {
  if (use_log) {
    tm_tot.StartTimer();
    tm_send_units.StartTimer(); // reset; stopped in DoUnitProc
  }
  LeabraNetwork* net = this->net();
  // we just build the unit list the conventional way, because with delta,
  // the list of active units will change a lot, so no real benefit to 
  // comparing to previous to avoid writes
  send_units.Reset();
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    if (!(lay->lesioned() || lay->hard_clamped)) {
      LeabraUnit* u;
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec.ptr();
      taLeafItr ui;
      FOR_ITR_EL(LeabraUnit, u, lay->units., ui) {
        net->send_pct_tot++;
	float act_ts = u->act;
	if(us->syn_delay.on) {
	  act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
	}
        if (act_ts > us->opt_thresh.send) {
	  float act_delta = act_ts - u->act_sent;
          if (fabsf(act_delta) > us->opt_thresh.delta) {
            net->send_pct_n++;
            send_units.Add(u);
          }
        } 
        else if (u->act_sent > us->opt_thresh.send) {
          net->send_pct_n++;
          send_units.Add(u);
        }
      }
    }
  }
  if (use_log) tm_send_units.EndTimer(); // 
  //note: could very well be 0 send units...
  
  // dispatch the tasks
  n_items = send_units.size;
  DoUnitProc(LeabraTask::P_Send_NetinDelta_list);
  
  // rollup the scratch and write back
  RollupWritebackScratch_NetinDelta();
  if (use_log) {
    tm_roll_write.EndTimer(); 
    tm_tot.EndTimer();
  }
  WriteLogRecord();
  return true;
}

void LeabraThreadEngineInst::setTaskCount(int val) {
  if (val < 0) val = 0;
  if (val > MAX_THREADS) val = MAX_THREADS;
  int old_cnt = taskCount();
  inherited::setTaskCount(val);
  if (old_cnt == val) return;
  if (old_cnt > val) {
    // remove excess
    for (int i = old_cnt - 1; ((i > 0 && (i >= val))); --i) {
      taTaskThread* tt = threads[i];
      taTaskThread::DeleteTaskThread(tt);
      threads[i] = NULL;
    }
  } else { // need new
    //NOTE: we don't create a thread for [0] because that is done in main thread
    // if size is different, then adjust as appropriate (only one of these may run:)
    // added needed new
    for (int i = old_cnt; i < val; ++i) {
      if (i > 0) {
        taTaskThread* tt = new taTaskThread(use_log, i);
        threads[i] = tt;
        tt->setTask(task(i));
        tt->start(); // starts paused
      }
    }
  }
}

void LeabraThreadEngineInst::WriteLogRecord_impl() {
  int t = 0;
  // normalize all results to be us/unit **total units in net**
  const double fact = 1000000.0 / (double) ((unitSize()) ? unitSize() : 1.0);
  col_n_units->SetValAsInt(n_items, -1);
  col_n_tasks->SetValAsInt(n_tasks, -1);
  col_tm_tot->SetValAsDouble(tm_tot.s_used*fact, -1);
  col_tm_send_units->SetValAsDouble(tm_send_units.s_used*fact, -1);
  col_tm_make_threads->SetValAsDouble(tm_make_threads.s_used*fact, -1);
  for (t = 1; t < n_tasks; ++t) {
    taTaskThread* tt = threads[t];
    col_tm_release->SetValAsDoubleM(tt->start_latency.s_used*fact, -1, t);
  }
  col_tm_run->SetValAsDoubleM(tm_run0.s_used*fact, -1, 0);
  for (t = 1; t < n_tasks; ++t) {
    taTaskThread* tt = threads[t];
    col_tm_run->SetValAsDoubleM(tt->run_time.s_used*fact, -1, t);
  }
  col_tm_nibble->SetValAsDouble(tm_nibble.s_used*fact, -1);
  col_tm_sync->SetValAsDouble(tm_sync.s_used*fact, -1);
  col_tm_roll_write->SetValAsDouble(tm_roll_write.s_used*fact, -1);
}

//////////////////////////////////
//  LeabraThreadEngineTask	//
//////////////////////////////////

int LeabraThreadEngineTask::g_l;

void LeabraThreadEngineTask::Initialize() {
  g_i = 0;
  init_done = false;
}

void LeabraThreadEngineTask::DoCompute_Act() {
  LeabraThreadEngineInst* inst = this->inst();
  const int n_items = inst->n_items;
  
  //NOTE: inherently greedy algorithm    
  int m_l = AtomicFetchAdd(&g_l, 1);
  while (m_l < n_items) {
    LeabraLayer* lay = inst->layer(m_l);
    if (!lay->lesioned()) {
      lay->Compute_Act();
    }
    m_l = AtomicFetchAdd(&g_l, 1);
#ifdef DEBUG
    AtomicFetchAdd(&(inst->n_items_done), 1);
#endif
  }
}

void LeabraThreadEngineTask::DoCompute_dWt() {
  LeabraThreadEngineInst* inst = this->inst();
  const int n_items = inst->n_items;
  //NOTE: inherently greedy algorithm    
  int m_l = AtomicFetchAdd(&g_l, 1);
  while (m_l < n_items) {
    LeabraLayer* lay = inst->layer(m_l);
    if (!lay->lesioned()) {
      lay->Compute_dWt();
    }
    m_l = AtomicFetchAdd(&g_l, 1);
#ifdef DEBUG
    AtomicFetchAdd(&(inst->n_items_done), 1);
#endif
  }
}

void LeabraThreadEngineTask::DoCompute_Weights() {
  LeabraThreadEngineInst* inst = this->inst();
  const int n_items = inst->n_items;
  //NOTE: inherently greedy algorithm    
  int m_l = AtomicFetchAdd(&g_l, 1);
  while (m_l < n_items) {
    LeabraLayer* lay = inst->layer(m_l);
    if (!lay->lesioned()) {
      lay->Compute_Weights();
    }
    m_l = AtomicFetchAdd(&g_l, 1);
#ifdef DEBUG
    AtomicFetchAdd(&(inst->n_items_done), 1);
#endif
  }
}

void LeabraThreadEngineTask::DoSend_Netin_Gp(bool is_excit, 
  SendCons* cg, float su_act)
{
  // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
  // LeabraUnitSpec::CheckConfig checks that this is ok.
  Unit* ru = cg->Un(0);
  float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
  if (is_excit) {
    CON_GROUP_LOOP(cg, 
      StatC_Send_Excit(
        (LeabraCon*)cg->Cn(i), 
        &excit[((LeabraUnit*)cg->Un(i))->flat_idx],
        su_act_eff)
      );
  } else {
    CON_GROUP_LOOP(cg, 
      StatC_Send_Inhib(
        (LeabraCon*)cg->Cn(i), 
        &inhib[((LeabraUnit*)cg->Un(i))->flat_idx],
        su_act_eff)
      );
  }
}


void LeabraThreadEngineTask::InitScratch_Send_Netin() {
  LeabraThreadEngineInst* inst = this->inst();
  // do the inhib first, since likely not used, so cache excit
  memset(inhib, 0, sizeof(float) * scr_units);
  if (task_id == 0) {
    //NOTE: only need to init into task[0][]
    const int unit_size = inst->unitSize();
    for (int i = 0; i < unit_size; ++i) {
      excit[i] = inst->unit(i)->clmp_net;
    }
  } else {
    memset(excit, 0, sizeof(float) * scr_units);
  }
}

void LeabraThreadEngineTask::DoSend_Netin() {
  LeabraThreadEngineInst* inst = this->inst();
  // local copies of constants
  const int n_items = inst->n_items;
  // span is the chunk size, times the number of threads working in parallel
  const int span_size = LeabraThreadEngineInst::UNIT_CHUNK_SIZE * inst->n_tasks;
  // guard init, because [0] could nibble us!
  if (!init_done) {
    InitScratch_Send_Netin();
    init_done = true; // so [0] can nibble us
  }
//HEREAFTER could get "nibbled" -- run by [0]
    
  // we work in chunks... so make a local guy
  int m_u = AtomicFetchAdd(&g_i, span_size);
  int chnki = 0; // count units within the chunk
  while (m_u < n_items) {
    LeabraUnit* u = (LeabraUnit*)inst->send_units[m_u];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    float act_ts = u->act;
    if(us->syn_delay.on) {
      act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
    }
    
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size) continue;
      bool is_excit = !((LeabraConSpec*)send_gp->GetConSpec())->inhib;
      DoSend_Netin_Gp(is_excit,send_gp,act_ts);
    }
    
    if (++chnki < LeabraThreadEngineInst::UNIT_CHUNK_SIZE) {
      ++m_u;
    } else {
      chnki = 0;
      m_u = AtomicFetchAdd(&g_i, span_size);
    }
#ifdef DEBUG
    AtomicFetchAdd(&(inst->n_items_done), 1);
#endif
  }
   //donzo! that's it
}

void LeabraThreadEngineTask::DoSend_NetinDelta_Gp(bool is_excit,
  SendCons* cg, float su_act_delta) 
{
  // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
  // LeabraUnitSpec::CheckConfig checks that this is ok.
  Unit* ru = cg->Un(0);
  float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act_delta;
  if (is_excit) {
    CON_GROUP_LOOP(cg, 
      StatC_Send_Excit(
        (LeabraCon*)cg->Cn(i), 
        &excit[((LeabraUnit*)cg->Un(i))->flat_idx],
        su_act_delta_eff)
      );
  } else {
    CON_GROUP_LOOP(cg, 
      StatC_Send_Inhib(
        (LeabraCon*)cg->Cn(i), 
        &inhib[((LeabraUnit*)cg->Un(i))->flat_idx],
        su_act_delta_eff)
      );
  }
}


void LeabraThreadEngineTask::InitScratch_Send_NetinDelta() {
//  LeabraThreadEngineInst* inst = this->inst();
  // do the inhib first, since likely not used, so cache excit
  memset(inhib, 0, sizeof(float) * scr_units);
  memset(excit, 0, sizeof(float) * scr_units);
}

void LeabraThreadEngineTask::DoSend_NetinDelta_flat() {
  LeabraThreadEngineInst* inst = this->inst();
  LeabraNetwork* net = inst->net();
  // local copies of constants
  const int n_items = inst->n_items;
  // span is the chunk size, times the number of threads working in parallel
  const int span_size = LeabraThreadEngineInst::UNIT_CHUNK_SIZE * inst->n_tasks;
  // guard init, because [0] could nibble us!
  if (!init_done) {
    InitScratch_Send_NetinDelta();
    init_done = true; // so [0] can nibble us
  }
//HEREAFTER could get "nibbled" -- run by [0]
    
  // we work in chunks... so must make a local guy
  int m_u = AtomicFetchAdd(&g_i, span_size);
  int chnki = 0; // count units within the chunk
  while (m_u < n_items) {
    LeabraUnit* u = (LeabraUnit*)inst->units[m_u];
    LeabraLayer* lay = u->own_lay();
    if (!(lay->hard_clamped || lay->lesioned())) {
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      AtomicFetchAdd(&(net->send_pct_tot),1);
      float act_ts = u->act;
      if(us->syn_delay.on) {
	act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
      }
      if (act_ts > us->opt_thresh.send) {
	float act_delta = act_ts - u->act_sent;
        if (fabsf(act_delta) > us->opt_thresh.delta) {
          AtomicFetchAdd(&(net->send_pct_n),1);
          // case: above threshold, and delta above delta threshold
          for(int g=0; g<u->send.size; g++) {
            LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
            LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
            if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size) continue;
            bool is_excit = !((LeabraConSpec*)send_gp->GetConSpec())->inhib;
            DoSend_NetinDelta_Gp(is_excit, send_gp, act_delta);
          }
          u->act_sent = act_ts;	// cache the last sent value
        }
      }
      else if (u->act_sent > us->opt_thresh.send) {
        AtomicFetchAdd(&(net->send_pct_n),1);
        // case: below threshold, and haven't unsent yet
        float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
        for(int g=0; g<u->send.size; g++) {
          LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
          LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
          if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size)	continue;
          bool is_excit = !((LeabraConSpec*)send_gp->GetConSpec())->inhib;
          DoSend_NetinDelta_Gp(is_excit, send_gp, act_delta);
        }
        u->act_sent = 0.0f;		// now it effectively sent a 0..
      }
    }
    if (++chnki < LeabraThreadEngineInst::UNIT_CHUNK_SIZE) {
      ++m_u;
    } else {
      chnki = 0;
      m_u = AtomicFetchAdd(&g_i, span_size);
    }
#ifdef DEBUG
    AtomicFetchAdd(&(inst->n_items_done), 1);
#endif
  }
   //donzo! that's it
}

void LeabraThreadEngineTask::DoSend_NetinDelta_list() {
  LeabraThreadEngineInst* inst = this->inst();
  // local copies of constants
  const int n_items = inst->n_items;
  // span is the chunk size, times the number of threads working in parallel
  const int span_size = LeabraThreadEngineInst::UNIT_CHUNK_SIZE * inst->n_tasks;
  // guard init, because [0] could nibble us!
  if (!init_done) {
    InitScratch_Send_NetinDelta();
    init_done = true; // so [0] can nibble us
  }
//HEREAFTER could get "nibbled" -- run by [0]
    
  // we work in chunks... so must make a local guy
  int m_u = AtomicFetchAdd(&g_i, span_size);
  int chnki = 0; // count units within the chunk
  while (m_u < n_items) {
    LeabraUnit* u = (LeabraUnit*)inst->send_units[m_u];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    float act_ts = u->act;
    if(us->syn_delay.on) {
      act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
    }

    // note: because we did these tests for putting units on the send list
    // we can then use the first part of first condition to distinguish the two cases
    if (act_ts > us->opt_thresh.send) {
      float act_delta = act_ts - u->act_sent;
      // case: above threshold, and delta above delta threshold
      for(int g=0; g<u->send.size; g++) {
        LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size) continue;
        bool is_excit = !((LeabraConSpec*)send_gp->GetConSpec())->inhib;
        DoSend_NetinDelta_Gp(is_excit, send_gp, act_delta);
      }
      u->act_sent = act_ts;	// cache the last sent value
    }
    else {
      // case: below threshold, and haven't unsent yet
      float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
      for(int g=0; g<u->send.size; g++) {
        LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size)	continue;
        bool is_excit = !((LeabraConSpec*)send_gp->GetConSpec())->inhib;
        DoSend_NetinDelta_Gp(is_excit, send_gp, act_delta);
      }
      u->act_sent = 0.0f;		// now it effectively sent a 0..
    }
    
    if (++chnki < LeabraThreadEngineInst::UNIT_CHUNK_SIZE) {
      ++m_u;
    } else {
      chnki = 0;
      m_u = AtomicFetchAdd(&g_i, span_size);
    }
#ifdef DEBUG
    AtomicFetchAdd(&(inst->n_items_done), 1);
#endif
  }
   //donzo! that's it
}

#endif // TA_USE_THREADS
