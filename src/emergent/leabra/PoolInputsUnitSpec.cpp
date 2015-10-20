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

#include "PoolInputsUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(PoolInputsUnitSpec);

void PoolInputsUnitSpec::Initialize() {
  pool_fun = MAX_POOL;
  Defaults_init();
}

void PoolInputsUnitSpec::Defaults_init() {
}

bool PoolInputsUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  LeabraUnit* u = (LeabraUnit*)un;
  bool rval = inherited::CheckConfig_Unit(un, quiet);

  LeabraNetwork* net = (LeabraNetwork*)un->own_net();

  LeabraConGroup* cg = (LeabraConGroup*)u->RecvConGroupSafe(0);
  if(u->CheckError(!cg, quiet, rval,
                   "Requires one recv projection!")) {
    return false;
  }
  LeabraLayer* lay = (LeabraLayer*)cg->prjn->layer;
  if(!lay->lesioned()) {        // probably we're not called if lesioned, but just in case
    LeabraLayer* fmlay = (LeabraLayer*)cg->prjn->from.ptr();
    if(u->CheckError(fmlay->lesioned(), quiet, rval,
                     "Sending layer is lesioned -- we should be lesioned too!")) {
      return false;
    }
  }

  return rval;
}

void PoolInputsUnitSpec::Compute_PooledAct(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  LeabraConGroup* cg = (LeabraConGroup*)u->RecvConGroupSafe(net, thr_no, 0);
  const int sz = cg->size;
  float new_act = 0.0f;
  if(pool_fun == MAX_POOL) {
    for(int i=0; i< sz; i++) {
      LeabraUnitVars* su = (LeabraUnitVars*)cg->UnVars(i, net);
      new_act = MAX(su->act_eq, new_act);
    }
  }
  else {                        // AVG_POOL
    for(int i=0; i< sz; i++) {
      LeabraUnitVars* su = (LeabraUnitVars*)cg->UnVars(i, net);
      new_act += su->act_eq;
    }
    if(sz > 0) {
      new_act /= (float)sz;
    }
  }
    
  u->act = new_act;
  u->act_eq = u->act_nd = u->act;
  TestWrite(u->da, 0.0f);
  // u->AddToActBuf(syn_delay); // todo
}

void PoolInputsUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_PooledAct(u, net, thr_no);
}

void PoolInputsUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_PooledAct(u, net, thr_no);
}

