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

#ifndef LeabraConSpec_inlines_h
#define LeabraConSpec_inlines_h 1

// parent includes:

// member includes:
#include "ta_vector_ops.h"

// declare all other types mentioned but not required to include:

#ifdef TA_VEC_USE

inline void LeabraConSpec::Send_NetinDelta_vec(LeabraConGroup* cg, 
                                               const float su_act_delta_eff,
                                               float* send_netin_vec, const float* wts)
{
  VECF sa(su_act_delta_eff);
  const int sz = cg->size;
  const int parsz = cg->vec_chunked_size;
  int i;
  for(i=0; i<parsz; i += TA_VEC_SIZE) {
    VECF wt;  wt.load(wts+i);
    VECF dp = wt * sa;
    VECF rnet;
    float* stnet = send_netin_vec + cg->UnIdx(i);
    rnet.load(stnet);
    rnet += dp;
    rnet.store(stnet);
  }

  // remainder of non-vector chunkable ones
  for(; i<sz; i++) {
    send_netin_vec[cg->UnIdx(i)] += wts[i] * su_act_delta_eff;
  }
}

#endif

inline void LeabraConSpec::Send_NetinDelta_impl(LeabraConGroup* cg, LeabraNetwork* net,
                                         int thr_no, const float su_act_delta,
                                         const float* wts) {
  const float su_act_delta_eff = cg->scale_eff * su_act_delta;
  if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thr_no auto set to 0 in parent call if no threads
    float* send_netin_vec = net->ThrSendNetinTmpPerPrjn(thr_no, cg->other_idx);
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_delta_eff, send_netin_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_NetinDelta(wts[i], send_netin_vec,
                                         cg->UnIdx(i), su_act_delta_eff));
#endif
  }
  else {
    float* send_netin_vec = net->ThrSendNetinTmp(thr_no);
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_delta_eff, send_netin_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_NetinDelta(wts[i], send_netin_vec,
                                         cg->UnIdx(i), su_act_delta_eff));
#endif
  }
}

inline void LeabraConSpec::Send_NetinDelta(LeabraConGroup* cg, LeabraNetwork* net,
                                           int thr_no, const float su_act_delta)
{
  // note: _impl is used b/c subclasses replace WT var with another variable
  Send_NetinDelta_impl(cg, net, thr_no, su_act_delta, cg->OwnCnVar(WT));
}


inline float LeabraConSpec::Compute_Netin(ConGroup* rcg, Network* net, int thr_no) {
  LeabraConGroup* cg = (LeabraConGroup*)rcg;
  // this is slow b/c going through the PtrCn
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,WT,net),
					     cg->UnVars(i,net)->act));
  return ((LeabraConGroup*)cg)->scale_eff * rval;
}


//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCAL

#ifdef TA_VEC_USE

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL_vec
(LeabraConGroup* cg, float* dwts, float* ru_avg_s, float* ru_avg_m, float* ru_avg_l,
 float* ru_avg_l_lrn, float* ru_deep,
 const bool deep_on, const float clrate, const float bg_lrate, const float fg_lrate,
 const float su_avg_s, const float su_avg_m) {
  VECF su_avg_s_v(su_avg_s);
  VECF su_avg_m_v(su_avg_m);

  const int sz = cg->size;
  const int parsz = cg->vec_chunked_size;
  int i;
  for(i=0; i<parsz; i += TA_VEC_SIZE) {
    const int ru_idx = cg->UnIdx(i);
    VECF ru_avg_s_v; VECF ru_avg_m_v;
    // VECF ru_avg_l; // VECF ru_avg_l_lrn;
    ru_avg_s_v.load(ru_avg_s + ru_idx);
    ru_avg_m_v.load(ru_avg_m + ru_idx);
    //    ru_avg_l.load(avg_l + ru_idx);
    //    ru_avg_l_lrn.load(avg_l_lrn + ru_idx);

    VECF srs = ru_avg_s_v * su_avg_s_v;
    VECF srm = ru_avg_m_v * su_avg_m_v;
    
    for(int j=0; j< TA_VEC_SIZE; j++) {
      const float srs_j = srs[j];
      const float srm_j = srm[j];
      const float ru_avg_l_j = ru_avg_l[ru_idx+j];
      const float ru_avg_l_lrn_j = ru_avg_l_lrn[ru_idx+j];
      float lrate_eff = clrate;
      if(deep_on) {
        lrate_eff *= (bg_lrate + fg_lrate * ru_deep[ru_idx+j]);
      }
      dwts[i+j] += lrate_eff * (ru_avg_l_lrn_j * xcal.dWtFun(srs_j, ru_avg_l_j) +
                                xcal.m_lrn * xcal.dWtFun(srs_j, srm_j));
    }
  }
  for(;i<sz;i++) {              // get the remainder
    const int ru_idx = cg->UnIdx(i);
    float lrate_eff = clrate;
    if(deep_on) {
      lrate_eff *= (bg_lrate + fg_lrate * ru_deep[ru_idx]);
    }
    C_Compute_dWt_CtLeabraXCAL
      (dwts[i], lrate_eff, ru_avg_s[ru_idx], ru_avg_m[ru_idx], su_avg_s, su_avg_m,
       ru_avg_l[ru_idx], ru_avg_l_lrn[ru_idx]);
  }
}

#endif

inline void LeabraConSpec::Compute_dWt(ConGroup* scg, Network* rnet, int thr_no) {
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  if(!learn || (ignore_unlearnable && net->unlearnable_trial)) return;
  LeabraConGroup* cg = (LeabraConGroup*)scg;
  LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
  LeabraUnitSpec* us = (LeabraUnitSpec*)su->unit_spec;
  if(su->avg_s < us->opt_thresh.xcal_lrn && su->avg_m < us->opt_thresh.xcal_lrn) return;
  // no need to learn!
  bool deep_on = deep.on;
  if(deep_on) {
    LeabraUnitSpec* rus = (LeabraUnitSpec*)cg->prjn->layer->GetUnitSpec();
    if(!rus->deep_norm.on)
      deep_on = false;          // only applicable to deep_norm active layers
  }
  float clrate = cur_lrate;

  float bg_lrate;
  float fg_lrate;
  if(deep_on) {
    bg_lrate = deep.bg_lrate;
    fg_lrate = deep.fg_lrate;
  }

  const float su_avg_s = su->avg_s_eff;
  const float su_avg_m = su->avg_m;
  float* dwts = cg->OwnCnVar(DWT);

  const int sz = cg->size;

#if 0 // TA_VEC_USE
  // at this point, code is so simple that this vec version probably not worth it..
  LeabraNetwork* lnet = (LeabraNetwork*)net;
  float* avg_s = lnet->UnVecVar(thr_no, LeabraNetwork::AVG_S);
  float* avg_m = lnet->UnVecVar(thr_no, LeabraNetwork::AVG_M);
  float* avg_l = lnet->UnVecVar(thr_no, LeabraNetwork::AVG_L);
  float* avg_l_lrn = lnet->UnVecVar(thr_no, LeabraNetwork::AVG_L_LRN);
  float* deep = lnet->UnVecVar(thr_no, LeabraNetwork::DEEP);
  Compute_dWt_CtLeabraXCAL_vec
    (cg, dwts, avg_s, avg_m, avg_l, avg_l_lrn, deep,
     deep_on, clrate, bg_lrate, fg_lrate,
     su_avg_s, su_avg_m);
#else
  for(int i=0; i<sz; i++) {
    LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i, net);
    float lrate_eff = clrate;
    if(deep_on) {
      lrate_eff *= (bg_lrate + fg_lrate * ru->deep_norm);
    }
    C_Compute_dWt_CtLeabraXCAL
      (dwts[i], lrate_eff, ru->avg_s_eff, ru->avg_m, su_avg_s, su_avg_m,
       ru->avg_l, ru->avg_l_lrn);
  }
#endif
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraXCAL

#ifdef TA_VEC_USE
inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL_vec
(LeabraConGroup* cg, float* wts, float* dwts, float* fwts, float* swts) {

  VECF zeros(0.0f);
  VECF sig_res_inv(wt_sig_fun.res_inv);
  VECI idx;

  const int sz = cg->size;
  const int parsz = cg->vec_chunked_size;
  int i;
  for(i=0; i<parsz; i += TA_VEC_SIZE) {
    VECF wt;  wt.load(wts+i);
    VECF dwt; dwt.load(dwts+i);
    VECF fwt; fwt.load(fwts+i);

    dwt *= select(dwt > 0.0f, 1.0f - fwt, fwt);
    fwt += dwt;

    // todo: try this also as a sub-loop
    // wt = SigFmLinWt(fwt)
    idx = truncate_to_int(fwt * sig_res_inv); // min is 0
    wt = lookup<10002>(idx, wt_sig_fun.el);

    dwt = zeros;

    wt.store(wts+i);
    dwt.store(dwts+i);
    fwt.store(fwts+i);
    fwt.store(swts+i);          // swt = fwt
  }
  for(;i<sz;i++) {              // get the remainder
    C_Compute_Weights_CtLeabraXCAL(wts[i], dwts[i], fwts[i], swts[i]);
  }
}

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL_fast_vec
(LeabraConGroup* cg, const float decay_dt, const float wt_dt, const float slow_lrate,
 float* wts, float* dwts, float* fwts, float* swts) {

  VECF zeros(0.0f);
  VECF sig_res_inv(wt_sig_fun.res_inv);
  VECI idx;

  VECF ddt(decay_dt);
  VECF wdt(wt_dt);
  VECF slow_lr(slow_lrate);

  const int sz = cg->size;
  const int parsz = cg->vec_chunked_size;
  int i;
  for(i=0; i<parsz; i += TA_VEC_SIZE) {
    VECF wt;  wt.load(wts+i);
    VECF dwt; dwt.load(dwts+i);
    VECF fwt; fwt.load(fwts+i);
    VECF swt; swt.load(swts+i);

    dwt *= select(dwt > 0.0f, 1.0f - fwt, fwt);
    fwt += dwt + ddt * (swt - fwt);
    swt += dwt * slow_lr;

    // todo: try this also as a sub-loop
    // wt = SigFmLinWt(fwt)
    idx = truncate_to_int(fwt * sig_res_inv); // min is 0
    VECF nwt;
    nwt = lookup<10002>(idx, wt_sig_fun.el);
    wt += wdt * (nwt - wt);

    dwt = zeros;

    wt.store(wts+i);
    dwt.store(dwts+i);
    fwt.store(fwts+i);
    swt.store(swts+i);
  }
  for(;i<sz;i++) {              // get the remainder
    C_Compute_Weights_CtLeabraXCAL_fast
      (decay_dt, wt_dt, slow_lrate, wts[i], dwts[i], fwts[i], swts[i]);
  }
}

#endif

inline void LeabraConSpec::Compute_Weights(ConGroup* scg, Network* net, int thr_no) {
  if(!learn) return;

  LeabraConGroup* cg = (LeabraConGroup*)scg;

  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* fwts = cg->OwnCnVar(FWT);
  float* swts = cg->OwnCnVar(SWT);

  if(fast_wts.on) {
    const float decay_dt = fast_wts.decay_dt;
    const float wt_dt = fast_wts.wt_dt;
    const float slow_lrate = fast_wts.slow_lrate;
#ifdef TA_VEC_USE
    Compute_Weights_CtLeabraXCAL_fast_vec(cg, decay_dt, wt_dt, slow_lrate,
                                          wts, dwts, fwts, swts);
#else
    CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL_fast
                   (decay_dt, wt_dt, slow_lrate, wts[i], dwts[i], fwts[i], swts[i]));
#endif
  }
  else {
#ifdef TA_VEC_USE
    Compute_Weights_CtLeabraXCAL_vec(cg, wts, dwts, fwts, swts);
#else
    CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL
                   (wts[i], dwts[i], fwts[i], swts[i]));
#endif
  }
}

//////////////////////////////////////////////////////////////////////////////////
//     Compute dWt Norm: receiver based 

inline void LeabraConSpec::Compute_dWt_Norm(LeabraConGroup* cg, LeabraNetwork* net,
                                            int thr_no) {
  if(!learn || !wt_sig.dwt_norm || cg->size < 2) return;
  float sum_dwt = 0.0f;
  for(int i=0; i<cg->size; i++) {
    sum_dwt += cg->PtrCn(i,DWT,net);
  }
  if(sum_dwt == 0.0f) return;
  float dwnorm = sum_dwt / (float)cg->size;
  for(int i=0; i<cg->size; i++) {
    cg->PtrCn(i,DWT,net) -= dwnorm;
  }
}

inline void LeabraConSpec::Compute_CopyWeights(ConGroup* cg, ConGroup* src_cg,
                                               LeabraNetwork* net) {
  const int mx = MIN(cg->size, src_cg->size);
  float* wts = cg->OwnCnVar(WT);
  float* src_wts = src_cg->OwnCnVar(WT);
  for(int i=0; i<mx; i++) {
    wts[i] = src_wts[i];
  }
}


#endif // LeabraConSpec_inlines_h
