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

#include "SNrThalLayerSpec.h"
#include <LeabraNetwork>
#include <MatrixLayerSpec>
#include <PBWMUnGpData>

#include <taMisc>


void SNrThalMiscSpec::Initialize() {
  go_thr = 0.5f;
  min_cycle = 15;
}

void SNrThalLayerSpec::Initialize() {
  gating_types = IN_MNT_OUT;

  Defaults_init();
}

void SNrThalLayerSpec::Defaults_init() {
  SetUnique("gating_types", true); // always unique

  // SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;

  // SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .7f;

  // SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 3;

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
 - Computes activation as standard Leabra only from Matrix Go layer -- NoGo only affects Go.\n\
 - No learning, wt init variance, in afferent cons\n\
 \nSNrThalLayerSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure.\n\
 - Matrix_Go projects using regular leabra cons (non-MarkerCons) to create basic standard netinput.";
  taMisc::Confirm(help);
}

void SNrThalLayerSpec::GatingTypesNStripes(LeabraLayer* lay, 
					   int& n_in, int& n_mnt, int& n_out, int& n_mnt_out, int& n_out_mnt) {
  n_in = 0;
  n_mnt = 0;
  n_out = 0;
  n_mnt_out = 0;
  n_out_mnt = 0; 

  gating_types = NO_GATE_TYPE;

  for(int g=0; g<lay->projections.size; g++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)prjn->from.ptr();
    if(fmlay->lesioned()) continue;
    if(!fmlay->GetLayerSpec()->InheritsFrom(TA_MatrixLayerSpec)) continue;
    MatrixLayerSpec* mls = (MatrixLayerSpec*)fmlay->GetLayerSpec();
    if(mls->go_nogo != MatrixLayerSpec::GO) continue;

    gating_types = (GatingTypes)(gating_types | mls->gating_type);
    if(mls->gating_type == INPUT) n_in += fmlay->gp_geom.n;
    if(mls->gating_type == MNT) n_mnt += fmlay->gp_geom.n;
    if(mls->gating_type == OUTPUT) n_out += fmlay->gp_geom.n;
    if(mls->gating_type == MNT_OUT) n_mnt_out += fmlay->gp_geom.n;
    if(mls->gating_type == OUT_MNT) n_out_mnt += fmlay->gp_geom.n;
    
  }
}

int SNrThalLayerSpec::SNrThalStartIdx(LeabraLayer* lay, GatingTypes gating_type,
				      int& n_in, int& n_mnt, int& n_out, int& n_mnt_out, int& n_out_mnt) {
  GatingTypesNStripes(lay, n_in, n_mnt, n_out, n_mnt_out, n_out_mnt);
  int snr_st_idx = -1;
  switch(gating_type) {
  case INPUT:
    snr_st_idx = 0;
    break;
  case MNT:
    snr_st_idx = n_in;
    break;
  case MNT_OUT:
    snr_st_idx = n_in + n_mnt;
    break;
  case OUTPUT:
    snr_st_idx = n_in + n_mnt + n_mnt_out;
    break;
  case OUT_MNT:
    snr_st_idx = n_in + n_mnt + n_mnt_out + n_out;
    break; 
  default:			// compiler food
    break;
  }
  return snr_st_idx;
}

bool SNrThalLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

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

  int n_in, n_mnt, n_mnt_out, n_out, n_out_mnt;
  GatingTypesNStripes(lay, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
  int snr_stripes = n_in + n_mnt + n_mnt_out + n_out + n_out_mnt;
  
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
    gpd->go_cycle = -1;
  }

  lay->SetUserData("n_fired_trial", 0);
  lay->SetUserData("n_fired_now", 0);
}

void SNrThalLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Init_GateStats(lay, net);
}

void SNrThalLayerSpec::Compute_GateActs_Maint(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  int n_fired_trial = 0;
  int n_fired_now = 0;

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, mg); // assuming one unit
    if(u->lesioned()) continue;

    if(u->act_eq >= snrthal.go_thr) {
      n_fired_trial++;
      n_fired_now++;
      gpd->go_fired_now = true;
      gpd->go_fired_trial = true;
      gpd->go_cycle = net->ct_cycle;
      gpd->prv_mnt_count = gpd->mnt_count; 
      gpd->mnt_count = 0;	// reset
    }
    else {
      u->act_eq = u->act_p = 0.0f; // turn off non-gated guys
    }
  }

  Compute_GateStats(lay, net); // update overall stats at this point
  lay->SetUserData("n_fired_trial", n_fired_trial);
  lay->SetUserData("n_fired_now", n_fired_now);
}

void SNrThalLayerSpec::Compute_GateActs_Output(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  // gating window
  if(net->ct_cycle < snrthal.min_cycle)
    return;

  int n_fired_trial = 0;
  int n_fired_now = 0;

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, mg); // assuming one unit
    if(u->lesioned()) continue;

    if(net->ct_cycle > net->mid_minus_cycle) {
      if(!gpd->go_fired_trial)
        u->act_eq = 0.0f;
      continue;
    }

    if(gpd->go_fired_trial) {
      n_fired_trial++;
      gpd->go_fired_now = false;
    }
    else {
      if(u->act_eq >= snrthal.go_thr) {
        n_fired_trial++;
        n_fired_now++;
        gpd->go_fired_now = true;
        gpd->go_fired_trial = true;
        gpd->go_cycle = net->ct_cycle;
        gpd->prv_mnt_count = gpd->mnt_count; 
        gpd->mnt_count = 0;	// reset
      }
    }
  }

  if(net->ct_cycle > net->mid_minus_cycle) {
    return;
  }

  Compute_GateStats(lay, net); // update overall stats at this point
  lay->SetUserData("n_fired_trial", n_fired_trial);
  lay->SetUserData("n_fired_now", n_fired_now);
}


void SNrThalLayerSpec::Compute_GateStats(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  int n_in, n_mnt, n_mnt_out, n_out, n_out_mnt;
  GatingTypesNStripes(lay, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
  int mnt_st = n_in;
  int out_st = n_in + n_mnt;

  const int n_stats = 4;	// 0 = global, 1 = in, 2 = mnt, 3 = out
  int	n_gated[n_stats] = {0,0,0,0};
  int	min_mnt_count[n_stats] = {0,0,0,0};
  int	max_mnt_count[n_stats] = {0,0,0,0};

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    if(gpd->go_fired_trial) {
      if(mg < n_in) n_gated[1]++;
      else if(mg >= mnt_st && mg < out_st) n_gated[2]++;
      else if(mg >= out_st) n_gated[3]++;
      n_gated[0]++;		// global
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

  if(n_mnt > 0) {
    lay->SetUserData("min_mnt_count", (float)min_mnt_count[2]);
    lay->SetUserData("max_mnt_count", (float)max_mnt_count[2]);
  }
}

void SNrThalLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  if(gating_types & OUTPUT) { // everything else is maint gating 
    Compute_GateActs_Output(lay, net);
  }
}

void SNrThalLayerSpec::PostSettle_Pre(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle_Pre(lay, net);
  if(net->phase_no == 1) {
    if(!(gating_types & OUTPUT)) { // everything else is maint gating 
      Compute_GateActs_Maint(lay, net);
    }
  }
}

void SNrThalLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  lay->SetUserData("n_fired_trial", 0);
  lay->SetUserData("n_fired_now", 0);

  lay->SetUserData("n_gated_all", 0.0f);
  lay->SetUserData("n_gated_in",  0.0f);
  lay->SetUserData("n_gated_mnt", 0.0f);
  lay->SetUserData("n_gated_out", 0.0f);

  lay->SetUserData("min_mnt_count", 0.0f);
  lay->SetUserData("max_mnt_count", 0.0f);
}

