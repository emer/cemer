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
  intwt_dt = 0.001f;
  ca_inc = .01f;
  ca_dec = .01f;
  sd_ca_thr = 0.2f;
  sd_ca_gain = 0.3f;
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CtCaDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CtLeabraConSpec::Initialize() {
  min_obj_type = &TA_CtLeabraCon;
  savg_cor.thresh = -1.0f;
  sym_dif = false;
}

void CtLeabraConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ca_dep.UpdateAfterEdit();
}

void CtLeabraRecvCons::Initialize() {
  SetConType(&TA_CtLeabraCon);
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
  opt_thresh.learn = -1.0f;
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
    cspec->sym_dif = net->ct_learn.sym_dif; // todo: not very efficient -- quick and dirty..
  }
  //  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_CtCycle((CtLeabraCon*)u->bias.Cn(0), u);
  if(u->n_recv_cons > 0)
    u->cai_avg /= (float)u->n_recv_cons;
  if(cspec) {
    u->syndep_avg = 1.0f - cspec->ca_dep.SynDep(u->cai_avg);
    u->syndep_max = 1.0f - cspec->ca_dep.SynDep(u->cai_max);
  }
}

void CtLeabraUnitSpec::Compute_dWtFlip(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork* net) {
  for(int g=0; g<u->recv.size; g++) {
    CtLeabraRecvCons* recv_gp = (CtLeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_dWtFlip(u);
  }
  // todo?:
  //  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_dWtFlip((CtLeabraCon*)u->bias.Cn(0), u);
}

void CtLeabraUnitSpec::Compute_ActMP(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork*) {
  u->act_m2 = u->p_act_m;	// double-back just for testing sym_dif learning
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

void CtLeabraLayerSpec::Compute_dWtFlip(CtLeabraLayer* lay, CtLeabraNetwork* net) {
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


//////////////////////////////////
// 	Ct Network
//////////////////////////////////

void CtNetLearnSpec::Initialize() {
  mode = CT_NOTHING_SYNC;
  interval = 5;
  noth_dwt_int = 10;
  first_cyc_st = 0;
  syndep_int = 1;
  sym_dif = false;
}

void CtLeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_CtLeabraLayer);
  cai_max = 0.0f;
  ct_lrn_time = 0.0f;
  ct_lrn_cycle = 0;
  ct_lrn_now = 0;
  phase_order = MINUS_PLUS_NOTHING;
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
  if(phase_no == 0 && cycle <= ct_learn.first_cyc_st) {
    if(cycle == ct_learn.first_cyc_st) {
      // start of new input phase -- auto update to new vals
      Compute_ActMP();
      if(ct_learn.sym_dif)	// do it again to propagate all the way back..
	Compute_ActMP();
      ct_lrn_now = 1;		// indicates update without actual learning
      ct_cycle = 1;		// time starts now -- 1 is to prevent learn interval right now
    }
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

void CtLeabraNetwork::Compute_CtdWt() {
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
      lay->Compute_dWt(this);
  }
}

void CtLeabraNetwork::Compute_CtdWtFlip() {
  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_dWtFlip(this);
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
  

////////////////////////////////////////////////
//		TODO

