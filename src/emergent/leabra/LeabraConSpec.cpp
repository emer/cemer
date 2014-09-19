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

#include "LeabraConSpec.h"
#include <LeabraNetwork>
#include <taProject>
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(XCalLearnSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraConSpec);
TA_BASEFUNS_CTORS_DEFN(WtScaleSpec);
TA_BASEFUNS_CTORS_DEFN(WtSigSpec);
TA_BASEFUNS_CTORS_DEFN(FastWtsSpec);
SMARTREF_OF_CPP(LeabraConSpec);

eTypeDef_Of(ExtRewLayerSpec);

void WtScaleSpec::Initialize() {
  rel = 1.0f;
  abs = 1.0f;
  sem_extra = 2;
}

void WtScaleSpec::Defaults_init() {
  sem_extra = 2;
}


float WtScaleSpec::SLayActScale(const float savg, const float lay_sz, const float n_cons) {
  int slay_act_n = (int)(savg * lay_sz + .5f); // sending layer actual # active
  slay_act_n = MAX(slay_act_n, 1);
  float rval = 1.0f;
  if(n_cons == lay_sz) {
    rval = 1.0f / (float)slay_act_n;
  }
  else {
    int r_max_act_n = MIN((int)n_cons, slay_act_n); // max number we could get
    int r_avg_act_n = (int)(savg * n_cons + .5f);// recv average actual # active if uniform
    r_avg_act_n = MAX(r_avg_act_n, 1);
    int r_exp_act_n = r_avg_act_n + sem_extra;
    r_exp_act_n = MIN(r_exp_act_n, r_max_act_n);
    rval = 1.0f / (float)r_exp_act_n;
  }
  return rval;
}

void WtSigSpec::Initialize() {
  gain = 6.0f;
  off = 1.0f;
  dwt_norm = false;

  // todo: need to turn this back on if we decide dwt_norm needs to happen again
  // if(taMisc::is_loading) {
  //   taVersion v533(5, 3, 3);
  //   if(taMisc::loading_version < v533) { // default prior to 533 is off
  //     dwt_norm = false;
  //   }
  // }
}

void WtSigSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(owner) owner->UpdateAfterEdit(); // update our conspec so it can recompute lookup function!
}

void FastWtsSpec::Initialize() {
  on = false;
  decay_tau = 600.0f;
  wt_tau = 20.0f;
  fast_lrate = 5.0f;

  decay_dt = 1.0f / decay_tau;
  wt_dt = 1.0f / wt_tau;
  slow_lrate = 1.0f / fast_lrate;

  Defaults_init();
}

void FastWtsSpec::Defaults_init() {
  nofast_lrate = 2.0f;
}

void FastWtsSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  decay_dt = 1.0f / decay_tau;
  wt_dt = 1.0f / wt_tau;
  slow_lrate = 1.0f / fast_lrate;
}

void XCalLearnSpec::Initialize() {
  l_mix = X_COS_DIFF;
  thr_l_mix = 0.05f;

  if(taMisc::is_loading) {
    taVersion v634(6, 3, 4);
    if(taMisc::loading_version < v634) { // default prior to 634 is off
      l_mix = L_MIX;
      thr_l_mix = 0.01f;
    }
  }

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

void LeabraConSpec::Initialize() {
  min_obj_type = &TA_LeabraCon;
  inhib = false;

  diff_scale_p = false;
  
  learn = true;
  
  ignore_unlearnable = true;

  // we are no longer going back-and-forth from inv to sig weights 
  // so we presumably can use an order of magnitude smaller lookup table
  wt_sig_fun.x_range.min = 0.0f;
  wt_sig_fun.x_range.max = 1.0f;
  wt_sig_fun.res = 1.0e-4f;  // 1.0e-5f;     // 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun.UpdateAfterEdit_NoGui();

  wt_sig_fun_inv.x_range.min = 0.0f;
  wt_sig_fun_inv.x_range.max = 1.0f;
  wt_sig_fun_inv.res = 1.0e-4f;  // 1.0e-5f; // 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun_inv.UpdateAfterEdit_NoGui();

  wt_sig_fun_lst.off = -1.0f;   wt_sig_fun_lst.gain = -1.0f; // trigger an update
  wt_sig_fun_res = -1.0f;

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
  lrate = .02f;
  cur_lrate = .02f;
  lrs_mult = 1.0f;
}

void LeabraConSpec::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LeabraConSpec);
  children.SetBaseType(&TA_LeabraConSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  CreateWtSigFun();
  // if(taMisc::is_loading || taMisc::is_duplicating) return;
}

void LeabraConSpec::UpdateAfterEdit_impl() {
  if(HasBaseFlag(BF_MISC2)) return; // flag used for marking UAE
  SetBaseFlag(BF_MISC2);              // now in it, mark..

  inherited::UpdateAfterEdit_impl();
  lrate_sched.UpdateAfterEdit_NoGui();
  fast_wts.UpdateAfterEdit_NoGui();
  xcal.UpdateAfterEdit_NoGui(); // this calls owner
  CreateWtSigFun();

  ClearBaseFlag(BF_MISC2);      // done..
}

bool LeabraConSpec::CheckConfig_RecvCons(RecvCons* cg, bool quiet) {
  bool rval = true;
  return rval;
}

void LeabraConSpec::Compute_NetinScale(LeabraRecvCons* recv_gp, LeabraLayer* from, 
                                       bool plus_phase) {
  float savg = from->acts_m_avg_eff;
  float from_sz = (float)from->units.leaves;
  float n_cons = (float)recv_gp->size;
  if(plus_phase && diff_scale_p)
    recv_gp->scale_eff = wt_scale_p.FullScale(savg, from_sz, n_cons);
  else
    recv_gp->scale_eff = wt_scale.FullScale(savg, from_sz, n_cons);
}

void LeabraConSpec::Trial_Init_Specs(LeabraNetwork* net) {
  cur_lrate = lrate;            // as a backup..
  lrs_mult = 1.0f;
  if(wt_sig.dwt_norm) {
    net->net_misc.dwt_norm = true;
  }
  if(fast_wts.on) {
    cur_lrate *= fast_wts.fast_lrate;
  }
  else {
    cur_lrate *= fast_wts.nofast_lrate;
  }

  lrs_mult = lrate_sched.GetVal(net->epoch);
  cur_lrate *= lrs_mult;
}

void LeabraConSpec::LogLrateSched(int epcs_per_step, float n_steps) {
  float log_ns[3] = {1, .5f, .2f};

  lrate_sched.SetSize((int)n_steps);
  for(int i=0;i<n_steps;i++) {
    lrate_sched[i]->start_ctr = i * epcs_per_step;
    lrate_sched[i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
  }
  UpdateAfterEdit();            // needed to update the sub guys
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

