// Copyright 2017, Regents of the University of Colorado,
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
TA_BASEFUNS_CTORS_DEFN(LeabraMomentum);
TA_BASEFUNS_CTORS_DEFN(WtBalanceSpec);
TA_BASEFUNS_CTORS_DEFN(AdaptWtScaleSpec);
TA_BASEFUNS_CTORS_DEFN(SlowWtsSpec);
TA_BASEFUNS_CTORS_DEFN(DeepLrateSpec);
TA_BASEFUNS_CTORS_DEFN(MarginLearnSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraConSpec);
SMARTREF_OF_CPP(LeabraConSpec);

eTypeDef_Of(ExtRewLayerSpec);


////////////////////////////////////////////////////////////////////

void LeabraConSpec::Initialize() {
  min_obj_type = &TA_LeabraCon;

  Initialize_core();
  lrate_sched.interpolate = false;
  taVersion v820(8, 2, 0);
  if(taMisc::is_loading && taMisc::loading_version < v820) {
    momentum.on = false;
  }
  else {
    momentum.on = true;
  }
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
}

void LeabraConSpec::UpdateAfterEdit_impl() {
  if(HasBaseFlag(BF_MISC2)) return; // flag used for marking UAE
  SetBaseFlag(BF_MISC2);              // now in it, mark..

  inherited::UpdateAfterEdit_impl();
  lrate_sched.UpdateAfterEdit_NoGui();
  slow_wts.UpdateAfterEdit_NoGui();
  xcal.UpdateAfterEdit_NoGui(); // this calls owner

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

void LeabraConSpec::Trial_Init_Specs(LeabraNetwork* net) {
  float prv_cur_lrate = cur_lrate;
  cur_lrate = lrate;            // as a backup..
  lrs_mult = 1.0f;
  if(!InheritsFrom(&TA_LeabraBiasSpec)) { // bias spec doesn't count
    if(wt_bal.on) {
      net->net_misc.wt_bal = true;
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

void LeabraConSpec::LogLrateSched(int epcs_per_step, int n_steps, int bump_step) {
  float log_ns[3] = {1, .5f, .2f};

  if(bump_step <= 0) {
    lrate_sched.SetSize(n_steps);
    for(int i=0;i<n_steps;i++) {
      lrate_sched[i]->start_ctr = i * epcs_per_step;
      lrate_sched[i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
      lrate_sched[i]->UpdateAfterEdit();
    }
  }
  else {
    int tot_steps = 2*bump_step + (n_steps - bump_step);
    lrate_sched.SetSize(tot_steps);
    int i;
    for(i=0;i<bump_step;i++) {
      lrate_sched[i]->start_ctr = i * epcs_per_step;
      lrate_sched[i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
      lrate_sched[i]->UpdateAfterEdit();
    }
    int st_i = bump_step;
    for(i=0;i<bump_step;i++) {
      lrate_sched[st_i + i]->start_ctr = (st_i + i) * epcs_per_step;
      lrate_sched[st_i + i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
      lrate_sched[st_i + i]->UpdateAfterEdit();
    }
    int st_i2 = bump_step*2;
    int last_ctr = lrate_sched[st_i2-1]->start_ctr + epcs_per_step;
    for(i=bump_step;i<n_steps;i++) {
      lrate_sched[st_i + i]->start_ctr = last_ctr + (i-bump_step) * epcs_per_step;
      lrate_sched[st_i + i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
      lrate_sched[st_i + i]->UpdateAfterEdit();
    }
  }
    
  UpdateAfterEdit();            // needed to update the sub guys
}

void LeabraConSpec::ExpLrateSched(int epcs_per_step, int n_steps, float pct_per_step) {
  float cur_pct = 1.0f;
  lrate_sched.SetSize(n_steps);
  for(int i=0;i<n_steps;i++) {
    lrate_sched[i]->start_ctr = i * epcs_per_step;
    lrate_sched[i]->start_val = cur_pct;
    lrate_sched[i]->UpdateAfterEdit();
    cur_pct *= pct_per_step;
  }
  UpdateAfterEdit();            // needed to update the sub guys
}

void LeabraConSpec::LinearLrateSched(int epcs_per_step, int n_steps, float final_factor,
                                     bool interpolate) {
  float decr = (1.0f - final_factor) / (n_steps - 1.0f);
  float cur_pct = 1.0f;
  lrate_sched.SetSize(n_steps);
  lrate_sched.interpolate = interpolate;
  for(int i=0;i<n_steps;i++) {
    lrate_sched[i]->start_ctr = i * epcs_per_step;
    lrate_sched[i]->start_val = cur_pct;
    lrate_sched[i]->UpdateAfterEdit();
    cur_pct -= decr;
  }
  UpdateAfterEdit();            // needed to update the sub guys
}

void LeabraConSpec::TriangleLrateSched(int epcs_per_step, int n_cycles, float low_factor, int log_drop_cycles, bool interpolate) {
  
  float log_ns[3] = {1, .5f, .2f};
  lrate_sched.interpolate = interpolate;

  lrate_sched.SetSize(n_cycles * 2 + 1);
  lrate_sched[0]->start_ctr = 0;
  lrate_sched[0]->start_val = 1.0f;
  lrate_sched[0]->UpdateAfterEdit();

  int log_drops = 0;
  float high_val = 1.0f;

  for(int i=0;i<n_cycles;i++) {
    int idx = i*2 + 1;
    lrate_sched[idx]->start_ctr = idx * epcs_per_step;
    lrate_sched[idx]->start_val = high_val;
    lrate_sched[idx]->UpdateAfterEdit();
    idx++;
    lrate_sched[idx]->start_ctr = idx * epcs_per_step;
    lrate_sched[idx]->start_val = low_factor * high_val;
    lrate_sched[idx]->UpdateAfterEdit();
    if((i+1) % log_drop_cycles == 0) {
      log_drops++;
      high_val = log_ns[log_drops%3] * powf(10.0f,-(log_drops/3));
    }
  }
  UpdateAfterEdit();            // needed to update the sub guys
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

void LeabraConSpec::GraphLrateSched(DataTable* graph_data) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_LrateSched", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* epc = graph_data->FindMakeColName("epoch", idx, VT_FLOAT);
  DataCol* fact = graph_data->FindMakeColName("factor", idx, VT_FLOAT);
  // epc->SetUserData("MIN", 0.0f);
  // epc->SetUserData("MAX", 1.0f);
  fact->SetUserData("MIN", 0.0f);
  fact->SetUserData("MAX", 1.0f);

  for(int i = 0; i < lrate_sched.size; i++) {
    graph_data->AddBlankRow();
    epc->SetValAsFloat(lrate_sched[i]->start_ctr, -1);
    fact->SetValAsFloat(lrate_sched[i]->start_val, -1);
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

bool LeabraConSpec::SaveConVarToWeights(Network* net, ConState_cpp* cg, MemberDef* md) {
  if(!md->HasOption("SAVE")) return false;
  if(md->name != "scale") return true;
  if(adapt_scale.on) return true;
  PrjnState_cpp* pst = cg->GetPrjnState(net->net_state);
  Projection* prjn = net->PrjnFromState(pst);
  ProjectionSpec* ps = prjn->GetPrjnSpec();
  return ps->HasRandomScale();  // if random scale, needs to save it!
}



///////////////////////////////////////////////////////
//      Currently Unused Vector-based Code

// NOTE: this is no longer used and is missing a few things!
// #ifdef TA_VEC_USE

// inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL_vec
// (LeabraConState_cpp* cg, float* dwts, float* ru_avg_s, float* ru_avg_m, float* ru_avg_l,
//  float* ru_avg_l_lrn, float* ru_deep,
//  const bool deep_on, const float clrate, const float bg_lrate, const float fg_lrate,
//  const float su_avg_s, const float su_avg_m) {
//   VECF su_avg_s_v(su_avg_s);
//   VECF su_avg_m_v(su_avg_m);

//   const int sz = cg->size;
//   const int parsz = cg->vec_chunked_size;
//   int i;
//   for(i=0; i<parsz; i += TA_VEC_SIZE) {
//     const int ru_idx = cg->UnIdx(i);
//     VECF ru_avg_s_v; VECF ru_avg_m_v;
//     // VECF ru_avg_l; // VECF ru_avg_l_lrn;
//     ru_avg_s_v.load(ru_avg_s + ru_idx);
//     ru_avg_m_v.load(ru_avg_m + ru_idx);
//     //    ru_avg_l.load(avg_l + ru_idx);
//     //    ru_avg_l_lrn.load(avg_l_lrn + ru_idx);

//     VECF srs = ru_avg_s_v * su_avg_s_v;
//     VECF srm = ru_avg_m_v * su_avg_m_v;
    
//     for(int j=0; j< TA_VEC_SIZE; j++) {
//       const float srs_j = srs[j];
//       const float srm_j = srm[j];
//       const float ru_avg_l_j = ru_avg_l[ru_idx+j];
//       const float ru_avg_l_lrn_j = ru_avg_l_lrn[ru_idx+j];
//       // note: above factor is not always used -- if set_l_lrn for example.
//       float lrate_eff = clrate;
//       if(deep_on) {
//         lrate_eff *= (bg_lrate + fg_lrate * ru_deep[ru_idx+j]);
//       }
//       dwts[i+j] += lrate_eff * (ru_avg_l_lrn_j * xcal.dWtFun(srs_j, ru_avg_l_j) +
//                                 xcal.m_lrn * xcal.dWtFun(srs_j, srm_j));
//     }
//   }
//   for(;i<sz;i++) {              // get the remainder
//     const int ru_idx = cg->UnIdx(i);
//     float lrate_eff = clrate;
//     if(deep_on) {
//       lrate_eff *= (bg_lrate + fg_lrate * ru_deep[ru_idx]);
//     }
//     C_Compute_dWt_CtLeabraXCAL
//       (dwts[i], lrate_eff, ru_avg_s[ru_idx], ru_avg_m[ru_idx], su_avg_s, su_avg_m,
//        ru_avg_l[ru_idx], ru_avg_l_lrn[ru_idx]);
//   }
// }
// #endif

// #if 0 // TA_VEC_USE
//   // at this point, code is so simple that this vec version probably not worth it..
//   // also, the set_l_lrn is not supported here..
//   float* avg_s = net->UnVecVar(thr_no, LeabraNetwork::AVG_S);
//   float* avg_m = net->UnVecVar(thr_no, LeabraNetwork::AVG_M);
//   float* avg_l = net->UnVecVar(thr_no, LeabraNetwork::AVG_L);
//   float* avg_l_lrn = net->UnVecVar(thr_no, LeabraNetwork::AVG_L_LRN);
//   float* deep = net->UnVecVar(thr_no, LeabraNetwork::DEEP);
//   Compute_dWt_CtLeabraXCAL_vec
//     (cg, dwts, avg_s, avg_m, avg_l, avg_l_lrn, deep,
//      deep_on, clrate, bg_lrate, fg_lrate,
//      su_avg_s, su_avg_m);
// #else

// NOTE: not using this -- not updated
// #ifdef TA_VEC_USE
// inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL_vec
// (LeabraConState_cpp* cg, float* wts, float* dwts, float* fwts, float* swts, float* scales) {

//   VECF zeros(0.0f);
//   VECF sig_res_inv(wt_sig_fun.res_inv);
//   VECI idx;

//   const int sz = cg->size;
//   const int parsz = cg->vec_chunked_size;
//   int i;
//   for(i=0; i<parsz; i += TA_VEC_SIZE) {
//     VECF wt;  wt.load(wts+i);
//     VECF dwt; dwt.load(dwts+i);
//     VECF fwt; fwt.load(fwts+i);
//     VECF scale; scale.load(scales+i);

//     dwt *= select(dwt > 0.0f, 1.0f - fwt, fwt);
//     fwt += dwt;

//     // todo: try this also as a sub-loop
//     // wt = SigFmLinWt(fwt)
//     idx = truncate_to_int(fwt * sig_res_inv); // min is 0
//     wt = scale * lookup<10002>(idx, wt_sig_fun.el);

//     dwt = zeros;

//     wt.store(wts+i);
//     dwt.store(dwts+i);
//     fwt.store(fwts+i);
//     //  swt.store(swts+i);  // leave swt as pristine original weight value -- saves time
//     // and is useful for visualization!
//   }
//   for(;i<sz;i++) {              // get the remainder
//     C_Compute_Weights_CtLeabraXCAL(wts[i], dwts[i], fwts[i], swts[i], scales[i], 1.0f, 1.0f);
//   }
// }

// inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL_slow_vec
// (LeabraConState_cpp* cg, float* wts, float* dwts, float* fwts, float* swts, float* scales) {

//   VECF zeros(0.0f);
//   VECF sig_res_inv(wt_sig_fun.res_inv);
//   VECI idx;

//   VECF spct(slow_wts.swt_pct);
//   VECF fpct(slow_wts.fwt_pct);
//   VECF wdt(slow_wts.wt_dt);
//   VECF sdt(slow_wts.slow_dt);

//   const int sz = cg->size;
//   const int parsz = cg->vec_chunked_size;
//   int i;
//   for(i=0; i<parsz; i += TA_VEC_SIZE) {
//     VECF wt;  wt.load(wts+i);
//     VECF dwt; dwt.load(dwts+i);
//     VECF fwt; fwt.load(fwts+i);
//     VECF swt; swt.load(swts+i);
//     VECF scale; scale.load(scales+i);

//     dwt *= select(dwt > 0.0f, 1.0f - fwt, fwt);
//     fwt += dwt;
//     VECF eff_wt;
//     eff_wt = spct * swt + fpct * fwt;

//     // todo: try this also as a sub-loop
//     // wt = SigFmLinWt(fwt)
//     idx = truncate_to_int(eff_wt * sig_res_inv); // min is 0
//     VECF nwt;
//     nwt = scale * lookup<10002>(idx, wt_sig_fun.el);
//     wt += wdt * (nwt - wt);
//     swt += sdt * (fwt - swt);

//     dwt = zeros;

//     wt.store(wts+i);
//     dwt.store(dwts+i);
//     fwt.store(fwts+i);
//     swt.store(swts+i);
//   }
//   for(;i<sz;i++) {              // get the remainder
//     C_Compute_Weights_CtLeabraXCAL_slow(wts[i], dwts[i], fwts[i], swts[i], scales[i], 1.0f, 1.0f);
//   }
// }

// #endif

//   else {
//     if(slow_wts.on) {
// #ifdef TA_VEC_USE
//       Compute_Weights_CtLeabraXCAL_slow_vec(cg, wts, dwts, fwts, swts, scales);
// #else
//       CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL_slow
//                      (wts[i], dwts[i], fwts[i], swts[i], scales[i]));
// #endif
//     }
//     else {
// #ifdef TA_VEC_USE
//       Compute_Weights_CtLeabraXCAL_vec(cg, wts, dwts, fwts, swts, scales);
// #else
//       CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL
//                      (wts[i], dwts[i], fwts[i], swts[i], scales[i]));
// #endif
//     }
//   }

