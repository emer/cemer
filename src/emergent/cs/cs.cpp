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

#include "cs.h"

static void cs_converter_init() {
  DumpFileCvt* cvt = new DumpFileCvt("Cs", "CsUnit");
  cvt->repl_strs.Add(NameVar("_MGroup", "_Group"));
  cvt->repl_strs.Add(NameVar("Project", "V3CsProject"));
  cvt->repl_strs.Add(NameVar("V3CsProject_Group", "Project_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("V3CsProjection", "Projection"));
  cvt->repl_strs.Add(NameVar("CsPrjn_Group", "Projection_Group"));
  cvt->repl_strs.Add(NameVar("Network", "CsNetwork"));
  cvt->repl_strs.Add(NameVar("CsNetwork_Group", "Network_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("CsWiz", "CsWizard"));
  cvt->repl_strs.Add(NameVar("Layer", "CsLayer"));
  // obsolete types get replaced with taBase..
  cvt->repl_strs.Add(NameVar("WinView_Group", "taBase_Group"));
  cvt->repl_strs.Add(NameVar("ProjViewState_List", "taBase_List"));
  cvt->repl_strs.Add(NameVar("NetView", "taNBase"));
  cvt->repl_strs.Add(NameVar("DataTable", "taNBase"));
  cvt->repl_strs.Add(NameVar("EnviroView", "taNBase"));
  cvt->repl_strs.Add(NameVar("Xform", "taBase"));
  cvt->repl_strs.Add(NameVar("ImageEnv", "ScriptEnv"));
  cvt->repl_strs.Add(NameVar("unique/w=", "unique"));
  taMisc::file_converters.Add(cvt);
}

void cs_module_init() {
  ta_Init_cs();			// initialize types 
  cs_converter_init();		// configure converter
}

// module initialization
InitProcRegistrar mod_init_cs(cs_module_init);


//////////////////////////
//  	Con, Spec	//
//////////////////////////

void CsConSpec::Initialize() {
  min_obj_type = &TA_CsCon;
  wt_limits.sym = true;		// default is to have symmetric initial weights!
  lrate = .01f;
  momentum = 0.0f;
  momentum_c = 1.0f;
  decay = 0.0f;
  decay_fun = NULL;
}

void CsConSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_CsConSpec); // allow any of this basic type here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void CsConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  momentum_c = 1.0f - momentum;
}

void Cs_Simple_WtDecay(CsConSpec* spec, CsCon* cn, Unit*, Unit*) {
  cn->dwt -= spec->decay * cn->wt;
}

void Cs_WtElim_WtDecay(CsConSpec* spec, CsCon* cn, Unit*, Unit*) {
  float denom = (1.0f + (cn->wt * cn->wt));
  cn->dwt -= spec->decay * ((2.0f * cn->wt * cn->wt) / (denom * denom));
}

void CsRecvCons::Initialize() {
  SetConType(&TA_CsCon);
}

void CsSendCons::Initialize() {
  SetConType(&TA_CsCon);
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
  state_decay = 0.5f;
  use_annealing = false;
  use_sharp = false;
}

void CsUnitSpec::InitLinks() {
  bias_spec.type = &TA_CsConSpec;
  inherited::InitLinks();
  children.SetBaseType(&TA_CsUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!

  taBase::Own(real_range, this);
  taBase::Own(noise_sched, this);
  taBase::Own(gain_sched, this);
}

void CsUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sqrt_step = sqrt(step);
}

void CsUnitSpec::Init_Acts(Unit* u, Network* net) {
  //  inherited::Init_Acts(u);  // this also initializes input data -- don't
  //  u->Init_InputData();
  u->net = 0.0f;
  u->act = 0.0f;
  CsUnit* cu = (CsUnit*)u;
  cu->da = 0.0f;
  cu->prv_net = 0.0f;
  cu->clmp_net = 0.0f;
  cu->act = act_range.Clip(initial_act.Gen());
}

void CsUnitSpec::DecayState(CsUnit* u, CsNetwork* net) {
  //  u->Init_InputData();
  CsUnit* cu = (CsUnit*)u;
  float trgact = act_range.Clip(initial_act.Gen());
  cu->act -= state_decay * (cu->act - trgact);
  cu->da = 0.0f;
  //  cu->prv_net -= state_decay * cu->prv_net;
  cu->prv_net = 0.0f;
  cu->clmp_net = 0.0f;
}

void CsUnitSpec::Init_Weights(Unit* u, Network* net) {
  inherited::Init_Weights(u, net);
  CsUnit* cu = (CsUnit*)u;
  cu->n_dwt_aggs = 0; // initialize it here
  cu->act_m = cu->act_p = 0.0f;
}

void CsUnitSpec::Compute_ClampAct(CsUnit* u, CsNetwork* net) {
  if((clamp_type != HARD_FAST_CLAMP) || !(u->ext_flag & Unit::EXT))
    return;
  u->act = real_range.Clip(u->ext);	// keep everything in range
  u->da = 0.0f;
}

void CsUnitSpec::Compute_ClampNet(CsUnit* u, CsNetwork* net) {
  u->clmp_net = 0.0f;
  if(clamp_type != HARD_FAST_CLAMP)
    return;
  for(int g=0; g<u->recv.size; g++) {
    CsRecvCons* recv_gp = (CsRecvCons*)u->recv.FastEl(g);
    Layer* lay = recv_gp->prjn->from;
    if(lay->lesioned() || !(lay->ext_flag & Unit::EXT))
      continue;
    u->clmp_net += recv_gp->Compute_Netin(u);
  }
}

void CsUnitSpec::Compute_Netin(Unit* u, Network* net, int thread_no) {
  CsUnit* cu = (CsUnit*)u;
  if(clamp_type == HARD_FAST_CLAMP) {
    if(cu->ext_flag & Unit::EXT)
      return;
    cu->net = 0.0f;
    for(int g=0; g<u->recv.size; g++) {
      CsRecvCons* recv_gp = (CsRecvCons*)u->recv.FastEl(g);
      Layer* lay = recv_gp->prjn->from;
      if(lay->lesioned() || (lay->ext_flag & Unit::EXT)) // don't add from clamped!
	continue;
      u->net += recv_gp->Compute_Netin(u);
    }
    u->net += u->bias.Cn(0)->wt;
  }
  else if(clamp_type == HARD_CLAMP) {
    if(cu->ext_flag & Unit::EXT) // no point in computing net for clamped units!
      return;
    inherited::Compute_Netin(u, net, thread_no);
  }
  else {			// soft clamping (or soft-then-hard), always compute net
    inherited::Compute_Netin(u, net, thread_no);
  }
  cu->net += cu->clmp_net; // add in clamped input
}

void CsUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  CsNetwork* cnet = (CsNetwork*)net;
  int cycle = cnet->cycle;
  int phase = cnet->phase;

  CsUnit* cu = (CsUnit*)u;
  if(u->ext_flag & Unit::EXT) {	// receiving external input
    if(clamp_type == HARD_FAST_CLAMP) // already processed this one!
      return;
    ClampType ct = clamp_type;
    if(clamp_type == SOFT_THEN_HARD_CLAMP) {
      if(phase == CsNetwork::MINUS_PHASE)
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

  //  
}

void CsUnitSpec::Aggregate_dWt(CsUnit* cu, CsNetwork* net, int thread_no) {
  int phase = net->phase;
  ((CsConSpec*)bias_spec.SPtr())->B_Aggregate_dWt((CsCon*)cu->bias.Cn(0), cu, phase);
  for(int g=0; g<cu->recv.size; g++) {
    CsRecvCons* recv_gp = (CsRecvCons*)cu->recv.FastEl(g);
    if(!recv_gp->prjn->from->lesioned())
      recv_gp->Aggregate_dWt(cu,phase);
  }
  cu->n_dwt_aggs++;
}

void CsUnitSpec::PhaseInit(CsUnit* u, CsNetwork* net) {
  if(!(u->ext_flag & Unit::TARG))
    return;

  if(net->phase == CsNetwork::MINUS_PHASE) {
    u->ext = 0.0f;
    u->UnSetExtFlag(Unit::EXT);
  }
  else {
    u->ext = u->targ;
    u->SetExtFlag(Unit::EXT);
  }
}

void CsUnitSpec::PostSettle(CsUnit* u, CsNetwork* net) {
  if(net->phase == CsNetwork::MINUS_PHASE)
    u->act_m = u->act;
  else
    u->act_p = u->act;
}

void CsUnitSpec::Compute_dWt(Unit* u, Network* net, int thread_no) {
  inherited::Compute_dWt(u, net, thread_no);
  CsUnit* cu = (CsUnit*)u;
  ((CsConSpec*)bias_spec.SPtr())->B_Compute_dWt((CsCon*)cu->bias.Cn(0), cu);
  cu->n_dwt_aggs = 0;		// reset after wts are aggd
}

void CsUnitSpec::Compute_Weights(Unit* u, Network* net, int thread_no) {
  inherited::Compute_Weights(u, net, thread_no);
  ((CsConSpec*)bias_spec.SPtr())->B_Compute_Weights((CsCon*)u->bias.Cn(0), u);
}

void CsUnitSpec::GraphActFun(DataTable* graph_data, float min, float max, int ncycles) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* netin = graph_data->FindMakeColName("Netin", idx, VT_FLOAT);
  DataCol* act = graph_data->FindMakeColName("Act", idx, VT_FLOAT);

  CsUnit un;

  float x;
  for(x = min; x <= max; x += .01f) {
    un.act = act_range.min + .5f * act_range.range;
    un.net = x;
    int cy;
    for(cy=0;cy<ncycles;cy++) {
      Compute_Act_impl(&un, 0, 0);
    }
    graph_data->AddBlankRow();
    netin->SetValAsFloat(x, -1);
    act->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void CsUnit::Initialize() {
  da = act_p = act_m = clmp_net = prv_net = 0.0f;
  n_dwt_aggs = 0;
}

void CsUnit::Copy_(const CsUnit& cp) {
  da = cp.da;
  prv_net = cp.prv_net;
  clmp_net = cp.clmp_net;
  act_m = cp.act_m;
  act_p = cp.act_p;
  n_dwt_aggs = cp.n_dwt_aggs;
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

void BoltzUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
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
}

void IACUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gain = 1.0f / decay;		// keep gain and decay aligned
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
  act_range.min = -1e20f;
  act_range.max = 1e20f;
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
  act_range.min = -1e20f;
  act_range.max = 1e20f;
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


/////////////////////////////////////////////////////////////////////////

//////////////////////////
//   CsLayer	        //
//////////////////////////

void CsLayer::Initialize() {
  units.SetBaseType(&TA_CsUnit);
  unit_spec.SetBaseType(&TA_CsUnitSpec);
}

void CsLayer::Init_Acts(Network* net) {	
  // commenting this out is only diff from orig fun:
  //  ext_flag = Unit::NO_EXTERNAL;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Init_Acts(net);
}


//////////////////////////////////
//	Cs Network		//
//////////////////////////////////

void CsNetwork::Initialize() {
  layers.SetBaseType(&TA_CsLayer);
  update_mode = SYNCHRONOUS;
  n_updates = 10;

  trial_init = INIT_STATE;
  between_phases = INIT_STATE;
  //  deterministic = false;
  deterministic = true;
  start_stats = 50;
  cycle_max = 100;
  sample = 0;
  phase_max = 2;
  phase = MINUS_PHASE;
  phase_no = 0;

  maxda_stopcrit = .01f;
  maxda = 0.0f;

  minus_cycles = 0.0f;

  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;
}

void CsNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

#ifdef DMEM_COMPILE
  if((update_mode == ASYNCHRONOUS) && (taMisc::dmem_nprocs > 1)) {
    taMisc::Error("CsCycle Error: cannot use ASYNCHRONOUS update_mode with distributed memory computation (np > 1)");
  }
#endif

  if(TestError(!deterministic && start_stats > cycle_max, "UpdateAfterEdit",
	       "start_stats is > cycle_max -- won't work -- setting to cycle_max -10 -- might want to set to desired value")) {
    start_stats = cycle_max - 10;
    if(start_stats < 0) start_stats = 0;
  }
}

void CsNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_CsCon;
  prjn->recvcons_type = &TA_CsRecvCons;
  prjn->sendcons_type = &TA_CsSendCons;
  prjn->con_spec.SetBaseType(&TA_CsConSpec);
}

void CsNetwork::Init_Counters() {
  inherited::Init_Counters();
  phase = MINUS_PHASE;
  phase_no = 0;
  sample = 0;
}

void CsNetwork::Init_Stats() {
  inherited::Init_Stats();
  maxda = 0.0f;
  minus_cycles = 0.0f;

  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;
}

////////////////////////////////
// 	     Cycle            //
////////////////////////////////

void CsNetwork::Compute_SyncAct() {
  // same as the basic call: just add a diff cost 
  ThreadUnitCall un_call(&Unit::Compute_Act);
  threads.Run(&un_call, .2f);	// todo: get new cost!
}

void CsNetwork::Compute_AsyncAct() {
  if(units_flat.size == 0) return; // error check
  Layer* lay;
  taLeafItr l;
  for (int i=0; i < n_updates; i++) {	// do this n_updates times
    int rnd_num = Random::IntZeroN(units_flat.size);
    CsUnit* u = (CsUnit*) units_flat[rnd_num];
    u->Compute_Netin(this);
    u->Compute_Act(this); // update this one unit...
  }
}

void CsNetwork::Compute_MaxDa() {
  // this has to be a separate step due to threading
  maxda = 0.0f;		// initialize
  output_name = "";	// todo: update this
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
    if(lay->lesioned()) continue;
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u) {
      maxda = MAX(fabsf(un->da), maxda);
    }
  }
}

void CsNetwork::Aggregate_dWt() {
  ThreadUnitCall un_call((ThreadUnitMethod)(CsUnitMethod)&CsUnit::Aggregate_dWt);
  threads.Run(&un_call, .5f);	// todo: update est
}

void CsNetwork::Cycle_Run() {
  if(update_mode == SYNCHRONOUS) {
    Compute_Netin();	// two-stage update of nets and acts
    Compute_SyncAct();
  }
  else {
    Compute_AsyncAct();
  }
  Compute_MaxDa();

  if(!deterministic && (cycle >= start_stats) && !(train_mode == Network::TEST))
    Aggregate_dWt();
}

////////////////////////////////
//         Settle             //
////////////////////////////////

void CsNetwork::Settle_Init() {
  if(phase == PLUS_PHASE) {
    if(between_phases == INIT_STATE)
      Init_Acts();
    else if(between_phases == DECAY_STATE)
      DecayState();
  }

  PhaseInit();

  // precompute clamping info so cycles aren't wasted during settling
  Compute_ClampAct();
  Compute_ClampNet();
}

void CsNetwork::Compute_ClampAct() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
    if(lay->lesioned() || !(lay->ext_flag & Unit::EXT)) // only clamped layers
      continue;
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u)
      un->Compute_ClampAct(this);
  }
}

void CsNetwork::Compute_ClampNet() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
    if(lay->lesioned() || (lay->ext_flag & Unit::EXT)) // only non-clamped layers
      continue;
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u)
      un->Compute_ClampNet(this);
  }
}

void CsNetwork::PhaseInit() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
    if(lay->lesioned())
      continue;
    if(lay->ext_flag & Unit::TARG) {
      if(phase == PLUS_PHASE)
	lay->SetExtFlag(Unit::EXT);
    }
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u)
      un->PhaseInit(this);
  }
}

void CsNetwork::DecayState() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
    if(lay->lesioned())
      continue;
    CsUnit* un;
    taLeafItr u;
    FOR_ITR_EL(CsUnit, un, lay->units., u)
      un->DecayState(this);
  }
}

void CsNetwork::PostSettle() {
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
    if(!lay->lesioned()) {
      CsUnit* un;
      taLeafItr u;
      FOR_ITR_EL(CsUnit, un, lay->units., u)
	un->PostSettle(this);
    }
  }
}

void CsNetwork::Settle_Final() {
  if(deterministic && (train_mode != TEST)) {
    Aggregate_dWt();
  }
  PostSettle();
}
  
////////////////////////////////
// 	   Trial             //
////////////////////////////////

void CsNetwork::Trial_Init() {
  phase_max = 2;
  phase = MINUS_PHASE;
  phase_no = 0;

  if(trial_init == INIT_STATE)
    Init_Acts();
  else if(trial_init == DECAY_STATE)
    DecayState();
}

void CsNetwork::Trial_Final() {
  if(train_mode != TEST) {
    Compute_dWt();
  }
}

void CsNetwork::Trial_UpdatePhase() {
  phase = PLUS_PHASE;		// always just next phase..
}

void CsNetwork::Compute_MinusCycles() {
  minus_cycles = cycle;
  avg_cycles_sum += minus_cycles;
  avg_cycles_n++;
}

void CsNetwork::Compute_TrialStats() {
  inherited::Compute_TrialStats();
  Compute_MinusCycles();
}

void CsNetwork::Compute_AvgCycles() {
  if(avg_cycles_n > 0) {
    avg_cycles = avg_cycles_sum / (float)avg_cycles_n;
  }
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;
}

void CsNetwork::Compute_EpochStats() {
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
}

// ////////////////////////////////
// // 	    CsSample          //
// ////////////////////////////////

// void CsSample::Initialize() {
//   sub_proc_type = &TA_CsNetwork;
//   cur_event = NULL;
//   sample.SetMax(1);

//   min_unit = &TA_CsUnit;
//   min_con_group = &TA_CsRecvCons;
//   min_con = &TA_CsCon;
// }

// void CsSample::InitLinks() {
//   TrialProcess::InitLinks();
//   taBase::Own(sample, this);
// }

// void CsSample::Init_impl() {
//   TrialProcess::Init_impl();
// }

// void CsSample::Compute_dWt() {
//   Compute_dWt();
// }  

// void CsSample::Final() {
//   TrialProcess::Final();	// do anything else

//   if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST))
//     Compute_dWt();
// }

// void CsMaxDa::Unit_Stat(Unit* unit) {
//   float fda = fabsf(((CsUnit*)unit)->da);
//   net_agg.ComputeAgg(&da, fda);
// }

// void CsMaxDa::Network_Stat() {
//   if((settle != NULL) && (settle->cycle.val < 5)) // don't stop before 5 cycles..
//     da.val = MAX(da.val, da.stopcrit.val);
// }


// ////////////////////////////////
// // 	   CsDistStat         //
// ////////////////////////////////

// void CsDistStat::Initialize() {
//   cs_settle = NULL;
//   tolerance = 0.25f;
//   n_updates = 0;
//   loop_init = INIT_START_ONLY;		// don't initialize in the loop!!
// }

// void CsDistStat::InitLinks() {
//   Stat::InitLinks();

//   cs_settle = GET_MY_OWNER(CsNetwork);
// }

// void CsDistStat::CutLinks() {
//   Stat::CutLinks();
//   cs_settle = NULL;
// }

// void CsDistStat::Copy_(const CsDistStat& cp) {
//   probs = cp.probs;
//   act_vals = cp.act_vals;
//   tolerance = cp.tolerance;
//   n_updates = cp.n_updates;
// }

// void CsDistStat::InitStat() {
//   probs.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void CsDistStat::Init() {
//   act_vals.Reset();
//   n_updates = 0;

//   if(cs_settle == NULL) {
//     probs.Init();
//     Init_impl();
//     return;
//   }
//   Event* ev = cur_event;
//   if(ev == NULL) {
//     probs.Init();
//     Init_impl();
//     return;
//   }
//   ProbEventSpec* es = (ProbEventSpec*)ev->spec.SPtr();

//   int num_targets = 0;
//   taLeafItr psitr;
//   PatternSpec* ps;
//   FOR_ITR_EL(PatternSpec, ps, es->patterns., psitr) {
//     if(ps->type == PatternSpec::TARGET)
//       num_targets++;
//   }

//   if(probs.size != num_targets) {
//     probs.EnforceSize(num_targets);
//     NameStatVals();
//   }

//   probs.Init();
//   Init_impl();
// }

// bool CsDistStat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   return probs.Crit();
// }

// void CsDistStat::Network_Stat() {
//   if(cs_settle == NULL) return;
//   Event* ev = cur_event;
//   if(ev == NULL) return;
//   ProbEventSpec* es = (ProbEventSpec*)ev->spec.SPtr();

//   Layer* cur_lay = NULL;

//   taLeafItr pitr,psitr;
//   Pattern* p;
//   PatternSpec* ps;
//   int statval_index = 0;
  
//   FOR_ITR_PAT_SPEC(Pattern, p, ev->patterns., pitr,
// 		   PatternSpec, ps, es->patterns., psitr)
//   {
//     if(ps->type == PatternSpec::TARGET) {
//       if((ps->layer != NULL) && (ps->layer != cur_lay)) {
//       	act_vals.Reset();
// 	Unit* u;
// 	taLeafItr uitr;
// 	cur_lay = ps->layer;
// 	FOR_ITR_EL(Unit, u, cur_lay->units., uitr) {
// 	  act_vals.Add(u->act);
// 	}
//       }
//       float within_tol = 0.0f;
//       if(p->value.SumSquaresDist(act_vals, false, tolerance) == 0.0f)
// 	within_tol = 1.0f;
//       StatVal* sv = (StatVal*)probs[statval_index];
//       sv->val =	((sv->val * (float) n_updates) + within_tol)
// 		     / (float) (n_updates + 1);
//       statval_index++;
//     }
//   }
//   n_updates++;
// }

// void CsDistStat::CreateAggregates(Aggregate::Operator default_op) {
//   if(own_proc == NULL)
//     return;

//   SchedProcess* sproc= (SchedProcess *) own_proc->super_proc;
//   Stat*		stat_to_agg = this;

//   while((sproc != NULL) && (sproc->InheritsFrom(&TA_SchedProcess))
// 	&& !(sproc->InheritsFrom(&TA_EpochProcess)))
//   {
//     Stat* nag = (Stat*)stat_to_agg->Clone(); // clone original one
//     sproc->loop_stats.Add(nag);
//     nag->time_agg.op = Aggregate::AVG;
//     taBase::SetPointer((TAPtr*)&(nag->time_agg.from), stat_to_agg);
//     nag->time_agg.UpdateAfterEdit();
//     nag->UpdateAfterEdit();
//     sproc = (SchedProcess *) sproc->super_proc;
//     stat_to_agg = nag;
//   }
  
//   CsSample* smp_proc = (CsSample*) own_proc->FindSuperProc(&TA_CsSample);
//   if(smp_proc == NULL)    return;
//   CsTIGstat* tig_stat = new CsTIGstat;
//   smp_proc->final_stats.Add(tig_stat);
//   taBase::SetPointer((TAPtr*)&(tig_stat->dist_stat), stat_to_agg);
//   tig_stat->UpdateAfterEdit();
//   tig_stat->CreateAggregates(default_op);
//   UpdateAfterEdit();
// }


// ////////////////////////////////
// // 	    CsTIGStat         //
// ////////////////////////////////

// void CsTIGstat::Initialize() {
//   trial_proc = NULL;
//   dist_stat = NULL;
// }

// void CsTIGstat::InitLinks() {
//   Stat::InitLinks();
//   trial_proc = GET_MY_OWNER(TrialProcess);
// }

// void CsTIGstat::CutLinks() {
//   Stat::CutLinks();
//   trial_proc = NULL;
//   taBase::DelPointer((TAPtr*) &dist_stat);
// }

// void CsTIGstat::Copy_(const CsTIGstat& cp) {
//   tig = cp.tig;
//   taBase::SetPointer((TAPtr*) &dist_stat, cp.dist_stat);
// }

// void CsTIGstat::InitStat() {
//   tig.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void CsTIGstat::Init() {
//   tig.Init();
//   Init_impl();
// }

// bool CsTIGstat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   return tig.Crit();
// }

// void CsTIGstat::Network_Init() {
//   InitStat();
// }

// void CsTIGstat::Network_Stat() {
//   if((trial_proc == NULL) || (dist_stat == NULL)) return;
//   Event* ev = trial_proc->cur_event;
//   if(ev == NULL) return;
//   ProbEventSpec* es = (ProbEventSpec*)ev->spec.SPtr();

//   taLeafItr pitr,psitr;
//   Pattern* p;
//   PatternSpec* ps;
//   int statval_index = 0;
  
//   FOR_ITR_PAT_SPEC(Pattern, p, ev->patterns., pitr,
// 		   PatternSpec, ps, es->patterns., psitr)
//   {
//     if(ps->type == PatternSpec::TARGET) {
//       float exp_prob = 1.0f;
//       float act_prob = ((StatVal*)((CsDistStat*) dist_stat)->probs[statval_index])->val;
//       if(p->InheritsFrom(&TA_ProbPattern))
// 	exp_prob = ((ProbPattern*)p)->prob;
//       if(act_prob < .0001f)
// 	act_prob = .0001f;
//       if(exp_prob <= 0.0f)
// 	net_agg.ComputeAgg(&tig, 0.0f);
//       else
// 	net_agg.ComputeAgg(&tig, exp_prob * logf(exp_prob / act_prob));

//       statval_index++;
//     }
//   }
// }


// ////////////////////////////////
// // 	   CsTargStat         //
// ////////////////////////////////

// void CsTargStat::Initialize() {
//   dist_stat = NULL;
// }

// void CsTargStat::InitLinks() {
//   Stat::InitLinks();
// }

// void CsTargStat::CutLinks() {
//   Stat::CutLinks();
//   taBase::DelPointer((TAPtr*) &dist_stat);
// }

// void CsTargStat::Copy_(const CsTargStat& cp) {
//   trg_pct = cp.trg_pct;
//   taBase::SetPointer((TAPtr*) &dist_stat, cp.dist_stat);
// }

// void CsTargStat::InitStat() {
//   trg_pct.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void CsTargStat::Init() {
//   trg_pct.Init();
//   Init_impl();
// }

// bool CsTargStat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   return trg_pct.Crit();
// }

// void CsTargStat::Network_Init() {
//   InitStat();
// }

// void CsTargStat::Network_Stat() {
//   if(dist_stat == NULL) return;

//   for (int i=0; i < dist_stat->probs.size; i++) {
//     float act_prob = ((StatVal*) dist_stat->probs[i])->val;
//     net_agg.ComputeAgg(&trg_pct, act_prob);
//   }
// }


// ////////////////////////////////
// // 	   CsGoodStat         //
// ////////////////////////////////

// void CsGoodStat::Initialize() {
//   use_netin = false;
//   netin_hrmny = 0;
// }

// void CsGoodStat::Destroy() {
// }

// void CsGoodStat::Copy_(const CsGoodStat& cp) {
//   use_netin = cp.use_netin;
//   hrmny = cp.hrmny;
//   strss = cp.strss;
//   gdnss = cp.gdnss;
// }

// void CsGoodStat::InitStat() {
//   hrmny.InitStat(InitStatVal());
//   strss.InitStat(InitStatVal());
//   gdnss.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void CsGoodStat::Init() {
//   hrmny.Init();
//   strss.Init();
//   gdnss.Init();
//   Init_impl();
// }

// bool CsGoodStat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   if(hrmny.Crit())	return true;
//   if(strss.Crit())	return true;
//   if(gdnss.Crit())	return true;
//   return false;
// }

// void CsGoodStat::Network_Init() {
//   InitStat();
// }

// void CsGoodStat::Unit_Init(Unit*) {
//   netin_hrmny = 0.0f;
// }  

// void CsGoodStat::RecvCon_Run(Unit* unit) {
//   if(use_netin)    return;
//   Stat::RecvCon_Run(unit);
//   netin_hrmny *= unit->act * 0.5f;
//   netin_hrmny += unit->act * (unit->bias.Cn(0)->wt + unit->ext);
//   // netinput+external input+ bias
//   net_agg.ComputeAggNoUpdt(hrmny.val, netin_hrmny);
// }

// void CsGoodStat::Con_Stat(Unit* , Connection* cn, Unit* su) {
//   netin_hrmny += su->act * cn->wt;
// }
  
// void CsGoodStat::Unit_Stat(Unit* un) {
//   // do harmony first since it was possibly computed before this in the recvcon
//   if(use_netin) {
//     CsUnit* csun = (CsUnit*) un;
//     // the .5 is only times the connections, not the bias weight or ext
//     // since everything is being mult by .5, but bias and ext
//     // were already included in the net, so they are added again giving 1
//     net_agg.ComputeAggNoUpdt(hrmny.val, 0.5f * csun->act *
// 			     (csun->net + csun->bias.Cn(0)->wt + csun->ext));
// 			// don't update because update happens on stress
//   }
//   // todo: this assumes a logistic-style activation function..
//   UnitSpec* us = un->spec.SPtr();
//   float above_min = (un->act - us->act_range.min) * us->act_range.scale;
//   float below_max = (us->act_range.max - un->act) * us->act_range.scale;
//   float stress = 0.0f;
//   if(above_min > 0.0f) stress = above_min * logf(above_min);
//   if(below_max > 0.0f) stress = below_max * logf(below_max);
//   stress += .6931472f;	// compensate for log vs. log2?
//   net_agg.ComputeAgg(strss.val, stress);
// }

// void CsGoodStat::Network_Stat() {
//   // this is not aggregated, because it is the difference of two sum terms..
//   gdnss.val = hrmny.val - strss.val;
// }


//////////////////////////
//   CsProject	        //
//////////////////////////

void CsProject::Initialize() {
  wizards.SetBaseType(&TA_CsWizard);
  networks.SetBaseType(&TA_CsNetwork);
}


void CsWizard::Initialize() {
  connectivity = BIDIRECTIONAL;
  default_net_type = &TA_CsNetwork;
}

bool CsWizard::StdProgs() {
  if(!StdProgs_impl("CsAll_Std")) return false;
  return true;
}

bool CsWizard::TestProgs(Program* call_test_from, bool call_in_loop, int call_modulus) {
  if(!TestProgs_impl("CsAll_Test", call_test_from, call_in_loop, call_modulus))
    return false;
  return true;
}

