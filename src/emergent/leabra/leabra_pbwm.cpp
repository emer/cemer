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
       if(!((tols->GetTypeDef() == &TA_MatrixLayerSpec) ||
	    (tols->GetTypeDef() == &TA_XMatrixLayerSpec))) continue; // only to matrix
       for(int j=0;j<send_gp->size; j++) {
	 ((LeabraUnit*)send_gp->Un(j))->misc_1 = snd_val;
       }
     }
     );
}

void PatchLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  ScalarValLayerSpec::Compute_CycleStats(lay, net);
  // do NOT report lvi value!
  Send_LVeToMatrix(lay, net);
}


void SNcLayerSpec::Initialize() {
  stripe_da_gain = 1.0f;
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
//	MatrixConSpec		//
//////////////////////////////////

void MatrixConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
  wt_sig.off = 1.0f;

  SetUnique("xcal", true);
  xcal.mvl_mix = 0.005f;
  xcal.s_mix = 0.8f;

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
  noise.var = 5.0e-5f;
  SetUnique("noise_adapt", true);
  noise_adapt.trial_fixed = true;
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

  LeabraLayer* lay = u->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  if(ls->InheritsFrom(&TA_XMatrixLayerSpec)) {
    int gp_sz = mugp->leaves / 3;
    XPFCGateSpec::GateSignal go_no = (XPFCGateSpec::GateSignal)(u->idx / gp_sz);
    if(go_no == XPFCGateSpec::GATE_OUT_GO) {
      XMatrixLayerSpec* xmls = (XMatrixLayerSpec*)lay->GetLayerSpec();
      noise_amp *= xmls->matrix.out_noise_amp; // extra noise for output gating guys
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

void MatrixRndGoSpec::Initialize() {
  nogo_thr = 50;
  nogo_p = .1f;
  nogo_da = 10.0f;
}

void MatrixGateBiasSpec::Initialize() {
  one_bias = true;
  bias = 5.0f;
  mnt_nogo = 5.0f;
  empty_go = 5.0f;
  out_pvr = 5.0f;
  mnt_pvr = 0.0f;
}

void MatrixGateBiasSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(one_bias) {
    mnt_nogo = bias;
    empty_go = bias;
    out_pvr = bias;
  }
}

void MatrixMiscSpec::Initialize() {
  da_gain = 1.0f;
  mnt_encode_gain = 0.1f;
  neg_da_bl = 0.0f; // 0.0002f;
  neg_gain = 1.0f; // 1.5f;
  no_snr_mod = false;
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
  gate_bias.UpdateAfterEdit();
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
      if(fmlay->spec.SPtr()->InheritsFrom(TA_SNrThalLayerSpec)) snr_lay = fmlay;
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
  return true;
}

void MatrixLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  LabelUnits(lay);
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
  int gp_sz = mugp->leaves / 2;
  bool nogo_rnd_go = (mugp->misc_state1 == PFCGateSpec::NOGO_RND_GO);

  int idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(idx / gp_sz);

    float bias_dav = 0.0f;

    if(bg_type == OUTPUT) {	// output gating guy
      if(net->pv_detected) {	// PV reward trial -- bias output gating
	// only if pfc is maintaining..
	if(pfc_mnt_cnt > 0)
	  bias_dav = gate_bias.out_pvr; // cur_dav is almost certainly 0
      }
      else {			// not a PV trial
	if(pfc_mnt_cnt > 0 && !nogo_rnd_go) // currently maintaining: bias NoGo for everything
	  bias_dav = -gate_bias.mnt_nogo;
      }
    }
    else {			// MAINT
      if(net->pv_detected) {
	// only if pfc is maintaining, bias output gating
	if(pfc_mnt_cnt > 0)
	  bias_dav = gate_bias.mnt_pvr;
      }
      else {			// not a PV trial
	if(pfc_mnt_cnt > 0 && !nogo_rnd_go) // currently maintaining: bias NoGo for everything
	  bias_dav = -gate_bias.mnt_nogo;
	else			// otherwise, bias to maintain/update
	  bias_dav = gate_bias.empty_go;
      }
    }
    // don't set here because we need the values for computation otherwise
//     u->dav = tonic_da + bias_dav;
    Compute_UnitBiasDaMod(u, bias_dav + tonic_da, go_no);
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

void MatrixLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // grab the da value in addition to activation
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->act_m2 = u->act_eq;
    u->act_p2 = u->dav;		// store mid-minus dav value
  }
}

void MatrixLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  // grab the dav in minus phase
  if(net->phase_no == 0) {
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
      u->misc_2 = u->dav;	// store end-of-minus dav value
    }
  }
  else if(net->phase_no == 1) {
    // end of plus -- compute da value used for learning
    Compute_LearnDaVal(lay, net);
  }
}

// this is called at end of plus phase, to establish a da value for driving learning

void MatrixLayerSpec::Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net) {
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
    
    int idx = 0;
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
      PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(idx / gp_sz);

      // critical gating activation value is mid-minus state
      float act_val = u->act_m2;
      float snrthal_act = snr_u->act_m2;

      float lrn_dav = 0.0f;	// learning dopamine value -- compute it!

      if(bg_type == OUTPUT) {
	if((gate_sig == PFCGateSpec::GATE_NOGO) || (gate_sig == PFCGateSpec::GATE_MNT_GO))
	  snrthal_act = 0.0f;	// if no OUT_GO, nothing for us..

	if(matrix.no_snr_mod)	// disable -- just for testing!
	  snrthal_act = 1.0f;

	lrn_dav = matrix.da_gain * snrthal_act * u->dav; // dav is current plus phase -- good for out gating
      }
      else {			// MAINT
	if((gate_sig == PFCGateSpec::GATE_NOGO) || (gate_sig == PFCGateSpec::GATE_OUT_GO))
	  snrthal_act = 0.0f;	// if no MNT_GO, nothing for us..

	if(matrix.no_snr_mod)	// disable -- just for testing!
	  snrthal_act = 1.0f;

	float lv_delta = u->misc_2 - u->act_p2; // end-of-minus - mid-minus delta
	if(pfc_mnt_cnt <= 0)			 // stripe is empty -- need encode gain
	  lv_delta = u->misc_2 * matrix.mnt_encode_gain; // just a multiplier on raw, instead of delta

        lrn_dav = matrix.da_gain * snrthal_act * lv_delta;
      }

      if(nogo_rnd_go)
	lrn_dav += rnd_go.nogo_da; // output gating also gets this too

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
    if(mugp->misc_state1 >= PFCGateSpec::NOGO_RND_GO)
      mugp->misc_state1 = PFCGateSpec::INIT_STATE;
  }
}

void MatrixLayerSpec::Compute_NoGoRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];

    if((int)fabs((float)mugp->misc_state) > rnd_go.nogo_thr) {
      if(Random::ZeroOne() < rnd_go.nogo_p) {
	mugp->misc_state1 = PFCGateSpec::NOGO_RND_GO;
      }
    }
  }
}

void MatrixLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    Compute_ClearRndGo(lay, net);
    Compute_NoGoRndGo(lay, net);
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

//////////////////////////////////
//	SNrThal Layer Spec	//
//////////////////////////////////

void SNrThalMiscSpec::Initialize() {
  go_thr = 0.1f;
  net_off = 0.2f;
  rnd_go_inc = 0.2f;
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
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_MatrixLayerSpec);

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
  MatrixLayerSpec* mls = (MatrixLayerSpec*)matrix_lay->spec.SPtr();
  SetUnique("bg_type", true);
  bg_type = (BGType)mls->bg_type;

  return true;
}

void SNrThalLayerSpec::Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net) {
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_MatrixLayerSpec);
//  MatrixLayerSpec* mls = (MatrixLayerSpec*)matrix_lay->spec.SPtr();

  float net_off_rescale = 1.0f / (1.0f + snrthal.net_off);

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    float gonogo = 0.0f;
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    MatrixUnitSpec* us = (MatrixUnitSpec*)matrix_lay->unit_spec.SPtr();
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
      if(sum_go + sum_nogo > 0.0f) {
	gonogo = (sum_go - sum_nogo) / (sum_go + sum_nogo);
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


//////////////////////////////////
//	PFC Layer Spec		//
//////////////////////////////////

void PFCGateSpec::Initialize() {
  base_gain = 0.0f;
  go_gain = 1.0f;
  graded_out_go = true;
  go_learn_base = 0.06f;
  go_learn_mod = 1.0f - go_learn_base;
  go_netin_gain = 0.01f;
  clear_decay = 0.0f;
  out_go_clear = true;
  off_accom = 0.0f;
}

void PFCGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  go_gain = 1.0f - base_gain;
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
  gate.UpdateAfterEdit();
}

void PFCLayerSpec::Defaults() {
  inherited::Defaults();
  gate.Defaults();
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

void PFCLayerSpec::GetSNrThalLayers(LeabraLayer* lay, LeabraLayer*& snrthal_mnt, LeabraLayer*& snrthal_out) {
  snrthal_mnt = NULL;
  snrthal_out = NULL;
  if(lay->units.leaves == 0) return;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(!fmlay) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayerSpec* ls = (LeabraLayerSpec*)fmlay->spec.SPtr();
      if(ls->InheritsFrom(&TA_SNrThalLayerSpec)) {
	if(((SNrThalLayerSpec*)ls)->bg_type == SNrThalLayerSpec::OUTPUT)
	  snrthal_out = fmlay;
	else
	  snrthal_mnt = fmlay;
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
		"requires LeabraNetwork min_minus_cycle > 0, I just set it to 30 for you")) {
    net->mid_minus_cycle = 30;
  }

  if(lay->CheckError(net->min_cycles < net->mid_minus_cycle, quiet, rval,
		"requires LeabraNetwork min_cycles >= mid_minus_cycle + 5, I just set it for you")) {
    net->min_cycles = net->mid_minus_cycle + 5;
  }

  if(lay->CheckError(net->sequence_init != LeabraNetwork::DO_NOTHING, quiet, rval,
		"requires network sequence_init = DO_NOTHING, I just set it for you")) {
    net->sequence_init = LeabraNetwork::DO_NOTHING;
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
  else {
    TestWarning(true, "CheckConfig", "No OUTPUT SNrThal projection found -- this will reduce functionality of PFC layer significantly -- consider running wizard to create it");
  }

  return true;
}

void PFCLayerSpec::Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    ugp->misc_state1 = PFCGateSpec::INIT_STATE;
    ugp->misc_state2 = PFCGateSpec::GATE_NOGO;
    ugp->misc_float = 0.0f;
    ugp->misc_float1 = 0.0f;	// reset raw gating signals..
  }
}

void PFCLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Compute_TrialInitGates(lay, net);
}

void PFCLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_MidMinus(lay, net);
  Compute_Gating(lay, net);	// basic gating happens in mid minus
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
  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)snrthal_mnt->spec.SPtr();
  float go_thr_mnt = snrthalsp->snrthal.go_thr;
  float go_thr_out = go_thr_mnt;
  if(snrthal_out) {
    go_thr_out = ((SNrThalLayerSpec*)snrthal_out->spec.SPtr())->snrthal.go_thr;
  }

  // this is the mid minus version -- when gating actually happens

  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp_mnt = (LeabraUnit_Group*)snrthal_mnt->units.gp[mg];
    LeabraUnit* snr_mnt_u = (LeabraUnit*)snrgp_mnt->Leaf(0);
    LeabraUnit* snr_out_u = NULL;
    if(snrthal_out) {
      LeabraUnit_Group* snrgp_out = (LeabraUnit_Group*)snrthal_out->units.gp[mg];
      snr_out_u = (LeabraUnit*)snrgp_out->Leaf(0);
    }

    int pfc_mnt_cnt = ugp->misc_state; // is pfc maintaining or not?

    float lrn_go_act = 0.0f;	// activation of go gating unit (out or mnt depending -- used for learning)
    float out_go_act = 0.0f;	// activation of output gating unit specifically

    PFCGateSpec::GateSignal gate_sig_out = PFCGateSpec::GATE_NOGO;
    PFCGateSpec::GateSignal gate_sig_mnt = PFCGateSpec::GATE_NOGO;

    // default NOGO results
    if(pfc_mnt_cnt > 0) // full stripe
      ugp->misc_state1 = PFCGateSpec::MAINT_NOGO;
    else
      ugp->misc_state1 = PFCGateSpec::EMPTY_NOGO;
    ugp->misc_state2 = PFCGateSpec::GATE_NOGO;

    // maintenance gating signal
    if(snr_mnt_u->act_eq > go_thr_mnt) {
      gate_sig_mnt = PFCGateSpec::GATE_GO;
      lrn_go_act = snr_mnt_u->act_eq;
      // provide summary just based on maintenance gating
      ugp->misc_state2 = PFCGateSpec::GATE_MNT_GO;
      if(pfc_mnt_cnt > 0) { // full stripe
	Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net);	 // clear maint currents
	ugp->misc_state1 = PFCGateSpec::MAINT_MNT_GO;
      }
      else {
	ugp->misc_state1 = PFCGateSpec::EMPTY_MNT_GO;
      }
    }

    // output gating signal
    if(snr_out_u && snr_out_u->act_eq > go_thr_out) {
      gate_sig_out = PFCGateSpec::GATE_GO;
      lrn_go_act = MAX(snr_out_u->act_eq, lrn_go_act);
      if(gate.graded_out_go)
	out_go_act = snr_out_u->act_eq;
      else
	out_go_act = 1.0f; // go all the way
      // provide integrated summary
      if(gate_sig_mnt == PFCGateSpec::GATE_NOGO) {
	// only output gating fired -- simple
	ugp->misc_state2 = PFCGateSpec::GATE_OUT_GO;
	if(pfc_mnt_cnt > 0) // full stripe
	  ugp->misc_state1 = PFCGateSpec::MAINT_OUT_GO;
	else
	  ugp->misc_state1 = PFCGateSpec::EMPTY_OUT_GO;
      }
      else {
	// both output and maint gating fired..
	ugp->misc_state2 = PFCGateSpec::GATE_OUT_MNT_GO;
	if(pfc_mnt_cnt > 0) // full stripe
	  ugp->misc_state1 = PFCGateSpec::MAINT_OUT_MNT_GO;
	else
	  ugp->misc_state1 = PFCGateSpec::EMPTY_OUT_MNT_GO;
      }
    }

    // misc_float has the go_learn_base factor incorporated
    ugp->misc_float = gate.go_learn_base + (gate.go_learn_mod * lrn_go_act);
    // misc_float1 includes net output gating multiplier:
    ugp->misc_float1 = gate.base_gain + (gate.go_gain * out_go_act);
  }
  SendGateStates(lay, net);
}

void PFCLayerSpec::SendGateStates(LeabraLayer* lay, LeabraNetwork*) {
  LeabraLayer* snrthal_mnt = NULL;
  LeabraLayer* snrthal_out = NULL;
  GetSNrThalLayers(lay, snrthal_mnt, snrthal_out);
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_mnt = FindLayerFmSpec(snrthal_mnt, mtx_prjn_idx, &TA_MatrixLayerSpec);
  LeabraLayer* matrix_out = NULL;
  if(snrthal_out) {
    matrix_out = FindLayerFmSpec(snrthal_out, mtx_prjn_idx, &TA_MatrixLayerSpec);
  }
  int mg;
  for(mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_mnt->units.gp[mg];
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_mnt->units.gp[mg];
    // everybody gets gate state info from PFC!
    snrgp->misc_state = mugp->misc_state = ugp->misc_state;
    snrgp->misc_state1 = ugp->misc_state1; 
    if(mugp->misc_state1 < PFCGateSpec::NOGO_RND_GO) { // don't override random go signals
      mugp->misc_state1 = ugp->misc_state1;
    }
    snrgp->misc_state2 = mugp->misc_state2 = ugp->misc_state2;
    snrgp->misc_float = mugp->misc_float = ugp->misc_float;
    snrgp->misc_float1 = mugp->misc_float1 = ugp->misc_float1;
    if(matrix_out) {
      snrgp = (LeabraUnit_Group*)snrthal_out->units.gp[mg];
      mugp = (LeabraUnit_Group*)matrix_out->units.gp[mg];
      snrgp->misc_state = mugp->misc_state = ugp->misc_state;
      snrgp->misc_state1 = ugp->misc_state1; 
      if(mugp->misc_state1 < PFCGateSpec::NOGO_RND_GO) { // don't override random go signals
	mugp->misc_state1 = ugp->misc_state1;
      }
      snrgp->misc_state2 = mugp->misc_state2 = ugp->misc_state2;
      snrgp->misc_float = mugp->misc_float = ugp->misc_float;
      snrgp->misc_float1 = mugp->misc_float1 = ugp->misc_float1;
    }
  }
}

void PFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == 1) {
    Compute_Gating_Final(lay, net);	// final gating
  }
}

void PFCLayerSpec::Compute_Gating_Final(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];

    // for NOGO, just update the misc_state counter
    if(ugp->misc_state1 == PFCGateSpec::EMPTY_NOGO || 
       ugp->misc_state1 == PFCGateSpec::MAINT_NOGO) {
      if(ugp->misc_state > 0)
	ugp->misc_state++;
      else
	ugp->misc_state--;
    }
    // look for store condition
    else if(ugp->misc_state1 == PFCGateSpec::MAINT_MNT_GO ||
	    ugp->misc_state1 == PFCGateSpec::EMPTY_MNT_GO ||
	    ugp->misc_state1 == PFCGateSpec::EMPTY_OUT_MNT_GO) {
      Compute_MaintUpdt_ugp(ugp, STORE, lay, net);     // store it
      if(ugp->misc_state1 == PFCGateSpec::MAINT_MNT_GO)
	ugp->misc_state = 1;	// just cleared mnt act, but couldn't reset mnt ctr
      else if(ugp->misc_state <= 0) ugp->misc_state = 1;
      else ugp->misc_state++;
    }
    // or basic output gate with no veto from maint
    else if(ugp->misc_state1 == PFCGateSpec::MAINT_OUT_GO ||
	    ugp->misc_state1 == PFCGateSpec::EMPTY_OUT_GO) {
      if(gate.out_go_clear && (ugp->misc_state > 0)) {		       // maintaining
	Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net);     // clear it!
	ugp->misc_state = 0;			     // empty
      }
      else {
	ugp->misc_state--;	// effectively a nogo -- continue incrementing
      }
    }
    // finally, must be a MAINT_OUT_MNT_GO -- veto normal clear state
    else {
      // just continue as if nothing happened
      if(ugp->misc_state > 0)
	ugp->misc_state++;
      else
	ugp->misc_state--;
    }
  }
}

void PFCLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
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

  PFCLayerSpec* pfcsp = (PFCLayerSpec*)pfc_lay->spec.SPtr();
  kwta = pfcsp->kwta;
  gp_kwta = pfcsp->gp_kwta;
  inhib_group = pfcsp->inhib_group;
  inhib.type = pfcsp->inhib.type;
  inhib.kwta_pt = pfcsp->inhib.kwta_pt;
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
      
      ru->act = gate_val * pfcu->act;
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 	New Xperimental version of PBWM


/////////////////////////////////////////////////////

void XMatrixDaSpec::Initialize() {
  gain = 1.0f;
  perf_contrast = 0.0f;
  mnt_lrn_contrast = 0.5f;
  out_lrn_contrast = 1.0f;
}

void XMatrixMiscSpec::Initialize() {
  one_bias_da = true;
  bias_da = 5.0f;
  mnt_nogo_da = 5.0f;
  empty_go_da = 5.0f;
  out_pvr_da = 5.0f;
  out_noise_amp = 10.0f;
  perf_gain = 0.0f;
  neg_da_bl = 0.0f;
  neg_gain = 1.0f;
  no_snr_mod = false;
}

void XMatrixMiscSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(one_bias_da) {
    mnt_nogo_da = bias_da;
    empty_go_da = bias_da;
    out_pvr_da = bias_da;
  }
}

void XMatrixLayerSpec::Initialize() {
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
}

void XMatrixLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  matrix.UpdateAfterEdit();
}

void XMatrixLayerSpec::Defaults() {
  inherited::Defaults();
  matrix.Defaults();
  da.Defaults();
  Initialize();
}

void XMatrixLayerSpec::HelpConfig() {
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

bool XMatrixLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
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
      if(fmlay->spec.SPtr()->InheritsFrom(TA_XSNrThalLayerSpec)) snr_lay = fmlay;
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
  return true;
}

void XMatrixLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  UNIT_GP_ITR(lay, 
	      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
	      u->misc_1 = 0.0f;
	      u->misc_2 = 0.0f;
	      );
  LabelUnits(lay);
}

void XMatrixLayerSpec::Compute_UnitPerfDaMod(LeabraUnit* u, float perf_dav,
					     float act_val, int go_no) {
  if(go_no == (int)XPFCGateSpec::GATE_NOGO) {
    if(perf_dav >= 0.0f) {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = perf_dav * ((1.0f - da.perf_contrast) + (da.perf_contrast * act_val));
      if(go_nogo_gain.on) u->vcb.g_a *= go_nogo_gain.nogo_p;
    }
    else {
      u->vcb.g_h = -matrix.neg_gain * perf_dav * ((1.0f - da.perf_contrast) + (da.perf_contrast * act_val));
      if(go_nogo_gain.on) u->vcb.g_h *= go_nogo_gain.nogo_n;
      u->vcb.g_a = 0.0f;
    }
  }
  else {			// must be a GO
    if(perf_dav >= 0.0f)  { 
      u->vcb.g_h = perf_dav * ((1.0f - da.perf_contrast) + (da.perf_contrast * act_val));
      if(go_nogo_gain.on) u->vcb.g_h *= go_nogo_gain.go_p;
      u->vcb.g_a = 0.0f;
    }
    else {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = -matrix.neg_gain * perf_dav * ((1.0f - da.perf_contrast) + (da.perf_contrast * act_val));
      if(go_nogo_gain.on) u->vcb.g_a *= go_nogo_gain.go_n;
    }
  }
}

void XMatrixLayerSpec::Compute_DaPerfMod(LeabraLayer* lay, LeabraUnit_Group* mugp,
					 int gpidx, LeabraNetwork* net) {
  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_PVLVDaLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.SPtr();
  float tonic_da = dals->da.tonic_da;

  int pfc_mnt_cnt = mugp->misc_state; // is pfc maintaining or not?

  int gp_sz = mugp->leaves / 3;

  bool nogo_rnd_go = (mugp->misc_state1 == XPFCGateSpec::NOGO_RND_GO);

  int idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    XPFCGateSpec::GateSignal go_no = (XPFCGateSpec::GateSignal)(idx / gp_sz);

    float act_val = u->act_eq; // current activity

    float cur_dav = matrix.perf_gain * (u->dav - tonic_da); // exclude tonic
    float new_dav = 0.0f;

    // apply da selectively to out vs mnt to bias
    if(net->pv_detected) {
      // only if pfc is maintaining, bias output gating
      if(pfc_mnt_cnt > 0 && (go_no != XPFCGateSpec::GATE_MNT_GO)) {
	new_dav = cur_dav + matrix.out_pvr_da; // cur_dav is almost certainly 0
      }
      else {			   // recompute wth out_pvr_da
	new_dav = 0.0f;	   // no da for MNT GO in out go situation
      }
    }
    else {			// not a PV trial
      if(pfc_mnt_cnt > 0 && !nogo_rnd_go) {	// currently maintaining: bias NoGo for everything
	new_dav = -matrix.mnt_nogo_da;
      }
      else {			// otherwise, bias to maintain/update
	if(go_no != XPFCGateSpec::GATE_OUT_GO)  {
	  new_dav = cur_dav + matrix.empty_go_da; // cur_dav should have LVe evaluation of this item	
	}
	else {
	  new_dav = 0.0f;	// no da for OUT Go in mnt go 
	}
      }
    }
    float perf_dav = da.gain * (new_dav + tonic_da);
    u->dav = perf_dav;		// record visually
    u->misc_2 = perf_dav;	// and for learn mod
    Compute_UnitPerfDaMod(u, perf_dav, act_val, go_no);
    idx++;
  }
}

void XMatrixLayerSpec::Compute_DaLearnMod(LeabraLayer* lay, LeabraUnit_Group* mugp,
					  int gpidx, LeabraNetwork* net) {
  int snr_prjn_idx = 0;
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_XSNrThalLayerSpec);

  LeabraUnit_Group* snrug = (LeabraUnit_Group*)snr_lay->units.gp[gpidx];
  LeabraUnit* snr_mnt_u = (LeabraUnit*)snrug->Leaf(0);
  LeabraUnit* snr_out_u = (LeabraUnit*)snrug->Leaf(1);

  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_PVLVDaLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.SPtr();
  float tonic_da = dals->da.tonic_da;

  // computed live in the PFC -- we'll be 1 trial behind..
  XPFCGateSpec::GateSignal gate_sig = (XPFCGateSpec::GateSignal)mugp->misc_state2;

  bool nogo_rnd_go = (mugp->misc_state1 == XPFCGateSpec::NOGO_RND_GO);

  int gp_sz = mugp->leaves / 3;
    
  int idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, mugp->, i) {
    XPFCGateSpec::GateSignal go_no = (XPFCGateSpec::GateSignal)(idx / gp_sz);

    // critical signal is in the minus phase
    float act_val = u->act_m;
    float snrthal_act = MAX(snr_mnt_u->act_m, snr_out_u->act_m);

    float perf_dav = u->misc_2;	// performance da: important: includes the tonic_da!
    float cur_dav = u->dav - tonic_da; // exclude tonic

    if(gate_sig == XPFCGateSpec::GATE_NOGO)
      snrthal_act = 0.0f;	// if we don't go, nothing happens
    else {
      if(gate_sig == XPFCGateSpec::GATE_MNT_GO && go_no == XPFCGateSpec::GATE_OUT_GO) {
	snrthal_act = 0.0f;	// don't reward output for mnt gating
      }
      else if(gate_sig == XPFCGateSpec::GATE_OUT_GO && go_no == XPFCGateSpec::GATE_MNT_GO) {
	snrthal_act = 0.0f;	// don't reward mnt for output gating
      }
    }

    if(matrix.no_snr_mod)	// disable!
      snrthal_act = 1.0f;

    float lrn_dav = da.gain * snrthal_act * cur_dav;
    if(nogo_rnd_go) {
      lrn_dav += rnd_go.nogo_da; 
    }

    float dav = perf_dav + lrn_dav;
    u->dav = dav;		// make it show up in display

    Compute_UnitPerfDaMod(u, perf_dav, act_val, go_no); // baseline is perf dav
    // now add learn dav:
    if(go_no == (int)XPFCGateSpec::GATE_NOGO) {
      if(lrn_dav >= 0.0f) {
	float ga = 0.0f;
	if(gate_sig == XPFCGateSpec::GATE_MNT_GO)
	  ga = lrn_dav * ((1.0f - da.mnt_lrn_contrast) + (da.mnt_lrn_contrast * act_val));
	else
	  ga = lrn_dav * ((1.0f - da.out_lrn_contrast) + (da.out_lrn_contrast * act_val));
	if(go_nogo_gain.on) ga *= go_nogo_gain.nogo_p;
	u->vcb.g_a += ga;
      }
      else {
	float gh = 0.f;
	if(gate_sig == XPFCGateSpec::GATE_MNT_GO)
	  gh = -matrix.neg_gain * lrn_dav * ((1.0f - da.mnt_lrn_contrast) + (da.mnt_lrn_contrast * act_val));
	else 
	  gh = -matrix.neg_gain * lrn_dav * ((1.0f - da.out_lrn_contrast) + (da.out_lrn_contrast * act_val));
	if(go_nogo_gain.on) gh *= go_nogo_gain.nogo_n;
	u->vcb.g_h += gh;
      }
    }
    else if(go_no == XPFCGateSpec::GATE_MNT_GO) {
      if(lrn_dav >= 0.0f)  {
	float gh = lrn_dav * ((1.0f - da.mnt_lrn_contrast) + (da.mnt_lrn_contrast * act_val));
	if(go_nogo_gain.on) gh *= go_nogo_gain.go_p;
	u->vcb.g_h += gh;
      }
      else {
	float ga = -matrix.neg_gain * lrn_dav * ((1.0f - da.mnt_lrn_contrast) + (da.mnt_lrn_contrast * act_val));
	if(go_nogo_gain.on) ga *= go_nogo_gain.go_n;
	u->vcb.g_a += ga;
      }
    }
    else if(go_no == XPFCGateSpec::GATE_OUT_GO) {
      if(lrn_dav >= 0.0f)  {
	float gh = lrn_dav * ((1.0f - da.out_lrn_contrast) + (da.out_lrn_contrast * act_val));
	if(go_nogo_gain.on) gh *= go_nogo_gain.go_p;
	u->vcb.g_h += gh;
      }
      else {
	float ga = -matrix.neg_gain * lrn_dav * ((1.0f - da.out_lrn_contrast) + (da.out_lrn_contrast * act_val));
	if(go_nogo_gain.on) ga *= go_nogo_gain.go_n;
	u->vcb.g_a += ga;
      }
    }
    idx++;
  }
}

void XMatrixLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_ApplyInhib(lay, net);
  
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    if(net->phase_no == 0)
      Compute_DaPerfMod(lay, mugp, gi, net);
    else if(net->phase_no == 1)
      Compute_DaLearnMod(lay, mugp, gi, net);
  }
}

void XMatrixLayerSpec::Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    if(mugp->misc_state1 >= XPFCGateSpec::NOGO_RND_GO)
      mugp->misc_state1 = XPFCGateSpec::EMPTY_MNT_GO;
  }
}

void XMatrixLayerSpec::Compute_NoGoRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];

    if((int)fabs((float)mugp->misc_state) > rnd_go.nogo_thr) {
      if(Random::ZeroOne() < rnd_go.nogo_p) {
	mugp->misc_state1 = XPFCGateSpec::NOGO_RND_GO;
      }
    }
  }
}

void XMatrixLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  // compute nogo rnd go at start of minus phase
  if(net->phase_no == 0) {
    Compute_ClearRndGo(lay, net);
    Compute_NoGoRndGo(lay, net);
  }
  inherited::Compute_HardClamp(lay, net);
}

bool XMatrixLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return true;
}

bool XMatrixLayerSpec::Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return false;
}

bool XMatrixLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return false;
}

void XMatrixLayerSpec::LabelUnits_impl(Unit_Group* ugp) {
  int gp_sz = ugp->leaves / 3;
  for(int i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    XPFCGateSpec::GateSignal go_no = (XPFCGateSpec::GateSignal)(i / gp_sz);
    if(go_no == XPFCGateSpec::GATE_MNT_GO)
      u->name = "GoM";
    else if(go_no == XPFCGateSpec::GATE_OUT_GO)
      u->name = "GoO";
    else
      u->name = "No";
  }
}

void XMatrixLayerSpec::LabelUnits(LeabraLayer* lay) {
  UNIT_GP_ITR(lay, LabelUnits_impl(ugp); );
}

//////////////////////////////////
//	XSNrThal Layer Spec	//
//////////////////////////////////

void XSNrThalLayer::Initialize() {
  
}

void XSNrThalLayer::BuildUnits() {
  inherited::BuildUnits();
  mnt_units.Reset();
  out_units.Reset();

  for(int mg=0; mg<units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)units.gp[mg];
    mnt_units.Add((LeabraUnit*)rugp->SafeEl(0));
    out_units.Add((LeabraUnit*)rugp->SafeEl(1));
  }
}

void XSNrThalMiscSpec::Initialize() {
  go_thr = 0.5f;
  net_off = -0.1f;
  rnd_go_inc = 0.2f;
}

void XSNrThalLayerSpec::Initialize() {
  min_obj_type = &TA_XSNrThalLayer;
  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  decay.phase = 0.0f;
  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 1;
  gp_kwta.gp_i = true;
  gp_kwta.gp_g = 0.95f;
  SetUnique("tie_brk", true);	// turn on tie breaking by default
  tie_brk.on = true;
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .45f;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  mnt_out_inhib.type = LeabraInhibSpec::KWTA_INHIB;
  mnt_out_inhib.kwta_pt = .25f;
  mnt_inhib = true;
  out_inhib = true;
  mnt_kwta.pct = .75f;
  out_kwta.k_from = KWTASpec::USE_K;
  out_kwta.k = 3;
}

void XSNrThalLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void XSNrThalLayerSpec::HelpConfig() {
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

bool XSNrThalLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(CheckError(mnt_out_inhib.type != LeabraInhibSpec::KWTA_INHIB, quiet, rval,
		"only KWTA_INHIB is currently supported for mnt/out inhibition -- I set it to that for you.")) {
    mnt_out_inhib.type = LeabraInhibSpec::KWTA_INHIB;
  }

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }
  if(lay->CheckError(lay->un_geom.n != 2, quiet, rval,
		"layer does not have 2 units per group -- I just fixed that for you")) {
    lay->un_geom.y = 2; lay->un_geom.x = 1;  lay->un_geom.UpdateAfterEdit();
  }

  // must have the appropriate ranges for unit specs..
  //  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  // check recv connection
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_XMatrixLayerSpec);

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

void XSNrThalLayerSpec::Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net) {
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_XMatrixLayerSpec);
//  XMatrixLayerSpec* mls = (XMatrixLayerSpec*)matrix_lay->spec.SPtr();

  float net_off_rescale = 1.0f / (1.0f + snrthal.net_off);

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    MatrixUnitSpec* us = (MatrixUnitSpec*)matrix_lay->unit_spec.SPtr();
    float mnt_go_net = 0.0f;
    float out_go_net = 0.0f;
    float patch_lve = 0.5f;
    if((mugp->size > 0) && (mugp->acts.max >= us->opt_thresh.send)) {

      // carry over the patch LVe value in misc_1, so it can be used for modulating output
      LeabraUnit* mu = (LeabraUnit*)mugp->FastEl(0);
      patch_lve = mu->misc_1;

      float sum_mnt_go = 0.0f;
      float sum_out_go = 0.0f;
      float sum_nogo = 0.0f;
      int gp_sz = mugp->leaves / 3;
      for(int i=0;i<mugp->size;i++) {
	LeabraUnit* u = (LeabraUnit*)mugp->FastEl(i);
	XPFCGateSpec::GateSignal go_no = (XPFCGateSpec::GateSignal)(i / gp_sz); 
	if(go_no == XPFCGateSpec::GATE_MNT_GO)
	  sum_mnt_go += u->act_eq;
	else if(go_no == XPFCGateSpec::GATE_OUT_GO)
	  sum_out_go += u->act_eq;
	else
	  sum_nogo += u->act_eq;
      }
      float norm_factor = (float)gp_sz; // normalization factor: number of go units
      mnt_go_net = (sum_mnt_go - sum_nogo) / norm_factor;
      out_go_net = (sum_out_go - sum_nogo) / norm_factor;
      if(mugp->misc_state1 >= XPFCGateSpec::NOGO_RND_GO) {
	if(mnt_go_net > out_go_net) {
	  mnt_go_net += snrthal.rnd_go_inc;
	  if(mnt_go_net > 1.0f) mnt_go_net = 1.0f;
	}
	else {
	  out_go_net += snrthal.rnd_go_inc;
	  if(out_go_net > 1.0f) out_go_net = 1.0f;
	}
      }
    }

    float mnt_net_eff = net_off_rescale * (mnt_go_net + snrthal.net_off);
    float out_net_eff = net_off_rescale * (out_go_net + snrthal.net_off);

    LeabraUnit* mnt_ru = (LeabraUnit*)rugp->FastEl(0);
    LeabraUnit* out_ru = (LeabraUnit*)rugp->FastEl(1);
    mnt_ru->net = mnt_net_eff;
    mnt_ru->i_thr = mnt_ru->Compute_IThresh(net);
    out_ru->net = out_net_eff;
    out_ru->i_thr = out_ru->Compute_IThresh(net);

    // carry this forward
    mnt_ru->misc_1 = patch_lve;
    out_ru->misc_1 = patch_lve;
  }
}

void XSNrThalLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  // note: this no longer has dt.net in effect here!! hopefully not a huge diff..
  Compute_GoNogoNet(lay, net);
  inherited::Compute_NetinStats(lay, net);
}

void XSNrThalLayerSpec::Compute_Active_K(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_Active_K(lay, net);

  XSNrThalLayer* snrlay = (XSNrThalLayer*)lay;
  Compute_Active_K_mntout(snrlay, &(snrlay->mnt_units), mnt_kwta, snrlay->mnt_kwta);
  Compute_Active_K_mntout(snrlay, &(snrlay->out_units), out_kwta, snrlay->out_kwta);
}

void XSNrThalLayerSpec::Compute_Active_K_mntout(XSNrThalLayer* lay, LeabraSort* ug,
						KWTASpec& kwtspec, KWTAVals& kvals)
{
  int new_k = 0;
  if(kwtspec.k_from == KWTASpec::USE_PCT)
    new_k = (int)(kwtspec.pct * (float)ug->size);
  else
    new_k = kwtspec.k;

//   if(inhib.type == LeabraInhibSpec::KWTA_INHIB)
    new_k = MIN(ug->size - 1, new_k);
//   else
//     new_k = MIN(ug->leaves, new_k);
  new_k = MAX(1, new_k);

  if(kvals.k != new_k) {
    // overkill but whatever
    lay->mnt_active_buf.size = 0;
    lay->mnt_inact_buf.size = 0;
    lay->out_active_buf.size = 0;
    lay->out_inact_buf.size = 0;
  }

  kvals.k = new_k;
  kvals.Compute_Pct(ug->size);
}


void XSNrThalLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_Inhib(lay, net);
  if(!out_inhib && !mnt_inhib) return;

  XSNrThalLayer* snrlay = (XSNrThalLayer*)lay;
  
  if(mnt_out_inhib.type == LeabraInhibSpec::KWTA_INHIB) { // only one supported as yet
    if(mnt_inhib)
      Compute_Inhib_kWTA_mntout(mnt_kwta, snrlay->mnt_kwta, snrlay->mnt_i_val, 
				snrlay->mnt_units, snrlay->mnt_active_buf,
				snrlay->mnt_inact_buf, net);
    if(out_inhib)
      Compute_Inhib_kWTA_mntout(out_kwta, snrlay->out_kwta, snrlay->out_i_val, 
				snrlay->out_units, snrlay->out_active_buf,
				snrlay->out_inact_buf, net);
  }
  else {
    TestError(true, "Compute_Inhib_impl", "only KWTA_INHIB is currently supported for mnt/out inhibition!");
  }
}

void XSNrThalLayerSpec::Compute_Inhib_kWTA_mntout(KWTASpec& kwta, KWTAVals& kvals,
			InhibVals& ivals, LeabraSort& uns, LeabraSort& active_buf,
			LeabraSort& inact_buf, LeabraNetwork*) {
  if(uns.size <= 1) {	// this is undefined
    return;
  }

  int k_plus_1 = kvals.k + 1;	// expand cutoff to include N+1th one
  k_plus_1 = MIN(uns.size,k_plus_1);
  float k1_net = FLT_MAX;
  int k1_idx = 0;

  Compute_Inhib_kWTA_Sort_mntout(uns, active_buf, inact_buf, k_plus_1, k1_net, k1_idx);

  // active_buf now has k+1 most active units, get the next-highest one
  int k_idx = -1;
  float net_k = FLT_MAX;
  for(int j=0; j < k_plus_1; j++) {
    float tmp = active_buf.FastEl(j)->i_thr;
    if((tmp < net_k) && (j != k1_idx)) {
      net_k = tmp;		k_idx = j;
    }
  }
  if(k_idx == -1) {		// we didn't find the next one
    k_idx = k1_idx;
    net_k = k1_net;
  }

  LeabraUnit* k1_u = (LeabraUnit*)active_buf[k1_idx];
  LeabraUnit* k_u = (LeabraUnit*)active_buf[k_idx];

  float k1_i = k1_u->i_thr;
  float k_i = k_u->i_thr;
  kvals.k_ithr = k_i;
  kvals.k1_ithr = k1_i;

  //  Compute_Inhib_BreakTie(thr);

  // place kwta inhibition between k and k+1
  float nw_gi = kvals.k1_ithr + mnt_out_inhib.kwta_pt * (kvals.k_ithr - kvals.k1_ithr);
  nw_gi = MAX(nw_gi, mnt_out_inhib.min_i);
  ivals.kwta = ivals.g_i = ivals.g_i_orig = nw_gi;
  kvals.Compute_IThrR();
}

void XSNrThalLayerSpec::Compute_Inhib_kWTA_Sort_mntout(LeabraSort& ug,
				     LeabraSort& act_buf, LeabraSort& inact_buf,
				     int k_eff, float& k_net, int& k_idx) {
  LeabraUnit* u;
  int j;
  if(act_buf.size != k_eff) { // need to fill the sort buf..
    act_buf.size = 0;
    for(j = 0; j < k_eff; ++j) {
      u = (LeabraUnit*)ug.FastEl(j);
      act_buf.Add(u);		// add unit to the list
      if(u->i_thr < k_net) {
	k_net = u->i_thr;	k_idx = j;
      }
    }
    inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; j<ug.size; ++j) {
      u = (LeabraUnit*)ug.FastEl(j);
      if(u->i_thr <=  k_net) {	// not bigger than smallest one in sort buffer
	inact_buf.Add(u);
	continue;
      }
      inact_buf.Add(act_buf[k_idx]); // now inactive
      act_buf.ReplaceIdx(k_idx, u);// replace the smallest with it
      k_net = u->i_thr;		// assume its the smallest
      for(j=0; j < k_eff; j++) { 	// and recompute the actual smallest
	float tmp = act_buf[j]->i_thr;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find k_net
    for(j=0; j < k_eff; j++) { 	// these should be the top ones, very fast!!
      float tmp = act_buf[j]->i_thr;
      if(tmp < k_net) {
	k_net = tmp;		k_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < inact_buf.size; j++) {
      u = inact_buf[j];
      if(u->i_thr <=  k_net)		// not bigger than smallest one in sort buffer
	continue;
      inact_buf.ReplaceIdx(j, act_buf[k_idx]);	// now inactive
      act_buf.ReplaceIdx(k_idx, u);// replace the smallest with it
      k_net = u->i_thr;		// assume its the smallest
      int i;
      for(i=0; i < k_eff; i++) { 	// and recompute the actual smallest
	float tmp = act_buf[i]->i_thr;
	if(tmp < k_net) {
	  k_net = tmp;		k_idx = i;
	}
      }
    }
  }
}

void XSNrThalLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing
  if(inhib.type == LeabraInhibSpec::UNIT_INHIB) return; // otherwise overwrites!

  XSNrThalLayer* snrlay = (XSNrThalLayer*)lay;

  for(int mg=0; mg<snrlay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)snrlay->units.gp[mg];
    LeabraUnit* mntu = (LeabraUnit*)rugp->SafeEl(0);
    LeabraUnit* outu = (LeabraUnit*)rugp->SafeEl(1);

    float inhib_val;
    if(inhib_group == ENTIRE_LAYER)
      inhib_val = lay->i_val.g_i;
    else
      inhib_val = rugp->i_val.g_i;

    if(mnt_inhib)
      mntu->Compute_ApplyInhib(net, MAX(snrlay->mnt_i_val.g_i, inhib_val));
    else
      mntu->Compute_ApplyInhib(net, inhib_val);

    if(out_inhib)
      outu->Compute_ApplyInhib(net, MAX(snrlay->out_i_val.g_i, inhib_val));
    else
      outu->Compute_ApplyInhib(net, inhib_val);
  }
}

//////////////////////////////////////////
//	XPFC Unit Spec	
//////////////////////////////////////////

void PFCUnitSpec::Initialize() {
  
}

void PFCUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(depress.on) {
    TestWarning(true, "UAE", "cannot use synaptic depression with these units as a variable (act_nd) has been co-opted temporarily in an incompatible way.  Email Randy if this is a problem.");
    depress.on = false;
  }
}

void PFCUnitSpec::DecayState(LeabraUnit* u, LeabraNetwork* net, float decay) {
  inherited::DecayState(u, net, decay);
  u->misc_2 = 0.0f;
}

void PFCUnitSpec::Send_NetinDelta(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  XPFCLayerSpec* ls = (XPFCLayerSpec*)lay->GetLayerSpec();
  if(!ls->gate.mnt_to_bg) {
    inherited::Send_NetinDelta(u, net, thread_no);
    return;
  }

  // sends act_eq to PVLV and Matrix, and act to everyone else

  if(thread_no < 0)
    net->send_pct_tot++;	// only safe for non-thread case

  {				// pvlv/matrix  guys
    float act_ts = u->act_eq;	// send act_eq = non-output-gated version
//     if(syn_delay.on) {
//       act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
//     }

    if(act_ts > opt_thresh.send) {
      float act_delta = act_ts - u->misc_2; // misc_2 = act_sent
      if(fabsf(act_delta) > opt_thresh.delta) {
// 	if(thread_no < 0)
// 	  net->send_pct_n++;
	for(int g=0; g<u->send.size; g++) {
	  LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	  LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	  if(tol->lesioned() || tol->hard_clamped || !send_gp->size)	continue;
	  LeabraLayerSpec* tols = (LeabraLayerSpec*)tol->GetLayerSpec();
	  if(tols->InheritsFrom(&TA_LVeLayerSpec) || tols->InheritsFrom(&TA_XMatrixLayerSpec))
	    send_gp->Send_NetinDelta(net, thread_no, act_delta);
	}
	u->misc_2 = act_ts;	// cache the last sent value
      }
    }
    else if(u->misc_2 > opt_thresh.send) {
//       if(thread_no < 0)
// 	net->send_pct_n++;
      float act_delta = - u->misc_2; // un-send the last above-threshold activation to get back to 0
      for(int g=0; g<u->send.size; g++) {
	LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	if(tol->lesioned() || tol->hard_clamped || !send_gp->size)	continue;
	LeabraLayerSpec* tols = (LeabraLayerSpec*)tol->GetLayerSpec();
	if(tols->InheritsFrom(&TA_LVeLayerSpec) || tols->InheritsFrom(&TA_XMatrixLayerSpec))
	  send_gp->Send_NetinDelta(net, thread_no, act_delta);
      }
      u->misc_2 = 0.0f;		// now it effectively sent a 0..
    }
  }

  {				// regular guys
    float act_ts = u->act;
//     if(syn_delay.on) {
//       act_ts = u->act_buf.CircSafeEl(0); // get first logical element..
//     }

    if(act_ts > opt_thresh.send) {
      float act_delta = act_ts - u->act_sent;
      if(fabsf(act_delta) > opt_thresh.delta) {
	if(thread_no < 0)
	  net->send_pct_n++;
	for(int g=0; g<u->send.size; g++) {
	  LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	  LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	  if(tol->lesioned() || tol->hard_clamped || !send_gp->size)	continue;
	  LeabraLayerSpec* tols = (LeabraLayerSpec*)tol->GetLayerSpec();
	  if(!(tols->InheritsFrom(&TA_LVeLayerSpec) || tols->InheritsFrom(&TA_XMatrixLayerSpec)))
	    send_gp->Send_NetinDelta(net, thread_no, act_delta);
	}
	u->act_sent = act_ts;	// cache the last sent value
      }
    }
    else if(u->act_sent > opt_thresh.send) {
      if(thread_no < 0)
	net->send_pct_n++;
      float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
      for(int g=0; g<u->send.size; g++) {
	LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	if(tol->lesioned() || tol->hard_clamped || !send_gp->size)	continue;
	LeabraLayerSpec* tols = (LeabraLayerSpec*)tol->GetLayerSpec();
	if(!(tols->InheritsFrom(&TA_LVeLayerSpec) || tols->InheritsFrom(&TA_XMatrixLayerSpec)))
	  send_gp->Send_NetinDelta(net, thread_no, act_delta);
      }
      u->act_sent = 0.0f;		// now it effectively sent a 0..
    }
  }
}

void PFCUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* lay = u->own_lay();
  XPFCLayerSpec* ls = (XPFCLayerSpec*)lay->GetLayerSpec();
  LeabraUnit_Group* ugp = (LeabraUnit_Group*)u->owner; // assume..
  u->net += ls->gate.go_netin_gain * u->act_eq * ugp->misc_float;  // go netin mod -- weight by actual activation
  inherited::Compute_Conduct(u, net);
}

void PFCUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net) {
  if(act_fun == SPIKE) {
    // todo: do something here..
    Compute_ActFmVm_spike(u, net); 
  }
  else {
    u->act = u->act_eq;		// copy back
    Compute_ActFmVm_rate(u, net); 
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)u->owner; // assume..
    u->act = u->act_eq * ugp->misc_float1;	      // net output go mod
    // modulate learning activation value (act_nd) by gating factor..
    if(net->phase == LeabraNetwork::PLUS_PHASE)
      u->act_nd = u->act_m + ugp->misc_float * (u->act_eq - u->act_m);
  }
}


//////////////////////////////////
//	XPFC Layer Spec		//
//////////////////////////////////

void XPFCGateSpec::Initialize() {
  base_gain = 0.5f;
  go_gain = 0.5f;
  graded_out_go = true;
  go_learn_base = 0.02f;
  go_learn_mod = 1.0f - go_learn_base;
  go_netin_gain = 0.01f;
  mnt_to_bg = true;
  clear_decay = 0.0f;
  mnt_clear_veto = true;
  out_go_clear = true;
  off_accom = 0.0f;
}

void XPFCGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  go_gain = 1.0f - base_gain;
  go_learn_mod = 1.0f - go_learn_base;
}

void XPFCLayerSpec::Initialize() {
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

void XPFCLayerSpec::Defaults() {
  inherited::Defaults();
  gate.Defaults();
  Initialize();
}

void XPFCLayerSpec::HelpConfig() {
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

bool XPFCLayerSpec::CheckConfig_Layer(Layer* ly,  bool quiet) {
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
		"requires LeabraNetwork phase_oder != MINUS_PLUS_PLUS, I just set it to MINUS_PLUS for you")) {
    net->phase_order = LeabraNetwork::MINUS_PLUS;
  }

  if(lay->CheckError(net->min_cycles_phase2 < 35, quiet, rval,
		"requires LeabraNetwork min_cycles_phase2 >= 35, I just set it for you")) {
    net->min_cycles_phase2 = 35;
  }

  if(lay->CheckError(net->sequence_init != LeabraNetwork::DO_NOTHING, quiet, rval,
		"requires network sequence_init = DO_NOTHING, I just set it for you")) {
    net->sequence_init = LeabraNetwork::DO_NOTHING;
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
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_XSNrThalLayerSpec);
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
  if(lay->CheckError(snrthal_lay->un_geom.n != 2, quiet, rval,
		"SNrThal does not have 2 units per group -- I just fixed that for you")) {
    snrthal_lay->un_geom.y = 2; snrthal_lay->un_geom.x = 1;  snrthal_lay->un_geom.UpdateAfterEdit();
  }

  return true;
}

void XPFCLayerSpec::Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    ugp->misc_state1 = XPFCGateSpec::INIT_STATE;
    ugp->misc_state2 = XPFCGateSpec::GATE_NOGO;
    ugp->misc_float = 0.0f;
    ugp->misc_float1 = 0.0f;	// reset raw gating signals..
  }
}

void XPFCLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Compute_TrialInitGates(lay, net);
}

void XPFCLayerSpec::Compute_MaintUpdt_ugp(LeabraUnit_Group* ugp, MaintUpdtAct updt_act,
					  LeabraLayer* lay, LeabraNetwork* net) {
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

void XPFCLayerSpec::Compute_Gating_Final(LeabraLayer* lay, LeabraNetwork* net) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_XSNrThalLayerSpec);
  XSNrThalLayerSpec* snrthalsp = (XSNrThalLayerSpec*)snrthal_lay->spec.SPtr();

  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
    LeabraUnit* snr_mnt_u = (LeabraUnit*)snrgp->Leaf(0);
    LeabraUnit* snr_out_u = (LeabraUnit*)snrgp->Leaf(1);

    if(ugp->misc_state1 == XPFCGateSpec::INIT_STATE) { // NOGO
      if(ugp->misc_state > 0) {
	ugp->misc_state1 = XPFCGateSpec::MAINT_NOGO;	  
	ugp->misc_state++;
      }
      else {
	ugp->misc_state1 = XPFCGateSpec::EMPTY_NOGO;
	ugp->misc_state--;
      }
    }
    else if(ugp->misc_state1 == XPFCGateSpec::MAINT_MNT_GO ||
	    ugp->misc_state1 == XPFCGateSpec::EMPTY_MNT_GO) {
      Compute_MaintUpdt_ugp(ugp, STORE, lay, net);     // store it
      if(ugp->misc_state <= 0) ugp->misc_state = 1;    // this has to be the case..
      else ugp->misc_state++;
    }
    else if(ugp->misc_state1 == XPFCGateSpec::MAINT_OUT_GO ||
	    ugp->misc_state1 == XPFCGateSpec::EMPTY_OUT_GO) {
      // output without any maint -- means clear!
      if(gate.out_go_clear && (ugp->misc_state > 0)) {		       // maintaining
	Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net);     // clear it!
	ugp->misc_state = 0;			     // empty
      }
      else {
	ugp->misc_state--;	// effectively a nogo -- continue incrementing
      }
    }
    else {			// a clear veto case (flag already consulted to get here)
      // just continue as if nothing happened
      if(ugp->misc_state > 0)
	ugp->misc_state++;
      else
	ugp->misc_state--;
    }
  }
}

void XPFCLayerSpec::Compute_Gating(LeabraLayer* lay, LeabraNetwork* net) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_XSNrThalLayerSpec);
  XSNrThalLayerSpec* snrthalsp = (XSNrThalLayerSpec*)snrthal_lay->spec.SPtr();

  // this is the online version during the trial, not final

  int max_mnt_go_cycle = net->min_cycles - 1;
  // maint has to happen before this point, otherwise it is too disruptive in the clearing
  // of mnt currents and can affect learning dynamics in pfc -- this gives it plenty of time
  // to clear out old and gate in new

  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
    LeabraUnit* snr_mnt_u = (LeabraUnit*)snrgp->Leaf(0);
    LeabraUnit* snr_out_u = (LeabraUnit*)snrgp->Leaf(1);

    ugp->misc_float = 0.0f;	// go_learn multiplier factor
    ugp->misc_float1 = 0.0f;	// output gating multiplier factor

    float go_act = 0.0f;	// activation of go gating unit (out or mnt depending -- used for learning)
    float out_go_act = 0.0f;	// activation of output gating unit specifically

    XPFCGateSpec::GateSignal gate_sig = XPFCGateSpec::GATE_NOGO;

    // output gating is a threshold to initiate -- once initiated, always apply so it is
    // more continuous in its behavior -- avoid potential oscillations etc
    bool out_already_fired = false;
    if(ugp->misc_state1 == XPFCGateSpec::MAINT_OUT_GO ||
       ugp->misc_state1 == XPFCGateSpec::EMPTY_OUT_GO || 
       ugp->misc_state1 == XPFCGateSpec::MAINT_OUT_MNT_GO ||
       ugp->misc_state1 == XPFCGateSpec::EMPTY_OUT_MNT_GO)
      out_already_fired = true;

    // compute output gating multiplier consistently for all cases -- always does this
    if(out_already_fired || snr_out_u->act_eq > snrthalsp->snrthal.go_thr) {
      gate_sig = XPFCGateSpec::GATE_OUT_GO;
      go_act = snr_out_u->act_eq;
      if(gate.graded_out_go)
	out_go_act = snr_out_u->act_eq;
      else
	out_go_act = 1.0f; // go all the way
    }

    // two states of operation: before any gating signal, and after a gating signal
    if(ugp->misc_state1 == XPFCGateSpec::INIT_STATE) {
      // before any gating signal -- out wins in a tie due to clear_veto logic
      if(gate_sig == XPFCGateSpec::GATE_OUT_GO) {
	if(net->phase_no == 0) {	// only update misc_state1 in minus phase
	  if(ugp->misc_state > 0)  // full stripe
	    ugp->misc_state1 = XPFCGateSpec::MAINT_OUT_GO;
	  else
	    ugp->misc_state1 = XPFCGateSpec::EMPTY_OUT_GO;
	}
      }
      else if(net->ct_cycle < max_mnt_go_cycle && // only allow mnt within early minus 
	      snr_mnt_u->act_eq > snrthalsp->snrthal.go_thr) {
	gate_sig = XPFCGateSpec::GATE_MNT_GO;
	go_act = snr_mnt_u->act_eq;

	if(ugp->misc_state > 0) { // full stripe
	  ugp->misc_state1 = XPFCGateSpec::MAINT_MNT_GO;
	  Compute_MaintUpdt_ugp(ugp, CLEAR, lay, net);	 // clear maint currents
	  ugp->misc_state = 0;	// record as cleared
	}
	else {
	  ugp->misc_state1 = XPFCGateSpec::EMPTY_MNT_GO;
	}
      }
    }
    else if(ugp->misc_state1 == XPFCGateSpec::MAINT_OUT_GO ||
	    ugp->misc_state1 == XPFCGateSpec::EMPTY_OUT_GO) {
      // already fired output go, so only question is: did mnt go fire too?
      if(gate.mnt_clear_veto && (snr_mnt_u->act_eq > snrthalsp->snrthal.go_thr)) {
	if(ugp->misc_state1 == XPFCGateSpec::MAINT_OUT_GO)
	  ugp->misc_state1 = XPFCGateSpec::MAINT_OUT_MNT_GO;
	else
	  ugp->misc_state1 = XPFCGateSpec::EMPTY_OUT_MNT_GO;
      }
    }
    else if(ugp->misc_state1 == XPFCGateSpec::MAINT_MNT_GO ||
	    ugp->misc_state1 == XPFCGateSpec::EMPTY_MNT_GO) {
      // pure mnt gating -- use it for the activations -- could also have output
      // going on too, but anyway this is considered dominant
      gate_sig = XPFCGateSpec::GATE_MNT_GO;
      go_act = snr_mnt_u->act_eq;
    }

    // misc_float has the go_learn_base factor incorporated
    ugp->misc_float = gate.go_learn_base + (gate.go_learn_mod * go_act);
    // misc_float1 includes net output gating multiplier:
    ugp->misc_float1 = gate.base_gain + (gate.go_gain * out_go_act);
    ugp->misc_state2 = gate_sig; // store the raw gating signal itself
  }
  SendGateStates(lay, net);
}

void XPFCLayerSpec::SendGateStates(LeabraLayer* lay, LeabraNetwork*) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_XSNrThalLayerSpec);
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(snrthal_lay, mtx_prjn_idx, &TA_XMatrixLayerSpec);
  int mg;
  for(mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    // everybody gets gate state info from PFC!
    snrgp->misc_state = mugp->misc_state = ugp->misc_state;
    snrgp->misc_state1 = ugp->misc_state1; 
    if(mugp->misc_state1 < XPFCGateSpec::NOGO_RND_GO) { // don't override random go signals
      mugp->misc_state1 = ugp->misc_state1;
    }
    snrgp->misc_state2 = mugp->misc_state2 = ugp->misc_state2;
    snrgp->misc_float = mugp->misc_float = ugp->misc_float;
    snrgp->misc_float1 = mugp->misc_float1 = ugp->misc_float1;
  }
}

void XPFCLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_ApplyInhib(lay, net);
  Compute_Gating(lay, net);	// online gating, after activations have been updated
}
  
void XPFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == 1) {
    Compute_Gating_Final(lay, net);	// final gating
  }
}


///////////////////////////////////////////////////////////////////////////////////////

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
  LeabraConSpec* old_matrix_bias = (LeabraConSpec*)bg_bias->children.FindSpecName("MatrixCons");
  if(old_matrix_bias) {
    bg_bias->children.RemoveEl(old_matrix_bias);
  }
  if(pfc_self == NULL || intra_pfc == NULL || matrix_cons == NULL || marker_cons == NULL 
     || matrix_bias == NULL)
    return false;

  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec);
  LViLayerSpec* lvisp = (LViLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec);
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

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  UniformRndPrjnSpec* topfc = (UniformRndPrjnSpec*)prjns->FindMakeSpec("ToPFC", &TA_UniformRndPrjnSpec);
  ProjectionSpec* pfc_selfps = (ProjectionSpec*)prjns->FindMakeSpec("PFCSelf", &TA_OneToOnePrjnSpec);
  GpRndTesselPrjnSpec* intra_pfcps = (GpRndTesselPrjnSpec*)prjns->FindMakeSpec("IntraPFC", &TA_GpRndTesselPrjnSpec);
  TesselPrjnSpec* input_pfc = (TesselPrjnSpec*)prjns->FindMakeSpec("Input_PFC", &TA_TesselPrjnSpec);
  PFCLVPrjnSpec* pfc_lv_prjn = (PFCLVPrjnSpec*)prjns->FindMakeSpec("PFC_LV_Prjn", &TA_PFCLVPrjnSpec);
  if(topfc == NULL || pfc_selfps == NULL || intra_pfcps == NULL || gponetoone == NULL || input_pfc == NULL) return false;

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

    net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);
    net->FindMakePrjn(matrix_o, pfc_m, gponetoone, mofmpfc_cons);

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
    net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);

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
      net->FindMakePrjn(hl, pfc_o, fullprjn, learn_cons);
      net->RemovePrjn(hl, pfc_m); // get rid of any existing ones
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
    topfc_cons->lrate = .001f;
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
  pfc_self->wt_scale.rel = .1f;

  matrix_cons->SetUnique("lrate", true);
  matrix_cons->lrate = .05f;
  matrix_cons->SetUnique("wt_sig", true);
  matrix_cons->wt_sig.gain = 1.0f;
  matrix_cons->wt_sig.off = 1.0f;
  matrix_cons->SetUnique("xcal", true);
  matrix_cons->xcal.mvl_mix = 0.005f;
  matrix_cons->xcal.s_mix = 0.8f;

  mfmpfc_cons->SetUnique("wt_scale", true);
  mfmpfc_cons->wt_scale.rel = .2f;
  mfmpfc_cons->SetUnique("lmix", false);

  matrix_bias->SetUnique("lrate", true);
  matrix_bias->lrate = 0.0f;		// default is no bias learning
  matrix_bias->SetUnique("xcal", false); // inherit

  matrix_units->g_bar.h = .01f; // old syn dep
  matrix_units->g_bar.a = .03f;
  matrix_units->noise_type = LeabraUnitSpec::NETIN_NOISE;
  matrix_units->noise.var = 5.0e-5f;
  matrix_units->noise_adapt.trial_fixed = true;
  matrix_units->noise_adapt.mode = NoiseAdaptSpec::PVLV_LVE;

  if(out_gate) {
    matrixo_units->SetUnique("g_bar", true);
    matrixo_units->g_bar.h = .02f; matrixo_units->g_bar.a = .06f;
    matrixo_units->SetUnique("noise_type", false);
    matrixo_units->SetUnique("noise", true);
    matrixo_units->noise.var = 0.005f;
    matrixo_units->SetUnique("noise_adapt", true);
    matrixo_units->noise_adapt.trial_fixed = true;
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
    matrixo_cons->SetUnique("xcal", false); // inherit

    mofmpfc_cons->SetUnique("wt_scale", true);
    mofmpfc_cons->wt_scale.rel = 1.0f; // works better with gp-one-to-one
    mofmpfc_cons->SetUnique("lmix", false);

    snrthalosp->SetUnique("kwta", true);
    snrthalosp->kwta.pct = .25f; // generally works better!
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
  if(snrthalsp->snrthal.net_off == 0.0f)
    snrthal_units->act.gain = 20.0f; // lower gain for net_off = 0
  else
    snrthal_units->act.gain = 600.0f;

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
    pfc_units->SelectForEditNm("g_bar", edit, "pfc");
    pfcmsp->SelectForEditNm("gate", edit, "pfc_m");
    matrixsp->SelectForEditNm("matrix", edit, "matrix");
    matrixsp->SelectForEditNm("contrast", edit, "matrix");
    matrix_units->SelectForEditNm("g_bar", edit, "matrix");
    matrix_cons->SelectForEditNm("lrate", edit, "matrix");
    matrix_cons->SelectForEditNm("xcal", edit, "matrix");
    //    matrix_cons->SelectForEditNm("lmix", edit, "matrix");
    mfmpfc_cons->SelectForEditNm("wt_scale", edit, "mtx_fm_pfc");
    snrthalsp->SelectForEditNm("kwta", edit, "snr_thal");
//       snrthal_units->SelectForEditNm("g_bar", edit, "snr_thal");
//       snrthal_units->SelectForEditNm("dt", edit, "snr_thal");
    if(out_gate) {
      matrixo_units->SelectForEditNm("g_bar", edit, "mtx_out");
      matrixo_cons->SelectForEditNm("lrate", edit, "mtx_out");
      matrixo_cons->SelectForEditNm("rnd", edit, "mtx_out");
      matrixo_cons->SelectForEditNm("wt_sig", edit, "mtx_out");
      matrixosp->SelectForEditNm("contrast", edit, "mtx_out");
      pfcosp->SelectForEditNm("out_gate", edit, "pfc_out");
      snrthalosp->SelectForEditNm("kwta", edit, "snr_thal_out");
    }    
  }

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM -- actually saves network specifically");
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////
//		PBWM V2
/////////////////////////////////////////////////////////////////////////////

bool LeabraWizard::PBWM_V2(LeabraNetwork* net, bool da_mod_all,
			   int n_stripes, bool nolrn_pfc) {
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
  bool matrix_new = false;  bool snrthal_new = false; bool pfc_new = false;

  LeabraLayer* patch = NULL;
  LeabraLayer* snc = NULL;

  LeabraLayer* matrix = NULL;
  LeabraLayer* snrthal = NULL;
  LeabraLayer* pfc = NULL;

  patch = (LeabraLayer*)bg_laygp->FindMakeLayer("Patch", NULL, patch_new, "Patch");
  snc = (LeabraLayer*)bg_laygp->FindMakeLayer("SNc", NULL, snc_new, "SNc");

  matrix = (LeabraLayer*)bg_laygp->FindMakeLayer("Matrix", NULL, matrix_new, "Matrix");
  snrthal = (LeabraLayer*)bg_laygp->FindMakeLayer("SNrThal", &TA_XSNrThalLayer, snrthal_new, "SNrThal");
  pfc = (LeabraLayer*)pfc_laygp->FindMakeLayer("PFC", NULL, pfc_new);

  if(!patch || !snc || !matrix || !snrthal || !pfc) return false;

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
       && lay != snrthal && lay != matrix && lay != pfc) {
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

  PFCUnitSpec* pfc_units = (PFCUnitSpec*)units->FindMakeSpec("PFCUnits", &TA_PFCUnitSpec);
  MatrixUnitSpec* matrix_units = (MatrixUnitSpec*)units->FindMakeSpec("MatrixUnits", &TA_MatrixUnitSpec);
  LeabraUnitSpec* snrthal_units = (LeabraUnitSpec*)units->FindMakeSpec("SNrThalUnits", &TA_LeabraUnitSpec);
  if(pfc_units == NULL || matrix_units == NULL) return false;

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
  PatchLayerSpec* patchsp = (PatchLayerSpec*)lvesp->FindMakeChild("PatchLayer", &TA_PatchLayerSpec);

  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindType(&TA_PVLVDaLayerSpec);
  NVLayerSpec* nvsp = (NVLayerSpec*)layers->FindMakeSpec("NVLayer", &TA_NVLayerSpec);
  SNcLayerSpec* sncsp = (SNcLayerSpec*)dasp->FindMakeChild("SNcLayer", &TA_SNcLayerSpec);

  XPFCLayerSpec* pfcmsp = (XPFCLayerSpec*)layers->FindMakeSpec("PFCLayer", &TA_XPFCLayerSpec);
  XMatrixLayerSpec* matrixsp = (XMatrixLayerSpec*)layers->FindMakeSpec("MatrixLayer", &TA_XMatrixLayerSpec);
  if(pfcmsp == NULL || matrixsp == NULL) return false;

  XSNrThalLayerSpec* snrthalsp = (XSNrThalLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_XSNrThalLayerSpec);

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  UniformRndPrjnSpec* topfc = (UniformRndPrjnSpec*)prjns->FindMakeSpec("ToPFC", &TA_UniformRndPrjnSpec);
  ProjectionSpec* pfc_selfps = (ProjectionSpec*)prjns->FindMakeSpec("PFCSelf", &TA_OneToOnePrjnSpec);
  GpRndTesselPrjnSpec* intra_pfcps = (GpRndTesselPrjnSpec*)prjns->FindMakeSpec("IntraPFC", &TA_GpRndTesselPrjnSpec);
  TesselPrjnSpec* input_pfc = (TesselPrjnSpec*)prjns->FindMakeSpec("Input_PFC", &TA_TesselPrjnSpec);
  PFCLVPrjnSpec* pfc_lv_prjn = (PFCLVPrjnSpec*)prjns->FindMakeSpec("PFC_LV_Prjn", &TA_PFCLVPrjnSpec);
  if(topfc == NULL || pfc_selfps == NULL || intra_pfcps == NULL || gponetoone == NULL || input_pfc == NULL) return false;

  input_pfc->send_offs.New(1); // this is all it takes!

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  // set bias specs for unit specs
  pfc_units->bias_spec.SetSpec(pfc_bias);
  matrix_units->bias_spec.SetSpec(matrix_bias);
  snrthal_units->bias_spec.SetSpec(bg_bias);

  patch->SetLayerSpec(patchsp); patch->SetUnitSpec(lv_units);
  snc->SetLayerSpec(sncsp); snc->SetUnitSpec(da_units);
  
  snrthal->SetLayerSpec(snrthalsp); snrthal->SetUnitSpec(snrthal_units);
  matrix->SetLayerSpec(matrixsp);   matrix->SetUnitSpec(matrix_units);
  pfc->SetLayerSpec(pfcmsp);	pfc->SetUnitSpec(pfc_units);

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

  net->RemovePrjn(matrix, vta); // no more vta prjn!
  net->FindMakePrjn(matrix, snc, gponetoone, marker_cons);

  net->FindMakePrjn(snrthal, matrix, gponetoone, marker_cons);
  net->FindMakePrjn(pfc, snrthal, gponetoone, marker_cons);
  net->FindMakePrjn(matrix, snrthal, gponetoone, marker_cons);
  net->FindMakePrjn(matrix, patch, gponetoone, marker_cons); // for noise

  net->FindMakePrjn(matrix, pfc, gponetoone, mfmpfc_cons);

  net->FindMakeSelfPrjn(pfc, pfc_selfps, pfc_self);
  //  net->FindMakeSelfPrjn(pfc, intra_pfcps, intra_pfc);

  net->FindMakePrjn(pvr, pfc, fullprjn, pvr_cons);
  net->FindMakePrjn(pvi, pfc, fullprjn, pvi_cons);
  net->FindMakePrjn(lve, pfc, pfc_lv_prjn, lve_cons);
  net->FindMakePrjn(lvi, pfc, pfc_lv_prjn, lvi_cons);
  net->FindMakePrjn(nv,  pfc, fullprjn, nv_cons);

  net->FindMakePrjn(patch, pfc, gponetoone, lve_cons);

  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];
    if(pfc_new) {
      if(nolrn_pfc)
	net->FindMakePrjn(pfc, il, input_pfc, topfc_cons);
      else
	net->FindMakePrjn(pfc, il, fullprjn, topfc_cons);
    }
    if(matrix_new)
      net->FindMakePrjn(matrix, il, fullprjn, matrix_cons);
  }
  for(i=0;i<hidden_lays.size;i++) {
    Layer* hl = (Layer*)hidden_lays[i];
    net->FindMakePrjn(hl, pfc, fullprjn, learn_cons);
  }
  if(pfc_new && !nolrn_pfc) {
    for(i=0;i<output_lays.size;i++) {
      Layer* ol = (Layer*)output_lays[i];
      net->FindMakePrjn(pfc, ol, fullprjn, topfc_cons);
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  net->min_cycles = 30;		// needed to clear out!
  net->phase_order = LeabraNetwork::MINUS_PLUS;

  // NOT unique: inherit from lve
  patchsp->SetUnique("decay", false);
  patchsp->SetUnique("kwta", false);
  patchsp->SetUnique("inhib_group", true);
  patchsp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
  patchsp->SetUnique("inhib", false);

  nvsp->nv.da_gain = 0.0f;	// turn off by default, for now
  dasp->da.pv_gain = 0.5f;

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
  pfc_self->wt_scale.rel = .1f;

  matrix_cons->SetUnique("lrate", true);
  matrix_cons->lrate = .05f;
  matrix_cons->SetUnique("wt_sig", true);
  matrix_cons->wt_sig.gain = 1.0f;
  matrix_cons->wt_sig.off = 1.0f;
  matrix_cons->SetUnique("xcal", true);
  matrix_cons->xcal.mvl_mix = 0.005f;
  matrix_cons->xcal.s_mix = 0.8f;

  mfmpfc_cons->SetUnique("wt_scale", true);
  mfmpfc_cons->wt_scale.rel = .2f;
  mfmpfc_cons->SetUnique("lmix", false);

  matrix_bias->SetUnique("lrate", true);
  matrix_bias->lrate = 0.0f;		// default is no bias learning
  matrix_bias->SetUnique("xcal", false); // inherit

  matrix_units->g_bar.h = .01f; // old syn dep
  matrix_units->g_bar.a = .03f;
  matrix_units->noise_type = LeabraUnitSpec::NETIN_NOISE;
  matrix_units->noise.var = 0.00005f;
  matrix_units->dt.vm = 0.2f;
  matrix_units->noise_adapt.trial_fixed = true;
  matrix_units->noise_adapt.mode = NoiseAdaptSpec::PVLV_PVI;
  matrix_units->matrix_noise.patch_noise = false;

  pfc_units->SetUnique("g_bar", true);
  if(nolrn_pfc)
    pfc_units->g_bar.h = 1.0f;
  else
    pfc_units->g_bar.h = .5f;
  pfc_units->g_bar.a = 2.0f;
  pfc_units->SetUnique("dt", true);
  pfc_units->dt.vm = .1f;	// slower is better..  .1 is even better!

  snrthal_units->dt.vm = 0.1f;
  snrthal_units->g_bar.l = 0.8f;
  snrthal_units->act.gain = 600.0f;

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
  
  matrixsp->gp_kwta.k_from = KWTASpec::USE_PCT;
  matrixsp->gp_kwta.pct = .25f;
  matrixsp->inhib.type = LeabraInhibSpec::KWTA_INHIB;
  matrixsp->inhib.kwta_pt = .25f;
  matrixsp->UpdateAfterEdit();

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  int n_lv_u;		// number of pvlv-type units
  if(lvesp->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  else if(lvesp->scalar.rep == ScalarValSpec::GAUSSIAN)
    n_lv_u = 12;
  else
    n_lv_u = 21;

  if(patch_new) {
    patch->pos.SetXYZ(vta->pos.x+3, 0, 0);
  }
  if(snc_new) {
    snc->pos.SetXYZ(vta->pos.x+3 + n_lv_u, 4, 0);
  }

  if(patch->un_geom.n != n_lv_u) { patch->un_geom.n = n_lv_u; patch->un_geom.x = n_lv_u; patch->un_geom.y = 1; }

  lay_set_geom(patch, n_stripes);
  lay_set_geom(snc, n_stripes, 1); // one unit

  if(pfc_new) {
    pfc->pos.SetXYZ(mx_z2 + 1, 0, 2);
    if(nolrn_pfc && (input_lays.size > 0)) {
      Layer* il = (Layer*)input_lays[0];
      pfc->un_geom = il->un_geom;
    }
    else {
      pfc->un_geom.n = 30; pfc->un_geom.x = 5; pfc->un_geom.y = 6;
    }
  }
  lay_set_geom(pfc, n_stripes);

  if(matrix_new) { 
    matrix->pos.SetXYZ(mx_z1+1, 0, 1);
    matrix->un_geom.n = 42; matrix->un_geom.x = 7; matrix->un_geom.y = 6;
  }
  lay_set_geom(matrix, n_stripes);

  if(snrthal_new) {
    snrthal->pos.SetXYZ(patch->pos.x + (patch->un_geom.x +1) * patch->gp_geom.x +1, 0, 0);
    snrthal->un_geom.y = 2; snrthal->un_geom.x = 1;  snrthal->un_geom.UpdateAfterEdit();
  }
  lay_set_geom(snrthal, n_stripes);

  // this is here, to allow it to get act_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, n_stripes);

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

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("PBWM");
  if(edit != NULL) {
    pfc_units->SelectForEditNm("g_bar", edit, "pfc");
    pfcmsp->SelectForEditNm("gate", edit, "pfc");
    matrixsp->SelectForEditNm("matrix", edit, "matrix");
    matrixsp->SelectForEditNm("contrast", edit, "matrix");
    matrix_units->SelectForEditNm("g_bar", edit, "matrix");
    matrix_units->SelectForEditNm("noise", edit, "matrix");
    matrix_cons->SelectForEditNm("lrate", edit, "matrix");
    matrix_cons->SelectForEditNm("xcal", edit, "matrix");
    mfmpfc_cons->SelectForEditNm("wt_scale", edit, "mtx_fm_pfc");
    snrthalsp->SelectForEditNm("gp_kwta", edit, "snr_thal");
    snrthalsp->SelectForEditNm("mnt_kwta", edit, "snr_thal");
    snrthalsp->SelectForEditNm("out_kwta", edit, "snr_thal");
    snrthalsp->SelectForEditNm("mnt_out_inhib", edit, "snr_thal");
    snrthal_units->SelectForEditNm("g_bar", edit, "snr_thal");
//       snrthal_units->SelectForEditNm("dt", edit, "snr_thal");
  }

  taMisc::CheckConfigStart(false, false);

  bool ok = patchsp->CheckConfig_Layer(patch, false);
  ok &= sncsp->CheckConfig_Layer(snc, false);
  ok &= pfcmsp->CheckConfig_Layer(pfc, false);
  ok &= matrixsp->CheckConfig_Layer(matrix, false);
  ok &= snrthalsp->CheckConfig_Layer(snrthal, false);

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

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_V2 -- actually saves network specifically");
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
