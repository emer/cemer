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

TA_BASEFUNS_CTORS_DEFN(PFCMaintSpec);
TA_BASEFUNS_CTORS_DEFN(PFCUnitSpec);

void PFCMaintSpec::Initialize() {
  Defaults_init();
}

void PFCMaintSpec::Defaults_init() {
  maint_first_row = 1;
  maint_last_row = -2;
  maint_d5b_to_super = 0.8f;
  d5b_updt_tau = 10.0f;
  
  d5b_updt_dt = 1.0f / d5b_updt_tau;
}

void PFCMaintSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  d5b_updt_dt = 1.0f / d5b_updt_tau;
}

void PFCUnitSpec::Initialize() {
  Defaults_init();
}

void PFCUnitSpec::Defaults_init() {
  act_avg.l_up_inc = 0.1f;       // needs a slower upside due to longer maintenance window..
  cifer_thal.on = true;
  cifer_thal.thal_thr = 0.1f;
  cifer_thal.thal_bin = true;
  cifer_d5b.on = true;
}

bool  PFCUnitSpec::ActiveMaint(LeabraUnit* u) {
  LeabraLayer* lay = (LeabraLayer*)u->own_lay();
  taVector2i ugpos = lay->UnitGpPosFmIdx(u->UnitGpIdx());
  int fst_row = pfc_maint.maint_first_row;
  if(fst_row < 0)
    fst_row = lay->gp_geom.y + fst_row;
  int lst_row = pfc_maint.maint_last_row;
  if(lst_row < 0)
    lst_row = lay->gp_geom.y + lst_row;
  return ((ugpos.y >= fst_row) && (ugpos.y <= lst_row));
}

float PFCUnitSpec::Compute_NetinExtras(float& net_syn, LeabraUnit* u,
                                       LeabraNetwork* net, int thread_no) {
  float net_ex = inherited::Compute_NetinExtras(net_syn, u, net, thread_no);
  bool act_mnt = ActiveMaint(u);
  if(act_mnt) {
    net_ex += pfc_maint.maint_d5b_to_super * u->deep5b;
  }
  return net_ex;
}

void PFCUnitSpec::Compute_Act_ThalDeep5b(LeabraUnit* u, LeabraNetwork* net) {
  if(cifer_thal.on) {
    if(u->thal < cifer_thal.thal_thr)
      u->thal = 0.0f;
    if(cifer_thal.thal_bin && u->thal > 0.0f)
      u->thal = 1.0f;
  }

  if(!cifer_d5b.on) return;

  bool act_mnt = ActiveMaint(u);
  
  if(Quarter_Deep5bNow(net->quarter)) {
    float act5b = u->act_eq;
    if(act5b < cifer_d5b.act5b_thr) {
      act5b = 0.0f;
    }
    act5b *= u->thal;

    if(act_mnt && u->thal_prv > 0.0f && u->thal > 0.0f) { // ongoing maintenance
      u->deep5b += pfc_maint.d5b_updt_dt * (act5b - u->deep5b);
    }
    else {                        // first update or off..
      u->deep5b = act5b;
    }
  }
  else {
    if(cifer_d5b.d5b_burst && !act_mnt) {
      u->deep5b = 0.0f;         // turn it off! only if not maint!
    }
  }
}

