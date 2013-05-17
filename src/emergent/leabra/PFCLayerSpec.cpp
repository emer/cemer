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

#include "PFCLayerSpec.h"
#include <LeabraNetwork>
#include <PBWMUnGpData>
#include <PFCDeepGatedConSpec>
#include <LViLayerSpec>

#include <taMisc>

void PFCGateSpec::Initialize() {
  ctxt_decay = 0.0f;
  ctxt_decay_c = 1.0f - ctxt_decay;
  out_nogate_gain = 0.0f;
}

void PFCGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ctxt_decay_c = 1.0f - ctxt_decay;
}


void PFCLayerSpec::Initialize() {
  pfc_type = SNrThalLayerSpec::MNT;
  Defaults_init();
}

void PFCLayerSpec::Defaults_init() {
  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .5f;

  // SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  gp_kwta.pct = 0.15f;

  unit_gp_inhib.on = false;
  unit_gp_inhib.gp_g = 0.5f;
  unit_gp_inhib.diff_act_pct = true;
  unit_gp_inhib.pct_fm_frac = true;
  unit_gp_inhib.act_denom = 3.0f;
  unit_gp_inhib.UpdateAfterEdit_NoGui();

  // SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
}

void PFCLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gate.UpdateAfterEdit_NoGui();
}

void PFCLayerSpec::HelpConfig() {
  String help = "PFCLayerSpec Computation:\n\
 PFC maintenance functions just like LeabraTI, except updating of deep context\n\
 depends on BG Gating.\n\
 PFC output layers are gated in min..max cycle window in SNrThalLayerSpec.\n\
 \nPFCLayerSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure layers.\n\
 - Must recv SNrThalLayerSpec marker connection\n\
 - Maint layer must have LeabraTICtxtConSpec self projection (typically unit group based)\n\
 - Units must be organized into unit groups (stripes).";
  taMisc::Confirm(help);
}

bool PFCLayerSpec::CheckConfig_Layer(Layer* ly,  bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  net->dwt_norm_enabled = true; // required for learning modulation

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
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
//  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative

  LeabraLayer* snr_lay = SNrThalLayer(lay);
  if(lay->CheckError(!snr_lay, quiet, rval,
                     "no projection from SNrThal Layer found: must exist with MarkerConSpec connection")) {
    return false;
  }

  return true;
}

LeabraLayer* PFCLayerSpec::SNrThalLayer(LeabraLayer* lay) {
  int snr_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* snr_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  return snr_lay;
}

LeabraLayer* PFCLayerSpec::SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
						   int& n_in, int& n_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt) {
  snr_st_idx = 0;
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  snr_st_idx = snr_ls->SNrThalStartIdx(snr_lay, pfc_type, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
  return snr_lay;
}

void PFCLayerSpec::CopySNrThalGpData(LeabraLayer* lay, LeabraNetwork* net) {
  int snr_st_idx, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
  for(int g=0; g < lay->gp_geom.n; g++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(g);
    PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + g);
    gpd->CopyPBWMData(*snr_gpd);
  }
}

void PFCLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  CopySNrThalGpData(lay, net);
}

void PFCLayerSpec::Compute_OutGatedAct(LeabraLayer* lay, LeabraNetwork* net) {
  CopySNrThalGpData(lay, net);
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  if(!snr_lay) return;
  SNrThalLayerSpec* snrls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    bool just_fired = false;
    if(snrls->snrthal.out_at_p) {
      just_fired = (gpd->mnt_count == 1);
    }
    else {
      just_fired = gpd->go_fired_trial;
    }
    if(!just_fired) {  // reset activation after 
      for(int i=0;i<nunits;i++) {
        LeabraUnit* ru = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
        if(ru->lesioned()) continue;
        ru->act *= gate.out_nogate_gain;
        ru->act_lrn = ru->act_eq = ru->act_nd = ru->act;
        ru->da = 0.0f;            // I'm fully settled!
        ru->AddToActBuf(rus->syn_delay);
      }      
    }
  }
}

void PFCLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(pfc_type & SNrThalLayerSpec::OUTPUT) {
    Compute_OutGatedAct(lay, net);
  }
  inherited::Compute_CycleStats(lay, net);
}

void PFCLayerSpec::TI_ClearContext(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);
//  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    gpd->Init_State();          // restart
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
      if(u->lesioned()) continue;
      u->TI_ClearContext(net);
    }
  }
}

void PFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  // make sure we have all the gating info from SNrThal before we do our own guys
  if(net->phase_no == 1) {
    CopySNrThalGpData(lay, net);
  }
  inherited::PostSettle(lay, net);
}


