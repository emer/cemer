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
  in_mnt = 1;
  out_mnt = 0;
  maint_pct = 0.9f;
  maint_decay = 0.02f;
  maint_thr = 0.2f;
  clear_decay = 0.0f;

  maint_pct_c = 1.0f - maint_pct;
}

void PFCGateSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  maint_pct_c = 1.0f - maint_pct;
}

void PFCLayerSpec::Initialize() {
  pfc_type = SNrThalLayerSpec::IN_MNT;
  pfc_layer = SUPER;

  Defaults_init();
}

void PFCLayerSpec::Defaults_init() {
  //  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .5f;

  // SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  gp_kwta.pct = 0.25f;

  unit_gp_inhib.on = true;
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
 The PFC deep layers are gated by thalamocortical prjns from SNrThal.\n\
 They maintain activation over time (activation-based working memory) via \
 excitatory intracelluar ionic mechanisms (implemented in hysteresis channels, gc.h),\
 and excitatory connections with superficial pfc layers, which is toggled by SNrThal.\n\
 Updating occurs by mid_minus_cycle, based on SNrThal act_mid activations.\n\
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

//     if((pfc_type != SNrThalLayerSpec::OUTPUT) || (pfc_type != SNrThalLayerSpec::OUT_MNT)) {
//       LeabraLayer* lve_lay = LVeLayer(lay);
//       if(lay->CheckError(!lve_lay, quiet, rval,
// 			 "LVe layer not found -- PFC deep layers must project to LVe")) {
//       }
//     }
    if(pfc_type == SNrThalLayerSpec::IN_MNT) {
      LeabraLayer* lve_lay = LVeLayer(lay);
      if(lay->CheckError(!lve_lay, quiet, rval,
			 "LVe layer not found -- PFCd_in_mnt deep layers must project to LVe")) {
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
  int snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
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
    u->act_mid = u->act_eq;
  }
}

void PFCLayerSpec::Clear_Maint(LeabraLayer* lay, LeabraNetwork* net, int stripe_no) {
  if(pfc_layer != DEEP) return;
  if(lay->lesioned()) return;
  Compute_MaintUpdt(lay, net, CLEAR, stripe_no);
}

void PFCLayerSpec::Compute_MaintUpdt(LeabraLayer* lay, LeabraNetwork* net,
				     MaintUpdtAct updt_act, int stripe_no) {
  if(pfc_layer != DEEP) return;
  if(lay->lesioned()) return;
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
						   int& n_in, int& n_in_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt) {
  snr_st_idx = 0;
  LeabraLayer* snr_lay = SNrThalLayer(lay);
  SNrThalLayerSpec* snr_ls = (SNrThalLayerSpec*)snr_lay->GetLayerSpec();
  snr_st_idx = snr_ls->SNrThalStartIdx(snr_lay, pfc_type, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
  return snr_lay;
}

void PFCLayerSpec::Compute_MaintUpdt_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
				 int gpidx, MaintUpdtAct updt_act, LeabraNetwork* net) {

  int snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
  PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gpidx);
  PBWMUnGpData* snr_gpd = (PBWMUnGpData*)snr_lay->ungp_data.FastEl(snr_st_idx + gpidx);
  int nunits = lay->UnitAccess_NUnits(acc_md);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();

  for(int j=0;j<nunits;j++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
    if(u->lesioned()) continue;
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(0);
    LeabraUnit* super_u = (LeabraUnit*)recv_gp->Un(0);
    switch(updt_act) {
    case STORE: {
      u->vcb.g_h = u->maint_h = super_u->act_eq; // note: store current superficial act
      super_u->vcb.g_h = super_u->maint_h = super_u->act_eq;
      break;
    }
    case CLEAR: {
      u->vcb.g_h = u->maint_h = 0.0f;
      super_u->vcb.g_h = super_u->maint_h = 0.0f;
      gpd->mnt_count = -1;	// indication of empty
      snr_gpd->mnt_count = -1;
      break;
    }
    case CLEAR_DECAY: {
      u->v_m -= gate.clear_decay * (u->v_m - us->v_m_init.mean);
      u->net -= gate.clear_decay * u->net;
      
      super_u->v_m -= gate.clear_decay * (super_u->v_m - us->v_m_init.mean);
      super_u->net -= gate.clear_decay * super_u->net;
      break;
    }
    case DECAY: {
      u->maint_h -= u->maint_h * gate.maint_decay;
      if(u->maint_h < 0.0f) u->maint_h = 0.0f;
      u->vcb.g_h = u->maint_h;
      super_u->vcb.g_h = super_u->maint_h = u->maint_h;
      break;
    }
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
      float dact = 0.0f;
      if(gpd->mnt_count >= 0) {
	LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(0);
	LeabraUnit* super_u = (LeabraUnit*)recv_gp->Un(0);
	dact = super_u->act;
      }
      u->act = u->act_eq = u->act_nd = dact;
      u->da = 0.0f;
    }
  }
  // super is now done exclusively in unit spec!
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
  net->init_netins_cycle_stat = true; // call net->Init_Netins() when done..
}

void PFCLayerSpec::Compute_Gating(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;

  int snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt;
  LeabraLayer* snr_lay = SNrThalStartIdx(lay, snr_st_idx, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
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
	if(net->ct_cycle == gate_cycle) {
	  if(gpd->prv_mnt_count > 0)	  // currently maintaining, clear act
	    Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR_DECAY, net);
	}
	Compute_MaintUpdt_ugp(lay, acc_md, mg, STORE, net);
      }
    }
    
    if(pfc_layer == SUPER && gpd->go_fired_trial) {
      if(net->ct_cycle == gate_cycle) {
	GateOnDeepPrjns_ugp(lay, acc_md, mg, net); // gate on the deep prjns
      }
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
  if((pfc_type == SNrThalLayerSpec::IN_MNT) || (pfc_type == SNrThalLayerSpec::MNT_OUT) || (pfc_type == SNrThalLayerSpec::OUT_MNT)) return;	// no clear
  if(pfc_layer != DEEP) return;

  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
//     if(pfc_type == SNrThalLayerSpec::INPUT && gpd->mnt_count == gate.in_mnt) {
//       Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);
    if(pfc_type == SNrThalLayerSpec::INPUT && gpd->mnt_count >= gate.in_mnt) {
      Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);  
    }
//     if(pfc_type == SNrThalLayerSpec::OUTPUT && gpd->mnt_count == gate.out_mnt) {
//       Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);
    if(pfc_type == SNrThalLayerSpec::OUTPUT && gpd->mnt_count >= gate.out_mnt) {
      Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);  
    }
  }
}

void PFCLayerSpec::Compute_FinalGating(LeabraLayer* lay, LeabraNetwork* net) {
  if(pfc_layer != DEEP) return;
  if((pfc_type != SNrThalLayerSpec::IN_MNT) && (pfc_type != SNrThalLayerSpec::MNT_OUT) && (pfc_type != SNrThalLayerSpec::OUT_MNT)) {
    Compute_ClearNonMnt(lay, net); 
    return;
  }

  Layer::AccessMode acc_md = Layer::ACC_GP;
  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);

    if(gpd->mnt_count >= 1) {
      if(gpd->acts.max < gate.maint_thr) { // below thresh, nuke!
	Compute_MaintUpdt_ugp(lay, acc_md, mg, CLEAR, net);
      }
      else {			// continue to decay..
	Compute_MaintUpdt_ugp(lay, acc_md, mg, DECAY, net);
      }
    }
  }
}

void PFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(net->phase_no == 1) {
    Compute_FinalGating(lay, net);     // final gating
  }
}


