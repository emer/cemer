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

#include "leabra_pbwm.h"

#include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

////////////////////////////////////////////////////////////////////
//	Patch/Striosomes and SNc

void PatchLayerSpec::Initialize() {
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  learn_mnt_only = false;
}

void PatchLayerSpec::Send_LVeToMatrix(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     const float snd_val = u->act_eq;
     for(int g=0; g<u->send.size; g++) {
       LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
       LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
       if(tol->lesioned())	continue;
       LeabraLayerSpec* tols = (LeabraLayerSpec*)tol->GetLayerSpec();
       if(tols->InheritsFrom(&TA_MatrixBaseLayerSpec)) {
	 for(int j=0;j<send_gp->size; j++) {
	   ((LeabraUnit*)send_gp->Un(j))->misc_1 = snd_val;
	 }
       }
     }
     );
}

void PatchLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  ScalarValLayerSpec::Compute_CycleStats(lay, net);
  // do NOT report lvi value!
  Send_LVeToMatrix(lay, net);
}


void PatchLayerSpec::Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  if(!learn_mnt_only) {
    inherited::Compute_LVPlusPhaseDwt(lay, net);
    return;
  }
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  if(!er_avail) return;					  // just let it ride..

  float pve_val = net->norew_val;
  if(net->ext_rew_avail) {
    pve_val = net->ext_rew;
  }

  for(int gi=0; gi < lay->units.gp.size; gi++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[gi];
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
    if(ugp->misc_state > 0) {	// stripe was maintaining at mid minus gating point
       u->ext = pve_val;
       ClampValue_ugp(ugp, net); 		// apply new value
       Compute_ExtToPlus_ugp(ugp, net); 	// copy ext values to act_p
    }
  }
}

//////////////////////////////////////////////////////
//		SNcLayerSpec

void SNcMiscSpec::Initialize() {
  stripe_lv_pct = 0.5f;
  global_lv_pct = 0.5f;
  lv_mnt_pv_out = true;
}

void SNcMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  global_lv_pct = 1.0f - stripe_lv_pct;
}

void SNcLayerSpec::Initialize() {
}

void SNcLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  snc.UpdateAfterEdit_NoGui();
}

void SNcLayerSpec::HelpConfig() {
  String help = "SNcLayerSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from PVLV layers.\n\
 - No Learning\n\
 \nSNcLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons marked with a MarkerConSpec from PVLV LVe, LVi, PVi, PVe, PVr, NV (all!)\n\
 - Recv cons marked with a MarkerConSpec from Patch layer, same number of units\n\
 - Send cons marked with a MarkerConSpec to Matrix layers\n\
 - This layer must be after recv layers in list of layers\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool SNcLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;
  if(lay->CheckError(lay->units.gp.size == 0, quiet, rval,
		     "requires unit groups, one per associated stripe")) {
    return false;
  }

  int patch_prjn_idx;
  LeabraLayer* patch_lay = FindLayerFmSpec(lay, patch_prjn_idx, &TA_PatchLayerSpec);
  if(lay->CheckError(!patch_lay, quiet, rval,
		"did not find Patch layer to get Da from!")) {
    return false;
  }

  if(lay->CheckError(lay->units.gp.size != patch_lay->units.gp.size, quiet, rval,
		     "our number of unit groups should equal those in the patch layer")) {
    return false;
  }

  return true;
}  

void SNcLayerSpec::Compute_Da(LeabraLayer* lay, LeabraNetwork* net) {
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

  int patch_prjn_idx;
  LeabraLayer* patch_lay = FindLayerFmSpec(lay, patch_prjn_idx, &TA_PatchLayerSpec);
  PatchLayerSpec* patch_sp = (PatchLayerSpec*)patch_lay->GetLayerSpec();

  // this is the global da from LVe, not stripe-specific
  float lv_da = lve_sp->Compute_LVDa(lve_lay, lvi_lay, net);

  // nv only contributes to lv, not pv..
  if(nv_lay) {
    lv_da += nvls->Compute_NVDa(nv_lay, net);
  }
  float pv_da = da.pv_gain * pvils->Compute_PVDa(pvi_lay, net);
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good

  Unit_Group* lvi_ugp;
  if(lvi_lay->units.gp.size > 0)
    lvi_ugp = (Unit_Group*)lvi_lay->units.gp[0];
  else
    lvi_ugp = (Unit_Group*)&(lvi_lay->units);
  // assuming only one lvi group of units for time being

  lay->dav = 0.0f;
  for(int gi=0; gi < lay->units.gp.size; gi++) {
    Unit_Group* snc_ugp = (Unit_Group*)lay->units.gp[gi];
    Unit_Group* patch_ugp = (Unit_Group*)patch_lay->units.gp[gi];
    LeabraUnit* snc_u = (LeabraUnit*)snc_ugp->FastEl(0);
//    LeabraUnit* patch_u = (LeabraUnit*)patch_ugp->FastEl(0);

    float str_da = patch_sp->Compute_LVDa_ugp(patch_ugp, lvi_ugp, net); // per stripe
    float lv_da_str = snc.global_lv_pct * lv_da + snc.stripe_lv_pct * str_da;

    if(er_avail) {
      snc_u->dav = pv_da;
      if(da.add_pv_lv) {
	snc_u->dav += lv_da_str;
      }
    }
    else {
      snc_u->dav = lv_da_str;
    }
    snc_u->ext = da.tonic_da + da.da_gain * snc_u->dav;
    snc_u->act_eq = snc_u->act_nd = snc_u->act = snc_u->net = snc_u->ext;
    lay->dav += snc_u->dav;

    // now send, with target specificity
    for(int g=0; g<snc_u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)snc_u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())	continue;
      LeabraLayerSpec* ls = (LeabraLayerSpec*)tol->spec.SPtr();
      float send_val = snc_u->act;
      if(snc.lv_mnt_pv_out && //ls->InheritsFrom(&TA_MatrixBaseLayerSpec)) {
	 ls->InheritsFrom(&TA_MatrixLayerSpec)) {
	if(((MatrixLayerSpec*)ls)->bg_type == MatrixLayerSpec::OUTPUT)
	  send_val = pv_da * da.da_gain; // send PV to output
	else
	  send_val = lv_da_str * da.da_gain; // and LV to maint
	for(int j=0;j<send_gp->size; j++) {
	  ((LeabraUnit*)send_gp->Un(j))->dav = send_val;
	}
      }
      else {
	for(int j=0;j<send_gp->size; j++) {
	  ((LeabraUnit*)send_gp->Un(j))->dav = send_val;
	}
      }
    }
  }
  lay->dav /= (float)lay->units.gp.size; // integrated average -- not really used
}

void SNcLayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork*) {
  // do nothing -- was sent in more targeted fashion in compute_da
}

//////////////////////////////////
//	SNrThal Layer Spec	//
//////////////////////////////////

void SNrThalMiscSpec::Initialize() {
  go_thr = 0.5f;
  net_off = 0.0f;
  rnd_go_inc = 0.1f;
  leak = 1.0f;
}

void SNrThalLayerSpec::Initialize() {
  bg_type = MAINT;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_PCT;
  kwta.pct = .75f;
  SetUnique("tie_brk", true);	// turn on tie breaking by default
  tie_brk.on = true;
  tie_brk.thr_gain = 0.2f;
//   tie_brk.loser_gain = 0.0f;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .8f;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void SNrThalLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void SNrThalLayerSpec::HelpConfig() {
  String help = "SNrThalLayerSpec Computation:\n\
 - act of unit(s) = act_dif of unit(s) in reward integration layer we recv from\n\
 - da is dynamically computed in plus phaes and sent all layers that recv from us\n\
 - No Learning\n\
 \nSNrThalLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Single recv connection marked with a MarkerConSpec from reward integration layer\
     (computes expectations and actual reward signals)\n\
 - This layer must be after corresp. reward integration layer in list of layers\n\
     (da signal from this layer put directly into da var on units)\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool SNrThalLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  // must have the appropriate ranges for unit specs..
  //  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  // check recv connection
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_MatrixBaseLayerSpec);

  if(lay->CheckError(matrix_lay == NULL, quiet, rval,
		"did not find Matrix layer to recv from!")) {
    return false;
  }

  if(lay->CheckError(matrix_lay->units.gp.size != lay->units.gp.size, quiet, rval,
		"MatrixLayer unit groups must = SNrThalLayer unit groups!")) {
    lay->unit_groups = true;
    lay->gp_geom.n = matrix_lay->units.gp.size;
    return false;
  }

  // grab bg_type from matrix guy
  MatrixBaseLayerSpec* mls = (MatrixBaseLayerSpec*)matrix_lay->spec.SPtr();
  if(mls->InheritsFrom(&TA_MatrixLayerSpec)) {
    SetUnique("bg_type", true);
    bg_type = (BGType)((MatrixLayerSpec*)mls)->bg_type;
  }

  return true;
}

void SNrThalLayerSpec::Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net) {
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_MatrixBaseLayerSpec);

  float net_off_rescale = 1.0f / (1.0f + snrthal.net_off);

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    float gonogo = 0.0f;
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    LeabraUnitSpec* us = (LeabraUnitSpec*)matrix_lay->unit_spec.SPtr();
    if((mugp->size > 0) && (mugp->acts.max >= us->opt_thresh.send)) {
      float sum_go = 0.0f;
      float sum_nogo = 0.0f;
      int gp_sz = mugp->size / 2;
      for(int i=0;i<mugp->size;i++) {
	LeabraUnit* u = (LeabraUnit*)mugp->FastEl(i);
	PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(i / gp_sz); // GO = 0, NOGO = 1
	if(go_no == PFCGateSpec::GATE_GO)
	  sum_go += u->act_eq;
	else
	  sum_nogo += u->act_eq;
      }
      float norm_factor = sum_go + sum_nogo + snrthal.leak;
      if(norm_factor > 0.0f) {
	gonogo = (sum_go - sum_nogo) / norm_factor;
      }
      if(mugp->misc_state1 >= PFCGateSpec::NOGO_RND_GO) {
	gonogo += snrthal.rnd_go_inc;
	if(gonogo > 1.0f) gonogo = 1.0f;
      }
    }

    float net_eff = net_off_rescale * (gonogo + snrthal.net_off);

    for(int i=0;i<rugp->size;i++) {
      LeabraUnit* ru = (LeabraUnit*)rugp->FastEl(i);
      ru->net = net_eff;
      ru->i_thr = ru->Compute_IThresh(net);
    }
  }
}

void SNrThalLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  // note: this no longer has dt.net in effect here!! hopefully not a huge diff..
  Compute_GoNogoNet(lay, net);
  inherited::Compute_NetinStats(lay, net);
}

void SNrThalLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // nop -- don't do the default thing -- already done by call to MidMinusAct
}

void SNrThalLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay, LeabraUnit_Group* ugp, 
					   int gp_idx, LeabraNetwork* net) {
  // capture at snrthal level
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ugp->, i) {
    u->act_m2 = u->act_eq;
  }

  // then at matrix level
  int dum_prjn_idx = 0;
  LeabraLayer* matrix = FindLayerFmSpec(lay, dum_prjn_idx, &TA_MatrixLayerSpec);
  MatrixLayerSpec* mls = (MatrixLayerSpec*)matrix->spec.SPtr();
  LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix->units.gp[gp_idx];
  mls->Compute_MidMinusAct_ugp(matrix, mugp, net); // tell matrix to do it
}


void SNrThalLayerSpec::SendGateStates(LeabraLayer* lay, LeabraNetwork* net,
				      LeabraLayer* pfc_lay) {
  int dum_prjn_idx = 0;
  LeabraLayer* matrix = FindLayerFmSpec(lay, dum_prjn_idx, &TA_MatrixLayerSpec);
  LeabraLayer* patch = FindLayerFmSpec(matrix, dum_prjn_idx, &TA_PatchLayerSpec);
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* pfcugp = (LeabraUnit_Group*)pfc_lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix->units.gp[mg];
    // everybody gets gate state info from PFC!
    snrgp->misc_state = mugp->misc_state = pfcugp->misc_state;
    snrgp->misc_state1 = pfcugp->misc_state1; 
    if(mugp->misc_state1 < PFCGateSpec::NOGO_RND_GO) { // don't override random go signals
      mugp->misc_state1 = pfcugp->misc_state1;
    }
    snrgp->misc_state2 = mugp->misc_state2 = pfcugp->misc_state2;
    // matrix does not get misc_float -- uses it otherwise
    snrgp->misc_float = pfcugp->misc_float;
    snrgp->misc_float1 = pfcugp->misc_float1;

    if(patch) {
      LeabraUnit_Group* patchgp = (LeabraUnit_Group*)patch->units.gp[mg];
      // everybody gets gate state info from PFC!
      patchgp->misc_state = snrgp->misc_state;
      patchgp->misc_state1 = snrgp->misc_state1;
      patchgp->misc_state2 = snrgp->misc_state2;
      patchgp->misc_float = snrgp->misc_float;
      patchgp->misc_float1 = snrgp->misc_float1;
    }
  }
}


//////////////////////////////////////////
//	Base Specs
//////////////////////////////////////////

void MatrixBaseLayerSpec::Initialize() {

}

void PFCBaseLayerSpec::Initialize() {

}

//////////////////////////////////
//	MatrixConSpec		//
//////////////////////////////////

void MatrixConSpec::Initialize() {
  min_obj_type = &TA_MatrixCon;

  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
  wt_sig.off = 1.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;
}

void MatrixConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  //  lmix.err_sb = false;
}

//////////////////////////////////////////
// 	Matrix Unit Spec		//
//////////////////////////////////////////

void MatrixBiasSpec::Initialize() {
  SetUnique("rnd", true);
  SetUnique("wt_limits", true);
  SetUnique("wt_scale", true);
  SetUnique("wt_scale_init", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;
  wt_limits.min = -1.0f;
  wt_limits.max = 5.0f;
  wt_limits.sym = false;
  wt_limits.type = WeightLimits::NONE;
  dwt_thresh = .1f;

  SetUnique("lrate", true);
  lrate = 0.0f;			// default is no lrate
}

void MatrixNoiseSpec::Initialize() {
  patch_noise = true;
}

void MatrixUnitSpec::Initialize() {
  SetUnique("bias_spec", true);
  bias_spec.SetBaseType(&TA_LeabraConSpec); // can't be bias spec b/c matrix derives from con
  bias_spec.type = &TA_MatrixBiasSpec;
  SetUnique("g_bar", true);
  g_bar.a = .03f;
  g_bar.h = .01f;

  SetUnique("act", true);
  act.i_thr = ActFunSpec::NO_AH; // key for dopamine effects

  SetUnique("noise_type", true);
  noise_type = NETIN_NOISE;
  SetUnique("noise", true);
  noise.var = 0.0002f;
  SetUnique("noise_adapt", true);
  noise_adapt.trial_fixed = true;
  noise_adapt.k_pos_noise = true;
  noise_adapt.mode = NoiseAdaptSpec::PVLV_LVE;
}

void MatrixUnitSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void MatrixUnitSpec::InitLinks() {
  inherited::InitLinks();
  bias_spec.type = &TA_MatrixBiasSpec;
}

void MatrixUnitSpec::Compute_MidMinusAct(LeabraUnit* u, LeabraNetwork* net) {
  u->act_m2 = u->act_eq;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->InheritsFrom(&TA_MatrixConSpec)) {
      ((MatrixConSpec*)cs)->Compute_MidMinusAct(recv_gp, u);
    }
  }
}

float MatrixUnitSpec::Compute_Noise(LeabraUnit* u, LeabraNetwork* net) {
  float noise_amp = 1.0f;		// noise amplitude multiplier
  LeabraUnit_Group* mugp = u->own_ugp();
  if(matrix_noise.patch_noise) {
    noise_amp = (1.0f - (noise_adapt.min_pct_c * u->misc_1)); // lve value on patch is in misc_1
  }
  else {
    if(noise_adapt.mode == NoiseAdaptSpec::SCHED_CYCLES) {
      noise_amp = noise_sched.GetVal(net->cycle);
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::SCHED_EPOCHS) {
      noise_amp = noise_sched.GetVal(net->epoch);
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_PVI) {
      noise_amp = (1.0f - (noise_adapt.min_pct_c * net->pvlv_pvi));
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_LVE) {
      noise_amp = (1.0f - (noise_adapt.min_pct_c * net->pvlv_lve));
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_MIN) {
      float pvlv_val = MIN(net->pvlv_pvi, net->pvlv_lve);
      noise_amp = (1.0f - (noise_adapt.min_pct_c * pvlv_val));
    }
  }

  float rval = 0.0f;
  if(noise_adapt.trial_fixed) {
    rval = u->noise; // u->noise is trial-level generated value
  }
  else {
    rval = noise.Gen();
    u->noise = rval;
  }

  return noise_amp * rval;
}


//////////////////////////////////
//	Matrix Layer Spec	//
//////////////////////////////////

void MatrixGateBiasSpec::Initialize() {
  mnt_mnt_nogo = 1.0f;
  mnt_rew_nogo = 2.0f;
  mnt_empty_go = 0.0f;
  out_rew_go = 1.0f;
  out_norew_nogo = 2.0f;
  out_empty_nogo = 2.0f;
  cur_trl_mnt = false;
}

void MatrixGateBiasSpec::SetAllBiases(float one_bias, float strong_mult) {
  mnt_mnt_nogo = one_bias;
  mnt_rew_nogo = strong_mult * one_bias;
  //  mnt_empty_go = 0.0f;
  out_rew_go = one_bias;
  out_norew_nogo = strong_mult * one_bias;
  out_empty_nogo = strong_mult * one_bias;
}

void MatrixMiscSpec::Initialize() {
  da_gain = 0.1f;
}

void MatrixRndGoSpec::Initialize() {
  nogo_thr = 30;
  nogo_rng = 30;
  nogo_da = 10.0f;
  nogo_noise = 0.02f;
  sub_norm = false;
}

void MatrixGoNogoGainSpec::Initialize() {
  on = false;
  go_p = go_n = nogo_p = nogo_n = 1.0f;
}

/////////////////////////////////////////////////////

void MatrixLayerSpec::Initialize() {
  //  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  //  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .25f;
  //  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .25f;

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  bg_type = MAINT;
}

void MatrixLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate_bias.UpdateAfterEdit_NoGui();
}

void MatrixLayerSpec::Defaults() {
  inherited::Defaults();
  matrix.Defaults();
  gate_bias.Defaults();
  go_nogo_gain.Defaults();
  rnd_go.Defaults();
  Initialize();
}

void MatrixLayerSpec::HelpConfig() {
  String help = "MatrixLayerSpec Computation:\n\
 There are 2 types of units arranged sequentially in the following order within each\
 stripe whose firing affects the gating status of the corresponding stripe in PFC:\n\
 - GO unit = toggle maintenance of units in PFC: this is the direct pathway\n\
 - NOGO unit = maintain existing state in PFC (i.e. do nothing): this is the indirect pathway\n\
 \nMatrixLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Units must have a MatrixUnitSpec and must recv from PVLVDaLayerSpec layer\
 (calld DA typically) to get da modulation for learning signal\n\
 - Recv connections need to be MatrixConSpec as learning occurs based on the da-signal\
 on the matrix units.\n\
 - This layer must be after DaLayers in list of layers\n\
 - Units must be organized into groups (stipes) of same number as PFC";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool MatrixLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space!")) {
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(!us->InheritsFrom(TA_MatrixUnitSpec), quiet, rval,
		"UnitSpec must be MatrixUnitSpec!")) {
    return false;
  }

  us->SetUnique("g_bar", true);

  // must have these not initialized every trial!
  if(lay->CheckError(us->hyst.init, quiet, rval,
		"requires UnitSpec hyst.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
  }
  if(lay->CheckError(us->acc.init, quiet, rval,
		"requires UnitSpec acc.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("acc", true);
    us->acc.init = false;
  }
  us->UpdateAfterEdit();

  LeabraBiasSpec* bs = (LeabraBiasSpec*)us->bias_spec.SPtr();
  if(lay->CheckError(bs == NULL, quiet, rval,
		"Error: null bias spec in unit spec", us->name)) {
    return false;
  }

  LeabraLayer* da_lay = NULL;
  LeabraLayer* snr_lay = NULL;
  LeabraLayer* patch_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) // self projection, skip it
      continue;
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(fmlay->spec.SPtr()->InheritsFrom(TA_PVLVDaLayerSpec)) da_lay = fmlay;
      if(fmlay->spec.SPtr()->InheritsFrom(TA_SNrThalLayerSpec)) snr_lay = fmlay;
      if(fmlay->spec.SPtr()->InheritsFrom(TA_PatchLayerSpec)) patch_lay = fmlay;
      continue;
    }
    MatrixConSpec* cs = (MatrixConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_MatrixConSpec), quiet, rval,
		  "Receiving connections must be of type MatrixConSpec!")) {
      return false;
    }
    if(lay->CheckError(cs->wt_limits.sym != false, quiet, rval,
		  "requires recv connections to have wt_limits.sym=false, I just set it for you in spec:",
		  cs->name,"(make sure this is appropriate for all layers that use this spec!)")) {
      cs->SetUnique("wt_limits", true);
      cs->wt_limits.sym = false;
    }
  }
  if(lay->CheckError(da_lay == NULL, quiet, rval,
		"Could not find DA layer (PVLVDaLayerSpec) -- must receive MarkerConSpec projection from one!")) {
    return false;
  }
  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int daidx = lay->own_net->layers.FindLeafEl(da_lay);
  lay->CheckError(daidx > myidx, quiet, rval,
		  "DA layer (PVLVDaLayerspec) layer must be *before* this layer in list of layers -- it is now after, won't work!");

  if(lay->CheckError(snr_lay == NULL, quiet, rval,
		"Could not find SNrThal layer -- must receive MarkerConSpec projection from one!")) {
    return false;
  }
  if(lay->CheckError(bg_type == MAINT && patch_lay == NULL, quiet, rval,
		"Could not find Patch layer -- MAINT Matrix must receive MarkerConSpec projection from one!")) {
    return false;
  }
  return true;
}

void MatrixLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  LabelUnits(lay);
}

void MatrixLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // nop -- don't do the default thing -- already done by call to MidMinusAct
}

void MatrixLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay, LeabraUnit_Group* mugp, 
					      LeabraNetwork* net) {
  MatrixUnitSpec* us = (MatrixUnitSpec*)lay->unit_spec.SPtr();
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    us->Compute_MidMinusAct(u, net);
  }
}

void MatrixLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_ApplyInhib(lay, net);
  
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    Compute_BiasDaMod(lay, mugp, net); // always just compute this one
  }
}

void MatrixLayerSpec::Compute_BiasDaMod(LeabraLayer* lay, LeabraUnit_Group* mugp, 
					LeabraNetwork* net) {
  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_PVLVDaLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.SPtr();
  float tonic_da = dals->da.tonic_da;
  int pfc_mnt_cnt = mugp->misc_state; // is pfc maintaining or not?
  bool pfc_is_mnt = pfc_mnt_cnt > 0;
  if(gate_bias.cur_trl_mnt) {
    if(mugp->misc_state2 == PFCGateSpec::GATE_MNT_GO ||
       mugp->misc_state2 == PFCGateSpec::GATE_OUT_MNT_GO)
      pfc_is_mnt = true;	// count current trial as mnt!
  }
  int rnd_go_thr = mugp->misc_state3; // random go threshold for this time
  int gp_sz = mugp->leaves / 2;
  bool nogo_rnd_go = (mugp->misc_state1 == PFCGateSpec::NOGO_RND_GO);

  float bias_dav = 0.0f;

  if(bg_type == OUTPUT) {	// output gating guy
    if(net->pv_detected) {	// PV reward trial -- bias output gating
      // only if pfc is maintaining..
      if(pfc_is_mnt) {
	bias_dav = gate_bias.out_rew_go;
	if(!nogo_rnd_go && pfc_mnt_cnt > rnd_go_thr) { // no rnd go yet, but over thresh
	  mugp->misc_state1 = PFCGateSpec::NOGO_RND_GO;
	  Compute_RndGoNoise_ugp(lay, mugp, net);
	}
      }
      else {
	bias_dav = -gate_bias.out_empty_nogo;
      }
    }
    else {			// not a PV trial
      bias_dav = -gate_bias.out_norew_nogo; // blanket no output gating thing
    }
  }
  else {			// MAINT
    if(net->pv_detected) {	// PV reward trial -- no maint gating
      bias_dav = -gate_bias.mnt_rew_nogo;
    }
    else {
      if(pfc_mnt_cnt > 0) {   	// currently maintaining: bias NoGo for everything
	bias_dav = -gate_bias.mnt_mnt_nogo;
      }
      else {			// otherwise, bias to maintain/update
	bias_dav = gate_bias.mnt_empty_go;
	if(!nogo_rnd_go && pfc_mnt_cnt < -rnd_go_thr) { // no rnd go yet, but over thresh
	  mugp->misc_state1 = PFCGateSpec::NOGO_RND_GO;
	  Compute_RndGoNoise_ugp(lay, mugp, net);
	}
      }
    }
  }

  float tot_dav = bias_dav + tonic_da;

  int idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(idx / gp_sz);
    Compute_UnitBiasDaMod(u, tot_dav, go_no);
    idx++;
  }
}

void MatrixLayerSpec::Compute_UnitBiasDaMod(LeabraUnit* u, float bias_dav, int go_no) {
  if(go_no == (int)PFCGateSpec::GATE_NOGO) {
    if(bias_dav >= 0.0f) {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = bias_dav;
      if(go_nogo_gain.on) u->vcb.g_a *= go_nogo_gain.nogo_p;
    }
    else {
      u->vcb.g_h = -bias_dav;
      if(go_nogo_gain.on) u->vcb.g_h *= go_nogo_gain.nogo_n;
      u->vcb.g_a = 0.0f;
    }
  }
  else {			// must be a GO
    if(bias_dav >= 0.0f)  { 
      u->vcb.g_h = bias_dav;
      if(go_nogo_gain.on) u->vcb.g_h *= go_nogo_gain.go_p;
      u->vcb.g_a = 0.0f;
    }
    else {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = -bias_dav;
      if(go_nogo_gain.on) u->vcb.g_a *= go_nogo_gain.go_n;
    }
  }
}

void MatrixLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == 1) {
    // end of plus -- compute da value used for learning
    Compute_LearnDaVal(lay, net);
  }
}

// this is called at end of plus phase, to establish a da value for driving learning
// NOTE: misc_state reflects count at time of gating -- not updated at Compute_Gate_Final

void MatrixLayerSpec::Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net) {
  int n_rnd_go = 0;		// find out if anyone has a rnd go
  float nogo_da_sub = 0.0f;
  if(rnd_go.sub_norm) {
    for(int gi=0; gi<lay->units.gp.size; gi++) {
      LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
      if(mugp->misc_state1 == PFCGateSpec::NOGO_RND_GO) n_rnd_go++;
    }
    if(n_rnd_go > 0) {
      nogo_da_sub = rnd_go.nogo_da / (float)(lay->units.gp.size - n_rnd_go);
      // how much to subtract from other units if one guy gets a random go
    }
  }
    
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    int snr_prjn_idx = 0;
    LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);

    LeabraUnit_Group* snrug = (LeabraUnit_Group*)snr_lay->units.gp[gi];
    LeabraUnit* snr_u = (LeabraUnit*)snrug->Leaf(0);

    PFCGateSpec::GateSignal gate_sig = (PFCGateSpec::GateSignal)mugp->misc_state2;
    int pfc_mnt_cnt = mugp->misc_state; // is pfc maintaining or not?
    bool nogo_rnd_go = (mugp->misc_state1 == PFCGateSpec::NOGO_RND_GO);
    int gp_sz = mugp->leaves / 2;

    float snrthal_act = snr_u->act_m2;
    if(bg_type == OUTPUT) {
      if((gate_sig == PFCGateSpec::GATE_NOGO) || (gate_sig == PFCGateSpec::GATE_MNT_GO))
	snrthal_act = 0.0f;	// if no OUT_GO, nothing for us..
    }
    else {			// MAINT
      if((gate_sig == PFCGateSpec::GATE_NOGO) || (gate_sig == PFCGateSpec::GATE_OUT_GO))
	snrthal_act = 0.0f;	// if no MNT_GO, nothing for us..
    }

    int idx = 0;
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
      PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(idx / gp_sz);

      // critical gating activation value is mid-minus state
      float act_val = u->act_m2;
      float lrn_dav = snrthal_act * u->dav; // dav is current plus phase

      if(nogo_rnd_go) {
	lrn_dav += rnd_go.nogo_da; // output gating also gets this too
      }
      else {
	lrn_dav -= nogo_da_sub;	// subtract from others -- keep it normalized -- val is 0 if none
      }

      lrn_dav *= matrix.da_gain;

      if(go_no == PFCGateSpec::GATE_NOGO)
	lrn_dav *= -1.0f;	// flip the sign for nogo!

      u->dav = lrn_dav;		// re-store back to da value, which is used in conspec lrule
      idx++;
    }
  }
}

void MatrixLayerSpec::Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    if((mugp->misc_state1 == PFCGateSpec::NOGO_RND_GO) || mugp->misc_state3 == 0) {
      mugp->misc_state1 = PFCGateSpec::INIT_STATE;
      // new treshold for when to fire rnd go next!
      mugp->misc_state3 = rnd_go.nogo_thr + Random::IntZeroN(rnd_go.nogo_rng);
    }
  }
}

void MatrixLayerSpec::Compute_RndGoNoise_ugp(LeabraLayer* lay, LeabraUnit_Group* mugp, 
					     LeabraNetwork* net) {
  if(rnd_go.nogo_noise == 0.0f) return;

  int n_go_units = mugp->leaves / 2;
  lay->unit_idxs.SetSize(n_go_units); // just do go guys
  lay->unit_idxs.FillSeq();
  lay->unit_idxs.Permute();
  int i;
  for(i=0; i<n_go_units; i++) {
    LeabraUnit* u = (LeabraUnit*)mugp->FastEl(lay->unit_idxs[i]);
    if(i < mugp->kwta.k)
      u->noise = rnd_go.nogo_noise;
    else
      u->noise = 0.0f;
  }
  for(; i<mugp->leaves; i++) {
    LeabraUnit* u = (LeabraUnit*)mugp->FastEl(i);
    u->noise = 0.0f;
  }
}

// void MatrixLayerSpec::Compute_NoGoRndGo(LeabraLayer* lay, LeabraNetwork* net) {
//   for(int gi=0; gi<lay->units.gp.size; gi++) {
//     LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];

//     int pfc_mnt_cnt = mugp->misc_state; // is pfc maintaining or not?

//     if((int)fabs((float)mugp->misc_state) > rnd_go.nogo_thr) {
//       if(rnd_go.sep_out_mnt) {
// 	if(bg_type == OUTPUT && pfc_mnt_cnt < 0) // no output when empty
//           continue;
//         else if(bg_type == MAINT && pfc_mnt_cnt > 0) // no maint when full
// 	  continue;
//       }

//       if(Random::ZeroOne() < rnd_go.nogo_p) {
// 	mugp->misc_state1 = PFCGateSpec::NOGO_RND_GO;
// 	Compute_RndGoNoise_ugp(lay, mugp, net);
//       }
//     }
//   }
// }

void MatrixLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    Compute_ClearRndGo(lay, net);
//     Compute_NoGoRndGo(lay, net);
  }

  inherited::Compute_HardClamp(lay, net);
}

void MatrixLayerSpec::LabelUnits_impl(Unit_Group* ugp) {
  int gp_sz = ugp->leaves / 2;
  for(int i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(i / gp_sz); // GO = 0, NOGO = 1
    if(go_no == PFCGateSpec::GATE_GO)
      u->name = "Go";
    else
      u->name = "No";
  }
}

void MatrixLayerSpec::LabelUnits(LeabraLayer* lay) {
  UNIT_GP_ITR(lay, LabelUnits_impl(ugp); );
}


//////////////////////////////////////////
//	PFC Unit Spec	
//////////////////////////////////////////

void PFCUnitSpec::Initialize() {
  
}

void PFCUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraNetwork* net) {
  LeabraUnit_Group* ugp = (LeabraUnit_Group*)u->owner; // assume..
  if(net->ct_cycle > net->mid_minus_cycle)  // only after gating computed, in plus phase!
    u->net += u->act_eq * ugp->misc_float2;
  // misc_float contains fully parameterized "go netin gain" value 
  inherited::Compute_Conduct(u, net);
}


//////////////////////////////////
//	PFC Layer Spec		//
//////////////////////////////////

void PFCGateSpec::Initialize() {
  graded_out_go = true;
  clear_decay = 0.9f;
  mid_minus_min = 10;
  base_gain = 0.0f;
  go_gain = 1.0f;
  max_maint = 100;
  off_accom = 0.0f;
}

void PFCGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  go_gain = 1.0f - base_gain;
}

void PFCGateSpec2::Initialize() {
  no_empty_out = true;
  no_mnt_rew = true;
  no_out_norew = true;
  out_norew_noclear = false;
  out_go_clear = true;
}

void PFCLearnSpec::Initialize() {
  go_learn_base = 0.06f;
  go_learn_mod = 1.0f - go_learn_base;
  go_netin_gain = 0.01f;
//   out_gate_act = OUT_M2;
}

void PFCLearnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  go_learn_mod = 1.0f - go_learn_base;
}

void PFCLayerSpec::Initialize() {
  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .15f;
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;
  SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.1f;
  decay.clamp_phase2 = false;	// this is the one exception!
}

void PFCLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate.UpdateAfterEdit_NoGui();
  learn.UpdateAfterEdit_NoGui();
}

void PFCLayerSpec::Defaults() {
  inherited::Defaults();
  gate.Defaults();
  learn.Defaults();
  Initialize();
}

void PFCLayerSpec::HelpConfig() {
  String help = "PFCLayerSpec Computation:\n\
 The PFC maintains activation over time (activation-based working memory) via\
 excitatory intracelluar ionic mechanisms (implemented via the hysteresis channels, gc.h),\
 and excitatory self-connections. These ion channels are toggled on and off via units in the\
 SNrThalLayerSpec layer, which are themsepves driven by MatrixLayerSpec units,\
 which are in turn trained up by dynamic dopamine changes computed by the PVLV system.\
 Updating occurs at the end of the 1st plus phase --- if a gating signal was activated, any previous ion\
 current is turned off, and the units are allowed to settle into a new state in the 2nd plus (update) --\
 then the ion channels are activated in proportion to activations at the end of this 2nd phase.\n\
 \nPFCLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Units must recv MarkerConSpec from SNrThalLayerSpec layer for gating\n\
 - This layer must be after SNrThalLayerSpec layer in list of layers\n\
 - Units must be organized into groups corresponding to the matrix groups (stripes).";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

void PFCLayerSpec::GetSNrThalLayers(LeabraLayer* lay, LeabraLayer*& snrthal_mnt,
				      LeabraLayer*& snrthal_out) {
  snrthal_mnt = NULL;
  snrthal_out = NULL;
  Projection* p;
  taLeafItr pi;
  FOR_ITR_EL(Projection, p, lay->projections., pi) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
    LeabraLayerSpec* ls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    if(ls->InheritsFrom(&TA_SNrThalLayerSpec)) {
      if(((SNrThalLayerSpec*)ls)->bg_type == SNrThalLayerSpec::OUTPUT)
	snrthal_out = fmlay;
      else
	snrthal_mnt = fmlay;
    }
  }
  if(snrthal_out) return;
  // look for snrthal_out in pfc_out layer
  FOR_ITR_EL(Projection, p, lay->send_prjns., pi) {
    LeabraLayer* tolay = (LeabraLayer*)p->layer;
    LeabraLayerSpec* ls = (LeabraLayerSpec*)tolay->spec.SPtr();
    if(ls->InheritsFrom(&TA_PFCOutLayerSpec)) {
      Projection* p2;
      taLeafItr pi2;
      FOR_ITR_EL(Projection, p2, tolay->projections., pi2) {
	LeabraLayer* fmlay = (LeabraLayer*)p2->from.ptr();
	LeabraLayerSpec* ls = (LeabraLayerSpec*)fmlay->spec.SPtr();
	if(ls->InheritsFrom(&TA_SNrThalLayerSpec)) {
	  snrthal_out = fmlay;	// must be
	}
      }
    }
  }
}

bool PFCLayerSpec::CheckConfig_Layer(Layer* ly,  bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  if(lay->CheckError(net->phase_order == LeabraNetwork::MINUS_PLUS_PLUS, quiet, rval,
		"requires LeabraNetwork phase_oder = MINUS_PLUS, I just set it for you")) {
    net->phase_order = LeabraNetwork::MINUS_PLUS;
  }

  if(lay->CheckError(net->mid_minus_cycle < 5, quiet, rval,
		"requires LeabraNetwork min_minus_cycle > 0, I just set it to 20 for you")) {
    net->mid_minus_cycle = 25;
  }

  if(lay->CheckError(net->min_cycles < net->mid_minus_cycle + 5, quiet, rval,
		"requires LeabraNetwork min_cycles >= mid_minus_cycle + 5, I just set it for you")) {
    net->min_cycles = net->mid_minus_cycle + 5;
  }

  if(lay->CheckError(gate.mid_minus_min >= net->mid_minus_cycle -1, quiet, rval,
		     "requires gate.mid_minus_min << mid_minus_cycle, I just set it to -10 for you")) {
    gate.mid_minus_min = net->mid_minus_cycle - 10;
  }

  if(lay->CheckError(net->sequence_init != LeabraNetwork::DO_NOTHING, quiet, rval,
		"requires network sequence_init = DO_NOTHING, I just set it for you")) {
    net->sequence_init = LeabraNetwork::DO_NOTHING;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->SetUnique("g_bar", true);
  if(lay->CheckError(us->hyst.init, quiet, rval,
		"requires UnitSpec hyst.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
  }
  if(lay->CheckError(us->acc.init, quiet, rval,
		"requires UnitSpec acc.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("acc", true);
    us->acc.init = false;
  }

  LeabraLayer* snrthal_mnt = NULL;
  LeabraLayer* snrthal_out = NULL;
  GetSNrThalLayers(lay, snrthal_mnt, snrthal_out);

  if(lay->CheckError(!snrthal_mnt, quiet, rval,
		"no projection from SNrThal MAINT Layer found: must exist with MarkerConSpec connection")) {
    return false;
  }
  if(lay->CheckError(snrthal_mnt->units.gp.size != lay->units.gp.size, quiet, rval,
		"Gating Layer unit groups must = PFCLayer unit groups!")) {
    snrthal_mnt->unit_groups = true;
    snrthal_mnt->gp_geom.n = lay->units.gp.size;
  }
  if(snrthal_out) {
    if(lay->CheckError(snrthal_out->units.gp.size != lay->units.gp.size, quiet, rval,
		       "Gating Layer unit groups must = PFCLayer unit groups!")) {
      snrthal_out->unit_groups = true;
      snrthal_out->gp_geom.n = lay->units.gp.size;
    }
  }

  return true;
}

void PFCLayerSpec::Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    ugp->misc_state1 = PFCGateSpec::INIT_STATE;
    ugp->misc_state2 = PFCGateSpec::GATE_NOGO;
    ugp->misc_float = learn.go_learn_base;
    ugp->misc_float1 = 0.0f;
    ugp->misc_float2 = 0.0f;
  }
}

void PFCLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Compute_TrialInitGates(lay, net);
}

void PFCLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // do NOT do this -- doing this per stripe anyway
//   inherited::Compute_MidMinus(lay, net);
  Compute_Gating_MidMinus(lay, net);
}
  
void PFCLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay, LeabraUnit_Group* ugp, 
					    int gp_idx, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ugp->, i) {
    u->act_m2 = u->act_eq;
  }
}

void PFCLayerSpec::Compute_MaintUpdt_ugp(LeabraUnit_Group* ugp, MaintUpdtAct updt_act,
					  LeabraLayer* lay, LeabraNetwork* net) {
  for(int j=0;j<ugp->size;j++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(j);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(updt_act == STORE) {
      u->vcb.g_h = u->maint_h = u->act_eq; // note: store current act value
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    else if(updt_act == CLEAR) {
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = gate.off_accom * u->vcb.g_h;
      u->vcb.g_h = u->maint_h = 0.0f;
      if(gate.clear_decay > 0.0f) {
	float decay = gate.clear_decay;
	// don't use DecayState -- only for between phases
	// just focus on key vars that will affect hystersis and upcoming act computation
	u->v_m -= decay * (u->v_m - us->v_m_init.mean);
	u->net -= decay * u->net;
      }
    }
    us->Compute_Conduct(u, net); // update displayed conductances!
  }
}

void PFCLayerSpec::Compute_Gating(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayer* snrthal_mnt = NULL;
  LeabraLayer* snrthal_out = NULL;
  GetSNrThalLayers(lay, snrthal_mnt, snrthal_out);
  SNrThalLayerSpec* snrthalsp_mnt = (SNrThalLayerSpec*)snrthal_mnt->spec.SPtr();
  float go_thr_mnt = snrthalsp_mnt->snrthal.go_thr;
  float go_thr_out = go_thr_mnt;
  SNrThalLayerSpec* snrthalsp_out = NULL;
  if(snrthal_out) {
    snrthalsp_out = (SNrThalLayerSpec*)snrthal_out->spec.SPtr();
    go_thr_out = snrthalsp_out->snrthal.go_thr;
  }

  // this is the continuous mid minus version -- when gating actually happens

  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp_mnt = (LeabraUnit_Group*)snrthal_mnt->units.gp[mg];
    LeabraUnit* snr_mnt_u = (LeabraUnit*)snrgp_mnt->Leaf(0);
    LeabraUnit* snr_out_u = NULL;
    LeabraUnit_Group* snrgp_out = NULL;
    if(snrthal_out) {
      snrgp_out = (LeabraUnit_Group*)snrthal_out->units.gp[mg];
      snr_out_u = (LeabraUnit*)snrgp_out->Leaf(0);
    }

    int pfc_mnt_cnt = ugp->misc_state; // is pfc maintaining or not?

    bool out_gate_fired = false;
    bool mnt_gate_fired = false;
    if((ugp->misc_state2 == PFCGateSpec::GATE_OUT_GO) ||
       (ugp->misc_state2 == PFCGateSpec::GATE_OUT_MNT_GO)) {
      out_gate_fired = true;
    }
    if((ugp->misc_state2 == PFCGateSpec::GATE_MNT_GO) ||
       (ugp->misc_state2 == PFCGateSpec::GATE_OUT_MNT_GO)) {
      mnt_gate_fired = true;
    }

    // what we do on this trial
    bool gate_out = false;
    bool gate_mnt = false;

    // output gating signal -- can only happen if hasn't happened yet!
    if(!out_gate_fired && snr_out_u && (snr_out_u->act_eq > go_thr_out) &&
       (!gate2.no_empty_out || pfc_mnt_cnt > 0) &&
       (!gate2.no_out_norew || net->pv_detected)) {
      gate_out = true;
      out_gate_fired = true;	// now it has..

      // compute out_gate multiplier in misc_float1
      float out_go_act = 1.0f;	// activation of output gating unit specifically
      if(gate.graded_out_go)
	out_go_act = snr_out_u->act_eq;
      ugp->misc_float1 = gate.base_gain + (gate.go_gain * out_go_act); // out gate multiplier

      // misc_float2 includes param -- update once only!
      Compute_MidMinusAct_ugp(lay, ugp, mg, net);
      snrthalsp_out->Compute_MidMinusAct_ugp(snrthal_out, snrgp_out, mg, net);
      // snrthal and associated matrix layer grab act_m2 vals based on current state!
    }

    // maintenance gating signal -- can only happen if hasn't happened yet!
    if(!mnt_gate_fired && snr_mnt_u->act_eq > go_thr_mnt && 
       (!gate2.no_mnt_rew || !net->pv_detected)) {
      gate_mnt = true;
      mnt_gate_fired = true;	// now it has..

      // compute out_gate multiplier in misc_float1 -- maint gating causes output gating too!
      float mnt_go_act = 1.0f;	// activation of output gating unit specifically
      if(gate.graded_out_go)
	mnt_go_act = snr_mnt_u->act_eq;
      ugp->misc_float1 = gate.base_gain + (gate.go_gain * mnt_go_act); // out gate multiplier

      Compute_MidMinusAct_ugp(lay, ugp, mg, net);
      snrthalsp_mnt->Compute_MidMinusAct_ugp(snrthal_mnt, snrgp_mnt, mg, net);
      // snrthal and associated matrix layer grab act_m2 vals based on current state!

      if(pfc_mnt_cnt > 0) // full stripe
	Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net);	 // clear maint currents if full -- toggle off
    }

    if(gate_mnt || gate_out) {	// something happened
      float lrn_go_act = 0.0f;	// activation of go gating unit (out or mnt depending -- used for learning)
      if(mnt_gate_fired) {
	if(out_gate_fired) {  // both output and maint gating fired..
	  ugp->misc_state2 = PFCGateSpec::GATE_OUT_MNT_GO;
	  if(pfc_mnt_cnt > 0) // full stripe
	    ugp->misc_state1 = PFCGateSpec::MAINT_OUT_MNT_GO;
	  else
	    ugp->misc_state1 = PFCGateSpec::EMPTY_OUT_MNT_GO;
	  lrn_go_act = MAX(snr_mnt_u->act_m2, snr_out_u->act_m2);
	}
	else {
	  // only maintenance fired
	  ugp->misc_state2 = PFCGateSpec::GATE_MNT_GO;
	  if(pfc_mnt_cnt > 0) // full stripe
	    ugp->misc_state1 = PFCGateSpec::MAINT_MNT_GO;
	  else
	    ugp->misc_state1 = PFCGateSpec::EMPTY_MNT_GO;
	  lrn_go_act = snr_mnt_u->act_m2;
	}
      }
      else if(out_gate_fired) {	// just out gate fired
	ugp->misc_state2 = PFCGateSpec::GATE_OUT_GO;
	if(pfc_mnt_cnt > 0) // full stripe
	  ugp->misc_state1 = PFCGateSpec::MAINT_OUT_GO;
	else
	  ugp->misc_state1 = PFCGateSpec::EMPTY_OUT_GO;
	lrn_go_act = snr_out_u->act_m2;
      }

      // misc_float has the go_learn_base factor incorporated
      ugp->misc_float = learn.go_learn_base + (learn.go_learn_mod * lrn_go_act);
      ugp->misc_float2 = learn.go_netin_gain * lrn_go_act;
    }
  }
}


void PFCLayerSpec::Compute_Gating_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayer* snrthal_mnt = NULL;
  LeabraLayer* snrthal_out = NULL;
  GetSNrThalLayers(lay, snrthal_mnt, snrthal_out);
  SNrThalLayerSpec* snrthalsp_mnt = (SNrThalLayerSpec*)snrthal_mnt->spec.SPtr();
  SNrThalLayerSpec* snrthalsp_out = NULL;
  if(snrthal_out) {
    snrthalsp_out = (SNrThalLayerSpec*)snrthal_out->spec.SPtr();
  }

  // cleanup gating signals at end of mid minus -- just nogo!

  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp_mnt = (LeabraUnit_Group*)snrthal_mnt->units.gp[mg];
    LeabraUnit* snr_mnt_u = (LeabraUnit*)snrgp_mnt->Leaf(0);
    LeabraUnit* snr_out_u = NULL;
    LeabraUnit_Group* snrgp_out = NULL;
    if(snrthal_out) {
      snrgp_out = (LeabraUnit_Group*)snrthal_out->units.gp[mg];
      snr_out_u = (LeabraUnit*)snrgp_out->Leaf(0);
    }

    int pfc_mnt_cnt = ugp->misc_state; // is pfc maintaining or not?

    if(ugp->misc_state2 == PFCGateSpec::GATE_NOGO) {
      // default NOGO results
      if(pfc_mnt_cnt > 0) // full stripe
	ugp->misc_state1 = PFCGateSpec::MAINT_NOGO;
      else
	ugp->misc_state1 = PFCGateSpec::EMPTY_NOGO;
      Compute_MidMinusAct_ugp(lay, ugp, mg, net);
      if(snrthal_out)
	snrthalsp_out->Compute_MidMinusAct_ugp(snrthal_out, snrgp_out, mg, net);
      snrthalsp_mnt->Compute_MidMinusAct_ugp(snrthal_mnt, snrgp_mnt, mg, net);

      // misc_float has the go_learn_base factor incorporated
      ugp->misc_float = learn.go_learn_base;
    }
  }
  
  // now have full set of info -- send it along!
  SendGateStates(lay, net);
}

void PFCLayerSpec::Compute_Gating_Final(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];

    // misc_state == maintenance/empty counter (maintenance = + numbers, empty = - numbers)
    // basically just update the misc_state counter and implement any
    // delayed STORE or CLEAR actions

    // for NOGO, just update the misc_state counter
    if(ugp->misc_state1 == PFCGateSpec::EMPTY_NOGO) {
      ugp->misc_state--;	// stay empty
    }
    else if(ugp->misc_state1 == PFCGateSpec::MAINT_NOGO) {
      ugp->misc_state++;	// continue maintaining
      if(ugp->misc_state > gate.max_maint) {
	Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net);     // clear it!
	ugp->misc_state = 0;			     // empty
      }
    }
    // look for store condition
    else if(ugp->misc_state1 == PFCGateSpec::MAINT_MNT_GO ||
	    ugp->misc_state1 == PFCGateSpec::EMPTY_MNT_GO ||
	    ugp->misc_state1 == PFCGateSpec::EMPTY_OUT_MNT_GO ||
	    ugp->misc_state1 == PFCGateSpec::MAINT_OUT_MNT_GO) {
      if(gate.max_maint > 0) {			     // if max_maint = 0 then never store
	Compute_MaintUpdt_ugp(ugp, STORE, lay, net);     // store it (never stored before)
	ugp->misc_state = 1;	// always reset on new store
      }
    }
    // or basic output gate with no veto from maint
    else if(ugp->misc_state1 == PFCGateSpec::MAINT_OUT_GO) {
      if(gate2.out_go_clear &&
	 (!gate2.out_norew_noclear || net->pv_detected)) { // only clear on true output trials
	Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net);     // clear it!
	ugp->misc_state = 0;			     // empty
      }
      else {
	ugp->misc_state++;	// continue maintaining
      }
    }
    else if(ugp->misc_state1 == PFCGateSpec::EMPTY_OUT_GO) {
      ugp->misc_state--;	// no real issue here..
    }
  }
  // NOTE: Do NOT send final gate states -- empty/maint status checks on other layers
  // need to reflect status at time of gating computation (mid minus)
  //  SendGateStates(lay, net);
}

void PFCLayerSpec::SendGateStates(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayer* snrthal_mnt = NULL;
  LeabraLayer* snrthal_out = NULL;
  GetSNrThalLayers(lay, snrthal_mnt, snrthal_out);
  ((SNrThalLayerSpec*)snrthal_mnt->spec.SPtr())->SendGateStates(snrthal_mnt, net, lay);
  if(snrthal_out) {
    ((SNrThalLayerSpec*)snrthal_out->spec.SPtr())->SendGateStates(snrthal_out, net, lay);
  }
}

void PFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == 1) {
    Compute_Gating_Final(lay, net);	// final gating
  }
}

void PFCLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle >= gate.mid_minus_min && net->ct_cycle <= net->mid_minus_cycle) {
    Compute_Gating(lay, net);	// continuously during mid minus phase
  }
  if(net->phase == LeabraNetwork::PLUS_PHASE)
    Compute_PfcMntAct(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void PFCLayerSpec::Compute_PfcMntAct(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];

    float lrn_mod_val = rugp->misc_float; // learning modulation value as function of gating
    
    for(int i=0;i<rugp->size;i++) {
      LeabraUnit* ru = (LeabraUnit*)rugp->FastEl(i);
      // only stray so far away from act_m  -- depending on gating strength
      ru->act_nd = ru->act_m + lrn_mod_val * (ru->act_nd - ru->act_m); 
    }
  }
}



//////////////////////////////////
//	PFCOut Layer Spec	//
//////////////////////////////////

void PFCOutLayerSpec::Initialize() {
  // this guy should always inherit from PFCLayerSpec
//   SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .15f;
//   SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
//   SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;
//   SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.1f;
  decay.clamp_phase2 = false;	// this is the one exception!
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void PFCOutLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void PFCOutLayerSpec::HelpConfig() {
  String help = "PFCOutLayerSpec Computation:\n\
 The PFCOut layer gets activations from corresponding PFC layer (via MarkerCon)\
 and sets unit activations as a function of the Go gating signals received from\
 associated SNrThal layer\n\
 \nPFCOutLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure PFC and BG layers.\n\
 - Units must recv MarkerConSpec from SNrThalLayerSpec layer for gating\n\
 - Units must recv MarkerConSpec from PFCLayerSpec layer for activations\n\
 - This layer must be after SNrThalLayerSpec layer in list of layers\n\
 - This layer must be after PFCLayerSpec layer in list of layers\n\
 - Units must be organized into groups corresponding to the matrix groups (stripes).";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool PFCOutLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  bool rval = true;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_PFCLayerSpec);
  if(lay->CheckError(!pfc_lay, quiet, rval,
		"no projection from PFC Layer found: must have MarkerConSpec!")) {
    return false;
  }
  if(lay->CheckError(pfc_lay->units.gp.size != lay->units.gp.size, quiet, rval,
		"PFC Layer unit groups must = PFCOutLayer unit groups, copiped from PFC Layer; Please do a Build of network")) {
    lay->unit_groups = true;
    lay->gp_geom.n = pfc_lay->units.gp.size;
  }
  if(lay->CheckError(pfc_lay->units.leaves != lay->units.leaves, quiet, rval,
		"PFC Layer units must = PFCOutLayer units, copied from PFC Layer; Please do a Build of network")) {
    lay->un_geom = pfc_lay->un_geom;
  }

  return true;
}

void PFCOutLayerSpec::Compute_PfcOutAct(LeabraLayer* lay, LeabraNetwork* net) {
  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_PFCLayerSpec);
  PFCLayerSpec* pfcspec = (PFCLayerSpec*)pfc_lay->spec.SPtr();

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* pfcgp = (LeabraUnit_Group*)pfc_lay->units.gp[mg];

    rugp->misc_state = pfcgp->misc_state;
    rugp->misc_state1 = pfcgp->misc_state1;
    rugp->misc_state2 = pfcgp->misc_state2;
    rugp->misc_float = pfcgp->misc_float;
    rugp->misc_float1 = pfcgp->misc_float1;

    float gate_val = rugp->misc_float1;
    if(net->ct_cycle <= net->mid_minus_cycle) {
      gate_val = pfcspec->gate.base_gain; // everyone is at base gain prior to output gate
    }
    
    for(int i=0;i<rugp->size;i++) {
      LeabraUnit* ru = (LeabraUnit*)rugp->FastEl(i);
      LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();
      LeabraUnit* pfcu = (LeabraUnit*)pfcgp->FastEl(i);

//       if(net->ct_cycle > net->mid_minus_cycle) {
// // 	if(pfcspec->learn.out_gate_act == PFCLearnSpec::OUT_M2)
// // 	  ru->act = gate_val * pfcu->act_m2; // use memory value, due to updating issues "hand off"
// // 	else
// 	ru->act = gate_val * pfcu->act_eq; // go live
//       }
//       else {
      ru->act = gate_val * pfcu->act_eq; // live val is fine
//       }
      ru->act_eq = ru->act_nd = ru->act;
      ru->da = 0.0f;		// I'm fully settled!
      ru->AddToActBuf(rus->syn_delay);
    }
  }
}

void PFCOutLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_PfcOutAct(lay, net);
  inherited::Compute_CycleStats(lay, net);
}


///////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////
// 		PFCLVPrjnSpec		//
//////////////////////////////////////////

void PFCLVPrjnSpec::Connect_Gp(Projection* prjn, Unit_Group* rugp, Unit_Group* sugp) {
  // pre-allocate connections!
  Unit* ru, *su;
  taLeafItr ru_itr, su_itr;
  FOR_ITR_EL(Unit, ru, rugp->, ru_itr) {
    ru->RecvConsPreAlloc(sugp->leaves, prjn);
  }
  // todo: this may not be right for fuller connectivity..
  FOR_ITR_EL(Unit, su, sugp->, su_itr) {
    su->SendConsPreAlloc(rugp->leaves, prjn);
  }

  FOR_ITR_EL(Unit, ru, rugp->, ru_itr) {
    FOR_ITR_EL(Unit, su, sugp->, su_itr) {
      if(self_con || (ru != su))
	ru->ConnectFrom(su, prjn);
    }
  }
}

void PFCLVPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  LeabraLayer* lv_lay = (LeabraLayer*)prjn->layer;
  int tot_pfc_prjns = 0;
  int tot_pfc_stripes = 0;
  int my_start_stripe = 0;
  int my_prjn_idx = 0;
  for(int i=0; i< lv_lay->projections.size; i++) {
    Projection* prj = lv_lay->projections[i];
    if(prj->spec.SPtr()->InheritsFrom(&TA_PFCLVPrjnSpec)) {
      if(prj == prjn) {
	my_start_stripe = tot_pfc_stripes;
	my_prjn_idx = tot_pfc_prjns;
      }
      tot_pfc_prjns++;
      tot_pfc_stripes += prj->from->units.gp.size;
    }
  }

  Unit_Group* lv_gp = &(prjn->layer->units); // lv = recv
  Unit_Group* pfc_gp = &(prjn->from->units); // pfc = send

  int n_lv_stripes = lv_gp->gp.size;
  int n_pfc_stripes = pfc_gp->gp.size;

  if(n_lv_stripes <= 1) {	// just full connectivity
    Connect_Gp(prjn, lv_gp, pfc_gp);
  }
  else if(n_lv_stripes == n_pfc_stripes) { // just one-to-one
    for(int i=0; i<n_pfc_stripes; i++) {
      Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(i);
      Unit_Group* sgp = (Unit_Group*)pfc_gp->gp.FastEl(i);
      Connect_Gp(prjn, rgp, sgp);
    }
  }
  else if(n_lv_stripes == n_pfc_stripes + 1) { // full plus one-to-one
    Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(0);
    Connect_Gp(prjn, rgp, pfc_gp); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(i+1);
      Unit_Group* sgp = (Unit_Group*)pfc_gp->gp.FastEl(i);
      Connect_Gp(prjn, rgp, sgp);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes) { // multi-pfc just one-to-one
    for(int i=0; i<n_pfc_stripes; i++) {
      Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(my_start_stripe + i);
      Unit_Group* sgp = (Unit_Group*)pfc_gp->gp.FastEl(i);
      Connect_Gp(prjn, rgp, sgp);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes + 1) { // multi-pfc full plus one-to-one
    Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(0);
    Connect_Gp(prjn, rgp, pfc_gp); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(my_start_stripe + i+1);
      Unit_Group* sgp = (Unit_Group*)pfc_gp->gp.FastEl(i);
      Connect_Gp(prjn, rgp, sgp);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes + tot_pfc_prjns) { // multi-pfc separate full plus one-to-one
    Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(my_start_stripe + my_prjn_idx);
    Connect_Gp(prjn, rgp, pfc_gp); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Unit_Group* rgp = (Unit_Group*)lv_gp->gp.FastEl(my_start_stripe + my_prjn_idx + i+1);
      Unit_Group* sgp = (Unit_Group*)pfc_gp->gp.FastEl(i);
      Connect_Gp(prjn, rgp, sgp);
    }
  }
  else {
    TestError(true, "Connect_impl", "Number of LV unit groups (stripes) must be either 1, equal to number of PFC stripes (sending prjn), or PFC stripes + 1 -- was not any of these -- connection failed");
  }
}


/////////////////////////////
//	  MatrixRnd	   //
/////////////////////////////

void MatrixRndPrjnSpec::Initialize() {
  p_con = .5f;
  same_seed = true;
  rndm_seed.GetCurrent();
  send_idx_ars.SetBaseType(&TA_int_Array);
}

void MatrixRndPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

void MatrixRndPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(same_seed)
    rndm_seed.OldSeed();

  int n_rgps = prjn->layer->units.gp.size;
  if(TestWarning(n_rgps == 0, "Connect_impl",
		 "requires recv layer to have unit groups!")) {
    return;
  }

  int n_sends = prjn->from->units.leaves;
  bool init_idxs = false;
  if(send_idx_ars.size != n_rgps) {
    init_idxs = true;
    send_idx_ars.SetSize(n_rgps);
  }
  int_Array* ari = (int_Array*)send_idx_ars[0];
  if(ari->size != n_sends)
    init_idxs = true;
  if(init_idxs) {
    for(int i=0;i<n_rgps;i++) {
      int_Array* ari = (int_Array*)send_idx_ars[i];
      ari->SetSize(n_sends);
      ari->FillSeq();
      ari->Permute();
    }
  }

  int recv_no = (int)((p_con * ((float)n_sends)) + .5f);
  if(recv_no <= 0) recv_no = 1;

  // sending number is even distribution across senders plus some imbalance factor
  float send_no_flt = (float)(prjn->layer->units.leaves * recv_no) / (float)n_sends;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(n_rgps); // n is n_rgps -- much noisier
  int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
  if(send_no > prjn->layer->units.leaves) send_no = prjn->layer->units.leaves;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  for(int gpi=0; gpi<n_rgps; gpi++) {
    Unit_Group* ru_gp = (Unit_Group*)prjn->layer->units.FastGp(gpi);
    int_Array* ari = (int_Array*)send_idx_ars[gpi];
    for(int rui=0; rui<ru_gp->leaves; rui++) {
      Unit* ru = ru_gp->Leaf(rui);
      for(int sui=0; sui<recv_no; sui++) {
	Unit* su = prjn->from->units.Leaf(ari->FastEl(sui));
	ru->ConnectFrom(su, prjn, false, false);
	// final false change to true = ignore errs -- to be expected
      }
    }
  }
}

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

///////////////////////////////////////////////////////////////
//			PBWM
///////////////////////////////////////////////////////////////

bool LeabraWizard::PBWM_ToLayerGroups(LeabraNetwork* net) {
  if(TestError(!net, "PBWM_ToLayerGroup", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  PVLV_ToLayerGroup(net);	// do the pvlv version

  bool new_bg_laygp = false;
  Layer_Group* bg_laygp = net->FindMakeLayerGroup("PBWM_BG", NULL, new_bg_laygp);
  bool new_pfc_laygp = false;
  Layer_Group* pfc_laygp = net->FindMakeLayerGroup("PBWM_PFC", NULL, new_pfc_laygp);

  Layer* lay;
  if(lay = net->FindLayer("Patch")) { bg_laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("SNc")) { bg_laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("Matrix")) { bg_laygp->Transfer(lay); lay->pos.z = 1; }
  if(lay = net->FindLayer("Matrix_mnt")) { bg_laygp->Transfer(lay); lay->pos.z = 1; }
  if(lay = net->FindLayer("Matrix_out")) { bg_laygp->Transfer(lay); lay->pos.z = 1; }
  if(lay = net->FindLayer("SNrThal")) { bg_laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("SNrThal_out")) { bg_laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("SNrThal_mnt")) { bg_laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("PFC")) { pfc_laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("PFC_mnt")) { pfc_laygp->Transfer(lay); lay->pos.z = 0; }
  if(lay = net->FindLayer("PFC_out")) { pfc_laygp->Transfer(lay); lay->pos.z = 0; }
  
  if(new_bg_laygp || new_pfc_laygp) {
    bg_laygp->pos.z = 0;
    pfc_laygp->pos.z = 2;
    net->RebuildAllViews();	// trigger update
  }

  return true;
}

static void lay_set_geom(LeabraLayer* lay, int n_stripes, int n_units = -1, bool sp = true) {
  lay->unit_groups = true;
  lay->SetNUnitGroups(n_stripes);
  if(n_stripes <= 5) {
    lay->gp_geom.x = n_stripes;
    lay->gp_geom.y = 1;
  }
  if(n_units > 0) {
    lay->SetNUnits(n_units);
  }
  if(sp) {
    lay->gp_spc.x = 1;
    lay->gp_spc.y = 1;
  }
  lay->UpdateAfterEdit();
}

static void set_n_stripes(LeabraNetwork* net, const char* nm, int n_stripes,
  int n_units, bool sp)
{
  LeabraLayer* lay = (LeabraLayer*)net->FindLayer(nm);
  if(lay == NULL) return;
  lay_set_geom(lay, n_stripes, n_units, sp);
}

bool LeabraWizard::PBWM_SetNStripes(LeabraNetwork* net, int n_stripes, int n_units) {
  if(TestError(!net, "PBWM_SetNStripes", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  // this is called as a subroutine a lot too so don't save here -- could do impl but
  // not really worth it..
//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_SetNStripes -- actually saves network specifically");
//   }

  set_n_stripes(net, "PFC", n_stripes, n_units, true);
  set_n_stripes(net, "PFC_mnt", n_stripes, n_units, true);
  set_n_stripes(net, "PFC_out", n_stripes, n_units, true);
  set_n_stripes(net, "Matrix", n_stripes, -1, true);
  set_n_stripes(net, "Matrix_mnt", n_stripes, -1, true);
  set_n_stripes(net, "Matrix_out", n_stripes, -1, true);
  set_n_stripes(net, "SNrThal", n_stripes, -1, false);
  set_n_stripes(net, "SNrThal_mnt", n_stripes, -1, false);
  set_n_stripes(net, "SNrThal_out", n_stripes, -1, false);
  set_n_stripes(net, "LVe", 1, -1, false);
  set_n_stripes(net, "LVi", 1, -1, false);
  set_n_stripes(net, "Patch", n_stripes, -1, false);
  set_n_stripes(net, "SNc", n_stripes, -1, false);
  net->LayoutUnitGroups();
  net->Build();
  return true;
}

bool LeabraWizard::PBWM(LeabraNetwork* net, bool da_mod_all,
			int n_stripes, bool out_gate, bool nolrn_pfc) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PBWM", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork(&TA_LeabraNetwork, net)) return false;
  }

  // first configure PVLV system..
  if(TestError(!PVLV(net, da_mod_all), "PBWM", "could not make PVLV")) return false;

  String msg = "Configuring PBWM (Prefrontal-cortex Basal-ganglia Working Memory) Layers:\n\n\
 There is one thing you will need to check manually after this automatic configuration\
 process completes (this note will be repeated when things complete --- there may be some\
 messages in the interim):\n\n";

  String man_msg = "1. Check the bidirectional connections between the PFC and all appropriate hidden layers\
 The con specs INTO the PFC should be ToPFC conspecs; the ones out should be regular learning conspecs.";

  msg += man_msg + "\n\nThe configuration will now be checked and a number of default parameters\
 will be set.  If there are any actual errors which must be corrected before\
 the network will run, you will see a message to that effect --- you will then need to\
 re-run this configuration process to make sure everything is OK.  When you press\
 Re/New/Init on the control process these same checks will be performed, so you\
 can be sure everything is ok.";
  taMisc::Confirm(msg);

  net->RemoveUnits();

  String pvenm = "PVe";  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";
  String vtanm = "VTA";

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  // Harvest from the PVLV function..
  Layer_Group* pvlv_laygp = net->FindMakeLayerGroup("PVLV");
  LeabraLayer* rew_targ_lay = (LeabraLayer*)pvlv_laygp->FindName("RewTarg");
  LeabraLayer* pve = (LeabraLayer*)pvlv_laygp->FindName(pvenm);
  LeabraLayer* pvr = (LeabraLayer*)pvlv_laygp->FindName(pvrnm);
  LeabraLayer* pvi = (LeabraLayer*)pvlv_laygp->FindName(pvinm);
  LeabraLayer* lve = (LeabraLayer*)pvlv_laygp->FindName(lvenm);
  LeabraLayer* lvi = (LeabraLayer*)pvlv_laygp->FindName(lvinm);
  LeabraLayer* nv =  (LeabraLayer*)pvlv_laygp->FindName(nvnm);
  LeabraLayer* vta = (LeabraLayer*)pvlv_laygp->FindName(vtanm);
  if(!vta)
    vta = (LeabraLayer*)pvlv_laygp->FindName("DA");
  if(!rew_targ_lay || !lve || !pve || !pvi || !vta) return false;

  bool new_bg_laygp = false;
  Layer_Group* bg_laygp = net->FindMakeLayerGroup("PBWM_BG", NULL, new_bg_laygp);
  bool new_pfc_laygp = false;
  Layer_Group* pfc_laygp = net->FindMakeLayerGroup("PBWM_PFC", NULL, new_pfc_laygp);
  
  if(new_bg_laygp || new_pfc_laygp) {
    PBWM_ToLayerGroups(net);	// doesn't hurt to just do this..
  }

  // if not new layers, don't make prjns into them!
  bool patch_new = false;     bool snc_new = false;
  bool matrix_m_new = false;  bool snrthal_m_new = false; bool pfc_m_new = false;
  bool matrix_o_new = false;  bool snrthal_o_new = false; bool pfc_o_new = false;

  LeabraLayer* patch = NULL;
  LeabraLayer* snc = NULL;

  LeabraLayer* matrix_m = NULL;
  LeabraLayer* snrthal_m = NULL;
  LeabraLayer* pfc_m = NULL;

  LeabraLayer* matrix_o = NULL;	// output gate versions
  LeabraLayer* snrthal_o = NULL;
  LeabraLayer* pfc_o = NULL;

  patch = (LeabraLayer*)bg_laygp->FindMakeLayer("Patch", NULL, patch_new, "Patch");
  snc = (LeabraLayer*)bg_laygp->FindMakeLayer("SNc", NULL, snc_new, "SNc");

  if(out_gate) {
    matrix_m = (LeabraLayer*)bg_laygp->FindMakeLayer("Matrix_mnt", NULL, matrix_m_new, "Matrix");
    matrix_o = (LeabraLayer*)bg_laygp->FindMakeLayer("Matrix_out", NULL, matrix_o_new);

    snrthal_m = (LeabraLayer*)bg_laygp->FindMakeLayer("SNrThal_mnt", NULL, snrthal_m_new, "SNrThal");
    snrthal_o = (LeabraLayer*)bg_laygp->FindMakeLayer("SNrThal_out", NULL, snrthal_o_new);

    pfc_m = (LeabraLayer*)pfc_laygp->FindMakeLayer("PFC_mnt", NULL, pfc_m_new, "PFC");
    pfc_o = (LeabraLayer*)pfc_laygp->FindMakeLayer("PFC_out", NULL, pfc_o_new);
  }
  else {
    matrix_m = (LeabraLayer*)bg_laygp->FindMakeLayer("Matrix", NULL, matrix_m_new, "Matrix");
    snrthal_m = (LeabraLayer*)bg_laygp->FindMakeLayer("SNrThal", NULL, snrthal_m_new, "SNrThal");
    pfc_m = (LeabraLayer*)pfc_laygp->FindMakeLayer("PFC", NULL, pfc_m_new);
  }

  if(!patch || !snc || !matrix_m || !snrthal_m || !pfc_m) return false;

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  int mx_z1 = 0;		// max x coordinate on layer z=1
  int mx_z2 = 0;		// z=2
  Layer_Group other_lays;  Layer_Group hidden_lays;
  Layer_Group output_lays;  Layer_Group input_lays;
  TDCoord lpos;
  int i;
  for(i=0;i<net->layers.leaves;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
    if(lay != rew_targ_lay && lay != pve && lay != pvr && lay != pvi
       && lay != lve && lay != lvi && lay != nv && lay != vta
       && lay != patch && lay != snc
       && lay != snrthal_m && lay != matrix_m && lay != pfc_m
       && lay != snrthal_o && lay != matrix_o && lay != pfc_o) {
      other_lays.Link(lay);
      lay->GetAbsPos(lpos);
      if(lpos.z == 0) lay->pos.z+=2; // nobody allowed in 0!
      int xm = lpos.x + lay->scaled_act_geom.x + 1;
      if(lpos.z == 1) mx_z1 = MAX(mx_z1, xm);
      if(lpos.z == 2) mx_z2 = MAX(mx_z2, xm);
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

  BaseSpec_Group* units = net->FindMakeSpecGp("PFC_BG_Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp("PFC_BG_Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp("PFC_BG_Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp("PFC_BG_Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return false;

  LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* lv_units = (LeabraUnitSpec*)pv_units->FindMakeChild("LVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_LeabraUnitSpec);

  LeabraUnitSpec* pfc_units = (LeabraUnitSpec*)units->FindMakeSpec("PFCUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* matrix_units = (LeabraUnitSpec*)units->FindMakeSpec("MatrixUnits", &TA_MatrixUnitSpec);
  LeabraUnitSpec* snrthal_units = (LeabraUnitSpec*)units->FindMakeSpec("SNrThalUnits", &TA_LeabraUnitSpec);
  MatrixUnitSpec* matrixo_units = NULL;
  if(out_gate) {
    matrixo_units = (MatrixUnitSpec*)matrix_units->FindMakeChild("MatrixOut", &TA_MatrixUnitSpec);
  }

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  LeabraConSpec* pvi_cons = (LeabraConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  LeabraConSpec* pvr_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("PVr", &TA_PVrConSpec);
  LeabraConSpec* lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_PVConSpec);
  LeabraConSpec* lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_PVConSpec);
  LeabraConSpec* nv_cons =  (LeabraConSpec*)pvi_cons->FindMakeChild("NV", &TA_PVConSpec);

  LeabraConSpec* topfc_cons = (LeabraConSpec*)learn_cons->FindMakeChild("ToPFC", &TA_LeabraConSpec);
  LeabraConSpec* intra_pfc = (LeabraConSpec*)topfc_cons->FindMakeChild("IntraPFC", &TA_LeabraConSpec);
  LeabraConSpec* pfc_bias = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCBias", &TA_LeabraBiasSpec);
  LeabraConSpec* fmpfcmnt_cons = NULL;
  LeabraConSpec* fmpfcout_cons = NULL;
  if(out_gate) {
    fmpfcmnt_cons = (LeabraConSpec*)learn_cons->FindMakeChild("FmPFC_mnt", &TA_LeabraConSpec);
    fmpfcout_cons = (LeabraConSpec*)learn_cons->FindMakeChild("FmPFC_out", &TA_LeabraConSpec);
  }
  MatrixConSpec* matrix_cons = (MatrixConSpec*)learn_cons->FindMakeChild("MatrixCons", &TA_MatrixConSpec);
  MatrixConSpec* mfmpfc_cons = (MatrixConSpec*)matrix_cons->FindMakeChild("MatrixFmPFC", &TA_MatrixConSpec);

  MatrixConSpec* matrixo_cons = NULL;
  MatrixConSpec* mofmpfc_cons = NULL;
  if(out_gate) {
    matrixo_cons = (MatrixConSpec*)matrix_cons->FindMakeChild("Matrix_out", &TA_MatrixConSpec);
    mofmpfc_cons = (MatrixConSpec*)matrixo_cons->FindMakeChild("Matrix_out_FmPFC", &TA_MatrixConSpec);
  }
  MatrixBiasSpec* matrix_bias = (MatrixBiasSpec*)matrix_cons->FindMakeChild("MatrixBias", &TA_MatrixBiasSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  LeabraConSpec* pfc_self = (LeabraConSpec*)cons->FindMakeSpec("PFCSelfCon", &TA_LeabraConSpec);

  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  LeabraConSpec* old_matrix_bias = (LeabraConSpec*)bg_bias->children.FindSpecName("MatrixCons");
  if(old_matrix_bias) {
    bg_bias->children.RemoveEl(old_matrix_bias);
  }
  LeabraLayerSpec* rewtargsp = (LeabraLayerSpec*)layers->FindMakeSpec("RewTargLayer", &TA_LeabraLayerSpec);
  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec);
  LViLayerSpec* lvisp = (LViLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec);
  NVLayerSpec* nvsp = (NVLayerSpec*)layers->FindMakeSpec(nvnm + "Layer", &TA_NVLayerSpec);
  PatchLayerSpec* patchsp = (PatchLayerSpec*)lvesp->FindMakeChild("PatchLayer", &TA_PatchLayerSpec);

  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindType(&TA_PVLVDaLayerSpec);
  SNcLayerSpec* sncsp = (SNcLayerSpec*)dasp->FindMakeChild("SNcLayer", &TA_SNcLayerSpec);

  PFCLayerSpec* pfcmsp = (PFCLayerSpec*)layers->FindMakeSpec("PFCLayer", &TA_PFCLayerSpec);
  PFCOutLayerSpec* pfcosp = NULL;
  if(out_gate)
    pfcosp = (PFCOutLayerSpec*)pfcmsp->FindMakeChild("PFCOutLayer", &TA_PFCOutLayerSpec);
  MatrixLayerSpec* matrixsp = (MatrixLayerSpec*)layers->FindMakeSpec("MatrixLayer", &TA_MatrixLayerSpec);

  MatrixLayerSpec* matrixosp = NULL;
  if(out_gate)
    matrixosp = (MatrixLayerSpec*)matrixsp->FindMakeChild("Matrix_out", &TA_MatrixLayerSpec);

  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snrthalosp = NULL;
  if(out_gate)
    snrthalosp = (SNrThalLayerSpec*)snrthalsp->FindMakeChild("SNrThalOut", &TA_SNrThalLayerSpec);

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  UniformRndPrjnSpec* topfc = (UniformRndPrjnSpec*)prjns->FindMakeSpec("ToPFC", &TA_UniformRndPrjnSpec);
  ProjectionSpec* pfc_selfps = (ProjectionSpec*)prjns->FindMakeSpec("PFCSelf", &TA_OneToOnePrjnSpec);
  GpRndTesselPrjnSpec* intra_pfcps = (GpRndTesselPrjnSpec*)prjns->FindMakeSpec("IntraPFC", &TA_GpRndTesselPrjnSpec);
  TesselPrjnSpec* input_pfc = (TesselPrjnSpec*)prjns->FindMakeSpec("Input_PFC", &TA_TesselPrjnSpec);
  PFCLVPrjnSpec* pfc_lv_prjn = (PFCLVPrjnSpec*)prjns->FindMakeSpec("PFC_LV_Prjn", &TA_PFCLVPrjnSpec);

  input_pfc->send_offs.New(1); // this is all it takes!

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  // set bias specs for unit specs
  pfc_units->bias_spec.SetSpec(pfc_bias);
  matrix_units->bias_spec.SetSpec(matrix_bias);

  patch->SetLayerSpec(patchsp); patch->SetUnitSpec(lv_units);
  snc->SetLayerSpec(sncsp); snc->SetUnitSpec(da_units);
  
  snrthal_m->SetLayerSpec(snrthalsp); snrthal_m->SetUnitSpec(snrthal_units);
  matrix_m->SetLayerSpec(matrixsp);   matrix_m->SetUnitSpec(matrix_units);
  pfc_m->SetLayerSpec(pfcmsp);	pfc_m->SetUnitSpec(pfc_units);
  if(out_gate) {
    snrthal_o->SetLayerSpec(snrthalosp); snrthal_o->SetUnitSpec(snrthal_units);
    matrix_o->SetLayerSpec(matrixosp);   matrix_o->SetUnitSpec(matrixo_units);
    pfc_o->SetLayerSpec(pfcosp);	pfc_o->SetUnitSpec(pfc_units);
    matrixo_units->bias_spec.SetSpec(matrix_bias);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,

  net->FindMakePrjn(snc, pvi, onetoone, marker_cons);
  net->FindMakePrjn(snc, lve, onetoone, marker_cons);
  net->FindMakePrjn(snc, lvi, onetoone, marker_cons);
  net->FindMakePrjn(snc, pvr, onetoone, marker_cons);
  net->FindMakePrjn(snc, nv,  onetoone, marker_cons);
  net->FindMakePrjn(snc, patch,  onetoone, marker_cons);

  // patch has same basic connectivity as lve
  net->FindMakePrjn(patch, pvr, onetoone, marker_cons);

  if(patch_new) {
    for(i=0;i<input_lays.size;i++) {
      Layer* il = (Layer*)input_lays[i];
      net->FindMakePrjn(patch, il, fullprjn, lve_cons);
    }
  }

  net->RemovePrjn(matrix_m, vta); // no more vta prjn!
  net->FindMakePrjn(matrix_m, snc, gponetoone, marker_cons);

  net->FindMakePrjn(snrthal_m, matrix_m, gponetoone, marker_cons);
  net->FindMakePrjn(pfc_m, snrthal_m, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_m, snrthal_m, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_m, patch, gponetoone, marker_cons); // for noise

  if(out_gate) {
    net->RemovePrjn(matrix_o, snc); // transiently made that so nuke it if there
    net->FindMakePrjn(matrix_o, vta, fullprjn, marker_cons);
    // output gets from vta, not snc

    net->FindMakePrjn(snrthal_o, matrix_o, gponetoone, marker_cons);
    net->FindMakePrjn(pfc_o, snrthal_o, gponetoone, marker_cons);
    net->FindMakePrjn(matrix_o, snrthal_o, gponetoone, marker_cons);

    net->FindMakePrjn(pfc_o, pfc_m, onetoone, marker_cons);

//     net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);
//     net->FindMakePrjn(matrix_o, pfc_m, gponetoone, mofmpfc_cons);

    net->FindMakeSelfPrjn(pfc_m, pfc_selfps, pfc_self);
    //  net->FindMakeSelfPrjn(pfc_m, intra_pfcps, intra_pfc);

    // this part in particular doesn't make sense for pfc_o only..
    // critics need up reflect updating!
    net->FindMakePrjn(pvr, pfc_m, fullprjn, pvr_cons);
    net->FindMakePrjn(pvi, pfc_m, fullprjn, pvi_cons);
    net->FindMakePrjn(lve, pfc_m, pfc_lv_prjn, lve_cons);
    net->FindMakePrjn(lvi, pfc_m, pfc_lv_prjn, lvi_cons);
    net->FindMakePrjn(nv,  pfc_m, fullprjn, nv_cons);

    net->FindMakePrjn(patch, pfc_m, gponetoone, lve_cons);
  }
  else {			// !out_gate
//     net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);

    net->FindMakeSelfPrjn(pfc_m, pfc_selfps, pfc_self);
    //  net->FindMakeSelfPrjn(pfc, intra_pfcps, intra_pfc);

    net->FindMakePrjn(pvr, pfc_m, fullprjn, pvr_cons);
    net->FindMakePrjn(pvi, pfc_m, fullprjn, pvi_cons);
    net->FindMakePrjn(lve, pfc_m, pfc_lv_prjn, lve_cons);
    net->FindMakePrjn(lvi, pfc_m, pfc_lv_prjn, lvi_cons);
    net->FindMakePrjn(nv,  pfc_m, fullprjn, nv_cons);

    net->FindMakePrjn(patch, pfc_m, gponetoone, lve_cons);
  }

  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];
    if(pfc_m_new) {
      if(nolrn_pfc)
	net->FindMakePrjn(pfc_m, il, input_pfc, topfc_cons);
      else
	net->FindMakePrjn(pfc_m, il, fullprjn, topfc_cons);
    }
    if(matrix_m_new)
      net->FindMakePrjn(matrix_m, il, fullprjn, matrix_cons);
    if(matrix_o_new)
      net->FindMakePrjn(matrix_o, il, fullprjn, matrixo_cons);
  }
  for(i=0;i<hidden_lays.size;i++) {
    Layer* hl = (Layer*)hidden_lays[i];
    if(out_gate) {
      net->FindMakePrjn(hl, pfc_o, fullprjn, fmpfcout_cons);
      net->FindMakePrjn(hl, pfc_m, fullprjn, fmpfcmnt_cons);
    }
    else {
      net->FindMakePrjn(hl, pfc_m, fullprjn, learn_cons);
    }
  }
  if(pfc_m_new && !nolrn_pfc) {
    for(i=0;i<output_lays.size;i++) {
      Layer* ol = (Layer*)output_lays[i];
      net->FindMakePrjn(pfc_m, ol, fullprjn, topfc_cons);
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  // todo: update these values!

  // different PVLV defaults
  lvesp->lv.min_lvi = 0.4f;
  nvsp->nv.da_gain = 0.1f;
  dasp->da.da_gain = 1.0f;
  dasp->da.pv_gain = 0.1f;

  matrixsp->matrix.da_gain = 0.1f;

  // NOT unique: inherit from lve
  patchsp->SetUnique("decay", false);
  patchsp->SetUnique("kwta", false);
  patchsp->SetUnique("inhib_group", true);
  patchsp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
  patchsp->SetUnique("inhib", false);

  // lr sched:
  learn_cons->lrs_value = LeabraConSpec::EXT_REW_STAT;
  learn_cons->lrate_sched.SetSize(2);
  SchedItem* si = (SchedItem*)learn_cons->lrate_sched.FastEl(0);
  si->start_val = 1.0f;
  si = (SchedItem*)learn_cons->lrate_sched.FastEl(1);
  si->start_ctr = 90;
  si->start_val = .1f;

  // slow learning rate on to pfc cons!
  topfc_cons->SetUnique("lrate", true);
  if(nolrn_pfc) {
    topfc_cons->lrate = 0.0f;
    topfc_cons->SetUnique("rnd", true);
    topfc_cons->rnd.var = 0.0f;
  }
  else {
    topfc_cons->lrate = .005f;
    topfc_cons->SetUnique("rnd", false);
    topfc_cons->rnd.var = 0.25f;
  }
  topfc_cons->SetUnique("lmix", true);
  topfc_cons->lmix.hebb = .001f;
  intra_pfc->SetUnique("wt_scale", true);
  intra_pfc->wt_scale.rel = .1f;

  pfc_self->SetUnique("lrate", true);
  pfc_self->lrate = 0.0f;
  pfc_self->SetUnique("rnd", true);
  pfc_self->rnd.mean = 0.9f;
  pfc_self->rnd.var = 0.0f;
  pfc_self->SetUnique("wt_scale", true);
  pfc_self->wt_scale.rel = .05f;

  matrix_cons->SetUnique("lrate", true);
  matrix_cons->lrate = .05f;
  matrix_cons->SetUnique("wt_sig", true);
  matrix_cons->wt_sig.gain = 1.0f;
  matrix_cons->wt_sig.off = 1.0f;

  mfmpfc_cons->SetUnique("wt_scale", true);
  mfmpfc_cons->wt_scale.rel = .02f;
  mfmpfc_cons->SetUnique("lmix", false);

  matrix_bias->SetUnique("lrate", true);
  matrix_bias->lrate = 0.0f;		// default is no bias learning

  matrix_units->g_bar.h = .01f; // old syn dep
  matrix_units->g_bar.a = .03f;
  matrix_units->noise_type = LeabraUnitSpec::NETIN_NOISE;
  matrix_units->noise.var = 0.0002f;
  matrix_units->noise_adapt.trial_fixed = true;
  matrix_units->noise_adapt.k_pos_noise = true;
  matrix_units->noise_adapt.mode = NoiseAdaptSpec::PVLV_LVE;

  if(out_gate) {
    matrixo_units->SetUnique("g_bar", true);
    matrixo_units->g_bar.h = .02f; matrixo_units->g_bar.a = .06f; // note: 2x..
    matrixo_units->SetUnique("noise_type", false);
    matrixo_units->SetUnique("noise", true);
    matrixo_units->noise.var = 0.001f;
    matrixo_units->SetUnique("noise_adapt", true);
    matrixo_units->noise_adapt.trial_fixed = true;
    matrixo_units->noise_adapt.k_pos_noise = true;
    matrixo_units->noise_adapt.mode = NoiseAdaptSpec::PVLV_PVI;
    matrixo_units->SetUnique("matrix_noise", true);
    matrixo_units->matrix_noise.patch_noise = false;
    
    matrixo_cons->SetUnique("lmix", false);
    matrixo_cons->SetUnique("lrate", true);
    matrixo_cons->lrate = .1f;
    matrixo_cons->SetUnique("rnd", true);
    matrixo_cons->rnd.var = .2f;
    matrixo_cons->SetUnique("wt_sig", true);
    matrixo_cons->wt_sig.gain = 1.0f;
    matrixo_cons->wt_sig.off = 1.0f;

    mofmpfc_cons->SetUnique("wt_scale", true);
    mofmpfc_cons->wt_scale.rel = 0.02f; // works better with gp-one-to-one
    mofmpfc_cons->SetUnique("lmix", false);

    snrthalosp->SetUnique("kwta", true);
    snrthalosp->kwta.pct = .25f; // generally works better!
    // inherit the rest from parent guy!
    snrthalosp->SetUnique("inhib", false);
    snrthalosp->SetUnique("inhib_group", false);
    snrthalosp->SetUnique("decay", false);
    snrthalosp->SetUnique("tie_brk", false);
    snrthalosp->SetUnique("ct_inhib_mod", false);

    fmpfcmnt_cons->SetUnique("wt_scale", true);
    fmpfcmnt_cons->wt_scale.rel = 1.0f; // .2 might be better in some cases
    fmpfcout_cons->SetUnique("wt_scale", true);
    fmpfcout_cons->wt_scale.rel = 1.0f; // 2 might be better in some cases
  }

  pfc_units->SetUnique("g_bar", true);
  if(nolrn_pfc)
    pfc_units->g_bar.h = 1.0f;
  else
    pfc_units->g_bar.h = .5f;
  pfc_units->g_bar.a = 2.0f;
  pfc_units->SetUnique("dt", true);
  pfc_units->dt.vm = .1f;	// slower is better..  .1 is even better!

  snrthal_units->dt.vm = .1f;
  snrthal_units->g_bar.l = .8f;
//   if(snrthalsp->snrthal.net_off == 0.0f)
//     snrthal_units->act.gain = 20.0f; // lower gain for net_off = 0
//   else
//     snrthal_units->act.gain = 600.0f;

  // set projection parameters
  topfc->p_con = .4f;
  pfc_selfps->self_con = true;

  // todo: out of date!
  {
    int half_stripes = MAX(n_stripes / 2, 1);
    intra_pfcps->def_p_con = .4f;
    intra_pfcps->recv_gp_n.y = 1;
    intra_pfcps->recv_gp_group.x = half_stripes;
    intra_pfcps->MakeRectangle(half_stripes, 1, 0, 1);
    intra_pfcps->wrap = false;
  }
  
  matrixsp->bg_type = MatrixLayerSpec::MAINT;
  // set these to fix old projects..
  matrixsp->gp_kwta.k_from = KWTASpec::USE_PCT;
  matrixsp->gp_kwta.pct = .25f;
  matrixsp->inhib.type = LeabraInhibSpec::KWTA_INHIB;
  matrixsp->inhib.kwta_pt = .25f;
  matrixsp->UpdateAfterEdit();

  if(out_gate) {
    matrixosp->SetUnique("bg_type", true);
    matrixosp->bg_type = MatrixLayerSpec::OUTPUT;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  int n_lv_u;		// number of pvlv-type units
  if(lvesp->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  else if(lvesp->scalar.rep == ScalarValSpec::GAUSSIAN)
    n_lv_u = 12;
  else
    n_lv_u = 21;

  lay_set_geom(lve, 1); // patch has the per-stripe lve guys basically
  lay_set_geom(lvi, 1);
  lvesp->SetUnique("inhib_group", true);
  lvesp->SetUnique("gp_kwta", true);
  lvisp->SetUnique("inhib_group", false);
  lvisp->SetUnique("gp_kwta", false);
  lvesp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
  lvesp->gp_kwta.k_from = KWTASpec::USE_K;
  lvesp->gp_kwta.k = 1;

  if(patch_new) {
    patch->pos.SetXYZ(vta->pos.x+3, 0, 0);
  }
  if(snc_new) {
    snc->pos.SetXYZ(vta->pos.x+3 + n_lv_u, 4, 0);
  }

  if(patch->un_geom.n != n_lv_u) { patch->un_geom.n = n_lv_u; patch->un_geom.x = n_lv_u; patch->un_geom.y = 1; }

  lay_set_geom(patch, n_stripes);
  lay_set_geom(snc, n_stripes, 1); // one unit

  if(pfc_m_new) {
    pfc_m->pos.SetXYZ(mx_z2 + 1, 0, 2);
    if(nolrn_pfc && (input_lays.size > 0)) {
      Layer* il = (Layer*)input_lays[0];
      pfc_m->un_geom = il->un_geom;
    }
    else {
      pfc_m->un_geom.n = 30; pfc_m->un_geom.x = 5; pfc_m->un_geom.y = 6;
    }
  }
  lay_set_geom(pfc_m, n_stripes);

  if(matrix_m_new) { 
    matrix_m->pos.SetXYZ(mx_z1+1, 0, 1);
    matrix_m->un_geom.n = 28; matrix_m->un_geom.x = 4; matrix_m->un_geom.y = 7;
  }
  lay_set_geom(matrix_m, n_stripes);

  if(snrthal_m_new) {
    snrthal_m->pos.SetXYZ(patch->pos.x + (patch->un_geom.x +1) * patch->gp_geom.x +1, 0, 0);
  }
  lay_set_geom(snrthal_m, n_stripes, 1);

  // this is here, to allow it to get act_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, n_stripes);

  if(out_gate) {
    if(pfc_o_new) {
      pfc_o->pos.z = pfc_m->pos.z; pfc_o->pos.y = pfc_m->pos.y;
      pfc_o->pos.x = pfc_m->pos.x + pfc_m->act_geom.x + 2;
      if(nolrn_pfc && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_o->un_geom = il->un_geom;
      }
      else {
	pfc_o->un_geom = pfc_m->un_geom;
      }
    }
    lay_set_geom(pfc_o, n_stripes);

    if(matrix_o_new) { 
      matrix_o->pos.z = matrix_m->pos.z; matrix_o->pos.y = matrix_m->pos.y;
      matrix_o->pos.x = matrix_m->pos.x + matrix_m->act_geom.x + 2; 
      matrix_o->un_geom.n = 28; matrix_o->un_geom.x = 4; matrix_o->un_geom.y = 7;
    }
    lay_set_geom(matrix_o, n_stripes);

    snrthal_o->un_geom.n = 1;
    if(snrthal_o_new) {
      snrthal_o->pos.SetXYZ(snrthal_m->pos.x + (snrthal_m->gp_geom.x*2)+1, 0, 0);
    }
    lay_set_geom(snrthal_o, n_stripes);
  }

  if(new_bg_laygp) {
    bg_laygp->pos.z = 0;
  }
  if(new_pfc_laygp) {
    pfc_laygp->pos.z = 2;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  PBWM_SetNStripes(net, n_stripes);

  net->LayerPos_Cleanup();

  // move back!
  if(new_bg_laygp || new_pfc_laygp) {
    bg_laygp->pos.z = 0;
    pfc_laygp->pos.z = 2;
    net->RebuildAllViews();	// trigger update
  }

  taMisc::CheckConfigStart(false, false);

  bool ok = patchsp->CheckConfig_Layer(patch, false);
  ok &= sncsp->CheckConfig_Layer(snc, false);
  ok &= pfcmsp->CheckConfig_Layer(pfc_m, false);
  ok &= matrixsp->CheckConfig_Layer(matrix_m, false);
  ok &= snrthalsp->CheckConfig_Layer(snrthal_m, false);
  if(out_gate) {
    ok &= pfcosp->CheckConfig_Layer(pfc_o, false);
    ok &= matrixosp->CheckConfig_Layer(matrix_o, false);
    ok &= snrthalosp->CheckConfig_Layer(snrthal_o, false);
  }

  taMisc::CheckConfigEnd(ok);

  if(!ok) {
    msg =
      "BG/PFC: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg = 
    "BG/PFC configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  for(int j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("PBWM");
  if(edit != NULL) {
    edit->auto_edit = true;
    String subgp;
    subgp = "PFC";
    pfc_units->SelectForEditNm("g_bar", edit, "pfc", subgp,
      "set g_bar.h to 1 for non-learning (localist) PFC, and to .5 for learning PFC");
    pfcmsp->SelectForEditNm("gate", edit, "pfc_m", subgp);
    pfcmsp->SelectForEditNm("learn", edit, "pfc_m", subgp);
    topfc_cons->SelectForEditNm("lrate", edit, "to_pfc", subgp,
	"PFC requires a slower learning rate in general, around .005 if go_learn_base is set to default of .06, otherwise .001 for go_learn_base of 1");
    if(out_gate) {
      fmpfcmnt_cons->SelectForEditNm("wt_scale", edit, "fm_pfc_mnt", subgp,
     "If SNrThal_out kwta k value is low (i.e., few output stripes activated in general), set wt_scale.rel to a lower value to compensate in balance between mnt and out projections");
      fmpfcout_cons->SelectForEditNm("wt_scale", edit, "fm_pfc_out", subgp,
     "If SNrThal_out kwta k value is low (i.e., few output stripes activated in general), set wt_scale.rel to a lower value to compensate in balance between mnt and out projections");
      pfcosp->SelectForEditNm("gp_kwta", edit, "pfc_o", subgp,
     "If SNrThal_out kwta k value is low (i.e., few output stripes activated in general), set dif_act_pct and act_pct to a lower value to compensate in balance between mnt and out projections");
      pfc_self->SelectForEditNm("wt_scale", edit, "pfc_self", subgp);
    }
    ////////////////////////////////
    subgp = "Matrix";
    matrixsp->SelectForEditNm("matrix", edit, "matrix", subgp);
    matrixsp->SelectForEditNm("gate_bias", edit, "matrix", subgp);
    matrixsp->SelectForEditNm("rnd_go", edit, "matrix", subgp);
    matrix_units->SelectForEditNm("noise", edit, "matrix", subgp,
  "For MAINT Matrix, default noise level is 5.0e-5 (.00005)");
    matrix_units->SelectForEditNm("noise_adapt", edit, "matrix", subgp);
    matrix_units->SelectForEditNm("patch_noise", edit, "matrix", subgp);
    if(out_gate) {
      matrixo_units->SelectForEditNm("noise", edit, "matrix_out", subgp,
  "For OUTPUT Matrix, default noise level is (.005)");
      matrixo_units->SelectForEditNm("noise_adapt", edit, "matrix_out", subgp);
      matrixo_units->SelectForEditNm("patch_noise", edit, "matrix_out", subgp);
    }
    matrix_cons->SelectForEditNm("lrate", edit, "matrix", subgp,
   "Default MAINT Matrix lrate is .05");
    if(out_gate) {
      matrixo_cons->SelectForEditNm("lrate", edit, "matrix_out", subgp,
   "Default OUTPUT Matrix lrate is .1");
      matrixo_cons->SelectForEditNm("rnd", edit, "matrix_out", subgp);
//       matrixo_cons->SelectForEditNm("wt_sig", edit, "mtx_out", subgp);
    }
    //    matrix_cons->SelectForEditNm("lmix", edit, "matrix", subgp);
    mfmpfc_cons->SelectForEditNm("wt_scale", edit, "mtx_fm_pfc", subgp,
        "Generally have lower influence of PFC on MAINT Matrix, which is driven more by inputs -- .2 is default");
    ////////////////////////////////
    subgp = "SNrThal";
    snrthalsp->SelectForEditNm("kwta", edit, "snrthal", subgp,
"Set MAINT kwta to a higher percent than OUTPUT kwta -- typically around 75% or so -- depends on how many different things PFC needs to maintain (lower the % for more maintenance demands)");
    if(out_gate) {
      snrthalosp->SelectForEditNm("kwta", edit, "snrthal_out", subgp,
"Set OUTPUT kwta to a lower percent than MAINT kwta -- typically around 25% or so -- depends on how many different things PFC needs to maintain (lower the % for more maintenance demands)");
    }    
    snrthalsp->SelectForEditNm("inhib", edit, "snrthal", subgp,
"Default is KWTA_AVG_INHIB with kwta_pt = .8 -- more competition but with some flexibility from avg-based computation");

//       snrthal_units->SelectForEditNm("g_bar", edit, "snr_thal", subgp);
//       snrthal_units->SelectForEditNm("dt", edit, "snr_thal", subgp);
    snrthalsp->SelectForEditNm("snrthal", edit, "snrthal", subgp);
//     snrthal_units->SelectForEditNm("act", edit, "snrthal", subgp);
    sncsp->SelectForEditNm("snc", edit, "snc", subgp);
  }

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM -- actually saves network specifically");
  }
  return true;
}

bool LeabraWizard::PBWM_Mode(LeabraNetwork* net, PBWMMode mode) {
  if(TestError(!net, "PBWM", "network must be specified and have PBWM already configured on it -- aborting!"))
    return false;

  bool out_gate = true;		// only for out gate case!

  String pvenm = "PVe";  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";
  String vtanm = "VTA";

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  BaseSpec_Group* units = net->FindMakeSpecGp("PFC_BG_Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp("PFC_BG_Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp("PFC_BG_Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp("PFC_BG_Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return false;

  LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* lv_units = (LeabraUnitSpec*)pv_units->FindMakeChild("LVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_LeabraUnitSpec);

  LeabraUnitSpec* pfc_units = (LeabraUnitSpec*)units->FindMakeSpec("PFCUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* matrix_units = (LeabraUnitSpec*)units->FindMakeSpec("MatrixUnits", &TA_MatrixUnitSpec);
  LeabraUnitSpec* snrthal_units = (LeabraUnitSpec*)units->FindMakeSpec("SNrThalUnits", &TA_LeabraUnitSpec);
  if(pfc_units == NULL || matrix_units == NULL) return false;
  MatrixUnitSpec* matrixo_units = NULL;
  if(out_gate) {
    matrixo_units = (MatrixUnitSpec*)matrix_units->FindMakeChild("MatrixOut", &TA_MatrixUnitSpec);
  }

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(!learn_cons) return false;

  LeabraConSpec* pvi_cons = (LeabraConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  LeabraConSpec* pvr_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("PVr", &TA_PVrConSpec);
  LeabraConSpec* lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_PVConSpec);
  LeabraConSpec* lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_PVConSpec);
  LeabraConSpec* nv_cons =  (LeabraConSpec*)pvi_cons->FindMakeChild("NV", &TA_PVConSpec);

  LeabraConSpec* topfc_cons = (LeabraConSpec*)learn_cons->FindMakeChild("ToPFC", &TA_LeabraConSpec);
  if(topfc_cons == NULL) return false;
  LeabraConSpec* intra_pfc = (LeabraConSpec*)topfc_cons->FindMakeChild("IntraPFC", &TA_LeabraConSpec);
  LeabraConSpec* pfc_bias = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCBias", &TA_LeabraBiasSpec);
  MatrixConSpec* matrix_cons = (MatrixConSpec*)learn_cons->FindMakeChild("MatrixCons", &TA_MatrixConSpec);
  MatrixConSpec* mfmpfc_cons = (MatrixConSpec*)matrix_cons->FindMakeChild("MatrixFmPFC", &TA_MatrixConSpec);

  MatrixConSpec* matrixo_cons = NULL;
  MatrixConSpec* mofmpfc_cons = NULL;
  if(out_gate) {
    matrixo_cons = (MatrixConSpec*)matrix_cons->FindMakeChild("Matrix_out", &TA_MatrixConSpec);
    mofmpfc_cons = (MatrixConSpec*)matrixo_cons->FindMakeChild("Matrix_out_FmPFC", &TA_MatrixConSpec);
  }
  MatrixBiasSpec* matrix_bias = (MatrixBiasSpec*)matrix_cons->FindMakeChild("MatrixBias", &TA_MatrixBiasSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  LeabraConSpec* pfc_self = (LeabraConSpec*)cons->FindMakeSpec("PFCSelfCon", &TA_LeabraConSpec);

  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  if(bg_bias == NULL) return false;

  if(pfc_self == NULL || intra_pfc == NULL || matrix_cons == NULL || marker_cons == NULL 
     || matrix_bias == NULL)
    return false;

  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec);
  LViLayerSpec* lvisp = (LViLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec);
  NVLayerSpec* nvsp = (NVLayerSpec*)layers->FindMakeSpec(nvnm + "Layer", &TA_NVLayerSpec);
  PatchLayerSpec* patchsp = (PatchLayerSpec*)lvesp->FindMakeChild("PatchLayer", &TA_PatchLayerSpec);

  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindType(&TA_PVLVDaLayerSpec);
  SNcLayerSpec* sncsp = (SNcLayerSpec*)dasp->FindMakeChild("SNcLayer", &TA_SNcLayerSpec);

  PFCLayerSpec* pfcmsp = (PFCLayerSpec*)layers->FindMakeSpec("PFCLayer", &TA_PFCLayerSpec);
  PFCOutLayerSpec* pfcosp = NULL;
  if(out_gate)
    pfcosp = (PFCOutLayerSpec*)pfcmsp->FindMakeChild("PFCOutLayer", &TA_PFCOutLayerSpec);
  MatrixLayerSpec* matrixsp = (MatrixLayerSpec*)layers->FindMakeSpec("MatrixLayer", &TA_MatrixLayerSpec);
  if(pfcmsp == NULL || matrixsp == NULL) return false;

  MatrixLayerSpec* matrixosp = NULL;
  if(out_gate)
    matrixosp = (MatrixLayerSpec*)matrixsp->FindMakeChild("Matrix_out", &TA_MatrixLayerSpec);

  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snrthalosp = NULL;
  if(out_gate)
    snrthalosp = (SNrThalLayerSpec*)snrthalsp->FindMakeChild("SNrThalOut", &TA_SNrThalLayerSpec);

  if(mode == PROMISCUOUS) {
    lvesp->lv.min_lvi = 0.1f;
    nvsp->nv.da_gain = 1.0f;
    dasp->da.pv_gain = 1.0f;
    dasp->da.da_gain = 1.0f;
    matrixsp->matrix.da_gain = 0.1f;
    
    sncsp->snc.lv_mnt_pv_out= false;

    snrthalsp->snrthal.net_off = 0.2f;
    snrthalsp->snrthal.go_thr = 0.1f;

    matrixsp->gate_bias.SetAllBiases(0.0f);
    matrixsp->UpdateAfterEdit();

    pfcmsp->gate2.out_go_clear = false;
    pfcmsp->gate2.no_empty_out = false;
    pfcmsp->gate.base_gain = 0.5f;
    pfcmsp->UpdateAfterEdit();
  }
  else if(mode == GATE_BIAS) {
    lvesp->lv.min_lvi = 0.4f;
    nvsp->nv.da_gain = 0.1f;
    dasp->da.pv_gain = 0.1f;
    dasp->da.da_gain = 1.0f;
    matrixsp->matrix.da_gain = 0.1f;

    sncsp->snc.lv_mnt_pv_out= true;

    snrthalsp->snrthal.net_off = 0.0f;
    snrthalsp->snrthal.go_thr = 0.5f;
    
    matrixsp->gate_bias.SetAllBiases(1.0f);
    matrixsp->gate_bias.mnt_empty_go = 0.0f;
    matrixsp->UpdateAfterEdit();

    pfcmsp->gate2.out_go_clear = true;
    pfcmsp->UpdateAfterEdit();
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////
//		PBWM Cvt V1
/////////////////////////////////////////////////////////////////////////////

bool LeabraWizard::PBWM_CvtV1_impl(String& proj_str) {
  proj_str.gsub("PatchLayerSpec", "V1PatchLayerSpec");
  proj_str.gsub("SNcLayerSpec", "V1SNcLayerSpec");
  proj_str.gsub("MatrixConSpec", "V1MatrixConSpec");
  proj_str.gsub("MatrixBiasSpec", "V1MatrixBiasSpec");
  proj_str.gsub("MatrixUnitSpec", "V1MatrixUnitSpec");
  proj_str.gsub("MatrixLayerSpec", "V1MatrixLayerSpec");
  proj_str.gsub("SNrThalLayerSpec", "V1SNrThalLayerSpec");
  proj_str.gsub("PFCLayerSpec", "V1PFCLayerSpec");
  proj_str.gsub("PFCOutLayerSpec", "V1PFCOutLayerSpec");
  proj_str.gsub("PFCGateSpec", "V1PFCGateSpec");
  return true;
}

bool LeabraWizard::PBWM_CvtV1File(const String& proj_file_nm, bool load_after) {
  String proj_str;
  fstream istrm;
  istrm.open(proj_file_nm, ios::in);
  if(TestError(!istrm.good(), "PBWM_CvtV1File", "Project file:",proj_file_nm,
	       "could not be opened for reading"))
    return false;
  proj_str.Load_str(istrm);
  istrm.close();

  PBWM_CvtV1_impl(proj_str);

  if(load_after) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    // below is from undo code
    ++taMisc::is_undo_loading;
    proj->Load_String(proj_str);
    taMisc::ProcessEvents();	// get any post-load things *before* turning off undo flag..
    --taMisc::is_undo_loading;
    tabMisc::DelayedFunCall_gui(proj,"RefreshAllViews");
  }
  else {
    fstream ostrm;
    ostrm.open(proj_file_nm, ios::out);
    if(TestError(!ostrm.good(), "PBWM_CvtV1File", "Project file:",proj_file_nm,
		 "could not be opened for writing"))
      return false;
    proj_str.Load_str(ostrm);
    ostrm.close();
  }
  return true;

}

//////////////////////////////
// 	Remove!!!

bool LeabraWizard::PBWM_Remove(LeabraNetwork* net) {
  if(TestError(!net, "PBWM_ToLayerGroup", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_Remove before -- actually saves network specifically");
  }

  net->RemoveUnits();
  net->layers.gp.RemoveName("PBWM_PFC");
  net->layers.gp.RemoveName("PBWM_BG");
  net->layers.gp.RemoveName("PVLV");

  net->specs.gp.RemoveName("PFC_BG_Prjns");
  net->specs.gp.RemoveName("PFC_BG_Layers");
  net->specs.gp.RemoveName("PFC_BG_Cons");
  net->specs.gp.RemoveName("PFC_BG_Units");

  net->CheckSpecs();		// could have nuked dependent specs!

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_Remove after -- actually saves network specifically");
  }
  return true;
}


////////////////////////////////////////////////////////////////////////
//	Following is OLD (now obsolete) "Version 1" of PBWM


////////////////////////////////////////////////////////////////////
//	Patch/Striosomes and SNc

void V1PatchLayerSpec::Initialize() {
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
}

void V1PatchLayerSpec::Send_LVeToMatrix(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     const float snd_val = u->act_eq;
     for(int g=0; g<u->send.size; g++) {
       LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
       LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
       if(tol->lesioned())	continue;
       LeabraLayerSpec* tols = (LeabraLayerSpec*)tol->GetLayerSpec();
       if(!((tols->GetTypeDef() == &TA_V1MatrixLayerSpec))) continue; // only to matrix
       for(int j=0;j<send_gp->size; j++) {
	 ((LeabraUnit*)send_gp->Un(j))->misc_1 = snd_val;
       }
     }
     );
}

void V1PatchLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  ScalarValLayerSpec::Compute_CycleStats(lay, net);
  // do NOT report lvi value!
  Send_LVeToMatrix(lay, net);
}


void V1SNcLayerSpec::Initialize() {
  stripe_da_gain = 1.0f;
}

void V1SNcLayerSpec::HelpConfig() {
  String help = "V1SNcLayerSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from PVLV layers.\n\
 - No Learning\n\
 \nV1SNcLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons marked with a MarkerConSpec from PVLV LVe, LVi, PVi, PVe, PVr, NV (all!)\n\
 - Recv cons marked with a MarkerConSpec from Patch layer, same number of units\n\
 - Send cons marked with a MarkerConSpec to Matrix layers\n\
 - This layer must be after recv layers in list of layers\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool V1SNcLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;
  if(lay->CheckError(lay->units.gp.size == 0, quiet, rval,
		     "requires unit groups, one per associated stripe")) {
    return false;
  }

  int patch_prjn_idx;
  LeabraLayer* patch_lay = FindLayerFmSpec(lay, patch_prjn_idx, &TA_V1PatchLayerSpec);
  if(lay->CheckError(!patch_lay, quiet, rval,
		"did not find Patch layer to get Da from!")) {
    return false;
  }

  if(lay->CheckError(lay->units.gp.size != patch_lay->units.gp.size, quiet, rval,
		     "our number of unit groups should equal those in the patch layer")) {
    return false;
  }

  return true;
}  

void V1SNcLayerSpec::Compute_Da(LeabraLayer* lay, LeabraNetwork* net) {
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

  int patch_prjn_idx;
  LeabraLayer* patch_lay = FindLayerFmSpec(lay, patch_prjn_idx, &TA_V1PatchLayerSpec);
  V1PatchLayerSpec* patch_sp = (V1PatchLayerSpec*)patch_lay->GetLayerSpec();

  // this is the global da from LVe, not stripe-specific
  float lv_da = lve_sp->Compute_LVDa(lve_lay, lvi_lay, net);

  // nv only contributes to lv, not pv..
  if(nv_lay) {
    lv_da += nvls->Compute_NVDa(nv_lay, net);
  }
  float pv_da = da.pv_gain * pvils->Compute_PVDa(pvi_lay, net);
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good

  Unit_Group* lvi_ugp;
  if(lvi_lay->units.gp.size > 0)
    lvi_ugp = (Unit_Group*)lvi_lay->units.gp[0];
  else
    lvi_ugp = (Unit_Group*)&(lvi_lay->units);
  // assuming only one lvi group of units for time being

  float da_norm = 1.0f / (1.0f + stripe_da_gain);

  lay->dav = 0.0f;
  for(int gi=0; gi < lay->units.gp.size; gi++) {
    Unit_Group* snc_ugp = (Unit_Group*)lay->units.gp[gi];
    Unit_Group* patch_ugp = (Unit_Group*)patch_lay->units.gp[gi];
    LeabraUnit* snc_u = (LeabraUnit*)snc_ugp->FastEl(0);
//    LeabraUnit* patch_u = (LeabraUnit*)patch_ugp->FastEl(0);

    float str_da = patch_sp->Compute_LVDa_ugp(patch_ugp, lvi_ugp, net); // per stripe
    if(er_avail) {
      snc_u->dav = da.da_gain * pv_da;
      if(da.add_pv_lv) {
	snc_u->dav += da.da_gain * da_norm * (lv_da + stripe_da_gain * str_da);
      }
    }
    else {
      snc_u->dav = da.da_gain * da_norm * (lv_da + stripe_da_gain * str_da);
      // simple additive model here (avg)
    }
    snc_u->ext = da.tonic_da + snc_u->dav;
    snc_u->act_eq = snc_u->act_nd = snc_u->act = snc_u->net = snc_u->ext;
    lay->dav += snc_u->dav;
  }
  lay->dav /= (float)lay->units.gp.size; // integrated average -- not really used
}


//////////////////////////////////
//	V1MatrixConSpec		//
//////////////////////////////////

void V1MatrixConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
  wt_sig.off = 1.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  matrix_rule = MAINT;
}

void V1MatrixConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  //  lmix.err_sb = false;
}

//////////////////////////////////////////
// 	Matrix Unit Spec		//
//////////////////////////////////////////

void V1MatrixBiasSpec::Initialize() {
  SetUnique("rnd", true);
  SetUnique("wt_limits", true);
  SetUnique("wt_scale", true);
  SetUnique("wt_scale_init", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;
  wt_limits.min = -1.0f;
  wt_limits.max = 5.0f;
  wt_limits.sym = false;
  wt_limits.type = WeightLimits::NONE;
  dwt_thresh = .1f;

  matrix_rule = MAINT;

  SetUnique("lrate", true);
  lrate = 0.0f;			// default is no lrate
}

void V1MatrixNoiseSpec::Initialize() {
  patch_noise = true;
  nogo_thr = 50;
  nogo_gain = 0.0f;
}

void V1MatrixUnitSpec::Initialize() {
  SetUnique("bias_spec", true);
  bias_spec.SetBaseType(&TA_LeabraConSpec); // can't be bias spec b/c matrix derives from con
  bias_spec.type = &TA_V1MatrixBiasSpec;
  SetUnique("g_bar", true);
  g_bar.a = .03f;
  g_bar.h = .01f;

  SetUnique("act", true);
  act.i_thr = ActFunSpec::NO_AH; // key for dopamine effects

  SetUnique("noise_type", true);
  noise_type = NETIN_NOISE;
  SetUnique("noise", true);
  noise.var = 5.0e-5f;
  SetUnique("noise_adapt", true);
  noise_adapt.trial_fixed = true;
  noise_adapt.mode = NoiseAdaptSpec::PVLV_LVE;

  freeze_net = true;
}

void V1MatrixUnitSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void V1MatrixUnitSpec::InitLinks() {
  inherited::InitLinks();
  bias_spec.type = &TA_V1MatrixBiasSpec;
}

void V1MatrixUnitSpec::Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  if(lay->hard_clamped) return;
  
  float eff_dt = dt.net;
  // this is the new part of the code: getting the effective dt relative to the freeze net fun
  if(freeze_net) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->spec.SPtr();
    if(ls->InheritsFrom(&TA_V1MatrixLayerSpec)) {
      V1MatrixLayerSpec* mls = (V1MatrixLayerSpec*)lay->spec.SPtr();
      if(mls->bg_type == V1MatrixLayerSpec::MAINT) {
	if(net->phase_no == 2) eff_dt = 0.0f;
      }
      else {
	if(net->phase_no >= 1) eff_dt = 0.0f;
      }
    }
    else {			// Xmatrix
      if(net->phase_no >= 1) eff_dt = 0.0f; // freeze in plus and beyond
    }
  }

  // remainder below here should be same as original, except dt.net -> eff_dt
  if(net->inhib_cons_used) {
    u->g_i_raw += u->g_i_delta;
    u->gc.i = u->g_i_raw;
    u->gc.i = u->prv_g_i + eff_dt * (u->gc.i - u->prv_g_i);
    u->prv_g_i = u->gc.i;
  }

  u->net_raw += u->net_delta;
  float tot_net = (u->bias_scale * u->bias.OwnCn(0)->wt) + u->net_raw;
  if(u->HasExtFlag(Unit::EXT)) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    tot_net += u->ext * ls->clamp.gain;
  }

  u->net_delta = 0.0f;	// clear for next use
  u->g_i_delta = 0.0f;	// clear for next use

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = tot_net;		// store directly for integration
    Compute_NetinInteg_Spike(u,net);
  }
  else {
    u->net = u->prv_net + eff_dt * (tot_net - u->prv_net);
    u->prv_net = u->net;
  }

  u->i_thr = Compute_IThresh(u, net);
}

float V1MatrixUnitSpec::Compute_Noise(LeabraUnit* u, LeabraNetwork* net) {
  float noise_amp = 1.0f;		// noise amplitude multiplier
  LeabraUnit_Group* mugp = u->own_ugp();
  if(matrix_noise.patch_noise) {
    noise_amp = (1.0f - (noise_adapt.min_pct_c * u->misc_1)); // lve value on patch is in misc_1
  }
  else {
    if(noise_adapt.mode == NoiseAdaptSpec::SCHED_CYCLES) {
      noise_amp = noise_sched.GetVal(net->cycle);
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::SCHED_EPOCHS) {
      noise_amp = noise_sched.GetVal(net->epoch);
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_PVI) {
      noise_amp = (1.0f - (noise_adapt.min_pct_c * net->pvlv_pvi));
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_LVE) {
      noise_amp = (1.0f - (noise_adapt.min_pct_c * net->pvlv_lve));
    }
    else if(noise_adapt.mode == NoiseAdaptSpec::PVLV_MIN) {
      float pvlv_val = MIN(net->pvlv_pvi, net->pvlv_lve);
      noise_amp = (1.0f - (noise_adapt.min_pct_c * pvlv_val));
    }
  }

  int nogos = (int)fabs((float)mugp->misc_state);
  if(nogos > matrix_noise.nogo_thr) {
    noise_amp *= matrix_noise.nogo_gain * (float)(nogos - matrix_noise.nogo_thr);
  }

  float rval = 0.0f;
  if(noise_adapt.trial_fixed) {
    rval = u->noise; // u->noise is trial-level generated value
  }
  else {
    rval = noise.Gen();
    u->noise = rval;
  }

  return noise_amp * rval;
}



//////////////////////////////////
//	Matrix Layer Spec	//
//////////////////////////////////

void V1MatrixMiscSpec::Initialize() {
  neg_da_bl = 0.0002f;
  neg_gain = 1.5f;
  perf_gain = 0.0f;
  snr_err_da = 1.0f;
  no_snr_mod = false;
}

void V1ContrastSpec::Initialize() {
  gain = 1.0f;
  one_val = true;
  contrast = .5f;
  go_p = .5f;
  go_n = .5f;
  nogo_p = .5f;
  nogo_n = .5f;
}

void V1MatrixGoNogoGainSpec::Initialize() {
  on = false;
  go_p = go_n = nogo_p = nogo_n = 1.0f;
}

void V1MatrixRndGoSpec::Initialize() {
  nogo_thr = 50;
  nogo_p = .1f;
  nogo_da = 10.0f;
}

/////////////////////////////////////////////////////

void V1MatrixLayerSpec::Initialize() {
  //  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  //  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .25f;
  //  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .25f;

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  bg_type = MAINT;
}

void V1MatrixLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(contrast.one_val) {
    contrast.go_p = contrast.go_n = contrast.nogo_p = contrast.nogo_n = contrast.contrast;
    // set them all
  }
}

void V1MatrixLayerSpec::Defaults() {
  inherited::Defaults();
  matrix.Defaults();
  contrast.Defaults();
  Initialize();
}

void V1MatrixLayerSpec::HelpConfig() {
  String help = "V1MatrixLayerSpec Computation:\n\
 There are 2 types of units arranged sequentially in the following order within each\
 stripe whose firing affects the gating status of the corresponding stripe in PFC:\n\
 - GO unit = toggle maintenance of units in PFC: this is the direct pathway\n\
 - NOGO unit = maintain existing state in PFC (i.e. do nothing): this is the indirect pathway\n\
 \nV1MatrixLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Units must have a V1MatrixUnitSpec and must recv from PVLVDaLayerSpec layer\
 (calld DA typically) to get da modulation for learning signal\n\
 - Recv connections need to be V1MatrixConSpec as learning occurs based on the da-signal\
 on the matrix units.\n\
 - This layer must be after DaLayers in list of layers\n\
 - Units must be organized into groups (stipes) of same number as PFC";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool V1MatrixLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space!")) {
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(!us->InheritsFrom(TA_V1MatrixUnitSpec), quiet, rval,
		"UnitSpec must be V1MatrixUnitSpec!")) {
    return false;
  }
  if(lay->CheckError(us->act.avg_dt <= 0.0f, quiet, rval,
		"requires UnitSpec act.avg_dt > 0, I just set it to .005 for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.005f;
  }
  if(lay->CheckError(us->act.i_thr != ActFunSpec::NO_AH, quiet, rval,
		"requires UnitSpec act.i_thr = NO_AH to support proper da modulation, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.i_thr = ActFunSpec::NO_AH; // key for dopamine effects
  }

  us->SetUnique("g_bar", true);

  // must have these not initialized every trial!
  if(lay->CheckError(us->hyst.init, quiet, rval,
		"requires UnitSpec hyst.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
  }
  if(lay->CheckError(us->acc.init, quiet, rval,
		"requires UnitSpec acc.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("acc", true);
    us->acc.init = false;
  }
  us->UpdateAfterEdit();

  LeabraBiasSpec* bs = (LeabraBiasSpec*)us->bias_spec.SPtr();
  if(lay->CheckError(bs == NULL, quiet, rval,
		"Error: null bias spec in unit spec", us->name)) {
    return false;
  }

  LeabraLayer* da_lay = NULL;
  LeabraLayer* snr_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) // self projection, skip it
      continue;
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(fmlay->spec.SPtr()->InheritsFrom(TA_PVLVDaLayerSpec)) da_lay = fmlay;
      if(fmlay->spec.SPtr()->InheritsFrom(TA_V1SNrThalLayerSpec)) snr_lay = fmlay;
      continue;
    }
    V1MatrixConSpec* cs = (V1MatrixConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_V1MatrixConSpec), quiet, rval,
		  "Receiving connections must be of type V1MatrixConSpec!")) {
      return false;
    }
    if(lay->CheckError(cs->wt_limits.sym != false, quiet, rval,
		  "requires recv connections to have wt_limits.sym=false, I just set it for you in spec:",
		  cs->name,"(make sure this is appropriate for all layers that use this spec!)")) {
      cs->SetUnique("wt_limits", true);
      cs->wt_limits.sym = false;
    }
    if(bg_type == V1MatrixLayerSpec::OUTPUT) {
      if(lay->CheckError((cs->matrix_rule != V1MatrixConSpec::OUTPUT), quiet, rval,
		    "OUTPUT BG requires V1MatrixConSpec matrix_rule of OUTPUT type, I just set it for you in spec:",
		    cs->name,"(make sure this is appropriate for all layers that use this spec!)")) {
	cs->SetUnique("matrix_rule", true);
	cs->matrix_rule = V1MatrixConSpec::OUTPUT;
      }
    }
    else {			// pfc
      if(lay->CheckError((cs->matrix_rule == V1MatrixConSpec::OUTPUT), quiet, rval,
		    "BG_pfc requires V1MatrixConSpec matrix_rule of MAINT type, I just set it for you in spec:",
		    cs->name,"(make sure this is appropriate for all layers that use this spec!)")) {
	cs->SetUnique("matrix_rule", true);
	cs->matrix_rule = V1MatrixConSpec::MAINT;
      }
    }
  }
  if(lay->CheckError(da_lay == NULL, quiet, rval,
		"Could not find DA layer (PVLVDaLayerSpec) -- must receive MarkerConSpec projection from one!")) {
    return false;
  }
  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int daidx = lay->own_net->layers.FindLeafEl(da_lay);
  lay->CheckError(daidx > myidx, quiet, rval,
		  "DA layer (PVLVDaLayerspec) layer must be *before* this layer in list of layers -- it is now after, won't work!");

  if(lay->CheckError(snr_lay == NULL, quiet, rval,
		"Could not find SNrThal layer -- must receive MarkerConSpec projection from one!")) {
    return false;
  }
  return true;
}

void V1MatrixLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  UNIT_GP_ITR(lay, 
	      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
	      u->misc_1 = 0.0f;
	      );
  LabelUnits(lay);
}

void V1MatrixLayerSpec::Compute_DaMod_NoContrast(LeabraUnit* u, float dav, int go_no) {
  if(go_no == (int)V1PFCGateSpec::GATE_GO) {	// we are a GO gate unit
    if(dav >= 0.0f)  { 
      u->vcb.g_h = dav;
      u->vcb.g_a = 0.0f;
    }
    else {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = -dav;
    }
  }
  else {			// we are a NOGO gate unit
    if(dav >= 0.0f) {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = dav;
    }
    else {
      u->vcb.g_h = -dav;
      u->vcb.g_a = 0.0f;
    }
  }
}


void V1MatrixLayerSpec::Compute_DaMod_Contrast(LeabraUnit* u, float dav, float act_val, int go_no) {
  if(go_no == (int)V1PFCGateSpec::GATE_GO) {	// we are a GO gate unit
    if(dav >= 0.0f)  { 
      u->vcb.g_h = contrast.gain * dav * ((1.0f - contrast.go_p) + (contrast.go_p * act_val));
      if(go_nogo_gain.on) u->vcb.g_h *= go_nogo_gain.go_p;
      u->vcb.g_a = 0.0f;
    }
    else {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = -matrix.neg_gain * contrast.gain * dav * ((1.0f - contrast.go_n) + (contrast.go_n * act_val));
      if(go_nogo_gain.on) u->vcb.g_a *= go_nogo_gain.go_n;
    }
  }
  else {			// we are a NOGO gate unit
    if(dav >= 0.0f) {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = contrast.gain * dav * ((1.0f - contrast.nogo_p) + (contrast.nogo_p * act_val));
      if(go_nogo_gain.on) u->vcb.g_a *= go_nogo_gain.nogo_p;
    }
    else {
      u->vcb.g_h = -matrix.neg_gain * contrast.gain * dav * ((1.0f - contrast.nogo_n) + (contrast.nogo_n * act_val));
      if(go_nogo_gain.on) u->vcb.g_h *= go_nogo_gain.nogo_n;
      u->vcb.g_a = 0.0f;
    }
  }
}

void V1MatrixLayerSpec::Compute_DaTonicMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork*) {
  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_PVLVDaLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.SPtr();
  float dav = contrast.gain * dals->da.tonic_da;
  int idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    V1PFCGateSpec::GateSignal go_no = (V1PFCGateSpec::GateSignal)(idx % 2); // GO = 0, NOGO = 1
    u->dav = dav;		// accurately reflect tonic modulation!
    Compute_DaMod_NoContrast(u, dav, go_no);
    idx++;
  }
}

void V1MatrixLayerSpec::Compute_DaPerfMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork*) {
  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_PVLVDaLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.SPtr();
  float tonic_da = dals->da.tonic_da;

  int idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    V1PFCGateSpec::GateSignal go_no = (V1PFCGateSpec::GateSignal)(idx % 2); // GO = 0, NOGO = 1

    // need to separate out the tonic and non-tonic because tonic contributes with contrast.gain
    // but perf is down-modulated by matrix.perf_gain..
    float non_tonic = u->dav - tonic_da;
    float dav = contrast.gain * (tonic_da + matrix.perf_gain * non_tonic);
    Compute_DaMod_NoContrast(u, dav, go_no);
    idx++;
  }
}

void V1MatrixLayerSpec::Compute_DaLearnMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork* net) {
  int snr_prjn_idx = 0;
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_V1SNrThalLayerSpec);

  V1PFCGateSpec::GateSignal gate_sig = (V1PFCGateSpec::GateSignal)mugp->misc_state2;

  int idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    V1PFCGateSpec::GateSignal go_no = (V1PFCGateSpec::GateSignal)(idx % 2); // GO = 0, NOGO = 1
    LeabraRecvCons* snrcg = (LeabraRecvCons*)u->recv[snr_prjn_idx];
    LeabraUnit* snrsu = (LeabraUnit*)snrcg->Un(0);

    float gating_act = 0.0f;	// activity of the unit during the gating action firing
    float snrthal_act = 0.0f;	// activity of the snrthal during gating action firing
    if(net->phase_no == 3) 	{ gating_act = u->act_m2; snrthal_act = snrsu->act_m2; } // TRANS
    else if(net->phase_no == 2) { gating_act = u->act_p;  snrthal_act = snrsu->act_p; }	// GOGO
    else if(net->phase_no == 1)	{ gating_act = u->act_m;  snrthal_act = snrsu->act_m; }	// OUTPUT

    if(gate_sig == V1PFCGateSpec::GATE_NOGO)	// if didn't actually GO (act > thresh), then no learning!
      snrthal_act = 0.0f;
    
    if(matrix.no_snr_mod)	// disable!
      snrthal_act = 1.0f;

    float dav = snrthal_act * u->dav - matrix.neg_da_bl; // da is modulated by snrthal; sub baseline
    if(mugp->misc_state1 == V1PFCGateSpec::NOGO_RND_GO) {
      dav += rnd_go.nogo_da; 
    }

    if((gate_sig == V1PFCGateSpec::GATE_NOGO) && (net->phase_no == 1) &&
       snr_lay->HasExtFlag(Unit::COMP) && (snrsu->targ > .5f)) {
      //  output gating -- get plus-phase err signal if avail, as COMP input to snr layer
      dav += matrix.snr_err_da;
    }

    u->dav = dav;		// make it show up in display
    Compute_DaMod_Contrast(u, dav, gating_act, go_no);
    idx++;
  }
}

void V1MatrixLayerSpec::Compute_MotorGate(LeabraLayer* lay, LeabraNetwork*) {
  int snr_prjn_idx = 0;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_V1SNrThalLayerSpec);
  V1SNrThalLayerSpec* snrthalsp = (V1SNrThalLayerSpec*)snrthal_lay->spec.SPtr();

  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
    LeabraUnit* snru = (LeabraUnit*)snrgp->Leaf(0);

    V1PFCGateSpec::GateSignal gate_sig = V1PFCGateSpec::GATE_NOGO;
    if(snru->act_eq > snrthalsp->snrthal.go_thr) {
      gate_sig = V1PFCGateSpec::GATE_GO;
      mugp->misc_state = 0;
    }
    else {
      mugp->misc_state++;
    }
    snrgp->misc_state2 = mugp->misc_state2 = gate_sig; // store the raw gating signal itself
  }
}

void V1MatrixLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_ApplyInhib(lay, net);
  
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    if(bg_type == V1MatrixLayerSpec::MAINT) {
      if(net->phase_no == 0)
	Compute_DaTonicMod(lay, mugp, net);
      else if(net->phase_no == 1)
	Compute_DaPerfMod(lay, mugp, net);
      else if(net->phase_no == 2)
	Compute_DaLearnMod(lay, mugp, net);
    }
    else {			// OUTPUT
      if(net->phase_no == 0)
	Compute_DaTonicMod(lay, mugp, net);
      else if(net->phase_no == 1)
	Compute_DaLearnMod(lay, mugp, net);
      // don't do anything in 2nd plus!
    }
  }
}

void V1MatrixLayerSpec::Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    if(mugp->misc_state1 >= V1PFCGateSpec::NOGO_RND_GO)
      mugp->misc_state1 = V1PFCGateSpec::EMPTY_GO;
  }
}

void V1MatrixLayerSpec::Compute_NoGoRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];

//     float rval = Random::ZeroOne(); // this is to match old code, just for debugging.

    if((int)fabs((float)mugp->misc_state) > rnd_go.nogo_thr) {
      if(Random::ZeroOne() < rnd_go.nogo_p) {
	mugp->misc_state1 = V1PFCGateSpec::NOGO_RND_GO;
      }
    }
  }
}

void V1MatrixLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0)
    Compute_ClearRndGo(lay, net);

  if(bg_type == V1MatrixLayerSpec::MAINT) {
    if(net->phase_no == 1) {
      Compute_NoGoRndGo(lay, net);
    }
  }
  else {			// OUTPUT
    if(net->phase_no == 0) {
      Compute_NoGoRndGo(lay, net);
    }
  }

  inherited::Compute_HardClamp(lay, net);
}

void V1MatrixLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(bg_type == V1MatrixLayerSpec::OUTPUT) {
    if(net->phase_no == 0)
      Compute_MotorGate(lay, net);
  }
}

bool V1MatrixLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(bg_type == V1MatrixLayerSpec::OUTPUT) {
    return true;
  }
  return false;
}

bool V1MatrixLayerSpec::Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(bg_type == V1MatrixLayerSpec::MAINT) {
    return true;
  }
  return false;
}

bool V1MatrixLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return false;
}

void V1MatrixLayerSpec::LabelUnits_impl(Unit_Group* ugp) {
  for(int i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    V1PFCGateSpec::GateSignal go_no = (V1PFCGateSpec::GateSignal)(i % 2); // GO = 0, NOGO = 1
    if(go_no == V1PFCGateSpec::GATE_GO)
      u->name = "Go";
    else
      u->name = "No";
  }
}

void V1MatrixLayerSpec::LabelUnits(LeabraLayer* lay) {
  UNIT_GP_ITR(lay, LabelUnits_impl(ugp); );
}

//////////////////////////////////
//	SNrThal Layer Spec	//
//////////////////////////////////

void V1SNrThalMiscSpec::Initialize() {
  go_thr = 0.1f;
  net_off = 0.2f;
  rnd_go_inc = 0.2f;
}

void V1SNrThalLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_PCT;
  kwta.pct = .75f;
  SetUnique("tie_brk", true);	// turn on tie breaking by default
  tie_brk.on = true;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void V1SNrThalLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void V1SNrThalLayerSpec::HelpConfig() {
  String help = "V1SNrThalLayerSpec Computation:\n\
 - act of unit(s) = act_dif of unit(s) in reward integration layer we recv from\n\
 - da is dynamically computed in plus phaes and sent all layers that recv from us\n\
 - No Learning\n\
 \nV1SNrThalLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Single recv connection marked with a MarkerConSpec from reward integration layer\
     (computes expectations and actual reward signals)\n\
 - This layer must be after corresp. reward integration layer in list of layers\n\
     (da signal from this layer put directly into da var on units)\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool V1SNrThalLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  // must have the appropriate ranges for unit specs..
  //  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  // check recv connection
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_V1MatrixLayerSpec);

  if(lay->CheckError(matrix_lay == NULL, quiet, rval,
		"did not find Matrix layer to recv from!")) {
    return false;
  }

  if(lay->CheckError(matrix_lay->units.gp.size != lay->units.gp.size, quiet, rval,
		"MatrixLayer unit groups must = SNrThalLayer unit groups!")) {
    lay->unit_groups = true;
    lay->gp_geom.n = matrix_lay->units.gp.size;
    return false;
  }

  return true;
}

void V1SNrThalLayerSpec::Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net) {
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_V1MatrixLayerSpec);
//  V1MatrixLayerSpec* mls = (V1MatrixLayerSpec*)matrix_lay->spec.SPtr();

  float net_off_rescale = 1.0f / (1.0f + snrthal.net_off);

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    float gonogo = 0.0f;
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    V1MatrixUnitSpec* us = (V1MatrixUnitSpec*)matrix_lay->unit_spec.SPtr();
    if((mugp->size > 0) && (mugp->acts.max >= us->opt_thresh.send)) {
      float sum_go = 0.0f;
      float sum_nogo = 0.0f;
      for(int i=0;i<mugp->size;i++) {
	LeabraUnit* u = (LeabraUnit*)mugp->FastEl(i);
	V1PFCGateSpec::GateSignal go_no = (V1PFCGateSpec::GateSignal)(i % 2); // GO = 0, NOGO = 1
	if(go_no == V1PFCGateSpec::GATE_GO)
	  sum_go += u->act_eq;
	else
	  sum_nogo += u->act_eq;
      }
      if(sum_go + sum_nogo > 0.0f) {
	gonogo = (sum_go - sum_nogo) / (sum_go + sum_nogo);
      }
      if(mugp->misc_state1 >= V1PFCGateSpec::NOGO_RND_GO) {
	gonogo += snrthal.rnd_go_inc;
	if(gonogo > 1.0f) gonogo = 1.0f;
      }
    }

    float net_eff = net_off_rescale * (gonogo + snrthal.net_off);

    for(int i=0;i<rugp->size;i++) {
      LeabraUnit* ru = (LeabraUnit*)rugp->FastEl(i);
      ru->net = net_eff;
      ru->i_thr = ru->Compute_IThresh(net);
    }
  }
}

void V1SNrThalLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  // note: this no longer has dt.net in effect here!! hopefully not a huge diff..
  Compute_GoNogoNet(lay, net);
  inherited::Compute_NetinStats(lay, net);
}


//////////////////////////////////
//	PFC Layer Spec		//
//////////////////////////////////

void V1PFCGateSpec::Initialize() {
  off_accom = 0.0f;
  out_gate_learn_mod = false;
  allow_clamp = false;
}

void V1PFCLayerSpec::Initialize() {
  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .15f;
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;
  SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.1f;
  decay.clamp_phase2 = false;	// this is the one exception!
}

void V1PFCLayerSpec::Defaults() {
  inherited::Defaults();
  gate.Defaults();
  Initialize();
}

void V1PFCLayerSpec::HelpConfig() {
  String help = "V1PFCLayerSpec Computation:\n\
 The PFC maintains activation over time (activation-based working memory) via\
 excitatory intracelluar ionic mechanisms (implemented via the hysteresis channels, gc.h),\
 and excitatory self-connections. These ion channels are toggled on and off via units in the\
 V1SNrThalLayerSpec layer, which are themsepves driven by V1MatrixLayerSpec units,\
 which are in turn trained up by dynamic dopamine changes computed by the PVLV system.\
 Updating occurs at the end of the 1st plus phase --- if a gating signal was activated, any previous ion\
 current is turned off, and the units are allowed to settle into a new state in the 2nd plus (update) --\
 then the ion channels are activated in proportion to activations at the end of this 2nd phase.\n\
 \nV1PFCLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Units must recv MarkerConSpec from V1SNrThalLayerSpec layer for gating\n\
 - This layer must be after V1SNrThalLayerSpec layer in list of layers\n\
 - Units must be organized into groups corresponding to the matrix groups (stripes).";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool V1PFCLayerSpec::CheckConfig_Layer(Layer* ly,  bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  if(lay->CheckError(net->phase_order == LeabraNetwork::MINUS_PLUS, quiet, rval,
		"requires LeabraNetwork phase_oder = MINUS_PLUS_PLUS, I just set it for you")) {
    net->phase_order = LeabraNetwork::MINUS_PLUS_PLUS;
  }

  if(lay->CheckError(net->min_cycles_phase2 < 35, quiet, rval,
		"requires LeabraNetwork min_cycles_phase2 >= 35, I just set it for you")) {
    net->min_cycles_phase2 = 35;
  }

  if(lay->CheckError(net->sequence_init != LeabraNetwork::DO_NOTHING, quiet, rval,
		"requires network sequence_init = DO_NOTHING, I just set it for you")) {
    net->sequence_init = LeabraNetwork::DO_NOTHING;
  }

  if(lay->CheckError(gate.out_gate_learn_mod, quiet, rval,
		"out_gate_learn_mod is not currently functional due to conflicts with the threading system -- sorry!  to avoid further warnings, please it is now turned off for time being")) {
    gate.out_gate_learn_mod = false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  if(lay->CheckError(us->act.avg_dt <= 0.0f, quiet, rval,
		"requires UnitSpec act.avg_dt > 0, I just set it to .005 for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.005f;
  }
  us->SetUnique("g_bar", true);
  if(lay->CheckError(us->hyst.init, quiet, rval,
		"requires UnitSpec hyst.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
  }
  if(lay->CheckError(us->acc.init, quiet, rval,
		"requires UnitSpec acc.init = false, I just set it for you in spec:",
		us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("acc", true);
    us->acc.init = false;
  }

  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(lay->CheckError(fmlay == NULL, quiet, rval,
		  "null from layer in recv projection:", (String)g)) {
      return false;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->InheritsFrom(TA_MarkerConSpec)) continue;
    // could check the conspec parameters here..
  }

  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_V1SNrThalLayerSpec);
  if(lay->CheckError(snrthal_lay == NULL, quiet, rval,
		"no projection from SNrThal Layer found: must have MarkerConSpec!")) {
    return false;
  }
  if(lay->CheckError(snrthal_lay->units.gp.size != lay->units.gp.size, quiet, rval,
		"Gating Layer unit groups must = PFCLayer unit groups!")) {
    snrthal_lay->unit_groups = true;
    snrthal_lay->gp_geom.n = lay->units.gp.size;
    return false;
  }

  return true;
}

void V1PFCLayerSpec::Compute_MaintUpdt_ugp(LeabraUnit_Group* ugp, MaintUpdtAct updt_act, LeabraLayer* lay, LeabraNetwork* net) {
  if(updt_act == NO_UPDT) return;
  for(int j=0;j<ugp->size;j++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(j);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(updt_act == STORE) {
      u->vcb.g_h = u->maint_h = u->act_eq;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    else if(updt_act == CLEAR) {
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = gate.off_accom * u->vcb.g_h;
      u->vcb.g_h = u->maint_h = 0.0f;
    }
    else if(updt_act == RESTORE) {
      u->vcb.g_h = u->act_eq = u->maint_h;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    else if(updt_act == TMP_STORE) {
      u->vcb.g_h = u->act_eq;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    else if(updt_act == TMP_CLEAR) {
      u->vcb.g_h = 0.0f;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    us->Compute_Conduct(u, net); // update displayed conductances!
  }
  if(updt_act == STORE) ugp->misc_state = 1;
  else if(updt_act == CLEAR) ugp->misc_state = 0;
}

void V1PFCLayerSpec::Compute_TmpClear(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    Compute_MaintUpdt_ugp(ugp, TMP_CLEAR, lay, net); // temporary clear for trans input!
  }
}

void V1PFCLayerSpec::Compute_MaintUpdt(MaintUpdtAct updt_act, LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    Compute_MaintUpdt_ugp(ugp, updt_act, lay, net);
  }
}

void V1PFCLayerSpec::Compute_GatingGOGO(LeabraLayer* lay, LeabraNetwork* net) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_V1SNrThalLayerSpec);
  V1SNrThalLayerSpec* snrthalsp = (V1SNrThalLayerSpec*)snrthal_lay->spec.SPtr();

  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
    LeabraUnit* snru = (LeabraUnit*)snrgp->Leaf(0);

    V1PFCGateSpec::GateSignal gate_sig = V1PFCGateSpec::GATE_NOGO;
    if(snru->act_eq > snrthalsp->snrthal.go_thr) {
      gate_sig = V1PFCGateSpec::GATE_GO;
    }

    ugp->misc_state2 = gate_sig; // store the raw gating signal itself

    if(net->phase_no == 1) {
      if(ugp->misc_state <= 0) { // empty stripe
	if(gate_sig == V1PFCGateSpec::GATE_GO) {
	  ugp->misc_state1 = V1PFCGateSpec::EMPTY_GO;
	  Compute_MaintUpdt_ugp(ugp, STORE, lay, net);
	}
	else {
	  ugp->misc_state1 = V1PFCGateSpec::EMPTY_NOGO;
	  ugp->misc_state--;	// more time off
	}
      }
      else {			// latched stripe
	if(gate_sig == V1PFCGateSpec::GATE_GO) {
	  ugp->misc_state1 = V1PFCGateSpec::LATCH_GO;
	  Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net); // clear in first phase
	}
	else {
	  ugp->misc_state1 = V1PFCGateSpec::LATCH_NOGO;
	  ugp->misc_state++;  // keep on mainting
	}
      }
    }
    else {			// second plus (2m)
      if(ugp->misc_state <= 0) {
	if(gate_sig == V1PFCGateSpec::GATE_GO) {
	  if(ugp->misc_state1 == V1PFCGateSpec::LATCH_GO)
	    ugp->misc_state1 = V1PFCGateSpec::LATCH_GOGO;
	  else
	    ugp->misc_state1 = V1PFCGateSpec::EMPTY_GO;
	  Compute_MaintUpdt_ugp(ugp, STORE, lay, net);
	}
      }
    }
  }
  SendGateStates(lay, net);
}

void V1PFCLayerSpec::SendGateStates(LeabraLayer* lay, LeabraNetwork*) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_V1SNrThalLayerSpec);
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(snrthal_lay, mtx_prjn_idx, &TA_V1MatrixLayerSpec);
  int mg;
  for(mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    // everybody gets gate state info from PFC!
    snrgp->misc_state = mugp->misc_state = ugp->misc_state;
    snrgp->misc_state1 = ugp->misc_state1; 
    if(mugp->misc_state1 < V1PFCGateSpec::NOGO_RND_GO) { // don't override random go signals
      mugp->misc_state1 = ugp->misc_state1;
    }
    snrgp->misc_state2 = mugp->misc_state2 = ugp->misc_state2;
  }
}

void V1PFCLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(gate.allow_clamp && clamp.hard && lay->HasExtFlag(Unit::EXT)) {
    inherited::Compute_HardClamp(lay, net);
  }
  else {
    // not to hard clamp: needs to update in 2nd plus phase!
    lay->hard_clamped = false;
    lay->Init_InputData(net);
  }
}

void V1PFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no >= 1) {
    Compute_GatingGOGO(lay, net);	// do gating
  }
}

// todo: this is no longer feasible -- would require a unit-level check function or something
// could potentially impl as a post-hoc zero of activations for non-gated layers.
// but overall, it seems a bit of a stretch in any case, and doesn't make a huge diff,
// so probably better to just let it go..

// void V1PFCLayerSpec::Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) {
//   if(!gate.out_gate_learn_mod) {
//     inherited::Compute_dWt_impl(lay, net);
//     return;
//   }
//   int pfcout_prjn_idx;
//   LeabraLayer* pfcout_lay = FindLayerFmSpec(lay, pfcout_prjn_idx, &TA_V1PFCOutLayerSpec);
//   if(!pfcout_lay) {
//     inherited::Compute_dWt_impl(lay, net);
//     return;
//   }

//   for(int mg=0;mg<lay->units.gp.size;mg++) {
//     LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
//     LeabraUnit_Group* outgp = (LeabraUnit_Group*)pfcout_lay->units.gp[mg];

//     if(outgp->misc_state2 != V1PFCGateSpec::GATE_NOGO) {
//       LeabraUnit* u;
//       taLeafItr i;
//       FOR_ITR_EL(LeabraUnit, u, ugp->, i)
// 	u->Compute_dWt(lay, net);
//     }
//   }
// }

//////////////////////////////////
//	PFCOut Layer Spec	//
//////////////////////////////////

void V1PFCOutGateSpec::Initialize() {
  base_gain = 0.5f;
  go_gain = 0.5f;
  graded_go = false;
}

void V1PFCOutGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  go_gain = 1.0f - base_gain;
}


void V1PFCOutLayerSpec::Initialize() {
  gain_sched_value = NO_BGS;
  gain_sched.interpolate = false;
  gain_sched.default_val = .5f;

  // this guy should always inherit from V1PFCLayerSpec
//   SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .15f;
//   SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
//   SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;
//   SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.1f;
  decay.clamp_phase2 = false;	// this is the one exception!
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void V1PFCOutLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  out_gate.UpdateAfterEdit_NoGui();
  gain_sched.UpdateAfterEdit_NoGui();
}

void V1PFCOutLayerSpec::Defaults() {
  inherited::Defaults();
  out_gate.Defaults();
  Initialize();
}

void V1PFCOutLayerSpec::HelpConfig() {
  String help = "V1PFCOutLayerSpec Computation:\n\
 The PFCOut layer gets activations from corresponding PFC layer (via MarkerCon)\
 and sets unit activations as a function of the Go gating signals received from\
 associated SNrThal layer\n\
 \nV1PFCOutLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure PFC and BG layers.\n\
 - Units must recv MarkerConSpec from V1SNrThalLayerSpec layer for gating\n\
 - Units must recv MarkerConSpec from V1PFCLayerSpec layer for activations\n\
 - This layer must be after V1SNrThalLayerSpec layer in list of layers\n\
 - This layer must be after V1PFCLayerSpec layer in list of layers\n\
 - Units must be organized into groups corresponding to the matrix groups (stripes).";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool V1PFCOutLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  bool rval = true;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_V1SNrThalLayerSpec);
  if(lay->CheckError(snrthal_lay == NULL, quiet, rval,
		"no projection from SNrThal Layer found: must have MarkerConSpec!")) {
    return false;
  }
  if(lay->CheckError(snrthal_lay->units.gp.size != lay->units.gp.size, quiet, rval,
		"Gating Layer unit groups must = PFCOutLayer unit groups!")) {
    snrthal_lay->unit_groups = true;
    snrthal_lay->gp_geom.n = lay->units.gp.size;
    return false;
  }

  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_V1PFCLayerSpec);
  if(!pfc_lay) {
    // try to find a non-pfclayerspec layer with marker cons that is not snrthal..
    Projection* p;
    taLeafItr pi;
    FOR_ITR_EL(Projection, p, lay->projections., pi) {
      LeabraConSpec* cs = (LeabraConSpec*)p->con_spec.SPtr();
      if(!cs) continue;
      LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
      if(cs->InheritsFrom(&TA_MarkerConSpec) && (fmlay != snrthal_lay)) {
	pfc_prjn_idx = p->recv_idx;
	pfc_lay = fmlay;
	break;
      }
    }
  }
  if(lay->CheckError(!pfc_lay, quiet, rval,
		"no projection from PFC Layer found: must have MarkerConSpec!")) {
    return false;
  }
  if(lay->CheckError(pfc_lay->units.gp.size != lay->units.gp.size, quiet, rval,
		"PFC Layer unit groups must = PFCOutLayer unit groups, copiped from PFC Layer; Please do a Build of network")) {
    lay->unit_groups = true;
    lay->gp_geom.n = pfc_lay->units.gp.size;
  }
  if(lay->CheckError(pfc_lay->units.leaves != lay->units.leaves, quiet, rval,
		"PFC Layer units must = PFCOutLayer units, copied from PFC Layer; Please do a Build of network")) {
    lay->un_geom = pfc_lay->un_geom;
  }

  V1PFCLayerSpec* pfcsp = (V1PFCLayerSpec*)pfc_lay->spec.SPtr();
  kwta = pfcsp->kwta;
  gp_kwta = pfcsp->gp_kwta;
  inhib_group = pfcsp->inhib_group;
  inhib.type = pfcsp->inhib.type;
  inhib.kwta_pt = pfcsp->inhib.kwta_pt;
  return true;
}

void V1PFCOutLayerSpec::SetCurBaseGain(LeabraNetwork* net) {
  if(gain_sched_value == NO_BGS) return;

  if(gain_sched_value == EXT_REW_AVG) {
    LeabraLayer* er_lay = LeabraLayerSpec::FindLayerFmSpecNet(net, &TA_ExtRewLayerSpec);
    if(er_lay != NULL) {
      LeabraUnit* un = (LeabraUnit*)er_lay->units.Leaf(0);
      float avg_rew = un->act_avg;
      int ar_pct = (int)(100.0f * avg_rew);
      out_gate.SetBaseGain(gain_sched.GetVal(ar_pct));
      return;
    }
    else {
      TestWarning(true, "SetCurLrate", "appropriate ExtRew layer not found for EXT_REW_AVG, reverting to EPOCH!");
      SetUnique("gain_sched_value", true);
      gain_sched_value = EPOCH;
      UpdateAfterEdit();
    }
  }
  if(gain_sched_value == EXT_REW_STAT) {
    int arval = 0;
    if(net->epoch < 1) {
      arval = gain_sched.last_ctr;
    }
    else {
      arval = (int)(100.0f * net->avg_ext_rew);
    }
    out_gate.SetBaseGain(gain_sched.GetVal(arval));
  }

  if(gain_sched_value == EPOCH) {
    out_gate.SetBaseGain(gain_sched.GetVal(net->epoch));
  }
}

void V1PFCOutLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  SetCurBaseGain(net);

  // not to hard clamp: needs to update in 2nd plus phase!
  lay->hard_clamped = false;
  lay->Init_InputData(net);
}

void V1PFCOutLayerSpec::Compute_PfcOutAct(LeabraLayer* lay, LeabraNetwork* net) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_V1SNrThalLayerSpec);
  V1SNrThalLayerSpec* snrthalsp = (V1SNrThalLayerSpec*)snrthal_lay->spec.SPtr();
//   int mtx_prjn_idx;
//   LeabraLayer* matrix_lay = FindLayerFmSpec(snrthal_lay, mtx_prjn_idx, &TA_V1MatrixLayerSpec);
  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_V1PFCLayerSpec);
  if(!pfc_lay) {
    // try to find a non-pfclayerspec layer with marker cons that is not snrthal..
    Projection* p;
    taLeafItr pi;
    FOR_ITR_EL(Projection, p, lay->projections., pi) {
      LeabraConSpec* cs = (LeabraConSpec*)p->con_spec.SPtr();
      LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
      if(!cs) continue;
      if(cs->InheritsFrom(&TA_MarkerConSpec) && (fmlay != snrthal_lay)) {
	pfc_prjn_idx = p->recv_idx;
	pfc_lay = fmlay;
	break;
      }
    }
  }

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* pfcgp = (LeabraUnit_Group*)pfc_lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
//     LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    LeabraUnit* snru = (LeabraUnit*)snrgp->Leaf(0);

    // note that random go is added into activation at the snrthal level, not here.

    float gain = out_gate.base_gain;
    V1PFCGateSpec::GateSignal gate_sig = V1PFCGateSpec::GATE_NOGO;
    if(!snrthal_lay->lesioned() && (snru->act_eq > snrthalsp->snrthal.go_thr)) {
      gate_sig = V1PFCGateSpec::GATE_GO;
      if(out_gate.graded_go) 
	gain += snru->act_eq * out_gate.go_gain;
      else
	gain += out_gate.go_gain;
    }

    rugp->misc_state2 = gate_sig; // store the raw gating signal itself
    
    for(int i=0;i<rugp->size;i++) {
      LeabraUnit* ru = (LeabraUnit*)rugp->FastEl(i);
      LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();
      LeabraUnit* pfcu = (LeabraUnit*)pfcgp->FastEl(i);
      
      ru->act = gain * pfcu->act;
      ru->act_eq = ru->act_nd = ru->act;
      ru->da = 0.0f;		// I'm fully settled!
      ru->AddToActBuf(rus->syn_delay);
    }
  }
}

void V1PFCOutLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_PfcOutAct(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

