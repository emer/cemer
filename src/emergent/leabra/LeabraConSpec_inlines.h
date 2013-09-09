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
#ifdef USE_SSE4
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


#ifdef USE_SSE4
inline void LeabraConSpec::Send_NetinDelta_sse4(LeabraSendCons* cg, 
                                                const float su_act_delta_eff,
                                                float* send_netin_vec, const float* wts)
{
  Vec4f sa(su_act_delta_eff);
  const int parsz = 4 * (cg->size / 4);
  for(int i=0; i<parsz; i+=4) {
    Vec4f wv;
    wv.load(wts+i);
    Vec4f dp = wv * sa;
    float sni[4];
    sni[0] = send_netin_vec[cg->unit_idxs[i+0]];
    sni[1] = send_netin_vec[cg->unit_idxs[i+1]];
    sni[2] = send_netin_vec[cg->unit_idxs[i+2]];
    sni[3] = send_netin_vec[cg->unit_idxs[i+3]];
    Vec4f sn;
    sn.load(sni);
    sn += dp;
    send_netin_vec[cg->unit_idxs[i+0]] = sn[0];
    send_netin_vec[cg->unit_idxs[i+1]] = sn[1];
    send_netin_vec[cg->unit_idxs[i+2]] = sn[2];
    send_netin_vec[cg->unit_idxs[i+3]] = sn[3];
  }
  // then do the extra parts that didn't fit in mod 4 bit
  const int sz = cg->size;
  if(parsz < sz)
    send_netin_vec[cg->unit_idxs[parsz]] += wts[parsz] * su_act_delta_eff;
  if(parsz+1 < sz)
    send_netin_vec[cg->unit_idxs[parsz+1]] += wts[parsz+1] * su_act_delta_eff;
  if(parsz+2 < sz)
    send_netin_vec[cg->unit_idxs[parsz+2]] += wts[parsz+2] * su_act_delta_eff;
}
#else 
inline void LeabraConSpec::Send_NetinDelta_sse4(LeabraSendCons* cg, 
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
#ifdef USE_SSE4                 // only faster on very recent ivy bridge machines
    Send_NetinDelta_sse4(cg, su_act_delta_eff, send_netin_vec, wts);
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
#ifdef USE_SSE4                 // only faster on very recent ivy bridge machines
      Send_NetinDelta_sse4(cg, su_act_delta_eff, send_netin_vec, wts);
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

inline void LeabraConSpec::Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) {
  if(ignore_unlearnable && net->unlearnable_trial) return;

  float su_avg_s = su->avg_s;
  float su_avg_m = su->avg_m;
  float su_act_mult = xcal.thr_l_mix * su->avg_m;

  float* dwts = cg->OwnCnVar(DWT);

  const int sz = cg->size;
  for(int i=0; i<sz; i++) {
    LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
    C_Compute_dWt_CtLeabraXCAL_trial(dwts[i], ru->avg_s, ru->avg_m, ru->avg_l,
                                     su_avg_s, su_avg_m, su_act_mult);
  }
}

/////////////////////////////////////
//	Compute_Weights_CtLeabraXCAL

inline void LeabraConSpec::Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg,
                                                        LeabraUnit* su,
                                                        LeabraNetwork* net) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  float* pdws = cg->OwnCnVar(PDW);
  float* lwts = cg->OwnCnVar(LWT);
  float* swts = cg->OwnCnVar(SWT);

  CON_GROUP_LOOP(cg, C_Compute_Weights_CtLeabraXCAL(wts[i], dwts[i], pdws[i], 
                                                    lwts[i], swts[i]));
  //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
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
