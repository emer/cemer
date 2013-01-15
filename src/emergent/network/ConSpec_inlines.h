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

inline void ConSpec::ApplyLimits(RecvCons* cg, Unit* ru) {
  if(wt_limits.type != WeightLimits::NONE) {
    CON_GROUP_LOOP(cg, C_ApplyLimits(cg->Cn(i), ru, cg->Un(i)));
  }
}

inline void ConSpec::C_Init_Weights(RecvCons*, Connection* cn, Unit* ru, Unit* su) {
  if(rnd.type != Random::NONE)  { // don't do anything (e.g. so connect fun can do it)
    cn->wt = rnd.Gen();
  }
  else {
    rnd.Gen();          // keep random seeds synchronized for dmem
  }
  C_ApplyLimits(cn,ru,su);
}

inline void ConSpec::C_AddRndWeights(RecvCons*, Connection* cn, Unit* ru, Unit* su, float scale) {
  if(rnd.type != Random::NONE)  { // don't do anything (e.g. so connect fun can do it)
    cn->wt += scale * rnd.Gen();
  }
  else {
    rnd.Gen();          // keep random seeds synchronized for dmem
  }
  C_ApplyLimits(cn,ru,su);
}

inline void ConSpec::Init_Weights(RecvCons* cg, Unit* ru) {
  Projection* prjn = cg->prjn;
  if(prjn->spec->init_wts) {
    prjn->C_Init_Weights(cg, ru); // NOTE: this must call PrjnSpec::C_Init_Weights which does basic ConSpec C_Init_Weights
    if(prjn->spec->add_rnd_wts) {
      float scl = prjn->spec->add_rnd_wts_scale;
      CON_GROUP_LOOP(cg, C_AddRndWeights(cg, cg->Cn(i), ru, cg->Un(i), scl));
    }
  }
  else {
    CON_GROUP_LOOP(cg, C_Init_Weights(cg, cg->Cn(i), ru, cg->Un(i)));
  }

  Init_dWt(cg,ru);
  ApplySymmetry(cg,ru);
}

inline void ConSpec::Init_Weights_post(BaseCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Init_Weights_post(cg, cg->Cn(i), ru, cg->Un(i)));
}

inline void ConSpec::Init_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Init_dWt(cg, cg->Cn(i), ru, cg->Un(i)));
}

inline float ConSpec::C_Compute_Netin(Connection* cn, Unit*, Unit* su) {
  return cn->wt * su->act;
}
inline float ConSpec::Compute_Netin(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->OwnCn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::C_Send_Netin(Connection* cn, float* send_netin_vec, Unit* ru, float su_act) {
  send_netin_vec[ru->flat_idx] += cn->wt * su_act;
}
inline void ConSpec::Send_Netin(SendCons* cg, Network* net, int thread_no, Unit* su) {
  const float su_act = su->act;
  float* send_netin_vec = net->send_netin_tmp.el + net->send_netin_tmp.FastElIndex(0, thread_no);
  CON_GROUP_LOOP(cg, C_Send_Netin(cg->OwnCn(i), send_netin_vec, cg->Un(i), su_act));
}

inline void ConSpec::Send_Netin_PerPrjn(SendCons* cg, Network* net, int thread_no, Unit* su) {
  const float su_act = su->act;
  float* send_netin_vec = net->send_netin_tmp.el +
    net->send_netin_tmp.FastElIndex(0, cg->recv_idx(), thread_no);
  CON_GROUP_LOOP(cg, C_Send_Netin(cg->OwnCn(i), send_netin_vec, cg->Un(i), su_act));
}

inline float ConSpec::C_Compute_Dist(Connection* cn, Unit*, Unit* su) {
  float tmp = su->act - cn->wt;
  return tmp * tmp;
}
inline float ConSpec::Compute_Dist(RecvCons* cg, Unit* ru) {
  float rval=0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_Dist(cg->OwnCn(i), ru, cg->Un(i)));
  return rval;
}

inline void ConSpec::Compute_Weights(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Weights(cg->OwnCn(i), ru, cg->Un(i)));
  ApplyLimits(cg,ru); // ApplySymmetry(cg,ru);  don't apply symmetry during learning..
}

inline void ConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt(cg->OwnCn(i), ru, cg->Un(i)));
}

#endif // ConSpec_inlines_h
