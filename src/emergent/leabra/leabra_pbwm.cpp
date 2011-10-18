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

void PBWMUnGpData::Initialize() {
  mnt_count = 0;
  gate_state = PFCGateSpec::INIT_STATE;
  gate_sig = PFCGateSpec::GATE_NOGO;
  rnd_go_thr = 0;
  cur_go_act = 0.0f;
  out_go_act = 0.0f;
}

void PBWMUnGpData::Copy_(const PBWMUnGpData& cp) {
  mnt_count = cp.mnt_count;
  gate_state = cp.gate_state;
  gate_sig = cp.gate_sig;
  rnd_go_thr = cp.rnd_go_thr;
  cur_go_act = cp.cur_go_act;
  out_go_act = cp.out_go_act;
}

void PBWMUnGpData::Init_State() {
  inherited::Init_State();
  mnt_count = 0;
  gate_state = PFCGateSpec::INIT_STATE;
  gate_sig = PFCGateSpec::GATE_NOGO;
  rnd_go_thr = 0;
  cur_go_act = 0.0f;
  out_go_act = 0.0f;
}

////////////////////////////////////////////////////////////////////
//	Patch/Striosomes and SNc

void PatchLayerSpec::Initialize() {
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
}

void PatchLayerSpec::Send_LVeToMatrix(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
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

//////////////////////////////////////////////////////
//		SNcLayerSpec

void SNcMiscSpec::Initialize() {
  stripe_lv_pct = 0.5f;
  global_lv_pct = 0.5f;
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
  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		     "requires unit groups, one per associated stripe")) {
    return false;
  }

  int patch_prjn_idx;
  LeabraLayer* patch_lay = FindLayerFmSpec(lay, patch_prjn_idx, &TA_PatchLayerSpec);
  if(lay->CheckError(!patch_lay, quiet, rval,
		"did not find Patch layer to get Da from!")) {
    return false;
  }

  if(lay->CheckError(lay->gp_geom.n != patch_lay->gp_geom.n, quiet, rval,
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

  lay->dav = 0.0f;
  for(int gi=0; gi < lay->gp_geom.n; gi++) {
    LeabraUnit* snc_u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, 0, gi);

    float str_da = patch_sp->Compute_LVDa_ugp(patch_lay, lvi_lay,
			      Layer::ACC_GP, gi, Layer::ACC_LAY, 0, net); // per stripe
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
      for(int j=0;j<send_gp->size; j++) {
	((LeabraUnit*)send_gp->Un(j))->dav = send_val;
      }
    }
  }
  lay->dav /= (float)lay->gp_geom.n; // integrated average -- not really used
}

void SNcLayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork*) {
  // do nothing -- was sent in more targeted fashion in compute_da
}

//////////////////////////////////
//	SNrThal Layer Spec	//
//////////////////////////////////

void SNrThalMiscSpec::Initialize() {
  net_off = 0.0f;
  go_thr = 0.5f;
  rnd_go_inc = 0.1f;
  leak = 1.0f;
  act_is_gate = true;
}

void SNrThalLayerSpec::Initialize() {
  bg_type = MAINT;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 4;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  Defaults_init();
}

void SNrThalLayerSpec::Defaults_init() {
  SetUnique("tie_brk", true);	// turn on tie breaking by default
  tie_brk.on = true;
  tie_brk.thr_gain = 0.2f;
  tie_brk.loser_gain = 1.0f;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .7f;
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

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

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

  if(lay->CheckError(matrix_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
		"MatrixLayer unit groups must = SNrThalLayer unit groups!")) {
    lay->unit_groups = true;
    lay->gp_geom.n = matrix_lay->gp_geom.n;
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

  int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
  int munits = matrix_lay->UnitAccess_NUnits(Layer::ACC_GP);
  int gp_sz = munits / 2;
  LeabraUnitSpec* us = (LeabraUnitSpec*)matrix_lay->GetUnitSpec();
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* mgpd = (PBWMUnGpData*)matrix_lay->ungp_data.FastEl(mg);
    float gonogo = 0.0f;
    if(mgpd->acts.max >= us->opt_thresh.send) {
      float sum_go = 0.0f;
      float sum_nogo = 0.0f;
      for(int i=0; i<munits; i++) {
	LeabraUnit* u = (LeabraUnit*)matrix_lay->UnitAccess(Layer::ACC_GP, i, mg);
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
      if(mgpd->gate_state >= PFCGateSpec::NOGO_RND_GO) {
	gonogo += snrthal.rnd_go_inc;
	if(gonogo > 1.0f) gonogo = 1.0f;
      }
    }

    float net_eff = net_off_rescale * (gonogo + snrthal.net_off);

    for(int i=0;i<nunits;i++) {
      LeabraUnit* ru = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, i, mg);
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

void SNrThalLayerSpec::Compute_GatedActs(LeabraLayer* lay, LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
  for(int g=0; g < lay->gp_geom.n; g++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(g);

    bool zap_act_eq = false;
    if((bg_type == MAINT) && (gpd->gate_sig == PFCGateSpec::GATE_OUT_GO)) {
      zap_act_eq = true;
    }
    if((bg_type == OUTPUT) && (gpd->gate_sig == PFCGateSpec::GATE_MNT_GO)) {
      zap_act_eq = true;
    }
    if(zap_act_eq) {
      // this is for display purposes, to show that no gating took place in this stripe
      // due to competition with other type of gating
      for(int i=0;i<nunits;i++) {
	LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, i, g);
	u->act = u->act_eq = 0.0f; // now zapping act too!
      }
    }
    else if(snrthal.act_is_gate) {
      if(gpd->gate_sig != PFCGateSpec::GATE_NOGO) { // some kind of gating happened
	// this is also for display purposes, to always show actual gating signal
	// that was active at time of gating, not what current activity is..
	for(int i=0;i<nunits;i++) {
	  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, i, g);
	  u->act = u->act_eq = u->act_m2; // always reflect act_m2 gating signal if gated!
	}
      }
    }
  }
}

void SNrThalLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_GatedActs(lay, net);
  inherited::Compute_CycleStats(lay, net);
}


void SNrThalLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // nop -- don't do the default thing -- already done by call to MidMinusAct
}

void SNrThalLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay,
					       Layer::AccessMode acc_md, int gpidx,
					       LeabraNetwork* net) {
  // capture at snrthal level
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    u->act_m2 = u->act_eq;
  }

  // then at matrix level
  int dum_prjn_idx = 0;
  LeabraLayer* matrix = FindLayerFmSpec(lay, dum_prjn_idx, &TA_MatrixLayerSpec);
  MatrixLayerSpec* mls = (MatrixLayerSpec*)matrix->spec.SPtr();
  mls->Compute_MidMinusAct_ugp(matrix, acc_md, gpidx, net); // tell matrix to do it
}


void SNrThalLayerSpec::SendGateStates(LeabraLayer* lay, LeabraNetwork* net,
				      LeabraLayer* pfc_lay) {
  int dum_prjn_idx = 0;
  LeabraLayer* matrix = FindLayerFmSpec(lay, dum_prjn_idx, &TA_MatrixLayerSpec);
  LeabraLayer* patch = FindLayerFmSpec(matrix, dum_prjn_idx, &TA_PatchLayerSpec);
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* pfcgpd = (PBWMUnGpData*)pfc_lay->ungp_data.FastEl(mg);
    PBWMUnGpData* snrgpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    PBWMUnGpData* mgpd = (PBWMUnGpData*)matrix->ungp_data.FastEl(mg);

    // everybody gets gate state info from PFC!
    snrgpd->mnt_count = mgpd->mnt_count = pfcgpd->mnt_count;
    snrgpd->gate_state = pfcgpd->gate_state;
    if(mgpd->gate_state < PFCGateSpec::NOGO_RND_GO) { // don't override random go signals
      mgpd->gate_state = pfcgpd->gate_state;
    }
    snrgpd->gate_sig = mgpd->gate_sig = pfcgpd->gate_sig;
    // matrix does not get cur_go_act -- uses it otherwise
    snrgpd->cur_go_act = pfcgpd->cur_go_act;
    snrgpd->out_go_act = pfcgpd->out_go_act;

    if(patch) {
      PBWMUnGpData* patchgpd = (PBWMUnGpData*)patch->ungp_data.FastEl(mg);
      // everybody gets gate state info from PFC!
      patchgpd->mnt_count = snrgpd->mnt_count;
      patchgpd->gate_state = snrgpd->gate_state;
      patchgpd->gate_sig = snrgpd->gate_sig;
      patchgpd->cur_go_act = snrgpd->cur_go_act;
      patchgpd->out_go_act = snrgpd->out_go_act;
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

  Defaults_init();
}

void MatrixConSpec::Defaults_init() {
  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
  wt_sig.off = 1.0f;
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

  Defaults_init();
}

void MatrixBiasSpec::Defaults_init() {
  SetUnique("lrate", true);
  lrate = 0.0f;			// default is no lrate
}

void MatrixNoiseSpec::Initialize() {
  patch_noise = true;
}

void MatrixUnitSpec::Initialize() {
  SetUnique("bias_spec", true);
  bias_spec.type = &TA_MatrixBiasSpec;
  SetUnique("g_bar", true);
  g_bar.a = .03f;
  g_bar.h = .01f;

  SetUnique("noise", true);
  noise.var = 0.0002f;
  SetUnique("noise_adapt", true);
  noise_adapt.mode = NoiseAdaptSpec::PVLV_LVE;

  Defaults_init();
}

void MatrixUnitSpec::Defaults_init() {
  SetUnique("act", true);
  act.i_thr = ActFunSpec::NO_AH; // key for dopamine effects

  SetUnique("maxda", true);
  maxda.val = MaxDaSpec::NO_MAX_DA;

  SetUnique("noise_type", true);
  noise_type = NETIN_NOISE;

  SetUnique("noise_adapt", true);
  noise_adapt.trial_fixed = true;
  noise_adapt.k_pos_noise = true;
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
  out_rew_go = 1.0f;
  out_norew_nogo = 1.0f;
  out_empty_nogo = 5.0f;
  mnt_rew_nogo = 5.0f;
  mnt_mnt_nogo = 0.0f;
  mnt_empty_go = 0.0f;
}

void MatrixGateBiasFunSpec::Initialize() {
  on = false;
  fun = LIN;
  off = 0;
  interval = 2;
  start = 0.0f;
  end = 1.0f;
  incr = 0.5f;
}

void MatrixGateBiasFunSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!on) return;
  if(TestWarning((interval < 1), "UAE",
		 "interval cannot be < 1 -- if constant, please turn off this function and use normal constant biases -- turning off for now")) {
    on = false;
    return;
  }
  if(TestWarning((start == end), "UAE",
		 "start cannot be equal to end -- if constant, please turn off this function and use normal constant biases -- turning off for now")) {
    on = false;
    return;
  }
  if(fun == LIN) {
    incr = (end - start) / (float)interval;
  }
  else {
    // todo: do exp
  }
}

void MatrixMiscSpec::Initialize() {
  da_gain = 0.1f;
  bias_gain = .1f;
  bias_pos_gain = 0.0f;
  mnt_only = false;
}

void MatrixRndGoSpec::Initialize() {
  nogo_thr = 20;
  rng_eq_thr = true;
  nogo_rng = nogo_thr;
  nogo_da = 10.0f;
  nogo_noise = 0.02f;
}

void MatrixRndGoSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(rng_eq_thr)
    nogo_rng = nogo_thr;
}

void MatrixGoNogoGainSpec::Initialize() {
  on = false;
  go_p = go_n = nogo_p = nogo_n = 1.0f;
}

/////////////////////////////////////////////////////

void MatrixLayerSpec::Initialize() {
//   SetUnique("tie_brk", true);	// turn on tie breaking by default
  //  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  //  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  //  SetUnique("inhib", true);
//   inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
//   inhib.kwta_pt = .5f;
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .2f;

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  bg_type = MAINT;

  Defaults_init();
}

void MatrixLayerSpec::Defaults_init() {
  // new default..
  //  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 4;

  tie_brk.on = true;
  tie_brk.diff_thr = 0.2f;
  tie_brk.thr_gain = 0.005f;
  tie_brk.loser_gain = 1.0f;

  out_rew_go_fun.start = 0.0f;
  out_rew_go_fun.end = 1.0f;
  out_rew_go_fun.off = 1;
  out_rew_go_fun.interval = 2;
  out_rew_go_fun.UpdateAfterEdit_NoGui();

  mnt_mnt_nogo_fun.start = 1.0f;
  mnt_mnt_nogo_fun.end = 0.0f;
  mnt_mnt_nogo_fun.off = 1;	// strong no immediate-update bias
  mnt_mnt_nogo_fun.interval = 2; // then decay quickly
  mnt_mnt_nogo_fun.UpdateAfterEdit_NoGui();

  mnt_empty_go_fun.start = 0.0f;
  mnt_empty_go_fun.end = 1.0f;
  mnt_empty_go_fun.off = 5;
  mnt_empty_go_fun.interval = 10;
  mnt_empty_go_fun.UpdateAfterEdit_NoGui();
}

void MatrixLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate_bias.UpdateAfterEdit_NoGui();
  rnd_go.UpdateAfterEdit_NoGui();
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

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

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

void MatrixLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay,
					      Layer::AccessMode acc_md, int gpidx,
					      LeabraNetwork* net) {
  MatrixUnitSpec* us = (MatrixUnitSpec*)lay->GetUnitSpec();
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    us->Compute_MidMinusAct(u, net);
  }
}

void MatrixLayerSpec::Compute_NetinStats_ugp(LeabraLayer* lay,
					     Layer::AccessMode acc_md, int gpidx,
					     LeabraInhib* thr,  LeabraNetwork* net) {
  float bias_dav = Compute_BiasDaMod(lay, acc_md, gpidx, net);
  Compute_MultBias(lay, acc_md, gpidx, net, bias_dav);

  inherited::Compute_NetinStats_ugp(lay, acc_md, gpidx, thr, net);
}

float MatrixLayerSpec::Compute_BiasDaMod(LeabraLayer* lay,
					 Layer::AccessMode acc_md, int gpidx,
					 LeabraNetwork* net) {
  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_PVLVDaLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.SPtr();
  PBWMUnGpData* mgpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  float tonic_da = dals->da.tonic_da;
  int pfc_mnt_cnt = mgpd->mnt_count; // is pfc maintaining or not?
  bool pfc_is_mnt = pfc_mnt_cnt > 0;
  int rnd_go_thr = mgpd->rnd_go_thr; // random go threshold for this time
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int gp_sz = nunits / 2;
  bool nogo_rnd_go = (mgpd->gate_state == PFCGateSpec::NOGO_RND_GO);

  float bias_dav = 0.0f;

  if(bg_type == OUTPUT) {	// output gating guy
    if(net->pv_detected) {	// PV reward trial -- bias output gating
      // only if pfc is maintaining..
      if(pfc_is_mnt) {
	if(out_rew_go_fun.on)
	  bias_dav = out_rew_go_fun.GetBias(pfc_mnt_cnt);
	else
	  bias_dav = gate_bias.out_rew_go;
	if(!nogo_rnd_go && pfc_mnt_cnt > rnd_go_thr) { // no rnd go yet, but over thresh
	  mgpd->gate_state = PFCGateSpec::NOGO_RND_GO;
	  Compute_RndGoNoise_ugp(lay, acc_md, gpidx, net);
	}
      }
      else {
	bias_dav = -gate_bias.out_empty_nogo;
      }
    }
    else {			// not a PV trial
      if(pfc_is_mnt) {
	bias_dav = -gate_bias.out_norew_nogo; // blanket no output gating thing
      }
      else {
	bias_dav = -gate_bias.out_empty_nogo;
      }
    }
  }
  else {			// MAINT
    if(!matrix.mnt_only && net->pv_detected) {	// PV reward trial -- no maint gating
      bias_dav = -gate_bias.mnt_rew_nogo;
    }
    else {
      if(pfc_mnt_cnt > 0) {   	// currently maintaining: bias NoGo for everything
	if(mnt_mnt_nogo_fun.on)
	  bias_dav = -mnt_mnt_nogo_fun.GetBias(pfc_mnt_cnt);
	else
	  bias_dav = -gate_bias.mnt_mnt_nogo;
      }
      else {			// otherwise, bias to maintain/update
	if(mnt_empty_go_fun.on)
	  bias_dav = mnt_empty_go_fun.GetBias(-pfc_mnt_cnt); // negative count for amount of time empty
	else
	  bias_dav = gate_bias.mnt_empty_go;
	if(!nogo_rnd_go && pfc_mnt_cnt < -rnd_go_thr) { // no rnd go yet, but over thresh
	  mgpd->gate_state = PFCGateSpec::NOGO_RND_GO;
	  Compute_RndGoNoise_ugp(lay, acc_md, gpidx, net);
	}
      }
    }
  }

  float tot_dav = bias_dav + tonic_da;
  return tot_dav;
}

void MatrixLayerSpec::Compute_MultBias(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       LeabraNetwork* net, float bias_dav) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int gp_sz = nunits / 2;
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(i / gp_sz);
    float netin_extra = 0.0f;
    if(go_no == PFCGateSpec::GATE_NOGO) {
      netin_extra = -matrix.bias_gain * bias_dav;
    }
    else {			// must be a GO
      netin_extra = matrix.bias_gain * bias_dav;
    }
    if(netin_extra > 0.0f) netin_extra *= matrix.bias_pos_gain;
    u->net *= (1.0f + netin_extra);
    u->misc_2 = netin_extra;	// record
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
// NOTE: mnt_count reflects count at time of gating -- not updated at Compute_Gate_Final

void MatrixLayerSpec::Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net) {
  int n_rnd_go = 0;		// find out if anyone has a rnd go
  int n_go = 0;
  float nogo_da_sub = 0.0f;

  // subtract the average -- this turns out to be important for preventing global
  // drift in weights upward when lots of rnd go is going on (e.g., loop model)
  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    PBWMUnGpData* mgpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);
    if(mgpd->gate_state == PFCGateSpec::NOGO_RND_GO) n_rnd_go++;
    else if(mgpd->gate_sig == PFCGateSpec::GATE_MNT_GO) n_go++;
  }
  if(n_rnd_go > 0) {
    nogo_da_sub = rnd_go.nogo_da / (float)(lay->gp_geom.n - n_rnd_go);
    // how much to subtract from other units if one guy gets a random go
  }

  int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
  int gp_sz = nunits / 2;

  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    PBWMUnGpData* mgpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);
    int snr_prjn_idx = 0;
    LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);

    PBWMUnGpData* snrgpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(gi);
    LeabraUnit* snr_u = (LeabraUnit*)snr_lay->UnitAccess(Layer::ACC_GP, 0, gi);

    PFCGateSpec::GateSignal gate_sig = (PFCGateSpec::GateSignal)mgpd->gate_sig;
    int pfc_mnt_cnt = mgpd->mnt_count; // is pfc maintaining or not?
    bool nogo_rnd_go = (mgpd->gate_state == PFCGateSpec::NOGO_RND_GO);

    float snrthal_act = snr_u->act_m2;
    if(bg_type == OUTPUT) {
      if((gate_sig == PFCGateSpec::GATE_NOGO) || (gate_sig == PFCGateSpec::GATE_MNT_GO))
	snrthal_act = 0.0f;	// if no OUT_GO, nothing for us..
    }
    else {			// MAINT
      if((gate_sig == PFCGateSpec::GATE_NOGO) || (gate_sig == PFCGateSpec::GATE_OUT_GO))
	snrthal_act = 0.0f;	// if no MNT_GO, nothing for us..
    }

    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, i, gi);
      PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(i / gp_sz);

      // critical gating activation value is mid-minus state
      float act_val = u->act_m2;
      float lrn_dav = snrthal_act * u->dav; // dav is current plus phase

      if(go_nogo_gain.on) {
	if(lrn_dav > 0.0f) {
	  if(go_no == PFCGateSpec::GATE_GO)
	    lrn_dav *= go_nogo_gain.go_p;
	  else
	    lrn_dav *= go_nogo_gain.nogo_p;
	}
	else if(lrn_dav < 0.0f) {
	  if(go_no == PFCGateSpec::GATE_GO)
	    lrn_dav *= go_nogo_gain.go_n;
	  else
	    lrn_dav *= go_nogo_gain.nogo_n;
	}
      }

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
    }
  }
}

void MatrixLayerSpec::Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    PBWMUnGpData* mgpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);
    if((mgpd->gate_state == PFCGateSpec::NOGO_RND_GO) || mgpd->rnd_go_thr == 0) {
      mgpd->gate_state = PFCGateSpec::INIT_STATE;
      // new treshold for when to fire rnd go next!
      mgpd->rnd_go_thr = rnd_go.nogo_thr + Random::IntZeroN(rnd_go.nogo_rng);
    }
  }
}

void MatrixLayerSpec::Compute_RndGoNoise_ugp(LeabraLayer* lay,
					     Layer::AccessMode acc_md, int gpidx,
					     LeabraNetwork* net) {
  if(rnd_go.nogo_noise == 0.0f) return;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  PBWMUnGpData* mgpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);

  int n_go_units = nunits / 2;
  lay->unit_idxs.SetSize(n_go_units); // just do go guys
  lay->unit_idxs.FillSeq();
  lay->unit_idxs.Permute();

  // Set k of the permuted "go" units to the noise value
  int i;
  for(i=0; i<mgpd->kwta.k; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, lay->unit_idxs[i], gpidx);
    u->noise = rnd_go.nogo_noise;
  }

  // Set the remainder of the "go" units to have no noise.
  for(; i<n_go_units; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, lay->unit_idxs[i], gpidx);
    u->noise = 0.0f;
  }

  // now fill in all the nogo guys with no noise just to be sure..
  for(; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx); // note: not unit_idxs[i] here!
    u->noise = 0.0f;
  }
}

void MatrixLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    Compute_ClearRndGo(lay, net);
  }

  inherited::Compute_HardClamp(lay, net);
}

void MatrixLayerSpec::LabelUnits_impl(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int gp_sz = nunits / 2;
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(i / gp_sz); // GO = 0, NOGO = 1
    if(go_no == PFCGateSpec::GATE_GO)
      u->name = "Go";
    else
      u->name = "No";
  }
}

void MatrixLayerSpec::LabelUnits(LeabraLayer* lay) {
  UNIT_GP_ITR(lay, LabelUnits_impl(lay, acc_md, gpidx); );
}


//////////////////////////////////
//	PFC Layer Spec		//
//////////////////////////////////

void PFCGateSpec::Initialize() {
  graded_out_go = true;
  clear_decay = 0.9f;
  go_learn_base = 0.05f;
  go_learn_mod = 1.0f - go_learn_base;
  mid_minus_min = 10;
  max_maint = 100;
  off_accom = 0.0f;
  out_go_clear = true;
  mnt_toggle = true;
  mnt_wins = false;
  updt_gch = false;
}

void PFCGateSpec::UpdateAfterEdit_impl() {
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
  inhib.kwta_pt = .5f;
  SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.1f;
  decay.clamp_phase2 = false;	// this is the one exception!
}

void PFCLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate.UpdateAfterEdit_NoGui();
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
    if(p->con_spec.type->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
      LeabraLayerSpec* ls = (LeabraLayerSpec*)fmlay->spec.SPtr();
      if(ls->InheritsFrom(&TA_SNrThalLayerSpec)) {
	if(((SNrThalLayerSpec*)ls)->bg_type == SNrThalLayerSpec::OUTPUT)
	  snrthal_out = fmlay;
	else
	  snrthal_mnt = fmlay;
      }
    }
  }
  if(snrthal_out) return;
  // look for snrthal_out in pfc_out layer
  FOR_ITR_EL(Projection, p, lay->send_prjns., pi) {
    if(p->con_spec.type->InheritsFrom(TA_MarkerConSpec)) {
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

  if(lay->CheckError(net->min_cycles < net->mid_minus_cycle + 10, quiet, rval,
		"requires LeabraNetwork min_cycles >= mid_minus_cycle + 10, I just set it for you")) {
    net->min_cycles = net->mid_minus_cycle + 10;
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
  if(lay->CheckError(snrthal_mnt->gp_geom.n != lay->gp_geom.n, quiet, rval,
		"Gating Layer unit groups must = PFCLayer unit groups!")) {
    snrthal_mnt->unit_groups = true;
    snrthal_mnt->gp_geom.n = lay->gp_geom.n;
  }
  if(snrthal_out) {
    if(lay->CheckError(snrthal_out->gp_geom.n != lay->gp_geom.n, quiet, rval,
		       "Gating Layer unit groups must = PFCLayer unit groups!")) {
      snrthal_out->unit_groups = true;
      snrthal_out->gp_geom.n = lay->gp_geom.n;
    }
  }

  return true;
}

void PFCLayerSpec::Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net) {
  for(int g=0; g < lay->gp_geom.n; g++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(g);
    gpd->gate_state = PFCGateSpec::INIT_STATE;
    gpd->gate_sig = PFCGateSpec::GATE_NOGO;
    gpd->cur_go_act = gate.go_learn_base;
    gpd->out_go_act = 0.0f;
  }
  // this makes sure that mnt_count is sent to all layers at the start of trial
  // so that biases are based on maintenance status at end of last trial!
  SendGateStates(lay, net);
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

void PFCLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay,
					   Layer::AccessMode acc_md, int gpidx,
					   LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    u->act_m2 = u->act_eq;
  }
}

void PFCLayerSpec::Compute_MaintUpdt_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
					 MaintUpdtAct updt_act, LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int j=0;j<nunits;j++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
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
    else if(updt_act == UPDT) {
      if(u->maint_h >= us->opt_thresh.send) { // still above threshold -- update
	u->vcb.g_h = u->maint_h = u->act_eq;
      }
      else {
	u->vcb.g_h = u->maint_h = 0.0f; // clear!
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
  Layer::AccessMode acc_md = Layer::ACC_GP;

  // this is the continuous mid minus version -- when gating actually happens

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    PBWMUnGpData* mgpd = (PBWMUnGpData*)snrthal_mnt->ungp_data.FastEl(mg);
    LeabraUnit* snr_mnt_u = (LeabraUnit*)snrthal_mnt->UnitAccess(acc_md, 0, mg);
    LeabraUnit* snr_out_u = NULL;
    if(snrthal_out) {
      snr_out_u = (LeabraUnit*)snrthal_out->UnitAccess(acc_md, 0, mg);
    }

    int pfc_mnt_cnt = gpd->mnt_count; // is pfc maintaining or not?
    bool out_gate_fired = (gpd->gate_sig == PFCGateSpec::GATE_OUT_GO);
    bool mnt_gate_fired = (gpd->gate_sig == PFCGateSpec::GATE_MNT_GO);
    bool gate_fired = mnt_gate_fired || out_gate_fired;

    // maintenance gating signal -- can only happen if hasn't happened yet and mutex with out
    bool allow_mnt_gate = !gate_fired;
    if(gate.mnt_wins)
      allow_mnt_gate = !mnt_gate_fired; // only depends on maint, not out!

    if(allow_mnt_gate && (snr_mnt_u->act_eq > go_thr_mnt)) {
      // compute out_gate multiplier in out_go_act -- maint gating causes output gating too!
      gpd->out_go_act = 1.0f; // out gate multiplier
      if(gate.graded_out_go)
	gpd->out_go_act = snr_mnt_u->act_eq; // out gate multiplier

      Compute_MidMinusAct_ugp(lay, acc_md, mg, net);
      snrthalsp_mnt->Compute_MidMinusAct_ugp(snrthal_mnt, acc_md, mg, net);
      // snrthal and associated matrix layer grab act_m2 vals based on current state!

      if(pfc_mnt_cnt > 0 && gate.mnt_toggle) // full stripe
	Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);	 // clear maint currents if full -- toggle off

      // update state info
      gpd->gate_sig = PFCGateSpec::GATE_MNT_GO;
      if(pfc_mnt_cnt > 0) // full stripe
	gpd->gate_state = PFCGateSpec::MAINT_MNT_GO;
      else
	gpd->gate_state = PFCGateSpec::EMPTY_MNT_GO;

      // cur_go_act has the go_learn_base factor incorporated
      float lrn_go_act = snr_mnt_u->act_eq;
      gpd->cur_go_act = gate.go_learn_base + (gate.go_learn_mod * lrn_go_act);
      SendGateStates(lay, net);	// update snrthal for turning act_eq guys off
    }

    // output gating signal -- can only happen if hasn't happened yet, and mutex with mnt gating
    if(!gate_fired && snr_out_u && (snr_out_u->act_eq > go_thr_out)) {
      // compute out_gate multiplier in out_go_act
      gpd->out_go_act = 1.0f; // out gate multiplier
      if(gate.graded_out_go)
	gpd->out_go_act = snr_out_u->act_eq; // out gate multiplier

      Compute_MidMinusAct_ugp(lay, acc_md, mg, net);
      snrthalsp_out->Compute_MidMinusAct_ugp(snrthal_out, acc_md, mg, net);
      // snrthal and associated matrix layer grab act_m2 vals based on current state!

      // update state info
      gpd->gate_sig = PFCGateSpec::GATE_OUT_GO;
      if(pfc_mnt_cnt > 0) // full stripe
	gpd->gate_state = PFCGateSpec::MAINT_OUT_GO;
      else
	gpd->gate_state = PFCGateSpec::EMPTY_OUT_GO;

      // cur_go_act has the go_learn_base factor incorporated
      float lrn_go_act = snr_out_u->act_eq;
      gpd->cur_go_act = gate.go_learn_base + (gate.go_learn_mod * lrn_go_act);
      SendGateStates(lay, net);	// update snrthal for turning act_eq guys off
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
  Layer::AccessMode acc_md = Layer::ACC_GP;

  // cleanup gating signals at end of mid minus -- just nogo!

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    LeabraUnit* snr_mnt_u = (LeabraUnit*)snrthal_mnt->UnitAccess(acc_md, 0, mg);
    LeabraUnit* snr_out_u = NULL;
    if(snrthal_out) {
      snr_out_u = (LeabraUnit*)snrthal_out->UnitAccess(acc_md, 0, mg);
    }

    int pfc_mnt_cnt = gpd->mnt_count; // is pfc maintaining or not?

    if(gpd->gate_sig == PFCGateSpec::GATE_NOGO) {
      // default NOGO results
      if(pfc_mnt_cnt > 0) // full stripe
	gpd->gate_state = PFCGateSpec::MAINT_NOGO;
      else
	gpd->gate_state = PFCGateSpec::EMPTY_NOGO;
      Compute_MidMinusAct_ugp(lay, acc_md, mg, net);
      if(snrthal_out)
	snrthalsp_out->Compute_MidMinusAct_ugp(snrthal_out, acc_md, mg, net);
      snrthalsp_mnt->Compute_MidMinusAct_ugp(snrthal_mnt, acc_md, mg, net);

      // cur_go_act has the go_learn_base factor incorporated
      gpd->cur_go_act = gate.go_learn_base;
    }
  }

  // now have full set of info -- send it along -- provides an update
  SendGateStates(lay, net);
}

void PFCLayerSpec::Compute_Gating_Final(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);

    // mnt_count == maintenance/empty counter (maintenance = + numbers, empty = - numbers)
    // basically just update the mnt_count counter and implement any
    // delayed STORE or CLEAR actions

    // for NOGO, just update the mnt_count counter
    if(gpd->gate_state == PFCGateSpec::EMPTY_NOGO) {
      gpd->mnt_count--;	// stay empty
    }
    else if(gpd->gate_state == PFCGateSpec::MAINT_NOGO) {
      if(gate.max_maint > 0) {			     // if max_maint = 0 then never store
	gpd->mnt_count++;	// continue maintaining
	if(gpd->mnt_count > gate.max_maint) {
	  Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);     // clear it!
	  gpd->mnt_count = 0;			     // empty
	}
	else if(gate.updt_gch) {
	  Compute_MaintUpdt_ugp(lay, acc_md, mg, UPDT, net);     // update it!
	}
      }
      else {
	gpd->mnt_count--;			     // nogo longer
      }
    }
    // look for store condition
    else if(gpd->gate_state == PFCGateSpec::MAINT_MNT_GO ||
	    gpd->gate_state == PFCGateSpec::EMPTY_MNT_GO) {
      if(gate.max_maint > 0) {			     // if max_maint = 0 then never store
	Compute_MaintUpdt_ugp(lay, acc_md, mg, STORE, net);     // store it (never stored before)
	gpd->mnt_count = 1;	// always reset on new store
      }
      else {
	gpd->mnt_count = 0;	// this go resets counter
      }
    }
    // or basic output gate with no veto from maint
    else if(gpd->gate_state == PFCGateSpec::MAINT_OUT_GO) {
      if(gate.out_go_clear) { // only clear on true output trials
	Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);     // clear it!
	gpd->mnt_count = 0;			     // empty
      }
      else {
	gpd->mnt_count++;	// continue maintaining
      }
    }
    else if(gpd->gate_state == PFCGateSpec::EMPTY_OUT_GO) {
      gpd->mnt_count--;	// no real issue here..
    }
  }
  // NOTE: Do NOT send final gate states -- this would corrupt the LearnDaVal
  // need to reflect status at time of gating computation (mid minus)
  // SendGateStates(lay, net);
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
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int mg=0; mg < lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);

    float lrn_mod_val = gpd->cur_go_act; // learning modulation value as function of gating

    for(int i=0;i<nunits;i++) {
      LeabraUnit* ru = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
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
  inhib.kwta_pt = .5f;
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
  if(lay->CheckError(pfc_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
		"PFC Layer unit groups must = PFCOutLayer unit groups, copiped from PFC Layer; Please do a Build of network")) {
    lay->unit_groups = true;
    lay->gp_geom.n = pfc_lay->gp_geom.n;
  }
  if(lay->CheckError(pfc_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
		"PFC Layer units must = PFCOutLayer units, copied from PFC Layer; Please do a Build of network")) {
    lay->un_geom = pfc_lay->un_geom;
  }

  return true;
}

void PFCOutLayerSpec::Compute_PfcOutAct(LeabraLayer* lay, LeabraNetwork* net) {
  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_PFCLayerSpec);
  PFCLayerSpec* pfcspec = (PFCLayerSpec*)pfc_lay->spec.SPtr();
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    PBWMUnGpData* pfcgpd = (PBWMUnGpData*)pfc_lay->ungp_data.FastEl(mg);

    gpd->mnt_count = pfcgpd->mnt_count;
    gpd->gate_state = pfcgpd->gate_state;
    gpd->gate_sig =   pfcgpd->gate_sig;
    gpd->cur_go_act = pfcgpd->cur_go_act;
    gpd->out_go_act = pfcgpd->out_go_act;

    float gate_val = gpd->out_go_act; // goes live whenver it goes live..

    for(int i=0;i<nunits;i++) {
      LeabraUnit* ru = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
      LeabraUnit* pfcu = (LeabraUnit*)pfc_lay->UnitAccess(acc_md, i, mg);

      // with mutex of mnt and out gating, can always just use current value!
      ru->act = gate_val * pfcu->act_eq;
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

void PFCLVPrjnSpec::Connect_Gp(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
				Layer::AccessMode sacc_md, int sgpidx) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  // pre-allocate connections!
  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
    ru->RecvConsPreAlloc(su_nunits, prjn);
  }
  for(int sui=0; sui < su_nunits; sui++) {
    Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
    su->SendConsPreAlloc(ru_nunits, prjn);
  }

  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      if(self_con || (ru != su))
	ru->ConnectFrom(su, prjn);
    }
  }
}

void PFCLVPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  LeabraLayer* lv_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* pfc_lay = (LeabraLayer*)prjn->from.ptr();

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
      tot_pfc_stripes += prj->from->gp_geom.n;
    }
  }

  int n_lv_stripes = lv_lay->gp_geom.n;
  int n_pfc_stripes = pfc_lay->gp_geom.n;

  if(n_lv_stripes <= 1) {	// just full connectivity
    Connect_Gp(prjn, Layer::ACC_LAY, 0, Layer::ACC_LAY, 0);
  }
  else if(n_lv_stripes == n_pfc_stripes) { // just one-to-one
    for(int i=0; i<n_pfc_stripes; i++) {
      Connect_Gp(prjn, Layer::ACC_GP, i, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == n_pfc_stripes + 1) { // full plus one-to-one
    Connect_Gp(prjn, Layer::ACC_GP, 0, Layer::ACC_LAY, 0); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Connect_Gp(prjn, Layer::ACC_GP, i+1, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes) { // multi-pfc just one-to-one
    for(int i=0; i<n_pfc_stripes; i++) {
      Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + i, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes + 1) { // multi-pfc full plus one-to-one
    Connect_Gp(prjn, Layer::ACC_GP, 0, Layer::ACC_LAY, 0); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + i, Layer::ACC_GP, i);
    }
  }
  else if(n_lv_stripes == tot_pfc_stripes + tot_pfc_prjns) { // multi-pfc separate full plus one-to-one
    Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + my_prjn_idx, Layer::ACC_LAY, 0); // full for first prjn
    for(int i=0; i<n_pfc_stripes; i++) { // then gp one-to-one
      Connect_Gp(prjn, Layer::ACC_GP, my_start_stripe + my_prjn_idx + i+1, Layer::ACC_GP, i);
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

  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  if(TestWarning(!recv_lay->unit_groups, "Connect_impl",
		 "requires recv layer to have unit groups!")) {
    return;
  }

  int n_rgps = recv_lay->gp_geom.n;
  int ru_nunits = recv_lay->un_geom.n;

  int n_sends = send_lay->units.leaves;
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
  float send_no_flt = (float)(recv_lay->units.leaves * recv_no) / (float)n_sends;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(n_rgps); // n is n_rgps -- much noisier
  int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
  if(send_no > recv_lay->units.leaves) send_no = recv_lay->units.leaves;

  // pre-allocate connections!
  recv_lay->RecvConsPreAlloc(recv_no, prjn);
  send_lay->SendConsPreAlloc(send_no, prjn);

  for(int gpi=0; gpi<n_rgps; gpi++) {
    int_Array* ari = (int_Array*)send_idx_ars[gpi];
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAtUnGpIdx(rui, gpi);
      for(int sui=0; sui<recv_no; sui++) {
	Unit* su = send_lay->units.Leaf(ari->FastEl(sui));
	ru->ConnectFrom(su, prjn, false, false);
	// final false change to true = ignore errs -- to be expected
      }
    }
  }
}

void MatrixGradRFPrjnSpec::Initialize() {
  invert_nogo = false;
  nogo_offset = 0.0f;
  wt_range.min = 0.0f;
  wt_range.max = 0.1f;
  wt_range.UpdateAfterEdit_NoGui();
  Defaults_init();
}

void MatrixGradRFPrjnSpec::Defaults_init() {
  use_gps = true;
}

void MatrixGradRFPrjnSpec::SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist,
				       int cg_idx) {
  int ru_idx = ru->GetIndex();
  Layer* recv_lay = (Layer*)prjn->layer;
  bool save_invert = invert;

  if(recv_lay->virt_groups)	// get index within unit group..
    ru_idx %= recv_lay->un_geom.n;

  int gp_sz = recv_lay->un_geom.n / 2;
  PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(ru_idx / gp_sz);
  if(go_no == PFCGateSpec::GATE_NOGO) {
    invert = true;
  }
  else {
    invert = false;
  }
  inherited::SetWtFmDist(prjn, cg, ru, dist, cg_idx);
  if(go_no == PFCGateSpec::GATE_NOGO) {
    cg->Cn(cg_idx)->wt += nogo_offset;
  }
  invert = save_invert;
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

static void lay_set_geom(LeabraLayer* lay, int n_stripes, int n_units = -1, bool sp = true,
			 int gp_geom_x=-1, int gp_geom_y=-1) {
  lay->unit_groups = true;
  lay->SetNUnitGroups(n_stripes);
  if(gp_geom_x > 0 && gp_geom_y > 0) {
    lay->gp_geom.x = gp_geom_x;
    lay->gp_geom.y = gp_geom_y;
  }
  else {
    if(n_stripes <= 5) {
      lay->gp_geom.x = n_stripes;
      lay->gp_geom.y = 1;
    }
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
			  int n_units, bool sp, int gp_geom_x=-1, int gp_geom_y=-1)
{
  LeabraLayer* lay = (LeabraLayer*)net->FindLayer(nm);
  if(lay == NULL) return;
  lay_set_geom(lay, n_stripes, n_units, sp, gp_geom_x, gp_geom_y);
}

bool LeabraWizard::PBWM_SetNStripes(LeabraNetwork* net, int n_stripes, int n_units,
				    int gp_geom_x, int gp_geom_y) {
  if(TestError(!net, "PBWM_SetNStripes", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  // this is called as a subroutine a lot too so don't save here -- could do impl but
  // not really worth it..
//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_SetNStripes -- actually saves network specifically");
//   }

  set_n_stripes(net, "PFC", n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "PFC_mnt", n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "PFC_out", n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "Matrix", n_stripes, -1, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "Matrix_mnt", n_stripes, -1, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "Matrix_out", n_stripes, -1, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "SNrThal", n_stripes, -1, false, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "SNrThal_mnt", n_stripes, -1, false, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "SNrThal_out", n_stripes, -1, false, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "LVe", 1, -1, false, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "LVi", 1, -1, false, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "Patch", n_stripes, -1, false, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "SNc", n_stripes, -1, false, gp_geom_x, gp_geom_y);
  net->Build();
  return true;
}

bool LeabraWizard::PBWM(LeabraNetwork* net, bool da_mod_all,
			int n_stripes, bool pfc_learns) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PBWM", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  // first configure PVLV system..
  if(TestError(!PVLV(net, da_mod_all), "PBWM", "could not make PVLV")) return false;

  bool out_gate = true;

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
      int xm = lpos.x + lay->scaled_disp_geom.x + 1;
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
  snrthal_units->bias_spec.SetSpec(bg_bias);

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
      if(pfc_learns)
	net->FindMakePrjn(pfc_m, il, fullprjn, topfc_cons);
      else
	net->FindMakePrjn(pfc_m, il, input_pfc, topfc_cons);
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
  if(pfc_m_new && pfc_learns) {
    for(i=0;i<output_lays.size;i++) {
      Layer* ol = (Layer*)output_lays[i];
      net->FindMakePrjn(pfc_m, ol, fullprjn, topfc_cons);
    }
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
    if(!pfc_learns && (input_lays.size > 0)) {
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

  // this is here, to allow it to get disp_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, n_stripes);

  if(out_gate) {
    if(pfc_o_new) {
      pfc_o->pos.z = pfc_m->pos.z; pfc_o->pos.y = pfc_m->pos.y;
      pfc_o->pos.x = pfc_m->pos.x + pfc_m->disp_geom.x + 2;
      if(!pfc_learns && (input_lays.size > 0)) {
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
      matrix_o->pos.x = matrix_m->pos.x + matrix_m->disp_geom.x + 2;
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
  PBWM_Defaults(net, pfc_learns); // sets all default params and gets selectedits

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

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM -- actually saves network specifically");
  }
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//		PBWM Defaults
/////////////////////////////////////////////////////////////////////////////

bool LeabraWizard::PBWM_Defaults(LeabraNetwork* net, bool pfc_learns) {
  if(!net) {
    if(TestError(!net, "PBWM", "network is NULL -- must be passed and already PBWM configured -- aborting!"))
      return false;
  }

  bool out_gate = true;

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
  LeabraConSpec* fixed_bias = (LeabraConSpec*)bg_bias->FindMakeChild("FixedBias", &TA_LeabraBiasSpec);

  LeabraConSpec* old_matrix_bias = (LeabraConSpec*)bg_bias->children.FindSpecName("MatrixCons");
  if(old_matrix_bias) {
    bg_bias->children.RemoveEl(old_matrix_bias);
  }
  LeabraLayerSpec* rewtargsp = (LeabraLayerSpec*)layers->FindMakeSpec("RewTargLayer", &TA_LeabraLayerSpec);
  ExtRewLayerSpec* pvesp = (ExtRewLayerSpec*)layers->FindMakeSpec(pvenm + "Layer", &TA_ExtRewLayerSpec);
  PVrLayerSpec* pvrsp = (PVrLayerSpec*)layers->FindMakeSpec(pvrnm + "Layer", &TA_PVrLayerSpec);
  PViLayerSpec* pvisp = (PViLayerSpec*)layers->FindMakeSpec(pvinm + "Layer", &TA_PViLayerSpec);
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

  //////////////////////////////////////////////////////////////////////////////////
  // first: all the basic defaults from specs

  units->Defaults();
  cons->Defaults();
  layers->Defaults();
  //  prjns->Defaults();

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  // different PVLV defaults

  lvesp->lv.min_lvi = 0.4f;
//   lvesp->bias_val.un = ScalarValBias::NO_UN;
//   pvisp->bias_val.un = ScalarValBias::NO_UN;
//   pvrsp->bias_val.un = ScalarValBias::NO_UN;

  nvsp->nv.da_gain = 0.1f;
  dasp->da.da_gain = 1.0f;
  dasp->da.pv_gain = 0.1f;

  // do NOT reset this -- unnec override of existing params!
//   matrixsp->matrix.da_gain = 0.1f;

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
  if(pfc_learns) {
    topfc_cons->lrate = .005f;
    topfc_cons->SetUnique("rnd", false);
    topfc_cons->rnd.var = 0.25f;
  }
  else {
    topfc_cons->lrate = 0.0f;
    topfc_cons->SetUnique("rnd", true);
    topfc_cons->rnd.var = 0.0f;
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
  pfc_self->wt_scale.rel = 0.2f;
  pfc_self->SetUnique("savg_cor", true);
  pfc_self->savg_cor.norm_con_n = true;

  matrix_cons->SetUnique("rnd", true);
  matrix_cons->rnd.var = .02f;
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

  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;		// default is no bias learning

  fixed_bias->SetUnique("lrate", true);
  fixed_bias->lrate = 0.0f;		// default is no bias learning

  matrix_units->BioParams(true);
  matrix_units->g_bar.h = .01f; // old syn dep
  matrix_units->g_bar.a = .03f;
  matrix_units->noise_type = LeabraUnitSpec::NETIN_NOISE;
  matrix_units->noise.var = 0.0001f;
  matrix_units->noise_adapt.trial_fixed = true;
  matrix_units->noise_adapt.k_pos_noise = true;
  matrix_units->noise_adapt.mode = NoiseAdaptSpec::PVLV_LVE;
  matrix_units->SetUnique("maxda", true);
  matrix_units->maxda.val = MaxDaSpec::NO_MAX_DA;

  matrixsp->bg_type = MatrixLayerSpec::MAINT;
//   matrixsp->inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
//   matrixsp->inhib.kwta_pt = 0.5f;
  matrixsp->inhib.type = LeabraInhibSpec::KWTA_INHIB;
  matrixsp->inhib.kwta_pt = 0.2f;

  pfcmsp->inhib.kwta_pt = 0.5f;

  snrthalsp->SetUnique("kwta", true);
  snrthalsp->kwta.k_from = KWTASpec::USE_K;
  snrthalsp->kwta.k = 2;
  snrthalsp->inhib.kwta_pt = 0.7f;

  snrthalsp->bg_type = SNrThalLayerSpec::MAINT;

  snrthal_units->BioParams(true);
  snrthal_units->SetUnique("maxda", true);
  snrthal_units->maxda.val = MaxDaSpec::NO_MAX_DA;

  if(out_gate) {
    matrixo_units->SetUnique("g_bar", true);
    matrixo_units->g_bar.h = .02f; matrixo_units->g_bar.a = .06f; // note: 2x..
    matrixo_units->SetUnique("noise_type", false);
    matrixo_units->SetUnique("noise", true);
    matrixo_units->noise.var = 0.0005f;
    matrixo_units->SetUnique("noise_adapt", true);
    matrixo_units->noise_adapt.trial_fixed = true;
    matrixo_units->noise_adapt.k_pos_noise = true;
    matrixo_units->noise_adapt.mode = NoiseAdaptSpec::PVLV_PVI;
    matrixo_units->SetUnique("matrix_noise", true);
    matrixo_units->matrix_noise.patch_noise = false;
    matrixo_units->SetUnique("maxda", false);

    matrixo_cons->SetUnique("lmix", false);
    matrixo_cons->SetUnique("lrate", true);
    matrixo_cons->lrate = .1f;
    matrixo_cons->SetUnique("rnd", false);
    matrixo_cons->SetUnique("wt_sig", true);
    matrixo_cons->wt_sig.gain = 1.0f;
    matrixo_cons->wt_sig.off = 1.0f;

    matrixosp->SetUnique("bg_type", true);
    matrixosp->bg_type = MatrixLayerSpec::OUTPUT;

    mofmpfc_cons->SetUnique("wt_scale", true);
    mofmpfc_cons->wt_scale.rel = 0.02f; // works better with gp-one-to-one
    mofmpfc_cons->SetUnique("lmix", false);

    snrthalosp->SetUnique("kwta", true);
    snrthalosp->kwta.k = 2;
    // inherit the rest from parent guy!
    snrthalosp->SetUnique("inhib", false);
    snrthalosp->SetUnique("inhib_group", false);
    snrthalosp->SetUnique("decay", false);
    snrthalosp->SetUnique("tie_brk", false);
    snrthalosp->SetUnique("ct_inhib_mod", false);

    snrthalosp->bg_type = SNrThalLayerSpec::OUTPUT;

    fmpfcmnt_cons->SetUnique("wt_scale", true);
    fmpfcmnt_cons->wt_scale.rel = 1.0f; // .2 might be better in some cases
    fmpfcout_cons->SetUnique("wt_scale", true);
    fmpfcout_cons->wt_scale.rel = 1.0f; // 2 might be better in some cases
  }

  pfc_units->BioParams(true);
  pfc_units->SetUnique("g_bar", true);
  if(pfc_learns)
    pfc_units->g_bar.h = .5f;	// weaker act maint for learning pfc..
  else
    pfc_units->g_bar.h = 1.0f;
  pfc_units->g_bar.a = 2.0f;
  pfc_units->SetUnique("dt", true);
  pfc_units->dt.vm = .1f;	// slower is better..  .1 is even better!

  snrthal_units->dt.vm = 0.3f;	// was 0.1f -- not good!
  snrthal_units->g_bar.l = .8f;

  /////////////////////////
  // some key stuff from PVLV:

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

  lv_units->SetUnique("maxda", false);
  pv_units->SetUnique("act", true);
  pv_units->SetUnique("act_fun", true);
  pv_units->SetUnique("dt", true);
  pv_units->act_fun = LeabraUnitSpec::NOISY_LINEAR;
  pv_units->act.gelin = false;
  pv_units->act.thr = .17f;
  pv_units->act.gain = 220.0f;
  pv_units->act.nvar = .01f;
  pv_units->v_m_init.mean = 0.15f;
  pv_units->e_rev.l = 0.15f;
  pv_units->e_rev.i = 0.15f;
  pv_units->g_bar.l = .1f;
  pv_units->g_bar.h = .03f;  pv_units->g_bar.a = .09f;
  pv_units->dt.vm_eq_cyc = 100; // go straight to equilibrium!
  pv_units->SetUnique("maxda", true);
  pv_units->maxda.val = MaxDaSpec::NO_MAX_DA;
  pv_units->SetUnique("act", true);
  pv_units->act.avg_dt = 0.0f;

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

  da_units->SetUnique("act_range", true);
  da_units->act_range.max = 2.0f;
  da_units->act_range.min = -2.0f;
  da_units->act_range.UpdateAfterEdit();
  da_units->SetUnique("clamp_range", true);
  da_units->clamp_range.max = 2.0f;
  da_units->clamp_range.min = -2.0f;
  da_units->clamp_range.UpdateAfterEdit();
  da_units->SetUnique("maxda", true);
  da_units->maxda.val = MaxDaSpec::NO_MAX_DA;
  da_units->SetUnique("act", true);
  da_units->act.avg_dt = 0.0f;

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
"Default is KWTA_AVG_INHIB with kwta_pt = .7 -- more competition but with some flexibility from avg-based computation");

//       snrthal_units->SelectForEditNm("g_bar", edit, "snr_thal", subgp);
//       snrthal_units->SelectForEditNm("dt", edit, "snr_thal", subgp);
    snrthalsp->SelectForEditNm("snrthal", edit, "snrthal", subgp);
//     snrthal_units->SelectForEditNm("act", edit, "snrthal", subgp);
    sncsp->SelectForEditNm("snc", edit, "snc", subgp);
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

  net->StructUpdate(true);

  net->RemoveUnits();
  net->layers.gp.RemoveName("PBWM_PFC");
  net->layers.gp.RemoveName("PBWM_BG");
  net->layers.gp.RemoveName("PVLV");

  net->specs.gp.RemoveName("PFC_BG_Prjns");
  net->specs.gp.RemoveName("PFC_BG_Layers");
  net->specs.gp.RemoveName("PFC_BG_Cons");
  net->specs.gp.RemoveName("PFC_BG_Units");

  net->CheckSpecs();		// could have nuked dependent specs!

  net->StructUpdate(false);

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_Remove after -- actually saves network specifically");
  }
  return true;
}


