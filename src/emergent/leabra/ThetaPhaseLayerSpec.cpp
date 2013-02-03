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

#include "ThetaPhaseLayerSpec.h"
#include <LeabraNetwork>

void ThetaPhaseLayerSpec::Initialize() {
  auto_m_cycles = 30;
  Defaults_init();
}

void ThetaPhaseLayerSpec::Defaults_init() {
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = 0.7f;
}

void ThetaPhaseLayerSpec::RecordActM2(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->act_mid = u->act_nd;      // record the minus phase before overwriting it..
  }

  // record stats for act_mid
  AvgMaxVals& vals = lay->acts_m2;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_mid");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_m2, mb_off);
      vals.UpdtFmAvgMax(gpd->acts_m2, nunits, g);
    }
    vals.CalcAvg(lay->units.leaves);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

void ThetaPhaseLayerSpec::Compute_AutoEncStats(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  float norm_err = 0.0f;
  float sse_err = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->act_dif2 = u->act_eq - u->act_mid;
    float sse = u->act_dif2;
    if(fabsf(sse) < us->sse_tol)
      sse = 0.0f;
    sse *= sse;
    sse_err += sse;

    if(net->on_errs) {
      if(u->act_mid > 0.5f && u->act_eq < 0.5f) norm_err += 1.0f;
    }
    if(net->off_errs) {
      if(u->act_mid < 0.5f && u->act_eq > 0.5f) norm_err += 1.0f;
    }
  }
  int ntot = 0;
  if(net->on_errs && net->off_errs)
    ntot = 2 * lay->kwta.k;
  else
    ntot = lay->kwta.k;
  if(ntot > 0)
    norm_err = norm_err / (float)ntot;
  if(norm_err > 1.0f)
    norm_err = 1.0f;
  lay->SetUserData("enc_sse", sse_err);
  lay->SetUserData("enc_norm_err", norm_err);
}
