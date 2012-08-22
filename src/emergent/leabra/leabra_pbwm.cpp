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
  go_fired_trial2 = false;
  go_forced = false;
  go_cycle = -1;
  mnt_count = -1;		// empty
  prv_mnt_count = -1;
}

void PBWMUnGpData::Copy_(const PBWMUnGpData& cp) {
  go_fired_now = cp.go_fired_now;
  go_fired_trial = cp.go_fired_trial;
  go_fired_trial2 = cp.go_fired_trial2;
  go_forced = cp.go_forced;
  go_cycle = cp.go_cycle;
  mnt_count = cp.mnt_count;
  prv_mnt_count = cp.prv_mnt_count;
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
  min_go_cycle = 25;
  go_b4_mid = 0;
  force = false; // true;
  loser_gain = 0.0f;
  nogo_gain = 0.0f;
  leak = 0.2f;
  act_is_gate = true;
}

void SNrThalLayerSpec::Initialize() {
  gating_types = MNT_OUT;

  Defaults_init();
}

void SNrThalLayerSpec::Defaults_init() {
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;

  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .8f;

  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 2;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  SetUnique("tie_brk", false);
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

void SNrThalLayerSpec::GatingTypesNStripes(GatingTypes gat_typs, int n_stripes_total, int& n_types,
					   int& n_in, int& n_mnt, int& n_out) {
  n_types = 0;
  n_in = 0;
  n_mnt = 0;
  n_out = 0;
  if(gat_typs & INPUT) n_types++;
  if(gat_typs & MAINT) n_types++;
  if(gat_typs & OUTPUT) n_types++;

  if(n_types == 0) return;
  int n_per = n_stripes_total / n_types;
  if(gat_typs & INPUT) n_in = n_per;
  if(gat_typs & MAINT) n_mnt = n_per;
  if(gat_typs & OUTPUT) n_out = n_per;
}

bool SNrThalLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  int max_go_cycle = net->mid_minus_cycle - snrthal.go_b4_mid;
  if(lay->CheckError(max_go_cycle <= snrthal.min_go_cycle, quiet, rval,
                "min go cycle not before max go cycle -- adjusting min_go_cycle -- you should double check")) {
    snrthal.min_go_cycle = max_go_cycle - 15;
    if(snrthal.min_go_cycle < 2)
      snrthal.min_go_cycle = 2;
  }

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  // must have the appropriate ranges for unit specs..
  //  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  LeabraLayer* go_lay = NULL;
  LeabraLayer* nogo_lay = NULL;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      if(fmlay->spec.SPtr()->InheritsFrom(TA_MatrixLayerSpec)) nogo_lay = fmlay;
      // todo: should NOT recv any other marker cons!
    }
    else {
      if(fmlay->spec.SPtr()->InheritsFrom(TA_MatrixLayerSpec)) go_lay = fmlay;
    }
  }

  if(lay->CheckError(go_lay == NULL, quiet, rval,
                "did not find Matrix Go layer to recv from!")) {
    return false;
  }

  if(lay->CheckError(go_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
                "MatrixLayer unit groups must = SNrThalLayer unit groups!")) {
    lay->unit_groups = true;
    lay->gp_geom.n = go_lay->gp_geom.n;
  }

  return true;
}

LeabraLayer* SNrThalLayerSpec::MatrixGoLayer(LeabraLayer* lay) {
  LeabraLayer* go_lay = NULL;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(fmlay->spec.SPtr()->InheritsFrom(TA_MatrixLayerSpec)) {
      if(!recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
	go_lay = fmlay;
	break;
      }
    }
  }
  return go_lay;
}

LeabraLayer* SNrThalLayerSpec::MatrixNoGoLayer(LeabraLayer* lay) {
  LeabraLayer* nogo_lay = NULL;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      if(fmlay->spec.SPtr()->InheritsFrom(TA_MatrixLayerSpec)) {
	nogo_lay = fmlay;
	break;
      }
    }
  }
  return nogo_lay;
}

void SNrThalLayerSpec::Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net) {
  if(snrthal.nogo_gain == 0.0f) return;

  LeabraLayer* nogo_lay = MatrixNoGoLayer(lay);
  if(!nogo_lay) return;		// nothing to do if no nogo

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)nogo_lay->ungp_data.FastEl(mg);
    float nogo_inhib = snrthal.nogo_gain * gpd->acts.avg;
    LeabraUnit* snr_u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, 0, mg);
    snr_u->net = snr_u->net / (snr_u->net + snrthal.leak + nogo_inhib);
  }
}

void SNrThalLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  // note: this no longer has dt.net in effect here!! hopefully not a huge diff..
  Compute_GoNogoNet(lay, net);
  inherited::Compute_NetinStats(lay, net);
}

void SNrThalLayerSpec::Compute_GatedActs(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..
  int max_go_cycle = net->mid_minus_cycle - snrthal.go_b4_mid;

  int n_fired_trial = 0;
  int n_fired_now = 0;

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    if(net->ct_cycle < snrthal.min_go_cycle) {
      // reset all for current trial
      gpd->go_fired_now = false;
      gpd->go_fired_trial = false;
      gpd->go_forced = false;
      gpd->go_cycle = -1;

      for(int i=0;i<nunits;i++) {
	LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
	if(u->lesioned()) continue;
	u->act_m2 = 0.0f;	// reset gating act
      }
      continue;			// if all mg < min_go_cycle, basically all done!
     }

    if(gpd->go_fired_trial) {	// this stripe already fired go
      n_fired_trial++;
      gpd->go_fired_now = false;
      if(snrthal.act_is_gate) {
	for(int i=0;i<nunits;i++) {
	  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
	  if(u->lesioned()) continue;
	  u->act = u->act_eq = u->act_m2;	// paste it back into act
	}
      }
      continue;
    }

    if(net->ct_cycle < max_go_cycle) {
      // check to see if above threshold for firing
      for(int i=0;i<nunits;i++) {
	LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
	if(u->lesioned()) continue;
	if(u->act_eq >= snrthal.go_thr) {
	  n_fired_trial++;
	  n_fired_now++;
	  gpd->go_fired_now = true;
	  gpd->go_fired_trial = true;
	  gpd->go_cycle = net->ct_cycle;
	  gpd->prv_mnt_count = gpd->mnt_count;
	  gpd->mnt_count = 0;	// reset
	  u->act_m2 = u->act_eq;	// gating act -- will tend to be just above go_thr -- is updated until max_go_cycle
	  break;			// out of unit loop
	}
      }
    }
  } // end first and outer for loop

  if(net->ct_cycle == max_go_cycle) {
    // first deal with no go cases..
    if(snrthal.force && n_fired_trial == 0) {	// no stripe has fired yet -- force max go firing
      int go_idx = lay->netin.max_i;
      if(go_idx < 0) {		// this really shouldn't happen, but if it does..
	  go_idx = Random::IntZeroN(lay->units.leaves);
      }
      int gp_idx = go_idx / nunits;
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gp_idx);
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
    // now find all the nogos and update them..
    for(int mg=0; mg<lay->gp_geom.n; mg++) {
      PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
      if(!gpd->go_fired_trial) {
	if(gpd->mnt_count < 0)
	  gpd->mnt_count--;	// more empty
	else
	  gpd->mnt_count++;	// more maint
	for(int i=0;i<nunits;i++) {
	  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
	  if(u->lesioned()) continue;
	  u->act_m2 = snrthal.loser_gain * u->act_eq;
	  if(snrthal.act_is_gate) {
	    u->act = u->act_eq = u->act_m2;	// paste it back into act
	  }
	}
      }
    }
    Compute_GateStats(lay, net); // update overall stats at this point
  }
  lay->SetUserData("n_fired_trial", n_fired_trial);
  lay->SetUserData("n_fired_now", n_fired_now);
}


void SNrThalLayerSpec::Compute_GateStats(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  int n_types, n_in, n_mnt, n_out;
  SNrThalLayerSpec::GatingTypesNStripes(gating_types, lay->gp_geom.n, n_types,
					n_in, n_mnt, n_out);
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
  Compute_GatedActs(lay, net);
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
  lay->SetUserData("n_fired_trial", 0);
  lay->SetUserData("n_fired_now", 0);
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
  nogo_inhib = 0.0f; // 0.2f;
  da_gain = 0.1f;
  go_pfc_thr = 0.0f;
}

/////////////////////////////////////////////////////

void MatrixLayerSpec::Initialize() {
  gating_types = SNrThalLayerSpec::MNT_OUT;
  go_nogo = GO;

  Defaults_init();
}

void MatrixLayerSpec::Defaults_init() {
  // todo: sync with above
  matrix.nogo_inhib = 0.0f; // 0.2f;
  matrix.da_gain = 0.1f;
  matrix.go_pfc_thr = 0.0f;

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
  gp_kwta.gp_i = true;
  gp_kwta.gp_g = 1.0f;

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

  if(lay->CheckError(snr_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
                "SNrThal layer does not have same number of stripes as we do!")) {
    return false;
  }

  SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  gating_types = snrls->gating_types; // we copy from snrthal

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
  int n_types, n_in, n_mnt, n_out;
  SNrThalLayerSpec::GatingTypesNStripes(gating_types, lay->gp_geom.n, n_types,
					n_in, n_mnt, n_out);
  int mnt_st = n_in;
  int out_st = n_in + n_mnt;

  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    String nm;
    if(mg < n_in) nm = "i";
    else if(mg >= mnt_st && mg < out_st) nm = "m";
    else if(mg >= out_st) nm = "o";

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

float MatrixLayerSpec::Compute_NoGoInhibGo(LeabraLayer* lay,
                                       Layer::AccessMode acc_md, int gpidx,
                                       LeabraNetwork* net) {
  if(go_nogo == NOGO) return 0.0f;	// only go-side recv fm nogo

  int nogo_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* nogo_lay = FindLayerFmSpec(lay, nogo_prjn_idx, &TA_MatrixLayerSpec);
  PBWMUnGpData* gpd = (PBWMUnGpData*)nogo_lay->ungp_data.FastEl(gpidx);
  float nogo_inhib = matrix.nogo_inhib * gpd->acts.avg;
  return nogo_inhib;
}

void MatrixLayerSpec::Compute_ApplyInhib_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraInhib* thr, LeabraNetwork* net)
{
  if(go_nogo == NOGO) {
    inherited::Compute_ApplyInhib_ugp(lay, acc_md, gpidx, thr, net);
    return;
  }

  float nogo_inhib = Compute_NoGoInhibGo(lay, acc_md, gpidx, net);

  // add extra inhibition if our group is not active yet -- hack to not have to 
  // deal with all the complicated connectivity
  if(matrix.go_pfc_thr > 0.0f) {
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
    int n_mnt_gps = 0;					  // first find # of mnt gps
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(!fmlay->name.contains("PFCs")) continue;
      if(fmlay->name.contains("_mnt")) {
	n_mnt_gps = fmlay->gp_geom.n;
	break;
      }
    }
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(!fmlay->name.contains("PFCs")) continue;
      if(fmlay->name.contains("_out")) {
	if(gpidx < n_mnt_gps) continue;
	int eff_gp = gpidx - n_mnt_gps;
	if(eff_gp >= fmlay->gp_geom.n) continue; // not good
	LeabraUnGpData* gpd = (LeabraUnGpData*)fmlay->ungp_data.FastEl(eff_gp);
	if(gpd->acts.avg < matrix.go_pfc_thr) {
	  nogo_inhib += 100.0f;	// big extra inhib
	  break;
	}
      }
      else {
	if(gpidx >= n_mnt_gps) continue;
	if(gpidx >= fmlay->gp_geom.n) continue; // not good
	LeabraUnGpData* gpd = (LeabraUnGpData*)fmlay->ungp_data.FastEl(gpidx);
	if(gpd->acts.avg < matrix.go_pfc_thr) {
	  nogo_inhib += 100.0f;	// big extra inhib
	  break;
	}
      }
    }
  }

  int nunits = lay->UnitAccess_NUnits(acc_md);
  float inhib_val = thr->i_val.g_i + nogo_inhib;
  if(thr->kwta.tie_brk == 1) {
    float inhib_thr = thr->kwta.k_ithr;
    float inhib_loser = thr->kwta.eff_loser_gain * inhib_val;
    for(int i=0; i<nunits; i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      u->Compute_ApplyInhib_LoserGain(net, inhib_thr, inhib_val, inhib_loser);
      u->misc_1 = nogo_inhib;
    }
  }
  else {
    for(int i=0; i<nunits; i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      u->Compute_ApplyInhib(net, inhib_val);
      u->misc_1 = nogo_inhib;
    }
  }
}

void MatrixLayerSpec::Compute_GatingActs_ugp(LeabraLayer* lay,
                                       Layer::AccessMode acc_md, int gpidx,
                                       LeabraNetwork* net) {
  int snr_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  int max_go_cycle = net->mid_minus_cycle - snr_ls->snrthal.go_b4_mid;

  PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(gpidx);
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  gpd->CopyPBWMData(*snr_gpd);	// always grab from snr
  if(snr_gpd->go_fired_now) {
    Compute_MidMinusAct_ugp(lay, acc_md, gpidx, net); // grab our mid minus
  }
  else {
    if(net->ct_cycle == max_go_cycle && !snr_gpd->go_fired_trial) { // nogos will be nogos for sure by now
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

void PFCsUnitSpec::Initialize() {
  Defaults_init();
}

void PFCsUnitSpec::Defaults_init() {
}

void PFCsUnitSpec::Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  Compute_LearnMod(u, net);	// don't call dwt norm itself -- done for units that actually learn
}

void PFCsUnitSpec::Compute_LearnMod(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return;

  if(rlay->GetLayerSpec()->InheritsFrom(&TA_PFCDeepLayerSpec)) return;
  // if we're the deep layer, then just bail

  // first, find the deep pfc layer that we send to
  PFCDeepLayerSpec* dls = NULL;
  LeabraLayer* dl = NULL;
  for(int i=0; i< rlay->send_prjns.size; i++) {
    Projection* prj = rlay->send_prjns[i];
    LeabraLayer* play = (LeabraLayer*)prj->layer;
    if(!play->GetLayerSpec()->InheritsFrom(&TA_PFCDeepLayerSpec)) continue;
    dls = (PFCDeepLayerSpec*)play->GetLayerSpec();
    dl = play;
    break;
  }
  if(TestWarning(!dls, "Compute_LearnMod", "PFC Deep layer spec not found, exiting"))
    return;

  // get my unit group index for accessing ungp data structures
  int rgpidx;
  int rui;
  rlay->UnGpIdxFmUnitIdx(u->idx, rui, rgpidx);

  PBWMUnGpData* dlgpd = (PBWMUnGpData*)dl->ungp_data.SafeEl(rgpidx);
  if(TestWarning(!dlgpd, "Compute_LearnMod", "PFC Deep layer unit group out of range", 
		 String(rgpidx)))
    return;

  bool learn_now = dlgpd->go_fired_trial || dlgpd->mnt_count > 0;

  if(learn_now || !dls->gate.learn_deep_act) {
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
  maint_decay = 0.02f;
}

void PFCGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void PFCDeepLayerSpec::Initialize() {
  pfc_type = MAINT;
  gating_types = SNrThalLayerSpec::MNT_OUT;

  Defaults_init();
}

void PFCDeepLayerSpec::Defaults_init() {
  gate.learn_deep_act = true;
  gate.maint_decay = 0.02f;

  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .5f;

  // SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  gp_kwta.pct = 0.15f;
  gp_kwta.gp_i = true;
  gp_kwta.gp_g = 1.0f;
  gp_kwta.diff_act_pct = true;
  gp_kwta.act_pct = 0.02f;

  // SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;   // this is the one exception!
}

void PFCDeepLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate.UpdateAfterEdit_NoGui();
}

void PFCDeepLayerSpec::HelpConfig() {
  String help = "PFCDeepLayerSpec Computation:\n\
 The PFC deep layers are gated by thalamocortical prjns from SNrThal.\n\
 They maintain activation over time (activation-based working memory) via \
 excitatory intracelluar ionic mechanisms (implemented in hysteresis channels, gc.h),\
 and excitatory connections with superficial pfc layers, which is toggled by SNrThal.\n\
 Updating occurs by mid_minus_cycle, based on SNrThal act_m2 activations.\n\
 \nPFCDeepLayerSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure layers.\n\
 - First prjn must be from PFC superficial layer (can be any spec type)\n\
 - Units must be organized into groups so that the sum of the number of all\
   groups across all PFC deep layers must correspond to the number of matrix.\
   groups (stripes).";
  taMisc::Confirm(help);
}

bool PFCDeepLayerSpec::CheckConfig_Layer(Layer* ly,  bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
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

  int snr_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);

  if(lay->CheckError(!snr_lay, quiet, rval,
                "no projection from SNrThal Layer found: must exist with MarkerConSpec connection")) {
    return false;
  }

  SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  gating_types = snrls->gating_types; // we copy from snrthal

  int snr_st_idx, n_types, n_in, n_mnt, n_out;
  Compute_SNrThalStartIdx(lay, snr_st_idx, n_types, n_in, n_mnt, n_out);
  int n_tot = n_in + n_mnt + n_out;
  if(lay->CheckError(n_tot != snr_lay->gp_geom.n, quiet, rval,
		     "SNrThal layer unit groups must = total number of PFCLayer unit groups -- not fixing because matrix must also be adjusted!  total_n:", String(n_tot))) {
  }

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
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

  return true;
}

LeabraLayer* PFCDeepLayerSpec::SNrThalLayer(LeabraLayer* lay) {
  int snr_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  return snr_lay;
}

LeabraLayer* PFCDeepLayerSpec::LVeLayer(LeabraLayer* lay) {
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

LeabraLayer* PFCDeepLayerSpec::LViLayer(LeabraLayer* lay) {
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

LeabraLayer* PFCDeepLayerSpec::MatrixGoLayer(LeabraLayer* lay) {
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  if(!snr_lay) return NULL;
  SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  return snrls->MatrixGoLayer(snr_lay);
}

LeabraLayer* PFCDeepLayerSpec::MatrixNoGoLayer(LeabraLayer* lay) {
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  if(!snr_lay) return NULL;
  SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  return snrls->MatrixNoGoLayer(snr_lay);
}

void PFCDeepLayerSpec::Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net) {
  int snr_st_idx, n_types, n_in, n_mnt, n_out;
  LeabraLayer* snr_lay = Compute_SNrThalStartIdx(lay, snr_st_idx, n_types, n_in, n_mnt, n_out);
  for(int g=0; g < lay->gp_geom.n; g++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(g);
    PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + g);
    gpd->CopyPBWMData(*snr_gpd);
  }
}

void PFCDeepLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Compute_TrialInitGates(lay, net);
}

void PFCDeepLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // do NOT do this -- triggered by the snrthal gating signal
}

void PFCDeepLayerSpec::Compute_MidMinusAct_ugp(LeabraLayer* lay,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_m2 = u->act_eq;
  }
}

void PFCDeepLayerSpec::Clear_Maint(LeabraLayer* lay, LeabraNetwork* net, int stripe_no) {
  Compute_MaintUpdt(lay, net, CLEAR, stripe_no);
}

void PFCDeepLayerSpec::Compute_MaintUpdt(LeabraLayer* lay, LeabraNetwork* net,
				     MaintUpdtAct updt_act, int stripe_no) {
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

LeabraLayer* PFCDeepLayerSpec::Compute_SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
						       int& n_types, int& n_in, int& n_mnt, int& n_out) {
  snr_st_idx = 0;
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  SNrThalLayerSpec::GatingTypesNStripes(gating_types, snr_lay->gp_geom.n, n_types, n_in, n_mnt, n_out);
  switch(pfc_type) {
  case INPUT:
    snr_st_idx = 0;
    break;
  case MAINT:
    snr_st_idx = n_in;
    break;
  case OUTPUT:
    snr_st_idx = n_in + n_mnt;
    break;
  }
  return snr_lay;
}

void PFCDeepLayerSpec::Compute_MaintUpdt_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                         MaintUpdtAct updt_act, LeabraNetwork* net) {
  int snr_st_idx, n_types, n_in, n_mnt, n_out;
  LeabraLayer* snr_lay = Compute_SNrThalStartIdx(lay, snr_st_idx, n_types, n_in, n_mnt, n_out);
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

void PFCDeepLayerSpec::Compute_MaintAct_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
					    int gpidx, LeabraNetwork* net) {
  // activity is always just a literal copy of the maint_h
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int j=0;j<nunits;j++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
    if(u->lesioned()) continue;
    u->act = u->act_eq = u->act_nd = u->maint_h;
    u->da = 0.0f;
  }
}

void PFCDeepLayerSpec::Compute_Gating(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;

  int snr_st_idx, n_types, n_in, n_mnt, n_out;
  LeabraLayer* snr_lay = Compute_SNrThalStartIdx(lay, snr_st_idx, n_types, n_in, n_mnt, n_out);
  SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();

  int max_go_cycle = net->mid_minus_cycle - snrls->snrthal.go_b4_mid;

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + mg);
    gpd->CopyPBWMData(*snr_gpd);		// always grab from snr, which is the source

    if(pfc_type == MAINT) {	// maint gates at end of trial only..
      if(net->ct_cycle == max_go_cycle+1) {
	// end of the gating period -- add 1 just to be safe
	Compute_MidMinusAct_ugp(lay, acc_md, mg, net); // store mid minus now..
      }
    }
    else {
      if(gpd->go_fired_now) {
	Compute_MaintUpdt_ugp(lay, acc_md, mg, STORE, net);
	Compute_MidMinusAct_ugp(lay, acc_md, mg, net); // store mid minus now..
      }
      else {
	if(!gpd->go_fired_trial && net->ct_cycle == max_go_cycle+1) {
	  // end of the gating period -- add 1 just to be safe
	  Compute_MidMinusAct_ugp(lay, acc_md, mg, net); // store mid minus now..
	}
      }
    }
    Compute_MaintAct_ugp(lay, acc_md, mg, net);
  }
}

void PFCDeepLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_Gating(lay, net);   // continuously during mid minus phase
  inherited::Compute_CycleStats(lay, net);
}

void PFCDeepLayerSpec::Compute_ClearNonMnt(LeabraLayer* lay, LeabraNetwork* net) {
  if(pfc_type == MAINT) return;	// no clear

  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);
  }
}

void PFCDeepLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(net->phase_no == 1) {
    Compute_FinalGating(lay, net);     // final gating
  }
}

void PFCDeepLayerSpec::Compute_FinalGating(LeabraLayer* lay, LeabraNetwork* net) {
  if(pfc_type != MAINT) {
    Compute_ClearNonMnt(lay, net);     // done!
    return; 
  }

  Layer::AccessMode acc_md = Layer::ACC_GP;

  int n_mnt_gated = 0;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);

    if(!gpd->go_fired_trial) {
      Compute_MaintUpdt_ugp(lay, acc_md, mg, DECAY, net);
      continue;
    }

    Compute_MaintUpdt_ugp(lay, acc_md, mg, STORE, net);
    Compute_MaintAct_ugp(lay, acc_md, mg, net);
    n_mnt_gated++;
  }
  if(!n_mnt_gated) return;	// no maint gating -- 

  Compute_FinalGating_LV(lay, net);
  Compute_FinalGating_DA(lay, net);
}

void PFCDeepLayerSpec::Compute_FinalGating_LV(LeabraLayer* lay, LeabraNetwork* net) {
  // update LV layers based on final gating

  Layer::AccessMode acc_md = Layer::ACC_GP;

  LeabraLayer* lve_lay = LVeLayer(lay);
  LeabraLayer* lvi_lay = LViLayer(lay);
  LVeLayerSpec* lve_ls = (LVeLayerSpec*)lve_lay->GetLayerSpec();
  LViLayerSpec* lvi_ls = (LViLayerSpec*)lvi_lay->GetLayerSpec();
  LeabraUnitSpec* lve_us = (LeabraUnitSpec*)lve_lay->GetUnitSpec();
  LeabraUnitSpec* lvi_us = (LeabraUnitSpec*)lvi_lay->GetUnitSpec();

  int orig_lve_vm_eq_cyc = lve_us->dt.vm_eq_cyc;
  int orig_lvi_vm_eq_cyc = lvi_us->dt.vm_eq_cyc;
  lve_us->dt.vm_eq_cyc = 10000;
  lvi_us->dt.vm_eq_cyc = 10000;
  
  LeabraUnit* u;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lve_lay->units) {
    if(u->lesioned()) continue;
    lve_us->Compute_Netin(u, net, -1); // u->net = new netin directly
    u->i_thr = lve_us->Compute_IThresh(u, net);
  }
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lvi_lay->units) {
    if(u->lesioned()) continue;
    lvi_us->Compute_Netin(u, net, -1); // u->net = new netin directly
    u->i_thr = lvi_us->Compute_IThresh(u, net);
  }

  lve_lay->Compute_Inhib(net);
  lvi_lay->Compute_Inhib(net);

  lve_lay->Compute_ApplyInhib(net);
  lvi_lay->Compute_ApplyInhib(net);

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lve_lay->units) {
    if(u->lesioned()) continue;
    lve_us->Compute_Act(u, net, -1);
  }
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lvi_lay->units) {
    if(u->lesioned()) continue;
    lvi_us->Compute_Act(u, net, -1);
  }

  lve_lay->Compute_CycleStats(net);
  lvi_lay->Compute_CycleStats(net);
  
  lve_us->dt.vm_eq_cyc = orig_lve_vm_eq_cyc;
  lvi_us->dt.vm_eq_cyc = orig_lvi_vm_eq_cyc;
}

void PFCDeepLayerSpec::Compute_FinalGating_DA(LeabraLayer* lay, LeabraNetwork* net) {
  // update dopamine based on updated lv activations

  Layer::AccessMode acc_md = Layer::ACC_GP;

  LeabraLayer* mtxgo_lay = MatrixGoLayer(lay);
  MatrixLayerSpec* mtxgo_ls = (MatrixLayerSpec*)mtxgo_lay->GetLayerSpec();
  LeabraLayer* da_lay = mtxgo_ls->PVLVDaLayer(mtxgo_lay);
  PVLVDaLayerSpec* da_ls = (PVLVDaLayerSpec*)da_lay->GetLayerSpec();
  
  da_ls->Compute_Da(da_lay, net);
  da_ls->Send_Da(da_lay, net);
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
//           SNrToPFCPrjnSpec           //
//////////////////////////////////////////

void SNrToPFCPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  LeabraLayer* pfc_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* snr_lay = (LeabraLayer*)prjn->from.ptr();

  PFCDeepLayerSpec* pfc_dls = (PFCDeepLayerSpec*)pfc_lay->GetLayerSpec();
  if(TestError(!pfc_dls || !pfc_dls->InheritsFrom(&TA_PFCDeepLayerSpec),
	       "Connect_impl", "snr sending projection using SNrToPFCPrjnSpec is not connecting to a PFCDeepLayerSpec.  recv layer:", pfc_lay->name))
    return;

  int snr_st_idx, n_types, n_in, n_mnt, n_out;
  pfc_dls->Compute_SNrThalStartIdx(pfc_lay, snr_st_idx, n_types, n_in, n_mnt, n_out);

  for(int i=0; i<pfc_lay->gp_geom.n; i++) {
    Connect_Gp(prjn, Layer::ACC_GP, i, Layer::ACC_GP, snr_st_idx + i);
  }
}


//////////////////////////////////////////
//           PFCdToNoGoPrjnSpec         //
//////////////////////////////////////////

void PFCdToNoGoPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  LeabraLayer* nogo_lay = (LeabraLayer*)prjn->layer;
  LeabraLayer* pfc_lay = (LeabraLayer*)prjn->from.ptr();

  PFCDeepLayerSpec* pfc_dls = (PFCDeepLayerSpec*)pfc_lay->GetLayerSpec();
  if(TestError(!pfc_dls || !pfc_dls->InheritsFrom(&TA_PFCDeepLayerSpec),
	       "Connect_impl", "Matrix NoGo projection from PFC deep using PFCdToNoGoPrjnSpec is not connecting from a PFCDeepLayerSpec.  recv layer:", pfc_lay->name))
    return;

  int snr_st_idx, n_types, n_in, n_mnt, n_out;
  pfc_dls->Compute_SNrThalStartIdx(pfc_lay, snr_st_idx, n_types, n_in, n_mnt, n_out);

  for(int i=0; i<pfc_lay->gp_geom.n; i++) {
    Connect_Gp(prjn, Layer::ACC_GP, snr_st_idx + i, Layer::ACC_GP, i);
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

  int n_types, n_in, n_mnt, n_out;
  SNrThalLayerSpec::GatingTypesNStripes(mtx_ls->gating_types, mtx_lay->gp_geom.n, n_types, n_in, n_mnt, n_out);

  int n_per_gp = mtx_lay->un_geom.n;
  Layer::AccessMode racc_md = Layer::ACC_GP;

  if(mtx_ls->gating_types & SNrThalLayerSpec::INPUT) {
    Unit* pvr_su = pvr_lay->units.SafeEl(2); // .5 middle value unit
    if(mtx_ls->gating_types & SNrThalLayerSpec::MAINT) {
    	pvr_su->SendConsPreAlloc((n_per_gp * (n_in + n_mnt)), prjn);
    }
    else {
    	pvr_su->SendConsPreAlloc(n_per_gp * n_in, prjn);
    }
    for(int rgi=0; rgi < n_in; rgi++) {
      for(int rui=0; rui < n_per_gp; rui++) {
	Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
	ru->RecvConsPreAlloc(1, prjn);
      }
    }

    for(int rgi=0; rgi < n_in; rgi++) {
      for(int rui=0; rui < n_per_gp; rui++) {
	Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
	ru->ConnectFrom(pvr_su, prjn);
      }
    }
  }

  if(mtx_ls->gating_types & SNrThalLayerSpec::MAINT) {
    Unit* pvr_su = pvr_lay->units.SafeEl(2); // .5 middle value unit
    if(mtx_ls->gating_types & SNrThalLayerSpec::INPUT) { // already allocated
    	//pvr_su->SendConsPreAlloc((n_per_gp * (n_in + n_mnt)), prjn);
    }
    else {
    	pvr_su->SendConsPreAlloc(n_per_gp * n_mnt, prjn);
    }

    for(int rgi=n_in; rgi < n_in + n_mnt; rgi++) {
      for(int rui=0; rui < n_per_gp; rui++) {
	Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
	ru->RecvConsPreAlloc(1, prjn);
      }
    }

    for(int rgi=n_in; rgi < n_in + n_mnt; rgi++) {
      for(int rui=0; rui < n_per_gp; rui++) {
	Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
	ru->ConnectFrom(pvr_su, prjn);
      }
    }
  }

  if(mtx_ls->gating_types & SNrThalLayerSpec::OUTPUT) {
    Unit* pvr_su = pvr_lay->units.SafeEl(3); // 1 right value unit
    pvr_su->SendConsPreAlloc(n_per_gp * n_out, prjn);

    //for(int rgi=n_in + n_out; rgi < n_in + n_mnt + n_out; rgi++) {
    for(int rgi=n_in + n_mnt; rgi < n_in + n_mnt + n_out; rgi++) {
      for(int rui=0; rui < n_per_gp; rui++) {
	Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
	ru->RecvConsPreAlloc(1, prjn);
      }
    }

    for(int rgi=n_in + n_out; rgi < n_in + n_mnt + n_out; rgi++) {
      for(int rui=0; rui < n_per_gp; rui++) {
	Unit* ru = mtx_lay->UnitAccess(racc_md, rui, rgi);
	ru->ConnectFrom(pvr_su, prjn);
      }
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
  Defaults_init();
}

void TopoWtsPrjnSpec::Defaults_init() {
  init_wts = true;
  add_rnd_wts = true;
  add_rnd_wts_scale = 1.0f;
}

void TopoWtsPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

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

// NOTE: Perhaps ideally, would have multiple n_stripes vars for as many PFC layer roles as used
//       For now, simply use same number for each role, then user can customize, if desired
bool LeabraWizard::PBWM_SetNStripes(LeabraNetwork* net, GatingTypes gating_types,
				    int n_stripes, int n_units,
                                    int gp_geom_x, int gp_geom_y) {
  if(TestError(!net, "PBWM_SetNStripes", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  // this is called as a subroutine a lot too so don't save here -- could do impl but
  // not really worth it..
//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_SetNStripes -- actually saves network specifically");
//   }

  set_n_stripes(net, "PFCs_mnt", n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "PFCd_mnt", n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "PFCs_out", n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "PFCd_out", n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "PFCs_in",  n_stripes, n_units, true, gp_geom_x, gp_geom_y);
  set_n_stripes(net, "PFCd_in",  n_stripes, n_units, true, gp_geom_x, gp_geom_y);

  // matrix layers need to preserve x dimension of pfc stripe geometry
  TwoDCoord pfc_geom;

  if(gp_geom_x > 0 && gp_geom_y > 0) {
      pfc_geom.x = gp_geom_x;
      pfc_geom.y = gp_geom_y;
  }
  else {
    if(n_stripes <= 5 || n_stripes == 7 || n_stripes == 11 || n_stripes >= 13) {
      pfc_geom.x = n_stripes;
      pfc_geom.y = 1;
    }
    else{
      if(n_stripes == 6 || n_stripes == 8 || n_stripes == 10) {
        pfc_geom.x = n_stripes / 2;
        pfc_geom.y = 2;
      }
      else {
        if(n_stripes == 9 || n_stripes == 12) {
          pfc_geom.x = n_stripes / 3;
          pfc_geom.y = 3;
        }
      }
    }
  }

  int n_types, n_in, n_mnt, n_out;
  SNrThalLayerSpec::GatingTypesNStripes((SNrThalLayerSpec::GatingTypes)gating_types, 24, // just care about n_types
					n_types, n_in, n_mnt, n_out);
  // Matrix_Go, _NoGo, SNrThal have as many stripes as TOTAL for all PFCd layers
  int tot_stripes = n_types * n_stripes;
  set_n_stripes(net, "Matrix_Go",   tot_stripes, -1, true, pfc_geom.x, n_types * pfc_geom.y);
  set_n_stripes(net, "Matrix_NoGo", tot_stripes, -1, true, pfc_geom.x, n_types * pfc_geom.y);
  set_n_stripes(net, "SNrThal", tot_stripes, -1, false, pfc_geom.x, n_types * pfc_geom.y);

  set_n_stripes(net, "LVe", 1, -1, false, 1, 1);
  set_n_stripes(net, "LVi", 1, -1, false, 1, 1);
  net->Build();
  return true;
}

bool LeabraWizard::PBWM(LeabraNetwork* net, GatingTypes gating_types,
			bool da_mod_all, int n_stripes, bool pfc_learns) {
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
  Layer_Group* pbwm_laygp = net->FindMakeLayerGroup("PBWM", NULL, new_pbwm_laygp);

  // if not new layers, don't make prjns into them!
  bool matrix_go_new =   false; bool pfc_s_mnt_new = false; bool pfc_d_mnt_new = false;
  bool matrix_nogo_new = false; bool pfc_s_out_new = false; bool pfc_d_out_new = false;
  bool snrthal_new =     false; bool pfc_s_in_new =  false; bool pfc_d_in_new =  false;

  LeabraLayer* pfc_s_mnt = NULL;
  LeabraLayer* pfc_d_mnt = NULL;
  LeabraLayer* pfc_s_out = NULL;
  LeabraLayer* pfc_d_out = NULL;
  LeabraLayer* pfc_s_in = NULL;
  LeabraLayer* pfc_d_in = NULL;

  LeabraLayer* matrix_go = NULL;
  LeabraLayer* matrix_nogo = NULL;
  LeabraLayer* snrthal = NULL;

  matrix_go = (LeabraLayer*)pbwm_laygp->FindMakeLayer("Matrix_Go", NULL, matrix_go_new, "Matrix");
  matrix_nogo = (LeabraLayer*)pbwm_laygp->FindMakeLayer("Matrix_NoGo", NULL, matrix_nogo_new);
  snrthal = (LeabraLayer*)pbwm_laygp->FindMakeLayer("SNrThal", NULL, snrthal_new, "SNrThal");

  if(gating_types & INPUT) {
    pfc_s_in =  (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCs_in",  NULL, pfc_s_in_new);
    pfc_d_in =  (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCd_in",  NULL, pfc_d_in_new);
  }
  if(gating_types & MAINT) {
    pfc_s_mnt = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCs_mnt", NULL, pfc_s_mnt_new, "PFC");
    pfc_d_mnt = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCd_mnt", NULL, pfc_d_mnt_new);
  }
  if(gating_types & OUTPUT) {
    pfc_s_out = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCs_out", NULL, pfc_s_out_new);
    pfc_d_out = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCd_out", NULL, pfc_d_out_new);
  }

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
       && lay != snrthal && lay != matrix_go && lay != matrix_nogo
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
  LeabraConSpec* topfcstrong_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCStrong", &TA_LeabraConSpec);
  LeabraConSpec* topfctopo_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCTopo", &TA_LeabraConSpec);
  LeabraConSpec* topfcfmout_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCFmOutput", &TA_LeabraConSpec);
  LeabraConSpec* topfcself_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCSelf", &TA_LeabraConSpec);

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

  PFCDeepLayerSpec* pfc_d_mnt_sp = (PFCDeepLayerSpec*)layers->FindMakeSpec("PFCDeep_mnt", &TA_PFCDeepLayerSpec);
  PFCDeepLayerSpec* pfc_d_out_sp = (PFCDeepLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_out", &TA_PFCDeepLayerSpec);
  PFCDeepLayerSpec* pfc_d_in_sp = (PFCDeepLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_in", &TA_PFCDeepLayerSpec);

  LeabraLayerSpec* pfc_s_mnt_sp = (LeabraLayerSpec*)layers->FindMakeSpec("PFCSuper_mnt", &TA_LeabraLayerSpec);
  LeabraLayerSpec* pfc_s_out_sp =  (LeabraLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_out", &TA_LeabraLayerSpec);
  LeabraLayerSpec* pfc_s_in_sp =  (LeabraLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_in", &TA_LeabraLayerSpec);

  MatrixLayerSpec* matrix_go_sp = (MatrixLayerSpec*)layers->FindMakeSpec("Matrix_Go", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_nogo_sp = (MatrixLayerSpec*)matrix_go_sp->FindMakeChild("Matrix_NoGo", &TA_MatrixLayerSpec);

  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_SNrThalLayerSpec);

  ////////////	PrjnSpecs

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);

  SNrToPFCPrjnSpec* snr_to_pfc_prjn = (SNrToPFCPrjnSpec*)prjns->FindMakeSpec("SNrToPFCPrjn", &TA_SNrToPFCPrjnSpec);
  PFCdToNoGoPrjnSpec* pfc_to_nogo_prjn = (PFCdToNoGoPrjnSpec*)prjns->FindMakeSpec("PFCdToNoGoPrjn", &TA_PFCdToNoGoPrjnSpec);
  PVrToMatrixGoPrjnSpec* pvr_to_mtx_prjn = (PVrToMatrixGoPrjnSpec*)prjns->FindMakeSpec("PVrToMatrixGoPrjn", &TA_PVrToMatrixGoPrjnSpec);

  TopoWtsPrjnSpec* topomaster = (TopoWtsPrjnSpec*)prjns->FindMakeSpec("TopoMaster", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topofminput = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoFmInput", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* intrapfctopo = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoIntraPFC", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixfmmnt = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixFmMnt", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixfmout = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixFmOut", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixfmin = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixFmIn", &TA_TopoWtsPrjnSpec);

  TesselPrjnSpec* input_pfc = (TesselPrjnSpec*)prjns->FindMakeSpec("Input_PFC", &TA_TesselPrjnSpec);
  input_pfc->send_offs.SetSize(1); // this is all it takes!

  // NOTE: moved to after SetNStripes() call -- so we know the geometries!
/*
  /////////////		Config Topo PrjnSpecs

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
  topofminput->send_range_end.y = (n_stripes / 2)-1; // unlikely to be generally useful..
  topofminput->SetUnique("custom_recv_range", true);
  topofminput->custom_recv_range = false;

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

  topomatrixfmmnt->SetUnique("grad_x", true);
  topomatrixfmmnt->grad_x = true;
  topomatrixfmmnt->SetUnique("grad_x_grad_y", true);
  topomatrixfmmnt->grad_x_grad_y = false;
  topomatrixfmmnt->SetUnique("grad_y", true);
  topomatrixfmmnt->grad_y = true;
  topomatrixfmmnt->SetUnique("grad_y_grad_x", true);
  topomatrixfmmnt->grad_y_grad_x = false;
  topomatrixfmmnt->SetUnique("wrap", true);
  topomatrixfmmnt->wrap = true;
  topomatrixfmmnt->SetUnique("use_recv_gps", true);
  topomatrixfmmnt->use_recv_gps = true;
  topomatrixfmmnt->SetUnique("use_send_gps", true);
  topomatrixfmmnt->use_send_gps = true;
  topomatrixfmmnt->SetUnique("custom_send_range", true);
  topomatrixfmmnt->custom_send_range = false;
  topomatrixfmmnt->SetUnique("custom_recv_range", true);
  topomatrixfmmnt->custom_recv_range = true;
  topomatrixfmmnt->SetUnique("recv_range_start", true);
  topomatrixfmmnt->recv_range_start.x = 0;
  topomatrixfmmnt->recv_range_start.y = 0; // TODO: only if no INPUT PFC layers!!
  topomatrixfmmnt->SetUnique("recv_range_end", true);
  topomatrixfmmnt->recv_range_end.x = -1;
  topomatrixfmmnt->recv_range_end.y = 1; // todo: only for n_stripes with 2 rows..

  topomatrixfmout->SetUnique("grad_x", true);
  topomatrixfmout->grad_x = true;
  topomatrixfmout->SetUnique("grad_x_grad_y", true);
  topomatrixfmout->grad_x_grad_y = false;
  topomatrixfmout->SetUnique("grad_y", true);
  topomatrixfmout->grad_y = true;
  topomatrixfmout->SetUnique("grad_y_grad_x", true);
  topomatrixfmout->grad_y_grad_x = false;
  topomatrixfmout->SetUnique("wrap", true);
  topomatrixfmout->wrap = true;
  topomatrixfmout->SetUnique("use_recv_gps", true);
  topomatrixfmout->use_recv_gps = true;
  topomatrixfmout->SetUnique("use_send_gps", true);
  topomatrixfmout->use_send_gps = true;
  topomatrixfmout->SetUnique("custom_send_range", true);
  topomatrixfmout->custom_send_range = false;
  topomatrixfmout->SetUnique("custom_recv_range", true);
  topomatrixfmout->custom_recv_range = true;
  topomatrixfmout->SetUnique("recv_range_start", true);
  topomatrixfmout->recv_range_start.x = 0;
  topomatrixfmout->recv_range_start.y = 2; // todo: only for n_stripes with 2 rows..
  topomatrixfmout->SetUnique("recv_range_end", true);
  topomatrixfmout->recv_range_end.x = -1;
  topomatrixfmout->recv_range_end.y = -1; // todo: only for n_stripes with 2 rows..

  topomatrixfmin->SetUnique("grad_x", true);
  topomatrixfmin->grad_x = true;
  topomatrixfmin->SetUnique("grad_x_grad_y", true);
  topomatrixfmin->grad_x_grad_y = false;
  topomatrixfmin->SetUnique("grad_y", true);
  topomatrixfmin->grad_y = true;
  topomatrixfmin->SetUnique("grad_y_grad_x", true);
  topomatrixfmin->grad_y_grad_x = false;
  topomatrixfmin->SetUnique("wrap", true);
  topomatrixfmin->wrap = false;
  topomatrixfmin->SetUnique("use_recv_gps", true);
  topomatrixfmin->use_recv_gps = true;
  topomatrixfmin->SetUnique("use_send_gps", true);
  topomatrixfmin->use_send_gps = true;
  topomatrixfmin->SetUnique("custom_send_range", true);
  topomatrixfmin->custom_send_range = false;
  topomatrixfmin->SetUnique("custom_recv_range", true);
  topomatrixfmin->custom_recv_range = true;
  topomatrixfmin->SetUnique("recv_range_start", true);
  topomatrixfmin->recv_range_start.x = 0;
  topomatrixfmin->recv_range_start.y = 0; // todo: only for n_stripes with 2 rows..
  topomatrixfmin->SetUnique("recv_range_end", true);
  topomatrixfmin->recv_range_end.x = -1;
  topomatrixfmin->recv_range_end.y = 1; // todo: only for n_stripes with 2 rows..
*/

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  snrthalsp->gating_types = (SNrThalLayerSpec::GatingTypes)gating_types;
  matrix_go_sp->gating_types = (SNrThalLayerSpec::GatingTypes)gating_types;
  pfc_d_mnt_sp->gating_types = (SNrThalLayerSpec::GatingTypes)gating_types;

  // set bias specs for unit specs
  pfc_units->bias_spec.SetSpec(pfc_bias);
  pfcd_units->bias_spec.SetSpec(pfc_bias);
  matrix_units->bias_spec.SetSpec(matrix_bias);
  matrix_nogo_units->bias_spec.SetSpec(matrix_bias);
  snrthal_units->bias_spec.SetSpec(bg_bias);

  snrthal->SetLayerSpec(snrthalsp); snrthal->SetUnitSpec(snrthal_units);
  matrix_go->SetLayerSpec(matrix_go_sp);   matrix_go->SetUnitSpec(matrix_units);
  matrix_nogo->SetLayerSpec(matrix_nogo_sp);   matrix_nogo->SetUnitSpec(matrix_nogo_units);

  if(gating_types & INPUT) {
    pfc_s_in->SetLayerSpec(pfc_s_in_sp);  pfc_s_in->SetUnitSpec(pfc_units);
    pfc_d_in->SetLayerSpec(pfc_d_in_sp);  pfc_d_in->SetUnitSpec(pfcd_units);
  }
  if(gating_types & MAINT) {
    pfc_s_mnt->SetLayerSpec(pfc_s_mnt_sp);  pfc_s_mnt->SetUnitSpec(pfc_units);
    pfc_d_mnt->SetLayerSpec(pfc_d_mnt_sp);  pfc_d_mnt->SetUnitSpec(pfcd_units);
  }
  if(gating_types & OUTPUT) {
    pfc_s_out->SetLayerSpec(pfc_s_out_sp);  pfc_s_out->SetUnitSpec(pfc_units);
    pfc_d_out->SetLayerSpec(pfc_d_out_sp);  pfc_d_out->SetUnitSpec(pfcd_units);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make projections


  //	  	 	   to		 from		prjn_spec	con_spec
  net->FindMakePrjn(snrthal, matrix_go, gponetoone, matrix_to_snrthal);
  net->FindMakePrjn(snrthal, matrix_nogo, gponetoone, marker_cons);

  net->FindMakePrjn(matrix_go, matrix_nogo, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_go, snrthal, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_go, vta, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_go, pvr, pvr_to_mtx_prjn, matrix_cons_fmpvr);
  // input -> matrix_go, pfc -> matrix come below

  net->FindMakePrjn(matrix_nogo, snrthal, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_nogo, vta, fullprjn, marker_cons);

  if(gating_types & MAINT) {
    net->FindMakePrjn(matrix_go, pfc_s_mnt, topomatrixfmmnt, matrix_cons_topo_weak);
    net->FindMakePrjn(matrix_nogo, pfc_d_mnt, topomatrixfmmnt, matrix_cons_nogo);

    net->FindMakePrjn(pfc_s_mnt, pfc_d_mnt, intrapfctopo, topfcself_cons);
    if(gating_types & INPUT) {
      net->FindMakePrjn(pfc_s_mnt, pfc_d_in, intrapfctopo, topfctopo_cons);
    }
    if(gating_types & OUTPUT) {
      // could be an extra feedback training signal, but not that useful (diluted)
      // net->FindMakePrjn(pfc_s_mnt, pfc_s_out, fullprjn, topfc_cons); signal!
    }
    net->FindMakePrjn(pfc_d_mnt, pfc_s_mnt, onetoone, marker_cons);
    net->FindMakePrjn(pfc_d_mnt, snrthal, snr_to_pfc_prjn, marker_cons);
  }

  if(gating_types & OUTPUT) {
    net->FindMakePrjn(matrix_go, pfc_s_out, topomatrixfmout, matrix_cons_topo);
    net->FindMakePrjn(matrix_nogo, pfc_d_out, topomatrixfmout, matrix_cons_nogo);

    net->FindMakePrjn(pfc_s_out, pfc_d_out, intrapfctopo, topfcself_cons);
    if(gating_types & MAINT) {
      net->FindMakePrjn(pfc_s_out, pfc_d_mnt, intrapfctopo, topfctopo_cons);
    }
    net->FindMakePrjn(pfc_d_out, pfc_s_out, onetoone, marker_cons);
    net->FindMakePrjn(pfc_d_out, snrthal, snr_to_pfc_prjn, marker_cons);
  }

  if(gating_types & INPUT) {
    net->FindMakePrjn(matrix_go, pfc_s_in, topomatrixfmin, matrix_cons_topo);
    net->FindMakePrjn(matrix_nogo, pfc_d_in, topomatrixfmin, matrix_cons_nogo);

    net->FindMakePrjn(pfc_s_in, pfc_d_in, intrapfctopo, topfcself_cons);
    if(gating_types & MAINT) {
      // could be an extra feedback training signal, but not that useful (diluted)
      // net->FindMakePrjn(pfc_s_in, pfc_s_mnt, fullprjn, topfc_cons);
    }
    if(gating_types & OUTPUT) {
      // could be an extra feedback training signal, but not that useful (diluted)
      // net->FindMakePrjn(pfc_s_in, pfc_s_out, fullprjn, topfc_cons);
    }
    net->FindMakePrjn(pfc_d_in, pfc_s_in, onetoone, marker_cons);
    net->FindMakePrjn(pfc_d_in, snrthal, snr_to_pfc_prjn, marker_cons);
  }

  // pvlv recv from deep only, maint only needed..
  if(gating_types & MAINT) {
    // net->FindMakePrjn(pvr, pfc_d_mnt, fullprjn, pvr_cons);
    net->FindMakePrjn(pvi, pfc_d_mnt, fullprjn, pvi_cons);
    net->FindMakePrjn(lve, pfc_d_mnt, fullprjn, lve_cons);
    net->FindMakePrjn(lvi, pfc_d_mnt, fullprjn, lvi_cons);
    net->FindMakePrjn(nv,  pfc_d_mnt, fullprjn, nv_cons);
  }

  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];

    if(matrix_go_new)  // posterior cortex presumably also projects from superficial..
      net->FindMakePrjn(matrix_go, il, topofminput, matrix_cons_topo);
    // if(matrix_nogo_new) // not to nogos!
    //   net->FindMakePrjn(matrix_nogo, il, topofminput, matrix_cons_topo);

    if(gating_types & INPUT) {
      if(pfc_s_in_new) {
        if(pfc_learns)
          net->FindMakePrjn(pfc_s_in, il, topofminput, topfctopo_cons);
        else
          net->FindMakePrjn(pfc_s_in, il, input_pfc, topfc_cons);
      }
    }

    if(gating_types & MAINT) {
      if(pfc_s_mnt_new) {
        if(pfc_learns)
          net->FindMakePrjn(pfc_s_mnt, il, topofminput, topfctopo_cons);
        else
          net->FindMakePrjn(pfc_s_mnt, il, input_pfc, topfc_cons);
      }
    }
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    if(gating_types & OUTPUT) {
      net->FindMakePrjn(ol, pfc_d_out, fullprjn, fmpfcd_out);
      if(pfc_d_out_new && pfc_learns) { // error feedback goes to superficial guys!
        net->FindMakePrjn(pfc_s_out, ol, fullprjn, topfcfmout_cons);
      }
    }
    if(gating_types & MAINT) {
      if(!(gating_types & OUTPUT)) {
	net->FindMakePrjn(ol, pfc_d_mnt, fullprjn, fmpfcd_out);
      }
      if(pfc_d_mnt_new && pfc_learns) { // error feedback goes to superficial guys!
        net->FindMakePrjn(pfc_s_mnt, ol, fullprjn, topfcfmout_cons);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  int lay_spc = 2;

  if(new_pbwm_laygp) {
    pbwm_laygp->pos.z = 0;
    pbwm_laygp->pos.x = 20;
  }

  ///////////////	PFC Layout first -- get into z = 1

  int pfc_st_x = 0;
  int pfc_st_y = 0;
  int pfc_deep_y = 18;
  int pfc_z = 1;
  if(gating_types & INPUT) {
    if(pfc_s_in_new) {
      pfc_s_in->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      if(!pfc_learns && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_s_in->un_geom = il->un_geom;
      }
      else {
	pfc_s_in->un_geom.n = 30; pfc_s_in->un_geom.x = 5; pfc_s_in->un_geom.y = 6;
      }
    }
    lay_set_geom(pfc_s_in, n_stripes);
    pfc_deep_y = pfc_s_in->disp_geom.y + 3 * lay_spc;

    // repeat for deep guys..
    if(pfc_d_in_new) {
      pfc_d_in->pos.SetXYZ(pfc_st_x, pfc_deep_y, pfc_z);
      pfc_d_in->un_geom = pfc_s_in->un_geom;
    }

    lay_set_geom(pfc_d_mnt, n_stripes);
    pfc_st_x += pfc_s_in->disp_geom.x + lay_spc;
  }

  if(gating_types & MAINT) {
    if(pfc_s_mnt_new) {
      pfc_s_mnt->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      if(!pfc_learns && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_s_mnt->un_geom = il->un_geom;
      }
      else {
	pfc_s_mnt->un_geom.n = 30; pfc_s_mnt->un_geom.x = 5; pfc_s_mnt->un_geom.y = 6;
      }
    }
    lay_set_geom(pfc_s_mnt, n_stripes);
    pfc_deep_y = pfc_s_mnt->disp_geom.y + 3 * lay_spc;

    // repeat for deep guys..
    if(pfc_d_mnt_new) {
      pfc_d_mnt->pos.SetXYZ(pfc_st_x, pfc_deep_y, pfc_z);
      pfc_d_mnt->un_geom = pfc_s_mnt->un_geom;
    }

    lay_set_geom(pfc_d_mnt, n_stripes);
    pfc_st_x += pfc_s_mnt->disp_geom.x + lay_spc;
  }

  if(gating_types & OUTPUT) {
    if(pfc_s_out_new) {
      pfc_s_out->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      if(!pfc_learns && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_s_out->un_geom = il->un_geom;
      }
      else {
	pfc_s_out->un_geom.n = 30; pfc_s_out->un_geom.x = 5; pfc_s_out->un_geom.y = 6;
      }
    }
    lay_set_geom(pfc_s_out, n_stripes);
    pfc_deep_y = pfc_s_out->disp_geom.y + 3 * lay_spc;

    // now repeat for out-deep guys;
    if(pfc_d_out_new) {
      pfc_d_out->pos.SetXYZ(pfc_st_x, pfc_deep_y, pfc_z);
      pfc_d_out->un_geom = pfc_s_out->un_geom;
    }
    lay_set_geom(pfc_d_out, n_stripes);
  }

  ///////////////	Now Matrix, SNrThal

  int n_lv_u;           // number of pvlv-type units
  if(lvesp->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  else if(lvesp->scalar.rep == ScalarValSpec::GAUSSIAN)
    n_lv_u = 12;
  else
    n_lv_u = 21;

  int n_types, n_in, n_mnt, n_out;
  SNrThalLayerSpec::GatingTypesNStripes((SNrThalLayerSpec::GatingTypes)gating_types, 24, // just care about n_types
					n_types, n_in, n_mnt, n_out);
  int tot_stripes = n_types * n_stripes;
  if(matrix_go_new) {
    matrix_go->un_geom.n = 28; matrix_go->un_geom.x = 4; matrix_go->un_geom.y = 7;
  }
  if(matrix_nogo_new) {
    matrix_nogo->un_geom.n = 28; matrix_nogo->un_geom.x = 4; matrix_nogo->un_geom.y = 7;
  }
  lay_set_geom(matrix_go, tot_stripes);
  lay_set_geom(snrthal, tot_stripes, 1);
  lay_set_geom(matrix_nogo, tot_stripes);

  // this is here, to allow it to get disp_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, gating_types, n_stripes);

  int mtx_x = 0;
  if(matrix_go_new) {
    matrix_go->pos.SetXYZ(mtx_x, 0, 0);
  }
  mtx_x += matrix_go->disp_geom.x + lay_spc; 

  // repeat for nogo guys; offset x by maybe 10
  if(matrix_nogo_new) {
    matrix_nogo->pos.SetXYZ(mtx_x + snrthal->disp_geom.x + lay_spc, 0, 0);
  }
  if(snrthal_new) { // put between go and nogo
    snrthal->pos.SetXYZ(mtx_x, 0, 0);
  }

  // this is here, to allow it to get disp_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, gating_types, n_stripes);

  if(new_pbwm_laygp) {
    pbwm_laygp->pos.z = 0;
    pbwm_laygp->pos.x = 20;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  PBWM_SetNStripes(net, gating_types, n_stripes);
  PBWM_Defaults(net, pfc_learns); // sets all default params and gets selectedits

  net->LayerPos_Cleanup();

  // move back!
  if(new_pbwm_laygp) {
    pbwm_laygp->pos.z = 0;
    net->RebuildAllViews();     // trigger update
  }

  /////////////		Config Topo PrjnSpecs

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
  topofminput->send_range_end.y = (n_stripes / 2)-1; // unlikely to be generally useful..
  topofminput->SetUnique("custom_recv_range", true);
  topofminput->custom_recv_range = false;

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

  topomatrixfmin->SetUnique("grad_x", true);
   topomatrixfmin->grad_x = true;
   topomatrixfmin->SetUnique("grad_x_grad_y", true);
   topomatrixfmin->grad_x_grad_y = false;
   topomatrixfmin->SetUnique("grad_y", true);
   topomatrixfmin->grad_y = true;
   topomatrixfmin->SetUnique("grad_y_grad_x", true);
   topomatrixfmin->grad_y_grad_x = false;
   topomatrixfmin->SetUnique("wrap", true);
   topomatrixfmin->wrap = false;
   topomatrixfmin->SetUnique("use_recv_gps", true);
   topomatrixfmin->use_recv_gps = true;
   topomatrixfmin->SetUnique("use_send_gps", true);
   topomatrixfmin->use_send_gps = true;
   topomatrixfmin->SetUnique("custom_send_range", true);
   topomatrixfmin->custom_send_range = false;
   topomatrixfmin->SetUnique("custom_recv_range", true);
   topomatrixfmin->custom_recv_range = true;
   topomatrixfmin->SetUnique("recv_range_start", true);
   topomatrixfmin->recv_range_start.x = 0;
   topomatrixfmin->recv_range_start.y = 0; // todo: only for n_stripes with 2 rows..
   topomatrixfmin->SetUnique("recv_range_end", true);
   topomatrixfmin->recv_range_end.x = -1;
   topomatrixfmin->recv_range_end.y = 1; // todo: only for n_stripes with 2 rows..

  topomatrixfmmnt->SetUnique("grad_x", true);
  topomatrixfmmnt->grad_x = true;
  topomatrixfmmnt->SetUnique("grad_x_grad_y", true);
  topomatrixfmmnt->grad_x_grad_y = false;
  topomatrixfmmnt->SetUnique("grad_y", true);
  topomatrixfmmnt->grad_y = true;
  topomatrixfmmnt->SetUnique("grad_y_grad_x", true);
  topomatrixfmmnt->grad_y_grad_x = false;
  topomatrixfmmnt->SetUnique("wrap", true);
  topomatrixfmmnt->wrap = true;
  topomatrixfmmnt->SetUnique("use_recv_gps", true);
  topomatrixfmmnt->use_recv_gps = true;
  topomatrixfmmnt->SetUnique("use_send_gps", true);
  topomatrixfmmnt->use_send_gps = true;
  topomatrixfmmnt->SetUnique("custom_send_range", true);
  topomatrixfmmnt->custom_send_range = false;
  topomatrixfmmnt->SetUnique("custom_recv_range", true);
  topomatrixfmmnt->custom_recv_range = true;
  topomatrixfmmnt->SetUnique("recv_range_start", true);
  topomatrixfmmnt->recv_range_start.x = 0;
  topomatrixfmmnt->recv_range_start.y = 0; // TODO: only if no INPUT PFC layers!!
  topomatrixfmmnt->SetUnique("recv_range_end", true);
  topomatrixfmmnt->recv_range_end.x = -1;
  topomatrixfmmnt->recv_range_end.y = 1; // todo: only for n_stripes with 2 rows..

  topomatrixfmout->SetUnique("grad_x", true);
  topomatrixfmout->grad_x = true;
  topomatrixfmout->SetUnique("grad_x_grad_y", true);
  topomatrixfmout->grad_x_grad_y = false;
  topomatrixfmout->SetUnique("grad_y", true);
  topomatrixfmout->grad_y = true;
  topomatrixfmout->SetUnique("grad_y_grad_x", true);
  topomatrixfmout->grad_y_grad_x = false;
  topomatrixfmout->SetUnique("wrap", true);
  topomatrixfmout->wrap = true;
  topomatrixfmout->SetUnique("use_recv_gps", true);
  topomatrixfmout->use_recv_gps = true;
  topomatrixfmout->SetUnique("use_send_gps", true);
  topomatrixfmout->use_send_gps = true;
  topomatrixfmout->SetUnique("custom_send_range", true);
  topomatrixfmout->custom_send_range = false;
  topomatrixfmout->SetUnique("custom_recv_range", true);
  topomatrixfmout->custom_recv_range = true;
  topomatrixfmout->SetUnique("recv_range_start", true);
  topomatrixfmout->recv_range_start.x = 0;
  topomatrixfmout->recv_range_start.y = 2; // todo: only for n_stripes with 2 rows..
  topomatrixfmout->SetUnique("recv_range_end", true);
  topomatrixfmout->recv_range_end.x = -1;
  topomatrixfmout->recv_range_end.y = -1; // todo: only for n_stripes with 2 rows..



  taMisc::CheckConfigStart(false, false);

  bool ok = false;
  ok = matrix_go_sp->CheckConfig_Layer(matrix_go, false);
  ok &= matrix_nogo_sp->CheckConfig_Layer(matrix_nogo, false);
  ok &= snrthalsp->CheckConfig_Layer(snrthal, false);
  if(gating_types & INPUT) {
    ok &= pfc_s_in_sp->CheckConfig_Layer(pfc_s_in, false);
    ok &= pfc_d_in_sp->CheckConfig_Layer(pfc_d_in, false);
  }
  if(gating_types & MAINT) {
    ok  = pfc_s_mnt_sp->CheckConfig_Layer(pfc_s_mnt, false);
    ok &= pfc_d_mnt_sp->CheckConfig_Layer(pfc_d_mnt, false);
  }
  if(gating_types & OUTPUT) {
    ok &= pfc_s_out_sp->CheckConfig_Layer(pfc_s_out, false);
    ok &= pfc_d_out_sp->CheckConfig_Layer(pfc_d_out, false);
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
  LeabraConSpec* topfcstrong_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCStrong", &TA_LeabraConSpec);
  LeabraConSpec* topfctopo_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCTopo", &TA_LeabraConSpec);
  LeabraConSpec* topfcfmout_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCFmOutput", &TA_LeabraConSpec);
  LeabraConSpec* topfcself_cons = (LeabraConSpec*)topfc_cons->FindMakeChild("ToPFCSelf", &TA_LeabraConSpec);

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

  PFCDeepLayerSpec* pfc_d_mnt_sp = (PFCDeepLayerSpec*)layers->FindMakeSpec("PFCDeep_mnt", &TA_PFCDeepLayerSpec);
  PFCDeepLayerSpec* pfc_d_out_sp = (PFCDeepLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_out", &TA_PFCDeepLayerSpec);
  PFCDeepLayerSpec* pfc_d_in_sp = (PFCDeepLayerSpec*)pfc_d_mnt_sp->FindMakeChild("PFCDeep_in", &TA_PFCDeepLayerSpec);

  LeabraLayerSpec* pfc_s_mnt_sp = (LeabraLayerSpec*)layers->FindMakeSpec("PFCSuper_mnt", &TA_LeabraLayerSpec);
  LeabraLayerSpec* pfc_s_out_sp =  (LeabraLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_out", &TA_LeabraLayerSpec);
  LeabraLayerSpec* pfc_s_in_sp =  (LeabraLayerSpec*)pfc_s_mnt_sp->FindMakeChild("PFCSuper_in", &TA_LeabraLayerSpec);

  MatrixLayerSpec* matrix_go_sp = (MatrixLayerSpec*)layers->FindMakeSpec("Matrix_Go", &TA_MatrixLayerSpec);
  MatrixLayerSpec* matrix_nogo_sp = (MatrixLayerSpec*)matrix_go_sp->FindMakeChild("Matrix_NoGo", &TA_MatrixLayerSpec);

  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_SNrThalLayerSpec);

  ////////////	PrjnSpecs

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);

  SNrToPFCPrjnSpec* snr_to_pfc_prjn = (SNrToPFCPrjnSpec*)prjns->FindMakeSpec("SNrToPFCPrjn", &TA_SNrToPFCPrjnSpec);
  PFCdToNoGoPrjnSpec* pfc_to_nogo_prjn = (PFCdToNoGoPrjnSpec*)prjns->FindMakeSpec("PFCdToNoGoPrjn", &TA_PFCdToNoGoPrjnSpec);
  PVrToMatrixGoPrjnSpec* pvr_to_mtx_prjn = (PVrToMatrixGoPrjnSpec*)prjns->FindMakeSpec("PVrToMatrixGoPrjn", &TA_PVrToMatrixGoPrjnSpec);

  TopoWtsPrjnSpec* topomaster = (TopoWtsPrjnSpec*)prjns->FindMakeSpec("TopoMaster", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topofminput = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoFmInput", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* intrapfctopo = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoIntraPFC", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixfmmnt = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixFmMnt", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixfmout = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixFmOut", &TA_TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* topomatrixfmin = (TopoWtsPrjnSpec*)topomaster->FindMakeChild("TopoMatrixFmIn", &TA_TopoWtsPrjnSpec);

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

  topfcstrong_cons->SetUnique("wt_scale", true);
  topfcstrong_cons->wt_scale.rel = 2.0f;

  topfctopo_cons->SetUnique("rnd", true);
  topfctopo_cons->rnd.mean = 0.0f;
  topfctopo_cons->rnd.var = 0.25f;

  topfcfmout_cons->SetUnique("wt_scale", true);
  topfcfmout_cons->wt_scale.rel = 0.5f;

  topfcself_cons->SetUnique("rnd", true);
  topfcself_cons->rnd.mean = 0.0f;
  topfcself_cons->rnd.var = 0.25f;
  topfcself_cons->SetUnique("wt_scale", true);
  topfcself_cons->wt_scale.rel = 0.5f;

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

  matrix_go_sp->go_nogo = MatrixLayerSpec::GO;
  matrix_nogo_sp->SetUnique("go_nogo", true);
  matrix_nogo_sp->go_nogo = MatrixLayerSpec::NOGO;

  pfc_s_mnt_sp->SetUnique("inhib",true);
  pfc_s_mnt_sp->inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  pfc_s_mnt_sp->inhib.kwta_pt = 0.5f;
  pfc_s_mnt_sp->SetUnique("decay",true);
  pfc_s_mnt_sp->decay.event = 0.0f;
  pfc_s_mnt_sp->decay.phase = 0.0f;
  pfc_s_mnt_sp->decay.phase2 = 0.0f;
  pfc_s_mnt_sp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
  pfc_s_mnt_sp->SetUnique("gp_kwta",true);
  pfc_s_mnt_sp->gp_kwta.pct = 0.15f;
  pfc_s_mnt_sp->gp_kwta.gp_i = true;
  pfc_s_mnt_sp->gp_kwta.gp_g = 1.0f;
  pfc_s_mnt_sp->gp_kwta.diff_act_pct = true;
  pfc_s_mnt_sp->gp_kwta.act_pct = 0.05f;
  // all other specs should inherit this!

  // todo -- sync: w above EXCEPT gp_g!
  pfc_s_out_sp->SetUnique("gp_kwta",true);
  pfc_s_out_sp->gp_kwta.pct = 0.15f;
  pfc_s_out_sp->gp_kwta.gp_i = true;
  pfc_s_out_sp->gp_kwta.gp_g = 0.8f;
  pfc_s_out_sp->gp_kwta.diff_act_pct = true;
  pfc_s_out_sp->gp_kwta.act_pct = 0.05f;

  pfc_d_mnt_sp->SetUnique("pfc_type",true);
  pfc_d_mnt_sp->pfc_type = PFCDeepLayerSpec::MAINT;

  pfc_d_out_sp->SetUnique("pfc_type",true);
  pfc_d_out_sp->pfc_type = PFCDeepLayerSpec::OUTPUT;

  pfc_d_in_sp->SetUnique("pfc_type",true);
  pfc_d_in_sp->pfc_type = PFCDeepLayerSpec::INPUT;

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
    pfc_d_mnt_sp->SelectForEditNm("gate", edit, "pfc_d", subgp);
    pfc_s_mnt_sp->SelectForEditNm("gp_kwta", edit, "pfc_s", subgp,
      "pfc kwta parameters -- pct, gp_g are main for pfc dynamics, and act_pct for balancing excitation to other layers");
    topfc_cons->SelectForEditNm("lrate", edit, "to_pfc", subgp,
        "PFC requires a slower learning rate in general, around .005 if go_learn_base is set to default of .06, otherwise .001 for go_learn_base of 1");

    subgp = "Matrix";
    matrix_go_sp->SelectForEditNm("matrix", edit, "matrix", subgp);
    matrix_go_sp->SelectForEditNm("gp_kwta", edit, "matrix", subgp,
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


