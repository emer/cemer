// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.




#include <cs/cs.h>
#include <pdp/enviro_extra.h>
#include <ta/tdefaults.h>


//////////////////////////
//  	Con, Spec	//
//////////////////////////

void CsConSpec::Initialize() {
  min_obj_type = &TA_CsCon_Group;
  min_con_type = &TA_CsCon;
  wt_limits.sym = true;		// default is to have symmetric initial weights!
  lrate = .01f;
  momentum = 0.0f;
  momentum_c = 1.0f;
  decay = 0.0f;
  decay_fun = NULL;
}

void CsConSpec::UpdateAfterEdit() {
  ConSpec::UpdateAfterEdit();
  momentum_c = 1.0f - momentum;
}

void Cs_Simple_WtDecay(CsConSpec* spec, CsCon* cn, Unit*, Unit*) {
  cn->dwt -= spec->decay * cn->wt;
}

void Cs_WtElim_WtDecay(CsConSpec* spec, CsCon* cn, Unit*, Unit*) {
  float denom = (1.0f + (cn->wt * cn->wt));
  cn->dwt -= spec->decay * ((2.0f * cn->wt * cn->wt) / (denom * denom));
}


//////////////////////////
//  	Unit, Spec	//
//////////////////////////

void CsUnitSpec::Initialize() {
  min_obj_type = &TA_CsUnit;
  bias_con_type = &TA_CsCon;
  bias_spec.SetBaseType(&TA_CsConSpec);
  act_range.min = -1.0f;
  act_range.max = 1.0f;
  real_range.min = -.98f;
  real_range.max = .98f;
  noise.type = Random::NONE;
  noise.var = .1f;
  step = .05f;
  sqrt_step = sqrt(step);
  gain = 1.0f;
  clamp_type = HARD_FAST_CLAMP;
  clamp_gain = 5.0f;
  initial_act.type = Random::NONE;
  modify_decay = 0.5f;
  use_annealing = false;
  use_sharp = false;
}

void CsUnitSpec::InitLinks() {
  UnitSpec::InitLinks();
  taBase::Own(real_range, this);
  taBase::Own(noise_sched, this);
  taBase::Own(gain_sched, this);
}

void CsUnitSpec::UpdateAfterEdit() {
  UnitSpec::UpdateAfterEdit();
  sqrt_step = sqrt(step);
}

void CsUnitSpec::InitState(Unit* u) {
  UnitSpec::InitState(u);
  CsUnit* cu = (CsUnit*)u;
  cu->da = 0.0f;
  cu->prv_net = 0.0f;
  cu->clmp_net = 0.0f;
  cu->act = act_range.Clip(initial_act.Gen());
}

void CsUnitSpec::ModifyState(Unit* u) {
  u->InitExterns();
  u->InitDelta();
  CsUnit* cu = (CsUnit*)u;
  float trgact = act_range.Clip(initial_act.Gen());
  cu->act -= modify_decay * (cu->act - trgact);
  cu->da = 0.0f;
  //  cu->prv_net -= modify_decay * cu->prv_net;
  cu->prv_net = 0.0f;
  cu->clmp_net = 0.0f;
}


void CsUnitSpec::InitWtState(Unit* u) {
  UnitSpec::InitWtState(u);
  CsUnit* cu = (CsUnit*)u;
  cu->n_dwt_aggs = 0; // initialize it here
  cu->act_m = cu->act_p = 0.0f;
}

void CsUnitSpec::Compute_ClampAct(CsUnit* u) {
  if((clamp_type != HARD_FAST_CLAMP) || !(u->ext_flag & Unit::EXT))
    return;
  u->act = real_range.Clip(u->ext);	// keep everything in range
  u->da = 0.0f;
}

void CsUnitSpec::Compute_ClampNet(CsUnit* u) {
  u->clmp_net = 0.0f;
  if(clamp_type != HARD_FAST_CLAMP)
    return;
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
    Layer* lay = recv_gp->prjn->from;
    if(lay->lesion || !(lay->ext_flag & Unit::EXT))
      continue;
    u->clmp_net += recv_gp->Compute_Net(u);
  }
}

void CsUnitSpec::Compute_Net(Unit* u) {
  CsUnit* cu = (CsUnit*)u;
  if(clamp_type == HARD_FAST_CLAMP) {
    if(cu->ext_flag & Unit::EXT)
      return;
    cu->net = 0.0f;
    Con_Group* recv_gp;
    int g;
    FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
      Layer* lay = recv_gp->prjn->from;
      if(lay->lesion || (lay->ext_flag & Unit::EXT)) // don't add from clamped!
	continue;
      u->net += recv_gp->Compute_Net(u);
    }
    u->net += u->bias->wt;
  }
  else if(clamp_type == HARD_CLAMP) {
    if(cu->ext_flag & Unit::EXT) // no point in computing net for clamped units!
      return;
    UnitSpec::Compute_Net(u);
  }
  else {			// soft clamping (or soft-then-hard), always compute net
    UnitSpec::Compute_Net(u);
  }
  cu->net += cu->clmp_net; // add in clamped input
}

void CsUnitSpec::Compute_Act(Unit* u, int cycle, int phase) {
  CsUnit* cu = (CsUnit*)u;
  if(u->ext_flag & Unit::EXT) {	// receiving external input
    if(clamp_type == HARD_FAST_CLAMP) // already processed this one!
      return;
    ClampType ct = clamp_type;
    if(clamp_type == SOFT_THEN_HARD_CLAMP) {
      if(phase == CsTrial::MINUS_PHASE)
	ct = SOFT_CLAMP;
      else
	ct = HARD_CLAMP;
    }
    if(ct == HARD_CLAMP) {
      cu->act = real_range.Clip(cu->ext);	// keep everything in range
      cu->da = 0.0f;
      if(noise.type != Random::NONE) { // can add noise to the inputs
	float noise_anneal = 1.0f;
	if((cycle != -1) && use_annealing)
	  noise_anneal = noise_sched.GetVal(cycle);
	u->act += sqrt_step * noise_anneal * noise.Gen();
      }
      // don't call Compute_Act_impl
    }
    else {			// soft clamp
      cu->net += clamp_gain * cu->ext;
      Compute_Act_impl(cu, cycle, phase); // go ahead and compute activation
    }
  }
  else {
    Compute_Act_impl(cu, cycle, phase); // go ahead and compute activation
  }

  if(cu->act >= real_range.max) {
    cu->act = real_range.max;
    cu->da = 0.0f;		// don't count pegged units..
  }
  else if(cu->act <= real_range.min) {
    cu->act = real_range.min;
    cu->da = 0.0f;		// ditto..
  }
}

void CsUnitSpec::Aggregate_dWt(Unit* u, int phase) {
  CsUnit* cu = (CsUnit*)u;
  ((CsConSpec*)bias_spec.spec)->B_Aggregate_dWt((CsCon*)cu->bias, cu, phase);
  CsCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(CsCon_Group, recv_gp, cu->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      recv_gp->Aggregate_dWt(cu,phase);
  }
  cu->n_dwt_aggs++;
}

void CsUnitSpec::PostSettle(CsUnit* u, int phase) {
  if(phase == CsTrial::MINUS_PHASE)
    u->act_m = u->act;
  else
    u->act_p = u->act;
}

void CsUnitSpec::Compute_dWt(Unit* u) {
  CsUnit* cu = (CsUnit*)u;
  UnitSpec::Compute_dWt(cu);
  ((CsConSpec*)bias_spec.spec)->B_Compute_dWt((CsCon*)cu->bias, cu);
  cu->n_dwt_aggs = 0;		// reset after wts are aggd
}

void CsUnitSpec::UpdateWeights(Unit* u) {
  UnitSpec::UpdateWeights(u);
  ((CsConSpec*)bias_spec.spec)->B_UpdateWeights((CsCon*)u->bias, u);
}

void CsUnitSpec::GraphActFun(GraphLog* graph_log, float min, float max, int ncycles) {
  if(graph_log == NULL) {
    graph_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
    if(graph_log == NULL) return;
  }
  graph_log->name = name + ": Act Fun";
  DataTable* dt = &(graph_log->data);
  dt->Reset();
  dt->NewColFloat("netin");
  dt->NewColFloat("act");

  CsUnit un;

  float x;
  for(x = min; x <= max; x += .01f) {
    un.act = act_range.min + .5f * act_range.range;
    un.net = x;
    int cy;
    for(cy=0;cy<ncycles;cy++) {
      Compute_Act_impl(&un, 0, 0);
    }
    dt->AddBlankRow();
    dt->SetLastFloatVal(x, 0);
    dt->SetLastFloatVal(un.act, 1);
  }
  dt->UpdateAllRanges();
  graph_log->ViewAllData();
}

void CsUnit::Initialize() {
  da = act_p = act_m = clmp_net = prv_net = 0.0f;
  n_dwt_aggs = 0;
  spec.SetBaseType(&TA_CsUnitSpec);
}

void CsUnit::InitLinks() {
  Unit::InitLinks();
}

void CsUnit::Targ_To_Ext() {
  if(ext_flag & Unit::TARG) {
    SetExtFlag(Unit::EXT);
    ext = targ;
  }
}


////////////////////////////
//  Activation Functions  //
////////////////////////////

// default is inverse-logistic
void CsUnitSpec::Compute_Act_impl(CsUnit* u, int cycle, int) {
  float	noise_anneal = 1.0f;	// like temp
  float	gain_sharp = 1.0f;	// sharpening
  
  if(cycle != -1) {
    if(use_annealing)
      noise_anneal = noise_sched.GetVal(cycle);
    if(use_sharp)
      gain_sharp = gain_sched.GetVal(cycle);
  }

  float x = (u->act - act_range.min) / (act_range.max - u->act);
  u->da = u->net - (logf(x) / (gain_sharp * gain)); // inverse logistic
  u->act += step * u->da + sqrt_step * noise_anneal * noise.Gen();
}

void SigmoidUnitSpec::Initialize() {
  step = .2f;
  sqrt_step = sqrtf(step);
  time_avg = NET_INPUT;
}

void SigmoidUnitSpec::Compute_Act_impl(CsUnit* u, int cycle, int) {
  float	noise_anneal = 1.0f;
  float	gain_sharp = 1.0f;

  if(cycle != -1) {
    if(use_annealing)
      noise_anneal = noise_sched.GetVal(cycle);
    if(use_sharp)
      gain_sharp = gain_sched.GetVal(cycle);
  }

  if(time_avg == NET_INPUT) {
    u->da = u->net - u->prv_net; // change is on net inputs
    u->net = u->prv_net + step * u->da;
    u->prv_net = u->net;
    float new_act = act_range.Project(1.0f / (1.0f + expf(-gain * gain_sharp * u->net)));
    u->act = new_act + sqrt_step * noise_anneal * noise.Gen();
  }
  else {
    float new_act = act_range.Project(1.0f / (1.0f + expf(-gain * gain_sharp * u->net)));
    u->da = new_act - u->act;
    u->act += step * u->da + sqrt_step * noise_anneal * noise.Gen();
  }
}

void BoltzUnitSpec::Initialize() {
  temp = 1.0f;
}

void BoltzUnitSpec::UpdateAfterEdit() {
  CsUnitSpec::UpdateAfterEdit();
  gain = 1.0f / temp;		// keep gain and temp aligned
}

void BoltzUnitSpec::Compute_Act_impl(CsUnit* u, int cycle, int) {
  float	temp_anneal = 1.0f;

  if(cycle != -1) {
    if(use_annealing)
      temp_anneal = noise_sched.GetVal(cycle);
  }

  float prob = (1.0f / (1.0f + expf((-1.0 / (temp_anneal * temp)) * u->net)));
  float nw_act = (Random::ZeroOne() < prob) ? act_range.max : act_range.min;
  u->da = nw_act - u->act;
  u->act = nw_act;
}

void IACUnitSpec::Initialize() {
  step = .1f;
  sqrt_step = sqrtf(step);
  rest = 0.0f;
  decay = .2f;
  gain = 5.0f;
  use_send_thresh = false;
  send_thresh = 0.0f;
}

void IACUnitSpec::UpdateAfterEdit() {
  CsUnitSpec::UpdateAfterEdit();
  gain = 1.0f / decay;		// keep gain and decay aligned
}

void IACUnitSpec::Send_Net(Unit* u, Layer* tolay) {
  if(u->act > send_thresh) {	// send to others
    if((clamp_type == HARD_FAST_CLAMP) || (clamp_type == HARD_CLAMP)) {
      if((clamp_type == HARD_FAST_CLAMP) && (u->ext_flag & Unit::EXT))
	return; // we have already sent!
      Con_Group* send_gp;
      int g;
      FOR_ITR_GP(Con_Group, send_gp, u->send., g) {
	Layer* lay = send_gp->prjn->layer;
	if(lay->lesion || (lay->ext_flag & Unit::EXT) || (lay != tolay)) // do not send to clamped!
	  continue;
	send_gp->Send_Net(u);
      }
    }
    else {			// soft clamping, always compute net
      Con_Group* send_gp;
      int g;
      FOR_ITR_GP(Con_Group, send_gp, u->send., g) {
	Layer* lay = send_gp->prjn->layer;
	if(lay->lesion || (lay != tolay))    continue;
	send_gp->Send_Net(u);
      }
    }
  }
}

void IACUnitSpec::Compute_Act_impl(CsUnit* u, int cycle, int) {
  float	noise_anneal = 1.0f;
  if(cycle != -1) {
    if(use_annealing)
      noise_anneal = noise_sched.GetVal(cycle);
  }
  if(u->net > 0.0f)
    u->da = (u->net * (act_range.max - u->act)) - (decay * (u->act - rest));
  else
    u->da = (u->net * (u->act - act_range.min)) - (decay * (u->act - rest));
  u->act += step * u->da + sqrt_step * noise_anneal * noise.Gen();
}

void LinearCsUnitSpec::Initialize() {
  SetUnique("act_range", true);
  act_range.min = -1e20;
  act_range.max = 1e20;
}

void LinearCsUnitSpec::Compute_Act_impl(CsUnit* u, int cycle, int) {
  float	noise_anneal = 1.0f;
  if(cycle != -1) {
    if(use_annealing)
      noise_anneal = noise_sched.GetVal(cycle);
  }
  // do netin-based time-averaging
  u->da = u->net - u->prv_net; // change is on net inputs
  u->net = u->prv_net + step * u->da;
  u->prv_net = u->net;
  u->act = u->net + sqrt_step * noise_anneal * noise.Gen();
}

void ThreshLinCsUnitSpec::Initialize() {
  SetUnique("act_range", true);
  act_range.min = -1e20;
  act_range.max = 1e20;
  threshold = 0.0f;
}

void ThreshLinCsUnitSpec::Compute_Act_impl(CsUnit* u, int cycle, int) {
  float	noise_anneal = 1.0f;
  if(cycle != -1) {
    if(use_annealing)
      noise_anneal = noise_sched.GetVal(cycle);
  }
  // do netin-based time-averaging
  u->da = u->net - u->prv_net; // change is on net inputs
  u->net = u->prv_net + step * u->da;
  u->prv_net = u->net;
  float new_act = (u->net > threshold) ? u->net - threshold : 0.0f;
  u->act = new_act + sqrt_step * noise_anneal * noise.Gen();
}



////////////////////////////////
// 	Processes             //
////////////////////////////////

////////////////////////////////
// 	   CsCycle            //
////////////////////////////////

void CsCycle::Initialize() {
  update_mode = SYNCHRONOUS;
  cs_settle = NULL;
  cs_trial = NULL;
  sub_proc_type = NULL;
  n_updates = 10;

  min_unit = &TA_CsUnit;
  min_con_group = &TA_CsCon_Group;
  min_con = &TA_CsCon;
}

void CsCycle::CutLinks() {
  CycleProcess::CutLinks();
  cs_settle = NULL;
  cs_trial = NULL;
}

void CsCycle::Copy_(const CsCycle& cp) {
  update_mode = cp.update_mode;
  n_updates = cp.n_updates;
}

void CsCycle::UpdateAfterEdit() {
  cs_settle = (CsSettle*) FindSuperProc(&TA_CsSettle);
  if((cs_settle == NULL) && (GET_MY_OWNER(TypeDefault) == NULL)) 
    taMisc::Error("CsCycle Process: ",name," does not have a parent",
		  "CsSettle process and will most likely crash\n",
		  "Please delete this object and recreate this process",
		  "as a subprocess of a CsSettle process");
  cs_trial = (CsTrial*)  FindSuperProc(&TA_CsTrial);
  if((cs_settle == NULL) && (GET_MY_OWNER(TypeDefault) == NULL)) 
    taMisc::Error("CsCycle Process: ", name," does not have a parent",
		  "CsTrial process and will most likely crash\n",
		  "Please delete and recreate this process as a subprocess",
		  "of a CsSettle process which is subprocess of a CsTrial");
#ifdef DMEM_COMPILE
  if((update_mode == ASYNCHRONOUS) && (taMisc::dmem_nprocs > 1)) {
    taMisc::Error("CsCycle Error: cannot use ASYNCHRONOUS update_mode with distributed memory computation (np > 1)");
  }
#endif

  CycleProcess::UpdateAfterEdit();
}

void CsCycle::Compute_SyncAct() {
  // netinput must already be computed in separate step
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    CsUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(CsUnit, u, lay->units., u_itr) {
      u->Compute_Act(cs_settle->cycle.val, cs_trial->phase);
    }
  }
}

void CsCycle::Compute_AsyncAct() {
  if(cs_settle->n_units == 0) return; // error check
  Layer* lay;
  taLeafItr l;
  for (int i=0; i < n_updates; i++) {	// do this n_updates times
    int rnd_num = Random::IntZeroN(cs_settle->n_units);
    int total_prob = 0;
    FOR_ITR_EL(Layer,lay,network->layers.,l) {
      if(lay->lesion)	continue;
      int lay_prob = lay->units.leaves;   // each layer gets prob in prop. to # units
      if(rnd_num < (lay_prob + total_prob)) {
	int whichunitleaf = rnd_num - total_prob;
	CsUnit* u = (CsUnit*) lay->units.Leaf(whichunitleaf);	
	u->Compute_Net();
	u->Compute_Act(cs_settle->cycle.val, cs_trial->phase); // update this one unit...
	break;
      } else {
	total_prob += lay_prob;
      }
    }
  }
}

void CsCycle::Aggregate_dWt() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    CsUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(CsUnit, u, lay->units., u_itr) {
      u->Aggregate_dWt(cs_trial->phase);
    }
  }
}

void CsCycle::Loop() {
  if(update_mode == SYNCHRONOUS) {
    network->Compute_Net();	// two-stage update of nets and acts
    Compute_SyncAct();
  }
  else if(update_mode == SYNC_SENDER_BASED) {
    network->InitDelta();	// initialize netin values
    network->Send_Net();	// send the netinput
    Compute_SyncAct();
  }
  else
    Compute_AsyncAct();

  if((cs_trial == NULL) || (cs_trial->epoch_proc == NULL))
    return;
  
  if(!cs_settle->deterministic &&
     (cs_settle->cycle.val >= cs_settle->start_stats)
     && !(cs_trial->epoch_proc->wt_update == EpochProcess::TEST))
    Aggregate_dWt();
}

////////////////////////////////
// 	   CsSettle           //
////////////////////////////////

void CsSettle::Initialize() {
  sub_proc_type = &TA_CsCycle;
  cs_trial = NULL;
  between_phases = INIT_STATE;
  cycle.SetMax(100);
  start_stats = 50;
  deterministic = false;

  min_unit = &TA_CsUnit;
  min_con_group = &TA_CsCon_Group;
  min_con = &TA_CsCon;
}

void CsSettle::UpdateAfterEdit() {
  cs_trial = (CsTrial*) FindSuperProc(&TA_CsTrial);

  SettleProcess::UpdateAfterEdit();
}

void CsSettle::CutLinks() {
  SettleProcess::CutLinks();
  cs_trial = NULL;
}	

void CsSettle::InitLinks() {
  SettleProcess::InitLinks();
  if(deterministic && !taMisc::is_loading && (loop_stats.size == 0))
    loop_stats.NewEl(1, &TA_CsMaxDa);
}

void CsSettle::Compute_ClampAct() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, network->layers., l) {
    if(lay->lesion || !(lay->ext_flag & Unit::EXT)) // only clamped layers
      continue;
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u)
      un->Compute_ClampAct();
  }
}

void CsSettle::Compute_ClampNet() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, network->layers., l) {
    if(lay->lesion || (lay->ext_flag & Unit::EXT)) // only non-clamped layers
      continue;
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u)
      un->Compute_ClampNet();
  }
}

void CsSettle::Compute_NUnits() {
  n_units = 0;
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer,lay,network->layers.,l) {
    if(!lay->lesion) {
      n_units += lay->units.leaves;  //find total for async update
    }
  }
}
  
void CsSettle::Targ_To_Ext() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, network->layers., l) {
    if(lay->lesion || !(lay->ext_flag & Unit::TARG))
      continue;
    lay->SetExtFlag(Unit::EXT);
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u)
      un->Targ_To_Ext();	// convert to ext..
  }
}

void CsSettle::PostSettle() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, network->layers., l) {
    if(!lay->lesion) {
      CsUnit* un;
      taLeafItr u;
      FOR_ITR_EL(CsUnit, un, lay->units., u)
	un->PostSettle(cs_trial->phase);
    }
  }
}

void CsSettle::Aggregate_dWt() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    CsUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(CsUnit, u, lay->units., u_itr) {
      u->Aggregate_dWt(cs_trial->phase);
    }
  }
}

void CsSettle::Init_impl() {
  SettleProcess::Init_impl();
  if(network == NULL) return;
  if(((CsCycle*)sub_proc)->update_mode == CsCycle::ASYNCHRONOUS) {
    Compute_NUnits();
  }

  if(cs_trial->phase == CsTrial::PLUS_PHASE) {
    if(between_phases == INIT_STATE)
      network->InitState();
    else
      network->InitExterns();
  }

  if((cs_trial != NULL) && (cs_trial->cur_event != NULL)) {
    Event* ev = cs_trial->cur_event;
    if(cs_trial->phase == CsTrial::PLUS_PHASE) {
      if(ev->spec.spec->InheritsFrom(TA_ProbEventSpec))
	((ProbEventSpec*) ev->spec.spec)->ApplySamePats(ev,network); // be sure to get same one
      else
	ev->ApplyPatterns(network);
      Targ_To_Ext();
    }
    // use time to set duration of event presentation..
    if(ev->InheritsFrom(TA_TimeEvent))
      cycle.SetMax((int)((TimeEvent*)ev)->time);
    else if(ev->InheritsFrom(TA_DurEvent))
      cycle.SetMax((int)((DurEvent*)ev)->duration);
  }
  // precompute clamping info so cycles aren't wasted during settling
  Compute_ClampAct();
  Compute_ClampNet();
}

void CsSettle::LoopStats() {
  if(!deterministic && (cycle.val < start_stats))	// otherwise do the loop stats..
    return;
  SettleProcess::LoopStats();
}

void CsSettle::Final() {
  if((cs_trial == NULL) || (cs_trial->epoch_proc == NULL))
    return;

  if(deterministic && !(cs_trial->epoch_proc->wt_update == EpochProcess::TEST)) {
    Aggregate_dWt();
  }
  PostSettle();
}
  
////////////////////////////////
// 	   CsTrial           //
////////////////////////////////

void CsTrial::Initialize() {
  sub_proc_type = &TA_CsSettle;
  phase = MINUS_PHASE;
  phase_no.SetMax(2);
  trial_init = INIT_STATE;
  no_plus_stats = true;
  no_plus_test = true;

  min_unit = &TA_CsUnit;
  min_con_group = &TA_CsCon_Group;
  min_con = &TA_CsCon;
}

void CsTrial::InitLinks() {
  TrialProcess::InitLinks();
  taBase::Own(phase_no, this);
  if(!taMisc::is_loading && (loop_stats.size == 0)) {
    SE_Stat* st = (SE_Stat*)loop_stats.NewEl(1, &TA_SE_Stat);
    st->CreateAggregates(Aggregate::SUM);
  }
}

void CsTrial::CutLinks() {
  TrialProcess::CutLinks();
}

void CsTrial::Copy_(const CsTrial& cp) {
  phase_no = cp.phase_no;
  phase = cp.phase;
  trial_init = cp.trial_init;
  no_plus_stats = cp.no_plus_stats;
  no_plus_test = cp.no_plus_test;
}

void CsTrial::UpdateAfterEdit() {
  TrialProcess::UpdateAfterEdit();
}

void CsTrial::Init_impl() {
  TrialProcess::Init_impl();

  phase_no.SetMax(2);
  phase = MINUS_PHASE;

  if(no_plus_test && (epoch_proc != NULL) && 
     (epoch_proc->wt_update == EpochProcess::TEST))
  {
    phase_no.SetMax(1);		// just do one loop (the minus phase)
  }

  if(trial_init == INIT_STATE)
    network->InitState();
  else if(trial_init == MODIFY_STATE)
    network->ModifyState();
  else
    network->InitExterns();

  if((cur_event == NULL) || (network == NULL))
     return;
  cur_event->ApplyPatterns(network);
}

bool CsTrial::Crit() {
  return SchedProcess::Crit();	// train proc defines an always-true crit..
}

void CsTrial::C_Code() {
  bool stop_crit = false;	// a stopping criterion was reached
  bool stop_force = false;	// either the Stop button was pressed or our Step level was reached

  if(re_init) {
    Init();
    InitProcs();
  }

  if((cur_event != NULL) && (cur_event->InheritsFrom(TA_DurEvent)) &&
     (((DurEvent*)cur_event)->duration <= 0)) {
    SetReInit(true);
    return;
  }

  do {
    if(no_plus_test && (phase == PLUS_PHASE) && (epoch_proc != NULL) &&
       (epoch_proc->wt_update == EpochProcess::TEST))
      break;
    Loop();			// user defined code goes here
    if(!bailing) {
      UpdateCounters();
      LoopProcs();		// check/run loop procs(mod based on counter)
      if(!no_plus_stats || (phase == MINUS_PHASE)) {
	LoopStats();		// update in-loop stats
	if(log_loop)
	  UpdateLogs();		// generate log output
      }
      UpdateState();		// update process state vars

      stop_crit = Crit();   	// check if stopping criterion was reached
      if(!stop_crit) {		// if at critera, going to quit anyway, so don't
	stop_force = StopCheck(); // check for stopping (either by Stop button or Step)
      }
    }
  }
  while(!bailing && !stop_crit && !stop_force);
  // loop until we reach criterion (e.g. ctr > max) or are forcibly stopped

  if(stop_crit) {
    Final();
    FinalProcs();
    FinalStats();
    if(!log_loop)
      UpdateLogs();
    UpdateDisplays();		// update displays after the loop
    SetReInit(true);		// made it through the loop
    FinalStepCheck();		// always stop at end if i'm the step process
  }
  else {
    bailing = true;
  }
}

void CsTrial::UpdateState() {
  TrialProcess::UpdateState();
  phase = PLUS_PHASE;
}

void CsTrial::GetCntrDataItems() {
  if(cntr_items.size < 2)
    cntr_items.EnforceSize(2);
  TrialProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetNarrowName("Phase");
  it->is_string = true;
}

void CsTrial::GenCntrLog(LogData* ld, bool gen) {
  TrialProcess::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 2)
      GetCntrDataItems();
    if(phase == MINUS_PHASE)
      ld->AddString(cntr_items.FastEl(1), "-");
    else
      ld->AddString(cntr_items.FastEl(1), "+");
  }
}

void CsTrial::Compute_dWt() {
  network->Compute_dWt();
}  

void CsTrial::Final() {
  TrialProcess::Final();	// do anything else

  // the CsSample process will compute the weight changes if it is there..
  if((super_proc != NULL) && super_proc->InheritsFrom(&TA_CsSample))
    return;

  if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST))
    Compute_dWt();
}


// this will also have cs-level checks on it..
bool CsTrial::CheckUnit(Unit* ck) {
  bool rval = TrialProcess::CheckUnit(ck);
  if(!rval) return rval;
  CsUnitSpec* usp = (CsUnitSpec*)ck->spec.spec;
  // check if using IAC send thresh
  if(usp == NULL)	return true;
  CsCycle* cyc = (CsCycle*)FindSubProc(&TA_CsCycle);
  if(cyc == NULL)	return true;
  if(usp->InheritsFrom(TA_IACUnitSpec) && ((IACUnitSpec*)usp)->use_send_thresh) {
    if(cyc->update_mode != CsCycle::SYNC_SENDER_BASED) {
      taMisc::Error("Using IACUnitSpec send_thresh without CsCycle::SYNC_SENDER_BASED does not work:",
		    "switching to that update_mode");
      cyc->update_mode = CsCycle::SYNC_SENDER_BASED;
    }
  }
  else {
    if(cyc->update_mode == CsCycle::SYNC_SENDER_BASED) {
      taMisc::Error("Using normal receiver based netin with CsCycle::SYNC_SENDER_BASED is slow:",
		    "switching to SYNCHRONOUS");
      cyc->update_mode = CsCycle::SYNCHRONOUS;
    }
  }
  return true;
}

bool CsTrial::CheckNetwork() {
  if(network && (network->dmem_sync_level != Network::DMEM_SYNC_NETWORK)) {
    network->dmem_sync_level = Network::DMEM_SYNC_NETWORK;
  }
  return TrialProcess::CheckNetwork();
}


////////////////////////////////
// 	    CsSample          //
////////////////////////////////

void CsSample::Initialize() {
  sub_proc_type = &TA_CsTrial;
  cur_event = NULL;
  sample.SetMax(1);

  min_unit = &TA_CsUnit;
  min_con_group = &TA_CsCon_Group;
  min_con = &TA_CsCon;
}

void CsSample::InitLinks() {
  TrialProcess::InitLinks();
  taBase::Own(sample, this);
}

// this will be the first process with cs-level checks on it..
bool CsSample::CheckUnit(Unit* ck) {
  bool rval = TrialProcess::CheckUnit(ck);
  if(!rval) return rval;
  CsUnitSpec* usp = (CsUnitSpec*)ck->spec.spec;
  // check if using IAC send thresh
  if(usp == NULL)	return true;
  CsCycle* cyc = (CsCycle*)FindSubProc(&TA_CsCycle);
  if(cyc == NULL)	return true;
  if(usp->InheritsFrom(TA_IACUnitSpec) && ((IACUnitSpec*)usp)->use_send_thresh) {
    if(cyc->update_mode != CsCycle::SYNC_SENDER_BASED) {
      taMisc::Error("Using IACUnitSpec send_thresh without CsCycle::SYNC_SENDER_BASED does not work:",
		    "switching to that update_mode");
      cyc->update_mode = CsCycle::SYNC_SENDER_BASED;
    }
  }
  else {
    if(cyc->update_mode == CsCycle::SYNC_SENDER_BASED) {
      taMisc::Error("Using normal receiver based netin with CsCycle::SYNC_SENDER_BASED is slow:",
		    "switching to SYNCHRONOUS");
      cyc->update_mode = CsCycle::SYNCHRONOUS;
    }
  }
  return true;
}

void CsSample::Init_impl() {
  TrialProcess::Init_impl();
}

void CsSample::Compute_dWt() {
  network->Compute_dWt();
}  

void CsSample::Final() {
  TrialProcess::Final();	// do anything else

  if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST))
    Compute_dWt();
}


////////////////////////////////
// 	   CsMaxDa            //
////////////////////////////////

void CsMaxDa::Initialize() {
  min_unit = &TA_CsUnit;
  da.stopcrit.flag = true;	// defaults
  da.stopcrit.val = .01f;
  da.stopcrit.cnt = 1;
  da.stopcrit.rel = CritParam::LESSTHANOREQUAL;
  has_stop_crit = true;
  settle = NULL;
  net_agg.op = Aggregate::MAX;
  loop_init = INIT_START_ONLY;
}

void CsMaxDa::InitLinks() {
  Stat::InitLinks();
  settle = GET_MY_OWNER(SettleProcess);
}

void CsMaxDa::CutLinks() {
  Stat::CutLinks();
  settle = NULL;
}

void CsMaxDa::Copy_(const CsMaxDa& cp) {
  da = cp.da;
}

void CsMaxDa::InitStat() {
  da.InitStat(InitStatVal());
  InitStat_impl();
}

void CsMaxDa::Init() {
  da.Init();
  Init_impl();
}

bool CsMaxDa::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return da.Crit();
}

void CsMaxDa::Network_Init() {
  InitStat();
}

void CsMaxDa::Unit_Stat(Unit* unit) {
  float fda = fabsf(((CsUnit*)unit)->da);
  net_agg.ComputeAgg(&da, fda);
}

void CsMaxDa::Network_Stat() {
  if((settle != NULL) && (settle->cycle.val < 5)) // don't stop before 5 cycles..
    da.val = MAX(da.val, da.stopcrit.val);
}


////////////////////////////////
// 	   CsDistStat         //
////////////////////////////////

void CsDistStat::Initialize() {
  cs_settle = NULL;
  tolerance = 0.25f;
  n_updates = 0;
  loop_init = INIT_START_ONLY;		// don't initialize in the loop!!
}

void CsDistStat::InitLinks() {
  Stat::InitLinks();

  cs_settle = GET_MY_OWNER(CsSettle);
}

void CsDistStat::CutLinks() {
  Stat::CutLinks();
  cs_settle = NULL;
}

void CsDistStat::Copy_(const CsDistStat& cp) {
  probs = cp.probs;
  act_vals = cp.act_vals;
  tolerance = cp.tolerance;
  n_updates = cp.n_updates;
}

void CsDistStat::InitStat() {
  probs.InitStat(InitStatVal());
  InitStat_impl();
}

void CsDistStat::Init() {
  act_vals.Reset();
  n_updates = 0;

  if(cs_settle == NULL) {
    probs.Init();
    Init_impl();
    return;
  }
  Event* ev = cs_settle->cs_trial->cur_event;
  if(ev == NULL) {
    probs.Init();
    Init_impl();
    return;
  }
  ProbEventSpec* es = (ProbEventSpec*)ev->spec.spec;

  int num_targets = 0;
  taLeafItr psitr;
  PatternSpec* ps;
  FOR_ITR_EL(PatternSpec, ps, es->patterns., psitr) {
    if(ps->type == PatternSpec::TARGET)
      num_targets++;
  }

  if(probs.size != num_targets) {
    probs.EnforceSize(num_targets);
    NameStatVals();
  }

  probs.Init();
  Init_impl();
}

bool CsDistStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return probs.Crit();
}

void CsDistStat::Network_Stat() {
  if(cs_settle == NULL) return;
  Event* ev = cs_settle->cs_trial->cur_event;
  if(ev == NULL) return;
  ProbEventSpec* es = (ProbEventSpec*)ev->spec.spec;

  Layer* cur_lay = NULL;

  taLeafItr pitr,psitr;
  Pattern* p;
  PatternSpec* ps;
  int statval_index = 0;
  
  FOR_ITR_PAT_SPEC(Pattern, p, ev->patterns., pitr,
		   PatternSpec, ps, es->patterns., psitr)
  {
    if(ps->type == PatternSpec::TARGET) {
      if((ps->layer != NULL) && (ps->layer != cur_lay)) {
      	act_vals.Reset();
	Unit* u;
	taLeafItr uitr;
	cur_lay = ps->layer;
	FOR_ITR_EL(Unit, u, cur_lay->units., uitr) {
	  act_vals.Add(u->act);
	}
      }
      float within_tol = 0.0f;
      if(p->value.SumSquaresDist(act_vals, false, tolerance) == 0.0f)
	within_tol = 1.0f;
      StatVal* sv = (StatVal*)probs[statval_index];
      sv->val =	((sv->val * (float) n_updates) + within_tol)
		     / (float) (n_updates + 1);
      statval_index++;
    }
  }
  n_updates++;
}

void CsDistStat::CreateAggregates(Aggregate::Operator default_op) {
  if(own_proc == NULL)
    return;

  SchedProcess* sproc= (SchedProcess *) own_proc->super_proc;
  Stat*		stat_to_agg = this;

  while((sproc != NULL) && (sproc->InheritsFrom(&TA_SchedProcess))
	&& !(sproc->InheritsFrom(&TA_EpochProcess)))
  {
    Stat* nag = (Stat*)stat_to_agg->Clone(); // clone original one
    sproc->loop_stats.Add(nag);
    nag->time_agg.op = Aggregate::AVG;
    taBase::SetPointer((TAPtr*)&(nag->time_agg.from), stat_to_agg);
    nag->time_agg.UpdateAfterEdit();
    nag->UpdateAfterEdit();
    sproc = (SchedProcess *) sproc->super_proc;
    stat_to_agg = nag;
  }
  
  CsSample* smp_proc = (CsSample*) own_proc->FindSuperProc(&TA_CsSample);
  if(smp_proc == NULL)    return;
  CsTIGstat* tig_stat = new CsTIGstat;
  smp_proc->final_stats.Add(tig_stat);
  taBase::SetPointer((TAPtr*)&(tig_stat->dist_stat), stat_to_agg);
  tig_stat->UpdateAfterEdit();
  tig_stat->CreateAggregates(default_op);
  UpdateAfterEdit();
}


////////////////////////////////
// 	    CsTIGStat         //
////////////////////////////////

void CsTIGstat::Initialize() {
  trial_proc = NULL;
  dist_stat = NULL;
}

void CsTIGstat::InitLinks() {
  Stat::InitLinks();
  trial_proc = GET_MY_OWNER(TrialProcess);
}

void CsTIGstat::CutLinks() {
  Stat::CutLinks();
  trial_proc = NULL;
  taBase::DelPointer((TAPtr*) &dist_stat);
}

void CsTIGstat::Copy_(const CsTIGstat& cp) {
  tig = cp.tig;
  taBase::SetPointer((TAPtr*) &dist_stat, cp.dist_stat);
}

void CsTIGstat::InitStat() {
  tig.InitStat(InitStatVal());
  InitStat_impl();
}

void CsTIGstat::Init() {
  tig.Init();
  Init_impl();
}

bool CsTIGstat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return tig.Crit();
}

void CsTIGstat::Network_Init() {
  InitStat();
}

void CsTIGstat::Network_Stat() {
  if((trial_proc == NULL) || (dist_stat == NULL)) return;
  Event* ev = trial_proc->cur_event;
  if(ev == NULL) return;
  ProbEventSpec* es = (ProbEventSpec*)ev->spec.spec;

  taLeafItr pitr,psitr;
  Pattern* p;
  PatternSpec* ps;
  int statval_index = 0;
  
  FOR_ITR_PAT_SPEC(Pattern, p, ev->patterns., pitr,
		   PatternSpec, ps, es->patterns., psitr)
  {
    if(ps->type == PatternSpec::TARGET) {
      float exp_prob = 1.0f;
      float act_prob = ((StatVal*)((CsDistStat*) dist_stat)->probs[statval_index])->val;
      if(p->InheritsFrom(&TA_ProbPattern))
	exp_prob = ((ProbPattern*)p)->prob;
      if(act_prob < .0001f)
	act_prob = .0001f;
      if(exp_prob <= 0.0f)
	net_agg.ComputeAgg(&tig, 0.0f);
      else
	net_agg.ComputeAgg(&tig, exp_prob * logf(exp_prob / act_prob));

      statval_index++;
    }
  }
}


////////////////////////////////
// 	   CsTargStat         //
////////////////////////////////

void CsTargStat::Initialize() {
  dist_stat = NULL;
}

void CsTargStat::InitLinks() {
  Stat::InitLinks();
}

void CsTargStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*) &dist_stat);
}

void CsTargStat::Copy_(const CsTargStat& cp) {
  trg_pct = cp.trg_pct;
  taBase::SetPointer((TAPtr*) &dist_stat, cp.dist_stat);
}

void CsTargStat::InitStat() {
  trg_pct.InitStat(InitStatVal());
  InitStat_impl();
}

void CsTargStat::Init() {
  trg_pct.Init();
  Init_impl();
}

bool CsTargStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return trg_pct.Crit();
}

void CsTargStat::Network_Init() {
  InitStat();
}

void CsTargStat::Network_Stat() {
  if(dist_stat == NULL) return;

  for (int i=0; i < dist_stat->probs.size; i++) {
    float act_prob = ((StatVal*) dist_stat->probs[i])->val;
    net_agg.ComputeAgg(&trg_pct, act_prob);
  }
}


////////////////////////////////
// 	   CsGoodStat         //
////////////////////////////////

void CsGoodStat::Initialize() {
  use_netin = false;
  netin_hrmny = 0;
}

void CsGoodStat::Destroy() {
}

void CsGoodStat::Copy_(const CsGoodStat& cp) {
  use_netin = cp.use_netin;
  hrmny = cp.hrmny;
  strss = cp.strss;
  gdnss = cp.gdnss;
}

void CsGoodStat::InitStat() {
  hrmny.InitStat(InitStatVal());
  strss.InitStat(InitStatVal());
  gdnss.InitStat(InitStatVal());
  InitStat_impl();
}

void CsGoodStat::Init() {
  hrmny.Init();
  strss.Init();
  gdnss.Init();
  Init_impl();
}

bool CsGoodStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  if(hrmny.Crit())	return true;
  if(strss.Crit())	return true;
  if(gdnss.Crit())	return true;
  return false;
}

void CsGoodStat::Network_Init() {
  InitStat();
}

void CsGoodStat::Unit_Init(Unit*) {
  netin_hrmny = 0.0f;
}  

void CsGoodStat::RecvCon_Run(Unit* unit) {
  if(use_netin)    return;
  Stat::RecvCon_Run(unit);
  netin_hrmny *= unit->act * 0.5f;
  netin_hrmny += unit->act * (unit->bias->wt + unit->ext);
  // netinput+external input+ bias
  net_agg.ComputeAggNoUpdt(hrmny.val, netin_hrmny);
}

void CsGoodStat::Con_Stat(Unit* , Connection* cn, Unit* su) {
  netin_hrmny += su->act * cn->wt;
}
  
void CsGoodStat::Unit_Stat(Unit* un) {
  // do harmony first since it was possibly computed before this in the recvcon
  if(use_netin) {
    CsUnit* csun = (CsUnit*) un;
    // the .5 is only times the connections, not the bias weight or ext
    // since everything is being mult by .5, but bias and ext
    // were already included in the net, so they are added again giving 1
    net_agg.ComputeAggNoUpdt(hrmny.val, 0.5f * csun->act *
			     (csun->net + csun->bias->wt + csun->ext));
			// don't update because update happens on stress
  }
  // todo: this assumes a logistic-style activation function..
  UnitSpec* us = un->spec.spec;
  float above_min = (un->act - us->act_range.min) * us->act_range.scale;
  float below_max = (us->act_range.max - un->act) * us->act_range.scale;
  float stress = 0.0f;
  if(above_min > 0.0f) stress = above_min * logf(above_min);
  if(below_max > 0.0f) stress = below_max * logf(below_max);
  stress += .6931472f;	// compensate for log vs. log2?
  net_agg.ComputeAgg(strss.val, stress);
}

void CsGoodStat::Network_Stat() {
  // this is not aggregated, because it is the difference of two sum terms..
  gdnss.val = hrmny.val - strss.val;
}
