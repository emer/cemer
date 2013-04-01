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

///////////////////////////////////////////////////////////
//      time course of p_act_p updates vs. ctxt updates
//      issue is that learning at time t relies on p_act_p reflecting
//      acts at t-1, and yet it gets updated at time t
// time         0       1       2       3       4
// gate         n       g       n       g       g
// ctxt         -       1       1       3       4
// p_act_p      -       -       1       1       3

PBWMUnGpData* PFCUnitSpec::PFCUnGpData(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return NULL;
  if(!rlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) return NULL;
  PFCLayerSpec* ls = (PFCLayerSpec*) rlay->GetLayerSpec();
  int rgpidx = u->UnitGpIdx();
  PBWMUnGpData* gpd = (PBWMUnGpData*)rlay->ungp_data.FastEl(rgpidx);
  return gpd;
}

void PFCUnitSpec::TI_Compute_CtxtAct(LeabraUnit* u, LeabraNetwork* net) {
  PBWMUnGpData* gpd = PFCUnGpData(u, net);
  if(gpd && gpd->go_fired_trial) {
    u->act_ctxt = u->net_ctxt;
  }
}

void PFCUnitSpec::PostSettle(LeabraUnit* u, LeabraNetwork* net) {
  float save_p_act_p = u->p_act_p;
  inherited::PostSettle(u, net);
  if(net->phase_no == 1) {
    PBWMUnGpData* gpd = PFCUnGpData(u, net);
    if(gpd) {
      bool gated_last_trial = (gpd->mnt_count == 1) ||
        (gpd->mnt_count == 0 && gpd->prv_mnt_count == 1);
      // either we gated last trial and are still maintaining, or just gated and last guy
      // maintained for 1 trial (note: mnt_count updated at start of trial so will be 1)
      if(gated_last_trial) {
        u->misc_1 = 1.0f;
        // p_act_p was just encoded as previous act_p value..
      }
      else {
        u->misc_1 = 0.0f;
        u->p_act_p = save_p_act_p; // if we didn't gate last trial, don't update this..
      }
    }
  }
}
