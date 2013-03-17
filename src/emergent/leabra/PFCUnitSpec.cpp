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

#include "PFCUnitSpec.h"
#include <LeabraNetwork>
#include <PFCLayerSpec>
#include <PBWMUnGpData>

void PFCUnitSpec::Initialize() {
  Defaults_init();
}

void PFCUnitSpec::Defaults_init() {
  g_bar.h = 0.0f;		// don't use it by default -- use mix
}

bool PFCUnitSpec::PFCStripeGated(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return true; // default is true..
  if(!rlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) return true;
  PFCLayerSpec* ls = (PFCLayerSpec*) rlay->GetLayerSpec();
  int rgpidx = u->UnitGpIdx();
  PBWMUnGpData* gpd = (PBWMUnGpData*)rlay->ungp_data.FastEl(rgpidx);
  return gpd->go_fired_trial;
}

void PFCUnitSpec::TI_Compute_CtxtAct(LeabraUnit* u, LeabraNetwork* net) {
  bool gated = PFCStripeGated(u, net);
  if(gated) {
    u->act_ctxt = u->net_ctxt;
  }
}

void PFCUnitSpec::PostSettle(LeabraUnit* u, LeabraNetwork* net) {
  float save_p_act_p = u->p_act_p;
  inherited::PostSettle(u, net);
  if(net->phase_no == 1) {
    bool gated = PFCStripeGated(u, net);
    if(!gated) {
      u->p_act_p = save_p_act_p; // do not update -- we didn't update!
    }
  }
}
