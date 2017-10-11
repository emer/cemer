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

#include "LeabraMultCopyUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(LeabraMultCopyUnitSpec);

void LeabraMultCopyUnitSpec::Initialize() {
  one_minus = false;
  mult_gain = 1.0f;
}

bool LeabraMultCopyUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;
  bool rval = true;
  
  if(lay->units.leaves == 0) return rval;
  LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0); // take first one
  
  const int nrg = un->NRecvConGps();
  if(lay->CheckError(nrg != 2, quiet, rval,
                "leabra mult copy must have exactly 2 recv prjns, first = act to copy, second = act to multiply")) {
    return false;               // fatal
  }

  LeabraConState_cpp* copy_gp = (LeabraConState_cpp*)un->RecvConState(0);
  if(lay->CheckError(copy_gp->size != 1, quiet, rval,
                "leabra mult copy first prjn (copy act source) must have exactly 1 connection to copy from")) {
    return false;               // fatal
  }
  LeabraConState_cpp* mult_gp = (LeabraConState_cpp*)un->RecvConState(1);
  if(lay->CheckError(mult_gp->size != 1, quiet, rval,
                "leabra mult copy second prjn (mult act source) must have exactly 1 connection to get mult act from")) {
    return false;               // fatal
  }
  return rval;
}

void LeabraMultCopyUnitSpec::Compute_MultCopy(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  LeabraConState_cpp* copy_gp = (LeabraConState_cpp*)u->RecvConState(net, thr_no, 0);
  LeabraConState_cpp* mult_gp = (LeabraConState_cpp*)u->RecvConState(net, thr_no, 1);
  
  LeabraUnitState_cpp* copy_un = (LeabraUnitState_cpp*)copy_gp->UnState(0,net);
  LeabraUnitState_cpp* mult_un = (LeabraUnitState_cpp*)mult_gp->UnState(0,net);

  float mult_eff = mult_gain * mult_un->act_eq;;
  if(mult_eff > 1.0f) mult_eff = 1.0f;
  float new_act;
  if(one_minus)
    new_act = copy_un->act_eq * (1.0f - mult_eff);
  else
    new_act = copy_un->act_eq * mult_eff;
  u->ext = new_act;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;               // I'm fully settled!
}
                                              
void LeabraMultCopyUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_MultCopy(u, net, thr_no);
}

void LeabraMultCopyUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

