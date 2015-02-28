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
  // act_avg.l_up_inc = 0.1f;       // needs a slower upside due to longer maintenance window..
  cifer_thal.on = true;
  cifer_thal.thal_thr = 0.1f;
  cifer_thal.thal_bin = true;
  cifer_d5b.on = true;
}

bool  PFCUnitSpec::ActiveMaint(LeabraUnit* u) {
  LeabraLayer* lay = (LeabraLayer*)u->own_lay();
  taVector2i ugpos = lay->UnitGpPosFmIdx(u->UnitGpIdx());
  return ((ugpos.y % 2) == 1);
}

float PFCUnitSpec::Compute_NetinExtras(LeabraUnitVars* uv, LeabraNetwork* net,
                            int thr_no, float& net_syn) {
  float net_ex = inherited::Compute_NetinExtras(uv, net, thr_no, net_syn);
  bool act_mnt = ActiveMaint((LeabraUnit*)uv->Un(net, thr_no));
  if(act_mnt) {
    net_ex += pfc_maint.maint_d5b_to_super * uv->deep5b;
  }
  return net_ex;
}

void PFCUnitSpec::Compute_Act_ThalDeep5b(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(cifer_thal.on) {
    if(cifer_thal.auto_thal) {
      u->thal = u->act_eq;
    }
    if(u->thal < cifer_thal.thal_thr)
      TestWrite(u->thal, 0.0f);
    if(cifer_thal.thal_bin && u->thal > 0.0f)
      TestWrite(u->thal, 1.0f);
  }

  if(!cifer_d5b.on) return;

  bool act_mnt = ActiveMaint((LeabraUnit*)u->Un(net, thr_no));
  
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
    if(cifer_d5b.burst && !act_mnt) {
      u->deep5b = 0.0f;         // turn it off! only if not maint!
    }
  }
}

