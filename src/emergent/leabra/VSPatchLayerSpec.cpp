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

#include "VSPatchLayerSpec.h"

#include <LeabraNetwork>
#include <MarkerConSpec>
#include <LeabraDeltaConSpec>

#include <taMisc>

void VSPatchLayerSpec::Initialize() {
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void VSPatchLayerSpec::Defaults_init() {
}

void VSPatchLayerSpec::HelpConfig() {
  String help = "VSPatchLayerSpec (PV driven VS units) Computation:\n\
 - Learns from PV values clamped in plus phase.\n\
 \nVSPatchLayerSpec Configuration:\n\
 - Use the Wizard gdPVLV button to automatically configure layers.\n\
 - Recv cons from PV marked with a MarkerConSpec";
  taMisc::Confirm(help);
}

bool VSPatchLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  LeabraLayer* pv_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      pv_lay = flay;
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_LeabraDeltaConSpec), quiet, rval,
                  "requires recv connections to be of type LeabraDeltaConSpec")) {
      return false;
    }
  }
  if(lay->CheckError(!pv_lay, quiet, rval,
                "did not find MarkerConSpec con from PV driver layer")) {
    rval = false;
  }
  return rval;
}

void VSPatchLayerSpec::Compute_PVPlus(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    float pv_val = 0.0f;
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
        pv_val = recv_gp->Un(0,net)->act;
        continue;
      }
    }
    u->act_p = pv_val;
  }
}

void VSPatchLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    Compute_PVPlus(lay, net);
  }
}

