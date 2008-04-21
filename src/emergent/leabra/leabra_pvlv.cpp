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

//////////////////////////////////
//	PV (NAc) Layer Spec	//
//////////////////////////////////

void PVDetectSpec::Initialize() {
  thr_min = 0.2f;
  thr_max = 0.8f;
}

void PViLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = .5f;		// default is no-information case; extrew = .5
}

void PViLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void PViLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  pv_detect.UpdateAfterEdit();
}

void PViLayerSpec::HelpConfig() {
  String help = "PViLayerSpec Primary Value (inhibitory) Computation:\n\
 Continuously learns about primary rewards, and cancels (inhibits) them in DA computation.\
 It is always trained on the current primary reward (PVe) value from the ExtRew layer\
 (0 = no reward or none avail).\n\
 - Activation is always expectation of reward for current trial\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nPViLayerSpec Configuration:\n\
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

bool PViLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError((us->opt_thresh.learn >= 0.0f), quiet, rval,
		"UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
		"I just set it for you in spec:", us->name,
		"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
  }
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
  
  if(lay->CheckError(ext_rew_lay == NULL, quiet, rval,
		"requires MarkerConSpec connection from PVe/ExtRewLayerSpec layer to get external rewards!")) {
    return false;
  }
  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int eridx = lay->own_net->layers.FindLeafEl(ext_rew_lay);
  if(lay->CheckError(eridx > myidx, quiet, rval,
		"PVe/ExtRew layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }

  return true;
}

float PViLayerSpec::Compute_PVe(LeabraLayer* lay, LeabraNetwork*, bool& actual_er_avail, bool& pv_detected) {
  float pve_val = 0.0f;
  actual_er_avail = true;

  int pve_prjn_idx = 0;
  LeabraLayer* pve_lay = FindLayerFmSpec(lay, pve_prjn_idx, &TA_ExtRewLayerSpec);
  if(pve_lay != NULL) {
    LeabraUnit* pveu = (LeabraUnit*)pve_lay->units.Leaf(0);
    pve_val = pveu->act_eq;
    if(pveu->misc_1 == 0.0f) { // indication of no reward available!
      actual_er_avail = false;
    }
  }

  pv_detected = false;
  LeabraUnit* pviu = (LeabraUnit*)lay->units.Leaf(0);
  if((MAX(pviu->act_m, pve_val) > pv_detect.thr_max) ||
     (MIN(pviu->act_m, pve_val) < pv_detect.thr_min))
    pv_detected = true;

  return pve_val;
}

void PViLayerSpec::Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
  }
}

void PViLayerSpec::Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = Compute_PVe(lay, net, actual_er_avail, pv_detected);

  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     u->ext = pve_val;		// clamp to pve value
     ClampValue(ugp, net); 	// apply new value
     Compute_ExtToPlus(ugp, net); // copy ext values to act_p
     Compute_dWt_Ugp(ugp, lay, net);
     );
}

void PViLayerSpec::Update_PVPrior(LeabraLayer* lay, bool er_avail, float pve_val) {
  LeabraUnit* pvisu = (LeabraUnit*)lay->units.Leaf(0);
  if(er_avail) {
    pvisu->misc_1 = 0.0f;
  }
  else {
    float pvd = pve_val - pvisu->act_m; 
    pvisu->misc_1 = pvd;
  }
}

void PViLayerSpec::Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL) {
    //    if(lay->sravg_sum == 0.0f) return; // if nothing, nothing!
    lay->sravg_nrm = 1.0f / lay->sravg_sum;
  }
  Compute_PVPlusPhaseDwt(lay, net);
  AdaptKWTAPt(lay, net);
  lay->sravg_sum = 0.0f;
}

void PViLayerSpec::Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net) {
  // only do FINAL dWt even though logically it should occur in first plus, because
  // if pbwm is used, then the clamped plus-phase value will be weird, and all the
  // state information should be available etc
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}

void PViLayerSpec::Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}

void PViLayerSpec::Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}

////////////////////////////////////////////////////////
//	PVr = PV reward detection system (habenula?)
////////////////////////////////////////////////////////

void PVrConSpec::Initialize() {
  wt_dec_mult = 0.01f;
}


//////////////////////////////////////////
// 	LV Con Spec			//
//////////////////////////////////////////

void LVConSpec::Initialize() {
//   min_con_type = &TA_PVLVCon;

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

  SetUnique("lrate", true);
  lrate = .05f;
  cur_lrate = .05f;

//   decay = 0.0f;

  syn_dep.depl = 1.1f;
  SetUnique("lrate_sched", true); // not to have any lrate schedule!!
  SetUnique("lrs_value", true); // not to have any lrate schedule!!
  lrs_value = NO_LRS;
}

//////////////////////////////////////////
//	LV Layer Spec: Perceived Value	//
//////////////////////////////////////////

void LVSpec::Initialize() {
  delta_on_sum = false;
  use_actual_er = false;
  min_lvi = 0.0f;
  syn_dep = false;
}

void LVeLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = 0.5f;		// 0.0 for old syn_dep
}

void LVeLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void LVeLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lv.UpdateAfterEdit();
}

void LVeLayerSpec::HelpConfig() {
  String help = "LVeLayerSpec Learned Value computation (excitatory/fast and inhibitory/slow):\n\
 Learns values (LV) according to the PVLV algorithm: looks at current network state\
 and computes how much it resembles states that have been associated with primary value (PV) in the past\n\
 - Activation is always learned values\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - (training only occurs when primary reward is detected by PVi as either present or expected\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nLVeLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons from relevant network state layers (must be LVConSpec)\n\
 - Marker recv con from PVi layer to get training (PVe, pv_detected) signal\n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool LVeLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError((us->opt_thresh.learn >= 0.0f), quiet, rval,
		"UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
		"I just set it for you in spec:", us->name,
		"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
  }
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
    if(!lv.syn_dep) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_LVConSpec), quiet, rval,
		  "requires recv connections to be of type LVConSpec")) {
      return false;
    }
  }
  
  if(lay->CheckError(pvi_lay == NULL, quiet, rval,
		"requires MarkerConSpec connection from PViLayerSpec layer to get DA values!")) {
    return false;
  }

  return true;
}

void LVeLayerSpec::Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
  }
}

void LVeLayerSpec::Compute_DepressWt(Unit_Group* ugp, LeabraLayer*, LeabraNetwork*) {
  int ui;
  for(ui=1;ui<ugp->size;ui++) {	// don't bother with first unit!
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(ui);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      if(!recv_gp->GetConSpec()->InheritsFrom(TA_LVConSpec)) continue;
      LVConSpec* cs = (LVConSpec*)recv_gp->GetConSpec();
      cs->Depress_Wt(recv_gp, u);
    }
  }
}

void LVeLayerSpec::Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  int pvi_prjn_idx = 0;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.SPtr();

  int pvr_prjn_idx = 0;
  LeabraLayer* pvr_lay = FindLayerFmSpec(lay, pvr_prjn_idx, &TA_PVrLayerSpec);
  PVrLayerSpec* pvrls = NULL;
  if(pvr_lay) pvrls = (PVrLayerSpec*)pvr_lay->spec.SPtr();

  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = 0.0f;

  if(pvr_lay) {			// if pvr avail, use it
    pve_val = pvrls->Compute_PVe(pvr_lay, net, actual_er_avail, pv_detected);
  }
  else {
    pve_val = pvils->Compute_PVe(pvi_lay, net, actual_er_avail, pv_detected);
  }

  bool er_avail = pv_detected;
  if(lv.use_actual_er) er_avail = actual_er_avail; // cheat..

  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);

     if(lv.syn_dep) {
       if(er_avail) {
	 u->ext = pve_val;
	 ClampValue(ugp, net); 		// apply new value
	 Compute_ExtToPlus(ugp, net); 	// copy ext values to act_p
	 Compute_dWt_Ugp(ugp, lay, net);
       }
       else {
	 Compute_DepressWt(ugp, lay, net); // always depress!!
       }
     }
     else {
       if(er_avail) {
	 u->ext = pve_val;
	 ClampValue(ugp, net); 		// apply new value
	 Compute_ExtToPlus(ugp, net); 	// copy ext values to act_p
	 Compute_dWt_Ugp(ugp, lay, net);
       }
     }
     );
}

float LVeLayerSpec::Compute_ActEqAvg(LeabraLayer* lay) {
  float act_eq_sum = 0.0f;
  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     act_eq_sum += u->act_eq;
     );

  if(lay->units.gp.size > 0)
    act_eq_sum /= (float)lay->units.gp.size; // average!
  return act_eq_sum;
}

float LVeLayerSpec::Compute_LVDa_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp) {
  LeabraUnit* lveu = (LeabraUnit*)lve_ugp->FastEl(0);
  LeabraUnit* lviu = (LeabraUnit*)lvi_ugp->FastEl(0);

  float lvd = lveu->act_eq - MAX(lviu->act_eq, lv.min_lvi);
  float lv_da = lvd - lveu->misc_1;
  return lv_da;
}

float LVeLayerSpec::Compute_LVDa(LeabraLayer* lve_lay, LeabraLayer* lvi_lay) {
  float lv_da = 0.0f;
  if(lv.delta_on_sum) { // note: this is not the default!  doesn't work as well as other
    LeabraUnit* lveu = (LeabraUnit*)lve_lay->units.Leaf(0); // first guy holds prior val
    float lve_avg = Compute_ActEqAvg(lve_lay);
    float lvi_avg = Compute_ActEqAvg(lvi_lay);
    float lvd = lve_avg - MAX(lvi_avg, lv.min_lvi);
    lv_da = lvd - lveu->misc_1;
  }
  else {
    int gi = 0;
    if(lve_lay->units.gp.size > 0) {
      for(gi=0; gi<lve_lay->units.gp.size; gi++) {
	Unit_Group* lve_ugp = (Unit_Group*)lve_lay->units.gp[gi];
	Unit_Group* lvi_ugp = (Unit_Group*)lvi_lay->units.gp[gi];
	lv_da += Compute_LVDa_ugp(lve_ugp, lvi_ugp);
      }
      lv_da /= (float)lve_lay->units.gp.size; // average!
    }
    else {
      Unit_Group* lve_ugp = (Unit_Group*)&(lve_lay->units);
      Unit_Group* lvi_ugp = (Unit_Group*)&(lvi_lay->units);
      lv_da = Compute_LVDa_ugp(lve_ugp, lvi_ugp);
    } 
  }
  return lv_da;
}

void LVeLayerSpec::Update_LVPrior_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp, bool er_avail) {
  LeabraUnit* lveu = (LeabraUnit*)lve_ugp->FastEl(0);
  if(er_avail) {
    lveu->misc_1 = 0.0f;
    return;
  }
  
  LeabraUnit* lviu = (LeabraUnit*)lvi_ugp->FastEl(0);
  float lvd = lveu->act_eq - lviu->act_eq;
  lveu->misc_1 = lvd;
}

void LVeLayerSpec::Update_LVPrior(LeabraLayer* lve_lay, LeabraLayer* lvi_lay, bool er_avail) {
  if(lv.delta_on_sum) {
    LeabraUnit* lveu = (LeabraUnit*)lve_lay->units.Leaf(0); // first guy holds prior val
    if(er_avail) {
      lveu->misc_1 = 0.0f;
    }
    else {
      float lve_avg = Compute_ActEqAvg(lve_lay);
      float lvi_avg = Compute_ActEqAvg(lvi_lay);
      float lvd = lve_avg - lvi_avg;
      lveu->misc_1 = lvd;
    }
  }
  else {
    int gi = 0;
    if(lve_lay->units.gp.size > 0) {
      for(gi=0; gi<lve_lay->units.gp.size; gi++) {
	Unit_Group* lve_ugp = (Unit_Group*)lve_lay->units.gp[gi];
	Unit_Group* lvi_ugp = (Unit_Group*)lvi_lay->units.gp[gi];
	Update_LVPrior_ugp(lve_ugp, lvi_ugp, er_avail);
      }
    }
    else {
      Unit_Group* lve_ugp = (Unit_Group*)&(lve_lay->units);
      Unit_Group* lvi_ugp = (Unit_Group*)&(lvi_lay->units);
      Update_LVPrior_ugp(lve_ugp, lvi_ugp, er_avail);
    } 
  }
}

void LVeLayerSpec::Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL) {
    //    if(lay->sravg_sum == 0.0f) return; // if nothing, nothing!
    lay->sravg_nrm = 1.0f / lay->sravg_sum;
  }
  Compute_LVPlusPhaseDwt(lay, net);
  AdaptKWTAPt(lay, net);
  lay->sravg_sum = 0.0f;
}

void LVeLayerSpec::Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net) {
  // only do FINAL dWt even though logically it should occur in first plus, because
  // if pbwm is used, then the clamped plus-phase value will be weird, and all the
  // state information should be available etc
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}

void LVeLayerSpec::Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}

void LVeLayerSpec::Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}


//////////////////////////////////////////
//	NV (Novelty Value) Layer Spec	//
//////////////////////////////////////////

void NVSpec::Initialize() {
  da_gain = 1.0f;
  val_thr = 0.1f;
}

void NVLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = 1.0f;		// this is the completely novel value
}

void NVLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
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

bool NVLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError((us->opt_thresh.learn >= 0.0f), quiet, rval,
		"UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
		"I just set it for you in spec:", us->name,
		"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
  }
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

float NVLayerSpec::Compute_NVDa(LeabraLayer* lay) {
  // currently only supporting one unit
  LeabraUnit* nvsu = (LeabraUnit*)lay->units.Leaf(0);
  float nvd = nvsu->act_m - nv.val_thr;
  if(nvd < 0.0f) nvd = 0.0f;
  float nv_da = nvd - nvsu->misc_1;
  return nv.da_gain * nv_da;
}

void NVLayerSpec::Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
  }
}

void NVLayerSpec::Compute_NVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     u->ext = 0.0f;		// clamp to pve value
     ClampValue(ugp, net); 	// apply new value
     Compute_ExtToPlus(ugp, net); // copy ext values to act_p
     Compute_dWt_Ugp(ugp, lay, net);
     );
}

void NVLayerSpec::Update_NVPrior(LeabraLayer* lay, bool er_avail) {
  LeabraUnit* nvsu = (LeabraUnit*)lay->units.Leaf(0);
  if(er_avail) {
    nvsu->misc_1 = 0.0f;	// reset
  }
  else {
    float nvd = nvsu->act_m - nv.val_thr;
    if(nvd < 0.0f) nvd = 0.0f;
    nvsu->misc_1 = nvd;
  }
}

void NVLayerSpec::Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::CTLEABRA_CAL) {
    //    if(lay->sravg_sum == 0.0f) return; // if nothing, nothing!
    lay->sravg_nrm = 1.0f / lay->sravg_sum;
  }
  Compute_NVPlusPhaseDwt(lay, net);
  AdaptKWTAPt(lay, net);
  lay->sravg_sum = 0.0f;
}

void NVLayerSpec::Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net) {
  // only do FINAL dWt even though logically it should occur in first plus, because
  // if pbwm is used, then the clamped plus-phase value will be weird, and all the
  // state information should be available etc
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}

void NVLayerSpec::Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}

void NVLayerSpec::Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_dWt_impl(lay,net);
}



//////////////////////////////////
//	PVLVDa Layer Spec	//
//////////////////////////////////

void PVLVDaSpec::Initialize() {
  da_gain = 1.0f;
  tonic_da = 0.0f;
  min_pvi = 0.0f;
  use_actual_er = false;
  syn_dep = false;
  min_lvi = 0.1f;
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

bool PVLVDaLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

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
      if(lay->CheckError(recv_gp->cons.size <= 0, quiet, rval,
		    "requires one recv projection with at least one unit!")) {
	return false;
      }
      if(fls->InheritsFrom(TA_LVeLayerSpec)) lve_lay = fmlay;
      if(fls->InheritsFrom(TA_LViLayerSpec)) lvi_lay = fmlay;
      if(fls->InheritsFrom(TA_PViLayerSpec)) pvi_lay = fmlay;
    }
  }

  if(lay->CheckError(lve_lay == NULL, quiet, rval,
		"did not find LVe layer to get Da from!")) {
    return false;
  }
  if(lay->CheckError(lvi_lay == NULL, quiet, rval,
		"did not find LVi layer to get Da from!")) {
    return false;
  }
  if(lay->CheckError(pvi_lay == NULL, quiet, rval,
		"did not find PVi layer to get Da from!")) {
    return false;
  }

  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int lvidx = lay->own_net->layers.FindLeafEl(lve_lay);
  if(lay->CheckError(lvidx > myidx, quiet, rval,
		"LVe layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }
  lvidx = lay->own_net->layers.FindLeafEl(lvi_lay);
  if(lay->CheckError(lvidx > myidx, quiet, rval,
		"LVi layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }
  lvidx = lay->own_net->layers.FindLeafEl(pvi_lay);
  if(lay->CheckError(lvidx > myidx, quiet, rval,
		"PVi layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }

  return true;
}

void PVLVDaLayerSpec::Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork*) {
  lay->dav = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->ext = da.tonic_da;
    u->SetExtFlag(Unit::EXT);
  }      
}

void PVLVDaLayerSpec::Compute_Da_SynDep(LeabraLayer* lay, LeabraNetwork* net) {
  int lve_prjn_idx;
  FindLayerFmSpec(lay, lve_prjn_idx, &TA_LVeLayerSpec);
  int lvi_prjn_idx;
  FindLayerFmSpec(lay, lvi_prjn_idx, &TA_LViLayerSpec);
  int pvi_prjn_idx;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.SPtr();

  int pvr_prjn_idx = 0;
  LeabraLayer* pvr_lay = FindLayerFmSpec(lay, pvr_prjn_idx, &TA_PVrLayerSpec);
  PVrLayerSpec* pvrls = NULL;
  if(pvr_lay) pvrls = (PVrLayerSpec*)pvr_lay->spec.SPtr();

  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = 0.0f;

  if(pvr_lay) {			// if pvr avail, use it
    pve_val = pvrls->Compute_PVe(pvr_lay, net, actual_er_avail, pv_detected);
  }
  else {
    pve_val = pvils->Compute_PVe(pvi_lay, net, actual_er_avail, pv_detected);
  }

  bool er_avail = pv_detected;
  if(da.use_actual_er) er_avail = actual_er_avail; // cheat..

  lay->dav = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    LeabraRecvCons* lvecg = (LeabraRecvCons*)u->recv[lve_prjn_idx];
    LeabraUnit* lvesu = (LeabraUnit*)lvecg->Un(0);
    LeabraRecvCons* lvicg = (LeabraRecvCons*)u->recv[lvi_prjn_idx];
    LeabraUnit* lvisu = (LeabraUnit*)lvicg->Un(0);
    LeabraRecvCons* pvicg = (LeabraRecvCons*)u->recv[pvi_prjn_idx];
    LeabraUnit* pvisu = (LeabraUnit*)pvicg->Un(0);
    float eff_lvi = MAX(lvisu->act_eq, da.min_lvi); // effective lvi value
    float lv_da = lvesu->act_eq - eff_lvi; 
    float pv_da = pve_val - pvisu->act_m; 

    if(net->phase_no == 0) {	// not used at this point..
      u->dav = da.da_gain * lv_da; 		// lviu->act_eq - avgbl;
    }
    else {
//       if(da.mode == PVLVDaSpec::LV_PLUS_IF_PV) {
	u->dav = da.da_gain * lv_da;
	if(er_avail)
	  u->dav += da.da_gain * pv_da;
//       }
//       else if(da.mode == PVLVDaSpec::IF_PV_ELSE_LV) {
// 	if(er_avail)
// 	  u->dav = pv_da;
// 	else
// 	  u->dav = lv_da;
//       }
//       else if(da.mode == PVLVDaSpec::PV_PLUS_LV) {
// 	u->dav = pv_da + lv_da;
//       }
    }
    u->ext = da.tonic_da + u->dav;
    u->act_eq = u->act = u->net = u->ext;
    lay->dav += u->dav;
  }
  if(lay->units.leaves > 0) lay->dav /= (float)lay->units.leaves;
}

void PVLVDaLayerSpec::Compute_Da_LvDelta(LeabraLayer* lay, LeabraNetwork* net) {
  int lve_prjn_idx;
  LeabraLayer* lve_lay = FindLayerFmSpec(lay, lve_prjn_idx, &TA_LVeLayerSpec);
  LVeLayerSpec* lve_sp = (LVeLayerSpec*)lve_lay->GetLayerSpec();
  int lvi_prjn_idx;
  LeabraLayer* lvi_lay = FindLayerFmSpec(lay, lvi_prjn_idx, &TA_LViLayerSpec);
//   LVeLayerSpec* lvi_sp = (LViLayerSpec*)lvi_lay->GetLayerSpec();

  int pvi_prjn_idx;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.SPtr();

  int pvr_prjn_idx = 0;
  LeabraLayer* pvr_lay = FindLayerFmSpec(lay, pvr_prjn_idx, &TA_PVrLayerSpec);
  PVrLayerSpec* pvrls = NULL;
  if(pvr_lay) pvrls = (PVrLayerSpec*)pvr_lay->spec.SPtr();

  int nv_prjn_idx;
  LeabraLayer* nv_lay = FindLayerFmSpec(lay, nv_prjn_idx, &TA_NVLayerSpec);
  NVLayerSpec* nvls = NULL;
  if(nv_lay) nvls = (NVLayerSpec*)nv_lay->spec.SPtr();

  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = 0.0f;

  if(pvr_lay) {			// if pvr avail, use it
    pve_val = pvrls->Compute_PVe(pvr_lay, net, actual_er_avail, pv_detected);
  }
  else {
    pve_val = pvils->Compute_PVe(pvi_lay, net, actual_er_avail, pv_detected);
  }

  bool er_avail = pv_detected;
  if(da.use_actual_er) er_avail = actual_er_avail; // cheat..

  float lv_da = lve_sp->Compute_LVDa(lve_lay, lvi_lay);

  // nv only contributes to lv, not pv..
  if(nv_lay) {
    lv_da += nvls->Compute_NVDa(nv_lay);
  }

  // note that multiple LV subgroups are supported, but not multiple PV's (yet!)
  LeabraUnit* pvisu = (LeabraUnit*)pvi_lay->units.Leaf(0);
  float pvd = pve_val - MAX(pvisu->act_m, da.min_pvi); 
  float pv_da = pvd - pvisu->misc_1;

  lay->dav = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    if(net->phase_no == 0) {
      u->dav = 0.0f;
    }
    else {
      if(er_avail) 				 // either PV or LV
	u->dav = da.da_gain * pv_da;
      else 
	u->dav = da.da_gain * lv_da;
    }

    u->ext = da.tonic_da + u->dav;
    u->act_eq = u->act = u->net = u->ext;
    lay->dav += u->dav;
  }
  if(lay->units.leaves > 0) lay->dav /= (float)lay->units.leaves;
}

void PVLVDaLayerSpec::Update_LvDelta(LeabraLayer* lay, LeabraNetwork* net) {
  int lve_prjn_idx;
  LeabraLayer* lve_lay = FindLayerFmSpec(lay, lve_prjn_idx, &TA_LVeLayerSpec);
  LVeLayerSpec* lve_sp = (LVeLayerSpec*)lve_lay->GetLayerSpec();
  int lvi_prjn_idx;
  LeabraLayer* lvi_lay = FindLayerFmSpec(lay, lvi_prjn_idx, &TA_LViLayerSpec);
//   LVeLayerSpec* lvi_sp = (LViLayerSpec*)lvi_lay->GetLayerSpec();

  int pvi_prjn_idx;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.SPtr();

  int pvr_prjn_idx = 0;
  LeabraLayer* pvr_lay = FindLayerFmSpec(lay, pvr_prjn_idx, &TA_PVrLayerSpec);
  PVrLayerSpec* pvrls = NULL;
  if(pvr_lay) pvrls = (PVrLayerSpec*)pvr_lay->spec.SPtr();

  int nv_prjn_idx;
  LeabraLayer* nv_lay = FindLayerFmSpec(lay, nv_prjn_idx, &TA_NVLayerSpec);
  NVLayerSpec* nvls = NULL;
  if(nv_lay) nvls = (NVLayerSpec*)nv_lay->spec.SPtr();

  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = 0.0f;

  if(pvr_lay) {			// if pvr avail, use it
    pve_val = pvrls->Compute_PVe(pvr_lay, net, actual_er_avail, pv_detected);
  }
  else {
    pve_val = pvils->Compute_PVe(pvi_lay, net, actual_er_avail, pv_detected);
  }

  bool er_avail = pv_detected;
  if(da.use_actual_er) er_avail = actual_er_avail; // cheat..

  lve_sp->Update_LVPrior(lve_lay, lvi_lay, er_avail);

  if(nv_lay) {
    nvls->Update_NVPrior(nv_lay, er_avail);
  }

  pvils->Update_PVPrior(pvi_lay, er_avail, pve_val);
}

void PVLVDaLayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork*) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())	continue;
      for(int j=0;j<send_gp->cons.size; j++) {
	((LeabraUnit*)send_gp->Un(j))->dav = u->act;
      }
    }
  }
}

void PVLVDaLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing
  if(da.syn_dep)
    Compute_Da_SynDep(lay, net);	// now get the da and clamp it to layer
  else
    Compute_Da_LvDelta(lay, net);
  Send_Da(lay, net);
  Compute_ActAvg(lay, net);
}

void PVLVDaLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = false;
  lay->UnSetExtFlag(Unit::EXT);
  inherited::Compute_HardClamp(lay, net);
}

void PVLVDaLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(da.syn_dep) return;

  if(net->phase_no == net->phase_max-1) { // only at very end!
    Update_LvDelta(lay, net);
  }
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

///////////////////////////////////////////////////////////////
//			PVLV
///////////////////////////////////////////////////////////////

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
 to the ExtRew layer, using the MarkerConSpec (MarkerCons) Con spec.\
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
  String vtanm = "DA";

  bool dumbo;
  LeabraLayer* rew_targ_lay = (LeabraLayer*)net->FindMakeLayer("RewTarg");
  LeabraLayer* pve = (LeabraLayer*)net->FindMakeLayer(pvenm, NULL, dumbo);
  LeabraLayer* pvr = (LeabraLayer*)net->FindMakeLayer(pvrnm, NULL, pvr_new);
  LeabraLayer* pvi = (LeabraLayer*)net->FindMakeLayer(pvinm, NULL, dumbo);
  LeabraLayer* lve = (LeabraLayer*)net->FindMakeLayer(lvenm, NULL, lve_new);
  LeabraLayer* lvi = (LeabraLayer*)net->FindMakeLayer(lvinm, NULL, dumbo);
  LeabraLayer* nv =  (LeabraLayer*)net->FindMakeLayer(nvnm, NULL, nv_new);
  LeabraLayer* vta = (LeabraLayer*)net->FindMakeLayer(vtanm, NULL, dumbo);
  if(rew_targ_lay == NULL || lve == NULL || pve == NULL || pvi == NULL || vta == NULL) return false;

  //////////////////////////////////////////////////////////////////////////////////
  // sort layers

  rew_targ_lay->name = "0000";  pve->name = "0001";
  pvr->name = "0002";  pvi->name = "0003";  
  lve->name = "0005";  lvi->name = "0006";
  nv->name =  "0008"; vta->name = "0009";

  net->layers.Sort();

  rew_targ_lay->name = "RewTarg";  pve->name = pvenm;
  pvr->name = pvrnm;  pvi->name = pvinm; 
  lve->name = lvenm; lvi->name = lvinm;
  nv->name = nvnm;   vta->name = vtanm;

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  Layer_Group other_lays;
  Layer_Group hidden_lays;
  Layer_Group output_lays;
  Layer_Group input_lays;
  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != lve && lay != pve && lay != pvr && lay != pvi &&
       lay != lvi && lay != nv && lay != vta
       && !laysp->InheritsFrom(&TA_PFCLayerSpec) && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
      other_lays.Link(lay);
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
//   if(!bio_labels)
//     gpprfx = "DA_";

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

  PVConSpec* pvi_cons = (PVConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  PVConSpec* pvr_cons = (PVConSpec*)pvi_cons->FindMakeChild("PVr", &TA_PVConSpec);
  LeabraConSpec* lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_PVConSpec);
  LeabraConSpec* lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_PVConSpec);
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

  lvesp->lv.syn_dep = false; // old_syn_dep;

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
  pvisp->pv_detect.thr_min = .2f;
  pvisp->pv_detect.thr_max = .8f;
  pvrsp->pv_detect.thr_min = .2f;
  pvrsp->pv_detect.thr_max = .8f;

  pv_units->SetUnique("act", true);
  pv_units->SetUnique("act_fun", true);
  pv_units->SetUnique("dt", true);
  //    pv_units->SetUnique("opt_thresh", true);
  pv_units->act_fun = LeabraUnitSpec::NOISY_LINEAR;
  pv_units->act.thr = .17f;
  pv_units->act.gain = 220.0f;
  pv_units->act.nvar = .01f;
  pv_units->g_bar.l = .1f;
  pv_units->g_bar.h = .03f;  pv_units->g_bar.a = .09f;
  pv_units->dt.vm = .05f;
  pv_units->dt.vm_eq_cyc = 100; // go straight to equilibrium!
  //    pv_units->opt_thresh.send = 0.0f; // scalar val may use this -- don't let it!

  pvi_cons->SetUnique("lmix", true);
  pvi_cons->lmix.err_sb = false; 
  pvi_cons->SetUnique("rnd", true);
  pvi_cons->rnd.mean = 0.1f;
  pvi_cons->rnd.var = 0.0f;

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

  if(pvi->un_geom.n != n_lv_u) { pvi->un_geom.n = n_lv_u; pvi->un_geom.x = n_lv_u; pvi->un_geom.y = 1; }
  if(lve->un_geom.n != n_lv_u) { lve->un_geom.n = n_lv_u; lve->un_geom.x = n_lv_u; lve->un_geom.y = 1; }
  if(lvi->un_geom.n != n_lv_u) { lvi->un_geom.n = n_lv_u; lvi->un_geom.x = n_lv_u; lvi->un_geom.y = 1; }
  if(pve->un_geom.n != n_lv_u) { pve->un_geom.n = n_lv_u; pve->un_geom.x = n_lv_u; pve->un_geom.y = 1; }
  if(pvr->un_geom.n != n_lv_u) { pvr->un_geom.n = n_lv_u; pvr->un_geom.x = n_lv_u; pvr->un_geom.y = 1; }
  if(nv->un_geom.n != n_lv_u) { nv->un_geom.n = n_lv_u; nv->un_geom.x = n_lv_u; nv->un_geom.y = 1; }
  vta->un_geom.n = 1;
  rew_targ_lay->un_geom.n = 1;
  rew_targ_lay->layer_type = Layer::INPUT;

  if(lve_new) {
    pve->pos.z = 0; pve->pos.y = 0; pve->pos.x = 0;
    pvi->pos.z = 0; pvi->pos.y = 2; pvi->pos.x = 0;

    vta->pos.z = 0; vta->pos.y = 4; vta->pos.x = 6;
    rew_targ_lay->pos.z = 0; rew_targ_lay->pos.y = 4; rew_targ_lay->pos.x = 15;

    lve->pos.z = 0; lve->pos.y = 0; lve->pos.x = 6;
    lvi->pos.z = 0; lvi->pos.y = 2; lvi->pos.x = 6;
  }

  if(pvr_new) {
    pvr->pos.z = 0; pvr->pos.y = 4; pvr->pos.x = 0;
  }
  if(nv_new) {
    nv->pos.z = 0;  nv->pos.y = 4; nv->pos.x = 9;
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

  bool ok = pvisp->CheckConfig_Layer(pvi, true) && lvesp->CheckConfig_Layer(lve, true)
    && lvisp->CheckConfig_Layer(lve, true)
    && dasp->CheckConfig_Layer(vta, true) && pvesp->CheckConfig_Layer(pve, true)
    && pvrsp->CheckConfig_Layer(pvr, true) && nvsp->CheckConfig_Layer(nv, true);
  
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
  if(edit != NULL) {
    pvr_cons->SelectForEditNm("lrate", edit, "pvr");
    pvi_cons->SelectForEditNm("lrate", edit, "pvi");
    lve_cons->SelectForEditNm("lrate", edit, "lve");
    lvi_cons->SelectForEditNm("lrate", edit, "lvi");
    nv_cons->SelectForEditNm("lrate", edit, "nv");
    pvesp->SelectForEditNm("rew", edit, "pve");
//     lvesp->SelectForEditNm("lv", edit, "lve");
    pvrsp->SelectForEditNm("pv_detect", edit, "pvr");
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
