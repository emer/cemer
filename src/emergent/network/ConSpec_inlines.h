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

inline void ConSpec::ApplyLimits(BaseCons* cg, Unit* un, Network* net) {
  float* wts = cg->OwnCnVar(WT);
  if(wt_limits.type != WeightLimits::NONE) {
    CON_GROUP_LOOP(cg, C_ApplyLimits(wts[i]));
  }
}

inline void ConSpec::Init_Weights_symflag(Network* net) {
  if(wt_limits.sym) net->needs_wt_sym = true;
}

inline void ConSpec::Init_Weights(BaseCons* cg, Unit* un, Network* net) {
  Init_Weights_symflag(net);

  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);

  if(rnd.type != Random::NONE) {
    for(int i=0; i<cg->size; i++) {
      C_Init_Weight_Rnd(wts[i]);
      C_Init_dWt(dwts[i]);
    }
  }
}

inline void ConSpec::Init_dWt(BaseCons* cg, Unit* un, Network* net) {
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Init_dWt(dwts[i]));
}

inline void ConSpec::B_Init_dWt(RecvCons* cg, Unit* ru, Network* net) {
  C_Init_dWt(cg->OwnCn(0, BaseCons::DWT));
}

inline void ConSpec::B_Init_Weights(RecvCons* cg, Unit* ru, Network* net) {
  C_Init_Weight_Rnd(cg->OwnCn(0, BaseCons::WT));
  B_Init_dWt(cg, ru, net);          // virtual..
}

inline float ConSpec::Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
  float rval=0.0f;
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(wts[i], cg->Un(i,net)->act));
  // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
  // could be a very fast vector op
  return rval;
}

inline void ConSpec::Send_Netin(SendCons* cg, Network* net, const int thread_no,
                                Unit* su) {
  // assumes sender-based own-cons
  const float su_act = su->act;
  float* send_netin_vec = net->send_netin_tmp.el +
    net->send_netin_tmp.FastElIndex(0, thread_no);
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, C_Send_Netin(wts[i], send_netin_vec, cg->Un(i,net)->flat_idx,
                                  su_act));
}

inline void ConSpec::Send_Netin_PerPrjn(SendCons* cg, Network* net,
                                        const int thread_no, Unit* su) {
  const float su_act = su->act;
  float* send_netin_vec = net->send_netin_tmp.el +
    net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, C_Send_Netin(wts[i], send_netin_vec, cg->Un(i,net)->flat_idx,
                                  su_act));
}

inline float ConSpec::Compute_Dist(RecvCons* cg, Unit* ru, Network* net) {
  float rval=0.0f;
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, rval += C_Compute_Dist(wts[i], cg->Un(i,net)->act));
  // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
  // could be a very fast vector op
  return rval;
}

inline void ConSpec::Compute_dWt(BaseCons* cg, Unit* un, Network* net) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  const float ru_act = un->act; // assume recv based
  CON_GROUP_LOOP(cg, C_Compute_dWt(wts[i], dwts[i], ru_act, cg->Un(i,net)->act));
}

inline void ConSpec::Compute_Weights(BaseCons* cg, Unit* un, Network* net) {
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Compute_Weights(wts[i], dwts[i]));
  ApplyLimits(cg,un,net); // ApplySymmetry_r(cg,ru);  don't apply symmetry during learning..
}

#endif // ConSpec_inlines_h
