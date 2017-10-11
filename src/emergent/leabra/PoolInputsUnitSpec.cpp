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

#include "PoolInputsUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(PoolInputsUnitSpec);

void PoolInputsUnitSpec::Initialize() {
  pool_fun = MAX_POOL;
  Defaults_init();
}

void PoolInputsUnitSpec::Defaults_init() {
}

bool PoolInputsUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Unit(lay, quiet);

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->units.leaves == 0) return rval;
  LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0); // take first one
  
  LeabraConState_cpp* cg = (LeabraConState_cpp*)un->RecvConStateSafe(0);
  if(lay->CheckError(!cg, quiet, rval,
                   "Requires one recv projection!")) {
    return false;
  }
  LeabraLayer* fmlay = (LeabraLayer*)cg->prjn->from.ptr();
  if(lay->CheckError(fmlay->lesioned(), quiet, rval,
                   "Sending layer is lesioned -- we should be lesioned too!")) {
    return false;
  }

  return rval;
}

void PoolInputsUnitSpec::Compute_PooledAct(LeabraUnitState_cpp* u, LeabraNetwork* net,
                                           int thr_no) {
  float new_act = 0.0f;
  int tot_n = 0;
  const int rsz = u->NRecvConGps(net);
  for(int g=0; g < rsz; g++) {
    LeabraConState_cpp* cg = (LeabraConState_cpp*)u->RecvConState(net, g);
    const int sz = cg->size;
    if(pool_fun == MAX_POOL) {
      for(int i=0; i< sz; i++) {
        LeabraUnitState_cpp* su = (LeabraUnitState_cpp*)cg->UnState(i, net);
        new_act = fmaxf(su->act_eq, new_act);
      }
    }
    else {                        // AVG_POOL
      for(int i=0; i< sz; i++) {
        LeabraUnitState_cpp* su = (LeabraUnitState_cpp*)cg->UnState(i, net);
        new_act += su->act_eq;
      }
      tot_n += sz;
    }
  }

  if(pool_fun == AVG_POOL) {
    if(tot_n > 0) {
      new_act /= (float)tot_n;
    }
  }
    
  u->act = new_act;
  u->act_eq = u->act_nd = u->act;
  u->da = 0.0f;
  // u->AddToActBuf(syn_delay); // todo
}

void PoolInputsUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_PooledAct(u, net, thr_no);
}

void PoolInputsUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_PooledAct(u, net, thr_no);
}

