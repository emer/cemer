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

#include "leabra_ct.h"

// #include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////////////
// 	Ct cons
//////////////////////////////////

void CtCaDepSpec::Initialize() {
  intwt_dt = 0.02f;		// base per-cycle is .001
  ca_inc = .2f;			// base per-cycle is .01
  ca_dec = .2f;			// base per-cycle is .01
  sd_ca_thr = 0.2f;
  sd_ca_gain = 0.3f;
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CtCaDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CtLrateSpec::Initialize() {
  first_dwt = 1.0f;
  noth_dwt = 1.0f;
}

void CtDwtNorm::Initialize() {
  on = false;
  norm_pct = .5f;
}

void CtLeabraConSpec::Initialize() {
  min_obj_type = &TA_CtLeabraCon;
  savg_cor.thresh = -1.0f;
  lmix.hebb = 0.0001f;
  lmix.err = 0.9999f;
}

void CtLeabraConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ca_dep.UpdateAfterEdit();
}

void CtLeabraRecvCons::Initialize() {
  SetConType(&TA_CtLeabraCon);
  dwt_mean = 0.0f;
}

void CtLeabraRecvCons::Copy_(const CtLeabraRecvCons& cp) {
  dwt_mean = cp.dwt_mean;
}

void CtLeabraBiasSpec::Initialize() {
  //  min_obj_type = &TA_CtLeabraCon;
}

//////////////////////////////////
// 	Ct Unit
//////////////////////////////////

void CtLeabraUnit::Initialize() {
  bias.con_type = &TA_CtLeabraCon;
  cai_avg = 0.0f;
  cai_max = 0.0f;
  syndep_avg = 0.0f;
  syndep_max = 0.0f;
}

void CtLeabraUnitSpec::Initialize() {
  min_obj_type = &TA_CtLeabraUnit;
  bias_con_type = &TA_CtLeabraCon;
  bias_spec.SetBaseType(&TA_CtLeabraBiasSpec);

  opt_thresh.updt_wts = false;
}

void CtLeabraUnitSpec::Compute_CtCycle(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork* net) {
  CtLeabraConSpec* cspec = NULL;
  u->cai_avg = 0.0f;
  u->cai_max = 0.0f;
  for(int g=0; g<u->recv.size; g++) {
    CtLeabraRecvCons* recv_gp = (CtLeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_CtCycle(u, u->cai_avg, u->cai_max);
    cspec = (CtLeabraConSpec*)recv_gp->GetConSpec();
  }
  //  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_CtCycle((CtLeabraCon*)u->bias.Cn(0), u);
  if(u->n_recv_cons > 0)
    u->cai_avg /= (float)u->n_recv_cons;
  if(cspec) {
    u->syndep_avg = 1.0f - cspec->ca_dep.SynDep(u->cai_avg);
    u->syndep_max = 1.0f - cspec->ca_dep.SynDep(u->cai_max);
  }
}

void CtLeabraUnitSpec::Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  CtLeabraUnit* cu = (CtLeabraUnit*)u;
  if((cu->p_act_p <= opt_thresh.learn) && (cu->p_act_m <= opt_thresh.learn))
    return;

  for(int g=0; g<u->recv.size; g++) {
    CtLeabraRecvCons* recv_gp = (CtLeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_dWtCt((CtLeabraUnit*)u, (CtLeabraNetwork*)net);
  }
  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_dWtCt((CtLeabraCon*)u->bias.Cn(0),
					 (CtLeabraUnit*)u, (CtLeabraNetwork*)net);
  //  Compute_dWt_impl(u, lay, net);
}


void CtLeabraUnitSpec::Compute_dWtFlip(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork* net) {
  if((u->p_act_p <= opt_thresh.learn) && (u->p_act_m <= opt_thresh.learn))
    return;

  for(int g=0; g<u->recv.size; g++) {
    CtLeabraRecvCons* recv_gp = (CtLeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_dWtFlip(u, net);
  }
  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_dWtFlip((CtLeabraCon*)u->bias.Cn(0), u, net);
}

void CtLeabraUnitSpec::Compute_ActMP(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork*) {
  u->p_act_m = u->p_act_p;	// update activations for learning
  u->p_act_p = u->act_eq;
}

void CtLeabraUnitSpec::Compute_ActM(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork*) {
  u->p_act_m = u->act_eq;
}

void CtLeabraUnitSpec::Compute_ActP(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork*) {
  u->p_act_p = u->act_eq;
}

//////////////////////////////////
// 	Ct Layer
//////////////////////////////////

void CtLeabraLayer::Initialize() {
  spec.SetBaseType(&TA_CtLeabraLayerSpec);
  units.SetBaseType(&TA_CtLeabraUnit);
  unit_spec.SetBaseType(&TA_CtLeabraUnitSpec);
  mean_cai_max = 0.0f;
}  

void CtLeabraLayer::Copy_(const CtLeabraLayer& cp) {
  mean_cai_max = cp.mean_cai_max;
}

void CtLeabraLayerSpec::Initialize() {
  min_obj_type = &TA_CtLeabraLayer;
  decay.event = 0.0f;
  decay.phase = 0.0f;
}

bool CtLeabraLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);
  if(!rval) return false;
  if(CheckError(decay.event != 0.0f || decay.phase != 0.0f, quiet, rval,
		"decay.event or phase is not zero -- must be for continuous time learning to function properly",
		"I just set it for you in layer spec:", name,
		"(make sure this is appropriate for all layers that use this spec!)")) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
  }
     
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(us->opt_thresh.updt_wts, quiet, rval,
		"UnitSpec opt_thresh.updt_wts must be false to allow proper learning of all units",
		"I just set it for you in spec:", us->name,
		"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("opt_thresh", true);
    //    us->opt_thresh.learn = -1.0f;
    us->opt_thresh.updt_wts = false;
  }
  return rval;
}

void CtLeabraLayerSpec::Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  if(lay->hard_clamped) return;	// this is key!  clamped layers do not age!

  lay->mean_cai_max = 0.0f;
  CtLeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
    u->Compute_CtCycle(lay, net);
    lay->mean_cai_max += u->cai_max;
  }
  if(lay->units.leaves > 0)
    lay->mean_cai_max /= (float)lay->units.leaves;
}

void CtLeabraLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  // problem is: we don't have savg_cor.thresh here!
  // if either phase is below, don't go
//   if((lay->acts_m.avg < savg_cor.thresh) || (lay->acts_p.avg < savg_cor.thresh))
//     return;			
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_dWt(lay, net);
  AdaptKWTAPt(lay, net);
}

void CtLeabraLayerSpec::Compute_dWtFlip(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  // problem is: we don't have savg_cor.thresh here!
  // if either phase is below, don't go
//   if((lay->acts_m.avg < savg_cor.thresh) || (lay->acts_p.avg < savg_cor.thresh))
//     return;			
  CtLeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
    u->Compute_dWtFlip(lay, net);
  }
}

void CtLeabraLayerSpec::Compute_ActMP(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  CtLeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
    u->Compute_ActMP(lay, net);
  }
}

void CtLeabraLayerSpec::Compute_ActM(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  CtLeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
    u->Compute_ActM(lay, net);
  }
}

void CtLeabraLayerSpec::Compute_ActP(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  CtLeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
    u->Compute_ActP(lay, net);
  }
}

// NOTE: the following functions are NEVER called during normal usage in std leabra
// because postsettle just does a cp from the current acts values, instead of recomputing
// from scratch. thus, we can co-opt these functions

void CtLeabraLayerSpec::Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr,
					  LeabraNetwork*) {
  thr->acts_m.avg = 0.0f;
  thr->acts_m.max = -FLT_MAX;
  thr->acts_m.max_i = -1;
  CtLeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(CtLeabraUnit, u, ug->, i) {
    thr->acts_m.avg += u->p_act_m; // NOTE: using p_act_m instead of act_m here!!
    if(u->p_act_m > thr->acts_m.max) {
      thr->acts_m.max = u->p_act_m;  thr->acts_m.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_m.avg /= (float)ug->leaves;
}

void CtLeabraLayerSpec::Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_p.avg = 0.0f;
  thr->acts_p.max = -FLT_MAX;
  thr->acts_p.max_i = -1;
  CtLeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(CtLeabraUnit, u, ug->, i) {
    thr->acts_p.avg += u->p_act_p;  // NOTE: using p_act_p instead of act_p here!!
    if(u->p_act_p > thr->acts_p.max) {
      thr->acts_p.max = u->p_act_p;  thr->acts_p.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_p.avg /= (float)ug->leaves;
}

//////////////////////////////////
// 	Ct Network
//////////////////////////////////

void CtNetLearnSpec::Initialize() {
  mode = CT_NOTHING_SYNC;
  interval = 20;
  noth_dwt_int = 10;
  syndep_int = 20;
  noth_trg_first = false;
}

void CtLeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_CtLeabraLayer);
  cai_max = 0.0f;
  ct_lrn_time = 0.0f;
  ct_lrn_cycle = 0;
  ct_lrn_now = 0;

  // set new defaults for some key things:
  phase_order = MINUS_PLUS_NOTHING;
  maxda_stopcrit = -1.0f;
}

void CtLeabraNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void CtLeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_CtLeabraCon;
  prjn->recvcons_type = &TA_CtLeabraRecvCons;
  prjn->sendcons_type = &TA_LeabraSendCons;
  prjn->con_spec.SetBaseType(&TA_CtLeabraConSpec);
}

void CtLeabraNetwork::Init_Counters() {
  inherited::Init_Counters();
  ct_cycle = 0;
}

void CtLeabraNetwork::Init_Stats() {
  inherited::Init_Stats();
  cai_max = 0.0f;
  ct_lrn_time = 0.0f;
  ct_lrn_cycle = 0;
  ct_lrn_now = 0;
}

void CtLeabraNetwork::Compute_CtCycle() {
  if(ct_cycle % ct_learn.syndep_int == 0) {
    cai_max = 0.0f;
    int nmax = 0;
    CtLeabraLayer* lay;
    taLeafItr l;
    FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
      if(lay->lesioned())	continue;
      lay->Compute_CtCycle(this);
      if(lay->layer_type == Layer::HIDDEN) {
	cai_max += lay->mean_cai_max;
	nmax++;
      }
    }
    if(nmax > 0)
      cai_max /= (float)nmax;
  }

  ct_cycle++;			// increment now, prior to learning -- learning uses

  switch(ct_learn.mode) {
  case CtNetLearnSpec::CT_MANUAL:
    break;
  case CtNetLearnSpec::CT_NOTHING_SYNC:
    CtLearn_NothingSync();
    break;
  }
}

// Note: 2632 has all the bump detection code, etc

// TODO: impl attenuating sensory responses to inputs instead of NOTHING
// phase stuff -- could produce more realistic dynamics -- novelty filter etc..

void CtLeabraNetwork::CtLearn_NothingSync() {
  ct_lrn_now = 0;
  if(phase_no == 0 && cycle == 0) {
    Compute_ActMP();
    ct_lrn_now = 1;		// indicates update without actual learning
    ct_cycle = 1;		// time starts now -- 1 is to prevent learn interval right now
    return;
  }

  if(nothing_phase) {
    if(cycle == ct_learn.noth_dwt_int) {
      Compute_ActMP();				      // encode new values
      ct_lrn_now = -2;				      // indicates flip actual learning
      Compute_CtdWtFlip();
    }
    return;			// otherwise no processing!
  }

  if(ct_cycle % ct_learn.interval != 0) return; // not time to learn

  Compute_ActMP();
  ct_lrn_now = 2;				      // indicates actual learning
  if(train_mode != TEST) {	// always just do wt update
    Compute_CtdWt();
  }
  // todo: explore a version that does update weight right then and there ?
}

void CtLeabraNetwork::Compute_ActMPAvgs() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ActMAvg(this);
    lay->Compute_ActPAvg(this);
  }
}

void CtLeabraNetwork::Compute_CtdWt() {
  // note: collect averages first so we can use those for filtering learning
  Compute_ActMPAvgs();
  inherited::Compute_dWt();
}

void CtLeabraNetwork::Compute_dWt() {
  return;			// nop to prevent spurious
}

void CtLeabraNetwork::Compute_dWt_NStdLay() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    if(lay->spec.SPtr()->GetTypeDef() != &TA_CtLeabraLayerSpec)
      lay->Compute_dWt();
  }
}

void CtLeabraNetwork::Compute_CtdWtFlip() {
  // note: collect averages first so we can use those for filtering learning
  Compute_ActMPAvgs();

  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_dWtFlip(this);
  }
  if(ct_learn.noth_trg_first) {
    TargExtToComp();		// all external input is now 'comparison'
    Compute_HardClamp();	// recompute -- should turn everything off
    Compute_NetinScale();	// and then compute net scaling
    Send_ClampNet();		// and send net from clamped inputs
  }
}
  
void CtLeabraNetwork::Cycle_Run() {
  inherited::Cycle_Run();
  Compute_CtCycle();
}

void CtLeabraNetwork::Compute_ActMP() {
  // assume we're learning now..
  ct_lrn_time = time;
  ct_lrn_cycle = ct_cycle;
  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ActMP(this);
  }
}

void CtLeabraNetwork::Compute_ActM() {
  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ActM(this);
  }
}
void CtLeabraNetwork::Compute_ActP() {
  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ActP(this);
  }
}
  
void CtLeabraNetwork::Settle_Init_Decay() {
  if(phase_no >= 3) { // second plus phase or more: use phase2..
    DecayPhase2();
  }
  else if(phase_no == 2) {
    DecayPhase2();		// decay before 2nd phase set
    if(phase_order == MINUS_PLUS_NOTHING) {
      if(!ct_learn.noth_trg_first)
	TargExtToComp();		// all external input is now 'comparison'
    }
  }
  else if(phase_no == 1) {
    if(phase_order == PLUS_NOTHING) { // actually a nothing phase
      DecayPhase2();
      if(!ct_learn.noth_trg_first)
	TargExtToComp();		// all external input is now 'comparison'
    }
    else
      DecayPhase();		// prepare for next phase
  }
}	

////////////////////////////////////////////////
//		TODO

