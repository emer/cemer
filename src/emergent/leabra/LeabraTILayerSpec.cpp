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

#include "LeabraTILayerSpec.h"
#include <LeabraNetwork>
#include <LayerActUnitSpec>

void LeabraTILayerSpec::Initialize() {
  lamina = DEEP;
  Defaults_init();
}

void LeabraTILayerSpec::Defaults_init() {
  decay.event = 0.0f;
  inhib.kwta_pt = 0.6f;
  kwta.pct = 0.15f;
}

void LeabraTILayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // ti.UpdateAfterEdit_NoGui();
}

bool LeabraTILayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->CheckError(net->no_plus_test, quiet, rval,
                "requires LeabraNetwork no_plus_test = false, I just set it for you")) {
    net->no_plus_test = false;
  }

  if(lamina == DEEP) {
    if(lay->CheckError(!lay->GetUnitSpec()->InheritsFrom(&TA_LayerActUnitSpec), 
		       quiet, rval,
		       "Deep layers should use LayerActUnitSpec unit specs to optimize computational costs")) {
      // todo: could try to fix this..
    }
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(lay->CheckError(!cg, quiet, rval,
		       "Deep layer requires one recv projection!")) {
      return false;
    }
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(lay->CheckError(!su, quiet, rval, 
		       "Deep layer requires one unit in recv projection!")) {
      return false;
    }
  }

  return rval;
}

void LeabraTILayerSpec::Clear_Maint(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->maint_h = 0.0f;
  }
}

void LeabraTILayerSpec::Compute_MaintFmSuper(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(!cg) return;
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(!su) return;
    u->maint_h = su->act;
  }
}

void LeabraTILayerSpec::Compute_ActFmSuper(LeabraLayer* lay, LeabraNetwork* net) {
  lay->Inhib_SetVals(inhib.kwta_pt);            // assume 0 - 1 clamped inputs
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(!cg) return;
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(!su) return;
    LeabraUnitSpec* rus = (LeabraUnitSpec*)u->GetUnitSpec();
    u->act = su->act;
    u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;            // I'm fully settled!
    u->AddToActBuf(rus->syn_delay);
  }
}

void LeabraTILayerSpec::Compute_ActFmMaint(LeabraLayer* lay, LeabraNetwork* net) {
  lay->Inhib_SetVals(inhib.kwta_pt);            // assume 0 - 1 clamped inputs
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    LeabraUnitSpec* rus = (LeabraUnitSpec*)u->GetUnitSpec();
    u->act = u->maint_h;
    u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;            // I'm fully settled!
    u->AddToActBuf(rus->syn_delay);
  }
}

void LeabraTILayerSpec::Compute_TIAct(LeabraLayer* lay, LeabraNetwork* net) {
  if(lamina == DEEP) {
    Compute_ActFmMaint(lay, net); // always fix to maintained value
  }
  // super is a NOP
}

void LeabraTILayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_TIAct(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void LeabraTILayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  if(lamina == DEEP) {
    if(net->phase_no == 1) {
      Compute_MaintFmSuper(lay, net); // grab and copy for next trial
    }
  }
  inherited::PostSettle(lay, net);
}
