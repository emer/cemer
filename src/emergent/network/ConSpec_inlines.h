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

// NOTE: most computationally intensive of these are written in
// optimized form assuming that the recv group owns the connections
// which is the default for most algos -- if using sender-own, then
// DEFINITELY need to re-write!!

inline void ConSpec::ApplyLimits(RecvCons* cg, Unit* ru, Network* net) {
  if(wt_limits.type != WeightLimits::NONE) {
    CON_GROUP_LOOP(cg, C_ApplyLimits(cg->Cn(i,WT,net), ru, cg->Un(i,net)));
  }
}

inline void ConSpec::C_Init_Weights(RecvCons* cg, const int idx, Unit* ru, Unit* su,
                                    Network* net) {
  float& wt = cg->Cn(idx, WT, net);
  if(rnd.type != Random::NONE)  { // don't do anything (e.g. so connect fun can do it)
    wt = rnd.Gen();
  }
  else {
    rnd.Gen();          // keep random seeds synchronized for dmem
  }
  C_ApplyLimits(wt,ru,su);
}

inline void ConSpec::C_AddRndWeights(RecvCons* cg, const int idx, Unit* ru, Unit* su,
                                     const float scale, Network* net) {
  float& wt = cg->Cn(idx, WT, net);
  if(rnd.type != Random::NONE)  { // don't do anything (e.g. so connect fun can do it)
    wt += scale * rnd.Gen();
  }
  else {
    rnd.Gen();          // keep random seeds synchronized for dmem
  }
  C_ApplyLimits(wt,ru,su);
}

inline void ConSpec::Init_Weights(RecvCons* cg, Unit* ru, Network* net) {
  Projection* prjn = cg->prjn;
  if(prjn->spec->init_wts) {
    prjn->C_Init_Weights(cg, ru); // NOTE: this must call PrjnSpec::C_Init_Weights which does basic ConSpec C_Init_Weights
    if(prjn->spec->add_rnd_wts) {
      const float scl = prjn->spec->add_rnd_wts_scale;
      CON_GROUP_LOOP(cg, C_AddRndWeights(cg, i, ru, cg->Un(i,net), scl, net));
    }
  }
  else {
    CON_GROUP_LOOP(cg, C_Init_Weights(cg, i, ru, cg->Un(i,net), net));
  }

  Init_dWt(cg,ru,net);
  ApplySymmetry(cg,ru,net);
}

inline void ConSpec::Init_Weights_post(BaseCons* cg, Unit* ru, Network* net) {
  CON_GROUP_LOOP(cg, C_Init_Weights_post(cg, i, ru, cg->Un(i,net), net));
}

inline void ConSpec::C_Init_dWt(RecvCons* cg, const int idx, Unit*, Unit*,
                                Network* net) {
  cg->Cn(idx,DWT,net) = 0.0f;
}

inline void ConSpec::Init_dWt(RecvCons* cg, Unit* ru, Network* net) {
  CON_GROUP_LOOP(cg, C_Init_dWt(cg, i, ru, cg->Un(i,net), net));
}

inline float ConSpec::Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
  float rval=0.0f;
  float* wts = cg->OwnCnVar(WT);
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(wts[i], cg->Un(i,net)->act));
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
  return rval;
}

inline void ConSpec::Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
  // assumes recv based owner
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  const float ru_act = ru->act;
  CON_GROUP_LOOP(cg, C_Compute_dWt(wts[i], dwts[i], ru_act, cg->Un(i,net)->act));
}

inline void ConSpec::Compute_Weights(RecvCons* cg, Unit* ru, Network* net) {
  // assumes recv based owner
  float* wts = cg->OwnCnVar(WT);
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Compute_Weights(wts[i], dwts[i]));
  ApplyLimits(cg,ru,net); // ApplySymmetry(cg,ru);  don't apply symmetry during learning..
}

#endif // ConSpec_inlines_h
