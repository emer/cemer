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

void CtDwtNorm::Initialize() {
  on = true;
  norm_pct = 1.0f;
}

void CtLeabraConSpec::Initialize() {
  min_obj_type = &TA_CtLeabraCon;
  savg_cor.thresh = -1.0f;
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
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

void CtLeabraUnitSpec::Init_Acts(LeabraUnit* ru, LeabraLayer*) {
  inherited::Init_Acts(ru);
  CtLeabraUnit* cu = (CtLeabraUnit*)ru;
  cu->cai_avg = 0.0f;
  cu->cai_max = 0.0f;
  cu->syndep_avg = 0.0f;
  cu->syndep_max = 0.0f;
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

void CtLeabraUnitSpec::Compute_SRAvg(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net) {
  for(int g=0; g<u->recv.size; g++) {
    CtLeabraRecvCons* recv_gp = (CtLeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_SRAvg(u, net, 1.0f); // sravg_wt = 1.0f
  }
  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_SRAvg((CtLeabraCon*)u->bias.Cn(0), u,
							 net, 1.0f);  // sravg_wt = 1.0f
  u->p_act_m = u->act_eq;	// snapshot at last sravg -- just for visual
}

void CtLeabraUnitSpec::Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  CtLeabraUnit* cu = (CtLeabraUnit*)u;
  // note: cannot do this because we need to clear out sravg for all connections regardless!!
//   if((cu->p_act_p <= opt_thresh.learn) && (cu->p_act_m <= opt_thresh.learn))
//     return;
  CtLeabraLayer* clay = (CtLeabraLayer*)lay;
  if(clay->sravg_sum > 0.0f) {	// had to have some sravg_sum at some point to learn..
    for(int g=0; g<u->recv.size; g++) {
      CtLeabraRecvCons* recv_gp = (CtLeabraRecvCons*)u->recv.FastEl(g);
      if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
      recv_gp->Compute_dWtCt(cu, (CtLeabraNetwork*)net);
    }
    ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_dWtCt((CtLeabraCon*)u->bias.Cn(0),
		   (CtLeabraUnit*)u, (CtLeabraLayer*)lay, (CtLeabraNetwork*)net);
  }
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
  maxda_sum = 0.0f;
  sravg_sum = 0.0f;
  sravg_cyc = 0;
}  

void CtLeabraLayer::Copy_(const CtLeabraLayer& cp) {
  maxda_sum = cp.maxda_sum;
  sravg_sum = cp.sravg_sum;
  sravg_cyc = cp.sravg_cyc;
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

void CtLeabraLayerSpec::Init_Weights(LeabraLayer* lay) {
  inherited::Init_Weights(lay);
  CtLeabraLayer* clay = (CtLeabraLayer*)lay;
  clay->mean_cai_max = 0.0f;
  clay->maxda_sum = 0.0f;
  clay->sravg_sum = 0.0f;
  clay->sravg_cyc = 0;
}

void CtLeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped)	return;	// say no more..
  inherited::Compute_Inhib(lay, net);
  
  Compute_CtDynamicInhib((CtLeabraLayer*)lay, (CtLeabraNetwork*)net);
}


void CtLeabraLayerSpec::Compute_CtDynamicInhib(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  float burst_i = 0.0f;
  float	inhib_i = 0.0f;
  if((net->ct_time.burst > 1) && (net->ct_cycle < net->ct_time.burst)) {
    burst_i = net->ct_inhib.burst_i *
      ((float)((net->ct_time.burst-1) - net->ct_cycle) / (float)(net->ct_time.burst-1));
  }
  if((net->ct_time.inhib_max >= 1) && (net->ct_cycle >= net->ct_time.inhib_start)) {
    float inhib_t = (float)(net->ct_cycle - net->ct_time.inhib_start);
    inhib_i = net->ct_inhib.inhib_i * (inhib_t / (float)net->ct_time.inhib_max);
    if(inhib_i > net->ct_inhib.inhib_i) inhib_i = net->ct_inhib.inhib_i;
  }
  // only one is going to be in effect at a time..
  lay->i_val.g_i += inhib_i * lay->i_val.g_i;
  lay->i_val.g_i -= burst_i * lay->i_val.g_i;

  if(inhib_group != ENTIRE_LAYER) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i += inhib_i * rugp->i_val.g_i;
      rugp->i_val.g_i -= burst_i * rugp->i_val.g_i;
    }
  }
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

void CtLeabraLayerSpec::Compute_SRAvg(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  lay->maxda_sum += lay->maxda;
  if((lay->maxda_sum >= net->ct_sravg.min_da_thr) && (lay->maxda < net->ct_sravg.max_da_thr)) {
    CtLeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
      u->Compute_SRAvg(lay, net);
    }
    lay->sravg_sum += 1.0f;	// add one to weighting factor
    lay->maxda_sum = 0.0f;
    lay->sravg_cyc = 0;
  }
  else {
    lay->sravg_cyc++;		// skipping
  }
}

void CtLeabraLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_dWt(lay, net);
  AdaptKWTAPt(lay, net);

  CtLeabraLayer* clay = (CtLeabraLayer*)lay;
  clay->maxda_sum = 0.0f;
  clay->sravg_sum = 0.0f;
  clay->sravg_cyc = 0;
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

void CtTrialTiming::Initialize() {
  minus = 40;
  plus = 40;
  inhib = 40;
  inhib_max = 40;
  burst = 20;

  sravg_start = 8;
  sravg_end = 10;

  syndep_int = 20;

  total_cycles = minus + plus + inhib;
  inhib_start = minus + plus;
}

void CtTrialTiming::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  total_cycles = minus + plus + inhib;
  inhib_start = minus + plus;
}

void CtInhibMod::Initialize() {
  inhib_i = 0.05f;
  burst_i = 0.05f;
}

void CtSRAvgSpec::Initialize() {
  min_da_thr = 0.005f;
  max_da_thr = 0.5f;
}

void CtLeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_CtLeabraLayer);
  ct_cycle = 0;
  cai_max = 0.0f;

  // set new defaults for some key things:
  phase_order = MINUS_PLUS_PLUS;
  maxda_stopcrit = -1.0f;
}

void CtLeabraNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ct_time.UpdateAfterEdit();
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
}

void CtLeabraNetwork::Compute_SRAvg() {
  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_SRAvg(this);
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
  
void CtLeabraNetwork::Compute_ActPAvgs() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ActPAvg(this);
  }
}

void CtLeabraNetwork::Compute_CtdWt() {
  // note: collect averages first so we can use those for filtering learning
  Compute_ActPAvgs();
  inherited::Compute_dWt();
}

void CtLeabraNetwork::Compute_dWt() {
  return;			// nop to prevent spurious
}

void CtLeabraNetwork::Compute_dWt_NStdLay() {
  // note: this is probably not even functional at this point..
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    if(lay->spec.SPtr()->GetTypeDef() != &TA_CtLeabraLayerSpec)
      lay->Compute_dWt();
  }
}

void CtLeabraNetwork::Compute_CtCycle() {
  if(phase_no == 0 && cycle == 0) // detect start of trial
    ct_cycle = 0;

  if(ct_cycle % ct_time.syndep_int == 0) { // time to do syndep
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

  if(ct_cycle == ct_time.inhib_start-1) {
    Compute_ActP();		// capture target activation state
  }

  if(train_mode != TEST) {	// for training mode only, do some learning
    if((ct_cycle >= ct_time.sravg_start) &&
       (ct_cycle < (ct_time.inhib_start + ct_time.sravg_end))) {
      // within sravg computation window
      Compute_SRAvg();
    }

    // at the end, do the weight update
    if(ct_cycle == ct_time.total_cycles-1) {
      Compute_CtdWt();
    }
  }

  ct_cycle++;
}

void CtLeabraNetwork::Cycle_Run() {
  inherited::Cycle_Run();
  Compute_CtCycle();
}

////////////////////////////////////////////////
//		TODO

// sravg modulated by rate of change:
// p_act_m is last value when sravg was computed
// if fabs(act_eq - p_act_m) > minus_thr, do sravg (need sravg_n per unit)
//    p_act_m = act_eq
// else incr counter 
// can plot ctr etc to see when plus phase is reliably detected.

// do the inhibition stuff!
