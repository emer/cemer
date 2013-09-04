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

#include "LeabraMultCopyLayerSpec.h"
#include <LeabraNetwork>


void LeabraMultCopyLayerSpec::Initialize() {
  one_minus = false;
  mult_gain = 1.0f;
}

bool LeabraMultCopyLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "leabra mult copy layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  if(lay->CheckError(u->recv.size != 2, quiet, rval,
                "leabra mult copy layer must have exactly 2 recv prjns, first = act to copy, second = act to multiply:", lay->name)) {
    return false;               // fatal
  }

  LeabraRecvCons* copy_gp = (LeabraRecvCons*)u->recv.FastEl(0);
  if(lay->CheckError(copy_gp->size != 1, quiet, rval,
                "leabra mult copy layer first prjn (copy act source) must have exactly 1 connection to copy from:", lay->name)) {
    return false;               // fatal
  }
  LeabraRecvCons* mult_gp = (LeabraRecvCons*)u->recv.FastEl(1);
  if(lay->CheckError(mult_gp->size != 1, quiet, rval,
                "leabra mult copy layer second prjn (mult act source) must have exactly 1 connection to get mult act from:", lay->name)) {
    return false;               // fatal
  }
  return rval;
}

void LeabraMultCopyLayerSpec::Compute_MultCopyAct(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    if(u->recv.size < 2) continue;

    LeabraRecvCons* copy_gp = (LeabraRecvCons*)u->recv.FastEl(0);
    LeabraRecvCons* mult_gp = (LeabraRecvCons*)u->recv.FastEl(1);

    if(copy_gp->size != 1) continue;
    if(mult_gp->size != 1) continue;

    LeabraUnitSpec* rus = (LeabraUnitSpec*)u->GetUnitSpec();

    LeabraUnit* copy_un = (LeabraUnit*)copy_gp->Un(0,net);
    LeabraUnit* mult_un = (LeabraUnit*)mult_gp->Un(0,net);

    float mult_eff = mult_gain * mult_un->act_eq;;
    if(mult_eff > 1.0f) mult_eff = 1.0f;
    float new_act;
    if(one_minus)
      new_act = copy_un->act_eq * (1.0f - mult_eff);
    else
      new_act = copy_un->act_eq * mult_eff;
    u->act = new_act;
    u->act_lrn = u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;               // I'm fully settled!
    u->AddToActBuf(rus->syn_delay);
  }
}

void LeabraMultCopyLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_MultCopyAct(lay, net);
  inherited::Compute_CycleStats(lay, net);
}
