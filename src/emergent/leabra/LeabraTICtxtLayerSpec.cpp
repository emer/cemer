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

#include "LeabraTICtxtLayerSpec.h"
#include <LeabraNetwork>
#include <LayerActUnitSpec>

void LeabraTICtxtLayerSpec::Initialize() {
  act_val = P_ACT_P;
  Defaults_init();
}

void LeabraTICtxtLayerSpec::Defaults_init() {
}

bool LeabraTICtxtLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->CheckError(!lay->GetUnitSpec()->InheritsFrom(&TA_LayerActUnitSpec), 
                     quiet, rval,
                     "Should use LayerActUnitSpec unit specs to optimize computational costs")) {
    // todo: could try to fix this..
  }
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
  if(lay->CheckError(!cg, quiet, rval,
                     "Requires one recv projection!")) {
    return false;
  }
  LeabraUnit* su = (LeabraUnit*)cg->Un(0);
  if(lay->CheckError(!su, quiet, rval, 
                     "Requires one unit in recv projection!")) {
    return false;
  }

  return rval;
}

void LeabraTICtxtLayerSpec::Compute_ActFmSource(LeabraLayer* lay, LeabraNetwork* net) {
  lay->Inhib_SetVals(inhib.kwta_pt);            // assume 0 - 1 clamped inputs
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(!cg) return;
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(!su) return;
    LeabraUnitSpec* rus = (LeabraUnitSpec*)u->GetUnitSpec();
    if(act_val == P_ACT_P) {
      u->act = su->p_act_p;
    }
    else {
      u->act = su->act_ctxt;
    }
    u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;            // I'm fully settled!
    u->AddToActBuf(rus->syn_delay);
  }
}

void LeabraTICtxtLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_ActFmSource(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

