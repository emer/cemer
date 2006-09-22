// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

// v3_compat.cpp -- version 3 compatability objects, only for converting

#include "v3_compat.h"

#include "ta_css.h"
#include "netstru.h"
#include "pdpshell.h"

#ifdef TA_GUI
#include "ta_qtviewer.h"
#include "ta_qtbrowse.h"

//////////////////////////
// 	CtrlPanelData	//
//////////////////////////

void CtrlPanelData::Initialize() {
  active = false;
  lft = 0.0f;
  top = 0.0f;
}

#endif


//////////////////////////
// 	Process		//
//////////////////////////

void Process::Initialize() {
  min_network = &TA_Network;
  min_layer = &TA_Layer;
  min_unit = &TA_Unit;
  min_con_group = &TA_Con_Group;
  min_con = &TA_Connection;
  type=C_CODE;
  network = NULL;
  environment = NULL;
}

void DataItem::Initialize() {
  is_string = false;
  vec_n = 0;
  disp_opts = " ";		// always pad with initial blank
}

void StatVal::Initialize() {
  val = 0.0f;
}

void AggStat::Initialize() {
  from = NULL;
  real_stat = NULL;
  type_safe = true;
}

void Stat::Initialize() {
  own_proc = NULL;
  has_stop_crit = false;
  n_copy_vals = 0;
  loop_init = INIT_IN_LOOP;
  log_stat = true;
  layer = NULL;
}

void SE_Stat::Initialize() {
  tolerance = 0.0f;
}

void MonitorStat::Initialize() {
  net_agg.op = Aggregate::COPY;
  variable = "act";
}

void StepParams::Initialize() {
  owner = NULL;
  proc = NULL;
  n = 1;
}

void SchedProcess::Initialize() {
  re_init=true;
  sub_proc_type = NULL;
  cntr = NULL;
  super_proc = NULL;
  sub_proc = NULL;
  log_loop = false;
  log_counter = false;
  loop_stats.SetBaseType(&TA_Stat);
  final_stats.SetBaseType(&TA_Stat);
  init_procs.SetBaseType(&TA_Process);
  loop_procs.SetBaseType(&TA_Process);
  final_procs.SetBaseType(&TA_Process);
  logs.SetBaseType(&TA_PDPLog);
  cntr_items.SetBaseType(&TA_DataItem);
}

void CycleProcess::Initialize() {
  sub_proc_type = NULL;
}

void SettleProcess::Initialize() {
  sub_proc_type = &TA_CycleProcess;
  cycle.SetMax(100);
  log_counter = true;
}

void TrialProcess::Initialize() {
  sub_proc_type = NULL;
  cur_event = NULL;
  epoch_proc = NULL;
  enviro_group = NULL;
}

void EpochProcess::Initialize() {
  sub_proc_type = &TA_TrialProcess;
  trial.SetMax(100);
  order = PERMUTED;
  cur_event = NULL;
  wt_update = ON_LINE;
  batch_n = 10;
  batch_n_eff = 10;
  enviro_group = NULL;
  dmem_nprocs = 1024;		// take a large number if possible!
}

void SequenceProcess::Initialize() {
  sub_proc_type = &TA_TrialProcess;
  tick.SetMax(10);
  order = SEQUENTIAL;
  sequence_init = INIT_STATE;
  sequence_epoch = NULL;
  cur_event_gp = NULL;
  cur_event = NULL;
  enviro_group = NULL;
}

void SequenceEpoch::Initialize() {
  sub_proc_type = &TA_SequenceProcess;
  small_batch = SEQUENCE;	// prior default setting..
  cur_event_gp = NULL;
}

void InteractiveEpoch::Initialize() {
  order = SEQUENTIAL;
  last_trial_val = -1;
}

void NEpochProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  epc_ctr.SetMax(50);
  epoch_proc = NULL;
  log_counter = true;
  epoch = 0;
}

void TrainProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  epoch.SetMax(1000);
  epoch_proc = NULL;
  log_counter = true;
}

void BatchProcess::Initialize() {
  sub_proc_type = &TA_TrainProcess;
  batch.SetMax(10);
}

//////////////////////////////////
//	Environment		//
//////////////////////////////////

void PatternSpec::Initialize() {
  type = INACTIVE;
  to_layer = LAY_NAME;
  layer_num = 0;
  layer = NULL;
  pattern_type = &TA_Pattern;
  layer_flags = DEFAULT;
  use_flags = USE_NO_FLAGS;
  n_vals = 0;
  geom.y = 1;
  initial_val = 0.0f;
  noise.type = Random::NONE;
  noise.mean = 0.0f;
  noise.var = 0.5f;
}

void PatternSpec_Group::Initialize() {
  pat_gp_type = &TA_Pattern_Group;
  SetBaseType(&TA_PatternSpec);
}

void EventSpec::Initialize() {
  last_net = NULL;
  pattern_layout = HORIZONTAL;
}

void Pattern::Initialize() {
}

void Event::Initialize() {
  index = -1;
  patterns.SetBaseType(&TA_Pattern);
}

void Environment::Initialize() {
  events.SetBaseType(&TA_Event);
  event_specs.SetBaseType(&TA_EventSpec);
//TODO  views.SetBaseType(&TA_EnviroView);
  event_ctr = 0;
}

void ScriptEnv::Initialize() {
}

void FreqEvent::Initialize() {
  frequency = 1.0f;
}

void FreqEvent_Group::Initialize() {
  fenv = NULL;
  frequency = 1.0f;
}

void FreqEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP_EVENT;
  sample_type = PERMUTED;
  events.SetBaseType(&TA_FreqEvent);
  events.gp.SetBaseType(&TA_FreqEvent_Group);
}

void TimeEvent_Group::Initialize() {
  end_time = 1.0f;
  interpolate = USE_ENVIRO;
  SetBaseType(&TA_TimeEvent);
}

void TimeEnvironment::Initialize() {
  events.SetBaseType(&TA_TimeEvent);
  events.gp.SetBaseType(&TA_TimeEvent_Group);
  interpolate = CONSTANT;
}

void FreqTimeEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP;		// group is typically used with time
  sample_type = RANDOM;
  events.SetBaseType(&TA_FreqTimeEvent);
  events.gp.SetBaseType(&TA_FreqTimeEvent_Group);
}

void ProbPattern::Initialize() {
  prob = 1.0f;
  applied = false;
}

void ProbPatternSpec_Group::Initialize() {
  last_pat = -1;
}

void ProbEventSpec::Initialize() {
  patterns.gp.el_typ = &TA_ProbPatternSpec_Group;
  default_prob = .25f;
}


void XYPatternSpec::Initialize() {
  pattern_type = &TA_XYPattern;
  wrap = false;
  apply_background = false;
  background_value = 0.0f;
}

void XYSubPatternSpec::Initialize() {
  pattern_type = &TA_XYSubPattern;
  wrap = false;
}

void GroupPatternSpec::Initialize() {
  sub_geom.SetGtEq(1);
  TDCoord& gg = gp_geom;
  gg = geom / sub_geom;
  trans_apply = false;
}

//////////////////////////////////
//	Basic Processes		//
//////////////////////////////////

void ScriptProcess::Initialize() {
}

void SyncEpochProc::Initialize() {
  second_proc_type = &TA_TrialProcess;
  second_proc = NULL;
  second_network = NULL;
}

void GridSearchBatch::Initialize() {
  start_val = .1f;
  inc_val = .1f;
  cur_val = .1f;
}

void SameSeedBatch::Initialize() {
  rnd_seeds.SetBaseType(&TA_RndSeed);
  in_goto_val = -1;
}

void ForkProcess::Initialize() {
  second_proc_type = &TA_TrialProcess;
  second_proc = NULL;
  second_network = NULL;
  second_environment = NULL;
}

void BridgeProcess::Initialize() {
  second_network = NULL;
  src_layer = NULL;
  trg_layer = NULL;
  direction = ONE_TO_TWO;
  src_variable = "act";
  trg_variable = "ext";
  trg_ext_flag = Unit::EXT;
}

void MultiEnvProcess::Initialize() {
  log_counter = true;
  env.SetMax(0);
  use_subctr_max = false;
}

void PatternFlagProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  log_counter = true;
  pattern_no = 0;
  flag = PatternSpec::NO_FLAGS;
  invert = false;
  val_idx.SetMax(0);
}

//////////////////////////////////
//	Stats Processes		//
//////////////////////////////////

void ClosestEventStat::Initialize() {
  trial_proc = NULL;
  cmp_type = float_RArray::SUM_SQUARES;
  dist_tol = 0.0f;
  norm = false;
  subgp_no = -1;
  net_agg.op = Aggregate::MIN;
}

void CyclesToSettle::Initialize() {
  settle = NULL;
  net_agg.op = Aggregate::LAST;
}

void ActThreshRTStat::Initialize() {
  loop_init = INIT_START_ONLY;
  settle = NULL;
  net_agg.op = Aggregate::LAST;
  act_thresh = .75f;
  // max_act.stopcrit.flag = true;  // don't actually stop..
  max_act.stopcrit.val = act_thresh;
  max_act.stopcrit.rel = CritParam::GREATERTHANOREQUAL;
  crossed_thresh = false;
}

void ScriptStat::Initialize() {
}
  
void CompareStat::Initialize() {
  stat_1 = NULL;
  stat_2 = NULL;
  cmp_type = float_RArray::CORREL;
  dist_tol = 0.0f;
  norm = false;
  net_agg.op = Aggregate::LAST;
}

void ProjectionStat::Initialize() {
  stat = NULL;
  dist_metric = float_RArray::INNER_PROD;
  dist_tol = 0.0f;
  dist_norm = false;
  net_agg.op = Aggregate::LAST;
}

void ComputeStat::Initialize() {
  stat_1 = NULL;
  stat_2 = NULL;
  net_agg.op = Aggregate::AVG;
}

void CopyToEnvStat::Initialize() {
  stat = NULL;
  data_env = NULL;
  accum_scope = SUPER;
  net_agg.op = Aggregate::LAST;
}

void EpochCounterStat::Initialize() {
  net_agg.op = Aggregate::LAST;
}

void ProcCounterStat::Initialize() {
  proc = NULL;
  net_agg.op = Aggregate::LAST;
}

void MaxActTrgStat::Initialize() {
}

void UnitActRFStat::Initialize() {
  data_env = NULL;
  norm_mode = NORM_UNIT;
  net_agg.op = Aggregate::AVG;
}

void UnitActRFStatResetProc::Initialize() {
  unit_act_rf_stat = NULL;
}

void UnitEventRFStat::Initialize() {
  epoch_proc = NULL;
  data_env = NULL;
  net_agg.op = Aggregate::AVG;
}

void UniquePatStat::Initialize() {
  loop_init = INIT_START_ONLY;
  pat_stat = NULL;
  data_env = NULL;
  cmp_type = float_RArray::CORREL;
  dist_tol = 0.0f;
  norm = false;
  uniq_tol = .8f;
  net_agg.op = Aggregate::AVG;
}

void TimeCounterStat::Initialize() {
  net_agg.op = Aggregate::LAST;
  loop_init = INIT_START_ONLY;
}

void TimeCounterStatResetProc::Initialize() {
  time_ctr_stat = NULL;
}

void DispDataEnvProc::Initialize() {
  data_env = NULL;
  pat_no = 0;
  disp_type = RAW_DATA_GRID;
  disp_log = NULL;
  dist_metric = float_RArray::EUCLIDIAN;
  dist_norm = false;
  dist_tol = 0.0f;
  x_axis_component = 0;
  y_axis_component = 1;
}

void DispNetWeightsProc::Initialize() {
  recv_layer = NULL;
  send_layer = NULL;
  grid_log = NULL;
}

void ClearLogProc::Initialize() {
  log_to_clear = NULL;
}

//////////////////////////////////
// 	 V3ProjectBase		//
//////////////////////////////////

void V3ProjectBase::Initialize() {
  specs.SetBaseType(&TA_BaseSpec);
  environments.SetBaseType(&TA_Environment);
  processes.SetBaseType(&TA_SchedProcess);
  //nn logs.SetBaseType(&TA_TextLog);
  scripts.SetBaseType(&TA_Script);
}

void V3ProjectBase::InitLinks_impl() {
  inherited::InitLinks_impl();
  taBase::Own(specs, this);
  taBase::Own(environments, this);
  taBase::Own(processes, this);
  taBase::Own(logs, this);
  taBase::Own(scripts, this);
}

void V3ProjectBase::CutLinks_impl() {
  scripts.CutLinks();
  logs.CutLinks();
  processes.CutLinks();
  environments.CutLinks();
  specs.CutLinks();
  inherited::CutLinks_impl();
}

void V3ProjectBase::Copy_(const V3ProjectBase& cp) {
  specs = cp.specs;
  environments = cp.environments;
  processes = cp.processes;
  logs = cp.logs;
  scripts = cp.scripts;
}

void V3ProjectBase::ConvertToV4() {
  int ch = taMisc::Choice("This will convert the legacy v3.x project to v4.x format -- you may see some error messages but pay more attention to final success or failure message.  Do you want to continue?", "Yes", "No");
  if (ch != 0) return;
  if (ConvertToV4_impl())
    taMisc::Choice("The conversion was successful!", "Ok");
  else
    taMisc::Choice("The conversion failed -- see console for mesesages", "Ok");
}

bool V3ProjectBase::ConvertToV4_impl() {
  taMisc::Error("Unable to convert a basic project -- must be a specific algorithm type!");
  return false;
}

bool V3ProjectBase::ConvertToV4_Enviros(ProjectBase* nwproj) {
  for(int ei=0; ei < environments.size; ei++) {
    Environment* env = environments[ei];
    DataTable* dt = nwproj->data.NewEl(1,&TA_DataTable);
    dt->name = env->name;
    String_Data* gpcol = NULL;
    int st_col = 0;		// starting column for standard fields
    if(env->events.gp.size > 0) {
      gpcol = dt->NewColString("group");
      st_col++;
    }
    String_Data* nmcol = dt->NewColString("name");
    if(env->event_specs.size == 0) continue;
    EventSpec* es = (EventSpec*)env->event_specs[0];
    for(int pi=0; pi < es->patterns.size; pi++) {
      PatternSpec* ps = (PatternSpec*)es->patterns[pi];
      dt->NewColMatrix(DataArray_impl::VT_FLOAT, ps->name, 2, MAX(ps->geom.x,1), MAX(ps->geom.y,1));
    }
    taLeafItr evi;
    Event* ev;
    FOR_ITR_EL(Event, ev, env->events., evi) {
      dt->AddBlankRow();
      if(gpcol) {
	Event_Group* eg = (Event_Group*)ev->GetOwner();
	if(!eg->name.empty())
	  dt->SetValAsString(eg->name, 0, -1); // -1 = last row
      }
      dt->SetValAsString(ev->name, st_col, -1); // -1 = last row
      for(int pi=0; pi < ev->patterns.size; pi++) {
	Pattern* pat = (Pattern*)ev->patterns[pi];
	taMatrix* mat = dt->GetValAsMatrix(st_col + 1 + pi, -1);
	for(int vi=0; vi<pat->value.size; vi++) {
	  mat->SetFmVar_Flat(pat->value[vi], vi);
	}
      }
    }
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_Nets(ProjectBase* nwproj) {
  taLeafItr ni;
  Network* net;
  FOR_ITR_EL(Network, net, networks., ni) {
    net->specs = specs;		// copy specs into network
    net->ReplaceSpecs_Gp(specs, net->specs); // replace pointers

    // convert layer types!
    taLeafItr li;
    Layer* lay;
    FOR_ITR_EL(Layer, lay, net->layers., li) {
      String lnm = lay->name;  lnm.downcase();
      if(lnm.contains("in") || lnm.contains("stim"))
	lay->layer_type = Layer::INPUT;
      else if(lnm.contains("out") || lnm.contains("trg") || lnm.contains("targ")
	      || lnm.contains("resp"))
	lay->layer_type = Layer::TARGET;
      else
	lay->layer_type = Layer::HIDDEN;
    }
  }

  nwproj->networks = networks;	// this should the do spec updating automatically!

  FOR_ITR_EL(Network, net, nwproj->networks., ni) {
    net->ShowInViewer();
    net->Build();
    net->Connect();
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_DefaultApplyInputs(ProjectBase* nwproj) {
  Program* apply_ins = ((Program_Group*)nwproj->programs.gp[0])->FindName("ApplyInputs");
  if(!apply_ins) return false;
  
  LayerWriter_List* lw_list = (LayerWriter_List*)apply_ins->objs.FindName("lw_list");
  if(!lw_list) return false;

  if(environments.leaves <= 0) return false;
  Environment* env = (Environment*)environments.Leaf(0);
  if(env->event_specs.size <= 0) return false;
  EventSpec* es = (EventSpec*)env->event_specs[0];

  DataTable* dt = (DataTable*)nwproj->data.FindName(env->name);
  if(!dt) return false;
  
  if(networks.leaves <= 0) return false;
  Network* net = (Network*)nwproj->networks.Leaf(0);

  return ConvertToV4_ApplyInputs(lw_list, es, net, dt);
}

bool V3ProjectBase::ConvertToV4_ApplyInputs(LayerWriter_List* lw_list, EventSpec* es,
					    Network* net, DataTable* dt) {
  lw_list->Reset();
  for(int i=0; i<es->patterns.size; i++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[i];
    if(ps->type == PatternSpec::INACTIVE) continue;
    LayerWriter* lw = (LayerWriter*)lw_list->New(1, &TA_LayerWriter);
    lw->data = dt;
    lw->chan_name = ps->name;
    Layer* lay = (Layer*)net->layers.FindName(ps->layer_name);
    lw->layer = lay;
    lw->value_names = ps->value_names;
    if(lay) {
      if(ps->type == PatternSpec::INPUT) lay->layer_type = Layer::INPUT;
      else if(ps->type == PatternSpec::TARGET) lay->layer_type = Layer::TARGET;
      else if(ps->type == PatternSpec::COMPARE) lay->layer_type = Layer::OUTPUT;
    }
  }
  return true;
}
