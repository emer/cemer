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

#ifndef ConSpec_inlines_h
#define ConSpec_inlines_h 1

// parent includes:
#include <Network>

// member includes:

// declare all other types mentioned but not required to include:

// NOTE: all this base-level code assumes it is being called with the 
// con group that owns the connections, as it calls OwnCnVar -- thus
// it is imperitive that the algorithm use this code appropriately!

inline void ConSpec::ApplyLimits(ConGroup* cg, Network* net, int thr_no) {
  float* wts = cg->OwnCnVar(WT);
  if(wt_limits.type != WeightLimits::NONE) {
    CON_GROUP_LOOP(cg, C_ApplyLimits(wts[i]));
  }
}

inline void ConSpec::Init_Weights_symflag(Network* net, int thr_no) {
  if(wt_limits.sym) net->needs_wt_sym = true;
}

inline void ConSpec::Init_Weights(ConGroup* cg, Network* net, int thr_no) {
  Init_Weights_symflag(net, thr_no);

  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);

  if(rnd.type != Random::NONE) {
    for(int i=0; i<cg->size; i++) {
      C_Init_Weight_Rnd(wts[i]);
      C_Init_dWt(dwts[i]);
    }
  }
}

inline void ConSpec::Init_dWt(ConGroup* cg, Network* net, int thr_no) {
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Init_dWt(dwts[i]));
}

inline void ConSpec::Init_Weights_sym_r(ConGroup* cg, Network* net, int thr_no) {
  if(cg->prjn->layer->units_flat_idx < cg->prjn->from->units_flat_idx)
    return;    // higher copies from lower, so if we're lower, bail..
  ApplySymmetry_r(cg, net, thr_no);
}

inline void ConSpec::Init_Weights_sym_s(ConGroup* cg, Network* net, int thr_no) {
  if(cg->prjn->layer->units_flat_idx < cg->prjn->from->units_flat_idx)
    return;    // higher copies from lower, so if we're lower, bail..
  ApplySymmetry_s(cg, net, thr_no);
}

inline float ConSpec::Compute_Netin(ConGroup* cg, Network* net, int thr_no) {
  float rval=0.0f;
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(wts[i], cg->UnVars(i,net)->act));
  // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
  // could be a very fast vector op
  return rval;
}

inline void ConSpec::Send_Netin(ConGroup* cg, Network* net, int thr_no) {
  UnitVars* su = cg->ThrOwnUnVars(net, thr_no);
  const float su_act = su->act;
  float* send_netin_vec = net->ThrSendNetinTmp(thr_no);
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, C_Send_Netin(wts[i], send_netin_vec, cg->UnIdx(i), su_act));
}

inline void ConSpec::Send_Netin_PerPrjn(ConGroup* cg, Network* net, int thr_no) {
  UnitVars* su = cg->ThrOwnUnVars(net, thr_no);
  const float su_act = su->act;
  float* send_netin_vec = net->ThrSendNetinTmpPerPrjn(thr_no, cg->other_idx);
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, C_Send_Netin(wts[i], send_netin_vec, cg->UnIdx(i), su_act));
}

inline float ConSpec::Compute_Dist(ConGroup* cg, Network* net, int thr_no) {
  float rval=0.0f;
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, rval += C_Compute_Dist(wts[i], cg->UnVars(i,net)->act));
  // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
  // could be a very fast vector op
  return rval;
}

inline void ConSpec::Compute_dWt(ConGroup* cg, Network* net, int thr_no) {
  UnitVars* ru = cg->ThrOwnUnVars(net, thr_no);
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  const float ru_act = ru->act; // assume recv based
  CON_GROUP_LOOP(cg, C_Compute_dWt(wts[i], dwts[i], ru_act,
                                   cg->UnVars(i,net)->act));
}

inline void ConSpec::Compute_Weights(ConGroup* cg, Network* net, int thr_no) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Compute_Weights(wts[i], dwts[i]));
  ApplyLimits(cg,net,thr_no); 
  // ApplySymmetry_r(cg,ru);  don't apply symmetry during learning..
}

inline void ConSpec::B_Init_Weights(UnitVars* uv, Network* net, int thr_no) {
  C_Init_Weight_Rnd(uv->bias_wt);
  C_Init_dWt(uv->bias_dwt);
}

inline void ConSpec::B_Init_dWt(UnitVars* uv, Network* net, int thr_no) {
  C_Init_dWt(uv->bias_dwt);
}

inline void ConSpec::B_Compute_dWt(UnitVars* uv, Network* net, int thr_no) {
  // todo: learning rule here..
}

inline void ConSpec::B_Compute_Weights(UnitVars* uv, Network* net, int thr_no) {
  C_Compute_Weights(uv->bias_wt, uv->bias_dwt);
}

#endif // ConSpec_inlines_h
