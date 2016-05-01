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
#include <MemberDef>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(WtScaleSpec);
TA_BASEFUNS_CTORS_DEFN(XCalLearnSpec);
TA_BASEFUNS_CTORS_DEFN(WtSigSpec);
TA_BASEFUNS_CTORS_DEFN(WtBalanceSpec);
TA_BASEFUNS_CTORS_DEFN(AdaptWtScaleSpec);
TA_BASEFUNS_CTORS_DEFN(SlowWtsSpec);
TA_BASEFUNS_CTORS_DEFN(DeepLrateSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraConSpec);
SMARTREF_OF_CPP(LeabraConSpec);

eTypeDef_Of(ExtRewLayerSpec);


void WtScaleSpec::Initialize() {
  rel = 1.0f;
  abs = 1.0f;
  Defaults_init();
}

void WtScaleSpec::Defaults_init() {
  no_plus_net = false;
}


float WtScaleSpec::SLayActScale(const float savg, const float lay_sz, const float n_cons) {
  const float sem_extra = 2.0f;    // nobody ever modifies this, so keep it const

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

void XCalLearnSpec::Initialize() {
  Defaults_init();
}

void XCalLearnSpec::Defaults_init() {
  m_lrn = 1.0f;
  set_l_lrn = false;
  l_lrn = 1.0f;
  d_rev = 0.10f;
  d_thr = 0.0001f;
  d_rev_ratio = -(1.0f - d_rev) / d_rev;
}

void XCalLearnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  d_rev_ratio = (1.0f - d_rev) / d_rev;
  if(d_rev > 0.0f)
    d_rev_ratio = -(1.0f - d_rev) / d_rev;
  else
    d_rev_ratio = -1.0f;
}

void WtSigSpec::Initialize() {
  Defaults_init();
}

void WtSigSpec::Defaults_init() {
  gain = 6.0f;
  off = 1.0f;
  dwt_norm = false;
  rugp_wt_sync = false;
}

void WtSigSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(owner) owner->UpdateAfterEdit(); // update our conspec so it can recompute lookup function!
}

void WtBalanceSpec::Initialize() {
  Defaults_init();
}

void WtBalanceSpec::Defaults_init() {
  taVersion v787(7, 8, 7);
  if(taMisc::is_loading && taMisc::loading_version < v787) {
    on = false;
  }
  else {
    on = true;                  // new default
  }
  trg = 0.3f;
  thr = 0.1f;
  gain = 2.0f;
  avg_updt = 10;

  hi_thr = trg + thr;
  lo_thr = trg - thr;
}

void WtBalanceSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  hi_thr = trg + thr;
  lo_thr = trg - thr;
}


void AdaptWtScaleSpec::Initialize() {
  on = false;
  Defaults_init();
}

void AdaptWtScaleSpec::Defaults_init() {
  tau = 5000.0f;
  lo_thr = 0.25f;
  hi_thr = 0.75f;
  lo_scale = 0.01f;
  hi_scale = 2.0f;
  dt = 1.0f / tau;
}

void AdaptWtScaleSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt = 1.0f / tau;
}


void SlowWtsSpec::Initialize() {
  on = false;
  wt_tau = 1.0f;

  Defaults_init();
}

void SlowWtsSpec::Defaults_init() {
  swt_pct = 0.8f;
  slow_tau = 100;

  fwt_pct = 1.0f - swt_pct;
  slow_dt = 1.0f /(float)slow_tau;
  wt_dt = 1.0f / wt_tau;
}

void SlowWtsSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  fwt_pct = 1.0f - swt_pct;
  slow_dt = 1.0f /(float)slow_tau;
  wt_dt = 1.0f / wt_tau;
}

void DeepLrateSpec::Initialize() {
  on = false;
  bg_lrate = 1.0f;
  fg_lrate = 0.0f;
  Defaults_init();
}

void DeepLrateSpec::Defaults_init() {
}


////////////////////////////////////////////////////////////////////

void LeabraConSpec::Initialize() {
  min_obj_type = &TA_LeabraCon;
  inhib = false;

  learn = true;
  learn_qtr = Q4;
  
  use_unlearnable = true;

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
  lrate = .04f;
  use_lrate_sched = true;
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
  slow_wts.UpdateAfterEdit_NoGui();
  xcal.UpdateAfterEdit_NoGui(); // this calls owner
  CreateWtSigFun();

  ClearBaseFlag(BF_MISC2);      // done..

  if(taMisc::is_loading) {
    taVersion v785(7, 8, 5);
    if(taMisc::loading_version < v785 && !HasBaseFlag(BF_MISC4)) { // prevent repeated updates!
      lrate = 2.0f * lrate;
      cur_lrate = 2.0f * cur_lrate;
      SetBaseFlag(BF_MISC4);
      TestWarning(true, "UAE",
                  "updated lrate in conspec = 2 * previous value -- as of version 7.8.5 a previous implicit 2x factor was removed");
    }
  }
}

void LeabraConSpec::GetPrjnName(Projection& prjn, String& nm) {
  if(inhib)
    nm = "Inhib_" + nm;
  else
    return inherited::GetPrjnName(prjn, nm);
}

bool LeabraConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  return rval;
}

void LeabraConSpec::Compute_NetinScale(LeabraConGroup* recv_gp, LeabraLayer* from, 
                                       bool plus_phase) {
  float savg = from->acts_p_avg_eff;
  float from_sz = (float)from->units.leaves;
  float n_cons = (float)recv_gp->size;
  recv_gp->scale_eff = wt_scale.FullScale(savg, from_sz, n_cons);
}

void LeabraConSpec::RenormScales(ConGroup* cg, Network* net, int thr_no,
                                 bool mult_norm, float avg_wt) {
  if(cg->size < 2) return;
  float avg = 0.0f;
  for(int i=0; i<cg->size; i++) {
    avg += cg->Cn(i, SCALE, net);
  }
  avg /= (float)cg->size;
  if(mult_norm) {
    float adj = avg_wt / avg;
    for(int i=0; i<cg->size; i++) {
      cg->Cn(i, SCALE, net) *= adj;
    }
  }
  else {
    float adj = avg_wt - avg;
    for(int i=0; i<cg->size; i++) {
      cg->Cn(i, SCALE, net) += adj;
    }
  }
}

#ifdef SUGP_NETIN
int LeabraConSpec::Init_SUGps(LeabraConGroup* cg, LeabraNetwork* net, int thr_no) {
  // have to do this recv-based, because that is the perspective for the netin rollup
  // so indexes need to be into the recv guys
  Layer* slay = cg->prjn->from.ptr();
  const int sz = cg->size;
  if(!slay->unit_groups) {
    for(int i=0; i<sz; i++) {
      cg->PtrCnInt(i,SUGP,net) = 0;
    }
    return 0;
  }
  // note: this assumes that connections are always made within unit groups as inner loop!
  int sugp_idx = -1;
  int last_ug = -1;
  for(int i=0; i<sz; i++) {
    LeabraUnit* su = (LeabraUnit*)cg->Un(i, net);
    int ugi = su->UnitGpIdx();
    if(ugi != last_ug) {
      last_ug = ugi;
      sugp_idx++;
    }
    cg->PtrCnInt(i,SUGP,net) = sugp_idx;
  }
  return sugp_idx+1;            // total n
}

bool LeabraConSpec::Init_SUGpChunkFlag(LeabraConGroup* cg, LeabraNetwork* net, int thr_no) {
  cg->SetConGroupFlag(ConGroup::CHUNKS_SAME_SUGP);
  Layer* slay = cg->prjn->from.ptr();
  if(!slay->unit_groups)
    return true;
  const int sz = cg->vec_chunked_size;
  const int vct = ConGroup::vec_chunk_targ;
  int32_t* sugps = cg->OwnCnVarInt(SUGP);
  for(int i=0; i<sz; i += vct) {
    int32_t sugp = sugps[i];
    for(int j=1; j<vct; j++) {
      if(sugps[i+j] != sugp) {
        cg->ClearConGroupFlag(ConGroup::CHUNKS_SAME_SUGP);
        break;
      }
    }
  }
  return cg->HasConGroupFlag(ConGroup::CHUNKS_SAME_SUGP);
}

#endif // SUGP_NETIN

void LeabraConSpec::Trial_Init_Specs(LeabraNetwork* net) {
  float prv_cur_lrate = cur_lrate;
  cur_lrate = lrate;            // as a backup..
  lrs_mult = 1.0f;
  if(!InheritsFrom(&TA_LeabraBiasSpec)) { // bias spec doesn't count
    if(wt_sig.dwt_norm) {
      net->net_misc.dwt_norm = true;
    }
    if(wt_bal.on) {
      net->net_misc.wt_bal = true;
    }
    if(wt_sig.rugp_wt_sync) {
      net->net_misc.rugp_wt_sync = true;
    }
  }

  if(use_lrate_sched) {
    lrs_mult = lrate_sched.GetVal(net->epoch);
    cur_lrate *= lrs_mult;
    if(cur_lrate != prv_cur_lrate) {
      net->net_misc.lrate_updtd = true;
    }
  }
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
  taProject* proj = GetMyProj();
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
  taProject* proj = GetMyProj();
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
  taProject* proj = GetMyProj();
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

void LeabraConSpec::GraphSlowWtsFun(int trials, DataTable* graph_data) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_SlowWtsFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* trialc = graph_data->FindMakeColName("trial", idx, VT_INT);
  DataCol* wtc = graph_data->FindMakeColName("wt", idx, VT_FLOAT);
  DataCol* fwtc = graph_data->FindMakeColName("fwt", idx, VT_FLOAT);
  DataCol* swtc = graph_data->FindMakeColName("swt", idx, VT_FLOAT);
  DataCol* dwtc = graph_data->FindMakeColName("dwt", idx, VT_FLOAT);
  // wtc->SetUserData("MIN", 0.0f);
  // wtc->SetUserData("MAX", 1.0f);
  // dwtc->SetUserData("MIN", 0.0f);
  // dwtc->SetUserData("MAX", 0.5f);

  float dwt = 0.0f;
  float wt = 0.5f;
  float fwt = 0.5f;
  float swt = 0.5f;
  float scale = 1.0f;
  for(int trl = 0; trl < trials; trl++) {
    dwt = Random::UniformMinMax(-lrate, lrate);
    float dwt_save = dwt;
    C_Compute_Weights_CtLeabraXCAL_slow(wt, dwt, fwt, swt, scale, 1.0f, 1.0f);
    graph_data->AddBlankRow();
    trialc->SetValAsInt(trl, -1);
    dwtc->SetValAsFloat(dwt_save, -1);
    wtc->SetValAsFloat(wt, -1);
    fwtc->SetValAsFloat(fwt, -1);
    swtc->SetValAsFloat(swt, -1);
  }

  float_Matrix* wtv = (float_Matrix*)wtc->AR();
  int mx_trl = 0;
  float mx_wt = taMath_float::vec_max(wtv, mx_trl);

  float swt_wt = wt_sig.SigFmLinWt(swt);
  float half_wt = swt_wt + 0.5f * (mx_wt - swt_wt);
  
  float prv_wt = mx_wt;
  int half_trl = 0;
  for(int i=mx_trl; i<wtv->size; i++) {
    wt = wtv->FastEl_Flat(i);
    if(wt <= half_wt && prv_wt >= half_wt) {
      half_trl = i;
      break;
    }
    prv_wt = wt;
  }
  
  taMisc::Info("weight max of:", String(mx_wt), "at trial:", String(mx_trl),
               "half weight of:", String(half_wt), "at trial:", String(half_trl));

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

bool LeabraConSpec::SaveConVarToWeights(MemberDef* md) {
  if(!md->HasOption("SAVE")) return false;
  if(md->name != "scale") return true;
  return adapt_scale.on;      // only save if adapting!
}
