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

#include "NVLayerSpec.h"
#include <LeabraNetwork>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(NVSpec);

TA_BASEFUNS_CTORS_DEFN(NVLayerSpec);

void NVSpec::Initialize() {
  da_gain = 0.0f;
  val_thr = 0.1f;
  prior_gain = 1.0f;
  er_reset_prior = true;
}

void NVLayerSpec::Initialize() {
  // SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = 1.0f;          // this is the completely novel value

  // SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void NVLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  nv.UpdateAfterEdit_NoGui();
}

void NVLayerSpec::HelpConfig() {
  String help = "NVLayerSpec Novelty Value Computation:\n\
 Continuously learns to de-activate value representation \n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nNVLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons from relevant network state layers (as PVConSpec)\n\
 - Marker recv con from PVe/ExtRew layer to get external rewards\n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool NVLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
                "requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
                us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

  us->UpdateAfterEdit();

  // note: increased flexibility here: we don't check for things we don't actually depend
  // on -- allows more modular re-use of elements

  return true;
}

float NVLayerSpec::Compute_NVDa_raw(LeabraLayer* lay, LeabraNetwork* net) {
  // currently only supporting one unit
  LeabraUnit* nvsu = (LeabraUnit*)lay->units.Leaf(0);
  float nvd = nvsu->act_m - nv.val_thr;
  if(nvd < 0.0f) nvd = 0.0f;
  float nv_da = nvd - nvsu->misc_1;

  for(int i=0;i<lay->units.leaves;i++) {
    LeabraUnit* du = (LeabraUnit*)lay->units.Leaf(i);
    du->dav = nvd;              // store in all units for visualization, updating -- NOT the nv_da which is already relative to prior
  }
  return nv_da;
}

float NVLayerSpec::Compute_NVDa(LeabraLayer* lay, LeabraNetwork* net) {
  float nv_da = Compute_NVDa_raw(lay, net);
  return nv.da_gain * nv_da;
}

void NVLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  float nv_da = Compute_NVDa_raw(lay, net);
  net->pvlv_nv = nv_da;
}

void NVLayerSpec::Compute_NVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext = 0.0f;             // clamp to pve value
     ClampValue_ugp(lay, acc_md, gpidx, net);   // apply new value
     Compute_ExtToPlus_ugp(lay, acc_md, gpidx, net); // copy ext values to act_p
     );
}

void NVLayerSpec::Update_NVPrior(LeabraLayer* lay, LeabraNetwork* net) {
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  LeabraUnit* nvsu = (LeabraUnit*)lay->units.Leaf(0);
  if(er_avail && nv.er_reset_prior) {
    nvsu->misc_1 = 0.0f;
  }
  else {
    nvsu->misc_1 = nv.prior_gain * nvsu->dav;   // previous da value
  }
}

void NVLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == net->phase_max-1) { // only at very end!
    Update_NVPrior(lay, net);
  }
}

void NVLayerSpec::Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_NVPlusPhaseDwt(lay, net);
}

bool NVLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  // only do FINAL dWt even though logically it should occur in first plus, because
  // if pbwm is used, then the clamped plus-phase value will be weird, and all the
  // state information should be available etc
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool NVLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

