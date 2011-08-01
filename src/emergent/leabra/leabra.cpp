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
#include "ta_dataanal.h"

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
  old = false;
  rel = 1.0f;
  abs = 1.0f;
  sem_extra = 2;
}

void WtScaleSpec::Defaults_init() {
}


float WtScaleSpec::SLayActScale(float savg, float lay_sz, float n_cons) {
  int slay_act_n = (int)(savg * lay_sz + .5f); // sending layer actual # active
  slay_act_n = MAX(slay_act_n, 1);
  float rval = 1.0f;
  if(n_cons == lay_sz) {
    rval = 1.0f / (float)slay_act_n;
  }
  else {
    int r_max_act_n = MIN(n_cons, slay_act_n); // max number we could get
    int r_avg_act_n = (int)(savg * n_cons + .5f);// recv average actual # active if uniform
    r_avg_act_n = MAX(r_avg_act_n, 1);
    int r_exp_act_n = r_avg_act_n + sem_extra;
    r_exp_act_n = MIN(r_exp_act_n, r_max_act_n);
    rval = 1.0f / (float)r_exp_act_n;
  }
  return rval;
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
  thr_l_mix = 0.01f;
  s_mix = 0.9f;
  d_rev = 0.10f;
  d_thr = 0.0001f;
  m_mix = 1.0f - s_mix;
  thr_m_mix = 1.0f - thr_l_mix;
  d_rev_ratio = -(1.0f - d_rev) / d_rev;
}

void XCalLearnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  m_mix = 1.0f - s_mix;
  thr_m_mix = 1.0f - thr_l_mix;
  d_rev_ratio = (1.0f - d_rev) / d_rev;
  if(d_rev > 0.0f)
    d_rev_ratio = -(1.0f - d_rev) / d_rev;
  else
    d_rev_ratio = -1.0f;
  if(owner) owner->UpdateAfterEdit(); // update our conspec so it can recompute lookup function!
}

void SAvgCorSpec::Initialize() {
  cor = .4f;
  thresh = .001f;
  norm_con_n = true;
}

void AdaptRelNetinSpec::Initialize() {
  on = false;
  Defaults_init();
}

void AdaptRelNetinSpec::Defaults_init() {
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
  learn_rule = LEABRA_CHL;
  inhib = false;

  wt_sig_fun.x_range.min = 0.0f;
  wt_sig_fun.x_range.max = 1.0f;
  wt_sig_fun.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun.UpdateAfterEdit_NoGui();

  wt_sig_fun_inv.x_range.min = 0.0f;
  wt_sig_fun_inv.x_range.max = 1.0f;
  wt_sig_fun_inv.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun_inv.UpdateAfterEdit_NoGui();

  wt_sig_fun_lst.off = -1.0f;   wt_sig_fun_lst.gain = -1.0f; // trigger an update
  wt_sig_fun_res = -1.0f;

  lrs_value = EPOCH;
  lrate_sched.interpolate = false;

  Defaults_init();
}

void LeabraConSpec::Defaults_init() {
  wt_limits.min = 0.0f;
  wt_limits.max = 1.0f;
  wt_limits.sym = true;
  wt_limits.type = WeightLimits::MIN_MAX;

  rnd.mean = .5f;
  rnd.var = .25f;
  lrate = .01f;
  cur_lrate = .01f;
}

void LeabraConSpec::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LeabraConSpec);
  children.SetBaseType(&TA_LeabraConSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  CreateWtSigFun();
  if(taMisc::is_loading || taMisc::is_duplicating) return;
  LeabraNetwork* mynet = GET_MY_OWNER(LeabraNetwork);
  if(mynet) {
    SetLearnRule(mynet);		// get current learning rule.
  }
}

void LeabraConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lrate_sched.UpdateAfterEdit_NoGui();
  lmix.UpdateAfterEdit_NoGui();
  CreateWtSigFun();
}

bool LeabraConSpec::CheckConfig_RecvCons(RecvCons* cg, bool quiet) {
  bool rval = true;
  if(learn_rule == CTLEABRA_CAL) {
    if(cg->prjn) {
      if(cg->prjn->CheckError(!cg->prjn->con_type->InheritsFrom(&TA_LeabraSRAvgCon), quiet, rval,
			      "does not have con_type = LeabraSRAvgCon -- required for CTLEABRA_CAL learning to hold the sravg connection-level values -- I just fixed this for you in this projection, but must do Build to get it to take effect")) {
	cg->prjn->con_type = &TA_LeabraSRAvgCon;
      }
    }
  }
  return rval;
}

void LeabraConSpec::SetLearnRule(LeabraNetwork* net) {
  if((int)net->learn_rule == (int)learn_rule) return;
  learn_rule = (LeabraConSpec::LearnRule)net->learn_rule;
  // todo: could set come conflicting params..
  if(learn_rule != LEABRA_CHL) {
    if(wt_sig.off == 1.25f)
      wt_sig.off = 1.0f;	// this is key
    if(GetTypeDef() == &TA_LeabraConSpec) { // only for generic conspecs -- not derived ones!
      if(lrate == 0.01f)
	lrate = 0.02f;		// also important
    }
  }
  else {
    if(wt_sig.off == 1.0f)
      wt_sig.off = 1.25f;
    if(GetTypeDef() == &TA_LeabraConSpec) { // only for generic conspecs -- not derived ones!
      if(lrate == 0.02f)
	lrate = 0.01f;		// also important
    }
  }
  UpdateAfterEdit();		// pick up flags
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

void LeabraConSpec::LogLrateSched(int epcs_per_step, float n_steps) {
  float log_ns[3] = {1, .5f, .2f};
  
  lrate_sched.SetSize(n_steps);
  for(int i=0;i<n_steps;i++) {
    lrate_sched[i]->start_ctr = i * epcs_per_step;
    lrate_sched[i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
  }
  UpdateAfterEdit();		// needed to update the sub guys
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

void LeabraConSpec::WtScaleCvt(float savg, int lay_sz, int n_cons,
			       bool norm_con_n) {
  int slay_act_n = (int)(savg * lay_sz + .5f); // sending layer actual # active
  slay_act_n = MAX(slay_act_n, 1);
  int r_avg_act_n = (int)(savg * n_cons + .5f);// recv average actual # active if uniform
  r_avg_act_n = MAX(r_avg_act_n, 1);
  float old_val;
  if(norm_con_n)
    old_val = 1.0f / (float)(savg * n_cons);
  else
    old_val = 1.0f / (float)slay_act_n;
  float new_val = wt_scale.SLayActScale(savg, lay_sz, n_cons);
  float new_old_rat = new_val / old_val;
  float old_new_rat = old_val / new_val;
  taMisc::Info("old_scale:", String(old_val), "new_scale:", String(new_val),
	       "new / old:", String(new_old_rat), "old / new:", String(old_new_rat),
	       String("cur wt_scale.abs: ") + String(wt_scale.abs) + String(" new abs to remain same: ") +
	       String(old_new_rat * wt_scale.abs));
  // new = new_abs.* new_sc
  // old = old_abs * old_sc
  // new_abs * new_sc = old_abs * old_sc
  // new_abs = old_abs * (old_sc / new_sc)
}

/////////////////////////////////////////////////////
//	LeabraRecvCons

void LeabraRecvCons::Initialize() {
  ClearBaseFlag(OWN_CONS);	// recv does NOT own!
  SetConType(&TA_LeabraCon);
  scale_eff = 0.0f;
  net = 0.0;
}

void LeabraRecvCons::Copy_(const LeabraRecvCons& cp) {
  scale_eff = cp.scale_eff;
  net = cp.net;
}

void LeabraSendCons::Initialize() {
  SetBaseFlag(OWN_CONS);	// send DOES own!
  SetConType(&TA_LeabraCon);
  scale_eff = 0.0f;
  savg_cor = 1.0f;
}

void LeabraSendCons::Copy_(const LeabraSendCons& cp) {
  scale_eff = cp.scale_eff;
  savg_cor = cp.savg_cor;
}

void LeabraBiasSpec::Initialize() {
  //   min_obj_type = &TA_RecvCons; // don't bother..
  SetUnique("rnd", true);
  SetUnique("wt_limits", true);
  SetUnique("wt_scale", true);
  SetUnique("wt_scale_init", true);

  Defaults_init();
}

void LeabraBiasSpec::Defaults_init() {
  rnd.mean = 0.0f;
  rnd.var = 0.0f;
  wt_limits.min = -1.0f;
  wt_limits.max = 5.0f;
  wt_limits.sym = false;
  wt_limits.type = WeightLimits::NONE;
  dwt_thresh = .1f;
}

bool LeabraBiasSpec::CheckObjectType_impl(taBase* obj) {
  // don't allow anything to point to a biasspec except the unitspec!
  if(!obj->InheritsFrom(TA_BaseSpec) &&
     !obj->InheritsFrom(TA_LeabraCon)) return false;
  return true;
}

//////////////////////////
//  	Unit, Spec	//
//////////////////////////

void ActFunSpec::Initialize() {
  i_thr = STD;
  Defaults_init();
}

void ActFunSpec::Defaults_init() {
  gelin = true;
  if(taMisc::is_loading) {
    taVersion v511(5, 1, 1);
    if(taMisc::loading_version < v511) { // default prior to 511 is non-gelin
      gelin = false;
    }
  }
  vm_mod_max = 0.95f;
  if(gelin) {
    thr = .5f;
    gain = 80.0f;
    nvar = .005f;
  }
  else {
    thr = .25f;
    gain = 600.0f;
    nvar = .005f;
  }
  avg_dt = .005f;
  avg_init = 0.15f;
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
  eq_gain = 10.0f;
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
  t_r = 0;
  vm_dend = 0.3f;
  vm_dend_dt = 0.16f;
  vm_dend_time = 1.0f / vm_dend_dt;
  clamp_max_p = .11f;
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

  dt_time = 1.0f / dt;
}

void ActAdaptSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt_time = 1.0f / dt;
}

void DepressSpec::Initialize() {
  on = false;
  Defaults_init();
}

void DepressSpec::Defaults_init() {
  rec = .2f;
  asymp_act = .5f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  interval = 1;
  max_amp = 1.0f;
}

void DepressSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(rec < .00001f) rec = .00001f;
  if(asymp_act < .00001f) asymp_act = .00001f;
  if(asymp_act > 1.0f) asymp_act = 1.0f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  depl = MAX(depl, 0.0f);
}

void FFBalanceSpec::Initialize() {
  on = false;
  Defaults_init();
}

void FFBalanceSpec::Defaults_init() {
  ff = 0.5f;
  ff_c = 1.0f - ff;
}

void FFBalanceSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ff_c = 1.0f - ff;
}

void SynDelaySpec::Initialize() {
  on = false;
  delay = 4;
}

void OptThreshSpec::Initialize() {
  send = .1f;
  delta = 0.005f;
  phase_dif = 0.0f;		// .8 also useful
}

void LeabraDtSpec::Initialize() {
  vm_eq_cyc = 0;
  Defaults_init();
}

void LeabraDtSpec::Defaults_init() {
  integ = 1.0f;
  vm = 0.3f;			// best for gelin
  net = 0.7f;
  d_vm_max = 100.0f;
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
  l_up_dt = 0.6f;
  l_dn_dt = 0.05f;
  m_dt = 0.1f;
  s_dt = 0.2f;
  ss_dt = 1.0f;
  use_nd = false;

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

  noise_type = NO_NOISE;
  noise.type = Random::GAUSSIAN;
  noise.var = .001f;

  noise_conv.x_range.min = -.05f;
  noise_conv.x_range.max = .05f;
  noise_conv.res = .001f;
  noise_conv.UpdateAfterEdit_NoGui();

  if(act.gelin) {
    nxx1_fun.x_range.min = -.1f;
    nxx1_fun.x_range.max = 1.0f;
    nxx1_fun.res = .001f;
    nxx1_fun.UpdateAfterEdit_NoGui();
  }
  else {
    nxx1_fun.x_range.min = -.03f;
    nxx1_fun.x_range.max = .20f;
    nxx1_fun.res = .001f;
    nxx1_fun.UpdateAfterEdit_NoGui();
  }

  Defaults_init();

  CreateNXX1Fun();
}

void LeabraUnitSpec::Defaults_init() {
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

  v_m_init.type = Random::UNIFORM;
  v_m_init.mean = e_rev.l;
  v_m_init.var = 0.0f;

  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  e_rev_sub_thr.h = e_rev.h - act.thr;
  e_rev_sub_thr.a = e_rev.a - act.thr;
  //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);

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
  clamp_range.UpdateAfterEdit_NoGui();
  vm_range.UpdateAfterEdit_NoGui();
  depress.UpdateAfterEdit_NoGui();
  noise_sched.UpdateAfterEdit_NoGui();
  spike.UpdateAfterEdit_NoGui();
  spike_misc.UpdateAfterEdit_NoGui();
  adapt.UpdateAfterEdit_NoGui();
  dt.UpdateAfterEdit_NoGui();
  ff_bal.UpdateAfterEdit_NoGui();
  act_avg.UpdateAfterEdit_NoGui();
  noise_adapt.UpdateAfterEdit_NoGui();
  CreateNXX1Fun();
  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  e_rev_sub_thr.h = e_rev.h - act.thr;
  e_rev_sub_thr.a = e_rev.a - act.thr;
  //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);
}

void LeabraUnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(CheckError(da_mod.on && act.i_thr != ActFunSpec::NO_AH, quiet, rval,
		"da_mod is on but act.i_thr != NO_AH -- this is generally required for da modulation to work properly as it operates through the a & h currents, and including them in i_thr computation leads to less clean modulation effects -- I set this for you in spec:", name)) {
    SetUnique("act", true);
    act.i_thr = ActFunSpec::NO_AH; // key for dopamine effects
  }
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(net) {
    if(CheckError(ff_bal.on && net->mid_minus_cycle <= 0, quiet, rval,
		  "if ff_bal.on is set, net->mid_minus_cycle must be > 0 -- is current not -- setting to ct_sravg.start:", name)) {
      net->mid_minus_cycle = net->ct_sravg.start;
    }
  }
}

bool LeabraUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  //  Network* net = GET_MY_OWNER(Network);
  bool rval = true;
 
  return rval;
}

void LeabraUnitSpec::CreateNXX1Fun() {
  // first create the gaussian noise convolver
  if(act.gelin) {
    nxx1_fun.x_range.max = 1.0f;
    nxx1_fun.res = .001f;	// needs same fine res to get the noise transitions
    nxx1_fun.UpdateAfterEdit_NoGui();
  }
  else {
    nxx1_fun.x_range.max = .20f;
    nxx1_fun.res = .001f;
    nxx1_fun.UpdateAfterEdit_NoGui();
  }
  float ns_rng = 3.0f * act.nvar;	// range factor based on noise level -- 3 sd 
  ns_rng = MAX(ns_rng, nxx1_fun.res);
  nxx1_fun.x_range.min = -ns_rng; 

  noise_conv.x_range.min = -ns_rng;
  noise_conv.x_range.max = ns_rng;
  noise_conv.res = nxx1_fun.res;
  noise_conv.UpdateAfterEdit_NoGui();

  noise_conv.AllocForRange();
  int i;
  float eff_nvar = MAX(act.nvar, 1.0e-6f); // just too lazy to do proper conditional for 0..
  float var = eff_nvar * eff_nvar;
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
  fun.UpdateAfterEdit_NoGui();
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

  nxx1_fun.Convolve(fun, noise_conv); // does alloc
}

void LeabraUnitSpec::SetLearnRule(LeabraNetwork* net) {
  if(bias_spec.SPtr())
    ((LeabraConSpec*)bias_spec.SPtr())->SetLearnRule(net);
}

void LeabraUnitSpec::Init_Weights(Unit* u, Network* net) {
  inherited::Init_Weights(u, net);
  LeabraUnit* lu = (LeabraUnit*)u;
  lu->act_avg = act.avg_init;
  lu->misc_1 = 0.0f;
  lu->misc_2 = 0.0f;
  lu->spk_amp = depress.max_amp;
  lu->vcb.hyst = lu->vcb.g_h = 0.0f;
  lu->vcb.hyst_on = false;
  lu->vcb.acc = lu->vcb.g_a = 0.0f;
  lu->vcb.acc_on = false;

  Init_ActAvg(lu, (LeabraNetwork*)net);
}

void LeabraUnitSpec::Init_ActAvg(LeabraUnit* u, LeabraNetwork* net) {
  u->act_avg = act.avg_init;
  u->avg_l = act.avg_init;
}  

void LeabraUnitSpec::Init_Acts(Unit* u, Network* net) {
  inherited::Init_Acts(u, net);
  LeabraUnit* lu = (LeabraUnit*)u;
  lu->net_scale = 0.0f;
  lu->bias_scale = 0.0f;
  lu->prv_net = 0.0f;
  lu->prv_g_i = 0.0f;
  if(hyst.init) {
    lu->vcb.hyst = lu->vcb.g_h = 0.0f;
    lu->vcb.hyst_on = false;
  }
  if(acc.init) {
    lu->vcb.acc = lu->vcb.g_a = 0.0f;
    lu->vcb.acc_on = false;
  }
  lu->gc.l = 0.0f;
  lu->gc.i = 0.0f;
  lu->gc.h = 0.0f;
  lu->gc.a = 0.0f;
  lu->I_net = 0.0f;
  lu->v_m = v_m_init.Gen();
  lu->vm_dend = 0.0f;
  lu->adapt = 0.0f;
  lu->da = 0.0f;
  lu->act = 0.0f;
  lu->act_eq = 0.0f;
  lu->act_nd = 0.0f;
  lu->act_p = lu->act_m = lu->act_dif = 0.0f;
  lu->act_m2 = lu->act_p2 = lu->act_dif2 = 0.0f;
  lu->avg_ss = act.avg_init;
  lu->avg_s = act.avg_init;
  lu->avg_m = act.avg_init;
  lu->davg = 0.0f;
  lu->dav = 0.0f;
  lu->noise = 0.0f;
  lu->maint_h = 0.0f;

  lu->act_sent = 0.0f;
  lu->net_raw = 0.0f;
  lu->net_delta = 0.0f;
  lu->g_i_raw = 0.0f;
  lu->g_i_delta = 0.0f;

  lu->i_thr = 0.0f;
  if(depress.on)
    lu->spk_amp = depress.max_amp;
  lu->spk_t = -1;
  lu->act_buf.Reset();
  lu->spike_buf.Reset();
}


void LeabraUnitSpec::DecayState(LeabraUnit* u, LeabraNetwork*, float decay) {
  u->v_m -= decay * (u->v_m - v_m_init.mean);
  u->vm_dend -= decay * u->vm_dend;
  u->adapt -= decay * u->adapt;
  u->act -= decay * u->act;
  u->act_nd -= decay * u->act_nd;
  u->act_eq -= decay * u->act_eq;
  // note: this is causing a problem in learning with xcal:
//   u->avg_ss -= decay * (u->avg_ss - act.avg_init);
//   u->avg_s -= decay * (u->avg_s - act.avg_init);
//   u->avg_m -= decay * (u->avg_m - act.avg_init);
  u->prv_net -= decay * u->prv_net;
  u->prv_g_i -= decay * u->prv_g_i;
  if(hyst.on && !hyst.trl)
    u->vcb.hyst -= decay * u->vcb.hyst;
  if(acc.on && !acc.trl)
    u->vcb.acc -= decay * u->vcb.acc;
  if(depress.on)
    u->spk_amp += (depress.max_amp - u->spk_amp) * decay;

  // reset the rest of this stuff just for clarity and accurate computation 
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
///////////////////////////////////////////////////////////////////////
//	TrialInit functions

void LeabraUnitSpec::SetCurLrate(LeabraNetwork* net, int epoch) {
  if(bias_spec.SPtr())
    ((LeabraConSpec*)bias_spec.SPtr())->SetCurLrate(net, epoch);
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
}

void LeabraUnitSpec::Trial_NoiseInit(LeabraUnit* u, LeabraNetwork* net) {
  if(noise_type != NO_NOISE && noise_adapt.trial_fixed && !noise_adapt.k_pos_noise && 
     (noise.type != Random::NONE)) {
    u->noise = noise.Gen();
  }
}

void LeabraUnitSpec::Trial_Init_SRAvg(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* lay = u->own_lay();

  if(net->learn_rule != LeabraNetwork::CTLEABRA_XCAL_C) {
    float lval = u->avg_m;
    if(lval > u->avg_l)
      u->avg_l += act_avg.l_up_dt * (lval - u->avg_l);
    else
      u->avg_l += act_avg.l_dn_dt * (lval - u->avg_l);
  }

  if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL || net->ct_sravg.force_con)  {
    for(int g = 0; g < u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->prjn->layer->lesioned() || !send_gp->size) continue;
      send_gp->Trial_Init_SRAvg(u);
    }
  }
}

///////////////////////////////////////////////////////////////////////
//	SettleInit functions

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
    if(!lay->HasExtFlag(Unit::TARG)) {	// layer isn't a target but unit is..
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
  else
    dkval = ls->decay.phase2;
  DecayState(u, net, dkval);
}

void LeabraUnitSpec::Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net) {
  // this is all receiver-based and done only at beginning of a trial
  u->net_scale = 0.0f;	// total of scale values for this unit's inputs

  float inhib_net_scale = 0.0f;
  int n_active_cons = 0;	// track this for bias weight scaling!
  bool old_scaling = false;
  // possible dependence on recv_gp->size is why this cannot be computed in Projection
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from.ptr();
    if(lay->lesioned() || !recv_gp->size)	continue;
     // this is the normalization value: takes into account target activity of layer
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    WtScaleSpec& wt_scale = cs->wt_scale;
    float savg = lay->kwta.pct;
    float lay_sz = (float)lay->units.leaves;
    float n_cons = (float)recv_gp->size;
    if(wt_scale.old) {
      old_scaling = true; // any old = old..
      if(cs->savg_cor.norm_con_n)	// this is default
	recv_gp->scale_eff = wt_scale.NetScale() / (n_cons * savg);
      else
	recv_gp->scale_eff = wt_scale.NetScale() / (lay_sz * savg);
    }
    else {			// new way!
      recv_gp->scale_eff = wt_scale.FullScale(savg, lay_sz, n_cons); 
    }
    if(cs->inhib && !old_scaling) {
      inhib_net_scale += wt_scale.rel;
    }
    else {
      n_active_cons++;
      u->net_scale += wt_scale.rel;
    }
  }
  // add the bias weight into the netinput, scaled by 1/n
  if(u->bias.size) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.SPtr();
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(u->n_recv_cons > 0)
      u->bias_scale /= (float)u->n_recv_cons; // one over n scaling for bias!
  }
  // now renormalize
  if(u->net_scale > 0.0f) {
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      Projection* prjn = (Projection*) recv_gp->prjn;
      LeabraLayer* lay = (LeabraLayer*) prjn->from.ptr();
      if(lay->lesioned() || !recv_gp->size)	continue;
      if(ff_bal.on && net->ct_cycle < net->mid_minus_cycle) { // before mid minus
	if(prjn->direction != Projection::FM_INPUT) {
	  recv_gp->scale_eff = 0.0f; // turn off everything except feedforward!
	  continue;
	}
      }
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(cs->inhib && !old_scaling) continue; // norm separately
      recv_gp->scale_eff /= u->net_scale; // normalize by total connection scale
    }
  }
  // separately normalize inhibitory connections
  if(inhib_net_scale > 0.0f) {
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from.ptr();
      if(lay->lesioned() || !recv_gp->size)	continue;
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(!cs->inhib) continue; // norm separately
      recv_gp->scale_eff /= inhib_net_scale; // normalize by total connection scale
    }
  }
}

void LeabraUnitSpec::Compute_NetinScale_Senders(LeabraUnit* u, LeabraNetwork*) {
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->size > 0) {
      Unit* ru = send_gp->Un(0);
      send_gp->scale_eff = ((LeabraRecvCons*)ru->recv.FastEl(send_gp->recv_idx()))->scale_eff;
    }
    else {
      send_gp->scale_eff = 0.0f;
    }
  }
}

void LeabraUnitSpec::Compute_HardClamp(LeabraUnit* u, LeabraNetwork*) {
  u->net = u->prv_net = u->ext;
  u->act_eq = clamp_range.Clip(u->ext);
  u->act_nd = u->act_eq;
  u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
  u->AddToActBuf(syn_delay);
}

// NOTE: these two functions should always be the same modulo the clamp_range.Clip

void LeabraUnitSpec::Compute_HardClampNoClip(LeabraUnit* u, LeabraNetwork*) {
  u->net = u->prv_net = u->ext;
  //  u->act_eq = clamp_range.Clip(u->ext);
  u->act_eq = u->ext;
  u->act_nd = u->act_eq;
  u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
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
//	Cycle Step 1: netinput 

void LeabraUnitSpec::Send_NetinDelta(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  if(thread_no < 0)
    net->send_pct_tot++;	// only safe for non-thread case
  float act_ts = u->act;
  if(syn_delay.on) {
    act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
  }

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->act_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      if(thread_no < 0)
	net->send_pct_n++;
      for(int g=0; g<u->send.size; g++) {
	LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	if(tol->lesioned() || tol->hard_clamped || !send_gp->size)	continue;
	send_gp->Send_NetinDelta(net, thread_no, act_delta);
      }
      u->act_sent = act_ts;	// cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    if(thread_no < 0)
      net->send_pct_n++;
    float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned() || tol->hard_clamped || !send_gp->size)	continue;
      send_gp->Send_NetinDelta(net, thread_no, act_delta);
    }
    u->act_sent = 0.0f;		// now it effectively sent a 0..
  }
}

void LeabraUnitSpec::Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  if(lay->hard_clamped) return;

  if(net->inhib_cons_used) {
    u->g_i_raw += u->g_i_delta;
    u->gc.i = u->prv_g_i + dt.net * (u->g_i_raw - u->prv_g_i);
    u->prv_g_i = u->gc.i;
  }

  u->net_raw += u->net_delta;
  float tot_net = (u->bias_scale * u->bias.OwnCn(0)->wt) + u->net_raw;
  if(u->HasExtFlag(Unit::EXT)) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    tot_net += u->ext * ls->clamp.gain;
  }

  u->net_delta = 0.0f;	// clear for next use
  u->g_i_delta = 0.0f;	// clear for next use

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = MAX(tot_net, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike(u,net);
  }
  else {
    float dnet = dt.net * (tot_net - u->prv_net);
    u->net = u->prv_net + dnet;
    u->prv_net = u->net;
    u->net = MAX(u->net, 0.0f);	// negative netin doesn't make any sense
  }

  // add just before computing i_thr -- after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += Compute_Noise(u, net);
  }
  u->i_thr = Compute_IThresh(u, net);
}

void LeabraUnitSpec::Compute_NetinInteg_Spike(LeabraUnit* u, LeabraNetwork* net) {
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
    float dnet = spike.gg_decay * sum - (u->prv_net * spike.oneo_decay);
    u->net = u->prv_net + dnet;
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
    float dnet =  dt.net * (u->net - u->prv_net);
    u->net = u->prv_net + dnet;
    u->prv_net = u->net;
  }
}

float LeabraUnitSpec::Compute_IThresh(LeabraUnit* u, LeabraNetwork* net) {
  switch(act.i_thr) {
  case ActFunSpec::STD:
    return Compute_IThreshStd(u, net);
  case ActFunSpec::NO_A:
    return Compute_IThreshNoA(u, net);
  case ActFunSpec::NO_H:
    return Compute_IThreshNoH(u, net);
  case ActFunSpec::NO_AH:
    return Compute_IThreshNoAH(u, net);
  case ActFunSpec::ALL:
    return Compute_IThreshAll(u, net);
  }
  return 0.0f;
} 

///////////////////////////////////////////////////////////////////////
//	Cycle Step 2: inhibition


///////////////////////////////////////////////////////////////////////
//	Cycle Step 3: activation

void LeabraUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  LeabraUnit* lu = (LeabraUnit*)u;
  LeabraNetwork* lnet = (LeabraNetwork*)net;
  LeabraLayer* lay = lu->own_lay();

  if((lnet->cycle >= 0) && lay->hard_clamped) {
    if(lay->hard_clamped && act_fun == SPIKE) {
      Compute_ClampSpike(lu, lnet, u->ext * spike_misc.clamp_max_p);
      lu->AddToActBuf(syn_delay);
    }
    return; // don't re-compute
  }

  Compute_Conduct(lu, lnet);
  Compute_Vm(lu, lnet);
  Compute_ActFmVm(lu, lnet);
  Compute_SelfReg_Cycle(lu, lnet);

  lu->AddToActBuf(syn_delay);
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
    return;			// do nothing further
  }
  if(fire_now) {
    u->v_m = spike_misc.spk_thr + 0.1f;	// make it fire
  }
  else {
    u->v_m = e_rev.l;		// make it not fire
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

void LeabraUnitSpec::Compute_DaMod_PlusCont(LeabraUnit* u, LeabraNetwork* net) {
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

void LeabraUnitSpec::Compute_Vm(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun == SPIKE && spike_misc.t_r > 0 && u->spk_t > 0) {
    int spkdel = net->ct_cycle - u->spk_t;
    if(spkdel >= 0 && spkdel <= spike_misc.t_r)
      return;			// just bail
  }

  if(net->cycle < dt.vm_eq_cyc) {
    // directly go to equilibrium value
    float new_v_m = Compute_EqVm(u);
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
    Compute_ActAdapt_spike(u, net);
  }
  else {
    Compute_ActFmVm_rate(u, net); 
    Compute_ActAdapt_rate(u, net);
  }
}

float LeabraUnitSpec::Compute_ActValFmVmVal_rate(float vm_val, float g_e_val, float g_e_thr) {
  float val_sub_thr;
  if(act.gelin) {
    val_sub_thr = g_e_val - g_e_thr;
  }
  else {
    val_sub_thr = vm_val - act.thr; // thresholded vm
  }
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
    break;			// compiler food
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

void LeabraUnitSpec::Compute_ActFmVm_rate(LeabraUnit* u, LeabraNetwork* net) {
  float new_act;
  if(act.gelin) {
    float g_e_val = u->net;
    float vm_eq = act.vm_mod_max * (Compute_EqVm(u) - v_m_init.mean); // relative to starting!
    if(vm_eq > 0.0f) {
      float vmrat = (u->v_m - v_m_init.mean) / vm_eq;
      if(vmrat > 1.0f) vmrat = 1.0f;
      else if(vmrat < 0.0f) vmrat = 0.0f;
      g_e_val *= vmrat;
    }
    float g_e_thr = Compute_EThresh(u);
    new_act = Compute_ActValFmVmVal_rate(u->v_m, g_e_val, g_e_thr);
  }
  else {
    new_act = Compute_ActValFmVmVal_rate(u->v_m, 0.0f, 0.0f);
  }
  if(depress.on) {		     // synaptic depression
    u->act_nd = act_range.Clip(new_act); // nd is non-discounted activation!!! solves tons of probs
    new_act *= MIN(u->spk_amp, 1.0f);
    if((net->ct_cycle+1) % depress.interval == 0) {
      u->spk_amp += -new_act * depress.depl + (depress.max_amp - u->spk_amp) * depress.rec;
    }
    if(u->spk_amp < 0.0f) 			u->spk_amp = 0.0f;
    else if(u->spk_amp > depress.max_amp)	u->spk_amp = depress.max_amp;
  }

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += Compute_Noise(u, net);
  }
  u->act = act_range.Clip(new_act);
  if(ff_bal.on && net->ct_cycle > net->mid_minus_cycle) {
    u->act = ff_bal.ff * u->act_m2 + ff_bal.ff_c * u->act;
  }
  u->act_eq = u->act;
  if(!depress.on)
    u->act_nd = u->act_eq;
}

void LeabraUnitSpec::Compute_ActAdapt_rate(LeabraUnit* u, LeabraNetwork* net) {
  if(!adapt.on)
    u->adapt = 0.0f;
  else {
    float dad = dt.integ * adapt.Compute_dAdapt(u->v_m, e_rev.l, u->adapt); // rest relative
    if(net->ct_cycle % adapt.interval == 0) {
      dad += u->act * adapt.spike_gain;	// rate code version of spiking
    }
    u->adapt += dad;
  }
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
  else {			// increment by phase
    if(net->cycle > 0)
      old_nd *= (float)net->cycle;
    new_nd = act_range.Clip(spike.eq_gain * (old_nd + u->act) / (float)(net->cycle+1));
  }
  u->da = new_nd - u->act_nd;	// da is on equilibrium activation
  u->act_nd = new_nd;

  if(depress.on) {
    u->act *= MIN(u->spk_amp, 1.0f);	// after eq
    u->spk_amp += -u->act * depress.depl + (depress.max_amp - u->spk_amp) * depress.rec;
    if(u->spk_amp < 0.0f) 			u->spk_amp = 0.0f;
    else if(u->spk_amp > depress.max_amp)	u->spk_amp = depress.max_amp;
    u->act_eq = u->spk_amp * u->act_nd; // act_eq is depressed rate code, nd is non-depressed!
  }
  else {
    u->act_eq = u->act_nd;	// eq = nd
  }
}

void LeabraUnitSpec::Compute_ActAdapt_spike(LeabraUnit* u, LeabraNetwork* net) {
  if(!adapt.on)
    u->adapt = 0.0f;
  else {
    float dad = dt.integ * adapt.Compute_dAdapt(u->v_m, e_rev.l, u->adapt);
    if(u->act > 0.0f) {						      // spiked
      dad += adapt.spike_gain;
    }
    u->adapt += dad;
  }
}

void LeabraUnitSpec::Compute_SelfReg_Cycle(LeabraUnit* u, LeabraNetwork*) {
  // fast time scale updated every cycle
  if(hyst.on && !hyst.trl)
    hyst.UpdateBasis(u->vcb.hyst, u->vcb.hyst_on, u->vcb.g_h, u->act_eq);
  if(acc.on && !acc.trl)
    acc.UpdateBasis(u->vcb.acc, u->vcb.acc_on, u->vcb.g_a, u->act_eq);
}

void LeabraUnitSpec::Compute_SelfReg_Trial(LeabraUnit* u, LeabraNetwork*) {
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

///////////////////////////////////////////////////////////////////////
//	Cycle Stats

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
//	Cycle Optional Misc

void LeabraUnitSpec::Compute_MidMinus(LeabraUnit* u, LeabraNetwork* net) {
  u->act_m2 = u->act_eq;
  if(ff_bal.on) {
    Compute_NetinScale(u, net); // turn other connections back on!
  }
}

void LeabraUnitSpec::Compute_CycSynDep(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->prjn->layer->lesioned() || !send_gp->size) continue;
    send_gp->Compute_CycSynDep(u);
  }
}


///////////////////////////////////////////////////////////////////////
//	Phase and Trial Activation Updating

void LeabraUnitSpec::PostSettle(LeabraUnit* u, LeabraNetwork* net) {
  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  switch(net->phase_order) {
  case LeabraNetwork::MINUS_PLUS:
    if(no_plus_testing) {
      u->act_m = u->act_p = u->act_nd;
      u->act_dif = 0.0f;
      Compute_ActTimeAvg(u, net);
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE)
	u->act_m = u->act_nd;
      else {
	u->act_p = u->act_nd;
	u->act_dif = u->act_p - u->act_m;
	Compute_DaMod_PlusPost(u, net);
	Compute_ActTimeAvg(u, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_MINUS:
    if(no_plus_testing) {
      u->act_m = u->act_p = u->act_nd;
      u->act_dif = 0.0f;
      Compute_ActTimeAvg(u, net);
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE) {
	u->act_m = u->act_nd;
	u->act_dif = u->act_p - u->act_m;
      }
      else {
	u->act_p = u->act_nd;
	Compute_ActTimeAvg(u, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_ONLY:
    u->act_m = u->act_p = u->act_nd;
    u->act_dif = 0.0f;
    Compute_ActTimeAvg(u, net);
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
      Compute_DaMod_PlusPost(u, net);
      Compute_ActTimeAvg(u, net);
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
      Compute_ActTimeAvg(u, net);
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
      Compute_DaMod_PlusPost(u, net);
      Compute_ActTimeAvg(u, net);
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
      Compute_DaMod_PlusPost(u, net);
      Compute_ActTimeAvg(u, net);
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


//////////////////////////////////////////
//	Stage 6: Learning 		//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_SRAvg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();

  bool do_s = net->sravg_vals.do_s; // set on net at start of call..

  float ru_act; // activation to use for updating averages
  if(net->learn_rule <= LeabraNetwork::CTLEABRA_CAL || act_avg.use_nd) {
    ru_act = u->act_nd;
  }
  else {
    ru_act = u->act;
    LeabraUnitSpec* rus = (LeabraUnitSpec*)u->GetUnitSpec();
    if(rus->act_fun == LeabraUnitSpec::SPIKE)
      ru_act *= rus->spike.eq_gain;
  }

  if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL_C) {
    // note: this is cascade version -- each builds upon the other
    u->avg_ss += act_avg.ss_dt * (ru_act - u->avg_ss);
    float ds = act_avg.s_dt * (u->avg_ss - u->avg_s);
    u->avg_s += ds;
    u->davg += net->ct_lrn_trig.davg_dt * (ds - u->davg);

    u->avg_m += act_avg.m_dt * (u->avg_s - u->avg_m);

    float lval = u->avg_m; // note this is *avg_m* not act_m..
    if(lval > u->avg_l)
      u->avg_l += act_avg.l_up_dt * (lval - u->avg_l);
    else
      u->avg_l += act_avg.l_dn_dt * (lval - u->avg_l);
  }
  else {
    // use continuous updating so these are always current -- no need for post-average step
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    float eff_m_sum = net->sravg_vals.m_sum;
    if(ls->ct_inhib_mod.sravg_delay > 0) {
      eff_m_sum = net->sravg_vals.m_sum - (float)ls->ct_inhib_mod.sravg_delay;
      if(eff_m_sum < 0.0f)
	return;			// not ready yet
    }
    if(eff_m_sum == 0.0f) {
      u->avg_m = ru_act;
    }
    else {
      u->avg_m = (ru_act + u->avg_m * eff_m_sum) / (eff_m_sum + 1.0f);
    }
    if(do_s) {			// not subject to delay!
      if(net->sravg_vals.s_sum == 0.0f) {
	u->avg_s = ru_act;
      }
      else {
	u->avg_s = (ru_act + u->avg_s * net->sravg_vals.s_sum) / (net->sravg_vals.s_sum + 1.0f);
      }
    }
  }

  if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL || net->ct_sravg.force_con) {
    if(net->train_mode != LeabraNetwork::TEST) {	// expensive con-level only for training
      for(int g=0; g<u->send.size; g++) {
	LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
	if(rlay->lesioned() || !send_gp->size || !rlay->Compute_SRAvg_Test(net)) continue;
	send_gp->Compute_SRAvg(u, do_s);
      }
    }
  }
}

void LeabraUnitSpec::Compute_dWt_FirstPlus(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  for(int g = 0; g < u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    if(rlay->lesioned() || !send_gp->size || !rlay->Compute_dWt_FirstPlus_Test(net)) continue;
    send_gp->Compute_Leabra_dWt(u);
  }

  LeabraLayer* lay = u->own_lay();
  if(!lay->Compute_dWt_FirstPlus_Test(net)) return; // applies to bias weights now

  LeabraConSpec* bspc = ((LeabraConSpec*)bias_spec.SPtr());
  bspc->B_Compute_Leabra_dWt((LeabraCon*)u->bias.OwnCn(0), u, lay);
}

void LeabraUnitSpec::Compute_dWt_SecondPlus(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  for(int g = 0; g < u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    if(rlay->lesioned() || !send_gp->size || !rlay->Compute_dWt_SecondPlus_Test(net)) continue;
    send_gp->Compute_Leabra_dWt(u);
  }

  LeabraLayer* lay = u->own_lay();
  if(!lay->Compute_dWt_SecondPlus_Test(net)) return; // applies to bias weights now

  LeabraConSpec* bspc = ((LeabraConSpec*)bias_spec.SPtr());
  bspc->B_Compute_Leabra_dWt((LeabraCon*)u->bias.OwnCn(0), u, lay);
}

void LeabraUnitSpec::Compute_dWt_Nothing(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  for(int g = 0; g < u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    if(rlay->lesioned() || !send_gp->size || !rlay->Compute_dWt_Nothing_Test(net)) continue;
    send_gp->Compute_Leabra_dWt(u);
  }

  LeabraLayer* lay = u->own_lay();
  if(!lay->Compute_dWt_Nothing_Test(net)) return; // applies to bias weights now

  LeabraConSpec* bspc = ((LeabraConSpec*)bias_spec.SPtr());
  bspc->B_Compute_Leabra_dWt((LeabraCon*)u->bias.OwnCn(0), u, lay);
}

void LeabraUnitSpec::Compute_Weights(Unit* u, Network* net, int thread_no) {
  LeabraUnit* lu = (LeabraUnit*)u;
  LeabraNetwork* lnet = (LeabraNetwork*)net;

  if(lnet->learn_rule == LeabraNetwork::CTLEABRA_XCAL ||
     lnet->learn_rule == LeabraNetwork::CTLEABRA_XCAL_C) {
    if(lnet->epoch < lnet->ct_time.n_avg_only_epcs) { // no learning while gathering data!
      return;
    }
  }

  for(int g = 0; g < lu->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)lu->send.FastEl(g);
    LeabraLayer* rlay = (LeabraLayer*)send_gp->prjn->layer;
    if(rlay->lesioned() || !send_gp->size) continue;
    send_gp->Compute_Leabra_Weights(lu);
  }
  LeabraConSpec* bspc = ((LeabraConSpec*)bias_spec.SPtr());
  bspc->B_Compute_Weights((LeabraCon*)u->bias.OwnCn(0), lu);
}

///////////////////////////////////////////////////////////////////////
//	Stats

float LeabraUnitSpec::Compute_SSE(Unit* u, Network* net, bool& has_targ) {
  // just replaces act_m for act in original
  float sse = 0.0f;
  has_targ = false;
  LeabraUnit* lu = (LeabraUnit*)u;
  if(lu->HasExtFlag(Unit::TARG | Unit::COMP)) {
    has_targ = true;
    float uerr = lu->targ - lu->act_m;
    if(fabsf(uerr) >= sse_tol)
      sse = uerr * uerr;
  }
  return sse;
}

bool LeabraUnitSpec::Compute_PRerr(Unit* u, Network* net, float& true_pos, float& false_pos, float& false_neg) {
  // just replaces act_m for act in original
  true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f;
  bool has_targ = false;
  LeabraUnit* lu = (LeabraUnit*)u;
  if(u->HasExtFlag(Unit::TARG | Unit::COMP)) {
    has_targ = true;
    float uerr = lu->targ - lu->act_m;
    if(fabsf(uerr) < sse_tol) {
      true_pos = lu->targ;
    }
    else {
      if(lu->targ > lu->act_m) {
	true_pos = lu->act_m;
	false_neg = lu->targ - lu->act_m;
      }
      else {
	true_pos = lu->targ;
	false_pos = lu->act_m - lu->targ;
      }
    }
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
//	 Misc Functions 		//
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
  spike_misc.vm_r = e_rev.l;					 // go back to leak

  adapt.dt = 1.0f / ((adapt_dt_time_ms * 1.0e-3f) / norm_sec);
  adapt.vm_gain = (adapt_vm_gain_nS * 1.0e-9f) / norm_siemens;
  adapt.spike_gain = (adapt_spk_gain_nA * 1.0e-9f) / norm_amp;

  v_m_init.mean = e_rev.l;
  vm_range.min = 0.0f;
  vm_range.max = 2.0f;
  dt.d_vm_max = 100.0f;		// no max

  if(gelin) {
    dt.vm = 0.3f;
    act.gain = 80;
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

  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  float x;
  for(x = min; x <= max; x += incr) {
    float aval;
    if(act.gelin) {
      aval = Compute_ActValFmVmVal_rate(0.0f, x, g_e_thr);
    }
    else {
      float v_m = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
	((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
      aval = Compute_ActValFmVmVal_rate(v_m, 0.0f, 0.0f);
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
  avg_ss = 0.15f;
  avg_s = 0.15f;
  avg_m = 0.15f;
  avg_l = 0.15f;
  davg = 0.0f;
  act_p = act_m = act_dif = 0.0f;
  act_m2 = act_p2 = act_dif2 = 0.0f;
  da = 0.0f;
  I_net = 0.0f;
  v_m = 0.0f;
  noise = 0.0f;
  dav = 0.0f;
  maint_h = 0.0f;

  in_subgp = false;
  net_scale = 0.0f;
  bias_scale = 0.0f;
  prv_net = 0.0f;
  prv_g_i = 0.0f;

  act_sent = 0.0f;
  net_raw = 0.0f;
  net_delta = 0.0f;
  g_i_raw = 0.0f;
  g_i_delta = 0.0f;

  i_thr = 0.0f;
  spk_amp = 1.0f;
  misc_1 = 0.0f;
  misc_2 = 0.0f;
  spk_t = -1;
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
  avg_s = cp.avg_s;
  avg_ss = cp.avg_ss;
  avg_m = cp.avg_m;
  avg_l = cp.avg_l;
  davg = cp.davg;
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
  noise = cp.noise;
  dav = cp.dav;
  maint_h = cp.maint_h;
  // not: in_subgp
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
  spk_t = cp.spk_t;
  act_buf = cp.act_buf;
  spike_buf = cp.spike_buf;
}

LeabraInhib* LeabraUnit::own_thr() const {
  LeabraLayer* lay = own_lay();
  if(!lay) return NULL;
  Unit_Group* own_sgp = own_subgp();
  if(own_sgp) {
    int gpidx = own_sgp->GetIndex();
    LeabraInhib* thr = (LeabraInhib*)lay->ungp_data.SafeEl(gpidx);
    if(thr) return thr;
  }
  return (LeabraInhib*)lay;
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

void LeabraPrjn::CheckInhibCons(LeabraNetwork* net) {
  LeabraLayer* fmlay = (LeabraLayer*)from.ptr();
  if(!fmlay || fmlay->lesioned()) return;
  if(con_spec.SPtr()) {
    if(((LeabraConSpec*)con_spec.SPtr())->inhib)
      net->inhib_cons_used = true;
  }
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
  type = KWTA_AVG_INHIB;
  low0 = false;
  min_i = 0.0f;
  Defaults_init();
}

void LeabraInhibSpec::Defaults_init() {
  switch(type) {
  case KWTA_INHIB:
  case KWTA_KV2K:
    kwta_pt = .2f;
    break;
  case KWTA_AVG_INHIB:
    kwta_pt = .5f;
    break;
  case AVG_MAX_PT_INHIB:
    kwta_pt = .2f;
    break;
  }
  net_gain = 0.35f;
  act_gain = 0.4f;
  kink_gain = 1.1f;
  avg_up_dt = 0.9f;
  avg_dn_dt = 0.3f;
  comp_thr = .5f;
  comp_gain = 2.0f;
  gp_pt = .2f;
}

void KWTASpec::Initialize() {
  k_from = USE_PCT;
  k = 12;
  pct = .25f;
  pat_q = .2f;
  diff_act_pct = false;
  act_pct = .1f;
  gp_i = false;
  gp_g = .5f;
}

void KWTASpec::Defaults_init() {
  pat_q = .5f;
}

void KwtaTieBreak::Initialize() {
  on = false;
  Defaults_init();
}

void KwtaTieBreak::Defaults_init() {
  k_thr = 1.0f;
  diff_thr = 0.2f;
  thr_gain = 0.005f;
  loser_gain = 1.0f;
}

void AdaptISpec::Initialize() {
  type = NONE;
  Defaults_init();
}

void AdaptISpec::Defaults_init() {
  tol = .02f;			// allow to be off by this amount before taking action
  p_dt = .1f;			// take reasonably quick action..
  mx_d = .9f;			// move this far in either direction
  l = .2f;			// proportion to assign to leak..
  a_dt = .005f;			// time averaging
}

void ClampSpec::Initialize() {
  hard = true;
  Defaults_init();
}

void ClampSpec::Defaults_init() {
  max_plus = true;
  plus = 0.01f;
  min_clamp = 0.5f;
  if(taMisc::is_loading) {
    taVersion v511(5, 1, 1);
    if(taMisc::loading_version < v511) { // default prior to 511 is max-plus off
      max_plus = false;
    }
  }
  gain = .2f;
}

void DecaySpec::Initialize() {
  event = 1.0f;
  phase = 1.0f;
  phase2 = 0.0f;
  clamp_phase2 = false;
}

void CtLayerInhibMod::Initialize() {
  sravg_delay = 0;
  use_sin = false;
  burst_i = 0.02f;
  trough_i = 0.02f;
  use_fin = false;
  inhib_i = 0.0f;
}

void LayAbsNetAdaptSpec::Initialize() {
  on = false;
  Defaults_init();
}

void LayAbsNetAdaptSpec::Defaults_init() {
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

void LeabraLayerSpec::Defaults_init() {
  // nothing at this level..
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
  InitLinks_taAuto(&TA_LeabraLayerSpec);
  children.SetBaseType(&TA_LeabraLayerSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

bool LeabraLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = true;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
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

  lay->CheckError((inhib_group == UNIT_GROUPS && !lay->unit_groups), quiet, rval,
		  "inhib_group is UNIT_GROUPS but layer is not set to use unit_groups!  will not work.");
		     
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


///////////////////////////////////////////////////////////////////////
//	General Init functions


void LeabraLayerSpec::BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net) {
  lay->Layer::BuildUnits_Threads(net); // call default
  lay->BuildKwtaBuffs();	       // also make sure kwta buffs are rebuilt!
}

void LeabraLayer::CheckInhibCons(LeabraNetwork* net) {
  LeabraPrjn* p;
  taLeafItr pi;
  FOR_ITR_EL(LeabraPrjn, p, projections., pi) {
    p->CheckInhibCons(net);
  }
}

void LeabraLayerSpec::SetLearnRule(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::LEABRA_CHL) {
    decay.phase = 1.0f;		// all phase decay
  }
  else {
    decay.phase = 0.0f;		// no phase decay -- these are not even called
    decay.phase2 = 0.0f;
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

void LeabraLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  lay->BuildKwtaBuffs();	// make sure kwta buffs are rebuilt!
  Compute_Active_K(lay, net);	// need kwta.pct for init
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_Weights(net);
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->Init_State();
    }
  }
  Init_Inhib(lay, net);		// initialize inhibition at start..
  Init_Stats(lay, net);
}

void LeabraLayerSpec::Init_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  lay->adapt_i.avg_avg = lay->kwta.pct;
  lay->adapt_i.i_kwta_pt = inhib.kwta_pt;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  lay->adapt_i.g_bar_i = us->g_bar.i;
  lay->adapt_i.g_bar_l = us->g_bar.l;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->adapt_i.avg_avg = gpd->kwta.pct;
      if(((inhib.type == LeabraInhibSpec::AVG_MAX_PT_INHIB)
	  || (inhib.type == LeabraInhibSpec::MAX_INHIB))
	 && (inhib_group != ENTIRE_LAYER))
	gpd->adapt_i.i_kwta_pt = inhib.gp_pt;
      else
	gpd->adapt_i.i_kwta_pt = inhib.kwta_pt;
      gpd->adapt_i.g_bar_i = us->g_bar.i;
      gpd->adapt_i.g_bar_l = us->g_bar.l;
    }
  }
}

void LeabraLayerSpec::Init_Stats(LeabraLayer* lay, LeabraNetwork* net) {
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

void LeabraLayerSpec::Init_ActAvg(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_ActAvg(net);
}

void LeabraLayerSpec::Init_Acts(LeabraLayer* lay, LeabraNetwork* net) {
  lay->ext_flag = Unit::NO_EXTERNAL;
  lay->hard_clamped = false;
  lay->ResetSortBuf();
  Compute_Active_K(lay, net);	// need kwta.pct for init
  lay->Inhib_Init_Acts(this);
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->Inhib_Init_Acts(this);
    }
  }
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_Acts(net);
}

void LeabraLayerSpec::DecayState(LeabraLayer* lay, LeabraNetwork* net, float decay) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayState(net, decay);
  }
}

///////////////////////////////////////////////////////////////////////
//	TrialInit -- at start of trial

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

void LeabraLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
}

// NOTE: the following are not typically used, as the Trial_Init_Units calls directly
// to the unit level -- if anything is added to the layer-level then a _Layers call is
// needed instead

void LeabraLayerSpec::Trial_DecayState(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Trial_DecayState(net);
  }
}

void LeabraLayerSpec::Trial_NoiseInit_KPos_ugp(LeabraLayer* lay,
					       Layer::AccessMode acc_md, int gpidx, 
					       LeabraInhib* thr, LeabraNetwork* net) {
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(lay->unit_idxs.size != nunits) {
    lay->unit_idxs.SetSize(nunits);
    lay->unit_idxs.FillSeq();
  }
  lay->unit_idxs.Permute();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, lay->unit_idxs[i], gpidx);
    if(i < thr->kwta.k)
      u->noise = us->noise.var;
    else
      u->noise = 0.0f;
  }
}

  
void LeabraLayerSpec::Trial_NoiseInit(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(!us->noise_adapt.trial_fixed) return;

  if(us->noise_adapt.k_pos_noise) {
    if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
      for(int g=0; g < lay->gp_geom.n; g++) {
	LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
	Trial_NoiseInit_KPos_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      }
    }
    else {
      Trial_NoiseInit_KPos_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    }
  }
  // don't do this b/c already done at unit level for this case..
//   else {
//     LeabraUnit* u;
//     taLeafItr i;
//     FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
//       u->Trial_NoiseInit(net);
//     }
//   }
}
  
void LeabraLayerSpec::Trial_Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Trial_Init_SRAvg(net);
  }
}


///////////////////////////////////////////////////////////////////////
//	SettleInit -- at start of settling

void LeabraLayerSpec::Compute_Active_K(LeabraLayer* lay, LeabraNetwork* net) {
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    int totk = 0;
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_Active_K_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, gp_kwta);
      totk += gpd->kwta.k;
    }
    if(inhib_group == UNIT_GROUPS) {
      if(lay->kwta.k != totk) {
	lay->lay_kbuffs.ResetAllBuffs();
	lay->Inhib_ResetSortBuf();
      }
      lay->kwta.k = totk;
      lay->kwta.Compute_Pct(lay->units.leaves);
      if(gp_kwta.diff_act_pct) 
	lay->kwta.pct = gp_kwta.act_pct;	// override!!
    }
  }
  if(inhib_group != UNIT_GROUPS) {
    Compute_Active_K_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, kwta);
    if(kwta.diff_act_pct)
      lay->kwta.pct = kwta.act_pct;	// override!!
  }
}

void LeabraLayerSpec::Compute_Active_K_ugp(LeabraLayer* lay, 
					   Layer::AccessMode acc_md, int gpidx, 
					   LeabraInhib* thr, KWTASpec& kwtspec)
{
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int new_k = 0;
  if(kwtspec.k_from == KWTASpec::USE_PCT)
    new_k = (int)(kwtspec.pct * (float)nunits);
  else if((kwtspec.k_from == KWTASpec::USE_PAT_K) && 
	  (lay->HasExtFlag(Unit::TARG | Unit::COMP | Unit::EXT)))
    new_k = Compute_Pat_K(lay, acc_md, gpidx, thr);
  else
    new_k = kwtspec.k;

  if(inhib.type == LeabraInhibSpec::KWTA_INHIB)
    new_k = MIN(nunits - 1, new_k);
  else
    new_k = MIN(nunits, new_k);
  new_k = MAX(1, new_k);

  if(thr->kwta.k != new_k) {
    KwtaSortBuff_List* srtbuff = lay->SortBuffList(acc_md);
    srtbuff->ResetAllBuffs();
    thr->Inhib_ResetSortBuf();
  }

  thr->kwta.k = new_k;
  thr->kwta.Compute_Pct(nunits);
}

int LeabraLayerSpec::Compute_Pat_K(LeabraLayer* lay, 
				   Layer::AccessMode acc_md, int gpidx, 
				   LeabraInhib* thr) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  bool use_comp = false;
  if(lay->HasExtFlag(Unit::COMP)) // only use comparison vals if entire lay is COMP!
    use_comp = true;
  int pat_k = 0;
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    // use either EXT or TARG information...
    if(u->HasExtFlag(Unit::EXT)) {
      if(u->ext >= kwta.pat_q)
	pat_k++;
    }
    else if(u->HasExtFlag(Unit::TARG)) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }
    else if(use_comp && u->HasExtFlag(Unit::COMP)) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }	      
  }
  return pat_k;
}

void LeabraLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  Settle_Init_TargFlags_Layer(lay, net);
}

void LeabraLayerSpec::Settle_Init_TargFlags(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Settle_Init_TargFlags(net);
  }

  Settle_Init_TargFlags_Layer(lay, net);
}

void LeabraLayerSpec::Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->HasExtFlag(Unit::TARG)) {	// only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE)
      lay->SetExtFlag(Unit::EXT);
  }
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
    u->Compute_HardClampNoClip(net); // important: uses no clip here: not really external values!
  }
  Compute_CycleStats(lay, net);	// compute once only now
}

void LeabraLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(decay.clamp_phase2 && (net->phase != LeabraNetwork::MINUS_PHASE) && (net->phase_no >= 2)) {
    Compute_HardClampPhase2(lay, net);
    return;
  }
  if(!(clamp.hard && lay->HasExtFlag(Unit::EXT))) {
    lay->hard_clamped = false;
    return;
  }
  lay->hard_clamped = true;	// cache this flag
  lay->Inhib_SetVals(inhib.kwta_pt);		// assume 0 - 1 clamped inputs

  if(clamp.max_plus && net->phase == LeabraNetwork::PLUS_PHASE && net->phase_no > 0 &&
     lay->HasExtFlag(Unit::TARG)) {
    float min_max = lay->acts_m.max;
    float clmp = min_max + clamp.plus;
    clmp = MAX(clmp, clamp.min_clamp);
    
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
      u->ext *= clmp;		// modify!
    }
  }

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_HardClamp(net);
  }
  Compute_CycleStats(lay, net);	// compute once only
}

void LeabraLayerSpec::ExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(Unit::EXT))	// only process ext
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->ExtToComp(net);
}

void LeabraLayerSpec::TargExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(Unit::TARG_EXT))	// only process w/ external input
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->TargExtToComp(net);
}

///////////////////////////////////////////////////////////////////////
//	Cycle Step 1: Netinput 

void LeabraLayerSpec::Compute_NetinStats_ugp(LeabraLayer* lay,
					     Layer::AccessMode acc_md, int gpidx,
					     LeabraInhib* thr,  LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  thr->netin.InitVals();  thr->i_thrs.InitVals();
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    thr->netin.UpdtVals(u->net, i);    thr->i_thrs.UpdtVals(u->i_thr, i);
  }
  thr->netin.CalcAvg(nunits);  thr->i_thrs.CalcAvg(nunits);

  if(thr->netin_top_k.cmpt) {
    thr->netin_top_k.InitVals();
    int k_eff = thr->kwta.k;	// keep cutoff at k
    KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
    if(k_eff <= 0 || act_buff->Size(gpidx) != k_eff)
      return; // no can do

    for(int j=0; j < k_eff; j++) {
      thr->netin_top_k.UpdtVals(act_buff->Un(j, gpidx)->net, j);
    }
    thr->netin_top_k.CalcAvg(k_eff);
  }
}

void LeabraLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    lay->netin.InitVals();    lay->i_thrs.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_NetinStats_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      lay->netin.UpdtFmAvgMax(gpd->netin, nunits, g);
      lay->i_thrs.UpdtFmAvgMax(gpd->i_thrs, nunits, g);
      lay->netin_top_k.UpdtFmAvgMax(gpd->netin_top_k, 1, g); // only compute gp-wise avg for avg top k (n=1 per group)
    }
    lay->netin.CalcAvg(lay->units.leaves);
    lay->i_thrs.CalcAvg(lay->units.leaves);
    lay->netin_top_k.CalcAvg(lay->gp_geom.n);
  }
  else {
    Compute_NetinStats_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

///////////////////////////////////////////////////////////////////////
//	Cycle Step 2: Inhibition, Basic computation

void LeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped)	return;	// say no more..

  if(inhib_group != UNIT_GROUPS) {
    Compute_Inhib_impl(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net, inhib);
  }
  if(lay->unit_groups) {
    if(inhib_group == UNIT_GROUPS) {
      lay->Inhib_SetVals(0.0f);
      for(int g=0; g < lay->gp_geom.n; g++) {
	LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
	Compute_Inhib_impl(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net, inhib);
	float gp_g_i = gpd->i_val.g_i;
	if(gp_kwta.gp_i)
	  gp_g_i *= gp_kwta.gp_g;
	lay->i_val.g_i = MAX(lay->i_val.g_i, gp_g_i);
      }
    }
    else if(inhib_group == LAY_AND_GPS) {
      for(int g=0; g < lay->gp_geom.n; g++) {
	LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
	Compute_Inhib_impl(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net, inhib);
      }
    }
    Compute_LayInhibToGps(lay, net);
  }

  Compute_CtDynamicInhib(lay, net);
}

void LeabraLayerSpec::Compute_Inhib_impl(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx, 
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  if(ispec.type == LeabraInhibSpec::UNIT_INHIB) {
    thr->i_val.g_i = 0.0f;	// make sure it's zero, cuz this gets added to units.. 
  }
  else {
    switch(ispec.type) {
    case LeabraInhibSpec::KWTA_INHIB:
      Compute_Inhib_kWTA(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::KWTA_AVG_INHIB:
      Compute_Inhib_kWTA_Avg(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::KWTA_KV2K:
      Compute_Inhib_kWTA_kv2k(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::KWTA_COMP_COST:
      Compute_Inhib_kWTA_CompCost(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::AVG_MAX_PT_INHIB:
      Compute_Inhib_AvgMaxPt(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::MAX_INHIB:
      Compute_Inhib_Max(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::AVG_NET_ACT:
      Compute_Inhib_AvgNetAct(lay, acc_md, gpidx, thr, net, ispec);
      break;
    }
    thr->i_val.g_i = thr->i_val.kwta;
  }

  thr->i_val.g_i_orig = thr->i_val.g_i;	// retain original values..
}

// basic sorting function:

void LeabraLayerSpec::Compute_Inhib_kWTA_Sort(LeabraLayer* lay, Layer::AccessMode acc_md,
				      int gpidx, int nunits, LeabraInhib* thr,
				      KwtaSortBuff& act_buff, KwtaSortBuff& inact_buff,
				      int k_eff, float& k_net, int& k_idx) {
  LeabraUnit* u;
  int j;
  if(act_buff.Size(gpidx) != k_eff) { // need to fill the sort buf..
    act_buff.ResetGp(gpidx);
    for(j=0; j < k_eff; j++) {
      u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
      act_buff.Add(u, gpidx); // add to list
      if(u->i_thr < k_net) {
	k_net = u->i_thr;	k_idx = j;
      }
    }
    inact_buff.ResetGp(gpidx);
    // now, use the "replace-the-lowest" sorting technique
    for(; j < nunits; j++) {
      u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
      if(u->i_thr <= k_net) {	// not bigger than smallest one in sort buffer
	inact_buff.Add(u, gpidx);
	continue;
      }
      inact_buff.Add(act_buff.Un(k_idx, gpidx), gpidx); // now inactive
      act_buff.Set(u, k_idx, gpidx); // replace the smallest with it
      k_net = u->i_thr;		// assume its the smallest
      for(int b=0; b < k_eff; b++) { 	// and recompute the actual smallest
	float tmp = act_buff.Un(b, gpidx)->i_thr;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = b;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find k_net
    for(j=0; j < k_eff; j++) { 	// these should be the top ones, very fast!!
      float tmp = act_buff.Un(j, gpidx)->i_thr;
      if(tmp < k_net) {
	k_net = tmp;		k_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    int iabsz = inact_buff.Size(gpidx);
    for(j=0; j < iabsz; j++) {
      u = inact_buff.Un(j, gpidx);
      if(u->i_thr <= k_net)		// not bigger than smallest one in sort buffer
	continue;
      inact_buff.Set(act_buff.Un(k_idx, gpidx), j, gpidx);	// now inactive
      act_buff.Set(u, k_idx, gpidx); // replace the smallest with it
      k_net = u->i_thr;		// assume its the smallest
      for(int b=0; b < k_eff; b++) { 	// and recompute the actual smallest
	float tmp = act_buff.Un(b, gpidx)->i_thr;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = b;
	}
      }
    }
  }
}

void LeabraLayerSpec::Compute_Inhib_BreakTie(LeabraInhib* thr) {
  if(thr->kwta.k_ithr > 0.0f)
    thr->kwta.ithr_diff = (thr->kwta.k_ithr - thr->kwta.k1_ithr) / thr->kwta.k_ithr;
  else
    thr->kwta.ithr_diff = 0.0f;
  thr->kwta.tie_brk = 0;
  if(tie_brk.on && (thr->kwta.k_ithr > tie_brk.k_thr)) {
    if(thr->kwta.ithr_diff < tie_brk.diff_thr) {
      // we now have an official tie: break it by reducing firing of "others"
      thr->kwta.tie_brk = 1;
      thr->kwta.tie_brk_gain = (tie_brk.diff_thr - thr->kwta.ithr_diff) / tie_brk.diff_thr;
      thr->kwta.k1_ithr *= (1.0f - thr->kwta.tie_brk_gain * tie_brk.thr_gain);
      thr->kwta.eff_loser_gain = 1.0f + (tie_brk.loser_gain * thr->kwta.tie_brk_gain);
    }
  }
}

// actual kwta impls:

void LeabraLayerSpec::Compute_Inhib_kWTA(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits <= 1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_plus_1 = thr->kwta.k + 1;	// expand cutoff to include N+1th one
  k_plus_1 = MIN(nunits, k_plus_1);
  float k1_net = FLT_MAX;
  int k1_idx = 0;

  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  KwtaSortBuff* inact_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::INACT);

  Compute_Inhib_kWTA_Sort(lay, acc_md, gpidx, nunits, thr,
			  *act_buff, *inact_buff, k_plus_1, k1_net, k1_idx);

  // active_buf now has k+1 most active units, get the next-highest one
  int k_idx = -1;
  float net_k = FLT_MAX;
  for(int j=0; j < k_plus_1; j++) {
    float tmp = act_buff->Un(j, gpidx)->i_thr;
    if((tmp < net_k) && (j != k1_idx)) {
      net_k = tmp;		k_idx = j;
    }
  }
  if(k_idx == -1) {		// we didn't find the next one
    k_idx = k1_idx;
    net_k = k1_net;
  }

  LeabraUnit* k1_u = act_buff->Un(k1_idx, gpidx);
  LeabraUnit* k_u =  act_buff->Un(k_idx, gpidx);

  float k1_i = k1_u->i_thr;
  float k_i = k_u->i_thr;
  thr->kwta.k_ithr = k_i;
  thr->kwta.k1_ithr = k1_i;

  Compute_Inhib_BreakTie(thr);

  // place kwta inhibition between k and k+1
  float nw_gi;
  if(inhib.low0)
    nw_gi = ispec.kwta_pt * thr->kwta.k_ithr;
  else
    nw_gi = thr->kwta.k1_ithr + ispec.kwta_pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Avg(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits <=  1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_eff = thr->kwta.k;	// keep cutoff at k
  float k_net = FLT_MAX;
  int k_idx = 0;

  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  KwtaSortBuff* inact_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::INACT);

  Compute_Inhib_kWTA_Sort(lay, acc_md, gpidx, nunits, thr,
			  *act_buff, *inact_buff, k_eff, k_net, k_idx);

  // active_buf now has k most active units, get averages of both groups
  int j;
  float k_avg = 0.0f;
  for(j=0; j < k_eff; j++)
    k_avg += act_buff->Un(j, gpidx)->i_thr;
  k_avg /= (float)k_eff;

  float oth_avg = 0.0f;
  int iabsz = inact_buff->Size(gpidx);
  for(j=0; j < iabsz; j++)
    oth_avg += inact_buff->Un(j, gpidx)->i_thr;
  if(iabsz > 0)
    oth_avg /= (float)iabsz;

  // place kwta inhibition between two averages
  // this uses the adapting point!
  float pt = ispec.kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;

  Compute_Inhib_BreakTie(thr);

  float nw_gi;
  if(inhib.low0)
    nw_gi = pt * thr->kwta.k_ithr;
  else
    nw_gi = thr->kwta.k1_ithr + pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);

  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_kv2k(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits <= 1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_eff = thr->kwta.k;
  float k_net = FLT_MAX;
  int k_idx = 0;

  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  KwtaSortBuff* inact_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::INACT);

  Compute_Inhib_kWTA_Sort(lay, acc_md, gpidx, nunits, thr,
			  *act_buff, *inact_buff, k_eff, k_net, k_idx);

  // active_buf now has k most active units, get average from act buf
  int j;
  float oth_avg = 0.0f;
  float k_avg = 0.0f;
  for(j=0; j < k_eff; j++)
    k_avg += act_buff->Un(j, gpidx)->i_thr;
  k_avg /= (float)k_eff;

  int k2_eff = 2 * thr->kwta.k;
  if(k2_eff >= nunits) {
    // just use inact buf: same as kwta_avg in this case..
    int iabsz = inact_buff->Size(gpidx);
    for(j=0; j < iabsz; j++)
      oth_avg += inact_buff->Un(j, gpidx)->i_thr;
    if(iabsz > 0)
      oth_avg /= (float)iabsz;
  }
  else {
    // find 2k guys
    float k2_net = FLT_MAX;
    int k2_idx = 0;

    KwtaSortBuff* act_buff2k = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE_2K);
    KwtaSortBuff* inact_buff2k = lay->SortBuff(acc_md, KwtaSortBuff_List::INACT_2K);

    Compute_Inhib_kWTA_Sort(lay, acc_md, gpidx, nunits, thr,
			    *act_buff2k, *inact_buff2k, k2_eff, k2_net, k2_idx);

    for(j=0; j < k2_eff; j++)
      oth_avg += act_buff2k->Un(j, gpidx)->i_thr;
    oth_avg /= (float)k2_eff;
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

void LeabraLayerSpec::Compute_Inhib_kWTA_CompCost(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits <=  1) {	// this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_eff = thr->kwta.k;	// keep cutoff at k
  float k_net = FLT_MAX;
  int k_idx = 0;

  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  KwtaSortBuff* inact_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::INACT);

  Compute_Inhib_kWTA_Sort(lay, acc_md, gpidx, nunits, thr,
			  *act_buff, *inact_buff, k_eff, k_net, k_idx);

  LeabraUnit* k_u = act_buff->Un(k_idx, gpidx);
  float k_ithr = k_u->i_thr;

  float oth_sum = 0.0f;
  int iabsz = inact_buff->Size(gpidx);
  for(int j=0; j < iabsz; j++) {
    float oth_nrm = inact_buff->Un(j,gpidx)->i_thr / k_ithr;
    if(oth_nrm >= ispec.comp_thr)
      oth_sum += oth_nrm;
  }
  if(iabsz > 0)
    oth_sum /= (float)iabsz;

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

void LeabraLayerSpec::Compute_Inhib_AvgMaxPt(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
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

void LeabraLayerSpec::Compute_Inhib_Max(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
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

void LeabraLayerSpec::Compute_Inhib_AvgNetAct(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  float pt = ispec.kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  else if((inhib_group != ENTIRE_LAYER) && ((LeabraInhib*)lay != thr))
    pt = ispec.gp_pt;		// use sub-group version for sub-groups..

  float eff_act_gain = ispec.act_gain;
  if(thr->acts_iavg.avg > thr->kwta.pct)
    eff_act_gain *= ispec.kink_gain;

  float nw_gi = thr->netin.avg * ispec.net_gain + thr->acts_iavg.avg * eff_act_gain;
  nw_gi *= pt;			// pt is basic overall multiplier

  thr->i_val.kwta = nw_gi;
  thr->kwta.k_ithr = nw_gi;
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
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.g_i += imod * gpd->i_val.g_i;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
//	Inhibition Stage 2.2: LayInhibToGps

void LeabraLayerSpec::Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork*) {
  if(!lay->unit_groups) return;

  if(inhib_group == ENTIRE_LAYER) {
    // propagate g_i to all subgroups even if doing ENTIRE_LAYER
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.g_i = lay->i_val.g_i;
    }
  }
  else if(inhib_group == UNIT_GROUPS) {
    if(gp_kwta.gp_i) {	// linking groups: get max from layer
      for(int g=0; g < lay->gp_geom.n; g++) {
	LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
	gpd->i_val.gp_g_i = lay->i_val.g_i;
	gpd->i_val.g_i = MAX(gpd->i_val.g_i, lay->i_val.g_i);
      }
    }
  }
  else if(inhib_group == LAY_AND_GPS) {
    // actual inhibition is max of layer and group
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.g_i = MAX(gpd->i_val.g_i, lay->i_val.g_i);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
//	Inhibition Stage 2.3: Apply Inhib

void LeabraLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  if(inhib.type == LeabraInhibSpec::UNIT_INHIB) return; // otherwise overwrites!

  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_ApplyInhib_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  else {
    Compute_ApplyInhib_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ApplyInhib_ugp(LeabraLayer* lay, 
					     Layer::AccessMode acc_md, int gpidx, 
					     LeabraInhib* thr, LeabraNetwork* net)
{
  int nunits = lay->UnitAccess_NUnits(acc_md);
  float inhib_val = thr->i_val.g_i;
  if(thr->kwta.tie_brk == 1) {
    float inhib_thr = thr->kwta.k_ithr;
    float inhib_loser = thr->kwta.eff_loser_gain * thr->i_val.g_i;
    for(int i=0; i<nunits; i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      u->Compute_ApplyInhib_LoserGain(net, inhib_thr, inhib_val, inhib_loser);
    }
  }
  else {
    for(int i=0; i<nunits; i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      u->Compute_ApplyInhib(net, inhib_val);
    }
  }
}

///////////////////////////////////////////////////////////////////////
//	Cycle Step 3: Activation

///////////////////////////////////////////////////////////////////////
//	Cycle Stats

void LeabraLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped) {
    Compute_OutputName(lay, net); // need to keep doing this because network clears it
    return;
  }

  Compute_Acts_AvgMax(lay, net);
  if(inhib.type == LeabraInhibSpec::AVG_NET_ACT)
    Compute_ActsIAvg_AvgMax(lay, net);
  Compute_MaxDa(lay, net);
  Compute_OutputName(lay, net);

  if(lay->un_g_i.cmpt)
    Compute_UnitInhib_AvgMax(lay, net);

  if(lay->HasExtFlag(Unit::TARG)) {
    net->trg_max_act = MAX(net->trg_max_act, lay->acts.max);
  }

  if(lay->Iconified()) {
    lay->icon_value = lay->acts.avg;
  }
}

void LeabraLayerSpec::Compute_AvgMaxVals_ugp(LeabraLayer* lay,
					     Layer::AccessMode acc_md, int gpidx,
					     AvgMaxVals& vals, ta_memb_ptr mb_off) {
  vals.InitVals();
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    float val = *((float*)MemberDef::GetOff_static((void*)u, 0, mb_off));
    vals.UpdtVals(val, i);
  }
  vals.CalcAvg(nunits);
}

void LeabraLayerSpec::Compute_AvgMaxActs_ugp(LeabraLayer* lay,
					     Layer::AccessMode acc_md, int gpidx,
					     LeabraInhib* thr) {
  thr->acts.InitVals();
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    thr->acts.UpdtVals(u->act_eq, i);
  }
  thr->acts.CalcAvg(nunits);

  if(thr->acts_top_k.cmpt) {
    thr->acts_top_k.InitVals();
    int k_eff = thr->kwta.k;	// keep cutoff at k
    KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
    if(k_eff <= 0 || act_buff->Size(gpidx) != k_eff)
      return; // no can do

    for(int j=0; j < k_eff; j++) {
      thr->acts_top_k.UpdtVals(act_buff->Un(j, gpidx)->act_eq, j);
    }
    thr->acts_top_k.CalcAvg(k_eff);
  }
}

void LeabraLayerSpec::Compute_Acts_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts;
//   static ta_memb_ptr mb_off = 0;
//   if(mb_off == 0) {
//     TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
//     TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_eq");
//   }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxActs_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd);
      vals.UpdtFmAvgMax(gpd->acts, nunits, g);
      lay->acts_top_k.UpdtFmAvgMax(gpd->acts_top_k, 1, g); // only compute gp-wise avg for avg top k (n=1 per group)
    }
    vals.CalcAvg(lay->units.leaves);
    lay->acts_top_k.CalcAvg(lay->gp_geom.n);
  }
  else {
    Compute_AvgMaxActs_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay);
  }
}

void LeabraLayerSpec::Compute_ActsIAvg_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts_iavg;
  AvgMaxVals vals_prv;
  vals_prv.CopyFrom(&lay->acts_iavg); // copy
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      AvgMaxVals gp_vals_prv;
      gp_vals_prv.CopyFrom(&gpd->acts_iavg); // copy
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_iavg, mb_off);

      if(gpd->acts_iavg.avg > gp_vals_prv.avg) {
	gpd->acts_iavg.avg = gp_vals_prv.avg + inhib.avg_up_dt * (gpd->acts_iavg.avg - gp_vals_prv.avg);
      }
      else {
	gpd->acts_iavg.avg = gp_vals_prv.avg + inhib.avg_dn_dt * (gpd->acts_iavg.avg - gp_vals_prv.avg);
      }

      vals.UpdtFmAvgMax(gpd->acts_iavg, nunits, g);
    }
    vals.CalcAvg(lay->units.leaves);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }

  if(vals.avg > vals_prv.avg) {
    vals.avg = vals_prv.avg + inhib.avg_up_dt * (vals.avg - vals_prv.avg);
  }
  else {
    vals.avg = vals_prv.avg + inhib.avg_dn_dt * (vals.avg - vals_prv.avg);
  }
}


void LeabraLayerSpec::Compute_MaxDa_ugp(LeabraLayer* lay,
					Layer::AccessMode acc_md, int gpidx,
					LeabraInhib* thr, LeabraNetwork* net) {
  thr->maxda = 0.0f;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    float fda = u->Compute_MaxDa(net);
    lay->maxda = MAX(fda, lay->maxda);
    thr->maxda = MAX(fda, thr->maxda);
    net->maxda = MAX(fda, net->maxda);
  }
}

void LeabraLayerSpec::Compute_MaxDa(LeabraLayer* lay, LeabraNetwork* net) {
  lay->maxda = 0.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_MaxDa_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  else {
    Compute_MaxDa_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_OutputName_ugp(LeabraLayer* lay, 
					     Layer::AccessMode acc_md, int gpidx,
					     LeabraInhib* thr, LeabraNetwork* net) {
  String* onm;
  if(lay->unit_groups)
    onm = &(lay->gp_output_names.FastEl_Flat(gpidx));
  else
    onm = &(lay->output_name);

  if(thr->acts.max_i < 0) {
    *onm = "n/a";
    return;
  }
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, thr->acts.max_i, gpidx);
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
  if(lay->unit_groups) {
    lay->output_name = "";
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_OutputName_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  else {
    Compute_OutputName_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_UnitInhib_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->un_g_i;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "gc.i");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->un_g_i, mb_off);
      vals.UpdtFmAvgMax(gpd->un_g_i, nunits, g);
    }
    vals.CalcAvg(lay->units.leaves);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

///////////////////////////////////////////////////////////////////////
//	Cycle Stats -- optional non-default guys

float LeabraLayerSpec::Compute_TopKAvgAct_ugp(LeabraLayer* lay, 
					      Layer::AccessMode acc_md, int gpidx,
					      LeabraInhib* thr, LeabraNetwork*) {
  int k_eff = thr->kwta.k;	// keep cutoff at k
  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  if(TestError(k_eff <= 0 || act_buff->Size(gpidx) != k_eff, "Compute_TopKAvgAct_ugp",
	       "Only usable when using a kwta function -- kwta sort buff was not set properly!")) {
    return -1;
  }

  float k_avg = 0.0f;
  for(int j=0; j < k_eff; j++)
    k_avg += act_buff->Un(j, gpidx)->act_eq;
  k_avg /= (float)k_eff;

  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgAct(LeabraLayer* lay, LeabraNetwork* net) {
  float k_avg = 0.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      k_avg += Compute_TopKAvgAct_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
    if(lay->unit_groups)
      k_avg /= (float)lay->gp_geom.n;
  }
  else {
    k_avg = Compute_TopKAvgAct_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgNetin_ugp(LeabraLayer* lay,
					      Layer::AccessMode acc_md, int gpidx,
					      LeabraInhib* thr, LeabraNetwork*) {
  int k_eff = thr->kwta.k;	// keep cutoff at k
  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  if(TestError(k_eff <= 0 || act_buff->Size(gpidx) != k_eff, "Compute_TopKAvgNetin_ugp",
	       "Only usable when using a kwta function -- kwta sort buff was not set properly!")) {
    return -1;
  }

  float k_avg = 0.0f;
  for(int j=0; j < k_eff; j++)
    k_avg += act_buff->Un(j, gpidx)->net;
  k_avg /= (float)k_eff;

  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgNetin(LeabraLayer* lay, LeabraNetwork* net) {
  float k_avg = 0.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      k_avg += Compute_TopKAvgNetin_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
    if(lay->unit_groups)
      k_avg /= (float)lay->gp_geom.n;
  }
  else {
    k_avg = Compute_TopKAvgNetin_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
  return k_avg;
}

///////////////////////////////////////////////////////////////////////
//	Cycle Optional Misc

void LeabraLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // just snapshot the activation state
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_MidMinus(net);
  }
}

///////////////////////////////////////////////////////////////////////
//	SettleFinal

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

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->PostSettle(net);

  if((adapt_i.type == AdaptISpec::G_BAR_I) || (adapt_i.type == AdaptISpec::G_BAR_IL)) {
    AdaptGBarI(lay, net);
  }
}

void LeabraLayerSpec::PostSettle_GetMinus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m = lay->acts;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_m = gpd->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetPlus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_p = lay->acts;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_p = gpd->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetMinus2(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m2 = lay->acts;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_m2 = gpd->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetPlus2(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_p2 = lay->acts;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_p2 = gpd->acts;
    }
  }
}
void LeabraLayerSpec::PostSettle_GetPhaseDifRatio(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->acts_p.avg > 0.0f)
    lay->phase_dif_ratio = lay->acts_m.avg / lay->acts_p.avg;
  else
    lay->phase_dif_ratio = 1.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      if(gpd->acts_p.avg > 0.0f)
	gpd->phase_dif_ratio = gpd->acts_m.avg / gpd->acts_p.avg;
      else
	gpd->phase_dif_ratio = 1.0f;
    }
  }
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

/////////////////////////////////////////////////////////
// NOTE: the following are not currently used..

void LeabraLayerSpec::Compute_ActM_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts_m;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_m");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_m, mb_off);
      vals.UpdtFmAvgMax(gpd->acts_m, nunits, g);
    }
    vals.CalcAvg(lay->units.leaves);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

void LeabraLayerSpec::Compute_ActP_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts_p;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_p");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_p, mb_off);
      vals.UpdtFmAvgMax(gpd->acts_p, nunits, g);
    }
    vals.CalcAvg(lay->units.leaves);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

///////////////////////////////////////////////////////////////////////
//	TrialFinal

// todo: could probably package these at unit level...

void LeabraLayerSpec::EncodeState(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->EncodeState(net);
}

void LeabraLayerSpec::Compute_SelfReg_Trial(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_SelfReg_Trial(net);
}


///////////////////////////////////////////////////////////////////////
//	Learning

bool LeabraLayerSpec::Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return true;
}

bool LeabraLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->phase_order == LeabraNetwork::MINUS_PLUS_NOTHING ||
      net->phase_order == LeabraNetwork::MINUS_PLUS_MINUS) && 
     (net->learn_rule != LeabraNetwork::LEABRA_CHL &&
      net->learn_rule != LeabraNetwork::CTLEABRA_XCAL_C)) { // xcal_c learns on 1st plus!
    return false;
  }
  return true;
}

bool LeabraLayerSpec::Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return false;		// standard layers never learn here
}

bool LeabraLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  // shouldn't happen, but just in case..
  if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL_C) return false; // only 1st plus
  return true; 		// all types learn here..
}

///////////////////////////////////////////////////////////////////////
//	Trial-level Stats

float LeabraLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, 
				   int& n_vals, bool unit_avg, bool sqrt) {
  return lay->Layer::Compute_SSE(net, n_vals, unit_avg, sqrt);
}

float LeabraLayerSpec::Compute_NormErr_ugp(LeabraLayer* lay,
					   Layer::AccessMode acc_md, int gpidx,
					   LeabraInhib* thr, LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  float nerr = 0.0f;
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    nerr += u->Compute_NormErr(net);
  }
  return nerr;
}

float LeabraLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;					 // assume not contributing
  if(!lay->HasExtFlag(Unit::TARG | Unit::COMP)) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  int ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      if(net->on_errs && net->off_errs)
	ntot += 2 * gpd->kwta.k;
      else
	ntot += gpd->kwta.k;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    if(net->on_errs && net->off_errs)
      ntot += 2 * lay->kwta.k;
    else
      ntot += lay->kwta.k;
  }
  if(ntot == 0) return -1.0f;

  lay->norm_err = nerr / (float)ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;		// no contributarse

  return lay->norm_err;
}

////////////////////////////////////////////////////////////////////////////////
//	Parameter Adaptation over longer timesales

void LeabraLayerSpec::AdaptKWTAPt_ugp(LeabraLayer* lay,
				      Layer::AccessMode acc_md, int gpidx,
				      LeabraInhib* thr, LeabraNetwork* net) {
  thr->adapt_i.avg_avg += adapt_i.a_dt * (thr->acts_m.avg - thr->adapt_i.avg_avg);
  float dif = thr->adapt_i.avg_avg - thr->kwta.pct;
  if(dif < -adapt_i.tol) {	// average is less than target
    // so reduce the point towards lower value
    thr->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((inhib.kwta_pt - adapt_i.mx_d) - thr->adapt_i.i_kwta_pt);
  }
  else if(dif > adapt_i.tol) {	// average is more than target
    // so increase point towards higher value
    thr->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((inhib.kwta_pt + adapt_i.mx_d) - thr->adapt_i.i_kwta_pt);
  }
}

void LeabraLayerSpec::AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->HasExtFlag(Unit::EXT) && !lay->HasExtFlag(Unit::TARG))
    return;			// don't adapt points for input-only layers
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      AdaptKWTAPt_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  AdaptKWTAPt_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
}

void LeabraLayerSpec::Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->netin.max < 0.01f) return; // not getting enough activation to count!

  lay->avg_netin_sum.avg += lay->netin.avg;
  lay->avg_netin_sum.max += lay->netin.max;
  lay->avg_netin_n++;

  float sum_net = 0.0f;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
    prjn->netin_avg = 0.0f;
    int netin_avg_n = 0;
    LeabraUnit* u;
    taLeafItr ui;
    FOR_ITR_EL(LeabraUnit, u, lay->units., ui) {
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      if(u->act_eq < us->opt_thresh.send) continue; // ignore if not above sending thr
      LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(prjn->recv_idx);
      if(!cg) continue;
      float netin = cg->Compute_Netin(u);
      cg->net = netin;
      prjn->netin_avg += netin;
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
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
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
    if(prjn->off || !prjn->from || prjn->from->lesioned()) {
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
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
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
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
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
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    cs->SetUnique("wt_scale", true);
    cs->wt_scale.abs += abs_net_adapt.abs_lrate * dst;
  }
}

///////////////////////////////////////////////////////////////////////
//			Leabra Layer

void LeabraLayer::CheckSpecs() {
  spec.CheckSpec();
  inherited::CheckSpecs();
}

void LeabraLayer::CheckThisConfig_impl(bool quiet, bool& rval) {
  if(lesioned()) return;
  inherited::CheckThisConfig_impl(quiet, rval);

  if(!spec->CheckConfig_Layer(this, quiet)) {
    rval = false;
  }
}


//////////////////////////
// 	LeabraUnGpData	//
//////////////////////////
  
void LeabraUnGpData::Initialize() {
  Inhib_Initialize();
}

void LeabraUnGpData::InitLinks() {
  inherited::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(netin_top_k, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);
  taBase::Own(acts_iavg, this);
  taBase::Own(acts_top_k, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);
  taBase::Own(acts_p2, this);
  taBase::Own(acts_m2, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);
}

void LeabraUnGpData::Copy_(const LeabraUnGpData& cp) {
  Inhib_Copy_(cp);
}

void LeabraUnGpData::Init_State() {
  // nop
}

////////////////////////////////////////////////////
// 	LeabraLayer  Misc Objs

void AvgMaxVals::Initialize() {
  cmpt = true;
  avg = max = 0.0f; max_i = -1;
}

void AvgMaxVals::Copy_(const AvgMaxVals& cp) {
  cmpt = cp.cmpt;
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
  tie_brk_gain = 0.0f;
  eff_loser_gain = 1.0f;
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
  tie_brk_gain = cp.tie_brk_gain;
  eff_loser_gain = cp.eff_loser_gain;
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

void KwtaSortBuff::Initialize() {
}

void KwtaSortBuff::InitLinks() {
  inherited::InitLinks();
  taBase::Own(kbuff, this);
  taBase::Own(sizes, this);
}

void KwtaSortBuff::CutLinks() {
  kbuff.CutLinks();
  sizes.CutLinks();
  inherited::CutLinks();
}

void KwtaSortBuff::Copy_(const KwtaSortBuff& cp) {
  kbuff = cp.kbuff;
  sizes = cp.sizes;
}

void KwtaSortBuff_List::ResetAllBuffs() {
  for(int i=0; i < size; i++) {
    FastEl(i)->ResetAll();
  }
}

void KwtaSortBuff_List::AllocAllBuffs(int nunits, int ngps) {
  for(int i=0; i < size; i++) {
    FastEl(i)->Alloc(nunits, ngps);
  }
}

void KwtaSortBuff_List::CreateStdBuffs() {
  SetSize(N_BUFFS);
}

void LeabraInhib::Inhib_Initialize() {
  kwta.k = 1;
  kwta.pct = .25;
  kwta.pct_c = .75;
  i_val.Init();
  phase_dif_ratio = 1.0f;
  maxda = 0.0f;
  un_g_i.cmpt = false;		// don't compute by default
}

void LeabraInhib::Inhib_Init_Acts(LeabraLayerSpec*) {
  i_val.Init();
  netin.InitVals();
  i_thrs.InitVals();
  acts.InitVals();
  acts_iavg.InitVals();
  un_g_i.InitVals();
  maxda = 0.0f;
}

/////////////////////////////////////////////////////////
//		LeabraLayer


void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  projections.SetBaseType(&TA_LeabraPrjn);
  send_prjns.SetBaseType(&TA_LeabraPrjn);
  units.SetBaseType(&TA_LeabraUnit);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Inhib_Initialize();
  hard_clamped = false;
  dav = 0.0f;
  norm_err = 0.0f;
  da_updt = false;

  avg_netin_n = 0;
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}  

void LeabraLayer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(netin_top_k, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);
  taBase::Own(acts_iavg, this);
  taBase::Own(acts_top_k, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);
  taBase::Own(acts_p2, this);
  taBase::Own(acts_m2, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);

  taBase::Own(avg_netin, this);
  taBase::Own(avg_netin_sum, this);

  taBase::Own(ungp_data, this);

  taBase::Own(unit_idxs, this);

  taBase::Own(lay_kbuffs, this);
  taBase::Own(gp_kbuffs, this);

  lay_kbuffs.CreateStdBuffs();
  gp_kbuffs.CreateStdBuffs();

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

  lay_kbuffs.CutLinks();
  gp_kbuffs.CutLinks();
}

void LeabraInhib::Inhib_Copy_(const LeabraInhib& cp) {
  netin = cp.netin;
  i_thrs = cp.i_thrs;
  acts = cp.acts;
  acts_iavg = cp.acts_iavg;
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
  hard_clamped = cp.hard_clamped;
  dav = cp.dav;
  norm_err = cp.norm_err;

  // this will update spec pointer to new network if we are copied from other guy
  // only if the network is not otherwise already copying too!!
  // (other pointers are already dealt with in Layer)
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Network);
}

void LeabraLayer::ResetSortBuf() {
  lay_kbuffs.ResetAllBuffs();
  gp_kbuffs.ResetAllBuffs();
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

void LeabraLayer::BuildKwtaBuffs() {
  ResetSortBuf();
  lay_kbuffs.AllocAllBuffs(flat_geom.n, 1);
  gp_kbuffs.AllocAllBuffs(un_geom.n, gp_geom.n);

  ungp_data.Reset();
  ungp_data.SetBaseType(((LeabraLayerSpec*)GetLayerSpec())->UnGpDataType());
  ungp_data.SetSize(gp_geom.n);
}

void LeabraLayer::BuildUnits() {
  ResetSortBuf();
  inherited::BuildUnits();
  BuildKwtaBuffs();
}

// void LeabraLayer::TriggerContextUpdate() -- in leabra_extra.cpp

#ifdef DMEM_COMPILE
void LeabraLayer::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraLayer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif


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
  n_avg_only_epcs = 0;

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
  force_con = false;
}

void CtSRAvgVals::Initialize() {
  s_sum = 0.0f;
  s_nrm = 1.0f;
  m_sum = 0.0f;
  m_nrm = 1.0f;
  do_s = false;
}

void CtSineInhibMod::Initialize() {
  start = 30;
  duration = 20;
  n_pi = 2.0f;
  burst_i = 0.0f;
  trough_i = 0.0f;
}

void CtFinalInhibMod::Initialize() {
  start = 20;
  end = 25;
  inhib_i = 0.0f;
}

void CtLrnTrigSpec::Initialize() {
  plus_lrn_cyc = -1;
  davg_dt = 0.1f;
  davg_s_dt = 0.05f;
  davg_m_dt = 0.03f;
  davg_l_dt = 0.0005f;
  thr_min = 0.0f;
  thr_max = 0.5f;
  loc_max_cyc = 8;
  loc_max_dec = 0.01f;
  lrn_delay = 40;
  lrn_refract = 100;
  davg_l_init = 0.0f;
  davg_max_init = 0.001f;

  davg_time = 1.0f / davg_dt;
  davg_s_time = 1.0f / davg_s_dt;
  davg_m_time = 1.0f / davg_m_dt;
  davg_l_time = 1.0f / davg_l_dt;

  lrn_delay_inc = 1.0f / MAX(1.0f, (float)lrn_delay);
  lrn_refract_inc = 1.0f / MAX(1.0f, (float)lrn_refract);
}

void CtLrnTrigSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  
  davg_time = 1.0f / davg_dt;
  davg_s_time = 1.0f / davg_s_dt;
  davg_m_time = 1.0f / davg_m_dt;
  davg_l_time = 1.0f / davg_l_dt;

  lrn_delay_inc = 1.0f / MAX(1.0f, (float)lrn_delay);
  lrn_refract_inc = 1.0f / MAX(1.0f, (float)lrn_refract);
}

void CtLrnTrigVals::Initialize() {
  davg = 0.0f;
  davg_s = 0.0f;
  davg_m = 0.0f;
  davg_smd = 0.0f;
  davg_l = 0.0f;
  davg_max = 0.001f;
  cyc_fm_inc = 0;
  cyc_fm_dec = 0;
  loc_max = 0.001f;
  lrn_max = 0.0f;
  lrn_trig = 0.0f;
  lrn = 0;

  Init_Stats();
}

void CtLrnTrigVals::Init_Stats() {
  lrn_min = 0.0f;
  lrn_min_cyc = 0.0f;
  lrn_min_thr = 0.0f;

  lrn_plus = 0.0f;
  lrn_plus_cyc = 0.0f;
  lrn_plus_thr = 0.0f;

  lrn_noth = 0.0f;
  lrn_noth_cyc = 0.0f;
  lrn_noth_thr = 0.0f;

  Init_Stats_Sums();
}

void CtLrnTrigVals::Init_Stats_Sums() {
  lrn_stats_n = 0;

  lrn_min_sum = 0.0f;
  lrn_min_cyc_sum = 0.0f;
  lrn_min_thr_sum = 0.0f;

  lrn_plus_sum = 0.0f;
  lrn_plus_cyc_sum = 0.0f;
  lrn_plus_thr_sum = 0.0f;

  lrn_noth_sum = 0.0f;
  lrn_noth_cyc_sum = 0.0f;
  lrn_noth_thr_sum = 0.0f;
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

  learn_rule = LEABRA_CHL;
  prv_learn_rule = -1;
  phase_order = MINUS_PLUS;
  no_plus_test = true;
  sequence_init = DO_NOTHING;
  phase = MINUS_PHASE;
  nothing_phase = false;
  phase_no = 0;
  phase_max = 2;

  ct_cycle = 0;
  time_inc = 1.0f;		// just a simple counter by default

  cycle_max = 60;
  mid_minus_cycle = -1;
  min_cycles = 15;
  min_cycles_phase2 = 35;

  thread_flags = TF_ALL;

  minus_cycles = 0.0f;
  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;

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
  ext_rew_avail = false;
  norew_val = 0.5f;
  avg_ext_rew = 0.0f;
  pvlv_pvi = 0.0f;
  pvlv_pvr = 0.0f;
  pvlv_lve = 0.0f;
  pvlv_lvi = 0.0f;
  pv_detected = false;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;

  on_errs = true;
  off_errs = true;

  norm_err = 0.0f;
  avg_norm_err = 1.0f;
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;
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
  ct_time.UpdateAfterEdit_NoGui();
  ct_lrn_trig.UpdateAfterEdit_NoGui();

  if(TestWarning(ct_sravg.plus_s_st >= ct_time.plus, "UAE",
	       "ct_sravg.plus_s_st is higher than ct_time.plus (# of cycles in plus phase)"
	       "just set it to plus-2")) {
    ct_sravg.plus_s_st = ct_time.plus -2;
  }

//   if(TestWarning(learn_rule != CTLEABRA_CAL && ct_sravg.interval == 5, "UAE",
//  	       "ct_sravg.interval should be 1 for all algorithms *EXCEPT* CTLEABRA_CAL -- I just set it to 1 for you.  Also, while I'm at it, I set n_avg_only_epcs = 0 as well, because that is the new default")) {
//     ct_sravg.interval = 1;
//     ct_time.n_avg_only_epcs = 0;
//   }
//   if(TestWarning(learn_rule == CTLEABRA_CAL && ct_sravg.interval == 1, "UAE",
// 	       "ct_sravg.interval should be > 1 for CTLEABRA_CAL -- I just set it to 5 for you")) {
//     ct_sravg.interval = 5;
//   }

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

///////////////////////////////////////////////////////////////////////
//	General Init functions

void LeabraNetwork::Init_Acts() {
  inherited::Init_Acts();
  send_inhib_tmp.InitVals(0.0f);
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
  ext_rew_avail = false;
  norew_val = 0.5f;
  avg_ext_rew = 0.0f;
  pvlv_pvi = 0.0f;
  pvlv_pvr = 0.0f;
  pvlv_lve = 0.0f;
  pvlv_lvi = 0.0f;
  pv_detected = false;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;

  norm_err = 0.0f;
  avg_norm_err = 1.0f;
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;

  lrn_trig.Init_Stats();
}

void LeabraNetwork::Init_Sequence() {
  inherited::Init_Sequence();
  if(sequence_init == INIT_STATE) {
    Init_Acts();
  }
  else if(sequence_init == DECAY_STATE) {
    Trial_DecayState();
  }
}

void LeabraNetwork::Init_Weights() {
  inherited::Init_Weights();
  sravg_vals.InitVals();

  lrn_trig.davg = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_s = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_m = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_smd = 0.0f;
  lrn_trig.davg_l = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_max = ct_lrn_trig.davg_max_init;

  lrn_trig.cyc_fm_inc = 0;
  lrn_trig.cyc_fm_dec = 0;
  lrn_trig.loc_max = 0.0f;
  lrn_trig.lrn_max = 0.0f;

  lrn_trig.lrn_trig = 0.0f;
  lrn_trig.lrn = 0.0f;
}

void LeabraNetwork::DecayState(float decay) {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayState(this, decay);
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
//     if(phase_order == MINUS_PLUS) {
//       phase_order = MINUS_PLUS_NOTHING;
//     }

    if(learn_rule == CTLEABRA_CAL) {
      ct_sravg.interval = 5;
    }
    else {
      ct_sravg.interval = 1;
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
    lay->SetLearnRule(this);
  }
}

void LeabraNetwork::CheckInhibCons() {
  inhib_cons_used = false;
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->CheckInhibCons(this);
  }
}

void LeabraNetwork::BuildUnits_Threads() {
  inherited::BuildUnits_Threads();
  // temporary storage for sender-based netinput computation
  CheckInhibCons();
  if(inhib_cons_used && units_flat.size > 0 && threads.n_threads > 0) {
    send_inhib_tmp.SetGeom(2, units_flat.size, threads.n_threads);
    send_inhib_tmp.InitVals(0.0f);
  }
}

///////////////////////////////////////////////////////////////////////
//	TrialInit -- at start of trial

void LeabraNetwork::Trial_Init() {
  cycle = -1;
  Trial_Init_Phases();
  SetCurLrate();

  Trial_Init_Unit(); // performs following at unit-level 
//   Trial_DecayState();
  Trial_NoiseInit();		// run for kpos case
//   Trial_Init_SRAvg();
  Trial_Init_Layer();
}

void LeabraNetwork::Trial_Init_Phases() {
  phase = MINUS_PHASE;
  nothing_phase = false;
  phase_no = 0;

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
}

void LeabraNetwork::SetCurLrate() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->SetCurLrate(this, epoch);
  }
}

void LeabraNetwork::Trial_Init_Unit() {
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Trial_Init_Unit);
  if(thread_flags & TRIAL_INIT)
    threads.Run(&un_call, .2f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  sravg_vals.InitVals();	// reset sravg vals, after Trial_Init_SRAvg!
}

void LeabraNetwork::Trial_NoiseInit() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Trial_NoiseInit(this);
  }
}

void LeabraNetwork::Trial_DecayState() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Trial_DecayState(this);
  }
}

void LeabraNetwork::Trial_Init_SRAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Trial_Init_SRAvg(this);
  }
  sravg_vals.InitVals();	// reset sravg vals, after Trial_Init_SRAvg!
}
  
void LeabraNetwork::Trial_Init_Layer() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Trial_Init_Layer(this);
  }
}


///////////////////////////////////////////////////////////////////////
//	SettleInit -- at start of settling

void LeabraNetwork::Settle_Init() {
  int tmp_cycle = cycle;
  cycle = -2;			// special signal for settle init

  Settle_Init_CtTimes();
  Compute_Active_K();		// compute here because could depend on pat_n

  Settle_Init_Unit();		// do chunk of following unit-level functions:

//   Settle_Init_TargFlags();
//   Settle_DecayState();
//   Compute_NetinScale();		// compute net scaling

  Compute_HardClamp();		// clamp all hard-clamped input acts: not easily threadable
  cycle = tmp_cycle;
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

void LeabraNetwork::Compute_Active_K() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Active_K(this);	// this gets done at the outset..
  }
}
  
void LeabraNetwork::Settle_Init_Unit() {
  if(nothing_phase) {
    if(phase_order != MINUS_PLUS_MINUS && phase_order != MINUS_PLUS_PLUS_MINUS) {
      TargExtToComp();		// all external input is now 'comparison' = nothing!
    }
  }

  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Settle_Init_Unit);
  if(thread_flags & SETTLE_INIT)
    threads.Run(&un_call, .1f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  Settle_Init_Layer();

  Compute_NetinScale_Senders();	// second phase after recv-based NetinScale
  // put it after Settle_Init_Layer to allow for mods to netin scale in that guy..
}

void LeabraNetwork::Settle_Init_Layer() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Settle_Init_Layer(this);
  }
}

void LeabraNetwork::Settle_Init_TargFlags() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  if(nothing_phase) {
    if(phase_order != MINUS_PLUS_MINUS && phase_order != MINUS_PLUS_PLUS_MINUS) {
      TargExtToComp();		// all external input is now 'comparison' = nothing!
    }
  }

  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Settle_Init_TargFlags(this);
  }
}

void LeabraNetwork::Settle_DecayState() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Settle_DecayState);
  threads.Run(&un_call, .1f);	// lowest number -- not for real use so doesn't matter
}

void LeabraNetwork::Compute_NetinScale() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_NetinScale);
  threads.Run(&un_call, .1f);	// lowest number -- not for real use so doesn't matter
}

void LeabraNetwork::Compute_NetinScale_Senders() {
  // NOTE: this IS called by default -- second phase of Settle_Init_Unit
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_NetinScale_Senders);
  threads.Run(&un_call, .1f);	// lowest number -- minimal computation
}

void LeabraNetwork::Compute_HardClamp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_HardClamp(this);
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

void LeabraNetwork::NewInputData_Init() {
  Settle_Init_Layer();
  Settle_Init_TargFlags();
  Compute_HardClamp();
}

////////////////////////////////////////////////////////////////
//	Cycle_Run

void LeabraNetwork::Cycle_Run() {
  // ct_cycle is pretty useful anyway
  if(phase_no == 0 && cycle == 0) // detect start of trial
    ct_cycle = 0;

  Send_Netin();
  Compute_ExtraNetin();		// layer level extra netinput for special algorithms
  Compute_NetinInteg();
  Compute_NetinStats();

  Compute_Inhib();
  Compute_ApplyInhib();

  Compute_Act();
  Compute_CycleStats();

  Compute_CycSynDep();

  Compute_SRAvg();		// note: only ctleabra variants do con-level compute here
  Compute_XCalC_dWt();
  Compute_MidMinus();		// check for mid-minus and run if so (PBWM)

  ct_cycle++;
  time += time_inc;			// always increment time..
}

///////////////////////////////////////////////////////
//	Cycle Stage 1: netinput

void LeabraNetwork::Send_Netin() {
  // always use delta mode!
  send_pct_n = send_pct_tot = 0;
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Send_NetinDelta);
  if(thread_flags & NETIN)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  // now need to roll up the netinput into unit vals
  const int nu = units_flat.size;
  const int nt = threads.tasks.size;
  if(threads.using_threads) {
    if(inhib_cons_used) {
      for(int i=1;i<nu;i++) {	// 0 = dummy idx
	LeabraUnit* un = (LeabraUnit*)units_flat[i];
	float nw_nt = 0.0f;
	float nw_inhb = 0.0f;
	for(int j=0;j<nt;j++) {
	  nw_nt += send_netin_tmp.FastEl(i, j);
	  nw_inhb += send_inhib_tmp.FastEl(i, j);
	}
	un->net_delta = nw_nt;
	un->g_i_delta = nw_inhb;
      }
      send_inhib_tmp.InitVals(0.0f); // reset for next time around
    }
    else {
      for(int i=1;i<nu;i++) {	// 0 = dummy idx
	LeabraUnit* un = (LeabraUnit*)units_flat[i];
	float nw_nt = 0.0f;
	for(int j=0;j<nt;j++) {
	  nw_nt += send_netin_tmp.FastEl(i, j);
	}
	un->net_delta = nw_nt;
      }
    }
    send_netin_tmp.InitVals(0.0f); // reset for next time around
  }

#ifdef DMEM_COMPILE
  dmem_share_units.Sync(3);
#endif
  if(send_pct_tot > 0) {	// only avail for non-threaded calls
    send_pct = (float)send_pct_n / (float)send_pct_tot;
    avg_send_pct_sum += send_pct;
    avg_send_pct_n++;
  }
}

void LeabraNetwork::Compute_ExtraNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ExtraNetin(this);
  }
}

void LeabraNetwork::Compute_NetinInteg() {
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_NetinInteg);
  if(thread_flags & NETIN_INTEG)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_NetinStats() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_NetinStats(this);
  }
}

///////////////////////////////////////////////////////////////////////
//	Cycle Step 2: Inhibition

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

	  if(lay->unit_groups) {
	    lay->Compute_LayInhibToGps(this);
	  }
	}
      }
    }
  }
}

void LeabraNetwork::Compute_ApplyInhib() {
  // this is not threadable due to interactions with inhib vals at layer level
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ApplyInhib(this);
  }
}

///////////////////////////////////////////////////////////////////////
//	Cycle Step 3: Activation

void LeabraNetwork::Compute_Act() {
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_Act);
  if(thread_flags & ACT)
    threads.Run(&un_call, 0.4f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

///////////////////////////////////////////////////////////////////////
//	Cycle Stats

void LeabraNetwork::Compute_CycleStats() {
  // stats are never threadable due to updating at higher levels
  output_name = "";		// this will be updated by layer
  maxda = 0.0f;		// initialize
  trg_max_act = 0.0f;

  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_CycleStats(this);
  }
}

///////////////////////////////////////////////////////////////////////
//	Cycle Optional Misc

void LeabraNetwork::Compute_CycSynDep() {
  if(!net_misc.cyc_syn_dep) return;
  if(ct_cycle % net_misc.syn_dep_int != 0) return;

  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_CycSynDep);
  threads.Run(&un_call, 0.6f); // todo: this # is an estimate -- not tested yet -- no flag for it 
}

void LeabraNetwork::Compute_MidMinus() {
  if(mid_minus_cycle <= 0) return;
  if(ct_cycle == mid_minus_cycle) {
    LeabraLayer* lay;
    taLeafItr l;
    FOR_ITR_EL(LeabraLayer, lay, layers., l) {
      if(lay->lesioned())	continue;
      lay->Compute_MidMinus(this);
    }
    // todo: this is for ff_bal -- probably want to make this conditional somehow..
    Compute_NetinScale_Senders();	// second phase after recv-based NetinScale
  }
}
  
///////////////////////////////////////////////////////////////////////
//	Settle Final

void LeabraNetwork::Settle_Final() {
  // all weight changes take place here for consistency!
  PostSettle();
  Settle_Compute_dWt();
}

void LeabraNetwork::PostSettle() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->PostSettle(this);
  }
}

void LeabraNetwork::Settle_Compute_dWt() {
  if(train_mode == TEST)
    return;

  switch(phase_order) {
  case MINUS_PLUS:
  case PLUS_MINUS:
  case PLUS_NOTHING:
    if(phase_no == 1) {
      Compute_dWt_FirstPlus();
      AdaptKWTAPt();
    }
    break;
  case PLUS_ONLY:
    Compute_dWt_FirstPlus();
    AdaptKWTAPt();
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    if(phase_no == 1)
      Compute_dWt_FirstPlus();
    else if(phase_no == 2) {
      Compute_dWt_Nothing();
      AdaptKWTAPt();
    }
    break;
  case MINUS_PLUS_PLUS:
    if(phase_no == 1)
      Compute_dWt_FirstPlus();
    else if(phase_no == 2) {
      Compute_dWt_SecondPlus();
      AdaptKWTAPt();
    }
    break;
  case MINUS_PLUS_PLUS_NOTHING:
  case MINUS_PLUS_PLUS_MINUS:
    if(phase_no == 1)
      Compute_dWt_FirstPlus();
    else if(phase_no == 2)
      Compute_dWt_SecondPlus();
    else if(phase_no == 3) {
      Compute_dWt_Nothing();
      AdaptKWTAPt();
    }
    break;
  }
}

void LeabraNetwork::AdaptKWTAPt() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->AdaptKWTAPt(this);
  }
}

 
///////////////////////////////////////////////////////////////////////
//	Trial Update and Final

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

///////////////////////////////////////////////////////////////////////
//	Learning

bool LeabraNetwork::Compute_SRAvg_Now() {
  int eff_int = ct_sravg.interval;
  if(phase == LeabraNetwork::PLUS_PHASE && cycle >= ct_sravg.plus_s_st) {
    if((ct_time.plus - ct_sravg.plus_s_st) < eff_int) {
      eff_int = 1;		// make sure you get short-time/plus phase info!
    }
  }
  if((ct_cycle >= ct_sravg.start) &&
     (ct_cycle < (ct_time.inhib_start + ct_sravg.end)) &&
     ((ct_cycle - ct_sravg.start) % eff_int == 0)) {
    return true;
  }
  return false;
}
  
void LeabraNetwork::Compute_SRAvg() {
  if(!Compute_SRAvg_Now()) return;

  if(phase == LeabraNetwork::PLUS_PHASE && cycle >= ct_sravg.plus_s_st)
    sravg_vals.do_s = true;
  else
    sravg_vals.do_s = false;

  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_SRAvg);
  if(thread_flags & SRAVG)
    threads.Run(&un_call, 0.9f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  sravg_vals.m_sum += 1.0f;	// normal weighting
  if(sravg_vals.do_s)
    sravg_vals.s_sum += 1.0f;
}
  
void LeabraNetwork::Compute_dWt_SRAvg() {
  if(sravg_vals.m_sum > 0.0f)
    sravg_vals.m_nrm = 1.0f / sravg_vals.m_sum;
  else
    sravg_vals.m_nrm = 1.0f;
  if(sravg_vals.s_sum > 0.0f) 
    sravg_vals.s_nrm = 1.0f / sravg_vals.s_sum;
  else
    sravg_vals.s_nrm = 1.0f;
}

void LeabraNetwork::Compute_XCalC_dWt() {
  if(learn_rule != CTLEABRA_XCAL_C) return;

  bool do_lrn = false;

  float tmp_davg = 0.0f;
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    tmp_davg += fabsf(un->davg);
  }
  lrn_trig.davg = tmp_davg / (float)units_flat.size;
  lrn_trig.davg_s += ct_lrn_trig.davg_s_dt * (lrn_trig.davg - lrn_trig.davg_s);
  lrn_trig.davg_m += ct_lrn_trig.davg_m_dt * (lrn_trig.davg_s - lrn_trig.davg_m);
  float nw_smd = lrn_trig.davg_s - lrn_trig.davg_m;
  float d_smd = nw_smd - lrn_trig.davg_smd;
  lrn_trig.davg_smd = nw_smd;

  lrn_trig.davg_l += ct_lrn_trig.davg_l_dt * (lrn_trig.davg_smd - lrn_trig.davg_l);
  if(lrn_trig.davg_smd >= lrn_trig.davg_max) {
    lrn_trig.davg_max = lrn_trig.davg_smd;
  }
  else {
    lrn_trig.davg_max += ct_lrn_trig.davg_l_dt * (lrn_trig.davg_l - lrn_trig.davg_max);
  }

  if(d_smd > 0.0f) {
    lrn_trig.loc_max = lrn_trig.davg_smd; // indicate local max
    lrn_trig.cyc_fm_inc = 0;
    lrn_trig.cyc_fm_dec++;
  }
  else {
    lrn_trig.cyc_fm_dec = 0;
    lrn_trig.cyc_fm_inc++;
  }

  float maxldif = lrn_trig.davg_max - lrn_trig.davg_l;
  float thr_min_eff = lrn_trig.davg_l +  ct_lrn_trig.thr_min * maxldif;
  float thr_max_eff = lrn_trig.davg_l +  ct_lrn_trig.thr_max * maxldif;

  float loc_dec_norm = (lrn_trig.loc_max - lrn_trig.davg_smd) / maxldif;

  if(lrn_trig.lrn_trig > 0.0f) {	// already met thresh, counting up to learn time
    lrn_trig.lrn_trig += ct_lrn_trig.lrn_delay_inc;
    if(lrn_trig.lrn_trig >= 1.0f) {
      do_lrn = true;
      lrn_trig.lrn_trig = -1.0f;  // begin refractory period
    }
  }
  else if(lrn_trig.lrn_trig < 0.0f) {	// already learned, counting down to refract
    lrn_trig.lrn_trig += ct_lrn_trig.lrn_refract_inc;
    if(lrn_trig.lrn_trig >= 0.0f) {
      lrn_trig.lrn_trig = 0.0f;	// reset -- eligible for learning again
    }
  }
  else if((lrn_trig.cyc_fm_dec == 0) && // going down
	  (lrn_trig.cyc_fm_inc == ct_lrn_trig.loc_max_cyc) && // x amount from inc
	  (loc_dec_norm >= ct_lrn_trig.loc_max_dec) && // x inc
	  (lrn_trig.loc_max >= thr_min_eff && lrn_trig.loc_max <= thr_max_eff)) { // max in range
    lrn_trig.lrn_max = lrn_trig.loc_max;
    lrn_trig.lrn_trig = ct_lrn_trig.lrn_delay_inc; // start counting
  }

  if(ct_lrn_trig.plus_lrn_cyc > 0) {
    // fake it override!
    do_lrn = (ct_cycle == (ct_time.minus + ct_lrn_trig.plus_lrn_cyc));
  }

  if(do_lrn) {
    // this is all the std code from Compute_dWt_FirstPlus
    Compute_dWt_SRAvg();
    Compute_dWt_Layer_pre();
    lrn_trig.lrn = 1;
    ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_FirstPlus);
    if(thread_flags & DWT)
      threads.Run(&un_call, 0.6f);
    else
      threads.Run(&un_call, -1.0f); // -1 = always run localized

    lrn_trig.lrn_stats_n++;
    if(phase_no == 0) {
      lrn_trig.lrn_min_sum += 1.0f;
      lrn_trig.lrn_min_cyc_sum += cycle;
      lrn_trig.lrn_min_thr_sum += lrn_trig.lrn_max;
    }
    else if(phase_no == 1) {
      lrn_trig.lrn_plus_sum += 1.0f;
      lrn_trig.lrn_plus_cyc_sum += cycle;
      lrn_trig.lrn_plus_thr_sum += lrn_trig.lrn_max;
    }
    else {
      lrn_trig.lrn_noth_sum += 1.0f;
      lrn_trig.lrn_noth_cyc_sum += cycle;
      lrn_trig.lrn_noth_thr_sum += lrn_trig.lrn_max;
    }
  }
  else {
    lrn_trig.lrn = 0;
  }
}

void LeabraNetwork::Compute_dWt_Layer_pre() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_dWt_Layer_pre(this);
  }
}

void LeabraNetwork::Compute_dWt_FirstPlus() {
  if(learn_rule == CTLEABRA_XCAL_C) return; // done separately
  if(learn_rule == LeabraNetwork::CTLEABRA_XCAL && epoch < ct_time.n_avg_only_epcs) {    
    return; // no learning while gathering data!
  }
  Compute_dWt_SRAvg();
  Compute_dWt_Layer_pre();
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_FirstPlus);
  if(thread_flags & DWT)
    threads.Run(&un_call, 0.6f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_dWt_SecondPlus() {
  if(learn_rule == CTLEABRA_XCAL_C) return; // done separately
  if(learn_rule == LeabraNetwork::CTLEABRA_XCAL && epoch < ct_time.n_avg_only_epcs) {    
    return; // no learning while gathering data!
  }
  Compute_dWt_SRAvg();
  Compute_dWt_Layer_pre();
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_SecondPlus);
  if(thread_flags & DWT)
    threads.Run(&un_call, 0.6f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_dWt_Nothing() {
  if(learn_rule == CTLEABRA_XCAL_C) return; // done separately
  if(learn_rule == LeabraNetwork::CTLEABRA_XCAL && epoch < ct_time.n_avg_only_epcs) {    
    return; // no learning while gathering data!
  }
  Compute_dWt_SRAvg();
  Compute_dWt_Layer_pre();
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_Nothing);
  if(thread_flags & DWT)
    threads.Run(&un_call, 0.6f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_Weights_impl() {
  ThreadUnitCall un_call(&Unit::Compute_Weights);
  if(thread_flags & WEIGHTS)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}


///////////////////////////////////////////////////////////////////////
//	Stats

void LeabraNetwork::Compute_ExtRew() {
  // assumes any ext rew computation has happened before this point, and set the
  // network ext_rew and ext_rew_avail flags appropriately
  if(ext_rew_avail) {
    avg_ext_rew_sum += ext_rew;
    avg_ext_rew_n++;
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

    avg_norm_err_sum += norm_err;
    avg_norm_err_n++;
  }
  else {
    norm_err = 0.0f;
  }
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

void LeabraNetwork::Compute_CtLrnTrigAvgs() {
  if(lrn_trig.lrn_stats_n > 0) {
    float ltrign = (float)lrn_trig.lrn_stats_n;
    if(lrn_trig.lrn_min_sum > 0.0f) {
      lrn_trig.lrn_min_cyc = lrn_trig.lrn_min_cyc_sum / lrn_trig.lrn_min_sum;
      lrn_trig.lrn_min_thr = lrn_trig.lrn_min_thr_sum / lrn_trig.lrn_min_sum;
    }
    else {
      lrn_trig.lrn_min_cyc = 0.0f;
      lrn_trig.lrn_min_thr = 0.0f;
    }
    lrn_trig.lrn_min = lrn_trig.lrn_min_sum / ltrign;

    if(lrn_trig.lrn_plus_sum > 0.0f) {
      lrn_trig.lrn_plus_cyc = lrn_trig.lrn_plus_cyc_sum / lrn_trig.lrn_plus_sum;
      lrn_trig.lrn_plus_thr = lrn_trig.lrn_plus_thr_sum / lrn_trig.lrn_plus_sum;
    }
    else {
      lrn_trig.lrn_plus_cyc = 0.0f;
      lrn_trig.lrn_plus_thr = 0.0f;
    }
    lrn_trig.lrn_plus = lrn_trig.lrn_plus_sum / ltrign;

    if(lrn_trig.lrn_noth_sum > 0.0f) {
      lrn_trig.lrn_noth_cyc = lrn_trig.lrn_noth_cyc_sum / lrn_trig.lrn_noth_sum;
      lrn_trig.lrn_noth_thr = lrn_trig.lrn_noth_thr_sum / lrn_trig.lrn_noth_sum;
    }
    else {
      lrn_trig.lrn_noth_cyc = 0.0f;
      lrn_trig.lrn_noth_thr = 0.0f;
    }
    lrn_trig.lrn_noth = lrn_trig.lrn_noth_sum / ltrign;
  }
  lrn_trig.Init_Stats_Sums();
}

void LeabraNetwork::Compute_EpochStats() {
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
  Compute_AvgNormErr();
  Compute_AvgExtRew();
  Compute_AvgSendPct();
  Compute_CtLrnTrigAvgs();
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
  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(!net) {
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
      ilay->pos.x = lay->pos.x + lay->disp_geom.x + 1;
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

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::UnitInhib -- actually saves network specifically");
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

