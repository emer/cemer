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

#include "MatrixLayerSpec.h"
#include <LeabraNetwork>
#include <PBWMUnGpData>
#include <MatrixUnitSpec>
#include <MatrixConSpec>
#include <MarkerConSpec>
#include <PVLVDaLayerSpec>
#include <PFCLayerSpec>

#include <taMisc>

void MatrixGoNogoGainSpec::Initialize() {
  on = false;
  go_p = go_n = nogo_p = nogo_n = 1.0f;
}

void MatrixMiscSpec::Initialize() {
  da_gain = 0.1f;
  nogo_inhib = 0.5f;
  refract_inhib = 0.0f;
  nogo_deep_gain = 0.5f;
}

/////////////////////////////////////////////////////

void MatrixLayerSpec::Initialize() {
  gating_type = SNrThalLayerSpec::MNT;
  go_nogo = GO;

  Defaults_init();
}

void MatrixLayerSpec::Defaults_init() {
  // todo: sync with above
  matrix.nogo_inhib = 0.5f;
  matrix.da_gain = 0.1f;
  matrix.nogo_deep_gain = 0.5f;

  // todo: update these to FF_FB defaults!
  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;

  //  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;

  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  //  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;

  // new default..
  //  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = 0.15f;
  unit_gp_inhib.on = true;
  unit_gp_inhib.gp_g = 1.0f;
  lay_gp_inhib.on = false;      // off now by default!
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
 - Units must have a MatrixUnitSpec and must recv some kind of dopamine signal in dav\n\
 - Must recv from SNrThalLayerSpec to get final Go signal\n\
 - Go layer recv marker cons from NoGo, gets inhibition in proportion to avg NoGo act\n\
 - Other Recv conns are MatrixConSpec for learning based on the da-signal.\n\
 - Units must be organized into groups (stripes) of same number as PFC";
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

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "layer must have unit_groups = true (= stripes) (multiple are good for independent searching of gating space!")) {
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

  int myidx = lay->own_net->layers.FindLeafEl(lay);

  // be more flexible about where we get the dav from
  // if(lay->CheckError(da_lay == NULL, quiet, rval,
  //               "Could not find DA layer (PVLVDaLayerSpec) -- must receive MarkerConSpec projection from one!")) {
  //   return false;
  // }
  if(da_lay) {
    // this is required so we can modulate the dav value by go vs. nogo
    int daidx = lay->own_net->layers.FindLeafEl(da_lay);
    lay->CheckError(daidx > myidx, quiet, rval,
                    "DA layer (PVLVDaLayerspec) layer must be *before* this layer in list of layers -- it is now after, won't work!");
  }

  if(lay->CheckError(snr_lay == NULL, quiet, rval,
                "Could not find SNrThal layer -- must receive MarkerConSpec projection from one!")) {
    return false;
  }

  int snridx = lay->own_net->layers.FindLeafEl(snr_lay);
  lay->CheckError(snridx > myidx, quiet, rval,
                  "SNrThallayer (SNrThalLayerSpec) layer must be *before* this layer in list of layers -- it is now after, won't work!");

  if(go_nogo == GO) {
    if(nogo_lay) {
      // nogo_lay also optional
      if(lay->CheckError(nogo_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
                         "NoGo Matrix layer does not have same number of stripes as we do!")) {
        return false;
      }
    }
  }

  return true;
}

LeabraLayer* MatrixLayerSpec::SNrThalLayer(LeabraLayer* lay) {
  int snr_prjn_idx = 0; // actual arg value doesn't matter
  return FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
}

LeabraLayer* MatrixLayerSpec::SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
					      int& n_in, int& n_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt) {
  snr_st_idx = 0;
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  snr_st_idx = snr_ls->SNrThalStartIdx(snr_lay, gating_type, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
  return snr_lay;
}

void MatrixLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  lay->SetUserData("tonic_da", 0.0f); // store tonic da per layer
  // NameMatrixUnits(lay, net);
}

void MatrixLayerSpec::NameMatrixUnits(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  String nm;
  switch(gating_type) {
  case SNrThalLayerSpec::INPUT:
    nm = "i";
    break;
  case SNrThalLayerSpec::MNT:
    nm = "m";
    break;
  case SNrThalLayerSpec::OUTPUT:
    nm = "o";
    break;
  case SNrThalLayerSpec::MNT_OUT:
    nm = "mo";
    break;
  case SNrThalLayerSpec::OUT_MNT:
    nm = "om";
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

float MatrixLayerSpec::Compute_NoGoInhibGo_ugp(LeabraLayer* lay,
					       Layer::AccessMode acc_md, int gpidx,
					       LeabraNetwork* net) {
  if(matrix.nogo_inhib == 0.0f) return 0.0f;

  int nogo_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* nogo_lay = FindLayerFmSpec(lay, nogo_prjn_idx, &TA_MatrixLayerSpec);
  if(nogo_lay) {                // optional
    PBWMUnGpData* nogo_gpd = (PBWMUnGpData*)nogo_lay->ungp_data.FastEl(gpidx);
    float nogo_i = matrix.nogo_inhib * nogo_gpd->acts.avg;
    if(nogo_i > 1.0f) nogo_i = 1.0f;
    return nogo_i;
  }
  else {
    return 0.0f;
  }
}

float MatrixLayerSpec::Compute_RefractInhib_ugp(LeabraLayer* lay,
						Layer::AccessMode acc_md, int gpidx,
						LeabraNetwork* net) {
  if(matrix.refract_inhib == 0.0f) return 0.0f;
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  if(gpd->mnt_count == 1) {	// only true if gated last trial
    // NOTE: mnt_count is updated at start of trial in SNrThalLayerSpec, and we grab
    // it every cycle, so this value should be current at all points
    return matrix.refract_inhib;
  }
  return 0.0f;
}

void MatrixLayerSpec::Compute_GoNetinMods(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);

    float nogo_i = Compute_NoGoInhibGo_ugp(lay, acc_md, gi, net);
    float refract_i = Compute_RefractInhib_ugp(lay, acc_md, gi, net);

    gpd->nogo_inhib = nogo_i; // this is for stats and monitoring purposes
    gpd->refract_inhib = refract_i; // this is for stats and monitoring purposes
      
    float mult_fact = 1.0f;
    if(nogo_i > 0.0f) mult_fact *= (1.0f - nogo_i);
    if(refract_i > 0.0f) mult_fact *= (1.0f - refract_i);

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

float MatrixLayerSpec::Compute_NoGoDeepGain_ugp(LeabraLayer* lay,
					       Layer::AccessMode acc_md, int gpidx,
					       LeabraNetwork* net) {
  if(matrix.nogo_deep_gain == 0.0f) return 0.0f;

  int pfc_prjn_idx = 0; // actual arg value doesn't matter
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_PFCLayerSpec);
  if(!pfc_lay) return 0.0f;
  Projection* prjn = lay->projections.SafeEl(pfc_prjn_idx);
  if(!prjn) return 0.0f;
  if(!prjn->con_spec->InheritsFrom(&TA_MarkerConSpec)) return 0.0f;
  PBWMUnGpData* pfc_gpd = (PBWMUnGpData*)pfc_lay->ungp_data.FastEl(gpidx);
  float nogo_deep = matrix.nogo_deep_gain * pfc_gpd->acts_ctxt.avg;
  return nogo_deep;
}

void MatrixLayerSpec::Compute_NoGoNetinMods(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);

    float nogo_deep = Compute_NoGoDeepGain_ugp(lay, acc_md, gi, net);

    gpd->nogo_deep = nogo_deep;
      
    float mult_fact = 1.0f;
    if(nogo_deep > 0.0f) mult_fact *= (1.0f + nogo_deep);

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

void MatrixLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(go_nogo == GO) {
    Compute_GoNetinMods(lay, net);
  }
  else {
    Compute_NoGoNetinMods(lay, net);
  }
  inherited::Compute_NetinStats(lay, net);
}

void MatrixLayerSpec::Compute_PreGatingAct_ugp(LeabraLayer* lay,
                                               Layer::AccessMode acc_md, int gpidx,
                                               LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_mid = 0.0f;
    u->act_m2 = 0.0f;
  }
}

void MatrixLayerSpec::Compute_NoGatingZeroAct_ugp(LeabraLayer* lay,
                                                  Layer::AccessMode acc_md, int gpidx,
                                                  LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_mid = 0.0f;
    u->act_lrn = u->act = u->act_nd = u->act_eq = 0.0f;
    u->da = 0.0f;
  }
}

void MatrixLayerSpec::Compute_NoGatingRecAct_ugp(LeabraLayer* lay,
                                                 Layer::AccessMode acc_md, int gpidx,
                                                 LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_m2 = u->act_eq;
  }
}

void MatrixLayerSpec::Compute_GoGatingAct_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_mid = u->act_eq;
    u->act_m2 = u->act_eq;       // also grab this

    // now save the sending unit activations at time of gating
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(cs->InheritsFrom(&TA_MatrixConSpec)) {
        ((MatrixConSpec*)cs)->Compute_NTr(recv_gp, u, net);
      }
    }
  }
}

void MatrixLayerSpec::Compute_ShowGatingAct_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraNetwork* net) {
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_lrn = u->act = u->act_nd = u->act_eq = u->act_mid;
    u->da = 0.0f;
    // NOTE: mnt_count is updated at start of trial in SNrThalLayerSpec, and we grab
    // it every cycle, so this value should be current at all points
    if(gpd->mnt_count > 0) {
      u->SetUnitFlag(Unit::LEARN); // signals that we are maintaining this trial from prior gating -- affects rate of decay of trace in MatrixConSpec
    }
    else {
      u->ClearUnitFlag(Unit::LEARN); // signals that we are NOT maintaining..
    }
  }
}

void MatrixLayerSpec::Compute_GatingActs_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraNetwork* net) {
  int snr_st_idx, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_mnt, n_mnt_out, n_out, n_out_mnt);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();

  PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + gpidx);
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  gpd->CopyPBWMData(*snr_gpd);	// always grab from snr
  if(net->ct_cycle < snr_ls->snrthal.min_cycle) {
    Compute_PreGatingAct_ugp(lay, acc_md, gpidx, net); // zero out guys before window
  }
  else if(snr_gpd->go_fired_trial) {
    if(snr_gpd->go_fired_now) {
      Compute_GoGatingAct_ugp(lay, acc_md, gpidx, net); // capture gating time values
    }
    else if(net->ct_cycle > snr_ls->snrthal.max_cycle) {
      Compute_ShowGatingAct_ugp(lay, acc_md, gpidx, net); // show what happened
    }
  }
  else {
    if(net->ct_cycle > snr_ls->snrthal.max_cycle) {
      Compute_NoGatingZeroAct_ugp(lay, acc_md, gpidx, net); // zero our act values
    }
    else if(net->ct_cycle >= snr_ls->snrthal.min_cycle) {
      Compute_NoGatingRecAct_ugp(lay, acc_md, gpidx, net); // record to act_m2
    }
  }
}

void MatrixLayerSpec::Compute_GatingActs(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int gpidx=0; gpidx<lay->gp_geom.n; gpidx++) {
    Compute_GatingActs_ugp(lay, acc_md, gpidx, net);
  }
}

void MatrixLayerSpec::Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net) {
  // float lay_ton_da = lay->GetUserDataAsFloat("tonic_da");
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good

  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    if(go_nogo == NOGO) {
      for(int i=0;i<nunits;i++) {
	LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, i, gi);
	if(u->lesioned()) continue;
        u->dav *= -matrix.da_gain; // inverting the da at this point -- uses same learning rule as GO otherwise
        if(go_nogo_gain.on) {
          if(u->dav >= 0.0f)
            u->dav *= go_nogo_gain.nogo_p;
          else
            u->dav *= go_nogo_gain.nogo_n;
        }
      }
    }
    else {			// GO
      for(int i=0;i<nunits;i++) {
	LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, i, gi);
	if(u->lesioned()) continue;
        u->dav *= matrix.da_gain;
        if(go_nogo_gain.on) {
          if(u->dav >= 0.0f)
            u->dav *= go_nogo_gain.go_p;
          else
            u->dav *= go_nogo_gain.go_n;
        }
      }
    }
  }
}

void MatrixLayerSpec::Compute_CycleStats(LeabraLayer* lay,  LeabraNetwork* net) {
  Compute_GatingActs(lay, net);
  Compute_LearnDaVal(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void MatrixLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // nop -- don't do the default thing -- already done by call to MidMinusAct
}
