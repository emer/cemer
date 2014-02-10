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
#ifdef USE_SSE8
#include "vectorclass.h"   // Abner Fog's wrapper for SSE intrinsics
#endif

// declare all other types mentioned but not required to include:

inline void LeabraConSpec::Compute_StableWeights(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) {
  if(!learn) return;
  float* wts = cg->OwnCnVar(WT);
  float* swts = cg->OwnCnVar(SWT);
  float* lwts = cg->OwnCnVar(LWT);

  CON_GROUP_LOOP(cg, C_Compute_StableWeights(wts[i], swts[i], lwts[i]));
}


#ifdef USE_SSE8
inline void LeabraConSpec::Send_NetinDelta_sse8(LeabraSendCons* cg, 
                                                const float su_act_delta_eff,
                                                float* send_netin_vec, const float* wts)
{
  Vec8f sa(su_act_delta_eff);
  const int sz = cg->size;
  const int parsz = 8 * (sz / 8);
  for(int i=0; i<parsz; i+=8) {
    Vec8f wt;  wt.load(wts+i);
    Vec8f dp = wt * sa;
    float sni[8];
    sni[0] = send_netin_vec[cg->unit_idxs[i+0]];
    sni[1] = send_netin_vec[cg->unit_idxs[i+1]];
    sni[2] = send_netin_vec[cg->unit_idxs[i+2]];
    sni[3] = send_netin_vec[cg->unit_idxs[i+3]];
    sni[4] = send_netin_vec[cg->unit_idxs[i+4]];
    sni[5] = send_netin_vec[cg->unit_idxs[i+5]];
    sni[6] = send_netin_vec[cg->unit_idxs[i+6]];
    sni[7] = send_netin_vec[cg->unit_idxs[i+7]];
    Vec8f sn;
    sn.load(sni);
    sn += dp;
    send_netin_vec[cg->unit_idxs[i+0]] = sn[0];
    send_netin_vec[cg->unit_idxs[i+1]] = sn[1];
    send_netin_vec[cg->unit_idxs[i+2]] = sn[2];
    send_netin_vec[cg->unit_idxs[i+3]] = sn[3];
    send_netin_vec[cg->unit_idxs[i+4]] = sn[4];
    send_netin_vec[cg->unit_idxs[i+5]] = sn[5];
    send_netin_vec[cg->unit_idxs[i+6]] = sn[6];
    send_netin_vec[cg->unit_idxs[i+7]] = sn[7];
  }
  // then do the extra parts that didn't fit in mod 8 bit
  if(parsz < sz)
    send_netin_vec[cg->unit_idxs[parsz]] += wts[parsz] * su_act_delta_eff;
  if(parsz+1 < sz)
    send_netin_vec[cg->unit_idxs[parsz+1]] += wts[parsz+1] * su_act_delta_eff;
  if(parsz+2 < sz)
    send_netin_vec[cg->unit_idxs[parsz+2]] += wts[parsz+2] * su_act_delta_eff;
  if(parsz+3 < sz)
    send_netin_vec[cg->unit_idxs[parsz+3]] += wts[parsz+3] * su_act_delta_eff;
  if(parsz+4 < sz)
    send_netin_vec[cg->unit_idxs[parsz+4]] += wts[parsz+4] * su_act_delta_eff;
  if(parsz+5 < sz)
    send_netin_vec[cg->unit_idxs[parsz+5]] += wts[parsz+5] * su_act_delta_eff;
  if(parsz+6 < sz)
    send_netin_vec[cg->unit_idxs[parsz+6]] += wts[parsz+6] * su_act_delta_eff;
}
#else 
inline void LeabraConSpec::Send_NetinDelta_sse8(LeabraSendCons* cg, 
                                                const float su_act_delta_eff,
                                                float* send_netin_vec, const float* wts) {
  // nop
}
#endif

inline void LeabraConSpec::Send_NetinDelta_impl(LeabraSendCons* cg, LeabraNetwork* net,
                                         const int thread_no, const float su_act_delta,
                                         const float* wts) {
  const float su_act_delta_eff = cg->scale_eff * su_act_delta;
  if(net->NetinPerPrjn()) { // always uses send_netin_tmp -- thread_no auto set to 0 in parent call if no threads
    float* send_netin_vec = net->send_netin_tmp.el
      + net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
#ifdef USE_SSE8                 // only faster on very recent ivy bridge machines
    Send_NetinDelta_sse8(cg, su_act_delta_eff, send_netin_vec, wts);
#else
    CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(wts[i], send_netin_vec,
                                                 cg->UnIdx(i), su_act_delta_eff));
#endif
  }
  else {
    // todo: might want to make everything go through tmp for vectorization speed..
    if(thread_no < 0) {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_NoThread(wts[i],
                                          ((LeabraUnit*)cg->Un(i,net))->net_delta,
                                           su_act_delta_eff));
    }
    else {
      float* send_netin_vec = net->send_netin_tmp.el
	+ net->send_netin_tmp.FastElIndex(0, thread_no);
#ifdef USE_SSE8                 // only faster on very recent ivy bridge machines
      Send_NetinDelta_sse8(cg, su_act_delta_eff, send_netin_vec, wts);
#else
      CON_GROUP_LOOP(cg, C_Send_NetinDelta_Thread(wts[i], send_netin_vec,
                                                  cg->UnIdx(i), su_act_delta_eff));
#endif
    }
  }
}

inline void LeabraConSpec::Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                           const int thread_no, const float su_act_delta)
{
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

  float* lwts = cg->OwnCnVar(LWT);
  float* dwts = cg->OwnCnVar(DWT);

  const int sz = cg->size;
  for(int i=0; i<sz; i++) {
    LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
    const float lin_wt = LinFmSigWt(lwts[i]);
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
  float* pdws = cg->OwnCnVar(PDW);
  float* lwts = cg->OwnCnVar(LWT);
  float* swts = cg->OwnCnVar(SWT);

  CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL(wts[i], dwts[i], pdws[i], 
                                                 lwts[i], swts[i]));
  //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
}

//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCAL

// this is not worth the cost and too hard to update with algo changes, such as thr_l_err:

// inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL_sse8(LeabraSendCons* cg,
//                                                          const float clrate,
//                                                          LeabraUnit* su,
//                                                          LeabraNetwork* net) {
//   const float su_act_mult_s = xcal.thr_l_mix * su->avg_m;

//   Vec8f su_avg_s(su->avg_s);
//   Vec8f su_avg_m(su->avg_m);
//   Vec8f su_act_mult(su_act_mult_s);
//   Vec8f s_mix(xcal.s_mix);
//   Vec8f m_mix(xcal.m_mix);
//   Vec8f thr_m_mix(xcal.thr_m_mix);
//   Vec8f clr(clrate);
//   Vec8f d_thr(xcal.d_thr);
//   Vec8f d_rev(xcal.d_rev);
//   Vec8f d_rev_ratio(xcal.d_rev_ratio);
//   Vec8f zeros(0.0f);

//   float* dwts = cg->OwnCnVar(DWT);

//   const int sz = cg->size;
//   const int parsz = 8 * (cg->size / 8);
//   int i;
//   for(i=0; i<parsz; i+=8) {
//     Vec8f dwt;  dwt.load(dwts+i);

//     float rus[8];    float rum[8];    float rul[8];
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+0,net);
//       rus[0] = ru->avg_s;  rum[0] = ru->avg_m;  rul[0] = ru->avg_l; }
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+1,net);
//       rus[1] = ru->avg_s;  rum[1] = ru->avg_m;  rul[1] = ru->avg_l; }
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+2,net);
//       rus[2] = ru->avg_s;  rum[2] = ru->avg_m;  rul[2] = ru->avg_l; }
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+3,net);
//       rus[3] = ru->avg_s;  rum[3] = ru->avg_m;  rul[3] = ru->avg_l; }
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+4,net);
//       rus[4] = ru->avg_s;  rum[4] = ru->avg_m;  rul[4] = ru->avg_l; }
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+5,net);
//       rus[5] = ru->avg_s;  rum[5] = ru->avg_m;  rul[5] = ru->avg_l; }
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+6,net);
//       rus[6] = ru->avg_s;  rum[6] = ru->avg_m;  rul[6] = ru->avg_l; }
//     { LeabraUnit* ru = (LeabraUnit*)cg->Un(i+7,net);
//       rus[7] = ru->avg_s;  rum[7] = ru->avg_m;  rul[7] = ru->avg_l; }

//     Vec8f ru_avg_s;     Vec8f ru_avg_m;      Vec8f ru_avg_l;
//     ru_avg_s.load(rus); ru_avg_m.load(rum);  ru_avg_l.load(rul);

//     Vec8f srs = ru_avg_s * su_avg_s;
//     Vec8f srm = ru_avg_m * su_avg_m;
//     Vec8f sm_mix = s_mix * srs + m_mix * srm;
//     Vec8f lthr = su_act_mult * ru_avg_l;
//     Vec8f effthr = thr_m_mix * srm + lthr;

//     //    dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
//     Vec8f rval = select(sm_mix > effthr * d_rev, sm_mix - effthr, sm_mix * d_rev_ratio);
//     rval = select(sm_mix < d_thr, zeros, rval);
    
//     dwt += clr * rval;
//     dwt.store(dwts+i);
//   }
//   for(;i<sz;i++) {              // get the remainder
//     LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
//     C_Compute_dWt_CtLeabraXCAL_trial(dwts[i], clrate, ru->avg_s, ru->avg_m, ru->avg_l,
//                                      su->avg_s, su->avg_m, su_act_mult_s);
//   }
// }

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) {
  if(ignore_unlearnable && net->unlearnable_trial) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)su->GetUnitSpec();
  if(su->avg_s < us->opt_thresh.xcal_lrn && su->avg_m < us->opt_thresh.xcal_lrn) return;
  // no need to learn!
  float clrate = cur_lrate;
  if(stable_mix.cos_diff_lrate) {
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    clrate *= rlay->cos_diff_lrate;
  }

  const float su_avg_s = su->avg_s;
  const float su_avg_m = su->avg_m;
  float* dwts = cg->OwnCnVar(DWT);
  const int sz = cg->size;

  if(xcal.l_mix == XCalLearnSpec::X_COS_DIFF) {
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    float rlay_cos_diff_avg = 1.0f - rlay->cos_diff_avg;
    if(rlay->layer_type != Layer::HIDDEN)
      rlay_cos_diff_avg = 0.0f; // any kind of clamped layer should not use this!
    const float efflmix = xcal.thr_l_mix * rlay_cos_diff_avg;
    const float effmmix = 1.0f - efflmix;
    const float su_act_mult = efflmix * su_avg_m;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      C_Compute_dWt_CtLeabraXCAL_cosdiff_trial(dwts[i], clrate, ru->avg_s,
                                    ru->avg_m, ru->avg_l, su_avg_s, su_avg_m,
                                               su_act_mult, effmmix);
    }
  }
  else if(xcal.l_mix == XCalLearnSpec::X_ERR) {
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      C_Compute_dWt_CtLeabraXCAL_thrlerr_trial(dwts[i], clrate, ru->avg_s,
                                  ru->avg_m, ru->avg_l, su_avg_s, su_avg_m);
    }
  }
  else {                        // L_MIX
    const float su_act_mult = xcal.thr_l_mix * su_avg_m;
    for(int i=0; i<sz; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
      C_Compute_dWt_CtLeabraXCAL_trial(dwts[i], clrate, ru->avg_s, ru->avg_m, ru->avg_l,
                                       su_avg_s, su_avg_m, su_act_mult);
    }
  }
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraXCAL

// this turns out to not be worth it, especially for no dwt_norm case, because
// we cannot take advantage of skipping all the dwt = 0 cases..

// inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL_sse8(LeabraSendCons* cg,
//                                                              LeabraUnit* su,
//                                                              LeabraNetwork* net) {
//   float* wts = cg->OwnCnVar(WT);
//   float* dwts = cg->OwnCnVar(DWT);
//   float* pdws = cg->OwnCnVar(PDW);
//   float* lwts = cg->OwnCnVar(LWT);
//   float* swts = cg->OwnCnVar(SWT);

//   Vec8f ones(1.0f);
//   Vec8f zeros(0.0f);
//   Vec8f invgn(1.0f / 6.0f);
//   Vec8f stable_pct(stable_mix.stable_pct);
//   Vec8f learn_pct(stable_mix.learn_pct);
//   Vec8f inv_res_inv(wt_sig_fun_inv.res_inv);
//   Vec8f sig_res_inv(wt_sig_fun.res_inv);
  
//   const int sz = cg->size;
//   const int parsz = 8 * (cg->size / 8);
//   int i;
//   for(i=0; i<parsz; i+=8) {
//     Vec8f wt;  wt.load(wts+i);
//     Vec8f dwt; dwt.load(dwts+i);
//     Vec8f pdw; pdw.load(pdws+i);
//     Vec8f lwt; lwt.load(lwts+i);
//     Vec8f swt; swt.load(swts+i);

//     Vec8i idx = truncate_to_int(lwt * inv_res_inv); // min is 0
//     Vec8f lin_wt = lookup<100002>(idx, wt_sig_fun_inv.el);

//     dwt *= select(dwt > 0.0f, 1.0f - lin_wt, lin_wt);
//     Vec8f nwt = lin_wt + dwt;

//     idx = truncate_to_int(nwt * sig_res_inv); // min is 0
//     lwt = lookup<100002>(idx, wt_sig_fun.el);

//     wt = stable_pct * swt + learn_pct * lwt;
//     pdw = dwt;
//     dwt = zeros;

//     wt.store(wts+i);
//     dwt.store(dwts+i);
//     pdw.store(pdws+i);
//     lwt.store(lwts+i);
//   }
//   for(;i<sz;i++) {              // get the remainder
//     C_Compute_Weights_CtLeabraXCAL(wts[i], dwts[i], pdws[i], 
//                                    lwts[i], swts[i]);
//   }
// }

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg,
                                                        LeabraUnit* su,
                                                        LeabraNetwork* net) {

// #ifdef USE_SSE8
//   Compute_Weights_CtLeabraXCAL_sse8(cg, su, net);
// #else
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* pdws = cg->OwnCnVar(PDW);
  float* lwts = cg->OwnCnVar(LWT);
  float* swts = cg->OwnCnVar(SWT);

  const int sz = cg->size;
  for(int i=0;i <sz; i++) {
    float& wt = wts[i];
    float& dwt = dwts[i];
    float& pdw = pdws[i];
    float& lwt = lwts[i];
    const float& swt = swts[i];
    if(dwt != 0.0f) {
      float lin_wt = LinFmSigWt(lwt);
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
      lwt = SigFmLinWt(lin_wt + dwt);
      C_Compute_EffWt(wt, swt, lwt);
    }
    pdw = dwt;
    dwt = 0.0f;
  }

  // it doesn't seem to be inlining this one, so doing it manually
  // CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL(wts[i], dwts[i], pdws[i], 
  //                                                   lwts[i], swts[i]));
  //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
// #endif
}


//////////////////////////////////////////////////////////////////////////////////
//     Computing dWt: CtLeabra_XCalC -- receiver based for triggered learning..

inline void LeabraConSpec::Compute_dWt_CtLeabraXCalC(LeabraRecvCons* cg, LeabraUnit* ru,
                                                     LeabraNetwork* net) {
  float ru_avg_s = ru->avg_s;
  float ru_avg_m = ru->avg_m;
  float ru_avg_l = ru->avg_l;
  const int sz = cg->size;
  for(int i=0; i<sz; i++) {
    LeabraUnit* su = (LeabraUnit*)cg->Un(i,net);
    C_Compute_dWt_CtLeabraXCalC(cg->PtrCn(i,DWT,net), ru_avg_s, ru_avg_m, ru_avg_l,
                                su->avg_s, su->avg_m);
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
//	Compute_Weights_CtLeabraCAL

inline void LeabraConSpec::Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                       LeabraNetwork* net) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* pdws = cg->OwnCnVar(PDW);
  float* lwts = cg->OwnCnVar(LWT);
  float* swts = cg->OwnCnVar(SWT);

  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraCAL(wts[i], dwts[i], pdws[i], 
                                                   lwts[i], swts[i]));
  //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
}


/////////////////////////////////////
// Master dWt function

inline void LeabraConSpec::Compute_Leabra_dWt(LeabraSendCons* cg, LeabraUnit* su,
                                              LeabraNetwork* net) {
  if(!learn) return;
  switch(learn_rule) {
  case LEABRA_CHL:
    Compute_dWt_LeabraCHL(cg, su, net);
    break;
  case CTLEABRA_CAL:
    Compute_dWt_CtLeabraCAL(cg, su, net);
    break;
  case CTLEABRA_XCAL:
  case CTLEABRA_XCAL_C:
    Compute_dWt_CtLeabraXCAL(cg, su, net);
    break;
  }
}

inline void LeabraConSpec::Compute_Leabra_Weights(LeabraSendCons* cg, LeabraUnit* su,
                                                  LeabraNetwork* net) {
  if(!learn) return;
  switch(learn_rule) {
  case LEABRA_CHL:
    Compute_Weights_LeabraCHL(cg, su, net);
    break;
  case CTLEABRA_CAL:
    Compute_Weights_CtLeabraCAL(cg, su, net);
    break;
  case CTLEABRA_XCAL:
  case CTLEABRA_XCAL_C:
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
  float& dwt = bias->OwnCn(0,DWT);
  float& pdw = bias->OwnCn(0,PDW);
  if(dwt != 0.0f) {
    float& lwt = bias->OwnCn(0,LWT);
    lwt += dwt;
    C_Compute_EffWt(bias->OwnCn(0,WT), bias->OwnCn(0,SWT), lwt);
  }
  pdw = dwt;
  dwt = 0.0f;
  C_ApplyLimits(bias->OwnCn(0,WT), ru, NULL);
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

inline void LeabraConSpec::B_Compute_Leabra_dWt(RecvCons* bias, LeabraUnit* ru,
                                                LeabraLayer* rlay) {
  if(!learn) return;
  switch(learn_rule) {
  case LEABRA_CHL:
    B_Compute_dWt_LeabraCHL(bias, ru);
    break;
  case CTLEABRA_CAL:
    B_Compute_dWt_CtLeabraCAL(bias, ru, rlay);
    break;
  case CTLEABRA_XCAL:
  case CTLEABRA_XCAL_C:
    B_Compute_dWt_CtLeabraXCAL(bias, ru, rlay);
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
