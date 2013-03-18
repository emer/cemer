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
  nogo_inhib = 0.2f;
  pvr_inhib = 0.0f;
  refract_inhib = 0.0f;
}

/////////////////////////////////////////////////////

void MatrixLayerSpec::Initialize() {
  gating_type = SNrThalLayerSpec::IN_MNT;
  go_nogo = GO;

  Defaults_init();
}

void MatrixLayerSpec::Defaults_init() {
  // todo: sync with above
  matrix.nogo_inhib = 0.0f; // 0.2f;
  matrix.da_gain = 0.1f;

  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .5f;

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

  int snridx = lay->own_net->layers.FindLeafEl(snr_lay);
  lay->CheckError(snridx > myidx, quiet, rval,
                  "SNrThallayer (SNrThalLayerSpec) layer must be *before* this layer in list of layers -- it is now after, won't work!");

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

LeabraLayer* MatrixLayerSpec::SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
					      int& n_in, int& n_in_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt) {
  snr_st_idx = 0;
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  snr_st_idx = snr_ls->SNrThalStartIdx(snr_lay, gating_type, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
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
  case SNrThalLayerSpec::IN_MNT:
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
  PBWMUnGpData* nogo_gpd = (PBWMUnGpData*)nogo_lay->ungp_data.FastEl(gpidx);
  float nogo_i = matrix.nogo_inhib * nogo_gpd->acts.avg;
  if(nogo_i > 1.0f) nogo_i = 1.0f;
  return nogo_i;
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
  // todo: this will be off by one trial now...
  if(matrix.pvr_inhib == 0.0f) return 0.0f;
  float pvr_i = 0.0f;
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  if(er_avail) {
    if((gating_type != SNrThalLayerSpec::OUTPUT) && (gating_type != SNrThalLayerSpec::OUT_MNT))
      pvr_i = matrix.pvr_inhib;
  }
  else {
    if((gating_type == SNrThalLayerSpec::OUTPUT) || (gating_type == SNrThalLayerSpec::OUT_MNT))
      pvr_i = matrix.pvr_inhib;
  }
  return pvr_i;
}

void MatrixLayerSpec::Compute_NetinMods(LeabraLayer* lay, LeabraNetwork* net) {
  if(go_nogo != GO) return;     // only Go gets mods

  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  for(int gi=0; gi<lay->gp_geom.n; gi++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gi);

    float nogo_i = Compute_NoGoInhibGo_ugp(lay, acc_md, gi, net);
    float refract_i = Compute_RefractInhib_ugp(lay, acc_md, gi, net);
    float pvr_i = Compute_PVrInhib_ugp(lay, acc_md, gi, net);

    gpd->nogo_inhib = nogo_i;
    gpd->refract_inhib = refract_i;
    gpd->pvr_inhib = pvr_i;
      
    float mult_fact = 1.0f;
    if(nogo_i > 0.0f) mult_fact *= (1.0f - nogo_i);
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

void MatrixLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_NetinMods(lay, net);
  inherited::Compute_NetinStats(lay, net);
}

void MatrixLayerSpec::Compute_ZeroGatingAct_ugp(LeabraLayer* lay,
                                                Layer::AccessMode acc_md, int gpidx,
                                                LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->act_eq = u->act_p = 0.0f;
  }
}

void MatrixLayerSpec::Compute_GatingActs_ugp(LeabraLayer* lay,
                                       Layer::AccessMode acc_md, int gpidx,
                                       LeabraNetwork* net) {
  int snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();

  PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + gpidx);
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  gpd->CopyPBWMData(*snr_gpd);	// always grab from snr
  if(!snr_gpd->go_fired_trial) {
    Compute_ZeroGatingAct_ugp(lay, acc_md, gpidx, net); // zero our act values
  }
}

void MatrixLayerSpec::Compute_GatingActs(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int gpidx=0; gpidx<lay->gp_geom.n; gpidx++) {
    Compute_GatingActs_ugp(lay, acc_md, gpidx, net);
  }
}

// this is called at end of plus phase, to establish a da value for driving learning

void MatrixLayerSpec::Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net) {
  // float lay_ton_da = lay->GetUserDataAsFloat("tonic_da");

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

void MatrixLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == 1) {
    // end of plus -- compute da value used for learning
    Compute_GatingActs(lay, net);
    Compute_LearnDaVal(lay, net);
  }
}


// note: MatrixUnitSpec has Compute_Weights which only calls on pv trials
