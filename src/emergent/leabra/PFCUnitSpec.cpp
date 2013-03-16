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

void PFCUnitSpec::TI_Compute_CtxtAct(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return;
  if(!rlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) return;
  PFCLayerSpec* ls = (PFCLayerSpec*) rlay->GetLayerSpec();
  int rgpidx = u->UnitGpIdx();
  PBWMUnGpData* gpd = (PBWMUnGpData*)rlay->ungp_data.FastEl(rgpidx);

  int snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt;
  LeabraLayer* snr_lay = ls->SNrThalStartIdx(rlay, snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
  PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + rgpidx);

  if(snr_gpd->go_fired_trial) { // only update if gated.. that's all there is to it..
    u->act_ctxt = u->net_ctxt;
  }
}
