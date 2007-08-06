// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
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
 - All units I recv from must be DaModUnit/Spec units\n\
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
  if(lay->CheckError(net->no_plus_test, quiet, rval,
		"requires LeabraNetwork no_plus_test = false, I just set it for you")) {
    net->no_plus_test = false;
  }
  if(lay->CheckError(!lay->units.el_typ->InheritsFrom(TA_DaModUnit), quiet, rval,
		"must have DaModUnits!")) {
    return false;
  }

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(!us->InheritsFrom(TA_DaModUnitSpec), quiet, rval,
		"UnitSpec must be DaModUnitSpec!")) {
    return false;
  }
  if(lay->CheckError((us->opt_thresh.learn >= 0.0f) || us->opt_thresh.updt_wts, quiet, rval,
		"UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
		"I just set it for you in spec:", us->name,
		"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
    us->opt_thresh.updt_wts = false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = false; // don't need prior state dwt
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
    if(recv_gp->prjn->from == recv_gp->prjn->layer) { // self projection, skip it
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

void PViLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return; // only do FINAL dwt!
  Compute_PVPlusPhaseDwt(lay, net);
  AdaptKWTAPt(lay, net);
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
 - All units I recv from must be DaModUnit/Spec units\n\
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
  if(lay->CheckError(net->no_plus_test, quiet, rval,
		"requires LeabraNetwork no_plus_test = false, I just set it for you")) {
    net->no_plus_test = false;
  }
  if(lay->CheckError(!lay->units.el_typ->InheritsFrom(TA_DaModUnit), quiet, rval,
		"must have DaModUnits!")) {
    return false;
  }

  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(!us->InheritsFrom(TA_DaModUnitSpec), quiet, rval,
		"UnitSpec must be DaModUnitSpec!")) {
    return false;
  }
  if(lay->CheckError((us->opt_thresh.learn >= 0.0f) || us->opt_thresh.updt_wts, quiet, rval,
		"UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
		"I just set it for you in spec:", us->name,
		"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
    us->opt_thresh.updt_wts = false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = false; // don't need prior state dwt
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* pvi_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from == recv_gp->prjn->layer) { // self projection, skip it
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
    DaModUnit* u = (DaModUnit*)ugp->FastEl(ui);
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
     DaModUnit* u = (DaModUnit*)ugp->FastEl(0);

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
     DaModUnit* u = (DaModUnit*)ugp->FastEl(0);
     act_eq_sum += u->act_eq;
     );

  if(lay->units.gp.size > 0)
    act_eq_sum /= (float)lay->units.gp.size; // average!
  return act_eq_sum;
}

float LVeLayerSpec::Compute_LvDa_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp) {
  DaModUnit* lveu = (DaModUnit*)lve_ugp->FastEl(0);
  DaModUnit* lviu = (DaModUnit*)lvi_ugp->FastEl(0);

  float lvd = lveu->act_eq - lviu->act_eq;
  float lv_da = lvd - lveu->misc_1;
  return lv_da;
}

float LVeLayerSpec::Compute_LvDa(LeabraLayer* lve_lay, LeabraLayer* lvi_lay) {
  float lv_da = 0.0f;
  if(lv.delta_on_sum) {
    DaModUnit* lveu = (DaModUnit*)lve_lay->units.Leaf(0); // first guy holds prior val
    float lve_avg = Compute_ActEqAvg(lve_lay);
    float lvi_avg = Compute_ActEqAvg(lvi_lay);
    float lvd = lve_avg - lvi_avg;
    lv_da = lvd - lveu->misc_1;
  }
  else {
    int gi = 0;
    if(lve_lay->units.gp.size > 0) {
      for(gi=0; gi<lve_lay->units.gp.size; gi++) {
	Unit_Group* lve_ugp = (Unit_Group*)lve_lay->units.gp[gi];
	Unit_Group* lvi_ugp = (Unit_Group*)lvi_lay->units.gp[gi];
	lv_da += Compute_LvDa_ugp(lve_ugp, lvi_ugp);
      }
      lv_da /= (float)lve_lay->units.gp.size; // average!
    }
    else {
      Unit_Group* lve_ugp = (Unit_Group*)&(lve_lay->units);
      Unit_Group* lvi_ugp = (Unit_Group*)&(lvi_lay->units);
      lv_da = Compute_LvDa_ugp(lve_ugp, lvi_ugp);
    } 
  }
  return lv_da;
}

void LVeLayerSpec::Update_LvPrior_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp, bool er_avail) {
  DaModUnit* lveu = (DaModUnit*)lve_ugp->FastEl(0);
  if(er_avail) {
    lveu->misc_1 = 0.0f;
    return;
  }
  
  DaModUnit* lviu = (DaModUnit*)lvi_ugp->FastEl(0);
  float lvd = lveu->act_eq - lviu->act_eq;
  lveu->misc_1 = lvd;
}

void LVeLayerSpec::Update_LvPrior(LeabraLayer* lve_lay, LeabraLayer* lvi_lay, bool er_avail) {
  if(lv.delta_on_sum) {
    DaModUnit* lveu = (DaModUnit*)lve_lay->units.Leaf(0); // first guy holds prior val
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
	Update_LvPrior_ugp(lve_ugp, lvi_ugp, er_avail);
      }
    }
    else {
      Unit_Group* lve_ugp = (Unit_Group*)&(lve_lay->units);
      Unit_Group* lvi_ugp = (Unit_Group*)&(lvi_lay->units);
      Update_LvPrior_ugp(lve_ugp, lvi_ugp, er_avail);
    } 
  }
}

void LVeLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  // doing second because act_p is computed only at end of settling!
  // this is better than clamping the value in the middle of everything
  // and then continuing with settling..
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_LVPlusPhaseDwt(lay, net);
  AdaptKWTAPt(lay, net);
}

//////////////////////////////////
//	PVLVDa Layer Spec	//
//////////////////////////////////

void PVLVDaSpec::Initialize() {
  da_gain = 1.0f;
  tonic_da = 0.0f;
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
  SetUnique("compute_i", true);
  compute_i = KWTA_INHIB;
  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .25;
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
 - Sending cons to units of type DaModUnit/Spec; puts into their da value\n\
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
      if(lay->CheckError(!recv_gp->Un(0)->InheritsFrom(TA_DaModUnit), quiet, rval,
		    "I need to receive from a DaModUnit!")) {
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

  // check sending layer projections for appropriate unit types
  int si;
  for(si=0;si<lay->send_prjns.size;si++) {
    Projection* prjn = (Projection*)lay->send_prjns[si];
    if(lay->CheckError(!prjn->from->units.el_typ->InheritsFrom(TA_DaModUnit), quiet, rval,
		  "all layers I send to must have DaModUnits!, layer:",
		  prjn->from->GetPath(),"doesn't")) {
      return false;
    }
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
  DaModUnit* u;
  taLeafItr i;
  FOR_ITR_EL(DaModUnit, u, lay->units., i) {
    LeabraRecvCons* lvecg = (LeabraRecvCons*)u->recv[lve_prjn_idx];
    DaModUnit* lvesu = (DaModUnit*)lvecg->Un(0);
    LeabraRecvCons* lvicg = (LeabraRecvCons*)u->recv[lvi_prjn_idx];
    DaModUnit* lvisu = (DaModUnit*)lvicg->Un(0);
    LeabraRecvCons* pvicg = (LeabraRecvCons*)u->recv[pvi_prjn_idx];
    DaModUnit* pvisu = (DaModUnit*)pvicg->Un(0);
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

  float lv_da = lve_sp->Compute_LvDa(lve_lay, lvi_lay);

  // note that multiple LV subgroups are supported, but not multiple PV's
  DaModUnit* pvisu = (DaModUnit*)pvi_lay->units.Leaf(0);
  float pvd = pve_val - pvisu->act_m; 
  float pv_da = pvd - pvisu->misc_1;

  lay->dav = 0.0f;
  DaModUnit* u;
  taLeafItr i;
  FOR_ITR_EL(DaModUnit, u, lay->units., i) {
    if(net->phase_no == 0) {	// not used at this point..
      u->dav = lv_da; 		// lviu->act_eq - avgbl;
    }
    else {
      // IF_PV_ELSE_LV mode always:
      if(er_avail) {
	u->dav = da.da_gain * pv_da;
      }
      else {
	u->dav = da.da_gain * lv_da;
      }
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

  lve_sp->Update_LvPrior(lve_lay, lvi_lay, er_avail);

  // todo: could put this in pv to support pv subgroups as in lv..
  DaModUnit* pvisu = (DaModUnit*)pvi_lay->units.Leaf(0);
  if(er_avail) {
    pvisu->misc_1 = 0.0f;
  }
  else {
    float pvd = pve_val - pvisu->act_m; 
    pvisu->misc_1 = pvd;
  }
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
	((DaModUnit*)send_gp->Un(j))->dav = u->act;
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

void PVLVDaLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both) {
  inherited::PostSettle(lay, net, set_both);

  if(da.syn_dep) return;

  if(net->phase_no == net->phase_max-1) { // only at very end!
    Update_LvDelta(lay, net);
  }
}

void PVLVDaLayerSpec::Compute_dWt(LeabraLayer*, LeabraNetwork*) {
  return;
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

///////////////////////////////////////////////////////////////
//			PVLV
///////////////////////////////////////////////////////////////

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

void LeabraWizard::PVLV(LeabraNetwork* net, bool bio_labels, bool localist_val,
			bool fm_hid_cons, bool fm_out_cons, bool da_mod_all,
			bool old_syn_dep) {
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
  String pvenm = "PVe_LHA";  String pvinm = "PVi_VSpatch";  String pvrnm = "PVr_VShab";
  String lvenm = "LVe_CNA";  String lvinm = "LVi_CNA";
  String vtanm = "VTA";
  String alt_pvenm = "PVe";  String alt_pvinm = "PVi";  String alt_pvrnm = "PVr";
  String alt_lvenm = "LVe";  String alt_lvinm = "LVi";
  String alt_vtanm = "DA";
  if(!bio_labels) {
    pvenm = "PVe"; 	pvinm = "PVi";    pvrnm = "PVr"; 
    lvenm = "LVe";    lvinm = "LVi";    vtanm = "DA";
    alt_pvenm = "PVe_LHA"; alt_pvinm = "PVi_VSpatch"; alt_pvrnm = "PVr_VShab";
    alt_lvenm = "LVe_CNA"; alt_lvinm = "LVi_VSpatch";
    alt_vtanm = "VTA";
  }

  bool dumbo;
  LeabraLayer* rew_targ_lay = (LeabraLayer*)net->FindMakeLayer("RewTarg");
  LeabraLayer* pve = (LeabraLayer*)net->FindMakeLayer(pvenm, NULL, dumbo, alt_pvenm);
  LeabraLayer* pvr = NULL;
  if(!old_syn_dep)
    pvr = (LeabraLayer*)net->FindMakeLayer(pvrnm, NULL, dumbo, alt_pvrnm);
  LeabraLayer* pvi = (LeabraLayer*)net->FindMakeLayer(pvinm, NULL, dumbo, alt_pvinm);
  LeabraLayer* lve = (LeabraLayer*)net->FindMakeLayer(lvenm, NULL, lve_new, alt_lvenm);
  LeabraLayer* lvi = (LeabraLayer*)net->FindMakeLayer(lvinm, NULL, dumbo, alt_lvinm);
  LeabraLayer* vta = (LeabraLayer*)net->FindMakeLayer(vtanm, NULL, dumbo, alt_vtanm);
  if(rew_targ_lay == NULL || lve == NULL || pve == NULL || pvi == NULL || vta == NULL) return;

  //////////////////////////////////////////////////////////////////////////////////
  // sort layers

  rew_targ_lay->name = "0000";  pve->name = "0001";
  if(!old_syn_dep)
    pvr->name = "0002";
  pvi->name = "0003";  lve->name = "0004";  lvi->name = "0005";    vta->name = "0006";

  net->layers.Sort();

  rew_targ_lay->name = "RewTarg";  pve->name = pvenm;
  if(!old_syn_dep)
    pvr->name = pvrnm;
  pvi->name = pvinm;  lve->name = lvenm; lvi->name = lvinm; vta->name = vtanm;

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
    lay->SetUnitType(&TA_DaModUnit);
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != lve && lay != pve && lay != pvr && lay != pvi &&
       lay != lvi && lay != vta
       && !laysp->InheritsFrom(&TA_PFCLayerSpec) && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_PatchLayerSpec) 
       && !laysp->InheritsFrom(&TA_SNcLayerSpec) && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
      other_lays.Link(lay);
      if(lay->layer_type == Layer::HIDDEN)
	hidden_lays.Link(lay);
      else if(lay->layer_type == Layer::INPUT)
	input_lays.Link(lay);
      else 
	output_lays.Link(lay);
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
      if(us == NULL || !us->InheritsFrom(TA_DaModUnitSpec)) {
	us->ChangeMyType(&TA_DaModUnitSpec);
      }
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
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return;

  LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* lv_units = (LeabraUnitSpec*)pv_units->FindMakeChild("LVUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_DaModUnitSpec);
  if(lv_units == NULL || pv_units == NULL || da_units == NULL) return;

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(learn_cons == NULL) return;

  PVConSpec* pvi_cons = (PVConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  PVConSpec* pvr_cons = NULL;
  LeabraConSpec* lve_cons = NULL;
  LeabraConSpec* lvi_cons = NULL;
  if(old_syn_dep) {
    lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_LVConSpec);
    lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_LVConSpec);
  }
  else {
    pvr_cons = (PVConSpec*)pvi_cons->FindMakeChild("PVr", &TA_PVConSpec);
    lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_PVConSpec);
    lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_PVConSpec);
  }
  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  if(bg_bias == NULL) return;
  LeabraConSpec* fixed_bias = (LeabraConSpec*)bg_bias->FindMakeChild("FixedBias", &TA_LeabraBiasSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  if(lve_cons == NULL || marker_cons == NULL || fixed_bias == NULL)
    return;

  ExtRewLayerSpec* pvesp = (ExtRewLayerSpec*)layers->FindMakeSpec(pvenm + "Layer", &TA_ExtRewLayerSpec);
  PVrLayerSpec* pvrsp = NULL;
  if(!old_syn_dep)
    pvrsp = (PVrLayerSpec*)layers->FindMakeSpec(pvrnm + "Layer", &TA_PVrLayerSpec, dumbo, alt_pvrnm + "Layer");
  PViLayerSpec* pvisp = (PViLayerSpec*)layers->FindMakeSpec(pvinm + "Layer", &TA_PViLayerSpec, dumbo, alt_pvinm + "Layer");
  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec, dumbo, alt_lvenm + "Layer");
  LViLayerSpec* lvisp = (LViLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec, dumbo, alt_lvinm + "Layer");
  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindMakeSpec(vtanm + "Layer", &TA_PVLVDaLayerSpec, dumbo, alt_vtanm + "Layer");
  if(lvesp == NULL || pvesp == NULL || pvisp == NULL || dasp == NULL) return;

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  if(fullprjn == NULL || onetoone == NULL) return;

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  //  learn_cons->lmix.hebb = .01f; // .01 hebb on learn cons
//   learn_cons->not_used_ok = true;
  learn_cons->UpdateAfterEdit();
  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;
  fixed_bias->SetUnique("lrate", true);
  fixed_bias->lrate = 0.0f;

  if(!old_syn_dep)
    pvr_cons->SetUnique("lrate", true);
  pvi_cons->SetUnique("lrate", true);
  lve_cons->SetUnique("lrate", true);
  lvi_cons->SetUnique("lrate", true);

  // NOT unique: inherit from pvi:
  lve_cons->SetUnique("rnd", false);
  lve_cons->SetUnique("wt_limits", false);
  lve_cons->SetUnique("wt_sig", false);
  lve_cons->SetUnique("lmix", false);
  lve_cons->SetUnique("lrate_sched", false);
  lve_cons->SetUnique("lrs_value", false);

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
  lvisp->SetUnique("compute_i", false);
  lvisp->SetUnique("i_kwta_pt", false);

  pv_units->SetUnique("g_bar", true);

  lvesp->lv.syn_dep = old_syn_dep;

  if(localist_val) {
    pvesp->scalar.rep = ScalarValSpec::LOCALIST;
    pvisp->scalar.rep = ScalarValSpec::LOCALIST;
    lvesp->scalar.rep = ScalarValSpec::LOCALIST;

    pvesp->scalar.min_sum_act = .2f;
    pvisp->scalar.min_sum_act = .2f;
    lvesp->scalar.min_sum_act = .2f;

    lvesp->bias_val.un = ScalarValBias::GC;
    lvesp->bias_val.wt = ScalarValBias::NO_WT;
    if(old_syn_dep)
      lvesp->bias_val.val = 0.0f;
    else
      lvesp->bias_val.val = 0.5f;
    pvisp->bias_val.un = ScalarValBias::GC;
    pvisp->bias_val.wt = ScalarValBias::NO_WT;
    pvisp->bias_val.val = 0.5f;
    pvisp->pv_detect.thr_min = .2f;
    pvisp->pv_detect.thr_max = .8f;

    pvesp->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB; pvesp->i_kwta_pt = 0.9f;
    pvisp->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB; pvisp->i_kwta_pt = 0.9f;
    lvesp->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB; lvesp->i_kwta_pt = 0.9f;

    pvesp->kwta.k_from = KWTASpec::USE_K; pvesp->gp_kwta.k_from = KWTASpec::USE_K;
    pvesp->kwta.k = 1; 	pvesp->gp_kwta.k = 1; 
    pvisp->kwta.k_from = KWTASpec::USE_K; pvisp->gp_kwta.k_from = KWTASpec::USE_K;
    pvisp->kwta.k = 1; 	pvisp->gp_kwta.k = 1; 
    lvesp->kwta.k_from = KWTASpec::USE_K; lvesp->gp_kwta.k_from = KWTASpec::USE_K;
    lvesp->kwta.k = 1; 	lvesp->gp_kwta.k = 1; 

    pvesp->unit_range.min = 0.0f;  pvesp->unit_range.max = 1.0f;
    pvesp->unit_range.UpdateAfterEdit();
    pvesp->val_range = pvesp->unit_range;

    pvisp->unit_range.min = 0.0f;  pvisp->unit_range.max = 1.0f;
    pvisp->unit_range.UpdateAfterEdit();
    pvisp->val_range = pvisp->unit_range;

    lvesp->unit_range.min = 0.0f;  lvesp->unit_range.max = 1.0f;
    lvesp->unit_range.UpdateAfterEdit();
    lvesp->val_range = lvesp->unit_range;

    pv_units->SetUnique("act", true);
    pv_units->SetUnique("act_fun", true);
    pv_units->SetUnique("dt", true);
    pv_units->SetUnique("opt_thresh", true);
    pv_units->act_fun = LeabraUnitSpec::NOISY_LINEAR;
    pv_units->act.thr = .17f;
    pv_units->act.gain = 220.0f;
    pv_units->act.nvar = .01f;
    pv_units->g_bar.l = .1f;
    pv_units->g_bar.h = .03f;  pv_units->g_bar.a = .09f;
    pv_units->dt.vm = .05f;
    pv_units->dt.vm_eq_cyc = 100; // go straight to equilibrium!
    pv_units->opt_thresh.send = 0.0f; // scalar val may use this -- don't let it!

    pvi_cons->SetUnique("lmix", true);
    pvi_cons->lmix.err_sb = false; 
    pvi_cons->SetUnique("rnd", true);
    pvi_cons->rnd.mean = 0.1f;
    pvi_cons->rnd.var = 0.0f;
  }
  else {			// GAUSSIAN
    pvesp->scalar.rep = ScalarValSpec::GAUSSIAN;
    pvisp->scalar.rep = ScalarValSpec::GAUSSIAN;
    lvesp->scalar.rep = ScalarValSpec::GAUSSIAN;

    lvesp->bias_val.un = ScalarValBias::GC;
    lvesp->bias_val.wt = ScalarValBias::NO_WT;
    if(old_syn_dep)
      lvesp->bias_val.val = 0.0f;
    else
      lvesp->bias_val.val = 0.5f;
    pvisp->bias_val.un = ScalarValBias::GC;
    pvisp->bias_val.wt = ScalarValBias::NO_WT;
    pvisp->bias_val.val = 0.5f;

    pvesp->compute_i = LeabraLayerSpec::KWTA_INHIB; pvesp->i_kwta_pt = 0.25f;
    pvisp->compute_i = LeabraLayerSpec::KWTA_INHIB; pvisp->i_kwta_pt = 0.25f;
    lvesp->compute_i = LeabraLayerSpec::KWTA_INHIB; lvesp->i_kwta_pt = 0.25f;

    pvesp->kwta.k = 3; 	pvesp->gp_kwta.k = 3; 
    pvisp->kwta.k = 3; 	pvisp->gp_kwta.k = 3; 
    lvesp->kwta.k = 3; 	lvesp->gp_kwta.k = 3; 

    pvesp->unit_range.min = -0.50f;  pvesp->unit_range.max = 1.5f;
    pvesp->unit_range.UpdateAfterEdit();

    pvisp->unit_range.min = -0.5f;  pvisp->unit_range.max = 1.5f;
    pvisp->unit_range.UpdateAfterEdit();

    lvesp->unit_range.min = -0.5f;  lvesp->unit_range.max = 1.5f;
    lvesp->unit_range.UpdateAfterEdit();

    pv_units->SetUnique("act", true);
    pv_units->SetUnique("act_fun", true);
    pv_units->SetUnique("dt", true);
    pv_units->act_fun = LeabraUnitSpec::NOISY_XX1;
    pv_units->act.thr = .25f;
    pv_units->act.gain = 600.0f;
    pv_units->act.nvar = .005f;
    pv_units->g_bar.l = .1f;
    pv_units->g_bar.h = .015f;  pv_units->g_bar.a = .045f;
    pv_units->dt.vm = .2f;

    pvi_cons->SetUnique("lmix", true);
    pvi_cons->lmix.err_sb = true;
    pvi_cons->SetUnique("rnd", true);
    pvi_cons->rnd.mean = 0.1f;
    pvi_cons->rnd.var = 0.0f;
  }

  pvi_cons->lrate = .01f;
  if(!old_syn_dep)
    pvr_cons->lrate = .02f;
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
  if(!old_syn_dep) {
    if(pvr->un_geom.n != n_lv_u) { pvr->un_geom.n = n_lv_u; pvr->un_geom.x = n_lv_u; pvr->un_geom.y = 1; }
  }
  vta->un_geom.n = 1;
  rew_targ_lay->un_geom.n = 1;
  rew_targ_lay->layer_type = Layer::INPUT;

  if(lve_new) {
    if(localist_val) {
      pve->pos.z = 0; pve->pos.y = 0; pve->pos.x = 0;
      pvi->pos.z = 0; pvi->pos.y = 2; pvi->pos.x = 0;
      if(!old_syn_dep) {
	pvr->pos.z = 0; pvr->pos.y = 4; pvr->pos.x = 0;
      }
      rew_targ_lay->pos.z = 0; rew_targ_lay->pos.y = 6; rew_targ_lay->pos.x = 0;

      vta->pos.z = 0; vta->pos.y = 4; vta->pos.x = 6;

      lve->pos.z = 0; lve->pos.y = 0; lve->pos.x = 9;
      lvi->pos.z = 0; lvi->pos.y = 2; lvi->pos.x = 9;
    }
    else {
      rew_targ_lay->pos.z = 0; rew_targ_lay->pos.y = 6; rew_targ_lay->pos.x = 0;
      pve->pos.z = 0; pve->pos.y = 6; pve->pos.x = 0;
      vta->pos.z = 0; vta->pos.y = 1; vta->pos.x = n_lv_u + 2;
      pvi->pos.z = 0; pvi->pos.y = 4; pvi->pos.x = 0;
      lvi->pos.z = 0; lvi->pos.y = 2; lvi->pos.x = 0;
      lve->pos.z = 0; lve->pos.y = 0; lve->pos.x = 0;
      if(!old_syn_dep) {
	pvr->pos.z = 0; pvr->pos.y = 8; pvr->pos.x = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  pve->SetLayerSpec(pvesp);	pve->SetUnitSpec(pv_units);
  pvi->SetLayerSpec(pvisp);	pvi->SetUnitSpec(pv_units);
  lve->SetLayerSpec(lvesp);	lve->SetUnitSpec(lv_units);
  lvi->SetLayerSpec(lvisp);	lvi->SetUnitSpec(lv_units);
  vta->SetLayerSpec(dasp);	vta->SetUnitSpec(da_units);
  if(!old_syn_dep) {
    pvr->SetLayerSpec(pvrsp);	pvr->SetUnitSpec(pv_units);
  }

  pv_units->bias_spec.SetSpec(bg_bias);
  lv_units->bias_spec.SetSpec(bg_bias);
  da_units->bias_spec.SetSpec(fixed_bias);
  
  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,
  net->FindMakePrjn(pve, rew_targ_lay, onetoone, marker_cons);
  net->FindMakePrjn(pvi, pve, onetoone, marker_cons);
  net->FindMakePrjn(lve, pvi, onetoone, marker_cons);
  net->FindMakePrjn(lvi, pvi, onetoone, marker_cons);

  net->FindMakePrjn(vta, pvi, onetoone, marker_cons);
  net->FindMakePrjn(vta, lve, onetoone, marker_cons);
  net->FindMakePrjn(vta, lvi, onetoone, marker_cons);

  net->FindMakePrjn(pvi,  vta, onetoone, marker_cons);
  net->FindMakePrjn(lve,  vta, onetoone, marker_cons);
  net->FindMakePrjn(lvi,  vta, onetoone, marker_cons);

  if(!old_syn_dep) {
    net->FindMakePrjn(pvr, pve, onetoone, marker_cons);
    net->FindMakePrjn(lve, pvr, onetoone, marker_cons);
    net->FindMakePrjn(lvi, pvr, onetoone, marker_cons);
    net->FindMakePrjn(vta, pvr, onetoone, marker_cons);
    net->FindMakePrjn(pvr,  vta, onetoone, marker_cons);
  }


  if(lve_new || fm_hid_cons || fm_out_cons) {
    for(i=0;i<input_lays.size;i++) {
      Layer* il = (Layer*)input_lays[i];
      net->FindMakePrjn(pvi, il, fullprjn, pvi_cons);
      net->FindMakePrjn(lve, il, fullprjn, lve_cons);
      net->FindMakePrjn(lvi, il, fullprjn, lvi_cons);
      if(!old_syn_dep) {
	net->FindMakePrjn(pvr, il, fullprjn, pvr_cons);
      }
    }
    if(fm_hid_cons) {
      for(i=0;i<hidden_lays.size;i++) {
	Layer* hl = (Layer*)hidden_lays[i];
	net->FindMakePrjn(pvi, hl, fullprjn, pvi_cons);
	net->FindMakePrjn(lve, hl, fullprjn, lve_cons);
	net->FindMakePrjn(lvi, hl, fullprjn, lvi_cons);
	if(!old_syn_dep) {
	  net->FindMakePrjn(pvr, hl, fullprjn, pvr_cons);
	}
      }
    }
    if(fm_out_cons) {
      for(i=0;i<output_lays.size;i++) {
	Layer* ol = (Layer*)output_lays[i];
	net->FindMakePrjn(pvi, ol, fullprjn, pvi_cons);
	net->FindMakePrjn(lve, ol, fullprjn, lve_cons);
	net->FindMakePrjn(lvi, ol, fullprjn, lvi_cons);
	if(!old_syn_dep) {
	  net->FindMakePrjn(pvr, ol, fullprjn, pvr_cons);
	}
      }
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

  bool ok = pvisp->CheckConfig_Layer(pvi, true) && lvesp->CheckConfig_Layer(lve, true)
    && lvisp->CheckConfig_Layer(lve, true)
    && dasp->CheckConfig_Layer(vta, true) && pvesp->CheckConfig_Layer(pve, true);

  if(!old_syn_dep && ok) {
    ok = pvrsp->CheckConfig_Layer(pvr, true);
  }

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

  if(!old_syn_dep)
    pvisp->UpdateAfterEdit();
  
  for(j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("PVLV");
  if(edit != NULL) {
    if(!old_syn_dep)
      pvr_cons->SelectForEditNm("lrate", edit, "pvr");
    pvi_cons->SelectForEditNm("lrate", edit, "pvi");
    lve_cons->SelectForEditNm("lrate", edit, "lve");
    lvi_cons->SelectForEditNm("lrate", edit, "lvi");
    //    lve_cons->SelectForEditNm("syn_dep", edit, "lve");
    pvesp->SelectForEditNm("rew", edit, "pve");
//     lvesp->SelectForEditNm("lv", edit, "lve");
    if(!old_syn_dep)
      pvrsp->SelectForEditNm("pv_detect", edit, "pvr");
    else
      pvisp->SelectForEditNm("pv_detect", edit, "pvi");
//     pvisp->SelectForEditNm("scalar", edit, "pvi");
//     lvesp->SelectForEditNm("scalar", edit, "lve");
//     pvisp->SelectForEditNm("bias_val", edit, "pvi");
//     lvesp->SelectForEditNm("bias_val", edit, "lve");
//    dasp->SelectForEditNm("avg_da", edit, "vta");
    dasp->SelectForEditNm("da", edit, "vta");
  }
}

