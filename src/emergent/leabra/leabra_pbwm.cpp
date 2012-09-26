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

//////////////////////////////////
//      PBWM Unit Gp Data       //
//////////////////////////////////

void PBWMUnGpData::Initialize() {
  go_fired_now = false;
  go_fired_trial = false;
  go_forced = false;
  go_cycle = -1;
  mnt_count = -1;		// empty
  prv_mnt_count = -1;
  nogo_inhib = 0.0f;
  refract_inhib = 0.0f;
  no_pfc_inhib = 0.0f;
  pvr_inhib = 0.0f;
}

void PBWMUnGpData::Copy_(const PBWMUnGpData& cp) {
  go_fired_now = cp.go_fired_now;
  go_fired_trial = cp.go_fired_trial;
  go_forced = cp.go_forced;
  go_cycle = cp.go_cycle;
  mnt_count = cp.mnt_count;
  prv_mnt_count = cp.prv_mnt_count;
  nogo_inhib = cp.nogo_inhib;
  refract_inhib = cp.refract_inhib;
  no_pfc_inhib = cp.no_pfc_inhib;
  pvr_inhib = cp.pvr_inhib;
}

void PBWMUnGpData::Init_State() {
  inherited::Init_State();
  Initialize();
}

void PBWMUnGpData::CopyPBWMData(const PBWMUnGpData& cp) {
  PBWMUnGpData::Copy_(cp);
}


//////////////////////////////////
//      SNrThal Layer Spec      //
//////////////////////////////////

void SNrThalMiscSpec::Initialize() {
  go_thr = 0.5f;
  gate_cycle = 25;
  force = false;
}

void SNrThalLayerSpec::Initialize() {
  gating_types = IN_MNT_OUT;

  Defaults_init();
}

void SNrThalLayerSpec::Defaults_init() {
  // SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;

  // SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .8f;

  // SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 2;

  // SetUnique("decay", true);
  decay.clamp_phase2 = false;

  // SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  // SetUnique("tie_brk", false);
  tie_brk.on = true;
  tie_brk.thr_gain = 0.2f;
  tie_brk.loser_gain = 1.0f;
}

void SNrThalLayerSpec::HelpConfig() {
  String help = "SNrThalLayerSpec Computation:\n\
 - net = Go if no NoGo, else Go / (Go + nogo_gain * NoGo + leak)\n\
 - act = raw activation from netin\n\
 - act_m2 = winner-filtered gating activations -- drives learning in Matrix\n\
 - No learning, wt init variance, in afferent cons\n\
 \nSNrThalLayerSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure.\n\
 - Matrix_Go now projects using regular leabra cons (non-MarkerCons) to create basic standard netinput\n\
 - optional NoGo projection with MarkerCons to provide divisive renorm of signal\n";
  taMisc::Confirm(help);
}

void SNrThalLayerSpec::GatingTypesNStripes(LeabraLayer* lay, 
					   int& n_in, int& n_mnt, int& n_out) {
  n_in = 0;
  n_mnt = 0;
  n_out = 0;

  gating_types = NO_GATE_TYPE;

  for(int g=0; g<lay->projections.size; g++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)prjn->from.ptr();
    if(!fmlay->GetLayerSpec()->InheritsFrom(TA_MatrixLayerSpec)) continue;
    MatrixLayerSpec* mls = (MatrixLayerSpec*)fmlay->GetLayerSpec();
    if(mls->go_nogo != MatrixLayerSpec::GO) continue;

    gating_types = (GatingTypes)(gating_types | mls->gating_type);
    if(mls->gating_type == INPUT) n_in += fmlay->gp_geom.n;
    if(mls->gating_type == MAINT) n_mnt += fmlay->gp_geom.n;
    if(mls->gating_type == OUTPUT) n_out += fmlay->gp_geom.n;
  }
}

int SNrThalLayerSpec::SNrThalStartIdx(LeabraLayer* lay, GatingTypes gating_type,
				      int& n_in, int& n_mnt, int& n_out) {
  GatingTypesNStripes(lay, n_in, n_mnt, n_out);
  int snr_st_idx = -1;
  switch(gating_type) {
  case INPUT:
    snr_st_idx = 0;
    break;
  case MAINT:
    snr_st_idx = n_in;
    break;
  case OUTPUT:
    snr_st_idx = n_in + n_mnt;
    break;
  default:			// compiler food
    break;
  }
  return snr_st_idx;
}

bool SNrThalLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  // SetUnique("decay", true);
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // some kind of check on cycle??

  // if(lay->CheckError(net->mid_minusmax_go_cycle <= snrthal.min_go_cycle, quiet, rval,
  //               "min go cycle not before max go cycle -- adjusting min_go_cycle -- you should double check")) {
  // }

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  int n_in, n_mnt, n_out;
  GatingTypesNStripes(lay, n_in, n_mnt, n_out);
  int snr_stripes = n_in + n_mnt + n_out;
  
  if(lay->CheckError(snr_stripes != lay->gp_geom.n, quiet, rval,
		     "SNrThalLayer does not have an appropriate number of unit groups given the inputs receiving from the Matrix. Should be:", String(snr_stripes), "is:", 
		     String(lay->gp_geom.n), "updated n stripes but this may not be sufficient")) {
    lay->unit_groups = true;
    lay->gp_geom.n = snr_stripes;
  }

  return true;
}

void SNrThalLayerSpec::Init_GateStats(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    // update mnt count at start of trial!
    if(gpd->mnt_count < 0)
      gpd->mnt_count--;	// more empty
    else
      gpd->mnt_count++;	// more maint
    gpd->go_fired_now = false;
    gpd->go_fired_trial = false;
    gpd->go_forced = false;
    gpd->go_cycle = -1;

    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
      if(u->lesioned()) continue;
      u->act_m2 = 0.0f;	// reset gating act
    }
  }

  lay->SetUserData("n_fired_trial", 0);
  lay->SetUserData("n_fired_now", 0);
}

void SNrThalLayerSpec::Compute_GateActs(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle < snrthal.gate_cycle)
    return;			// nothing to do before gating starts

  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  int n_fired_trial = 0;
  int n_fired_now = 0;

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, mg); // assuming one unit
    if(u->lesioned()) continue;

    if(net->ct_cycle == snrthal.gate_cycle) {
      if(u->act_eq >= snrthal.go_thr) {
	n_fired_trial++;
	n_fired_now++;
	gpd->go_fired_now = true;
	gpd->go_fired_trial = true;
	gpd->go_cycle = net->ct_cycle;
	gpd->prv_mnt_count = gpd->mnt_count;
	gpd->mnt_count = 0;	// reset
	u->act_m2 = u->act_eq;	// gating act
      }
    }
    else {
      gpd->go_fired_now = false; // turn it off after one cycle
      u->act = u->act_eq = u->act_nd = u->act_m2; // activity is always gating signal
    }
  }

  // no stripe has fired yet -- force max go firing
  if(net->ct_cycle == snrthal.gate_cycle && snrthal.force && n_fired_trial == 0) {
    int go_idx = lay->netin.max_i;
    if(go_idx < 0) {		// this really shouldn't happen, but if it does..
      go_idx = Random::IntZeroN(lay->units.leaves);
    }
    int gp_idx = go_idx / nunits;
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gp_idx); // assuming one unit
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gp_idx);
    n_fired_trial++;
    n_fired_now++;
    gpd->go_fired_now = true;
    gpd->go_fired_trial = true;
    gpd->go_forced = true;	// yes this is forced
    gpd->go_cycle = net->ct_cycle;
    gpd->prv_mnt_count = gpd->mnt_count;
    gpd->mnt_count = 0;	// reset
    u->act_m2 = snrthal.go_thr; // must be at least at threshold -- otherwise won't gate for pbwm!
  }

  if(net->ct_cycle == snrthal.gate_cycle) {
    Compute_GateStats(lay, net); // update overall stats at this point
    lay->SetUserData("n_fired_trial", n_fired_trial);
    lay->SetUserData("n_fired_now", n_fired_now);
  }
}


void SNrThalLayerSpec::Compute_GateStats(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  int n_in, n_mnt, n_out;
  GatingTypesNStripes(lay, n_in, n_mnt, n_out);
  int mnt_st = n_in;
  int out_st = n_in + n_mnt;

  const int n_stats = 4;	// 0 = global, 1 = in, 2 = mnt, 3 = out
  int	n_gated[n_stats] = {0,0,0,0};
  int	n_forced[n_stats] = {0,0,0,0};
  int	min_go_cycle[n_stats] = {1000,1000,1000,1000};
  int	max_go_cycle[n_stats] = {0,0,0,0};
  int	min_mnt_count[n_stats] = {0,0,0,0};
  int	max_mnt_count[n_stats] = {0,0,0,0};

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    if(gpd->go_fired_trial) {
      if(mg < n_in) n_gated[1]++;
      else if(mg >= mnt_st && mg < out_st) n_gated[2]++;
      else if(mg >= out_st) n_gated[3]++;
      n_gated[0]++;		// global

      if(mg < n_in) min_go_cycle[1] = MIN(min_go_cycle[1], gpd->go_cycle);
      else if(mg >= mnt_st && mg < out_st) min_go_cycle[2] = MIN(min_go_cycle[2], gpd->go_cycle);
      else if(mg >= out_st) min_go_cycle[3] = MIN(min_go_cycle[3], gpd->go_cycle);
      min_go_cycle[0] = MIN(min_go_cycle[0], gpd->go_cycle);

      if(mg < n_in) max_go_cycle[1] = MAX(max_go_cycle[1], gpd->go_cycle);
      else if(mg >= mnt_st && mg < out_st) max_go_cycle[2] = MAX(max_go_cycle[2], gpd->go_cycle);
      else if(mg >= out_st) max_go_cycle[3] = MAX(max_go_cycle[3], gpd->go_cycle);
      max_go_cycle[0] = MAX(max_go_cycle[0], gpd->go_cycle);
    }
    if(gpd->go_forced) {
      if(mg < n_in) n_forced[1]++;
      else if(mg >= mnt_st && mg < out_st) n_forced[2]++;
      else if(mg >= out_st) n_forced[3]++;
      n_forced[0]++;		// global
    }

    // only mnt layers contribute to this!
    if(mg >= mnt_st && mg < out_st) {
      min_mnt_count[2] = MIN(min_mnt_count[2], gpd->mnt_count);
      max_mnt_count[2] = MAX(max_mnt_count[2], gpd->mnt_count);
    }
  }

  lay->SetUserData("n_gated_all", (float)n_gated[0]);
  if(n_in > 0) lay->SetUserData("n_gated_in", (float)n_gated[1]);
  if(n_mnt > 0) lay->SetUserData("n_gated_mnt", (float)n_gated[2]);
  if(n_out > 0) lay->SetUserData("n_gated_out", (float)n_gated[3]);

  lay->SetUserData("n_forced_all", (float)n_forced[0]);
  if(n_in > 0) lay->SetUserData("n_forced_in", (float)n_forced[1]);
  if(n_mnt > 0) lay->SetUserData("n_forced_mnt", (float)n_forced[2]);
  if(n_out > 0) lay->SetUserData("n_forced_out", (float)n_forced[3]);

lay->SetUserData("min_go_cycle_all", (float)(n_gated[0] > 0 ? min_go_cycle[0] : -1));
 if(n_in > 0) lay->SetUserData("min_go_cycle_in", (float)(n_gated[1] > 0 ? min_go_cycle[1] : -1));
 if(n_mnt > 0) lay->SetUserData("min_go_cycle_mnt", (float)(n_gated[2] > 0 ? min_go_cycle[2] : -1));
 if(n_out > 0) lay->SetUserData("min_go_cycle_out", (float)(n_gated[3] > 0 ? min_go_cycle[3] : -1));

  lay->SetUserData("max_go_cycle_all", (float)(n_gated[0] > 0 ? max_go_cycle[0] : -1));
  if(n_in > 0) lay->SetUserData("max_go_cycle_in", (float)(n_gated[1] > 0 ? max_go_cycle[1] : -1));
  if(n_mnt > 0) lay->SetUserData("max_go_cycle_mnt", (float)(n_gated[2] > 0 ? max_go_cycle[2] : -1));
  if(n_out > 0) lay->SetUserData("max_go_cycle_out", (float)(n_gated[3] > 0 ? max_go_cycle[3] : -1));

  if(n_mnt > 0) {
    lay->SetUserData("min_mnt_count", (float)min_mnt_count[2]);
    lay->SetUserData("max_mnt_count", (float)max_mnt_count[2]);
  }
}

void SNrThalLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_GateActs(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void SNrThalLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // nop -- don't do the default thing -- already done by GatedActs
}

void SNrThalLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  lay->SetUserData("n_fired_trial", 0);
  lay->SetUserData("n_fired_now", 0);

  lay->SetUserData("n_gated_all", 0.0f);
  lay->SetUserData("n_gated_in",  0.0f);
  lay->SetUserData("n_gated_mnt", 0.0f);
  lay->SetUserData("n_gated_out", 0.0f);

  lay->SetUserData("n_forced_all", 0.0f);
  lay->SetUserData("n_forced_in",  0.0f);
  lay->SetUserData("n_forced_mnt", 0.0f);
  lay->SetUserData("n_forced_out", 0.0f);

  lay->SetUserData("min_go_cycle_all", 0.0f);
  lay->SetUserData("min_go_cycle_in",  0.0f);
  lay->SetUserData("min_go_cycle_mnt", 0.0f);
  lay->SetUserData("min_go_cycle_out", 0.0f);

  lay->SetUserData("max_go_cycle_all", 0.0f);
  lay->SetUserData("max_go_cycle_in",  0.0f);
  lay->SetUserData("max_go_cycle_mnt", 0.0f);
  lay->SetUserData("max_go_cycle_out", 0.0f);

  lay->SetUserData("min_mnt_count", 0.0f);
  lay->SetUserData("max_mnt_count", 0.0f);
}

void SNrThalLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Init_GateStats(lay, net);
}

//////////////////////////////////
//      MatrixConSpec           //
//////////////////////////////////

void MatrixConSpec::Initialize() {
  min_obj_type = &TA_MatrixCon;
  err_gain = 0.0f;

  Defaults_init();
}

void MatrixConSpec::Defaults_init() {
  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  // SetUnique("wt_sig", true);
  wt_sig.gain = 6.0f;
  wt_sig.off = 1.25f;
}

void MatrixConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  //  lmix.err_sb = false;
}

//////////////////////////////////
//      MatrixNoGoConSpec       //
//////////////////////////////////

void MatrixNoGoConSpec::Initialize() {
  //  min_obj_type = &TA_MatrixCon;

  Defaults_init();
}

void MatrixNoGoConSpec::Defaults_init() {
  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  // SetUnique("wt_sig", true);
  wt_sig.gain = 6.0f;
  wt_sig.off = 1.25f;
}

void MatrixNoGoConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  //  lmix.err_sb = false;
}

//////////////////////////////////////////
//      Matrix Unit Spec                //
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
  lrate = 0.0f;                 // default is no lrate
}

void MatrixUnitSpec::Initialize() {
  SetUnique("bias_spec", true);
  bias_spec.type = &TA_MatrixBiasSpec;

  Defaults_init();
}

void MatrixUnitSpec::Defaults_init() {
  SetUnique("act", true);
  act.i_thr = ActFunSpec::NO_AH; // key for dopamine effects

  SetUnique("g_bar", true);
  g_bar.a = .03f;
  g_bar.h = .01f;

  SetUnique("maxda", true);
  maxda.val = MaxDaSpec::NO_MAX_DA;

  // SetUnique("noise_type", true);
  noise_type = NETIN_NOISE;

  // SetUnique("noise", true);
  noise.var = 0.0005f;
  // SetUnique("noise_adapt", true);
  noise_adapt.mode = NoiseAdaptSpec::FIXED_NOISE;
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

//////////////////////////////////
//      Matrix Layer Spec       //
//////////////////////////////////

void MatrixGoNogoGainSpec::Initialize() {
  on = false;
  go_p = go_n = nogo_p = nogo_n = 1.0f;
}

void MatrixMiscSpec::Initialize() {
  da_gain = 0.1f;
  nogo_inhib = 0.0f; // 0.2f;
  pvr_inhib = 0.8f;
  refract_inhib = 0.0f;
  no_pfc_thr = 0.0f;
  no_pfc_inhib = 0.2f;
}

/////////////////////////////////////////////////////

void MatrixLayerSpec::Initialize() {
  gating_type = SNrThalLayerSpec::MAINT;
  go_nogo = GO;

  Defaults_init();
}

void MatrixLayerSpec::Defaults_init() {
  // todo: sync with above
  matrix.nogo_inhib = 0.0f; // 0.2f;
  matrix.da_gain = 0.1f;
  matrix.no_pfc_thr = 0.0f;

  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .5f;

  //  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  //  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;

  // new default..
  //  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = 0.25f;
  unit_gp_inhib.on = true;
  unit_gp_inhib.gp_g = 1.0f;
  lay_gp_inhib.on = true;
  lay_gp_inhib.gp_g = 1.0f;

  //  SetUnique("tie_brk", true);        // turn on tie breaking by default
  tie_brk.on = false;
  tie_brk.diff_thr = 0.2f;
  tie_brk.thr_gain = 0.005f;
  tie_brk.loser_gain = 1.0f;
}

void MatrixLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void MatrixLayerSpec::HelpConfig() {
  String help = "MatrixLayerSpec Computation:\n\
 Each Matrix layer is either all Go or NoGo, with stripe-wise alignment between two\n\
 \nMatrixLayerSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure.\n\
 - Units must have a MatrixUnitSpec and must recv from PVLVDaLayerSpec layer\
 (calld DA typically) to get da modulation for learning signal\n\
 - Must recv from SNrThalLayerSpec to get final Go signal\n\
 - Go layer recv marker cons from NoGo, gets inhibition in proportion to avg NoGo act\n\
 - Other Recv conns are MatrixConSpec for learning based on the da-signal.\n\
 - Units must be organized into groups (stipes) of same number as PFC";
  taMisc::Confirm(help);
}

bool MatrixLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // SetUnique("decay", true);
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
  LeabraLayer* nogo_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) // self projection, skip it
      continue;
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(fmlay->spec.SPtr()->InheritsFrom(TA_PVLVDaLayerSpec)) da_lay = fmlay;
      if(fmlay->spec.SPtr()->InheritsFrom(TA_SNrThalLayerSpec)) snr_lay = fmlay;
      if(fmlay->spec.SPtr()->InheritsFrom(TA_MatrixLayerSpec)) nogo_lay = fmlay;
      continue;
    }
    if(go_nogo == NOGO) continue;
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

  if(go_nogo == GO) {
    if(lay->CheckError(nogo_lay == NULL, quiet, rval,
                "Could not find NoGo Matrix layer for Go Matrix -- must receive MarkerConSpec projection from one!")) {
      return false;
    }
    if(lay->CheckError(nogo_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
		       "NoGo Matrix layer does not have same number of stripes as we do!")) {
      return false;
    }
  }

  return true;
}

LeabraLayer* MatrixLayerSpec::SNrThalLayer(LeabraLayer* lay) {
  int snr_prjn_idx = 0; // actual arg value doesn't matter
  return FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
}
LeabraLayer* MatrixLayerSpec::PVLVDaLayer(LeabraLayer* lay) {
  int pvlvda_prjn_idx = 0; // actual arg value doesn't matter
  return FindLayerFmSpec(lay, pvlvda_prjn_idx, &TA_PVLVDaLayerSpec);
}

void MatrixLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  lay->SetUserData("tonic_da", 0.0f); // store tonic da per layer

  NameMatrixUnits(lay, net);
}

void MatrixLayerSpec::NameMatrixUnits(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  String nm;
  switch(gating_type) {
  case SNrThalLayerSpec::INPUT:
    nm = "i";
    break;
  case SNrThalLayerSpec::MAINT:
    nm = "m";
    break;
  case SNrThalLayerSpec::OUTPUT:
    nm = "o";
    break;
  default:			// compiler food
    break;
  }
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
      if(u->lesioned()) continue;
      u->name = nm;
    }
  }
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
    if(u->lesioned()) continue;
    us->Compute_MidMinusAct(u, net);
  }
}

void MatrixLayerSpec::Compute_ZeroMidMinusAct_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_m2 = 0.0f;
  }
}

float MatrixLayerSpec::Compute_NoGoInhibGo_ugp(LeabraLayer* lay,
					       Layer::AccessMode acc_md, int gpidx,
					       LeabraNetwork* net) {
  if(matrix.nogo_inhib == 0.0f) return 0.0f;

  int nogo_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* nogo_lay = FindLayerFmSpec(lay, nogo_prjn_idx, &TA_MatrixLayerSpec);
  PBWMUnGpData* nogo_gpd = (PBWMUnGpData*)nogo_lay->ungp_data.FastEl(gpidx);
  float nogo_i = matrix.nogo_inhib * nogo_gpd->acts.avg;
  if(nogo_i > 1.0f) nogo_i = 1.0f;
  return nogo_i;
}

float MatrixLayerSpec::Compute_GoPfcThrInhib_ugp(LeabraLayer* lay,
						 Layer::AccessMode acc_md, int gpidx,
						 LeabraNetwork* net) {
  if(matrix.no_pfc_thr == 0.0f || matrix.no_pfc_inhib) return 0.0f;

  float noact_i = 0.0f;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(!fmlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) continue;
    PFCLayerSpec* pfcls = (PFCLayerSpec*)fmlay->GetLayerSpec();
    if(pfcls->pfc_layer != PFCLayerSpec::SUPER) continue; // only super matters..
    if(pfcls->pfc_type != gating_type) continue; // only same type matters!
    if(gpidx < fmlay->gp_geom.n) {
      LeabraUnGpData* fm_gpd = (LeabraUnGpData*)fmlay->ungp_data.FastEl(gpidx);
      if(fm_gpd->acts.avg < matrix.no_pfc_thr) {
	noact_i = matrix.no_pfc_inhib;
	break;
      }
    }
  }
  return noact_i;
}

float MatrixLayerSpec::Compute_RefractInhib_ugp(LeabraLayer* lay,
						Layer::AccessMode acc_md, int gpidx,
						LeabraNetwork* net) {
  if(matrix.refract_inhib == 0.0f) return 0.0f;
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  if(gpd->mnt_count == 1) {	// only true if gated last trial
    return matrix.refract_inhib;
  }
  return 0.0f;
}

float MatrixLayerSpec::Compute_PVrInhib_ugp(LeabraLayer* lay,
					    Layer::AccessMode acc_md, int gpidx,
					    LeabraNetwork* net) {
  if(matrix.pvr_inhib == 0.0f) return 0.0f;
  float pvr_i = 0.0f;
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  if(er_avail) {
    if(gating_type != SNrThalLayerSpec::OUTPUT)
      pvr_i = matrix.pvr_inhib;
  }
  else {
    if(gating_type == SNrThalLayerSpec::OUTPUT)
      pvr_i = matrix.pvr_inhib;
  }
  return pvr_i;
}


void MatrixLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  if(go_nogo == GO) {
    for(int gi=0; gi<lay->gp_geom.n; gi++) {
      PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);

      float nogo_i = Compute_NoGoInhibGo_ugp(lay, acc_md, gi, net);
      float noact_i = Compute_GoPfcThrInhib_ugp(lay, acc_md, gi, net);
      float refract_i = Compute_RefractInhib_ugp(lay, acc_md, gi, net);
      float pvr_i = Compute_PVrInhib_ugp(lay, acc_md, gi, net);

      gpd->nogo_inhib = nogo_i;
      gpd->no_pfc_inhib = noact_i;
      gpd->refract_inhib = refract_i;
      gpd->pvr_inhib = pvr_i;
      
      float mult_fact = 1.0f;
      if(nogo_i > 0.0f) mult_fact *= (1.0f - nogo_i);
      if(noact_i > 0.0f) mult_fact *= (1.0f - noact_i);
      if(refract_i > 0.0f) mult_fact *= (1.0f - refract_i);
      if(pvr_i > 0.0f) mult_fact *= (1.0f - pvr_i);

      if(mult_fact != 1.0f) {
	for(int j=0;j<nunits;j++) {
	  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gi);
	  if(u->lesioned()) continue;
	  u->net *= mult_fact;
	  u->i_thr = u->Compute_IThresh(net);
	}
      }
    }
  }

  inherited::Compute_NetinStats(lay, net);
}

void MatrixLayerSpec::Compute_GatingActs_ugp(LeabraLayer* lay,
                                       Layer::AccessMode acc_md, int gpidx,
                                       LeabraNetwork* net) {
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  int gate_cycle = snr_ls->snrthal.gate_cycle;

  PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(gpidx);
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  gpd->CopyPBWMData(*snr_gpd);	// always grab from snr
  if(snr_gpd->go_fired_now) {
    Compute_MidMinusAct_ugp(lay, acc_md, gpidx, net); // grab our mid minus
  }
  else {
    if(net->ct_cycle == gate_cycle+1 && !snr_gpd->go_fired_trial) { // nogos will be nogos for sure by now
      Compute_ZeroMidMinusAct_ugp(lay, acc_md, gpidx, net); // zero our mid minus
    }
  }
}

// will clear anyone you tell it too -- clears activations of stripes, typically that fired Go early to allow others to Go in the late window, i.e., output gating guys
void MatrixLayerSpec::Compute_ClearActAfterGo(LeabraLayer* lay, Layer::AccessMode acc_md,
				  int gpidx, LeabraNetwork* net) {
  if(gpidx < 0) { // -1 means do all
    for(int gpidx=0; gpidx<lay->gp_geom.n; gpidx++) {
       Compute_ClearActAfterGo_ugp(lay, acc_md, gpidx, net);
    }
  }
  else if(gpidx < lay->gp_geom.n) {
     Compute_ClearActAfterGo_ugp(lay, acc_md, gpidx, net);
  }
}

void MatrixLayerSpec::Compute_ClearActAfterGo_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
						  int gpidx, LeabraNetwork* net) {
  int snr_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  int nunits = lay->UnitAccess_NUnits(acc_md); // lay = matrix_go - how many units each unit group
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int j=0;j<nunits;j++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
    if(u->lesioned()) continue;
    u->vcb.g_a = 1.0f;
    us->Compute_Conduct(u, net); // update displayed conductances!
  }
}

void MatrixLayerSpec::Compute_CycleStats(LeabraLayer* lay,  LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int gpidx=0; gpidx<lay->gp_geom.n; gpidx++) {
    Compute_GatingActs_ugp(lay, acc_md, gpidx, net);
  }
  inherited::Compute_CycleStats(lay, net);
}

// this is called at end of plus phase, to establish a da value for driving learning

void MatrixLayerSpec::Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net) {
  // float lay_ton_da = lay->GetUserDataAsFloat("tonic_da");

  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  int snr_prjn_idx = 0;
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);

  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    LeabraUnit* snr_u = (LeabraUnit*)snr_lay->UnitAccess(Layer::ACC_GP, 0, gi);
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);
    float snrthal_act = 0.0f;
    if(!snr_u->lesioned())
      snrthal_act = matrix.da_gain * snr_u->act_m2;

    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, i, gi);
      if(u->lesioned()) continue;
      if(go_nogo == NOGO) {
	if(gpd->go_fired_trial)
	  u->dav = 0.0f;	  // we don't learn on our own gating trial!
	else
	  u->dav *= matrix.da_gain; // not gated by snrthal act -- always happens!
      }
      else {
	u->dav *= snrthal_act;
      }
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

//////////////////////////////////
//      PFC Layer Spec          //
//////////////////////////////////

void PFCDeepGatedConSpec::Initialize() {
  Defaults_init();
}

void PFCDeepGatedConSpec::Defaults_init() {
}


void PFCsUnitSpec::Initialize() {
  Defaults_init();
}

void PFCsUnitSpec::Defaults_init() {
}

// this sets default wt scale from deep prjns to super to 0
void PFCsUnitSpec::Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net) {
  // NOTE: must keep this sync'd with one in leabra.cpp
  // this is all receiver-based and done only at beginning of a trial
  u->net_scale = 0.0f;  // total of scale values for this unit's inputs

  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return;
  if(!rlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) return;
  PFCLayerSpec* ls = (PFCLayerSpec*) rlay->GetLayerSpec();
  int rgpidx = u->UnitGpIdx();
  PBWMUnGpData* gpd = (PBWMUnGpData*)rlay->ungp_data.FastEl(rgpidx);

  float inhib_net_scale = 0.0f;
  int n_active_cons = 0;        // track this for bias weight scaling!
  bool old_scaling = false;
  // possible dependence on recv_gp->size is why this cannot be computed in Projection
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    if(from->lesioned() || !recv_gp->size)       continue;
     // this is the normalization value: takes into account target activity of layer
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    cs->Compute_NetinScale(recv_gp, from);
    if(cs->wt_scale.old)
      old_scaling = true; // any old = old..

    // PBWM New part goes right here!
    if(ls->pfc_layer == PFCLayerSpec::SUPER) {
      if(from->GetLayerSpec()->GetTypeDef() == &TA_PFCLayerSpec) {
	PFCLayerSpec* fmls = (PFCLayerSpec*)from->GetLayerSpec();
	if(fmls->pfc_layer == PFCLayerSpec::DEEP) {
	  if(!gpd->go_fired_trial)
	    recv_gp->scale_eff = 0.0f; // negate!!
	}
      }
    }
    // end new part

    if(cs->inhib && !old_scaling) {
      inhib_net_scale += cs->wt_scale.rel;
    }
    else {
      n_active_cons++;
      u->net_scale += cs->wt_scale.rel; // note: negated still contributes to rel!
    }
  }
  // add the bias weight into the netinput, scaled by 1/n
  if(u->bias.size) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.SPtr();
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(u->n_recv_cons > 0)
      u->bias_scale /= (float)u->n_recv_cons; // one over n scaling for bias!
  }
  // now renormalize
  if(u->net_scale > 0.0f) {
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      Projection* prjn = (Projection*) recv_gp->prjn;
      LeabraLayer* from = (LeabraLayer*) prjn->from.ptr();
      if(from->lesioned() || !recv_gp->size)     continue;
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(cs->inhib && !old_scaling) continue; // norm separately
      recv_gp->scale_eff /= u->net_scale; // normalize by total connection scale
    }
  }
  // separately normalize inhibitory connections
  if(inhib_net_scale > 0.0f) {
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
      if(from->lesioned() || !recv_gp->size)     continue;
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(!cs->inhib) continue; // norm separately
      recv_gp->scale_eff /= inhib_net_scale; // normalize by total connection scale
    }
  }
}

void PFCsUnitSpec::Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  Compute_LearnMod(u, net);	// don't call dwt norm itself -- done for units that actually learn
}

void PFCsUnitSpec::Compute_LearnMod(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return;

  if(!rlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) return;
  PFCLayerSpec* ls = (PFCLayerSpec*) rlay->GetLayerSpec();

  // get my unit group index for accessing ungp data structures
  int rgpidx;
  int rui;
  rlay->UnGpIdxFmUnitIdx(u->idx, rui, rgpidx);

  PBWMUnGpData* gpd = (PBWMUnGpData*)rlay->ungp_data.SafeEl(rgpidx);
  if(TestWarning(!gpd, "Compute_LearnMod", "PFC Deep layer unit group out of range", 
		 String(rgpidx)))
    return;

  bool learn_now = gpd->go_fired_trial || gpd->mnt_count > 0;

  if(learn_now || !ls->gate.learn_deep_act) {
    // just do norm and go on your way
    inherited::Compute_dWt_Norm(u, net, thread_no);
    return;
  }

  // no learning now: zero out the weight changes
  for(int g = 0; g < u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->size) continue;

    for(int i=0; i<recv_gp->size; i++) {
      LeabraCon* cn = (LeabraCon*)recv_gp->PtrCn(i);
      cn->dwt = 0.0f;
    }
  }
}

void PFCGateSpec::Initialize() {
  learn_deep_act = true;
  in_mnt = 1;
  out_mnt = 0;
  maint_decay = 0.02f;
  maint_thr = 0.2f;
}

void PFCGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void PFCLayerSpec::Initialize() {
  pfc_type = SNrThalLayerSpec::MAINT;
  pfc_layer = SUPER;

  Defaults_init();
}

void PFCLayerSpec::Defaults_init() {
  gate.learn_deep_act = true;
  gate.maint_decay = 0.02f;

  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .5f;

  // SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  gp_kwta.pct = 0.15f;
  gp_kwta.diff_act_pct = true;
  gp_kwta.act_pct = 0.05f; // s = 0.05, d = 0.02

  unit_gp_inhib.on = true;
  unit_gp_inhib.gp_g = 1.0f;

  // SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;   // this is the one exception!
}

void PFCLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate.UpdateAfterEdit_NoGui();
}

void PFCLayerSpec::HelpConfig() {
  String help = "PFCLayerSpec Computation:\n\
 The PFC deep layers are gated by thalamocortical prjns from SNrThal.\n\
 They maintain activation over time (activation-based working memory) via \
 excitatory intracelluar ionic mechanisms (implemented in hysteresis channels, gc.h),\
 and excitatory connections with superficial pfc layers, which is toggled by SNrThal.\n\
 Updating occurs by mid_minus_cycle, based on SNrThal act_m2 activations.\n\
 \nPFCLayerSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure layers.\n\
 - First prjn must be from PFC superficial layer (can be any spec type)\n\
 - Units must be organized into groups so that the sum of the number of all\
   groups across all PFC deep layers must correspond to the number of matrix.\
   groups (stripes).";
  taMisc::Confirm(help);
}

bool PFCLayerSpec::CheckConfig_Layer(Layer* ly,  bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    // SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.0f;
    decay.clamp_phase2 = false;
  }

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  net->dwt_norm_enabled = true; // required for learning modulation

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  if(lay->CheckError(net->phase_order == LeabraNetwork::MINUS_PLUS_PLUS, quiet, rval,
                "requires LeabraNetwork phase_oder = MINUS_PLUS, I just set it for you")) {
    net->phase_order = LeabraNetwork::MINUS_PLUS;
  }

  if(lay->CheckError(net->no_plus_test, quiet, rval,
                "requires LeabraNetwork no_plus_test = false, I just set it for you")) {
    net->no_plus_test = false;
  }

  if(lay->CheckError(net->mid_minus_cycle < 5, quiet, rval,
                "requires LeabraNetwork min_minus_cycle > 5, I just set it to 40 for you")) {
    net->mid_minus_cycle = 40;
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

  // SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative

  if(pfc_layer == DEEP) {
    LeabraLayer* snr_lay = SNrThalLayer(lay);
    if(lay->CheckError(!snr_lay, quiet, rval,
		       "no projection from SNrThal Layer found: must exist with MarkerConSpec connection")) {
      return false;
    }

    if(lay->CheckError(u->recv.size < 2, quiet, rval,
		       "Must receive at least 2 projections (0 = from superficial pfc, other from SNrThal")) {
      return false;
    }
    if(lay->CheckError(u->recv.size < 2, quiet, rval,
		       "Must receive at least 2 projections (0 = from superficial pfc, other from SNrThal")) {
      return false;
    }
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(0);
    if(lay->CheckError(recv_gp->size == 0, quiet, rval,
		       "first projection from superficial pfc does not have a connection")) {
      return false;
    }
    if(lay->CheckError(recv_gp->size > 1, quiet, rval,
		       "warning: first projection from superficial pfc has more than 1 connection -- should just be a single one-to-one projection from superficial to deep!")) {
    }

    if(pfc_type != SNrThalLayerSpec::OUTPUT) {
      LeabraLayer* lve_lay = LVeLayer(lay);
      if(lay->CheckError(!lve_lay, quiet, rval,
			 "LVe layer not found -- PFC deep layers must project to LVe")) {
      }
    }
  }
  else {			// SUPER
    LeabraLayer* deep = DeepLayer(lay);
    if(lay->CheckError(!deep, quiet, rval,
		       "Corresponding Deep layer not found -- PFC SUPER layers must project sending one-to-one prjn to DEEP layers")) {
    }

    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
      if(from->lesioned() || !recv_gp->size)       continue;
      if(from->GetLayerSpec()->GetTypeDef() == &TA_PFCLayerSpec) {
	PFCLayerSpec* fmls = (PFCLayerSpec*)from->GetLayerSpec();
	if(fmls->pfc_layer == PFCLayerSpec::DEEP) {
	  LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
	  if(lay->CheckError(!cs->InheritsFrom(&TA_PFCDeepGatedConSpec), quiet, rval,
					       "Connection from DEEP PFC to SUPER PFC is not using a PFCDeepGatedConSpec -- this will not work properly with the gating of these connections!  con from layer:", from->name)) {
	  }
	}
      }
    }
  }

  return true;
}

// super accesses snrthal etc via deep -- everything there is indirect!

LeabraLayer* PFCLayerSpec::DeepLayer(LeabraLayer* lay) {
  if(TestError(pfc_layer != SUPER, "DeepLayer",
	       "Programmer error: trying to get deep layer from deep layer!"))
    return NULL;
  LeabraLayer* deep = NULL;
  for(int i=0; i< lay->send_prjns.size; i++) {
    Projection* prj = lay->send_prjns[i];
    LeabraLayer* play = (LeabraLayer*)prj->layer;
    if(play->GetLayerSpec()->GetTypeDef() != &TA_PFCLayerSpec) continue;
    PFCLayerSpec* pfcls = (PFCLayerSpec*)play->GetLayerSpec();
    if(pfcls->pfc_layer == DEEP) {
      deep = play;
      break;
    }
  }
  return deep;
}  

LeabraLayer* PFCLayerSpec::SNrThalLayer(LeabraLayer* lay) {
  if(pfc_layer == SUPER) {
    LeabraLayer* deep = DeepLayer(lay);
    if(!deep) return NULL;
    PFCLayerSpec* dls = (PFCLayerSpec*)deep->GetLayerSpec();
    return dls->SNrThalLayer(deep);
  }
  int snr_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  return snr_lay;
}

LeabraLayer* PFCLayerSpec::LVeLayer(LeabraLayer* lay) {
  if(pfc_layer == SUPER) {
    LeabraLayer* deep = DeepLayer(lay);
    if(!deep) return NULL;
    return LVeLayer(deep);
  }
  // find the LVe layer that we drive
  LeabraLayer* lve = NULL;
  for(int i=0; i< lay->send_prjns.size; i++) {
    Projection* prj = lay->send_prjns[i];
    LeabraLayer* play = (LeabraLayer*)prj->layer;
    if(play->GetLayerSpec()->GetTypeDef() != &TA_LVeLayerSpec) continue;
    lve = play;
    break;
  }
  return lve;
}

LeabraLayer* PFCLayerSpec::LViLayer(LeabraLayer* lay) {
  if(pfc_layer == SUPER) {
    LeabraLayer* deep = DeepLayer(lay);
    if(!deep) return NULL;
    return LViLayer(deep);
  }
  // find the Lvi layer that we drive
  LeabraLayer* lvi = NULL;
  for(int i=0; i< lay->send_prjns.size; i++) {
    Projection* prj = lay->send_prjns[i];
    LeabraLayer* play = (LeabraLayer*)prj->layer;
    if(play->GetLayerSpec()->GetTypeDef() != &TA_LViLayerSpec) continue;
    lvi = play;
    break;
  }
  return lvi;
}

void PFCLayerSpec::Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net) {
  if(pfc_layer != DEEP) return;
  int snr_st_idx, n_in, n_mnt, n_out;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_mnt, n_out);
  for(int g=0; g < lay->gp_geom.n; g++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(g);
    PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + g);
    gpd->CopyPBWMData(*snr_gpd);
  }
}

void PFCLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Compute_TrialInitGates(lay, net);
}

void PFCLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // do NOT do this -- triggered by the snrthal gating signal
}

void PFCLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_m2 = u->act_eq;
  }
}

void PFCLayerSpec::Clear_Maint(LeabraLayer* lay, LeabraNetwork* net, int stripe_no) {
  if(pfc_layer != DEEP) return;
  Compute_MaintUpdt(lay, net, CLEAR, stripe_no);
}

void PFCLayerSpec::Compute_MaintUpdt(LeabraLayer* lay, LeabraNetwork* net,
				     MaintUpdtAct updt_act, int stripe_no) {
  if(pfc_layer != DEEP) return;
  Layer::AccessMode acc_md = Layer::ACC_GP;
  if(stripe_no < 0) {
    for(int gpidx=0; gpidx<lay->gp_geom.n; gpidx++) {
      Compute_MaintUpdt_ugp(lay, acc_md, gpidx, updt_act, net);
    }
  }
  else if(stripe_no < lay->gp_geom.n) {
    Compute_MaintUpdt_ugp(lay, acc_md, stripe_no, updt_act, net);
  }
}

LeabraLayer* PFCLayerSpec::SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
						   int& n_in, int& n_mnt, int& n_out) {
  snr_st_idx = 0;
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  snr_st_idx = snr_ls->SNrThalStartIdx(snr_lay, pfc_type, n_in, n_mnt, n_out);
  return snr_lay;
}

void PFCLayerSpec::Compute_MaintUpdt_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                         MaintUpdtAct updt_act, LeabraNetwork* net) {
  int snr_st_idx, n_in, n_mnt, n_out;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_mnt, n_out);
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + gpidx);
  int nunits = lay->UnitAccess_NUnits(acc_md);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int j=0;j<nunits;j++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
    if(u->lesioned()) continue;
    if(updt_act == STORE) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(0);
      LeabraUnit* super_u = (LeabraUnit*)recv_gp->Un(0);
      u->vcb.g_h = u->maint_h = super_u->act_eq; // note: store current superficial act value
    }
    else if(updt_act == CLEAR) {
      u->vcb.g_h = u->maint_h = 0.0f;
      gpd->mnt_count = -1;	// indication of empty
      snr_gpd->mnt_count = -1;
    }
    else if(updt_act == DECAY) {
      u->maint_h -= u->maint_h * gate.maint_decay;
      if(u->maint_h < 0.0f) u->maint_h = 0.0f;
      u->vcb.g_h = u->maint_h;
    }
    us->Compute_Conduct(u, net); // update displayed conductances!
  }
}

void PFCLayerSpec::Compute_MaintAct_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
					    int gpidx, LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);

  if(pfc_layer == DEEP) {
    for(int j=0;j<nunits;j++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
      if(u->lesioned()) continue;
      u->act = u->act_eq = u->act_nd = u->maint_h; // we always always just copy maint
      u->da = 0.0f;
    }
  }
  else {			// SUPER
    if(gpd->mnt_count > 0) {	// post-gated maintenance mode -- get from deep layer
      LeabraLayer* deep = DeepLayer(lay);
      for(int j=0;j<nunits;j++) {
	LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
	LeabraUnit* du = (LeabraUnit*)deep->UnitAccess(acc_md, j, gpidx);
	if(u->lesioned()) continue;
	u->act = u->act_eq = u->act_nd = u->maint_h = u->gc.h = du->maint_h; // copy deep! 
	u->da = 0.0f;
      }
    }
  }
}

void PFCLayerSpec::GateOnDeepPrjns_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
				      int gpidx,LeabraNetwork* net) {
  if(pfc_layer != SUPER) return;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int j=0;j<nunits;j++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
    if(u->lesioned()) continue;

    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
      if(from->lesioned() || !recv_gp->size)       continue;
      if(from->GetLayerSpec()->GetTypeDef() == &TA_PFCLayerSpec) {
	PFCLayerSpec* fmls = (PFCLayerSpec*)from->GetLayerSpec();
	if(fmls->pfc_layer == PFCLayerSpec::DEEP) {
	  LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
	  cs->Compute_NetinScale(recv_gp, from);
	  recv_gp->scale_eff /= u->net_scale; // normalize by total connection scale (prev computed)
	}
      }
    }
  }
}

void PFCLayerSpec::Compute_Gating(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;

  int snr_st_idx, n_in, n_mnt, n_out;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_mnt, n_out);
  SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();

  int gate_cycle = snrls->snrthal.gate_cycle;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + mg);
    gpd->CopyPBWMData(*snr_gpd);		// always grab from snr, which is the source

    if(net->ct_cycle == gate_cycle) {
      Compute_MidMinusAct_ugp(lay, acc_md, mg, net); // mid minus reflects gating time activations in any case for all trials
    }

    if(pfc_layer == DEEP && net->ct_cycle >= gate_cycle) {
      if(gpd->go_fired_trial) {	// continuously update on trial itself
	Compute_MaintUpdt_ugp(lay, acc_md, mg, STORE, net);
      }
    }
    
    if(pfc_layer == SUPER && net->ct_cycle == gate_cycle && gpd->go_fired_trial) {
      GateOnDeepPrjns_ugp(lay, acc_md, mg, net); // gate on the deep prjns
    }

    // always update activations regardless
    Compute_MaintAct_ugp(lay, acc_md, mg, net);
  }
}

void PFCLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_Gating(lay, net);   // continuously during whole trial
  inherited::Compute_CycleStats(lay, net);
}

void PFCLayerSpec::Compute_ClearNonMnt(LeabraLayer* lay, LeabraNetwork* net) {
  if(pfc_type == SNrThalLayerSpec::MAINT) return;	// no clear
  if(pfc_layer != DEEP) return;

  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    if(pfc_type == SNrThalLayerSpec::INPUT && gpd->mnt_count == gate.in_mnt) {
      Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);
    }
    if(pfc_type == SNrThalLayerSpec::OUTPUT && gpd->mnt_count == gate.out_mnt) {
      Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);
    }
  }
}

void PFCLayerSpec::Compute_FinalGating(LeabraLayer* lay, LeabraNetwork* net) {
  if(pfc_layer != DEEP) return;
  if(pfc_type != SNrThalLayerSpec::MAINT) {
    Compute_ClearNonMnt(lay, net); 
    return;
  }

  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);

    if(gpd->mnt_count >= 1) {
      Compute_MaintUpdt_ugp(lay, acc_md, mg, DECAY, net);
    }
    // todo: apply maint_thr here!
  }
}

void PFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(net->phase_no == 1) {
    Compute_FinalGating(lay, net);     // final gating
  }
}


///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////
//          GpCustomPrjnSpecBase        //
//////////////////////////////////////////

void GpCustomPrjnSpecBase::AllocGp_Recv(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			       Layer::AccessMode sacc_md, int n_send) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md) * n_send;

  // pre-allocate connections!
  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
    if(ru)
      ru->RecvConsPreAlloc(su_nunits, prjn);
  }
}

void GpCustomPrjnSpecBase::AllocGp_Send(Projection* prjn, Layer::AccessMode sacc_md, int sgpidx,
			       Layer::AccessMode racc_md, int n_recv) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md) * n_recv;
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  // pre-allocate connections!
  for(int sui=0; sui < su_nunits; sui++) {
    Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
    if(su)
      su->SendConsPreAlloc(ru_nunits, prjn);
  }
}

void GpCustomPrjnSpecBase::Connect_Gp(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			     Layer::AccessMode sacc_md, int sgpidx,
			     int extra_rgp_alloc, int extra_sgp_alloc) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);
  int alloc_ru = ru_nunits;
  if(extra_rgp_alloc > 0) {
    alloc_ru *= extra_rgp_alloc;
  }
  int alloc_su = su_nunits;
  if(extra_sgp_alloc > 0) {
    alloc_su *= extra_sgp_alloc;
  }

  // pre-allocate connections!
  if(extra_sgp_alloc >= 0) {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      if(ru)
	ru->RecvConsPreAlloc(alloc_su, prjn);
    }
  }
  if(extra_rgp_alloc >= 0) {
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      if(su)
	su->SendConsPreAlloc(alloc_ru, prjn);
    }
  }

  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      if(su && (self_con || (ru != su)))
        ru->ConnectFrom(su, prjn);
    }
  }
}

//////////////////////////////////////////
//              PFCLVPrjnSpec           //
//////////////////////////////////////////

void PFCLVPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

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

  if(n_lv_stripes <= 1) {       // just full connectivity
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


//////////////////////////////////////////
//           SNrPrjnSpec                //
//////////////////////////////////////////

void SNrPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  LeabraLayer* to_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* fm_lay = (LeabraLayer*)prjn->from.ptr();

  int n_in, n_mnt, n_out;
  int snr_st_idx = -1;
  if(to_lay->GetLayerSpec()->InheritsFrom(&TA_SNrThalLayerSpec)) {
    SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)to_lay->GetLayerSpec();
    if(fm_lay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) {
      PFCLayerSpec* pfcls = (PFCLayerSpec*)fm_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(to_lay, pfcls->pfc_type, n_in, n_mnt, n_out);
    }
    else if(fm_lay->GetLayerSpec()->InheritsFrom(&TA_MatrixLayerSpec)) {
      MatrixLayerSpec* mtxls = (MatrixLayerSpec*)fm_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(to_lay, mtxls->gating_type, n_in, n_mnt, n_out);
    }
    if(snr_st_idx >= 0) {
      for(int i=0; i<fm_lay->gp_geom.n; i++) {
	Connect_Gp(prjn, Layer::ACC_GP, snr_st_idx + i, Layer::ACC_GP, i);
      }
    }
  }
  else if(fm_lay->GetLayerSpec()->InheritsFrom(&TA_SNrThalLayerSpec)) {
    SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)fm_lay->GetLayerSpec();
    if(to_lay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) {
      PFCLayerSpec* pfcls = (PFCLayerSpec*)to_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(fm_lay, pfcls->pfc_type, n_in, n_mnt, n_out);
    }
    else if(to_lay->GetLayerSpec()->InheritsFrom(&TA_MatrixLayerSpec)) {
      MatrixLayerSpec* mtxls = (MatrixLayerSpec*)to_lay->GetLayerSpec();
      snr_st_idx = snr_ls->SNrThalStartIdx(fm_lay, mtxls->gating_type, n_in, n_mnt, n_out);
    }
    if(snr_st_idx >= 0) {
      for(int i=0; i<to_lay->gp_geom.n; i++) {
	Connect_Gp(prjn, Layer::ACC_GP, i, Layer::ACC_GP, snr_st_idx + i);
      }
    }
  }
  else {
    if(TestError(true,
		 "Connect_impl", "either the recv or send layer must be a SNrThal layer, recv layer:", to_lay->name, "send layer:", fm_lay->name))
      return;
  }
}

//////////////////////////////////////////
//           PVrToMatrixGoPrjnSpec      //
//////////////////////////////////////////

void PVrToMatrixGoPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  LeabraLayer* mtx_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* pvr_lay = (LeabraLayer*)prjn->from.ptr();

  MatrixLayerSpec* mtx_ls = (MatrixLayerSpec*)mtx_lay->GetLayerSpec();
  if(TestError(!mtx_ls || !mtx_ls->InheritsFrom(&TA_MatrixLayerSpec),
	       "Connect_impl", "recv layer is not a MatrixLayerSpec.  recv layer:", mtx_lay->name))
    return;
  if(TestError(pvr_lay->units.size < 4,
	       "Connect_impl", "PVr layer doesn't have at least 4 units:", pvr_lay->name))
    return;

  int n_per_gp = mtx_lay->un_geom.n;
  int n_gps = mtx_lay->gp_geom.n;
  int tot_un = n_per_gp * n_gps;

  Layer::AccessMode racc_md = Layer::ACC_GP;

  Unit* pvr_su = NULL;
  switch(mtx_ls->gating_type) {
  case SNrThalLayerSpec::INPUT:
    pvr_su = pvr_lay->units.SafeEl(2); // .5 middle value unit
    break;
  case SNrThalLayerSpec::MAINT:
    pvr_su = pvr_lay->units.SafeEl(2); // .5 middle value unit
    break;
  case SNrThalLayerSpec::OUTPUT:
    pvr_su = pvr_lay->units.SafeEl(3); // 1 right value unit
    break;
  default:			// compiler food
    break;
  }

  if(!pvr_su) return;
  pvr_su->SendConsPreAlloc(tot_un, prjn);

  for(int rgi=0; rgi < n_gps; rgi++) {
    for(int rui=0; rui < n_per_gp; rui++) {
      Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
      ru->RecvConsPreAlloc(1, prjn);
    }
  }

  for(int rgi=0; rgi < n_gps; rgi++) {
    for(int rui=0; rui < n_per_gp; rui++) {
      Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
      ru->ConnectFrom(pvr_su, prjn);
    }
  }
}


/////////////////////////////////////
//        TopoWtsPrjnSpec      //
/////////////////////////////////////

void TopoWtsPrjnSpec::Initialize() {
  wt_range.min = 0.1f;
  wt_range.max = 0.5f;
  wt_range.UpdateAfterEdit_NoGui();
  invert = false;
  grad_x = true;
  grad_x_grad_y = false;
  grad_y = true;
  grad_y_grad_x = false;
  wrap = true;
  grad_type = LINEAR;
  use_send_gps = false;
  use_recv_gps = false;
  custom_send_range = false;
  custom_recv_range = false;
  send_range_start = 0;
  send_range_end = -1;
  recv_range_start = 0;
  recv_range_end = -1;
  gauss_sig = 0.3f;

  already_warned = 0;

  Defaults_init();
}

void TopoWtsPrjnSpec::Defaults_init() {
  init_wts = true;
  add_rnd_wts = true;
  add_rnd_wts_scale = 1.0f;
}

bool TopoWtsPrjnSpec::TestWarning(bool test, const char* fun_name,
				  const char* a, const char* b, const char* c,
				  const char* d, const char* e, const char* f,
				  const char* g, const char* h) const {
  if(!test) return false;
  const_cast<TopoWtsPrjnSpec*>(this)->already_warned++;
  if(already_warned > 10)
    return true;
  return taMisc::TestWarning(this, test, fun_name, a, b, c, d, e, f, g, h);
}

void TopoWtsPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  already_warned = 0;		// reset

  int recv_no = prjn->from->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no--;

  int send_no = prjn->layer->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    send_no--;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  if(!use_recv_gps) { // recv NO -- not using recv_gps
    for(int y = 0; y < prjn->layer->flat_geom.y; y++) {
      for(int x = 0; x < prjn->layer->flat_geom.x; x++) {
	Unit* ru = prjn->layer->UnitAtCoord(x,y);
	//ru->ConnectFrom(su, prjn);
	if(!use_send_gps) { // send NO -- not using send_gps
	  for(int y = 0; y < prjn->from->flat_geom.y; y++) {
	    for(int x = 0; x < prjn->from->flat_geom.x; x++) {
	      Unit* su = prjn->from->UnitAtCoord(x,y);
	      ru->ConnectFrom(su, prjn);
	    }
	  }
	}
	else { // send YES -- using send_gps
	  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
	    if(self_con || (ru != su))
	      ru->ConnectFrom(su, prjn);
	  }
	}
      }
    }
  }
  else { // recv YES -- using recv_gps
    FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
      //bool ignore_unit_gps = false;
      //prjn->from.ptr()->unit_groups
      //if(!use_send_gps && prjn->from.ptr()->unit_groups)
      //ignore_unit_gps = true;
      //if(ignore_unit_gps) {
      if(!use_send_gps) { // send NO -- not using send_gps
	for(int y = 0; y < prjn->from->flat_geom.y; y++) {
	  for(int x = 0; x < prjn->from->flat_geom.x; x++) {
	    Unit* su = prjn->from->UnitAtCoord(x,y);
	    ru->ConnectFrom(su, prjn);
	  }
	}
      }
      else { // send YES -- using send gps
	FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
	  if(self_con || (ru != su))
	    ru->ConnectFrom(su, prjn);
	}
      }
    }
  }
}

void TopoWtsPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(TestError((grad_x_grad_y && grad_y && !grad_y_grad_x), "TopoWtsPrjnSpec::C_InitWeights ", " both send_x and send_y are mapping to recv_y; you must set either: grad_y = false or grad_y_grad_x = true -- aborting!"))
    return;
  if(TestError((grad_y_grad_x && grad_x && !grad_x_grad_y), "TopoWtsPrjnSpec::C_InitWeights ", " both send_y and send_x are mapping to recv_x; you must set either: grad_x = false or grad_x_grad_y = true -- aborting!"))
    return;
  if(TestError(((send_range_end.x != -1 && send_range_end.x < send_range_start.x) || (send_range_end.y != -1 && send_range_end.y < send_range_start.y)), "TopoWtsPrjnSpec::C_InitWeights ", " either send_range_end.x or send_range_end.y is less than start_idx. Please correct -- aborting!"))
    return;
  if(TestError(((recv_range_end.x != -1 && recv_range_end.x < recv_range_start.x) || (recv_range_end.y != -1 && recv_range_end.y < recv_range_start.y)), "TopoWtsPrjnSpec::C_InitWeights ", " either recv_range_end.x or recv_range_end.y is less than start_idx. Please correct -- aborting!"))
    return;

  if(TestError((use_send_gps && !(prjn->from.ptr()->unit_groups)), "TopoWtsPrjnSpec::C_InitWeights ", " use_send_gps TRUE, but send_lay does not have unit groups. Please fix -- aborting!"))
    return;
  if(TestError((use_recv_gps && !(prjn->layer->unit_groups)), "TopoWtsPrjnSpec::C_InitWeights ", " use_recv_gps TRUE, but recv_lay does not have unit groups. Please fix -- aborting!"))
    return;

  if(use_send_gps && prjn->from.ptr()->unit_groups) {
    if (use_recv_gps && prjn->layer->unit_groups)
      InitWeights_SendGpsRecvGps(prjn, cg, ru);
    else
      InitWeights_SendGpsRecvFlat(prjn, cg, ru);
  }
  else
    if (use_recv_gps && prjn->layer->unit_groups)
      InitWeights_SendFlatRecvGps(prjn, cg, ru);
    else
      InitWeights_SendFlatRecvFlat(prjn, cg, ru);
}

void TopoWtsPrjnSpec::SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist,
				  int cg_idx) {
  float wt_val;
  if(grad_type == LINEAR) {
    if(invert)
      wt_val = wt_range.min + dist * wt_range.Range();
    else
      wt_val = wt_range.max - dist * wt_range.Range();
  }
  else if(grad_type == GAUSSIAN) {
    float gaus = taMath_float::gauss_den_nonorm(dist, gauss_sig);
    if(invert)
      wt_val = wt_range.max - gaus * wt_range.Range();
    else
      wt_val = wt_range.min + gaus * wt_range.Range();
  }
  cg->Cn(cg_idx)->wt = wt_val;
}

// wrap calculations ///////////////////////////////
////////////////////////////////////////////////////
//      0       1       2       3    recv
//      0       .33     .66     1    rgp_x
//
//      0       1       2       3    send
//      0       .33     .66     1    sgp_x
//      4       5       6       7    wrp_x > .5  int
//      1.33    1.66    2       2.33 wrp_x > .5  flt
//      -4      -3      -2      -1   wrp_x < .5  int
//      -1.33   -1      -.66    -.33 wrp_x < .5  flt

void TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  TwoDCoord srs = send_range_start; // actual send range start
  TwoDCoord sre = send_range_end;   // actual send range end
  TwoDCoord rrs = recv_range_start; // actual recv range start
  TwoDCoord rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->gp_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->gp_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->gp_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->gp_geom.y-1;

  if(TestWarning((sre.x > send_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "send_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    sre.x = send_lay->gp_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "send_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    sre.y = send_lay->gp_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "recv_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    rre.x = recv_lay->gp_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "recv_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    rre.y = recv_lay->gp_geom.y-1;
  }
  // else should be the values set by user

  int rgpidx = ru->UnitGpIdx();
  TwoDCoord rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom

  float max_dist = 1.0f;
  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
    max_dist = sqrtf(2.0f);
  float dist = 0.0f;

  // is recv un_gp (and ru) in range?  -- if not, init wts and get next cg/ru
  if(rgp_pos.x < rrs.x || rgp_pos.x > rre.x || rgp_pos.y < rrs.y || rgp_pos.y > rre.y) {
    for(int i=0; i<cg->size; i++) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
    }
    return; // done -- get a new cg/ru
  }

  // if in-range, normalize recv range (for topological congruence to send range)
  rgp_pos.x -= rrs.x;
  rgp_pos.y -= rrs.y;

  //float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom.x-1, 1);
  //float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom.y-1, 1);
  float rgp_x = (float)rgp_pos.x / (float)MAX((rre.x - rrs.x), 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX((rre.y - rrs.y), 1);

  //  float max_dist = 1.0f;
  //  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
  //    max_dist = sqrtf(2.0f);

  // now adjust the send groups..
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int sgpidx = su->UnitGpIdx();
    TwoDCoord sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // position relative to overall gp geom
    // send un_gp in range?  -- get next un_gp if not
    if(sgp_pos.x < srs.x || sgp_pos.x > sre.x || sgp_pos.y < srs.y || sgp_pos.y > sre.y) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
      continue;
    }
    // if in-range, normalize send range (for topological congruence to recv range)
    sgp_pos.x -= srs.x;
    sgp_pos.y -= srs.y;

    float sgp_x = (float)sgp_pos.x / mxs_x;
    float sgp_y = (float)sgp_pos.y / mxs_y;

    float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps and send gps?
    if(wrap) {
      if(rgp_x > .5f)   wrp_x = (float)(sgp_pos.x + send_lay->gp_geom.x) / mxs_x;
      else              wrp_x = (float)(sgp_pos.x - send_lay->gp_geom.x) / mxs_x;
      if(rgp_y > .5f)   wrp_y = (float)(sgp_pos.y + send_lay->gp_geom.y) / mxs_y;
      else              wrp_y = (float)(sgp_pos.y - send_lay->gp_geom.y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	dist = taMath_float::euc_dist(sgp_x, sgp_y, rgp_x, rgp_y);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, rgp_x, rgp_y);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, rgp_x, rgp_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
      else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	dist = taMath_float::euc_dist(sgp_x, sgp_y, rgp_y, rgp_x);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, rgp_y, rgp_x);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_y, rgp_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, rgp_y, rgp_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
    }
    else if(grad_x) { // two cases: x to x and send_x to recv_y
      if(!grad_x_grad_y) { // i.e., the usual x to x case
	dist = fabsf(sgp_x - rgp_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - rgp_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_x to recv_y case
	dist = fabsf(sgp_x - rgp_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - rgp_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }
    else if(grad_y) { // again two cases: y to y and send_y to recv_x
      if(!grad_y_grad_x) { // i.e. the usual y to y case
	dist = fabsf(sgp_y - rgp_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - rgp_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_y to recv_x case
	dist = fabsf(sgp_y - rgp_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - rgp_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i); // i corresponds to simple su (not sgp) -- one of entries in the cg (congroup) list
  }
}

void TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat(Projection* prjn,
						  RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  TwoDCoord srs = send_range_start; // actual send range start
  TwoDCoord sre = send_range_end;   // actual send range end
  TwoDCoord rrs = recv_range_start; // actual recv range start
  TwoDCoord rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->gp_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->gp_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->flat_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->flat_geom.y-1;

  if(TestWarning((sre.x > send_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "send_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    sre.x = send_lay->gp_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "send_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    sre.y = send_lay->gp_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    rre.y = recv_lay->flat_geom.y-1;
  }
  // else should be the values set by user

  float ru_x = 0.0f;
  float ru_y = 0.0f;
  float max_dist = 1.0f;
  float dist = 0.0f;
  if (!recv_lay->unit_groups) { // no unit groups to worry about!
    TwoDCoord ru_pos;
    recv_lay->UnitLogPos(ru, ru_pos);
    if(grad_x && grad_y) {// applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    float dist = 0.0f;
    // is recv unit in range?  -- if not, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x || ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x; 	// start at start-indexed units!
    //ru_pos.y = ru_pos.y - rrs.y;
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    //float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }
  else { // recv_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    TwoDCoord ru_pos; // hold planar Cartesian coordinates within unit group
    int runidx = 0;
    int rgpidx = 0;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
    ru_pos.x = runidx % recv_lay->un_geom.x;
    ru_pos.y = runidx / recv_lay->un_geom.x;
    TwoDCoord rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
    // convert to planar x, y across whole layer
    //ru_pos.x = ru_pos.x + (recv_lay->un_geom.x * rgp_pos.x);
    ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
    ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;

    if(grad_x && grad_y) { // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    // is recv unit out-of-range?  -- if so, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x || ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x;		  // start at start-indexed units!
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }

  // now normalize send groups and correct for any offsets!
  //float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1); // original guys..
  //float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int sgpidx = su->UnitGpIdx();
    TwoDCoord sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // position relative to overall gp geom
    // send un_gp in range?  -- get next un_gp if not
    if(sgp_pos.x < srs.x || sgp_pos.x > sre.x) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
      continue;
    }
    if(sgp_pos.y < srs.y || sgp_pos.y > sre.y) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
      continue;
    }
    // if in-range, normalize send range (for topological congruence to recv range)
    sgp_pos.x -= srs.x; // start at start indexed units!
    sgp_pos.y -= srs.y;

    float sgp_x = (float)sgp_pos.x / mxs_x;
    float sgp_y = (float)sgp_pos.y / mxs_y;

    float wrp_x, wrp_y; // TODO: confirm sometime that this wrap stuff is still good with recv gps and/or send gps?
    if(wrap) {
      if(ru_x > .5f)    wrp_x = (float)(sgp_pos.x + send_lay->gp_geom.x) / mxs_x;
      else              wrp_x = (float)(sgp_pos.x - send_lay->gp_geom.x) / mxs_x;
      if(ru_y > .5f)    wrp_y = (float)(sgp_pos.y + send_lay->gp_geom.y) / mxs_y;
      else              wrp_y = (float)(sgp_pos.y - send_lay->gp_geom.y) / mxs_y;
    }
    float dist = 0.0f;
    if(grad_x && grad_y) {
      if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	dist = taMath_float::euc_dist(sgp_x, sgp_y, ru_x, ru_y);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, ru_x, ru_y);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, ru_x, ru_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
      else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	dist = taMath_float::euc_dist(sgp_x, sgp_y, ru_y, ru_x);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, ru_y, ru_x);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_y, ru_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, ru_y, ru_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
    }
    else if(grad_x) { // two cases: x to x and send_x to recv_y
      if(!grad_x_grad_y) { // i.e., the usual x to x case
	dist = fabsf(sgp_x - ru_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - ru_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_x to recv_y case
	dist = fabsf(sgp_x - ru_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - ru_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }
    else if(grad_y) { // again two cases: y to y and send_y to recv_x
      if(!grad_y_grad_x) { // i.e. the usual y to y case
	dist = fabsf(sgp_y - ru_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - ru_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_y to recv_x case
	dist = fabsf(sgp_y - ru_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - ru_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i); // i is still the simple su -- one of entries in the cg (congroup) list
  }
}

void TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  TwoDCoord srs = send_range_start; // actual send range start
  TwoDCoord sre = send_range_end;   // actual send range end
  TwoDCoord rrs = recv_range_start; // actual recv range start
  TwoDCoord rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->flat_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->flat_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->gp_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->gp_geom.y-1;

  if(TestWarning((sre.x > send_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "recv_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    rre.x = recv_lay->gp_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "recv_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    rre.y = recv_lay->gp_geom.y-1;
  }

  // TODO: should have tests for start > end at this point, but not very likely

  int rgpidx = ru->UnitGpIdx();
  TwoDCoord rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom

  //  // recv un_gp in range?  -- get next un_gp if not
  //  if(rgp_pos.x < rrs.x || rgp_pos.x > rre.x)
  //    return;
  //  if(rgp_pos.y < rrs.y || rgp_pos.y > rre.y)
  //    return;

  float max_dist = 1.0f;
  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
    max_dist = sqrtf(2.0f);
  float dist = 0.0f;

  // is recv un_gp in range?  -- if not, init wts and get next cg/ru
  if(rgp_pos.x < rrs.x || rgp_pos.x > rre.x || rgp_pos.y < rrs.y || rgp_pos.y > rre.y) {
    for(int i=0; i<cg->size; i++) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!) -- 1.0 = max dist
    }
    return; // done -- get a new cg/ru
  }

  // if in-range, normalize recv range (for topological congruence to send range)
  rgp_pos.x -= rrs.x;
  rgp_pos.y -= rrs.y;

  //float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom.x-1, 1);
  //float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom.y-1, 1);
  float rgp_x = (float)rgp_pos.x / (float)MAX((rre.x - rrs.x), 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX((rre.y - rrs.y), 1);

  //  float max_dist = 1.0f;
  //  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
  //    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  if(!send_lay->unit_groups) { // sending layer does NOT have unit groups anyway -- nothing to worry about!
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      TwoDCoord su_pos;
      send_lay->UnitLogPos(su, su_pos);
      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // SPECIAL CASE: only one send unit in range so everyone gets max wt!
      if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    SetWtFmDist(prjn, cg, ru, 0.0f, i); // 0.0 = min dist -> max wts
    continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_x, rgp_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_x, rgp_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_y, rgp_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_y, rgp_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
  else { // send_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      int sunidx = 0;
      int sgpidx = 0;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp) su_pos.x = sunidx % send_lay->un_geom.x;

      TwoDCoord su_pos;
      su_pos.x = sunidx % send_lay->un_geom.x;
      su_pos.y = sunidx / send_lay->un_geom.x;
      TwoDCoord sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom

      // convert to planar x, y across whole layer
      //su_pos.x = su_pos.x + (send_lay->un_geom.x * sgp_pos.x);
      su_pos.x += send_lay->un_geom.x * sgp_pos.x;
      su_pos.y += send_lay->un_geom.y * sgp_pos.y;

      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // SPECIAL CASE: only one send unit in range so everyone gets max wt!
      if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    SetWtFmDist(prjn, cg, ru, 0.0f, i); // 0.0 = min dist -> max wts
    continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_x, rgp_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_x, rgp_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_y, rgp_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_y, rgp_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
}

void TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru) {

  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  TwoDCoord srs = send_range_start; // actual send range start
  TwoDCoord sre = send_range_end;   // actual send range end
  TwoDCoord rrs = recv_range_start; // actual recv range start
  TwoDCoord rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->flat_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->flat_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->flat_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->flat_geom.y-1;

  if(TestWarning((sre.x > send_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    rre.y = recv_lay->flat_geom.y-1;
  }
  // else should be the values set by user

  float ru_x = 0.0f;
  float ru_y = 0.0f;
  float max_dist = 1.0f;
  float dist = 0.0f;
  if (!recv_lay->unit_groups) { // no unit groups to worry about!
    TwoDCoord ru_pos;
    recv_lay->UnitLogPos(ru, ru_pos);
    if(grad_x && grad_y) {// applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    float dist = 0.0f;
    // is recv unit in range?  -- if not, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x ||
       ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x; 	// start at start-indexed units!
    //ru_pos.y = ru_pos.y - rrs.y;
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    //float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }
  else { // recv_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    TwoDCoord ru_pos; // hold planar Cartesian coordinates within unit group
    int runidx = 0;
    int rgpidx = 0;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
    ru_pos.x = runidx % recv_lay->un_geom.x;
    ru_pos.y = runidx / recv_lay->un_geom.x;
    TwoDCoord rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
    // convert to planar x, y across whole layer
    //ru_pos.x = ru_pos.x + (recv_lay->un_geom.x * rgp_pos.x);
    ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
    ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;

    if(grad_x && grad_y) { // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    // is recv unit out-of-range?  -- if so, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x ||
       ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x;		  // start at start-indexed units!
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }

  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  if(!send_lay->unit_groups) { // send layer does NOT have unit groups -- nothing to worry about!
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      TwoDCoord su_pos;
      send_lay->UnitLogPos(su, su_pos);
      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // SPECIAL CASE: only one send unit in range so everyone gets max wt!
      if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    SetWtFmDist(prjn, cg, ru, 0.0f, i); // 0.0 = min dist -> max wts
    continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(ru_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(ru_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, ru_x, ru_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_x, ru_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, ru_y, ru_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_y, ru_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
  else { // send_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      int sunidx = 0;
      int sgpidx = 0;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp) su_pos.x = sunidx % send_lay->un_geom.x;

      TwoDCoord su_pos;
      su_pos.x = sunidx % send_lay->un_geom.x;
      su_pos.y = sunidx / send_lay->un_geom.x;
      TwoDCoord sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom

      // convert to planar x, y across whole layer
      //su_pos.x = su_pos.x + (send_lay->un_geom.x * sgp_pos.x);
      su_pos.x += send_lay->un_geom.x * sgp_pos.x;
      su_pos.y += send_lay->un_geom.y * sgp_pos.y;

      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(ru_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(ru_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, ru_x, ru_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_x, ru_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, ru_y, ru_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_y, ru_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
}

//////////////////////////////////
//              Wizard          //
//////////////////////////////////

///////////////////////////////////////////////////////////////
//                      PBWM
///////////////////////////////////////////////////////////////

static void lay_set_geom(LeabraLayer* lay, int n_stripes, int n_units = -1, bool sp = true,
                         int gp_geom_x=-1, int gp_geom_y=-1) {
  lay->unit_groups = true;
  lay->SetNUnitGroups(n_stripes);
  if(gp_geom_x > 0 && gp_geom_y > 0) {
    lay->gp_geom.x = gp_geom_x;
    lay->gp_geom.y = gp_geom_y;
  }
  else {
    if(n_stripes <= 5 || n_stripes == 7 || n_stripes == 11 || n_stripes >= 13) {
      lay->gp_geom.x = n_stripes;
      lay->gp_geom.y = 1;
    }
    else{
      if(n_stripes == 6 || n_stripes == 8 || n_stripes == 10) {
        lay->gp_geom.x = n_stripes / 2;
        lay->gp_geom.y = 2;
      }
      else {
        if(n_stripes == 9 || n_stripes == 12) {
          lay->gp_geom.x = n_stripes / 3;
          lay->gp_geom.y = 3;
        }
      }
    }
  } // default whatever SetNUnitGroups() set..
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

bool LeabraWizard::PBWM_SetNStripes(LeabraNetwork* net, int in_stripes, int mnt_stripes,
				    int out_stripes, int n_matrix_units,
				    int n_pfc_units) {
  if(TestError(!net, "PBWM_SetNStripes", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  // this is called as a subroutine a lot too so don't save here -- could do impl but
  // not really worth it..
//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_SetNStripes -- actually saves network specifically");
//   }

  set_n_stripes(net, "PFCs_in",  in_stripes, n_pfc_units, true);
  set_n_stripes(net, "PFCd_in",  in_stripes, n_pfc_units, true);
  set_n_stripes(net, "PFCs_mnt", mnt_stripes, n_pfc_units, true);
  set_n_stripes(net, "PFCd_mnt", mnt_stripes, n_pfc_units, true);
  set_n_stripes(net, "PFCs_out", out_stripes, n_pfc_units, true);
  set_n_stripes(net, "PFCd_out", out_stripes, n_pfc_units, true);

  set_n_stripes(net, "Matrix_Go_in",   in_stripes, n_matrix_units, true);
  set_n_stripes(net, "Matrix_NoGo_in", in_stripes, n_matrix_units, true);
  set_n_stripes(net, "Matrix_Go_mnt",  mnt_stripes, n_matrix_units, true);
  set_n_stripes(net, "Matrix_NoGo_mnt",mnt_stripes, n_matrix_units, true);
  set_n_stripes(net, "Matrix_Go_out",  out_stripes, n_matrix_units, true);
  set_n_stripes(net, "Matrix_NoGo_out",out_stripes, n_matrix_units, true);

  int snr_stripes = in_stripes + mnt_stripes + out_stripes;
  set_n_stripes(net, "SNrThal", snr_stripes, 1, false, snr_stripes, 1); // default is n x 1

  set_n_stripes(net, "LVe", 1, -1, false, 1, 1);
  set_n_stripes(net, "LVi", 1, -1, false, 1, 1);
  net->Build();
  return true;
}

bool LeabraWizard::PBWM(LeabraNetwork* net, int in_stripes, int mnt_stripes,
			int out_stripes, bool da_mod_all, bool pfc_learns) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PBWM", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  // first configure PVLV system..
  if(TestError(!PVLV(net, da_mod_all), "PBWM", "could not make PVLV")) return false;


  String msg = "Configuring PBWM (Prefrontal-cortex Basal-ganglia Working Memory) Layers:\n\n\
 There is one thing you will need to check manually after this automatic configuration\
 process completes (this note will be repeated when things complete --- there may be some\
 messages in the interim):\n\n";

  String man_msg = "1. Check the bidirectional connections between the PFC and all appropriate layers.\
 Except for special intra-PFC connections, the conspecs INTO ALL superficial PFC layers should be ToPFC conspecs; those out from PFC layers should be regular learning conspecs.";

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

  bool new_pbwm_laygp = false;
  Layer_Group* pbwm_laygp_go = NULL;
  Layer_Group* pbwm_laygp_nogo = NULL;
  Layer_Group* pbwm_laygp_pfc = NULL;

  pbwm_laygp_go = net->FindMakeLayerGroup("PBWM_Go", NULL, new_pbwm_laygp);
  pbwm_laygp_nogo = net->FindMakeLayerGroup("PBWM_NoGo", NULL, new_pbwm_laygp);
  pbwm_laygp_pfc = net->FindMakeLayerGroup("PBWM_PFC", NULL, new_pbwm_laygp);

  // new gets full update, otherwise more just params
  bool matrix_new =   false; 
  bool pfc_new = false; 
  bool snrthal_new =     false; 

  LeabraLayer* pfc_s_mnt = NULL;
  LeabraLayer* pfc_d_mnt = NULL;
  LeabraLayer* pfc_s_out = NULL;
  LeabraLayer* pfc_d_out = NULL;
  LeabraLayer* pfc_s_in = NULL;
  LeabraLayer* pfc_d_in = NULL;

  LeabraLayer* matrix_go_in = NULL;
  LeabraLayer* matrix_go_mnt = NULL;
  LeabraLayer* matrix_go_out = NULL;
  LeabraLayer* matrix_nogo_in = NULL;
  LeabraLayer* matrix_nogo_mnt = NULL;
  LeabraLayer* matrix_nogo_out = NULL;
  LeabraLayer* snrthal = NULL;

  if(in_stripes > 0) {
    matrix_go_in = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("Matrix_Go_in", NULL,
							      matrix_new);
    matrix_nogo_in = (LeabraLayer*)pbwm_laygp_nogo->FindMakeLayer("Matrix_NoGo_in", NULL,
								  matrix_new);
    pfc_s_in =  (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCs_in",  NULL, pfc_new);
    pfc_d_in =  (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCd_in",  NULL, pfc_new);
  }

  if(mnt_stripes > 0) {
    matrix_go_mnt = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("Matrix_Go_mnt", NULL,
							      matrix_new, "Matrix");
    matrix_nogo_mnt = (LeabraLayer*)pbwm_laygp_nogo->FindMakeLayer("Matrix_NoGo_mnt", NULL,
								  matrix_new);
    pfc_s_mnt = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCs_mnt", NULL, pfc_new);
    pfc_d_mnt = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCd_mnt", NULL, pfc_new);
  }

  if(out_stripes > 0) {
    matrix_go_out = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("Matrix_Go_out", NULL,
							      matrix_new, "Matrix");
    matrix_nogo_out = (LeabraLayer*)pbwm_laygp_nogo->FindMakeLayer("Matrix_NoGo_out", NULL,
								  matrix_new);
    pfc_s_out = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCs_out", NULL, pfc_new);
    pfc_d_out = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCd_out", NULL, pfc_new);
  }

  // stick this in go..
  snrthal = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("SNrThal", NULL, snrthal_new);

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  int mx_z1 = 0;         // max x coordinate on layer z=1
  int mx_z2 = 0;         // z=2
  Layer_Group other_lays;   Layer_Group hidden_lays;
  Layer_Group output_lays;  Layer_Group input_lays;
  TDCoord lpos;
  int i;
  for(i=0;i<net->layers.leaves;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
    if(lay != rew_targ_lay && lay != pve && lay != pvr && lay != pvi
       && lay != lve && lay != lvi && lay != nv && lay != vta
       && lay != snrthal
       && lay != matrix_go_in && lay != matrix_go_mnt && lay != matrix_go_out
       && lay != matrix_nogo_in && lay != matrix_nogo_mnt && lay != matrix_nogo_out
       && lay != pfc_s_mnt && lay != pfc_d_mnt
       && lay != pfc_s_out && lay != pfc_d_out
       && lay != pfc_s_in  && lay != pfc_d_in) {
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
  // NOTE: make specs for all the pfc stripe roles: no harm if not used and could be helpful to user!

  BaseSpec_Group* units = net->FindMakeSpecGp("PFC_BG_Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp("PFC_BG_Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp("PFC_BG_Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp("PFC_BG_Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return false;

  ////////////	UnitSpecs

  LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* lv_units = (LeabraUnitSpec*)pv_units->FindMakeChild("LVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_LeabraUnitSpec);

  LeabraUnitSpec* pfc_units = (LeabraUnitSpec*)units->FindMakeSpec("PFCUnits", &TA_PFCsUnitSpec);
  LayerActUnitSpec* pfcd_units = (LayerActUnitSpec*)pfc_units->FindMakeChild("PFCdUnits", &TA_LayerActUnitSpec);
  LeabraUnitSpec* matrix_units = (LeabraUnitSpec*)units->FindMakeSpec("MatrixUnits", &TA_MatrixUnitSpec);
  LeabraUnitSpec* matrix_nogo_units = (LeabraUnitSpec*)matrix_units->FindMakeChild("MatrixNoGo", &TA_MatrixUnitSpec);
  LeabraUnitSpec* snrthal_units = (LeabraUnitSpec*)units->FindMakeSpec("SNrThalUnits", &TA_LeabraUnitSpec);

  ////////////	ConSpecs

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  LeabraConSpec* pvi_cons = (LeabraConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  LeabraConSpec* pvr_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("PVr", &TA_PVrConSpec);
  LeabraConSpec* lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_PVConSpec);
  LeabraConSpec* lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_PVConSpec);
  LeabraConSpec* nv_cons =  (LeabraConSpec*)pvi_cons->FindMakeChild("NV", &TA_PVConSpec);

  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);

  LeabraConSpec* topfc_cons = (LeabraConSpec*)learn_cons->FindMakeChild("ToPFC", &TA_LeabraConSpec);
  LeabraConSpec* pfc_bias = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCBias", &TA_LeabraBiasSpec);
  LeabraConSpec* pfctopfc_s_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCtoPFC_s", &TA_LeabraConSpec);
  LeabraConSpec* pfctopfc_d_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCtoPFC_d", &TA_LeabraConSpec);
  LeabraConSpec* topfcfmin_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCFmInput", &TA_LeabraConSpec);
  LeabraConSpec* topfcfmout_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCFmOutput", &TA_LeabraConSpec);

  MatrixConSpec* matrix_cons = (MatrixConSpec*)learn_cons->FindMakeChild("MatrixCons", &TA_MatrixConSpec);
  MatrixBiasSpec* matrix_bias = (MatrixBiasSpec*)matrix_cons->FindMakeChild("MatrixBias", &TA_MatrixBiasSpec);
  MatrixConSpec* matrix_cons_topo = (MatrixConSpec*)matrix_cons->FindMakeChild("MatrixConsTopo", &TA_MatrixConSpec);
  MatrixConSpec* matrix_cons_topo_weak = (MatrixConSpec*)matrix_cons_topo->FindMakeChild("MatrixConsTopoWeak", &TA_MatrixConSpec);
  MatrixConSpec* matrix_cons_topo_strong = (MatrixConSpec*)matrix_cons_topo->FindMakeChild("MatrixConsTopoStrong", &TA_MatrixConSpec);
  MatrixNoGoConSpec* matrix_cons_nogo = (MatrixNoGoConSpec*)matrix_cons->FindMakeChild("MatrixConsNoGo", &TA_MatrixNoGoConSpec);
  MatrixConSpec* matrix_cons_fmpvr = (MatrixConSpec*)matrix_cons->FindMakeChild("MatrixFmPvr", &TA_MatrixConSpec);

  LeabraConSpec* fmpfcd_out = (LeabraConSpec*)learn_cons->FindMakeChild("FmPFCd_out", &TA_LeabraConSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  LeabraConSpec* matrix_to_snrthal = (LeabraConSpec*)cons->FindMakeSpec("MatrixToSNrThal", &TA_LeabraConSpec);
  // matrix nogo to snrthal just a marker con

  ////////////	LayerSpecs

  LeabraLayerSpec* rewtargsp = (LeabraLayerSpec*)layers->FindMakeSpec("RewTargLayer", &TA_LeabraLayerSpec);
  ExtRewLayerSpec* pvesp = (ExtRewLayerSpec*)layers->FindMakeSpec(pvenm + "Layer", &TA_ExtRewLayerSpec);
  PVrLayerSpec* pvrsp = (PVrLayerSpec*)layers->FindMakeSpec(pvrnm + "Layer", &TA_PVrLayerSpec);
  PViLayerSpec* pvisp = (PViLayerSpec*)layers->FindMakeSpec(pvinm + "Layer", &TA_PViLayerSpec);
  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec);
  LViLayerSpec* lvisp = (LViLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec);
  NVLayerSpec* nvsp = (NVLayerSpec*)layers->FindMakeSpec(nvnm + "Layer", &TA_NVLayerSpec);

  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindType(&TA_PVLVDaLayerSpec);

  PFCLayerSpec* pfc_s_mnt_sp = (PFCLayerSpec*)layers->FindMakeSpec("PFCSuper_mnt", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_s_in_sp = (PFCLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_in", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_s_out_sp = (PFCLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_out", &TA_PFCLayerSpec);

  PFCLayerSpec* pfc_d_mnt_sp = (PFCLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCDeep_mnt", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_d_in_sp = (PFCLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_in", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_d_out_sp = (PFCLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_out", &TA_PFCLayerSpec);

  MatrixLayerSpec* matrix_go_mnt_sp = (MatrixLayerSpec*)layers->FindMakeSpec("Matrix_Go_mnt", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_go_in_sp = (MatrixLayerSpec*)matrix_go_mnt_sp->FindMakeChild("Matrix_Go_in", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_go_out_sp = (MatrixLayerSpec*)matrix_go_mnt_sp->FindMakeChild("Matrix_Go_out", &TA_MatrixLayerSpec);

  MatrixLayerSpec* matrix_nogo_mnt_sp = (MatrixLayerSpec*)matrix_go_mnt_sp->FindMakeChild("Matrix_NoGo_mnt", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_nogo_in_sp = (MatrixLayerSpec*)matrix_nogo_mnt_sp->FindMakeChild("Matrix_NoGo_in", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_nogo_out_sp = (MatrixLayerSpec*)matrix_nogo_mnt_sp->FindMakeChild("Matrix_NoGo_out", &TA_MatrixLayerSpec);

  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_SNrThalLayerSpec);

  ////////////	PrjnSpecs

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);

  SNrPrjnSpec* snr_prjn = (SNrPrjnSpec*)prjns->FindMakeSpec("SNrPrjn", &TA_SNrPrjnSpec);
  PVrToMatrixGoPrjnSpec* pvr_to_mtx_prjn = (PVrToMatrixGoPrjnSpec*)prjns->FindMakeSpec("PVrToMatrixGoPrjn", &TA_PVrToMatrixGoPrjnSpec);

  TopoWtsPrjnSpec* topomaster = (TopoWtsPrjnSpec*)prjns->FindMakeSpec("TopoMaster", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topofminput = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoFmInput", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* intrapfctopo = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoIntraPFC", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixpfc_self = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixPFC_Self", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixpfc_other = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixPFC_Other", &TA_TopoWtsPrjnSpec);
  TesselPrjnSpec* input_pfc = (TesselPrjnSpec*)prjns->FindMakeSpec("Input_PFC", &TA_TesselPrjnSpec);
  input_pfc->send_offs.SetSize(1); // this is all it takes!


  ////	Config Topo PrjnSpecs

  topomaster->wt_range.min = 0.1f;
  topomaster->wt_range.max = 0.5f;
  topomaster->grad_type = TopoWtsPrjnSpec::GAUSSIAN;
  topomaster->gauss_sig = 0.1f;
  topomaster->use_recv_gps = true;
  topomaster->use_send_gps = true;

  topofminput->SetUnique("grad_x", true);
  topofminput->grad_x = false;
  topofminput->SetUnique("grad_y", true);
  topofminput->grad_y = true;
  topofminput->SetUnique("grad_y_grad_x", true);
  topofminput->grad_y_grad_x = true;
  topofminput->SetUnique("wrap", true);
  topofminput->wrap = false;
  topofminput->SetUnique("use_recv_gps", true);
  topofminput->use_recv_gps = true;
  topofminput->SetUnique("use_send_gps", true);
  topofminput->use_send_gps = false;
  topofminput->SetUnique("custom_send_range", true);
  topofminput->custom_send_range = true;
  topofminput->SetUnique("send_range_start", true);
  topofminput->send_range_start.x = 0;
  topofminput->send_range_start.y = 0;
  topofminput->SetUnique("send_range_end", true);
  topofminput->send_range_end.x = -1;
  // topofminput->send_range_end.y = (n_stripes / 2)-1; // unlikely to be generally useful..
  topofminput->SetUnique("custom_recv_range", true);
  topofminput->custom_recv_range = false;

  topomatrixpfc_self->SetUnique("grad_x", true);
  topomatrixpfc_self->grad_x = true;
  topomatrixpfc_self->SetUnique("grad_x_grad_y", true);
  topomatrixpfc_self->grad_x_grad_y = false;
  topomatrixpfc_self->SetUnique("grad_y", true);
  topomatrixpfc_self->grad_y = true; // this is key diff for self vs. other
  topomatrixpfc_self->SetUnique("grad_y_grad_x", true);
  topomatrixpfc_self->grad_y_grad_x = false;
  topomatrixpfc_self->SetUnique("wrap", true);
  topomatrixpfc_self->wrap = true;
  topomatrixpfc_self->SetUnique("use_recv_gps", true);
  topomatrixpfc_self->use_recv_gps = true;
  topomatrixpfc_self->SetUnique("use_send_gps", true);
  topomatrixpfc_self->use_send_gps = true;
  topomatrixpfc_self->SetUnique("custom_send_range", true);
  topomatrixpfc_self->custom_send_range = false;
  topomatrixpfc_self->SetUnique("custom_recv_range", true);
  topomatrixpfc_self->custom_recv_range = false;

  topomatrixpfc_other->SetUnique("grad_x", true);
  topomatrixpfc_other->grad_x = true;
  topomatrixpfc_other->SetUnique("grad_x_grad_y", true);
  topomatrixpfc_other->grad_x_grad_y = false;
  topomatrixpfc_other->SetUnique("grad_y", true);
  topomatrixpfc_other->grad_y = false;
  topomatrixpfc_other->SetUnique("grad_y_grad_x", true);
  topomatrixpfc_other->grad_y_grad_x = false;
  topomatrixpfc_other->SetUnique("wrap", true);
  topomatrixpfc_other->wrap = true;
  topomatrixpfc_other->SetUnique("use_recv_gps", true);
  topomatrixpfc_other->use_recv_gps = true;
  topomatrixpfc_other->SetUnique("use_send_gps", true);
  topomatrixpfc_other->use_send_gps = true;
  topomatrixpfc_other->SetUnique("custom_send_range", true);
  topomatrixpfc_other->custom_send_range = false;
  topomatrixpfc_other->SetUnique("custom_recv_range", true);
  topomatrixpfc_other->custom_recv_range = false;

  intrapfctopo->SetUnique("grad_x", true);
  intrapfctopo->grad_x = true;
  intrapfctopo->SetUnique("grad_x_grad_y", true);
  intrapfctopo->grad_x_grad_y = false;
  intrapfctopo->SetUnique("grad_y", true);
  intrapfctopo->grad_y = false;
  intrapfctopo->SetUnique("grad_y_grad_x", true);
  intrapfctopo->grad_y_grad_x = false;
  intrapfctopo->SetUnique("wrap", true);
  intrapfctopo->wrap = true;
  intrapfctopo->SetUnique("use_recv_gps", true);
  intrapfctopo->use_recv_gps = true;
  intrapfctopo->SetUnique("use_send_gps", true);
  intrapfctopo->use_send_gps = true;
  intrapfctopo->SetUnique("custom_send_range", true);
  intrapfctopo->custom_send_range = false;
  intrapfctopo->SetUnique("custom_recv_range", true);
  intrapfctopo->custom_recv_range = false;

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  // set bias specs for unit specs
  pfc_units->bias_spec.SetSpec(pfc_bias);
  pfcd_units->bias_spec.SetSpec(pfc_bias);
  matrix_units->bias_spec.SetSpec(matrix_bias);
  matrix_nogo_units->bias_spec.SetSpec(matrix_bias);
  snrthal_units->bias_spec.SetSpec(bg_bias);

  snrthal->SetLayerSpec(snrthalsp); snrthal->SetUnitSpec(snrthal_units);

  if(in_stripes > 0) {
    matrix_go_in->SetLayerSpec(matrix_go_in_sp);
    matrix_go_in->SetUnitSpec(matrix_units);
    matrix_nogo_in->SetLayerSpec(matrix_nogo_in_sp); 
    matrix_nogo_in->SetUnitSpec(matrix_nogo_units);

    pfc_s_in->SetLayerSpec(pfc_s_in_sp);  pfc_s_in->SetUnitSpec(pfc_units);
    pfc_d_in->SetLayerSpec(pfc_d_in_sp);  pfc_d_in->SetUnitSpec(pfcd_units);
  }
  if(mnt_stripes > 0) {
    matrix_go_mnt->SetLayerSpec(matrix_go_mnt_sp);
    matrix_go_mnt->SetUnitSpec(matrix_units);
    matrix_nogo_mnt->SetLayerSpec(matrix_nogo_mnt_sp); 
    matrix_nogo_mnt->SetUnitSpec(matrix_nogo_units);

    pfc_s_mnt->SetLayerSpec(pfc_s_mnt_sp);  pfc_s_mnt->SetUnitSpec(pfc_units);
    pfc_d_mnt->SetLayerSpec(pfc_d_mnt_sp);  pfc_d_mnt->SetUnitSpec(pfcd_units);
  }
  if(out_stripes > 0) {
    matrix_go_out->SetLayerSpec(matrix_go_out_sp);
    matrix_go_out->SetUnitSpec(matrix_units);
    matrix_nogo_out->SetLayerSpec(matrix_nogo_out_sp); 
    matrix_nogo_out->SetUnitSpec(matrix_nogo_units);

    pfc_s_out->SetLayerSpec(pfc_s_out_sp);  pfc_s_out->SetUnitSpec(pfc_units);
    pfc_d_out->SetLayerSpec(pfc_d_out_sp);  pfc_d_out->SetUnitSpec(pfcd_units);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  //	  	 	   to		 from		prjn_spec	con_spec

  // matrix <-> snrthal
  if(in_stripes > 0) {
    net->FindMakePrjn(snrthal, matrix_go_in, snr_prjn, matrix_to_snrthal);
    net->FindMakePrjn(matrix_go_in, matrix_nogo_in, gponetoone, marker_cons);
    net->FindMakePrjn(matrix_go_in, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_go_in, vta, fullprjn, marker_cons);
    // net->FindMakePrjn(matrix_go_in, pvr, pvr_to_mtx_prjn, matrix_cons_fmpvr);

    net->FindMakePrjn(matrix_nogo_in, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_in, vta, fullprjn, marker_cons);
  }
  if(mnt_stripes > 0) {
    net->FindMakePrjn(snrthal, matrix_go_mnt, snr_prjn, matrix_to_snrthal);
    net->FindMakePrjn(matrix_go_mnt, matrix_nogo_mnt, gponetoone, marker_cons);
    net->FindMakePrjn(matrix_go_mnt, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_go_mnt, vta, fullprjn, marker_cons);
    // net->FindMakePrjn(matrix_go_mnt, pvr, pvr_to_mtx_prjn, matrix_cons_fmpvr);

    net->FindMakePrjn(matrix_nogo_mnt, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_mnt, vta, fullprjn, marker_cons);
  }
  if(out_stripes > 0) {
    net->FindMakePrjn(snrthal, matrix_go_out, snr_prjn, matrix_to_snrthal);
    net->FindMakePrjn(matrix_go_out, matrix_nogo_out, gponetoone, marker_cons);
    net->FindMakePrjn(matrix_go_out, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_go_out, vta, fullprjn, marker_cons);
    // net->FindMakePrjn(matrix_go_out, pvr, pvr_to_mtx_prjn, matrix_cons_fmpvr);

    net->FindMakePrjn(matrix_nogo_out, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_out, vta, fullprjn, marker_cons);
  }

  // matrix <-> pfc and pfc <-> pfc

  if(in_stripes > 0) {
    net->FindMakePrjn(matrix_go_in, pfc_s_in, topomatrixpfc_self, matrix_cons_topo);
    net->FindMakePrjn(matrix_nogo_in, pfc_d_in, topomatrixpfc_self, matrix_cons_nogo);

    if(mnt_stripes > 0) {
      net->FindMakePrjn(pfc_s_in, pfc_s_mnt, intrapfctopo, pfctopfc_s_cons);
      net->FindMakePrjn(pfc_s_in, pfc_d_mnt, intrapfctopo, pfctopfc_d_cons);

      net->FindMakePrjn(matrix_go_in, pfc_s_mnt, topomatrixpfc_other, matrix_cons_topo);
      net->FindMakePrjn(matrix_nogo_in, pfc_d_mnt, topomatrixpfc_other, matrix_cons_nogo);
    }
    if(out_stripes > 0) {
      net->FindMakePrjn(pfc_s_in, pfc_s_out, intrapfctopo, pfctopfc_s_cons);
      net->FindMakePrjn(pfc_s_in, pfc_d_out, intrapfctopo, pfctopfc_d_cons);

      net->FindMakePrjn(matrix_go_in, pfc_s_out, topomatrixpfc_other, matrix_cons_topo);
      net->FindMakePrjn(matrix_nogo_in, pfc_d_out, topomatrixpfc_other, matrix_cons_nogo);
    }
    net->FindMakePrjn(pfc_d_in, pfc_s_in, onetoone, marker_cons);
    net->FindMakePrjn(pfc_d_in, snrthal, snr_prjn, marker_cons);
  }

  if(mnt_stripes > 0) {
    net->FindMakePrjn(matrix_go_mnt, pfc_s_mnt, topomatrixpfc_self, matrix_cons_topo_weak);
    net->FindMakePrjn(matrix_nogo_mnt, pfc_d_mnt, topomatrixpfc_self, matrix_cons_nogo);

    if(in_stripes > 0) {
      net->FindMakePrjn(pfc_s_mnt, pfc_s_in, intrapfctopo, pfctopfc_s_cons);
      net->FindMakePrjn(pfc_s_mnt, pfc_d_in, intrapfctopo, pfctopfc_d_cons);

      net->FindMakePrjn(matrix_go_mnt, pfc_s_in, topomatrixpfc_other, matrix_cons_topo);
      net->FindMakePrjn(matrix_nogo_mnt, pfc_d_in, topomatrixpfc_other, matrix_cons_nogo);
    }
    if(out_stripes > 0) {
      net->FindMakePrjn(pfc_s_mnt, pfc_s_out, intrapfctopo, pfctopfc_s_cons);
      net->FindMakePrjn(pfc_s_mnt, pfc_d_out, intrapfctopo, pfctopfc_d_cons);

      net->FindMakePrjn(matrix_go_mnt, pfc_s_out, topomatrixpfc_other, matrix_cons_topo);
      net->FindMakePrjn(matrix_nogo_mnt, pfc_d_out, topomatrixpfc_other, matrix_cons_nogo);
    }
    net->FindMakePrjn(pfc_d_mnt, pfc_s_mnt, onetoone, marker_cons);
    net->FindMakePrjn(pfc_d_mnt, snrthal, snr_prjn, marker_cons);
  }

  if(out_stripes > 0) {
    net->FindMakePrjn(matrix_go_out, pfc_s_out, topomatrixpfc_self, matrix_cons_topo);
    net->FindMakePrjn(matrix_nogo_out, pfc_d_out, topomatrixpfc_self, matrix_cons_nogo);

    if(in_stripes > 0) {
      net->FindMakePrjn(pfc_s_out, pfc_s_in, intrapfctopo, pfctopfc_s_cons);
      net->FindMakePrjn(pfc_s_out, pfc_d_in, intrapfctopo, pfctopfc_d_cons);

      net->FindMakePrjn(matrix_go_out, pfc_s_in, topomatrixpfc_other, matrix_cons_topo);
      net->FindMakePrjn(matrix_nogo_out, pfc_d_in, topomatrixpfc_other, matrix_cons_nogo);
    }
    if(mnt_stripes > 0) {
      net->FindMakePrjn(pfc_s_out, pfc_s_mnt, intrapfctopo, pfctopfc_s_cons);
      net->FindMakePrjn(pfc_s_out, pfc_d_mnt, intrapfctopo, pfctopfc_d_cons);

      net->FindMakePrjn(matrix_go_out, pfc_s_mnt, topomatrixpfc_other, matrix_cons_topo);
      net->FindMakePrjn(matrix_nogo_out, pfc_d_mnt, topomatrixpfc_other, matrix_cons_nogo);
    }
    net->FindMakePrjn(pfc_d_out, pfc_s_out, onetoone, marker_cons);
    net->FindMakePrjn(pfc_d_out, snrthal, snr_prjn, marker_cons);
  }

  // pvlv recv from deep only, input and maint
  if(in_stripes > 0) {
    // net->FindMakePrjn(pvr, pfc_d_mnt, fullprjn, pvr_cons);
    net->FindMakePrjn(pvi, pfc_d_in, fullprjn, pvi_cons);
    net->FindMakePrjn(lve, pfc_d_in, fullprjn, lve_cons);
    net->FindMakePrjn(lvi, pfc_d_in, fullprjn, lvi_cons);
    net->FindMakePrjn(nv,  pfc_d_in, fullprjn, nv_cons);
  }
  if(mnt_stripes > 0) {
    // net->FindMakePrjn(pvr, pfc_d_mnt, fullprjn, pvr_cons);
    net->FindMakePrjn(pvi, pfc_d_mnt, fullprjn, pvi_cons);
    net->FindMakePrjn(lve, pfc_d_mnt, fullprjn, lve_cons);
    net->FindMakePrjn(lvi, pfc_d_mnt, fullprjn, lvi_cons);
    net->FindMakePrjn(nv,  pfc_d_mnt, fullprjn, nv_cons);
  }

  // TODO: error message not working; revisit at some point..
  /*else {
	if(TestWarning((!(mnt_stripes > 0)),
	  "Since there are no MAINT layers there's no PFC projection to PVLV. ",
	  "You may wish to connect from PFCd_in using the PVLVConnect Wizard. ")) {
	  // nothing to do, but might want to just go ahead and make that connection
      // in the future
	 }
  }*/

  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];

    if(matrix_new) {  // posterior cortex presumably also projects from superficial..
      if(in_stripes > 0) {
	net->FindMakePrjn(matrix_go_in, il, topofminput, matrix_cons_topo);
      }
      else if(mnt_stripes > 0) { // only maint if no input
	net->FindMakePrjn(matrix_go_mnt, il, topofminput, matrix_cons_topo);
      }
      // net->FindMakePrjn(matrix_nogo, il, topofminput, matrix_cons_topo);
    }

    if(pfc_new) {
      if(in_stripes > 0) {
        if(pfc_learns)
          net->FindMakePrjn(pfc_s_in, il, topofminput, topfcfmin_cons);
        else
          net->FindMakePrjn(pfc_s_in, il, input_pfc, topfc_cons);
      }
      else if(mnt_stripes > 0) { // only maint if no input
        if(pfc_learns)
          net->FindMakePrjn(pfc_s_mnt, il, topofminput, topfcfmin_cons);
        else
          net->FindMakePrjn(pfc_s_mnt, il, input_pfc, topfc_cons);
      }
    }
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];

    if(pfc_new) {
      if(in_stripes > 0) {
	if(pfc_learns) { // error feedback goes to superficial guys!
	  net->FindMakePrjn(pfc_s_in, ol, fullprjn, topfcfmout_cons);
	}
      }
      if(out_stripes > 0) {
	net->FindMakePrjn(ol, pfc_d_out, fullprjn, fmpfcd_out);
	if(pfc_learns) { // error feedback goes to superficial guys!
	  net->FindMakePrjn(pfc_s_out, ol, fullprjn, topfcfmout_cons);
	}
      }
      if(mnt_stripes > 0) {
	if(!(out_stripes > 0)) {
	  net->FindMakePrjn(ol, pfc_d_mnt, fullprjn, fmpfcd_out);
	}
	if(pfc_learns) { // error feedback goes to superficial guys!
	  net->FindMakePrjn(pfc_s_mnt, ol, fullprjn, topfcfmout_cons);
	}
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  snrthal->brain_area = ".*/.*/.*/.*/Substantia Nigra";
  // these are just random suggestions:
  if(in_stripes > 0) {
    if(matrix_go_in->brain_area.empty()) 
      matrix_go_in->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(matrix_nogo_in->brain_area.empty()) 
      matrix_nogo_in->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(pfc_s_in->brain_area.empty()) {
      pfc_s_in->brain_area = ".*/.*/.*/.*/BA45";
      pfc_d_in->brain_area = ".*/.*/.*/.*/BA45";
    }
  }
  if(mnt_stripes > 0) {
    if(matrix_go_mnt->brain_area.empty()) 
      matrix_go_mnt->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(matrix_nogo_mnt->brain_area.empty()) 
      matrix_nogo_mnt->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(pfc_s_mnt->brain_area.empty()) {
      pfc_s_mnt->brain_area = ".*/.*/.*/.*/BA9";
      pfc_d_mnt->brain_area = ".*/.*/.*/.*/BA9";
    }
  }
  if(out_stripes > 0) {
    if(matrix_go_out->brain_area.empty()) 
      matrix_go_out->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(matrix_nogo_out->brain_area.empty()) 
      matrix_nogo_out->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(pfc_s_out->brain_area.empty()) {
      pfc_s_out->brain_area = ".*/.*/.*/.*/BA44";
      pfc_d_out->brain_area = ".*/.*/.*/.*/BA44";
    }
  }

  int lay_spc = 2;

  int n_lv_u;           // number of pvlv-type units
  if(lvesp->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  else if(lvesp->scalar.rep == ScalarValSpec::GAUSSIAN)
    n_lv_u = 12;
  else
    n_lv_u = 21;


  if(new_pbwm_laygp) {
    pbwm_laygp_go->pos.z = 0;
    pbwm_laygp_go->pos.x = 20;
    pbwm_laygp_go->pos.y = 0;
    pbwm_laygp_nogo->pos.z = 0;
    pbwm_laygp_nogo->pos.x = 20;
    pbwm_laygp_nogo->pos.y = 50;
    pbwm_laygp_pfc->pos.z = 1;
    pbwm_laygp_pfc->pos.x = 20;
    pbwm_laygp_pfc->pos.y = 0;
  }

  ///////////////	Matrix Layout

  int mtx_st_x = 0;
  int mtx_st_y = 0;
  int mtx_nogo_y = mtx_st_y + matrix_go_mnt->disp_geom.y + 3 * lay_spc;
  int mtx_go_y = 2 * lay_spc;
  int mtx_z = 0;

  if(in_stripes > 0) {
    if(matrix_new) {
      matrix_go_in->pos.SetXYZ(mtx_st_x, mtx_st_y + mtx_go_y, mtx_z);
      matrix_nogo_in->pos.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);

      matrix_go_in->un_geom.n = 28; matrix_go_in->un_geom.x = 4;
      matrix_go_in->un_geom.y = 7;
      matrix_nogo_in->un_geom.n = 28; matrix_nogo_in->un_geom.x = 4;
      matrix_nogo_in->un_geom.y = 7;
    }
    lay_set_geom(matrix_go_in, in_stripes);
    lay_set_geom(matrix_nogo_in, in_stripes);

    mtx_nogo_y = MAX(mtx_nogo_y, mtx_st_y + matrix_go_in->disp_geom.y + 3 * lay_spc);
    mtx_st_x += matrix_go_in->disp_geom.x + lay_spc; // move over..
  }
  if(mnt_stripes > 0) {
    if(matrix_new) {
      matrix_go_mnt->pos.SetXYZ(mtx_st_x, mtx_st_y + mtx_go_y, mtx_z);
      matrix_nogo_mnt->pos.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);

      matrix_go_mnt->un_geom.n = 28; matrix_go_mnt->un_geom.x = 4;
      matrix_go_mnt->un_geom.y = 7;
      matrix_nogo_mnt->un_geom.n = 28; matrix_nogo_mnt->un_geom.x = 4;
      matrix_nogo_mnt->un_geom.y = 7;
    }
    lay_set_geom(matrix_go_mnt, mnt_stripes);
    lay_set_geom(matrix_nogo_mnt, mnt_stripes);

    mtx_nogo_y = MAX(mtx_nogo_y, mtx_st_y + matrix_go_mnt->disp_geom.y + 3 * lay_spc);
    mtx_st_x += matrix_go_mnt->disp_geom.x + lay_spc; // move over..
  }
  if(out_stripes > 0) {
    if(matrix_new) {
      matrix_go_out->pos.SetXYZ(mtx_st_x, mtx_st_y + mtx_go_y, mtx_z);
      matrix_nogo_out->pos.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);

      matrix_go_out->un_geom.n = 28; matrix_go_out->un_geom.x = 4;
      matrix_go_out->un_geom.y = 7;
      matrix_nogo_out->un_geom.n = 28; matrix_nogo_out->un_geom.x = 4;
      matrix_nogo_out->un_geom.y = 7;
    }
    lay_set_geom(matrix_go_out, out_stripes);
    lay_set_geom(matrix_nogo_out, out_stripes);

    mtx_nogo_y = MAX(mtx_nogo_y, mtx_st_y + matrix_go_out->disp_geom.y + 3 * lay_spc);
    mtx_st_x += matrix_go_out->disp_geom.x + lay_spc; // move over..
  }

  if(new_pbwm_laygp) {
    pbwm_laygp_nogo->pos.y = mtx_nogo_y; // move over!
  }

  ///////////////	PFC Layout first -- get into z = 1

  int pfc_st_x = 0;
  int pfc_st_y = 0;
  int pfc_deep_y = 18;
  int pfc_z = 0;
  if(in_stripes > 0) {
    if(pfc_new) {
      pfc_s_in->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      if(!pfc_learns && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_s_in->un_geom = il->un_geom;
      }
      else {
	pfc_s_in->un_geom.n = 30; pfc_s_in->un_geom.x = 5; pfc_s_in->un_geom.y = 6;
      }
    }
    lay_set_geom(pfc_s_in, in_stripes);
    pfc_deep_y = pfc_st_y + pfc_s_in->disp_geom.y + 3 * lay_spc;

    // repeat for deep guys..
    if(pfc_new) {
      pfc_d_in->pos.SetXYZ(pfc_st_x, pfc_deep_y, pfc_z);
      pfc_d_in->un_geom = pfc_s_in->un_geom;
    }
    lay_set_geom(pfc_d_in, in_stripes);
    pfc_st_x += pfc_s_in->disp_geom.x + lay_spc; // move starting x over for next type
  }

  if(mnt_stripes > 0) {
    if(pfc_new) {
      pfc_s_mnt->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      if(!pfc_learns && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_s_mnt->un_geom = il->un_geom;
      }
      else {
	pfc_s_mnt->un_geom.n = 30; pfc_s_mnt->un_geom.x = 5; pfc_s_mnt->un_geom.y = 6;
      }
    }
    lay_set_geom(pfc_s_mnt, mnt_stripes);
    pfc_deep_y = pfc_st_y + pfc_s_mnt->disp_geom.y + 3 * lay_spc;

    // repeat for deep guys..
    if(pfc_new) {
      pfc_d_mnt->pos.SetXYZ(pfc_st_x, pfc_deep_y, pfc_z);
      pfc_d_mnt->un_geom = pfc_s_mnt->un_geom;
    }

    lay_set_geom(pfc_d_mnt, mnt_stripes);
    pfc_st_x += pfc_s_mnt->disp_geom.x + lay_spc;
  }

  if(out_stripes > 0) {
    if(pfc_new) {
      pfc_s_out->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      if(!pfc_learns && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_s_out->un_geom = il->un_geom;
      }
      else {
	pfc_s_out->un_geom.n = 30; pfc_s_out->un_geom.x = 5; pfc_s_out->un_geom.y = 6;
      }
    }
    lay_set_geom(pfc_s_out, out_stripes);
    pfc_deep_y = pfc_st_y + pfc_s_out->disp_geom.y + 3 * lay_spc;

    // now repeat for out-deep guys;
    if(pfc_new) {
      pfc_d_out->pos.SetXYZ(pfc_st_x, pfc_deep_y, pfc_z);
      pfc_d_out->un_geom = pfc_s_out->un_geom;
    }
    lay_set_geom(pfc_d_out, out_stripes);
  }

  ///////////////	Now SNrThal

  int snr_stripes = in_stripes + mnt_stripes + out_stripes;
  lay_set_geom(snrthal, snr_stripes, 1);

  if(snrthal_new) { // put at front of go
    snrthal->pos.SetXYZ(0, 0, mtx_z);
  }

  // here to allow it to get disp_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, in_stripes, mnt_stripes, out_stripes);

  if(new_pbwm_laygp) {
    pbwm_laygp_go->pos.z = 0;
    pbwm_laygp_go->pos.x = 20;
    pbwm_laygp_go->pos.y = 0;
    pbwm_laygp_nogo->pos.z = 0;
    pbwm_laygp_nogo->pos.x = 20;
    pbwm_laygp_nogo->pos.y = mtx_nogo_y;
    pbwm_laygp_pfc->pos.z = 1;
    pbwm_laygp_pfc->pos.x = 20;
    pbwm_laygp_pfc->pos.y = 0;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  PBWM_Defaults(net, pfc_learns); // sets all default params and gets selectedits

  net->LayerPos_Cleanup();

  net->Build();			// rebuild after defaults in place

  // move back!
  if(new_pbwm_laygp) {
    pbwm_laygp_go->pos.z = 0;
    net->RebuildAllViews();     // trigger update
  }

  taMisc::CheckConfigStart(false, false);

  bool ok = false;
  ok = snrthalsp->CheckConfig_Layer(snrthal, false);
  if(in_stripes > 0) {
    ok &= pfc_s_in_sp->CheckConfig_Layer(pfc_s_in, false);
    ok &= pfc_d_in_sp->CheckConfig_Layer(pfc_d_in, false);
    ok &= matrix_go_in_sp->CheckConfig_Layer(matrix_go_in, false);
    ok &= matrix_nogo_in_sp->CheckConfig_Layer(matrix_nogo_in, false);
  }
  if(mnt_stripes > 0) {
    ok &= pfc_s_mnt_sp->CheckConfig_Layer(pfc_s_mnt, false);
    ok &= pfc_d_mnt_sp->CheckConfig_Layer(pfc_d_mnt, false);
    ok &= matrix_go_mnt_sp->CheckConfig_Layer(matrix_go_mnt, false);
    ok &= matrix_nogo_mnt_sp->CheckConfig_Layer(matrix_nogo_mnt, false);
  }
  if(out_stripes > 0) {
    ok &= pfc_s_out_sp->CheckConfig_Layer(pfc_s_out, false);
    ok &= pfc_d_out_sp->CheckConfig_Layer(pfc_d_out, false);
    ok &= matrix_go_out_sp->CheckConfig_Layer(matrix_go_out, false);
    ok &= matrix_nogo_out_sp->CheckConfig_Layer(matrix_nogo_out, false);
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
//              PBWM Defaults
/////////////////////////////////////////////////////////////////////////////

bool LeabraWizard::PBWM_Defaults(LeabraNetwork* net, bool pfc_learns) {
  if(!net) {
    if(TestError(!net, "PBWM", "network is NULL -- must be passed and already PBWM configured -- aborting!"))
      return false;
  }

  String pvenm = "PVe";  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";
  String vtanm = "VTA";

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  // IMPORTANT: this MUST be a literal copy from the main PBWM config -- must remain fully in-sync

  BaseSpec_Group* units = net->FindMakeSpecGp("PFC_BG_Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp("PFC_BG_Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp("PFC_BG_Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp("PFC_BG_Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return false;

  ////////////	UnitSpecs

  LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* lv_units = (LeabraUnitSpec*)pv_units->FindMakeChild("LVUnits", &TA_LeabraUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_LeabraUnitSpec);

  LeabraUnitSpec* pfc_units = (LeabraUnitSpec*)units->FindMakeSpec("PFCUnits", &TA_PFCsUnitSpec);
  LayerActUnitSpec* pfcd_units = (LayerActUnitSpec*)pfc_units->FindMakeChild("PFCdUnits", &TA_LayerActUnitSpec);
  LeabraUnitSpec* matrix_units = (LeabraUnitSpec*)units->FindMakeSpec("MatrixUnits", &TA_MatrixUnitSpec);
  LeabraUnitSpec* matrix_nogo_units = (LeabraUnitSpec*)matrix_units->FindMakeChild("MatrixNoGo", &TA_MatrixUnitSpec);
  LeabraUnitSpec* snrthal_units = (LeabraUnitSpec*)units->FindMakeSpec("SNrThalUnits", &TA_LeabraUnitSpec);

  ////////////	ConSpecs

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  LeabraConSpec* pvi_cons = (LeabraConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  LeabraConSpec* pvr_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("PVr", &TA_PVrConSpec);
  LeabraConSpec* lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_PVConSpec);
  LeabraConSpec* lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_PVConSpec);
  LeabraConSpec* nv_cons =  (LeabraConSpec*)pvi_cons->FindMakeChild("NV", &TA_PVConSpec);

  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);

  LeabraConSpec* topfc_cons = (LeabraConSpec*)learn_cons->FindMakeChild("ToPFC", &TA_LeabraConSpec);
  LeabraConSpec* pfc_bias = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCBias", &TA_LeabraBiasSpec);
  LeabraConSpec* pfctopfc_s_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCtoPFC_s", &TA_LeabraConSpec);
  LeabraConSpec* pfctopfc_d_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCtoPFC_d", &TA_LeabraConSpec);
  LeabraConSpec* topfcfmin_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCFmInput", &TA_LeabraConSpec);
  LeabraConSpec* topfcfmout_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCFmOutput", &TA_LeabraConSpec);

  MatrixConSpec* matrix_cons = (MatrixConSpec*)learn_cons->FindMakeChild("MatrixCons", &TA_MatrixConSpec);
  MatrixBiasSpec* matrix_bias = (MatrixBiasSpec*)matrix_cons->FindMakeChild("MatrixBias", &TA_MatrixBiasSpec);
  MatrixConSpec* matrix_cons_topo = (MatrixConSpec*)matrix_cons->FindMakeChild("MatrixConsTopo", &TA_MatrixConSpec);
  MatrixConSpec* matrix_cons_topo_weak = (MatrixConSpec*)matrix_cons_topo->FindMakeChild("MatrixConsTopoWeak", &TA_MatrixConSpec);
  MatrixConSpec* matrix_cons_topo_strong = (MatrixConSpec*)matrix_cons_topo->FindMakeChild("MatrixConsTopoStrong", &TA_MatrixConSpec);
  MatrixNoGoConSpec* matrix_cons_nogo = (MatrixNoGoConSpec*)matrix_cons->FindMakeChild("MatrixConsNoGo", &TA_MatrixNoGoConSpec);
  MatrixConSpec* matrix_cons_fmpvr = (MatrixConSpec*)matrix_cons->FindMakeChild("MatrixFmPvr", &TA_MatrixConSpec);

  LeabraConSpec* fmpfcd_out = (LeabraConSpec*)learn_cons->FindMakeChild("FmPFCd_out", &TA_LeabraConSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  LeabraConSpec* matrix_to_snrthal = (LeabraConSpec*)cons->FindMakeSpec("MatrixToSNrThal", &TA_LeabraConSpec);
  // matrix nogo to snrthal just a marker con

  ////////////	LayerSpecs

  LeabraLayerSpec* rewtargsp = (LeabraLayerSpec*)layers->FindMakeSpec("RewTargLayer", &TA_LeabraLayerSpec);
  ExtRewLayerSpec* pvesp = (ExtRewLayerSpec*)layers->FindMakeSpec(pvenm + "Layer", &TA_ExtRewLayerSpec);
  PVrLayerSpec* pvrsp = (PVrLayerSpec*)layers->FindMakeSpec(pvrnm + "Layer", &TA_PVrLayerSpec);
  PViLayerSpec* pvisp = (PViLayerSpec*)layers->FindMakeSpec(pvinm + "Layer", &TA_PViLayerSpec);
  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec);
  LViLayerSpec* lvisp = (LViLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec);
  NVLayerSpec* nvsp = (NVLayerSpec*)layers->FindMakeSpec(nvnm + "Layer", &TA_NVLayerSpec);

  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindType(&TA_PVLVDaLayerSpec);

  PFCLayerSpec* pfc_s_mnt_sp = (PFCLayerSpec*)layers->FindMakeSpec("PFCSuper_mnt", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_s_in_sp = (PFCLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_in", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_s_out_sp = (PFCLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_out", &TA_PFCLayerSpec);

  PFCLayerSpec* pfc_d_mnt_sp = (PFCLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCDeep_mnt", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_d_in_sp = (PFCLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_in", &TA_PFCLayerSpec);
  PFCLayerSpec* pfc_d_out_sp = (PFCLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_out", &TA_PFCLayerSpec);

  MatrixLayerSpec* matrix_go_mnt_sp = (MatrixLayerSpec*)layers->FindMakeSpec("Matrix_Go_mnt", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_go_in_sp = (MatrixLayerSpec*)matrix_go_mnt_sp->FindMakeChild("Matrix_Go_in", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_go_out_sp = (MatrixLayerSpec*)matrix_go_mnt_sp->FindMakeChild("Matrix_Go_out", &TA_MatrixLayerSpec);

  MatrixLayerSpec* matrix_nogo_mnt_sp = (MatrixLayerSpec*)matrix_go_mnt_sp->FindMakeChild("Matrix_NoGo_mnt", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_nogo_in_sp = (MatrixLayerSpec*)matrix_nogo_mnt_sp->FindMakeChild("Matrix_NoGo_in", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_nogo_out_sp = (MatrixLayerSpec*)matrix_nogo_mnt_sp->FindMakeChild("Matrix_NoGo_out", &TA_MatrixLayerSpec);

  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_SNrThalLayerSpec);

  ////////////	PrjnSpecs

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);

  SNrPrjnSpec* snr_prjn = (SNrPrjnSpec*)prjns->FindMakeSpec("SNrPrjn", &TA_SNrPrjnSpec);
  PVrToMatrixGoPrjnSpec* pvr_to_mtx_prjn = (PVrToMatrixGoPrjnSpec*)prjns->FindMakeSpec("PVrToMatrixGoPrjn", &TA_PVrToMatrixGoPrjnSpec);

  TopoWtsPrjnSpec* topomaster = (TopoWtsPrjnSpec*)prjns->FindMakeSpec("TopoMaster", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topofminput = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoFmInput", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* intrapfctopo = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoIntraPFC", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixpfc_self = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixPFC_Self", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixpfc_other = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixPFC_Other", &TA_TopoWtsPrjnSpec);

  TesselPrjnSpec* input_pfc = (TesselPrjnSpec*)prjns->FindMakeSpec("Input_PFC", &TA_TesselPrjnSpec);
  input_pfc->send_offs.SetSize(1); // this is all it takes!

  //////////////////////////////////////////////////////////////////////////////////
  // first: all the basic defaults from specs

  if(net->mid_minus_cycle < 5) {
    net->mid_minus_cycle = 40;
    net->min_cycles = net->mid_minus_cycle + 15;
  }

  units->Defaults();
  cons->Defaults();
  layers->Defaults();
  //  prjns->Defaults();

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters
  // IMPORTANT: where there are custom specs with appropriate unique parameters, these should
  // all be put into the Defaults_init which gets called in Defaults() above
  // code below should only be for cases where above defaults are not applicable

  // different PVLV defaults

  lvesp->lv.min_lvi = 0.4f;

  nvsp->nv.da_gain = 0.0f;
  dasp->da.da_gain = 1.0f;
  dasp->da.pv_gain = 0.1f;

  // lr sched:
  learn_cons->lrs_value = LeabraConSpec::NO_LRS;
  //learn_cons->lrate_sched.SetSize(2);
  //SchedItem* si = (SchedItem*)learn_cons->lrate_sched.FastEl(0);
  //si->start_val = 1.0f;
  //si = (SchedItem*)learn_cons->lrate_sched.FastEl(1);
  //si->start_ctr = 90;
  //si->start_val = .1f;

  // slow learning rate on to pfc cons!
  topfc_cons->SetUnique("lrate", true);
  if(pfc_learns) {
    topfc_cons->learn = true;
    topfc_cons->lrate = .005f;
    topfc_cons->SetUnique("rnd", false);
    topfc_cons->rnd.var = 0.25f;
  }
  else {
    topfc_cons->lrate = 0.0f;
    topfc_cons->learn = false;
    topfc_cons->SetUnique("rnd", true);
    topfc_cons->rnd.var = 0.0f;
  }

  topfc_cons->SetUnique("lmix", true);
  topfc_cons->lmix.hebb = .001f;

  pfctopfc_s_cons->SetUnique("rnd", true);
  pfctopfc_s_cons->rnd.mean = 0.0f;
  pfctopfc_s_cons->rnd.var = 0.25f;
  // todo: wt_scale??

  pfctopfc_d_cons->SetUnique("rnd", true);
  pfctopfc_d_cons->rnd.mean = 0.0f;
  pfctopfc_d_cons->rnd.var = 0.25f;
  // todo: wt_scale??

  topfcfmin_cons->SetUnique("rnd", true);
  topfcfmin_cons->rnd.mean = 0.0f;
  topfcfmin_cons->rnd.var = 0.25f;
  // todo: wt_scale??

  topfcfmout_cons->SetUnique("wt_scale", true);
  topfcfmout_cons->wt_scale.rel = 0.5f;

  matrix_cons->SetUnique("rnd", true);
  matrix_cons->rnd.mean = 0.5f; 
  matrix_cons->rnd.var = .25f; 
  matrix_cons->SetUnique("lrate", true);
  matrix_cons->lrate = .05f;
  matrix_cons->SetUnique("wt_sig", true);
  matrix_cons->wt_sig.gain = 6.0f;
  matrix_cons->wt_sig.off = 1.25f;

  matrix_cons_topo->SetUnique("rnd", true);
  matrix_cons_topo->rnd.mean = 0.0f;
  matrix_cons_topo->rnd.var = .25f;
  matrix_cons_topo->SetUnique("wt_sig", false);
  matrix_cons_topo->SetUnique("lmix", false);

  matrix_cons_nogo->SetUnique("wt_scale", true);
  matrix_cons_nogo->wt_scale.abs = .5f;
  matrix_cons_nogo->SetUnique("rnd", true);
  matrix_cons_nogo->rnd.mean = 0.0f;
  matrix_cons_nogo->rnd.var = .25f;
  matrix_cons_nogo->SetUnique("wt_sig", false);
  matrix_cons_nogo->SetUnique("lmix", false);

  matrix_cons_topo_weak->SetUnique("wt_scale", true);
  matrix_cons_topo_weak->wt_scale.rel = .2f;

  matrix_cons_topo_strong->SetUnique("wt_scale", true);
  matrix_cons_topo_strong->wt_scale.rel = 2.0f;

  matrix_cons_fmpvr->SetUnique("wt_scale", true);
  matrix_cons_fmpvr->wt_scale.rel = 1.0f; // just in case you want to manip..
  matrix_cons_fmpvr->SetUnique("rnd", true);
  matrix_cons_fmpvr->rnd.mean = 0.9f;
  matrix_cons_fmpvr->rnd.var = 0.0f;
  matrix_cons_fmpvr->SetUnique("learn", true);
  matrix_cons_fmpvr->learn = false;

  fmpfcd_out->SetUnique("wt_scale", true);
  fmpfcd_out->SetUnique("wt_sig", true);
  fmpfcd_out->wt_sig.dwt_norm = false; // not sure if this is necc but whatever..

  matrix_to_snrthal->SetUnique("rnd", true);
  matrix_to_snrthal->rnd.mean = 0.7f;
  matrix_to_snrthal->rnd.var = 0.0f;
  matrix_to_snrthal->SetUnique("learn", true);
  matrix_to_snrthal->learn = false;
  matrix_to_snrthal->SetUnique("lrate", true);
  matrix_to_snrthal->lrate = 0.0f;

  matrix_bias->SetUnique("lrate", true);
  matrix_bias->lrate = 0.0f;            // default is no bias learning

  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;                // default is no bias learning

  matrix_go_mnt_sp->go_nogo = MatrixLayerSpec::GO;
  matrix_go_mnt_sp->gating_type = SNrThalLayerSpec::MAINT;
  matrix_go_in_sp->SetUnique("gating_type",true);
  matrix_go_in_sp->gating_type = SNrThalLayerSpec::INPUT;
  matrix_go_out_sp->SetUnique("gating_type",true);
  matrix_go_out_sp->gating_type = SNrThalLayerSpec::OUTPUT;

  matrix_nogo_mnt_sp->SetUnique("go_nogo",true);
  matrix_nogo_mnt_sp->go_nogo = MatrixLayerSpec::NOGO;
  matrix_nogo_mnt_sp->gating_type = SNrThalLayerSpec::MAINT;
  matrix_nogo_in_sp->SetUnique("gating_type",true);
  matrix_nogo_in_sp->gating_type = SNrThalLayerSpec::INPUT;
  matrix_nogo_out_sp->SetUnique("gating_type",true);
  matrix_nogo_out_sp->gating_type = SNrThalLayerSpec::OUTPUT;

  pfc_d_in_sp->SetUnique("gp_kwta",true);
  pfc_d_in_sp->gp_kwta.act_pct = 0.02f;

  pfc_d_mnt_sp->SetUnique("gp_kwta",true);
  pfc_d_mnt_sp->gp_kwta.act_pct = 0.02f;

  pfc_d_out_sp->SetUnique("gp_kwta",true);
  pfc_d_out_sp->gp_kwta.act_pct = 0.02f;

  pfc_s_out_sp->unit_gp_inhib.gp_g = 0.8f;

  pfc_s_mnt_sp->pfc_layer = PFCLayerSpec::SUPER;
  pfc_s_mnt_sp->pfc_type = SNrThalLayerSpec::MAINT;

  pfc_s_in_sp->SetUnique("pfc_type",true);
  pfc_s_in_sp->pfc_type = SNrThalLayerSpec::INPUT;

  pfc_s_out_sp->SetUnique("pfc_type",true);
  pfc_s_out_sp->pfc_type = SNrThalLayerSpec::OUTPUT;

  pfc_d_mnt_sp->SetUnique("pfc_layer",true);
  pfc_d_mnt_sp->pfc_layer = PFCLayerSpec::DEEP;

  pfc_d_in_sp->SetUnique("pfc_type",true);
  pfc_d_in_sp->pfc_type = SNrThalLayerSpec::INPUT;

  pfc_d_out_sp->SetUnique("pfc_type",true);
  pfc_d_out_sp->pfc_type = SNrThalLayerSpec::OUTPUT;

  snrthal_units->SetUnique("maxda", true);
  snrthal_units->maxda.val = MaxDaSpec::NO_MAX_DA;

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
  pvr_cons->lrate = .1f;
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

  //////////	Prjns -- mostly config in main function, to not overwrite key params

  topomaster->wt_range.min = 0.1f;
  topomaster->wt_range.max = 0.5f;
  topomaster->grad_type = TopoWtsPrjnSpec::GAUSSIAN;
  topomaster->gauss_sig = 0.1f;

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
    pfc_s_mnt_sp->SelectForEditNm("gate", edit, "pfc_s_mnt", subgp);
    pfc_s_mnt_sp->SelectForEditNm("gp_kwta", edit, "pfc_s_mnt", subgp,
      "pfc kwta parameters -- pct is main param for pfc dynamics, and act_pct for balancing excitation to other layers");
    topfc_cons->SelectForEditNm("lrate", edit, "to_pfc", subgp,
        "PFC requires a slower learning rate in general, around .005 if go_learn_base is set to default of .06, otherwise .001 for go_learn_base of 1");

    subgp = "Matrix";
    matrix_go_mnt_sp->SelectForEditNm("matrix", edit, "matrix", subgp);
    matrix_go_mnt_sp->SelectForEditNm("gp_kwta", edit, "matrix", subgp,
      "matrix kwta parameters -- pct, gp_g are main for matrix dynamics (gp_g = 1 almost always best)");

    matrix_units->SelectForEditNm("noise", edit, "matrix", subgp,
      "matrix noise -- variance around .001 seems best overall");
    matrix_units->SelectForEditNm("noise_adapt", edit, "matrix", subgp);
    matrix_cons->SelectForEditNm("lrate", edit, "matrix", subgp,
     "Default Matrix lrate is .05");
    matrix_cons->SelectForEditNm("err_gain", edit, "matrix", subgp);

    subgp = "SNrThal";
    snrthalsp->SelectForEditNm("kwta", edit, "snrthal", subgp,
      "snrthal kwta parameters -- k = 2 is default");
    snrthalsp->SelectForEditNm("inhib", edit, "snrthal", subgp,
      "Default is KWTA_AVG_INHIB with kwta_pt = .8 -- more competition but with some flexibility from avg-based computation");
    snrthalsp->SelectForEditNm("snrthal", edit, "snrthal", subgp);
  }
  return true;
}

//////////////////////////////
//      Remove!!!

bool LeabraWizard::PBWM_Remove(LeabraNetwork* net) {
  if(TestError(!net, "PBWM_ToLayerGroup", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_Remove before -- actually saves network specifically");
  }

  net->StructUpdate(true);

  net->RemoveUnits();
  net->layers.gp.RemoveName("PBWM");
  net->layers.gp.RemoveName("PBWM_PFC");
  net->layers.gp.RemoveName("PBWM_BG");
  net->layers.gp.RemoveName("PBWM_Go");
  net->layers.gp.RemoveName("PBWM_NoGo");
  net->layers.gp.RemoveName("PBWM_PFC");
  net->layers.gp.RemoveName("PVLV");

  net->specs.gp.RemoveName("PFC_BG_Prjns"); // TODO: make sure right names
  net->specs.gp.RemoveName("PFC_BG_Layers");
  net->specs.gp.RemoveName("PFC_BG_Cons");
  net->specs.gp.RemoveName("PFC_BG_Units");

  net->CheckSpecs();            // could have nuked dependent specs!

  net->StructUpdate(false);

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_Remove after -- actually saves network specifically");
  }
  return true;
}


