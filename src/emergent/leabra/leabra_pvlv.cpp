// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_pvlv.h"

// #include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////////////////////
// 	PV Con Spec			//
//////////////////////////////////////////

void PVConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  lmix.err_sb = false;

  SetUnique("rnd", true);
  rnd.mean = 0.1f;
  rnd.var = 0.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;  wt_sig.off = 1.0f;

  SetUnique("xcalm", true);
  xcalm.use_sb = false;

  SetUnique("lrate", true);
  lrate = .01f;
  cur_lrate = .01f;

  SetUnique("lrate_sched", true); // not to have any lrate schedule!!
  SetUnique("lrs_value", true); // not to have any lrate schedule!!
  lrs_value = NO_LRS;
}

void PVConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  lmix.err_sb = false;
}

/////////////////////////////////////////////////////////////////////
//	PVi (NAc) Layer Spec

void PVMiscSpec::Initialize() {
  min_pvi = 0.4f;
  prior_discount = 1.0f;
  er_reset_prior = true;
}

void PViLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = .5f;		// default is no-information case; extrew = .5

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void PViLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
  pv.Defaults();
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
  cerr << help << endl << flush;
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
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
		"requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* ext_rew_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
      if(fls->InheritsFrom(TA_ExtRewLayerSpec)) ext_rew_lay = flay;
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_PVConSpec), quiet, rval,
		  "requires recv connections to be of type PVConSpec")) {
      return false;
    }
  }
  
  LeabraLayer* pvr_lay = FindLayerFmSpecNet(net, &TA_PVrLayerSpec);
  if(lay->CheckError(!pvr_lay, quiet, rval,
		"PVLV requires a PVrLayerSpec layer to detect when primary rewards are avail -- run wizard or create manually!")) {
    return false;
  }

  return true;
}

void PViLayerSpec::Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  float pvi_targ_val = net->norew_val; // no reward is default
  if(net->ext_rew_avail) {
    pvi_targ_val = net->ext_rew;
  }

  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     u->ext = pvi_targ_val;	// clamp to target value
     ClampValue_ugp(ugp, net); 	// apply new value
     Compute_ExtToPlus_ugp(ugp, net); // copy ext values to act_p
     );
}

// todo: to support multiple different PVe's, we really need everything to be in vector form
// but currently using network ext_rew* vals as intermediary to save a lot of hassle
// so even though this supports multiple PVi's, it does not support multiple PVe's in a 
// pairwise manner, even though it is possible to have multiple PVe's

float PViLayerSpec::Compute_PVDa_ugp(Unit_Group* pvi_ugp, float pve_val, LeabraNetwork* net) {
  LeabraUnit* u = (LeabraUnit*)pvi_ugp->FastEl(0);

  float pvd = 0.0f;
  float pv_da = 0.0f;
  if(net->phase_no > 0) {
    pvd = pve_val - MAX(u->act_m, pv.min_pvi);
    pv_da = pvd - u->misc_1; // delta relative to prior
  }

  for(int i=0;i<pvi_ugp->size;i++) {
    LeabraUnit* du = (LeabraUnit*)pvi_ugp->FastEl(i);
    du->dav = pvd;		// store in all units for visualization & prior updating -- note: NOT the pv_da guy which already has prior delta subtracted!
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
     pv_da += Compute_PVDa_ugp(ugp, pve_val, net);
     n_da++;
     );
  if(n_da > 0) {
    pv_da /= (float)n_da;
  }
  return pv_da;
}

void PViLayerSpec::Update_PVPrior_ugp(Unit_Group* pvi_ugp, bool er_avail) {
  LeabraUnit* u = (LeabraUnit*)pvi_ugp->FastEl(0);
  if(er_avail && pv.er_reset_prior) {
    u->misc_1 = 0.0f;
  }
  else {
    u->misc_1 = pv.prior_discount * u->dav;	// already stored in da value: note includes min_pvi, which is appropriate -- this was missing prior to 2/12/2009
  }
}

void PViLayerSpec::Update_PVPrior(LeabraLayer* lay, LeabraNetwork* net) {
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  UNIT_GP_ITR(lay, Update_PVPrior_ugp(ugp, er_avail); );
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

bool PViLayerSpec::Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool PViLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

////////////////////////////////////////////////////////
//	PVr = PV reward detection system (habenula?)
////////////////////////////////////////////////////////

void PVrConSpec::Initialize() {
  wt_dec_mult = 0.01f;
}

void PVDetectSpec::Initialize() {
  thr = 0.7f;
}

void PVrLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = .5f;		// default is no-information case; extrew = .5

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void PVrLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void PVrLayerSpec::HelpConfig() {
  String help = "PVrLayerSpec Primary Value Reward Detection Computation:\n\
 Continuously learns about primary rewards, and cancels (inhibits) them in DA computation.\
 It is always trained on the current primary reward (PVe) value from the ExtRew layer\
 with extremization: PVe > norew_val = 1, PVe < norew_val = 0, else .5.\n\
 - Activation is always expectation of reward detection for current trial\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nPVrLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons from relevant network state layers (as PVConSpec)\n\
 - [Optional: Marker recv con from PVe/ExtRew layer -- for display only] \n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool PVrLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
		"requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* ext_rew_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
      if(fls->InheritsFrom(TA_ExtRewLayerSpec)) ext_rew_lay = flay;
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_PVrConSpec), quiet, rval,
		  "requires recv connections to be of type PVrConSpec")) {
      return false;
    }
  }
  
  return true;
}

bool PVrLayerSpec::Compute_PVDetect(LeabraLayer* lay, LeabraNetwork* net) {
  bool pv_detected = false;
  LeabraUnit* pvru = (LeabraUnit*)lay->units.Leaf(0);
  if(pvru->act_eq > pv_detect.thr)
    pv_detected = true;
  net->pv_detected = pv_detected;
  net->pvlv_pvr = pvru->act_eq;
  return pv_detected;
}

void PVrLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  if(net->phase_no == 0)
    Compute_PVDetect(lay, net);	// detect in the minus phase -- continuous!
}

void PVrLayerSpec::Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  float pvr_targ_val = 0.5f;
  if(net->ext_rew_avail)
    pvr_targ_val = 1.0f;	// any kind of primary reward is trained with a 1

  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     u->ext = pvr_targ_val;	// clamp to target value
     ClampValue_ugp(ugp, net); 	// apply new value
     Compute_ExtToPlus_ugp(ugp, net); // copy ext values to act_p
     );
}

void PVrLayerSpec::Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_PVPlusPhaseDwt(lay, net);
}

bool PVrLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  // only do FINAL dWt even though logically it should occur in first plus, because
  // if pbwm is used, then the clamped plus-phase value will be weird, and all the
  // state information should be available etc
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool PVrLayerSpec::Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool PVrLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

//////////////////////////////////////////
//	LV Layer Spec: Perceived Value	//
//////////////////////////////////////////

void LVMiscSpec::Initialize() {
  min_lvi = 0.1f;
  prior_discount = 1.0f;
  er_reset_prior = true;
}

void LVeLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = 0.5f;

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void LVeLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
  lv.Defaults();
}

void LVeLayerSpec::HelpConfig() {
  String help = "LVeLayerSpec Learned Value computation (excitatory/fast and inhibitory/slow):\n\
 Learns values (LV) according to the PVLV algorithm: looks at current network state\
 and computes how much it resembles states that have been associated with primary value (PV) in the past\n\
 - Activation is always learned values\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - (training only occurs when primary reward is detected by PVr as either present or expected\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nLVeLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons from relevant network state layers (must be LVConSpec)\n\
 - [Optional: Marker recv con from PVi layer -- for display only\n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool LVeLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* pvi_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
      if(fls->InheritsFrom(TA_PViLayerSpec)) pvi_lay = flay;
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_PVConSpec), quiet, rval,
		       "requires recv connections to be of type PVConSpec")) {
      return false;
    }
  }
  
//   if(lay->CheckError(!pvi_lay, quiet, rval,
// 		"requires MarkerConSpec connection from PViLayerSpec layer to get DA values!")) {
//     return false;
//   }

  return true;
}

void LVeLayerSpec::Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  float pve_val = net->norew_val;
  if(net->ext_rew_avail) {
    pve_val = net->ext_rew;
  }

  if(er_avail) {
    UNIT_GP_ITR
      (lay, 
       LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
       u->ext = pve_val;
       ClampValue_ugp(ugp, net); 		// apply new value
       Compute_ExtToPlus_ugp(ugp, net); 	// copy ext values to act_p
     );
  }
}

float LVeLayerSpec::Compute_LVDa_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp, LeabraNetwork* net) {
  LeabraUnit* lveu = (LeabraUnit*)lve_ugp->FastEl(0);
  LeabraUnit* lviu = (LeabraUnit*)lvi_ugp->FastEl(0);

  float lvd = lveu->act_eq - MAX(lviu->act_eq, lv.min_lvi);
  float lv_da = lvd - lveu->misc_1;

  for(int i=0;i<lve_ugp->size;i++) {
    LeabraUnit* du = (LeabraUnit*)lve_ugp->FastEl(i);
    du->dav = lvd;		// store in all units for visualization and prior update (NOT lv_da which already has misc1 subtracted!)
  }
  return lv_da;
}

float LVeLayerSpec::Compute_LVDa(LeabraLayer* lve_lay, LeabraLayer* lvi_lay,
				 LeabraNetwork* net) {
  float lv_da = 0.0f;
  int gi = 0;
  if((lve_lay->units.gp.size > 1) && (lve_lay->units.gp.size == lvi_lay->units.gp.size)) {
    for(gi=0; gi<lve_lay->units.gp.size; gi++) {
      Unit_Group* lve_ugp = (Unit_Group*)lve_lay->units.gp[gi];
      Unit_Group* lvi_ugp = (Unit_Group*)lvi_lay->units.gp[gi];
      lv_da += Compute_LVDa_ugp(lve_ugp, lvi_ugp, net);
    }
    lv_da /= (float)lve_lay->units.gp.size; // average!
  }
  else if(lve_lay->units.gp.size > 1) {
    // one lvi and multiple lve's
    Unit_Group* lvi_ugp;
    if(lvi_lay->units.gp.size > 0)    lvi_ugp = (Unit_Group*)lvi_lay->units.gp[0];
    else			      lvi_ugp = (Unit_Group*)&(lvi_lay->units);
    for(gi=0; gi<lve_lay->units.gp.size; gi++) {
      Unit_Group* lve_ugp = (Unit_Group*)lve_lay->units.gp[gi];
      lv_da += Compute_LVDa_ugp(lve_ugp, lvi_ugp, net);
    }
    lv_da /= (float)lve_lay->units.gp.size; // average!
  }
  else if(lvi_lay->units.gp.size > 1) {
    // one lve and multiple lvi's
    Unit_Group* lve_ugp;
    if(lve_lay->units.gp.size > 0)    lve_ugp = (Unit_Group*)lve_lay->units.gp[0];
    else			      lve_ugp = (Unit_Group*)&(lve_lay->units);
    for(gi=0; gi<lvi_lay->units.gp.size; gi++) {
      Unit_Group* lvi_ugp = (Unit_Group*)lvi_lay->units.gp[gi];
      lv_da += Compute_LVDa_ugp(lve_ugp, lvi_ugp, net);
    }
    lv_da /= (float)lvi_lay->units.gp.size; // average!
  }
  else {
    Unit_Group* lve_ugp;
    if(lve_lay->units.gp.size > 0)    lve_ugp = (Unit_Group*)lve_lay->units.gp[0];
    else			      lve_ugp = (Unit_Group*)&(lve_lay->units);
    Unit_Group* lvi_ugp;
    if(lvi_lay->units.gp.size > 0)    lvi_ugp = (Unit_Group*)lvi_lay->units.gp[0];
    else			      lvi_ugp = (Unit_Group*)&(lvi_lay->units);
    lv_da = Compute_LVDa_ugp(lve_ugp, lvi_ugp, net);
  } 
  return lv_da;
}

void LVeLayerSpec::Update_LVPrior_ugp(Unit_Group* lve_ugp, bool er_avail) {
  LeabraUnit* lveu = (LeabraUnit*)lve_ugp->FastEl(0);
  if(er_avail && lv.er_reset_prior) {
    lveu->misc_1 = 0.0f;
  }
  else {
    lveu->misc_1 = lv.prior_discount * lveu->dav;	// already stored in da value: note includes min_lvi, which is appropriate -- this was missing prior to 2/12/2009
  }
}

void LVeLayerSpec::Update_LVPrior(LeabraLayer* lay, LeabraNetwork* net) {
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  UNIT_GP_ITR(lay, Update_LVPrior_ugp(ugp, er_avail); );
}

void LVeLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  // take the 1st guy as the overall general guy
  LeabraUnit* lvesu = (LeabraUnit*)lay->units.Leaf(0);
  net->pvlv_lve = lvesu->act_eq;
  // this is primarily used for noise modulation
}

void LViLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  ScalarValLayerSpec::Compute_CycleStats(lay, net);
  // take the 1st guy as the overall general guy
  LeabraUnit* lvisu = (LeabraUnit*)lay->units.Leaf(0);
  net->pvlv_lvi = lvisu->act_eq;
  // this is primarily used for noise modulation
}

void LVeLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == net->phase_max-1) { // only at very end!
    Update_LVPrior(lay, net);
  }
}

void LVeLayerSpec::Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_LVPlusPhaseDwt(lay, net);
}

bool LVeLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  // only do FINAL dWt even though logically it should occur in first plus, because
  // if pbwm is used, then the clamped plus-phase value will be weird, and all the
  // state information should be available etc
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool LVeLayerSpec::Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool LVeLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}


//////////////////////////////////////////
//	NV (Novelty Value) Layer Spec	//
//////////////////////////////////////////

void NVConSpec::Initialize() {
  decay = 0.0f;
}

void NVSpec::Initialize() {
  da_gain = 1.0f;
  val_thr = 0.1f;
  prior_discount = 1.0f;
  er_reset_prior = true;
}

void NVLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = 1.0f;		// this is the completely novel value

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void NVLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
  nv.Defaults();
}

void NVLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  nv.UpdateAfterEdit();
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
  cerr << help << endl << flush;
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
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
		"requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_PVConSpec), quiet, rval,
		       "requires recv connections to be of type PVConSpec")) {
      return false;
    }
  }
  
  return true;
}

float NVLayerSpec::Compute_NVDa(LeabraLayer* lay, LeabraNetwork* net) {
  // currently only supporting one unit
  LeabraUnit* nvsu = (LeabraUnit*)lay->units.Leaf(0);
  float nvd = nvsu->act_m - nv.val_thr;
  if(nvd < 0.0f) nvd = 0.0f;
  float nv_da = nvd - nvsu->misc_1;

  for(int i=0;i<lay->units.leaves;i++) {
    LeabraUnit* du = (LeabraUnit*)lay->units.Leaf(i);
    du->dav = nvd;		// store in all units for visualization, updating -- NOT the nv_da which is already relative to prior
  }
  return nv.da_gain * nv_da;
}

void NVLayerSpec::Compute_NVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     u->ext = 0.0f;		// clamp to pve value
     ClampValue_ugp(ugp, net); 	// apply new value
     Compute_ExtToPlus_ugp(ugp, net); // copy ext values to act_p
     );
}

void NVLayerSpec::Update_NVPrior(LeabraLayer* lay, LeabraNetwork* net) {
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  LeabraUnit* nvsu = (LeabraUnit*)lay->units.Leaf(0);
  if(er_avail && nv.er_reset_prior) {
    nvsu->misc_1 = 0.0f;
  }
  else {
    nvsu->misc_1 = nv.prior_discount * nvsu->dav;	// previous da value
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

bool NVLayerSpec::Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

bool NVLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return false;
  return true;
}

//////////////////////////////////
//	PVLVDa Layer Spec	//
//////////////////////////////////

void PVLVDaSpec::Initialize() {
  da_gain = 1.0f;
  tonic_da = 0.0f;
}

void PVLVDaLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .25;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void PVLVDaLayerSpec::Defaults() {
  inherited::Defaults();
  da.Defaults();
  Initialize();
}

void PVLVDaLayerSpec::HelpConfig() {
  String help = "PVLVDaLayerSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from PVLV layers.\n\
 - No Learning\n\
 \nPVLVDaLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons marked with a MarkerConSpec from PVLV\n\
 - This layer must be after recv layers in list of layers\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool PVLVDaLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f), quiet, rval,
		"requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act_range", true);
    us->act_range.max = 2.0f;
    us->act_range.min = -2.0f;
    us->act_range.UpdateAfterEdit();
  }
  if(lay->CheckError((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f), quiet, rval,
		"requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("clamp_range", true);
    us->clamp_range.max = 2.0f;
    us->clamp_range.min = -2.0f;
    us->clamp_range.UpdateAfterEdit();
  }
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
		"requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }

  // check recv connection
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraLayer* lve_lay = NULL;
  LeabraLayer* lvi_lay = NULL;
  LeabraLayer* pvi_lay = NULL;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    if(cs->InheritsFrom(TA_MarkerConSpec)) {
      if(lay->CheckError(recv_gp->size <= 0, quiet, rval,
		    "requires one recv projection with at least one unit!")) {
	return false;
      }
      if(fls->InheritsFrom(TA_LVeLayerSpec)) lve_lay = fmlay;
      if(fls->InheritsFrom(TA_LViLayerSpec)) lvi_lay = fmlay;
      if(fls->InheritsFrom(TA_PViLayerSpec)) pvi_lay = fmlay;
    }
  }

  if(lay->CheckError(!lve_lay, quiet, rval,
		"did not find LVe layer to get Da from!")) {
    return false;
  }
  if(lay->CheckError(!lvi_lay, quiet, rval,
		"did not find LVi layer to get Da from!")) {
    return false;
  }
  if(lay->CheckError(!pvi_lay, quiet, rval,
		"did not find PVi layer to get Da from!")) {
    return false;
  }

  return true;
}

void PVLVDaLayerSpec::Compute_Da(LeabraLayer* lay, LeabraNetwork* net) {
  float net_da = 0.0f;
  int lve_prjn_idx;
  LeabraLayer* lve_lay = FindLayerFmSpec(lay, lve_prjn_idx, &TA_LVeLayerSpec);
  LVeLayerSpec* lve_sp = (LVeLayerSpec*)lve_lay->GetLayerSpec();
  int lvi_prjn_idx;
  LeabraLayer* lvi_lay = FindLayerFmSpec(lay, lvi_prjn_idx, &TA_LViLayerSpec);
  //   LVeLayerSpec* lvi_sp = (LViLayerSpec*)lvi_lay->GetLayerSpec();

  int pvi_prjn_idx;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.SPtr();

  int nv_prjn_idx;
  LeabraLayer* nv_lay = FindLayerFmSpec(lay, nv_prjn_idx, &TA_NVLayerSpec);
  NVLayerSpec* nvls = NULL;
  if(nv_lay) nvls = (NVLayerSpec*)nv_lay->spec.SPtr();

  float lv_da = lve_sp->Compute_LVDa(lve_lay, lvi_lay, net);
  // nv only contributes to lv, not pv..
  if(nv_lay) {
    lv_da += nvls->Compute_NVDa(nv_lay, net);
  }
  float pv_da = pvils->Compute_PVDa(pvi_lay, net);
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  if(er_avail)
    net_da = da.da_gain * pv_da;
  else
    net_da = da.da_gain * lv_da;

  lay->dav = net_da;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->dav = net_da;
    u->ext = da.tonic_da + u->dav;
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
  }
}

void PVLVDaLayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork*) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    const float snd_val = u->act;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())	continue;
      for(int j=0;j<send_gp->size; j++) {
	((LeabraUnit*)send_gp->Un(j))->dav = snd_val;
      }
    }
  }
}

void PVLVDaLayerSpec::BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net) {
  // that's it: don't do any processing on this layer: set all idx to 0
  lay->units_flat_idx = 0;
  Unit* un;
  taLeafItr ui;
  FOR_ITR_EL(Unit, un, lay->units., ui) {
    un->flat_idx = 0;
  }
}

void PVLVDaLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_Da(lay, net);
  Send_Da(lay, net);
}

void PVLVDaLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = false;
  lay->UnSetExtFlag(Unit::EXT);
  inherited::Compute_HardClamp(lay, net);
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

///////////////////////////////////////////////////////////////
//			PVLV
///////////////////////////////////////////////////////////////


bool LeabraWizard::PVLV_ToLayerGroup(LeabraNetwork* net) {
  if(TestError(!net, "PVLV_ToLayerGroup", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  String pvenm = "PVe";  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";
  String vtanm = "VTA";

  bool new_laygp = false;
  Layer_Group* laygp = net->FindMakeLayerGroup("PVLV", NULL, new_laygp);

  Layer* lay;
  if(lay = net->FindLayer(pvenm)) { laygp->Transfer(lay); lay->pos.z = 0; } 
  if(lay = net->FindLayer(pvinm)) { laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer(pvrnm)) { laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer(lvenm)) { laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer(lvinm)) { laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer(nvnm)) { laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer(vtanm)) { laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("DA")) { laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("RewTarg")) { laygp->Transfer(lay); lay->pos.z = 0; }

  if(new_laygp) {
    laygp->pos.z = 0;
    net->RebuildAllViews();	// trigger update
  }

  return true;
}

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

bool LeabraWizard::PVLV(LeabraNetwork* net, bool da_mod_all) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PVLV", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork(&TA_LeabraNetwork, net)) return false;
  }

  String msg = "Configuring Pavlov (PVLV) Layers:\n\n\
 There is one thing you will need to check manually after this automatic configuration\
 process completes (this note will be repeated when things complete --- there may be some\
 messages in the interim):\n\n";

  String man_msg = "1. Check that connection(s) were made from all appropriate output layers\
 to the PVe (ExtRewLayerSpec) layer, using the MarkerConSpec (MarkerCons) Con spec.\
 This will provide the error signal to the system based on output error performance.\n\n";

  msg += man_msg + "\n\nThe configuration will now be checked and a number of default parameters\
 will be set.  If there are any actual errors which must be corrected before\
 the network will run, you will see a message to that effect --- you will then need to\
 re-run this configuration process to make sure everything is OK.  When you press\
 Re/New/Init on the control process these same checks will be performed, so you\
 can be sure everything is ok.";
  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool	lve_new = false;
  bool	pvr_new = false;
  bool	nv_new = false;
  String pvenm = "PVe";  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";
  String vtanm = "VTA";

  bool new_laygp = false;
  Layer_Group* laygp = net->FindMakeLayerGroup("PVLV", NULL, new_laygp);

  LeabraLayer* rew_targ_lay;
  LeabraLayer* pve;  LeabraLayer* pvr; LeabraLayer* pvi; LeabraLayer* lve; LeabraLayer* lvi;
  LeabraLayer* nv;   LeabraLayer* vta;

  if(new_laygp) {
    PVLV_ToLayerGroup(net);	// doesn't hurt to just do this..
  }

  bool dumbo;
  rew_targ_lay = (LeabraLayer*)laygp->FindMakeLayer("RewTarg");
  pve = (LeabraLayer*)laygp->FindMakeLayer(pvenm, NULL, dumbo);
  pvr = (LeabraLayer*)laygp->FindMakeLayer(pvrnm, NULL, pvr_new);
  pvi = (LeabraLayer*)laygp->FindMakeLayer(pvinm, NULL, dumbo);
  lve = (LeabraLayer*)laygp->FindMakeLayer(lvenm, NULL, lve_new);
  lvi = (LeabraLayer*)laygp->FindMakeLayer(lvinm, NULL, dumbo);
  nv =  (LeabraLayer*)laygp->FindMakeLayer(nvnm, NULL, nv_new);
  vta = (LeabraLayer*)laygp->FindMakeLayer(vtanm, NULL, dumbo, "DA");
  if(rew_targ_lay == NULL || lve == NULL || pve == NULL || pvi == NULL || vta == NULL) return false;

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  Layer_Group other_lays;
  Layer_Group hidden_lays;
  Layer_Group output_lays;
  Layer_Group input_lays;
  int i;
  for(i=0;i<net->layers.leaves;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != lve && lay != pve && lay != pvr && lay != pvi &&
       lay != lvi && lay != nv && lay != vta
       && !laysp->InheritsFrom(&TA_PFCLayerSpec) && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
      other_lays.Link(lay);
      if(lay->pos.z == 0) lay->pos.z = 2; // nobody allowed in 0!
      if(lay->layer_type == Layer::HIDDEN)
	hidden_lays.Link(lay);
      else if(lay->layer_type == Layer::INPUT)
	input_lays.Link(lay);
      else 
	output_lays.Link(lay);
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  String gpprfx = "PFC_BG_";

  BaseSpec_Group* units = net->FindMakeSpecGp(gpprfx + "Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp(gpprfx + "Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp(gpprfx + "Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp(gpprfx + "Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return false;

  LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* lv_units = (LeabraUnitSpec*)pv_units->FindMakeChild("LVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_LeabraUnitSpec);
  if(lv_units == NULL || pv_units == NULL || da_units == NULL) return false;

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(learn_cons == NULL) return false;

  bool pvi_cons_new = false;
  PVConSpec* pvi_cons = (PVConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec, pvi_cons_new);
//   LeabraConSpec* pvr_old = (LeabraConSpec*)pvi_cons->children.FindName("PVr");
//   if(pvr_old && !pvr_old->InheritsFrom(&TA_PVrConSpec)) {
//     pvi_cons->RemoveChild("PVr");
//   }
  bool pvr_cons_new = false;
  PVConSpec* pvr_cons = (PVConSpec*)pvi_cons->FindMakeChild("PVr", &TA_PVrConSpec, pvr_cons_new);
  PVConSpec* lve_cons = (PVConSpec*)pvi_cons->FindMakeChild("LVe", &TA_PVConSpec);
  PVConSpec* lvi_cons = (PVConSpec*)lve_cons->FindMakeChild("LVi", &TA_PVConSpec);
  PVConSpec* nv_cons = (PVConSpec*)pvi_cons->FindMakeChild("NV", &TA_PVConSpec);
  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  if(bg_bias == NULL) return false;
  LeabraConSpec* fixed_bias = (LeabraConSpec*)bg_bias->FindMakeChild("FixedBias", &TA_LeabraBiasSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  if(lve_cons == NULL || marker_cons == NULL || fixed_bias == NULL)
    return false;

  ExtRewLayerSpec* pvesp = (ExtRewLayerSpec*)layers->FindMakeSpec(pvenm + "Layer", &TA_ExtRewLayerSpec);
  PVrLayerSpec* pvrsp = (PVrLayerSpec*)layers->FindMakeSpec(pvrnm + "Layer", &TA_PVrLayerSpec, dumbo);
  PViLayerSpec* pvisp = (PViLayerSpec*)layers->FindMakeSpec(pvinm + "Layer", &TA_PViLayerSpec, dumbo);
  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec, dumbo);
  LViLayerSpec* lvisp = (LViLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec, dumbo);
  NVLayerSpec* nvsp = (NVLayerSpec*)layers->FindMakeSpec(nvnm + "Layer", &TA_NVLayerSpec, dumbo);
  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindMakeSpec(vtanm + "Layer", &TA_PVLVDaLayerSpec);
  if(lvesp == NULL || pvesp == NULL || pvisp == NULL || dasp == NULL) return false;

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  if(fullprjn == NULL || onetoone == NULL) return false;

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  //  learn_cons->lmix.hebb = .01f; // .01 hebb on learn cons
//   learn_cons->not_used_ok = true;
  learn_cons->UpdateAfterEdit();
  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;
  fixed_bias->SetUnique("lrate", true);
  fixed_bias->lrate = 0.0f;

  pvr_cons->SetUnique("lrate", true);
  pvi_cons->SetUnique("lrate", true);
  nv_cons->SetUnique("lrate", true);
  lve_cons->SetUnique("lrate", true);
  lvi_cons->SetUnique("lrate", true);

  // NOT unique: inherit from pvi:
  lve_cons->SetUnique("rnd", false);
  lve_cons->SetUnique("wt_limits", false);
  lve_cons->SetUnique("wt_sig", false);
  lve_cons->SetUnique("lmix", false);
  lve_cons->SetUnique("lrate_sched", false);
  lve_cons->SetUnique("lrs_value", false);

  pvr_cons->SetUnique("rnd", false);
  pvr_cons->SetUnique("wt_limits", false);
  pvr_cons->SetUnique("wt_sig", false);
  pvr_cons->SetUnique("lmix", false);
  pvr_cons->SetUnique("lrate_sched", false);
  pvr_cons->SetUnique("lrs_value", false);

  nv_cons->SetUnique("rnd", false);
  nv_cons->SetUnique("wt_limits", false);
  nv_cons->SetUnique("wt_sig", false);
  nv_cons->SetUnique("lmix", false);
  nv_cons->SetUnique("lrate_sched", false);
  nv_cons->SetUnique("lrs_value", false);

  // NOT unique: inherit from lve:
  lvi_cons->SetUnique("rnd", false);
  lvi_cons->SetUnique("wt_limits", false);
  lvi_cons->SetUnique("wt_sig", false);
  lvi_cons->SetUnique("lmix", false);
  lvi_cons->SetUnique("lrate_sched", false);
  lvi_cons->SetUnique("lrs_value", false);

  // NOT unique: inherit from lve
  lvisp->SetUnique("decay", false);
  lvisp->SetUnique("kwta", false);
  lvisp->SetUnique("inhib_group", false);
  lvisp->SetUnique("inhib", false);

  pv_units->SetUnique("g_bar", true);

  // setup localist values!
  ScalarValLayerSpec* valspecs[6] = {pvesp, pvisp, lvesp, lvisp, pvrsp, nvsp};
  for(int i=0;i<6;i++) {
    ScalarValLayerSpec* lsp = valspecs[i];
    lsp->scalar.rep = ScalarValSpec::LOCALIST;
    lsp->scalar.min_sum_act = .2f;
    lsp->inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB; lsp->inhib.kwta_pt = 0.9f;
    lsp->kwta.k_from = KWTASpec::USE_K;    lsp->kwta.k = 1;
    lsp->gp_kwta.k_from = KWTASpec::USE_K; lsp->gp_kwta.k = 1; 
    lsp->unit_range.min = 0.0f;  lsp->unit_range.max = 1.0f;
    lsp->unit_range.UpdateAfterEdit();
    lsp->val_range = lsp->unit_range;
  }

  lvesp->bias_val.un = ScalarValBias::GC;
  lvesp->bias_val.wt = ScalarValBias::NO_WT;
  lvesp->bias_val.val = 0.5f;
  pvisp->bias_val.un = ScalarValBias::GC;
  pvisp->bias_val.wt = ScalarValBias::NO_WT;
  pvisp->bias_val.val = 0.5f;
  pvrsp->bias_val.un = ScalarValBias::GC;
  pvrsp->bias_val.wt = ScalarValBias::NO_WT;
  pvrsp->bias_val.val = 0.5f;
  nvsp->bias_val.un = ScalarValBias::GC;
  nvsp->bias_val.wt = ScalarValBias::NO_WT;
  nvsp->bias_val.val = 1.0f;
//   pvrsp->pv_detect.thr_min = .2f;
//   pvrsp->pv_detect.thr_max = .8f;

  pv_units->SetUnique("act", true);
  pv_units->SetUnique("act_fun", true);
  pv_units->SetUnique("dt", true);
  pv_units->act_fun = LeabraUnitSpec::NOISY_LINEAR;
  pv_units->act.thr = .17f;
  pv_units->act.gain = 220.0f;
  pv_units->act.nvar = .01f;
  pv_units->g_bar.l = .1f;
  pv_units->g_bar.h = .03f;  pv_units->g_bar.a = .09f;
  pv_units->dt.vm = .05f;
  pv_units->dt.vm_eq_cyc = 100; // go straight to equilibrium!

  pvi_cons->SetUnique("lmix", true);
  pvi_cons->lmix.err_sb = false; 
  pvi_cons->SetUnique("rnd", true);
  pvi_cons->rnd.mean = 0.1f;
  pvi_cons->rnd.var = 0.0f;
  // for XCAL:
  pvi_cons->SetUnique("xcalm", true);
  pvi_cons->xcalm.use_sb = false;

  pvi_cons->lrate = .01f;
  pvr_cons->lrate = .02f;
  nv_cons->lrate = .0005f;
  lve_cons->lrate = .05f;
  lvi_cons->lrate = .001f;

  if(output_lays.size > 0)
    pvesp->rew_type = ExtRewLayerSpec::OUT_ERR_REW;
  else
    pvesp->rew_type = ExtRewLayerSpec::EXT_REW;

  int n_lv_u;		// number of pvlv-type units
  if(pvisp->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  else if(pvisp->scalar.rep == ScalarValSpec::GAUSSIAN)
    n_lv_u = 12;
  else
    n_lv_u = 21;

  // optimization to speed up settling in phase 2: only the basic layers here
  int j;
  for(j=0;j<net->specs.size;j++) {
    if(net->specs[j]->InheritsFrom(TA_LeabraLayerSpec)) {
      LeabraLayerSpec* sp = (LeabraLayerSpec*)net->specs[j];
      sp->decay.clamp_phase2 = true;
      sp->UpdateAfterEdit();
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  if(lve_new) {
    pve->pos.SetXYZ(0,0,0);
    pvi->pos.SetXYZ(0,2,0);

    lve->pos.SetXYZ(6,0,0);
    lvi->pos.SetXYZ(6,2,0);

    vta->pos.SetXYZ(12,0,0);
    rew_targ_lay->pos.SetXYZ(12,4,0);
  }

  if(pvr_new) {
    pvr->pos.SetXYZ(0,4,0);
  }
  if(nv_new) {
    nv->pos.SetXYZ(6,4,0);
  }

  if(pvi->un_geom.n != n_lv_u) { pvi->un_geom.n = n_lv_u; pvi->un_geom.x = n_lv_u; pvi->un_geom.y = 1; }
  if(lve->un_geom.n != n_lv_u) { lve->un_geom.n = n_lv_u; lve->un_geom.x = n_lv_u; lve->un_geom.y = 1; }
  if(lvi->un_geom.n != n_lv_u) { lvi->un_geom.n = n_lv_u; lvi->un_geom.x = n_lv_u; lvi->un_geom.y = 1; }
  if(pve->un_geom.n != n_lv_u) { pve->un_geom.n = n_lv_u; pve->un_geom.x = n_lv_u; pve->un_geom.y = 1; }
  if(pvr->un_geom.n != n_lv_u) { pvr->un_geom.n = n_lv_u; pvr->un_geom.x = n_lv_u; pvr->un_geom.y = 1; }
  if(nv->un_geom.n != n_lv_u) { nv->un_geom.n = n_lv_u; nv->un_geom.x = n_lv_u; nv->un_geom.y = 1; }
  vta->un_geom.n = 1;
  rew_targ_lay->un_geom.n = 1;
  rew_targ_lay->layer_type = Layer::INPUT;

  if(new_laygp) {
    laygp->pos.z = 0;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  pve->SetLayerSpec(pvesp);	pve->SetUnitSpec(pv_units);
  pvi->SetLayerSpec(pvisp);	pvi->SetUnitSpec(pv_units);
  lve->SetLayerSpec(lvesp);	lve->SetUnitSpec(lv_units);
  lvi->SetLayerSpec(lvisp);	lvi->SetUnitSpec(lv_units);
  vta->SetLayerSpec(dasp);	vta->SetUnitSpec(da_units);
  pvr->SetLayerSpec(pvrsp);	pvr->SetUnitSpec(pv_units);
  nv->SetLayerSpec(nvsp);	nv->SetUnitSpec(pv_units);

  pv_units->bias_spec.SetSpec(bg_bias);
  lv_units->bias_spec.SetSpec(bg_bias);
  da_units->bias_spec.SetSpec(fixed_bias);
  
  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,
  net->FindMakePrjn(pve, rew_targ_lay, onetoone, marker_cons);
  net->FindMakePrjn(pvr, pve, onetoone, marker_cons);
  net->FindMakePrjn(pvi, pve, onetoone, marker_cons);

  net->FindMakePrjn(lve, pvr, onetoone, marker_cons);
  net->FindMakePrjn(lvi, pvr, onetoone, marker_cons);

  net->FindMakePrjn(vta, pvi, onetoone, marker_cons);
  net->FindMakePrjn(vta, lve, onetoone, marker_cons);
  net->FindMakePrjn(vta, lvi, onetoone, marker_cons);
  net->FindMakePrjn(vta, pvr, onetoone, marker_cons);
  net->FindMakePrjn(vta, nv,  onetoone, marker_cons);

  if(lve_new) {
    for(i=0;i<input_lays.size;i++) {
      Layer* il = (Layer*)input_lays[i];
      net->FindMakePrjn(pvr, il, fullprjn, pvr_cons);
      net->FindMakePrjn(pvi, il, fullprjn, pvi_cons);
      net->FindMakePrjn(lve, il, fullprjn, lve_cons);
      net->FindMakePrjn(lvi, il, fullprjn, lvi_cons);
      net->FindMakePrjn(nv,  il, fullprjn, nv_cons);
    }
  }

  if(pvr_cons_new && !pvi_cons_new) {
    // fix pvr cons because conspec was orig created as PV instead of PVr..
    for(i=0; i<pvr->projections.size;i++) {
      Projection* prjn = pvr->projections[i];
      if(!prjn->con_spec.GetSpec())
	prjn->SetConSpec(pvr_cons);
    }
  }

  if(da_mod_all) {
    for(i=0;i<other_lays.size;i++) {
      Layer* ol = (Layer*)other_lays[i];
      net->FindMakePrjn(ol, vta, fullprjn, marker_cons);
    }
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    net->FindMakePrjn(pve, ol, onetoone, marker_cons);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();
  net->LayerPos_Cleanup();

  if(new_laygp) {
    laygp->pos.z = 0;		// move back!
    net->RebuildAllViews();	// trigger update
  }

  taMisc::CheckConfigStart(false, false);

  bool ok = pvisp->CheckConfig_Layer(pvi, true) && lvesp->CheckConfig_Layer(lve, true)
    && lvisp->CheckConfig_Layer(lve, true)
    && dasp->CheckConfig_Layer(vta, true) && pvesp->CheckConfig_Layer(pve, true)
    && pvrsp->CheckConfig_Layer(pvr, true) && nvsp->CheckConfig_Layer(nv, true);

  taMisc::CheckConfigEnd(ok);

  if(!ok) {
    msg =
      "PVLV: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg = 
    "PVLV configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  pvesp->UpdateAfterEdit();
  pvisp->UpdateAfterEdit();
  lvesp->UpdateAfterEdit();
  lvisp->UpdateAfterEdit();
  pvrsp->UpdateAfterEdit();
  nvsp->UpdateAfterEdit();
  
  for(j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("PVLV");
  if(edit) {
    pvr_cons->SelectForEditNm("lrate", edit, "pvr");
    pvi_cons->SelectForEditNm("lrate", edit, "pvi");
    lve_cons->SelectForEditNm("lrate", edit, "lve");
    lvi_cons->SelectForEditNm("lrate", edit, "lvi");
    nv_cons->SelectForEditNm("lrate", edit, "nv");
    pvesp->SelectForEditNm("rew", edit, "pve");
    pvisp->SelectForEditNm("pv", edit, "pvi");
    lvesp->SelectForEditNm("lv", edit, "lve");
    pvrsp->SelectForEditNm("pv_detect", edit, "pvr");
    nvsp->SelectForEditNm("nv", edit, "nv");
//     pvisp->SelectForEditNm("scalar", edit, "pvi");
//     lvesp->SelectForEditNm("scalar", edit, "lve");
//     pvisp->SelectForEditNm("bias_val", edit, "pvi");
//     lvesp->SelectForEditNm("bias_val", edit, "lve");
//    dasp->SelectForEditNm("avg_da", edit, "vta");
    dasp->SelectForEditNm("da", edit, "vta");
  }
  return true;
}

bool LeabraWizard::PVLV_ConnectLayer(LeabraNetwork* net, LeabraLayer* sending_layer,
				     bool disconnect) {
  if(TestError(!net || !sending_layer, "PVLV_ConnectLayer", "must specify a network and a sending layer!")) return false;

  // String pvenm = "PVe";
  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";

//   LeabraLayer* pve = (LeabraLayer*)net->FindLayer(pvenm);
  LeabraLayer* pvr = (LeabraLayer*)net->FindLayer(pvrnm);
  LeabraLayer* pvi = (LeabraLayer*)net->FindLayer(pvinm);
  LeabraLayer* lve = (LeabraLayer*)net->FindLayer(lvenm);
  LeabraLayer* lvi = (LeabraLayer*)net->FindLayer(lvinm);
  LeabraLayer* nv =  (LeabraLayer*)net->FindLayer(nvnm);

  BaseSpec_Group* prjns = net->FindMakeSpecGp("PFC_BG_Prjns");
  BaseSpec_Group* cons = net->FindMakeSpecGp("PFC_BG_Cons");

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(TestError(!learn_cons, "PVLV_ConnectLayer", "LearnCons not found -- not properly configured for PVLV"))
    return false;

  LeabraConSpec* pvi_cons = (LeabraConSpec*)learn_cons->children.FindSpecName("PVi");
  LeabraConSpec* pvr_cons = (LeabraConSpec*)pvi_cons->children.FindSpecName("PVr");
  LeabraConSpec* lve_cons = (LeabraConSpec*)pvi_cons->children.FindSpecName("LVe");
  if(TestError(!lve_cons, "PVLV_ConnectLayer", "LVe Cons not found -- not properly configured for PVLV"))
    return false;
  LeabraConSpec* lvi_cons = (LeabraConSpec*)lve_cons->children.FindSpecName("LVi");
  LeabraConSpec* nv_cons =  (LeabraConSpec*)pvi_cons->children.FindSpecName("NV");

  if(disconnect) {
    if(pvr && pvr_cons)
      net->RemovePrjn(pvr, sending_layer);
    if(pvi && pvi_cons)
      net->RemovePrjn(pvi, sending_layer);
    if(lve && lve_cons)
      net->RemovePrjn(lve, sending_layer);
    if(lvi && lvi_cons)
      net->RemovePrjn(lvi, sending_layer);
    if(nv && nv_cons)
      net->RemovePrjn(nv,  sending_layer);
  }
  else {
    if(pvr && pvr_cons)
      net->FindMakePrjn(pvr, sending_layer, fullprjn, pvr_cons);
    if(pvi && pvi_cons)
      net->FindMakePrjn(pvi, sending_layer, fullprjn, pvi_cons);
    if(lve && lve_cons)
      net->FindMakePrjn(lve, sending_layer, fullprjn, lve_cons);
    if(lvi && lvi_cons)
      net->FindMakePrjn(lvi, sending_layer, fullprjn, lvi_cons);
    if(nv && nv_cons)
      net->FindMakePrjn(nv,  sending_layer, fullprjn, nv_cons);
  }
  return true;
}

bool LeabraWizard::PVLV_OutToPVe(LeabraNetwork* net, LeabraLayer* output_layer,
				     bool disconnect) {
  if(TestError(!net || !output_layer, "PVLV_OutToPVe", "must specify a network and an output layer!")) return false;

  String pvenm = "PVe";
  LeabraLayer* pve = (LeabraLayer*)net->FindLayer(pvenm);

  BaseSpec_Group* prjns = net->FindMakeSpecGp("PFC_BG_Prjns");
  BaseSpec_Group* cons = net->FindMakeSpecGp("PFC_BG_Cons");

  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  if(TestError(!marker_cons || !onetoone, "PVLV_OutToPVe", "MarkerCons and/or OneToOne Prjn not found -- not properly configured for PVLV"))
    return false;

  if(disconnect) {
    net->RemovePrjn(pve, output_layer);
  }
  else {
    net->FindMakePrjn(pve, output_layer, onetoone, marker_cons);
  }
  return true;
}
