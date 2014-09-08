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

inline void LeabraConSpec::Send_NetinDelta_vec(LeabraSendCons* cg, 
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

inline void LeabraConSpec::Send_NetinDelta_impl(LeabraSendCons* cg, LeabraNetwork* net,
                                         const int thread_no, const float su_act_delta,
                                         const float* wts) {
  const float su_act_delta_eff = cg->scale_eff * su_act_delta;
  if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
    float* send_netin_vec = net->send_netin_tmp.el
      + net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_delta_eff, send_netin_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(wts[i], send_netin_vec,
                                                cg->UnIdx(i), su_act_delta_eff));
#endif
  }
  else {
    float* send_netin_vec = net->send_netin_tmp.el
      + net->send_netin_tmp.FastElIndex(0, thread_no);
#ifdef TA_VEC_USE
    Send_NetinDelta_vec(cg, su_act_delta_eff, send_netin_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(wts[i], send_netin_vec,
                                                cg->UnIdx(i), su_act_delta_eff));
#endif
  }
}

inline void LeabraConSpec::Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                           const int thread_no, const float su_act_delta)
{
  // note: _impl is used b/c subclasses replace WT var with another variable
  Send_NetinDelta_impl(cg, net, thread_no, su_act_delta, cg->OwnCnVar(WT));
}


inline float LeabraConSpec::Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
  // this is slow b/c going through the PtrCn
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,WT,net),
					     cg->Un(i,net)->act));
  return ((LeabraRecvCons*)cg)->scale_eff * rval;
}


////////////////////////////////////////////////////
//     Computing dWt: LeabraCHL

inline void LeabraConSpec::Compute_SAvgCor(LeabraSendCons* cg, LeabraUnit*,
                                           LeabraNetwork* net) {
  LeabraLayer* fm = (LeabraLayer*)cg->prjn->from.ptr();
  float savg = .5f + savg_cor.cor * (fm->kwta.pct - .5f);
  savg = MAX(savg_cor.thresh, savg); // keep this computed value within bounds
  cg->savg_cor = .5f / savg;
}

inline void LeabraConSpec::Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) {
  if(ignore_unlearnable && net->unlearnable_trial) return;

  Compute_SAvgCor(cg, su, net);
  if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

  float* fwts = cg->OwnCnVar(FWT);
  float* dwts = cg->OwnCnVar(DWT);

  const int sz = cg->size;
  for(int i=0; i<sz; i++) {
    LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
    const float lin_wt = fwts[i]; // already linear
    C_Compute_dWt_LeabraCHL(dwts[i],
		  C_Compute_Hebb(cg->savg_cor, lin_wt, ru->act_p, su->act_p),
		  C_Compute_Err_LeabraCHL(lin_wt, ru->act_p, ru->act_m,
					  su->act_p, su->act_m));
  }
}

/////////////////////////////////////
//	Compute_Weights_LeabraCHL

inline void LeabraConSpec::Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                     LeabraNetwork* net) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* fwts = cg->OwnCnVar(FWT);
  float* swts = cg->OwnCnVar(SWT);

  if(fast_wts.on) {
    const float decay = fast_wts.decay;
    const float slow_lrate = fast_wts.slow_lrate;
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL_fast
                   (decay, slow_lrate, wts[i], dwts[i], fwts[i], swts[i]));
  }
  else {
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL(wts[i], dwts[i], 
                                                   fwts[i], swts[i]));
  }
  //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
}

//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCAL

#ifdef TA_VEC_USE

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL_cosdiff_vec
(LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net, float* dwts,
 float* avg_s, float* avg_m, float* avg_l, float* thal,
 const bool cifer_on, const float clrate, const float bg_lrate, const float fg_lrate,
 const float su_avg_s, const float su_avg_m, const float effmmix, const float su_act_mult) {

  VECF su_avg_s_v(su_avg_s);
  VECF su_avg_m_v(su_avg_m);
  VECF effmmix_v(effmmix);
  VECF su_act_mult_v(su_act_mult);
  VECF s_mix(xcal.s_mix);
  VECF m_mix(xcal.m_mix);

  const int sz = cg->size;
  const int parsz = cg->vec_chunked_size;
  int i;
  for(i=0; i<parsz; i += TA_VEC_SIZE) {
    const int ru_idx = cg->UnIdx(i);
    VECF ru_avg_s;     VECF ru_avg_m;      VECF ru_avg_l;
    ru_avg_s.load(avg_s + ru_idx);
    ru_avg_m.load(avg_m + ru_idx);
    ru_avg_l.load(avg_l + ru_idx);

    VECF srs = ru_avg_s * su_avg_s_v;
    VECF srm = ru_avg_m * su_avg_m_v;
    VECF sm_mix = s_mix * srs + m_mix * srm;
    
    VECF lthr = su_act_mult_v * ru_avg_l;
    VECF effthr = effmmix_v * srm + lthr;

    for(int j=0; j< TA_VEC_SIZE; j++) {
      const float sm_mix_j = sm_mix[j];
      const float effthr_j = effthr[j];
      float lrate_eff = clrate;
      if(cifer_on) {
        lrate_eff *= (bg_lrate + fg_lrate * thal[ru_idx+j]);
      }
      dwts[i+j] += lrate_eff * xcal.dWtFun(sm_mix_j, effthr_j);
    }
  }
  for(;i<sz;i++) {              // get the remainder
    const int ru_idx = cg->UnIdx(i);
    float lrate_eff = clrate;
    if(cifer_on) {
      lrate_eff *= (bg_lrate + fg_lrate * thal[ru_idx]);
    }
    C_Compute_dWt_CtLeabraXCAL_cosdiff
      (dwts[i], lrate_eff, avg_s[ru_idx], avg_m[ru_idx], avg_l[ru_idx],
       su_avg_s, su_avg_m, su_act_mult, effmmix);
  }
}

#endif

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) {
  if(ignore_unlearnable && net->unlearnable_trial) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)su->GetUnitSpec();
  if(su->avg_s < us->opt_thresh.xcal_lrn && su->avg_m < us->opt_thresh.xcal_lrn) return;
  // no need to learn!
  const bool cifer_on = us->cifer.on; // should be global for send, recv..
  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  float clrate = cur_lrate;

  float bg_lrate;
  float fg_lrate;
  if(cifer_on) {
    LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->unit_spec.SPtr();
    bg_lrate = rus->cifer.bg_lrate;
    fg_lrate = rus->cifer.fg_lrate;
  }

  const float su_avg_s = su->avg_s;
  const float su_avg_m = su->avg_m;
  float* dwts = cg->OwnCnVar(DWT);

  float* avg_s = net->UnVecVar(LeabraNetwork::AVG_S);
  float* avg_m = net->UnVecVar(LeabraNetwork::AVG_M);
  float* avg_l = net->UnVecVar(LeabraNetwork::AVG_L);
  float* thal = net->UnVecVar(LeabraNetwork::THAL);

  const int sz = cg->size;

  if(xcal.l_mix == XCalLearnSpec::X_COS_DIFF) {
    float rlay_cos_diff_avg = 1.0f - rlay->cos_diff_avg;
    if(rlay->layer_type != Layer::HIDDEN)
      rlay_cos_diff_avg = 0.0f; // any kind of clamped layer should not use this!
    const float efflmix = xcal.thr_l_mix * rlay_cos_diff_avg;
    const float effmmix = 1.0f - efflmix;
    const float su_act_mult = efflmix * su_avg_m;

#ifdef TA_VEC_USE
    Compute_dWt_CtLeabraXCAL_cosdiff_vec
      (cg, su, net, dwts, avg_s, avg_m, avg_l, thal,
       cifer_on, clrate, bg_lrate, fg_lrate,
       su_avg_s, su_avg_m, effmmix, su_act_mult);
#else
    for(int i=0; i<sz; i++) {
      const int ru_idx = cg->UnIdx(i);
      float lrate_eff = clrate;
      if(cifer_on) {
        lrate_eff *= (bg_lrate + fg_lrate * thal[ru_idx]);
      }
      C_Compute_dWt_CtLeabraXCAL_cosdiff
        (dwts[i], lrate_eff, avg_s[ru_idx], avg_m[ru_idx], avg_l[ru_idx],
         su_avg_s, su_avg_m, su_act_mult, effmmix);
    }
#endif
  }
  else {                        // L_MIX
    const float su_act_mult = xcal.thr_l_mix * su_avg_m;
    for(int i=0; i<sz; i++) {
      const int ru_idx = cg->UnIdx(i);
      float lrate_eff = clrate;
      if(cifer_on) {
        lrate_eff *= (bg_lrate + fg_lrate * thal[ru_idx]);
      }
      C_Compute_dWt_CtLeabraXCAL(dwts[i], lrate_eff,
                                 avg_s[ru_idx], avg_m[ru_idx], avg_l[ru_idx],
                                 su_avg_s, su_avg_m, su_act_mult);
    }
  }
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraXCAL

// this turns out to not be worth it, especially for no dwt_norm case, because
// we cannot take advantage of skipping all the dwt = 0 cases..

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL_fast_vec
(LeabraSendCons* cg, const float decay, const float slow_lrate,
 float* wts, float* dwts, float* fwts, float* swts) {

  VECF zeros(0.0f);
  VECF inv_res_inv(wt_sig_fun_inv.res_inv);
  VECF sig_res_inv(wt_sig_fun.res_inv);
  VECI idx;

  VECF dk(decay);
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
    fwt += dwt + dk * (swt - fwt);
    swt += dwt * slow_lr;

    // todo: try this also as a sub-loop
    // wt = SigFmLinWt(fwt)
    idx = truncate_to_int(fwt * sig_res_inv); // min is 0
    wt = lookup<100002>(idx, wt_sig_fun.el);

    dwt = zeros;

    wt.store(wts+i);
    dwt.store(dwts+i);
    fwt.store(fwts+i);
    swt.store(swts+i);
  }
  for(;i<sz;i++) {              // get the remainder
    C_Compute_Weights_CtLeabraXCAL_fast
      (decay, slow_lrate, wts[i], dwts[i], fwts[i], swts[i]);
  }
}

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL
(LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* fwts = cg->OwnCnVar(FWT);
  float* swts = cg->OwnCnVar(SWT);

  if(fast_wts.on) {
    const float decay = fast_wts.decay;
    const float slow_lrate = fast_wts.slow_lrate;
#ifdef TA_VEC_USE
    Compute_Weights_CtLeabraXCAL_fast_vec(cg, decay, slow_lrate, wts, dwts, fwts, swts);
#else
    CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL_fast
                   (decay, slow_lrate, wts[i], dwts[i], fwts[i], swts[i]));
#endif
  }
  else {
    CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL
                   (wts[i], dwts[i], fwts[i], swts[i]));
  }
}


//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_CAL -- NOTE: Requires LeabraSRAvgCon connections!

//////////////////////////////////////////////////////////////////////////////////
//     CtLeabra CAL SRAvg stuff

inline void LeabraConSpec::Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net, const bool do_s) {
  float* srm = cg->OwnCnVar(SRAVG_M);
  float* srs = cg->OwnCnVar(SRAVG_S);
  const float su_act_lrn = su->act_lrn;
  if(do_s) {
    CON_GROUP_LOOP(cg, C_Compute_SRAvg_ms(srm[i], srs[i],
					  ((LeabraUnit*)cg->Un(i,net))->act_lrn,
                                          su_act_lrn));
  }
  else {
    CON_GROUP_LOOP(cg, C_Compute_SRAvg_m(srm[i], 
					 ((LeabraUnit*)cg->Un(i,net))->act_lrn,
                                         su_act_lrn));
  }
}

inline void LeabraConSpec::Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su,
                                            LeabraNetwork* net) {
  float* srm = cg->OwnCnVar(SRAVG_M);
  float* srs = cg->OwnCnVar(SRAVG_S);
  CON_GROUP_LOOP(cg, C_Trial_Init_SRAvg(srm[i], srs[i]));
}

inline void LeabraConSpec::Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                   LeabraNetwork* net) {
  if(ignore_unlearnable && net->unlearnable_trial) return;

  float* dwts = cg->OwnCnVar(DWT);
  float* srm = cg->OwnCnVar(SRAVG_M);
  float* srs = cg->OwnCnVar(SRAVG_S);
  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  const float sravg_s_nrm = rlay->sravg_vals.s_nrm;
  const float sravg_m_nrm = rlay->sravg_vals.m_nrm;

  CON_GROUP_LOOP(cg, C_Compute_dWt_CtLeabraCAL(dwts[i], srs[i], srm[i],
                                               sravg_s_nrm, sravg_m_nrm));
}

/////////////////////////////////////
// Master dWt function

inline void LeabraConSpec::Compute_dWt(BaseCons* bcg, Unit* bsu, Network* bnet) {
  if(!learn) return;
  LeabraSendCons* cg = (LeabraSendCons*)bcg;
  LeabraUnit* su = (LeabraUnit*)bsu;
  LeabraNetwork* net = (LeabraNetwork*)bnet;
  switch(learn_rule) {
  case CTLEABRA_XCAL:
    Compute_dWt_CtLeabraXCAL(cg, su, net);
    break;
  case LEABRA_CHL:
    Compute_dWt_LeabraCHL(cg, su, net);
    break;
  case CTLEABRA_CAL:
    Compute_dWt_CtLeabraCAL(cg, su, net);
    break;
  }
}

inline void LeabraConSpec::Compute_Weights(BaseCons* bcg, Unit* bsu, Network* bnet) {
  if(!learn) return;
  LeabraSendCons* cg = (LeabraSendCons*)bcg;
  LeabraUnit* su = (LeabraUnit*)bsu;
  LeabraNetwork* net = (LeabraNetwork*)bnet;
  switch(learn_rule) {
  case CTLEABRA_XCAL:
    Compute_Weights_CtLeabraXCAL(cg, su, net);
    break;
  case LEABRA_CHL:
    Compute_Weights_LeabraCHL(cg, su, net);
    break;
  case CTLEABRA_CAL:
    Compute_Weights_CtLeabraXCAL(cg, su, net);
    break;
  }
}

//////////////////////////////////////////////////////////////////////////////////
//     Compute dWt Norm: receiver based 

inline void LeabraConSpec::Compute_dWt_Norm(LeabraRecvCons* cg, LeabraUnit* ru,
                                            LeabraNetwork* net) {
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

//////////////////////////////////////////////////////////////////////
//	Bias Weights: threshold dwt versions in LeabraBiasSpec

inline void LeabraConSpec::B_Compute_dWt_LeabraCHL(RecvCons* bias, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  bias->OwnCn(0,DWT) += cur_lrate * err;
}
  
// default is not to do anything tricky with the bias weights
inline void LeabraConSpec::B_Compute_Weights(RecvCons* bias, LeabraUnit* ru) {
  if(!learn) return;
  float& wt =  bias->OwnCn(0,WT);
  float& dwt = bias->OwnCn(0,DWT);
  float& fwt = bias->OwnCn(0,FWT);
  float& swt = bias->OwnCn(0,SWT);
  fwt += dwt;
  if(fast_wts.on) {
    fwt += fast_wts.decay * (swt - fwt);
    swt += fast_wts.slow_lrate * dwt;
  }
  else {
    swt = fwt;
  }
  wt = fwt;
  dwt = 0.0f;
  C_ApplyLimits(wt);
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraXCAL(RecvCons* bias, LeabraUnit* ru,
						      LeabraLayer* rlay) {
  // cal only for bias weights: only err is useful contributor to this learning
  float dw = ru->avg_s - ru->avg_m;
  bias->OwnCn(0,DWT) += cur_lrate * dw;
}

inline void LeabraConSpec::B_Compute_dWt_CtLeabraCAL(RecvCons* bias, LeabraUnit* ru,
						     LeabraLayer* rlay) {
  float dw = ru->avg_s - ru->avg_m;
  bias->OwnCn(0,DWT) += cur_lrate * dw;
}

/////////////////////////////////////
// Master Bias dWt function

inline void LeabraConSpec::B_Compute_dWt(RecvCons* bias, LeabraUnit* ru,
                                         LeabraLayer* rlay) {
  if(!learn) return;
  switch(learn_rule) {
  case CTLEABRA_XCAL:
    B_Compute_dWt_CtLeabraXCAL(bias, ru, rlay);
    break;
  case LEABRA_CHL:
    B_Compute_dWt_LeabraCHL(bias, ru);
    break;
  case CTLEABRA_CAL:
    B_Compute_dWt_CtLeabraCAL(bias, ru, rlay);
    break;
  }
}


inline void LeabraConSpec::Compute_CopyWeights(LeabraSendCons* cg, LeabraSendCons* src_cg,
                                               LeabraNetwork* net) {
  const int mx = MIN(cg->size, src_cg->size);
  float* wts = cg->OwnCnVar(WT);
  float* src_wts = src_cg->OwnCnVar(WT);
  for(int i=0; i<mx; i++) {
    wts[i] = src_wts[i];
  }
}


/////////////////////////////////////
//      LeabraBiasSpec

inline void LeabraBiasSpec::B_Compute_dWt_LeabraCHL(RecvCons* bias, LeabraUnit* ru) {
  float err = ru->act_p - ru->act_m;
  if(fabsf(err) >= dwt_thresh) {
    bias->OwnCn(0,DWT) += cur_lrate * err;
  }
}

inline void LeabraBiasSpec::B_Compute_dWt_CtLeabraXCAL(RecvCons* bias, LeabraUnit* ru,
                                                       LeabraLayer* rlay) {
  // cal only for bias weights: only err is useful contributor to this learning
  float dw = ru->avg_s - ru->avg_m;
  if(fabsf(dw) >= dwt_thresh) {
    bias->OwnCn(0,DWT) += cur_lrate * dw;
  }
}

inline void LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(RecvCons* bias, LeabraUnit* ru,
                                                      LeabraLayer* rlay) {
  float dw = ru->avg_s - ru->avg_m;
  if(fabsf(dw) >= dwt_thresh) {
    bias->OwnCn(0,DWT) += cur_lrate * dw;
  }
}


#endif // LeabraConSpec_inlines_h
