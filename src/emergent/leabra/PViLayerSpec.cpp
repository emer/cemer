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

#include "PViLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>

#include <taMisc>

void PVMiscSpec::Initialize() {
  gd_pvlv = false;
  min_pvi = 0.4f;
  prior_gain = 1.0f;
  er_reset_prior = true;
}

void PViLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = .5f;           // default is no-information case; extrew = .5

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void PViLayerSpec::HelpConfig() {
  String help = "PViLayerSpec Primary Value (inhibitory) Computation:\n\
 Continuously learns about primary rewards, and cancels (inhibits) them in DA computation.\n\
 It is always trained on the current primary reward (PVe) value from the ExtRew layer\n\
 - Activation is always expectation of reward for current trial\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nPViLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons from relevant network state layers (as PVConSpec)\n\
 - [Optional: Marker recv con from PVe/ExtRew layer -- for display only]\n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool PViLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
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

  if(lay->CheckError(lay->units.leaves == 0, quiet, rval,
                     "requires at least one unit in the layer"))
    return false;

  // note: increased flexibility here: we don't check for things we don't actually depend
  // on -- allows more modular re-use of elements

  return true;
}

void PViLayerSpec::Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  float pvi_targ_val = net->norew_val; // no reward is default
  if(net->ext_rew_avail) {
    pvi_targ_val = net->ext_rew;
  }

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext = pvi_targ_val;     // clamp to target value
     ClampValue_ugp(lay, acc_md, gpidx, net);   // apply new value
     Compute_ExtToPlus_ugp(lay, acc_md, gpidx, net); // copy ext values to act_p
     );
}

// todo: to support multiple different PVe's, we really need everything to be in vector form
// but currently using network ext_rew* vals as intermediary to save a lot of hassle
// so even though this supports multiple PVi's, it does not support multiple PVe's in a
// pairwise manner, even though it is possible to have multiple PVe's

float PViLayerSpec::Compute_PVDa_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                     float pve_val, LeabraNetwork* net) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);

  float pvd = 0.0f;
  float pv_da = 0.0f;
  if(net->phase_no > 0) {
    pvd = pve_val - MAX(u->act_m, pv.min_pvi);
    if(pv.gd_pvlv) {
      pv_da = pvd; // no Y-dot --- what you see is what you get!
    }
    else {
      pv_da = pvd - u->misc_1; // delta relative to prior -- the original PVLV case
    }
  }

  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* du = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(du->lesioned()) continue;
    du->dav = pvd;              // store in all units for visualization & prior updating -- note: NOT the pv_da guy which already has prior delta subtracted!
  }
  return pv_da;
}

float PViLayerSpec::Compute_PVDa(LeabraLayer* lay, LeabraNetwork* net) {
  float pve_val = net->norew_val;
  if(net->ext_rew_avail) {
    pve_val = net->ext_rew;
  }

  float pv_da = 0.0f;
  int n_da = 0;
  UNIT_GP_ITR
    (lay,
     pv_da += Compute_PVDa_ugp(lay, acc_md, gpidx, pve_val, net);
     n_da++;
     );
  if(n_da > 0) {
    pv_da /= (float)n_da;
  }
  return pv_da;
}

void PViLayerSpec::Update_PVPrior_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                      bool er_avail) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  if(er_avail && pv.er_reset_prior) {
    u->misc_1 = 0.0f;
  }
  else {
    u->misc_1 = pv.prior_gain * u->dav; // already stored in da value: note includes min_pvi, which is appropriate -- this was missing prior to 2/12/2009
  }
}

void PViLayerSpec::Update_PVPrior(LeabraLayer* lay, LeabraNetwork* net) {
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  UNIT_GP_ITR(lay, Update_PVPrior_ugp(lay, acc_md, gpidx, er_avail); );
}

void PViLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  // take the 1st guy as the overall general guy
  LeabraUnit* pvisu = (LeabraUnit*)lay->units.Leaf(0);
  net->pvlv_pvi = pvisu->act_eq;
  // this is primarily used for noise modulation
}

void PViLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == net->phase_max-1) { // only at very end!
    Update_PVPrior(lay, net);
  }
}

void PViLayerSpec::Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_PVPlusPhaseDwt(lay, net);
}

bool PViLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  // only do FINAL dWt even though logically it should occur in first plus, because
  // if pbwm is used, then the clamped plus-phase value will be weird, and all the
  // state information should be available etc
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool PViLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}


