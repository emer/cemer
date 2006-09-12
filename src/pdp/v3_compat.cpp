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

#include "leabra.h"
#include "leabra_v3_compat.h"


#ifdef TA_GUI
#include "ta_qtviewer.h"
#include "ta_qtbrowse.h"
#include <QMessageBox>

//////////////////////////
// 	CtrlPanelData	//
//////////////////////////

void CtrlPanelData::Initialize() {
  active = false;
  lft = 0.0f;
  top = 0.0f;
}

void CtrlPanelData::Copy_(const CtrlPanelData& cp) {
  active = cp.active;
  lft = cp.lft;
  top = cp.top;
}
#endif


//////////////////////////
// 	Process		//
//////////////////////////

void Process::Initialize() {
  type=C_CODE;
  project = NULL;
  network = NULL;
  environment = NULL;
}

void Process::InitLinks() {
  inherited::InitLinks();
  taBase::Own(mod, this);
  taBase::Own(rndm_seed, this);
#ifdef TA_GUI
  taBase::Own(ctrl_panel, this);
#endif
  SetDefaultPNEPtrs();
  if(script_file->fname.empty()) { // initialize only on startup up, not transfer
    SetScript("");		// open the file thing
    type = C_CODE;		// reset to C_Code after setting script..
  }
}

void Process::Destroy() {
  CutLinks();
}

void Process::CutLinks() {
  static bool in_repl = false;
  if(in_repl || (owner == NULL)) return; // already replacing or already dead
  taBase::DelPointer((TAPtr*)&network);
  taBase::DelPointer((TAPtr*)&environment);
#ifdef TA_GUI
  ctrl_panel.CutLinks();
#endif
  inherited::CutLinks();
}


void Process::Copy_(const Process& cp) {
  rndm_seed = cp.rndm_seed;
  type = cp.type;
  mod = cp.mod;
  taBase::SetPointer((TAPtr*)&network, cp.network);
  taBase::SetPointer((TAPtr*)&environment, cp.environment);
  script_file = cp.script_file;
  script_string = cp.script_string;
#ifdef TA_GUI
  ctrl_panel = cp.ctrl_panel;
#endif
}

void Process::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  if((!script_file->fname.empty() && script_file->file_selected) || !script_string.empty())
    type = SCRIPT;
/*obs  if(type == SCRIPT) {
    if((script == NULL) || (script->run == cssEl::Waiting)
       || (script->run == cssEl::Stopping))
//      UpdateReCompile();
  } */
}

void Process::CopyPNEPtrs(Network* net, Environment* env) {
  taBase::SetPointer((TAPtr*)&network, net);
  taBase::SetPointer((TAPtr*)&environment, env);
}

void Process::SetDefaultPNEPtrs() {
  if(project == NULL)
    project = GET_MY_OWNER(Project);
  if(project == NULL)	return;

  if(network == NULL)
    taBase::SetPointer((TAPtr*)&network, project->networks.DefaultEl());
  if(environment == NULL)
    taBase::SetPointer((TAPtr*)&environment, project->environments.DefaultEl());
}

void Process::LoadScript(const char* nm) {
  ScriptBase::LoadScript(nm);
  type = SCRIPT;
}

bool Process::RunScript() {
  if(type == C_CODE) return false;
  return ScriptBase::RunScript();
}

/////////////////////////
//	DataItem	//
//////////////////////////

void DataItem::Initialize() {
  is_string = false;
  vec_n = 0;
  disp_opts = " ";		// always pad with initial blank
}

void DataItem::Copy_(const DataItem& cp) {
  name = cp.name;
  disp_opts = cp.disp_opts;
  is_string = cp.is_string;
  vec_n = cp.vec_n;
}

void DataItem::SetStringName(const char* nm) {
  name = "$";
  name += nm;
  is_string = true;
}

void DataItem::SetNarrowName(const char* nm) {
  name = "|";
  name += nm;
  AddDispOption("NARROW");
}

void DataItem::SetFloatVecNm(const char* nm, int n) {
  if(n > 1) {
    name = String("<") + (String)n + ">" + nm;
    vec_n = n;
  }
  else {
    name = nm;
    vec_n = 0;
  }
}

void DataItem::SetStringVecNm(const char* nm, int n) {
  is_string = true;
  if(n > 1) {
    name = String("$<") + (String)n + ">" + nm;
    vec_n = n;
  }
  else {
    name = String("$") + nm;
    vec_n = 0;
  }
}

void DataItem::AddDispOption(const char* opt) {
  String nm = " ";		// pad with preceding blank to provide start cue
  nm += String(opt) + ",";
  if(HasDispOption(nm))
    return;
  disp_opts += nm;
}

//////////////////////////
//  	StatVal     	//
//////////////////////////

void StatVal::Initialize() {
  val = 0.0f;
}

void StatVal::InitLinks() {
  DataItem::InitLinks();
  taBase::Own(stopcrit, this);
}

void StatVal::Copy_(const StatVal& cp) {
  val = cp.val;
  str_val = cp.str_val;
  stopcrit = cp.stopcrit;
}

//////////////////////////
//  	AggStat     	//
//////////////////////////

void AggStat::Initialize() {
  from = NULL;
  real_stat = NULL;
  type_safe = true;
}

void AggStat::CutLinks() {
  taBase::DelPointer((TAPtr*)&from);
  StatValAgg::CutLinks();
}

void AggStat::Copy_(const AggStat& cp) {
  //  taBase::SetPointer((TAPtr*)&from,cp.from);
  type_safe = cp.type_safe;
}

//////////////////////////
//  	Stat     	//
//////////////////////////

void Stat::Initialize() {
  own_proc = NULL;
  has_stop_crit = false;
  n_copy_vals = 0;
  loop_init = INIT_IN_LOOP;
  log_stat = true;
  layer = NULL;
}

void Stat::InitLinks() {
  Process::InitLinks();
  own_proc = GET_MY_OWNER(SchedProcess);
  taBase::Own(time_agg, this);
  taBase::Own(net_agg, this);
  taBase::Own(copy_vals, this);
  // own all the stats in the object
  // just in this one case..
  TypeDef*   td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++){
    MemberDef* md=td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal))
      taBase::Own((StatVal*)md->GetOff(this), this);
    else if(md->type->InheritsFrom(&TA_StatVal_List))
      taBase::Own((StatVal_List*) md->GetOff(this), this);
  }
  Init();
}

void Stat::CutLinks() {
  copy_vals.CutLinks();
  time_agg.CutLinks();
  net_agg.CutLinks();
  taBase::DelPointer((TAPtr*)&layer);
  own_proc = NULL;
  Process::CutLinks();
}

void Stat::Copy_(const Stat& cp) {
  //  has_stop_crit = cp.has_stop_crit;
  //  n_copy_vals = cp.n_copy_vals;
  loop_init = cp.loop_init;
  log_stat = cp.log_stat;
  time_agg = cp.time_agg;
  net_agg = cp.net_agg;
  copy_vals = cp.copy_vals;
  taBase::SetPointer((TAPtr*)&layer, cp.layer);
}

//////////////////////////////////
// 	Stat_Group		//
//////////////////////////////////

bool Stat_Group::Close_Child(TAPtr obj) {
  SchedProcess* sp = GET_MY_OWNER(SchedProcess);
  if(sp != NULL) {
    taMisc::DelayedMenuUpdate(sp);
  }
  return Remove(obj);		// otherwise just nuke it
}


//////////////////////////
// 	SE_Stat		//
//////////////////////////

void SE_Stat::Initialize() {
  tolerance = 0.0f;
}

///////////////////////////
// 	Monitor Stat	 //
///////////////////////////

void MonitorStat::Initialize() {
  net_agg.op = Aggregate::COPY;
  variable = "act";
}

void MonitorStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(objects,this);
  taBase::Own(ptrs,this);
  objects.SetBaseType(&TA_taBase);
  ptrs.SetBaseType(&TA_taBase);
  taBase::Own(pre_proc_1,this);
  taBase::Own(pre_proc_2,this);
  taBase::Own(pre_proc_3,this);
}

void MonitorStat::CutLinks() {
  MonitorStat* rstat = (MonitorStat*)time_agg.real_stat;
  if(rstat != NULL) {
    int i;
    for(i=0;i<rstat->objects.size;i++) {
      TAPtr obj = rstat->objects.FastEl(i);
      if(obj == NULL)
	continue;
      Network* net = GET_OWNER(obj, Network);
      if(net != NULL)
	taMisc::DelayedMenuUpdate(net);
    }
  }
  objects.RemoveAll();
  ptrs.RemoveAll();
  Stat::CutLinks();
}

void MonitorStat::Copy_(const MonitorStat& cp) {
  mon_vals = cp.mon_vals;
  objects.BorrowUnique(cp.objects);	// borrow because its a link group
  variable = cp.variable;
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
}

//////////////////////////
// 	StepParams	//
//////////////////////////

void StepParams::Initialize() {
  owner = NULL;
  proc = NULL;
  n = 1;
}

void StepParams::InitLinks() {
  taBase::InitLinks();
  taBase::SetPointer((TAPtr*)&proc, owner);
}

void StepParams::CutLinks() {
  owner = NULL;
  taBase::DelPointer((TAPtr*)&proc);
  taBase::CutLinks();
}

void StepParams::Copy_(const StepParams& cp) {
  n = cp.n;
}

TAPtr 	StepParams::GetOwner() const	{ return (taBase*)owner; }
TAPtr	StepParams::GetOwner(TypeDef* tp) const { return taBase::GetOwner(tp); }
TAPtr 	StepParams::SetOwner(TAPtr ta)	{ owner = (SchedProcess*)ta; return ta; }


//////////////////////////
// 	SchedProcess	//
//////////////////////////

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

void SchedProcess::InitLinks() {
  inherited::InitLinks();
  TypeDef* td = GetTypeDef();
  int i;
  for(i=TA_SchedProcess.members.size;i<td->members.size;i++){
    MemberDef* md = td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_Counter)){
      cntr = (Counter *) md->GetOff(this);
      cntr->name = md->name;
      break;
    }
  }
  taBase::Own(step, this);
  taBase::Own(loop_stats, this);
  taBase::Own(final_stats, this);
  taBase::Own(init_procs, this);
  taBase::Own(loop_procs, this);
  taBase::Own(final_procs, this);
//TODO  taBase::Own(displays, this);
  taBase::Own(logs, this);
  taBase::Own(cntr_items, this);
}

void SchedProcess::CutLinks() {
  logs.CutLinks();
//TODO  displays.CutLinks();
  loop_stats.CutLinks();
  final_stats.CutLinks();
  init_procs.CutLinks();
  loop_procs.CutLinks();
  final_procs.CutLinks();
  super_proc = NULL;
  cntr = NULL;
  step.CutLinks();
  if(sub_proc != NULL) {
    Process_Group* gp = GET_MY_OWNER(Process_Group);
    if(gp != NULL)
      gp->RemoveLeaf(sub_proc);
  }
  taBase::DelPointer((TAPtr*)&sub_proc);
  inherited::CutLinks();
}

void SchedProcess::Copy_(const SchedProcess& cp) {
  //  cntr_items = cp.cntr_items;   // do not copy this!  bad when changing types
  re_init = cp.re_init;
  step = cp.step;
  loop_stats = cp.loop_stats;
  final_stats = cp.final_stats;
  init_procs = cp.init_procs;
  loop_procs = cp.loop_procs;
  final_procs = cp.final_procs;
  log_loop = cp.log_loop;
  log_counter = cp.log_counter;
}

SchedProcess* SchedProcess::FindSubProc(TypeDef* td) {
  SchedProcess* sp = sub_proc;
  while((sp != NULL) && !(sp->InheritsFrom(td)))
    sp = sp->sub_proc;
  return sp;
}

SchedProcess* SchedProcess::FindSuperProc(TypeDef* td) {
  SchedProcess* sp = super_proc;
  while((sp != NULL) && !(sp->InheritsFrom(td)))
    sp = sp->super_proc;
  return sp;
}

SchedProcess* SchedProcess::FindProcOfType(TypeDef* td) {
  if(InheritsFrom(td))
    return this;
  else {
    SchedProcess* rval = FindSuperProc(td);
    if(rval == NULL)
      rval = FindSubProc(td);
    return rval;
  }
}

SchedProcess* SchedProcess::GetTopProc() {
  SchedProcess* sp = this;
  while(sp->super_proc != NULL) sp = sp->super_proc;
  return sp;
}

//////////////////////////
// 	CycleProcess	//
//////////////////////////

void CycleProcess::Initialize() {
  sub_proc_type = NULL;
}

//////////////////////////
// 	SettleProcess	//
//////////////////////////

void SettleProcess::Initialize() {
  sub_proc_type = &TA_CycleProcess;
  cycle.SetMax(100);
  log_counter = true;
}

void SettleProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(cycle, this);
  TrialProcess* tp = (TrialProcess*)FindSuperProc(&TA_TrialProcess);
/*NN??  if(tp != NULL) {
    CyclesToSettle* cs = (CyclesToSettle*)tp->loop_stats.FindType(&TA_CyclesToSettle);
    if(cs != NULL)
      cs->settle = this;
  } */
}


//////////////////////////
// 	TrialProcess	//
//////////////////////////

void TrialProcess::Initialize() {
  sub_proc_type = NULL;
  cur_event = NULL;
  epoch_proc = NULL;
  enviro_group = NULL;
}

void TrialProcess::CutLinks() {
  epoch_proc = NULL;
  taBase::DelPointer((TAPtr*)&cur_event);
  taBase::DelPointer((TAPtr*)&enviro_group);
  SchedProcess::CutLinks();
}

void TrialProcess::Copy_(const TrialProcess& cp) {
  taBase::SetPointer((TAPtr*)&cur_event, cp.cur_event);
  taBase::SetPointer((TAPtr*)&enviro_group, cp.enviro_group);
}

//////////////////////////
// 	EpochProcess	//
//////////////////////////

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

void EpochProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(event_list, this);
  taBase::Own(trial, this);
}

void EpochProcess::CutLinks() {
  taBase::DelPointer((TAPtr*)&cur_event);
  taBase::DelPointer((TAPtr*)&enviro_group);
  SchedProcess::CutLinks();
}

void EpochProcess::Copy_(const EpochProcess& cp) {
  trial = cp.trial;
  taBase::SetPointer((TAPtr*)&cur_event,cp.cur_event);
  order = cp.order;
  wt_update = cp.wt_update;
  batch_n = cp.batch_n;
  batch_n_eff = cp.batch_n_eff;
  event_list = cp.event_list;
  taBase::SetPointer((TAPtr*)&enviro_group,cp.enviro_group);
  dmem_nprocs = cp.dmem_nprocs;
}

//////////////////////////////////
// 	SequenceProcess		//
//////////////////////////////////

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

void SequenceProcess::Destroy() {
  CutLinks();
}

void SequenceProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(tick, this);
}

void SequenceProcess::CutLinks() {
  sequence_epoch = NULL;
  taBase::DelPointer((TAPtr*)&cur_event_gp);
  taBase::DelPointer((TAPtr*)&cur_event);
  taBase::DelPointer((TAPtr*)&enviro_group);
  SchedProcess::CutLinks();
}

void SequenceProcess::Copy_(const SequenceProcess& cp) {
  tick = cp.tick;
  taBase::SetPointer((TAPtr*)&cur_event,cp.cur_event);
  taBase::SetPointer((TAPtr*)&cur_event_gp,cp.cur_event_gp);
  order = cp.order;
  sequence_init = cp.sequence_init;
  event_list = cp.event_list;
  taBase::SetPointer((TAPtr*)&enviro_group,cp.enviro_group);
}

//////////////////////////////////
// 	SequenceEpoch		//
//////////////////////////////////

void SequenceEpoch::Initialize() {
  sub_proc_type = &TA_SequenceProcess;
  small_batch = SEQUENCE;	// prior default setting..
  cur_event_gp = NULL;
}

void SequenceEpoch::Destroy() {
  CutLinks();
}

void SequenceEpoch::CutLinks() {
  taBase::DelPointer((TAPtr*)&cur_event_gp);
  EpochProcess::CutLinks();
}

void SequenceEpoch::Copy_(const SequenceEpoch& cp) {
  taBase::SetPointer((TAPtr*)&cur_event_gp, cp.cur_event_gp);
}

//////////////////////////////////
// 	InteractiveEpoch	//
//////////////////////////////////

void InteractiveEpoch::Initialize() {
  order = SEQUENTIAL;
  last_trial_val = -1;
}


//////////////////////////
// 	NEpochProcess	//
//////////////////////////

void NEpochProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  epc_ctr.SetMax(50);
  epoch_proc = NULL;
  log_counter = true;
  epoch = 0;
}

void NEpochProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(epc_ctr, this);
}

void NEpochProcess::CutLinks() {
  epoch_proc = NULL;
  SchedProcess::CutLinks();
}

//////////////////////////
// 	TrainProcess	//
//////////////////////////

void TrainProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  epoch.SetMax(1000);
  epoch_proc = NULL;
  log_counter = true;
}

void TrainProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(epoch, this);
}

void TrainProcess::CutLinks() {
  epoch_proc = NULL;
  SchedProcess::CutLinks();
}

//////////////////////////
// 	BatchProcess	//
//////////////////////////

void BatchProcess::Initialize() {
  sub_proc_type = &TA_TrainProcess;
  batch.SetMax(10);
}

void BatchProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(batch, this);
}


//////////////////////
//   PatternSpec    //
//////////////////////

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

void PatternSpec::InitLinks(){
  BaseSubSpec::InitLinks();
  taBase::Own(pos, this);
  taBase::Own(geom, this);
  taBase::Own(noise,this);
  taBase::Own(value_names, this);
  taBase::Own(global_flags, this);
  EventSpec* evs = GET_MY_OWNER(EventSpec);
  if(evs == NULL)
    return;
  int index = evs->patterns.FindLeaf(this);
}

void PatternSpec::Destroy() {
  CutLinks();
}

void PatternSpec::CutLinks() {
  taBase::DelPointer((TAPtr*)&layer);
  BaseSubSpec::CutLinks();
}

void PatternSpec::Copy_(const PatternSpec& cp) {
  type=cp.type;
  to_layer=cp.to_layer;
  layer_name=cp.layer_name;
  layer_num = cp.layer_num;
  taBase::SetPointer((TAPtr*)&layer,cp.layer);
  pattern_type = cp.pattern_type;
  layer_flags = cp.layer_flags;
  use_flags = cp.use_flags;
  n_vals = cp.n_vals;
  geom = cp.geom;
  pos = cp.pos;
  initial_val = cp.initial_val;
  noise = cp.noise;
  value_names = cp.value_names;
  global_flags = cp.global_flags;
}

float PatternSpec::Value(Pattern* pat, int index) {
  return pat->value.SafeEl(index);
}

int PatternSpec::Flag(PatUseFlags flag_type, Pattern* pat, int index) {
  static int noflags = NO_FLAGS;
  switch(flag_type) {
  case USE_NO_FLAGS:
    return noflags;
  case USE_PATTERN_FLAGS:
    return pat->flag.SafeEl(index);
  case USE_GLOBAL_FLAGS:
    return global_flags.SafeEl(index);
  case USE_PAT_THEN_GLOBAL_FLAGS: {
    int flags = pat->flag.SafeEl(index);
    if(flags == NO_FLAGS)
      return global_flags.SafeEl(index);
    return flags;
  }
  }
  return noflags;
}


////////////////////////////
//   PatternSpec_Group    //
////////////////////////////

void PatternSpec_Group::Initialize() {
  pat_gp_type = &TA_Pattern_Group;
  SetBaseType(&TA_PatternSpec);
}

void PatternSpec_Group::Destroy() {
  CutLinks();
}

void PatternSpec_Group::Copy_(const PatternSpec_Group& cp) {
  pat_gp_type = cp.pat_gp_type;
}

void PatternSpec_Group::CutLinks() {
  if((pat_gp_type == NULL) || (owner == NULL)) {
    taBase_Group::CutLinks();
    return;
  }
  pat_gp_type = NULL;		// don't do it more than once

  if(!owner->InheritsFrom(TA_taSubGroup)) { // only for created groups
    taBase_Group::CutLinks();
    return;
  }
  EventSpec* es = GET_MY_OWNER(EventSpec);
  if(es == NULL) {
    taBase_Group::CutLinks();
    return;
  }
  Environment* env = GET_OWNER(es,Environment);
  if(env == NULL) {
    taBase_Group::CutLinks();
    return;
  }
  String my_path = GetPath(NULL, es); // my path is same as corresp pat group
  Event* ev;
  taLeafItr e;
  FOR_ITR_EL(Event, ev, env->events., e) {
    if(ev->spec.spec != es)
      continue;			// only for events that use me!
    MemberDef* md;
    Pattern_Group* pg = (Pattern_Group*)ev->FindFromPath(my_path, md);
    if((pg == NULL) || !pg->InheritsFrom(TA_Pattern_Group) ||
       !pg->owner->InheritsFrom(TA_taSubGroup))
      continue;
    taSubGroup* og = (taSubGroup*)pg->owner;
    og->Remove(pg);
  }
  inherited::CutLinks();
}


////////////////////////////
//        EventSpec       //
////////////////////////////

void EventSpec::Initialize() {
  last_net = NULL;
  pattern_layout = HORIZONTAL;
}

void EventSpec::InitLinks() {
  BaseSpec::InitLinks();
  taBase::Own(patterns, this);
}

void EventSpec::CutLinks() {
  patterns.CutLinks();
  taBase::DelPointer((TAPtr*)&last_net);
  BaseSpec::CutLinks();
}

void EventSpec::Copy(const EventSpec& cp) {
  BaseSpec::Copy(cp);
  patterns = cp.patterns;
  pattern_layout = cp.pattern_layout;
}


BaseSpec_Group* EventSpec_SPtr::GetSpecGroup() {
  Environment* env = GET_OWNER(owner,Environment);
  if(env == NULL)
    return NULL;
  return &(env->event_specs);
}

////////////////////////////
//        Pattern         //
////////////////////////////

void Pattern::Initialize() {
}

void Pattern::InitLinks() {
  taBase::Own(value, this);
  taBase::Own(flag, this);
  taOBase::InitLinks();
}

void Pattern::Copy_(const Pattern& cp) {
  value = cp.value;
  flag = cp.flag;
}

void Pattern::CutLinks() {
  value.Reset();
  flag.Reset();
  taOBase::CutLinks();
}

//////////////////////
//      Event       //
//////////////////////


void Event::Initialize() {
  index = -1;
  patterns.SetBaseType(&TA_Pattern);
}

void Event::Destroy() {
  CutLinks();
}

void Event::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(patterns, this);
  spec.SetDefaultSpec(this);
}

void Event::CutLinks() {
  index = -1;
  patterns.Reset();
  spec.CutLinks();
  taNBase::CutLinks();
}

void Event::Copy(const Event& cp) {
  taNBase::Copy(cp);
  spec = cp.spec;
  patterns = cp.patterns;
//  GetLocalSpec();
}


//////////////////////
//   Environment    //
//////////////////////

void Environment::Initialize() {
  events.SetBaseType(&TA_Event);
  event_specs.SetBaseType(&TA_EventSpec);
//TODO  views.SetBaseType(&TA_EnviroView);
  event_ctr = 0;
}

void Environment::Destroy(){
  CutLinks();
}

void Environment::InitLinks() {
  taBase::Own(events, this);
  taBase::Own(event_specs, this);
  inherited::InitLinks();
}

void Environment::CutLinks() {
  event_specs.CutLinks();
  events.CutLinks();
  inherited::CutLinks();	// winmgrs cut views first..
}

void Environment::Copy(const Environment& cp) {
  inherited::Copy(cp);
  event_specs = cp.event_specs;
  events = cp.events;
  event_ctr = cp.event_ctr;
}

int Environment::GroupCount() {
  return events.LeafGpCount();
}

Event_Group* Environment::GetGroup(int i) {
  return (Event_Group*)events.SafeLeafGp(i);
}


//////////////////////////
//       ScriptEnv 	//
//////////////////////////

void ScriptEnv::InitLinks() {
  Environment::InitLinks();
  taBase::Own(s_args, this);
  if(script_file->fname.empty())	// initialize only on startup up, not transfer
    SetScript("");
}

void ScriptEnv::Copy_(const ScriptEnv& cp) {
  s_args = cp.s_args;
  *script_file = *(cp.script_file);
  script_string = cp.script_string;
}

void ScriptEnv::UpdateAfterEdit() {
  Environment::UpdateAfterEdit();
//v3  UpdateReCompile();
  CompileScript(true);
}


//////////////////////////
//      Frequency 	//
//////////////////////////

void FreqEvent::Initialize() {
  frequency = 1.0f;
}

void FreqEvent::Copy_(const FreqEvent& cp) {
  frequency = cp.frequency;
}

void FreqEvent_Group::Initialize() {
  frequency = 1.0f;
  fenv = NULL;
}

void FreqEvent_Group::InitLinks() {
  Event_Group::InitLinks();
  taBase::Own(list, this);
  fenv = GET_MY_OWNER(FreqEnv);
}

void FreqEvent_Group::CutLinks() {
  fenv = NULL;
  Event_Group::CutLinks();
}

void FreqEvent_Group::Copy_(const FreqEvent_Group& cp) {
  frequency = cp.frequency;
  list = cp.list;
}


//////////////////////////
//      FreqEnv 	//
//////////////////////////


void FreqEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP_EVENT;
  sample_type = PERMUTED;
  events.SetBaseType(&TA_FreqEvent);
  events.gp.SetBaseType(&TA_FreqEvent_Group);
}

void FreqEnv::InitLinks() {
  Environment::InitLinks();
  taBase::Own(list, this);
  events.gp.SetBaseType(&TA_FreqEvent_Group);
}

void FreqEnv::CutLinks() {
  list.CutLinks();
  Environment::CutLinks();
}

void FreqEnv::Copy_(const FreqEnv& cp) {
  n_sample = cp.n_sample;
  freq_level = cp.freq_level;
  sample_type = cp.sample_type;
  list = cp.list;
}


int FreqEnv::EventCount() {
  if(freq_level == EVENT) {
    return list.size;
  }
  return Environment::EventCount();
}

Event* FreqEnv::GetEvent(int i) {
  if(freq_level == EVENT) {
    return (Event*)events.Leaf(list.SafeEl(i));
  }
  return Environment::GetEvent(i);
}

int FreqEnv::GroupCount() {
  if((freq_level == GROUP) || (freq_level == GROUP_EVENT)) {
    return list.size;
  }
  return Environment::GroupCount();
}

Event_Group* FreqEnv::GetGroup(int i) {
  if((freq_level == GROUP) || (freq_level == GROUP_EVENT)) {
    return (Event_Group*)events.SafeLeafGp(list.SafeEl(i));
  }
  return Environment::GetGroup(i);
}


//////////////////////////
//  TimeEvent_Group 	//
//////////////////////////

void TimeEvent_Group::Initialize() {
  end_time = 1.0f;
  interpolate = USE_ENVIRO;
  SetBaseType(&TA_TimeEvent);
}

void TimeEvent_Group::Copy_(const TimeEvent_Group& cp) {
  interpolate = cp.interpolate;
  end_time = cp.end_time;
}

void TimeEvent_Group::UpdateAfterEdit() {
  Event_Group::UpdateAfterEdit();
  if(leaves == 0)
    return;
  TimeEvent* ev = (TimeEvent*)Leaf(leaves-1);
  if(!ev->InheritsFrom(TA_TimeEvent))
    return;
  if(end_time < ev->time)	// make sure end_time is big enough
    end_time = ev->time;
}

void TimeEnvironment::Initialize() {
  events.SetBaseType(&TA_TimeEvent);
  events.gp.SetBaseType(&TA_TimeEvent_Group);
  interpolate = CONSTANT;
}

void TimeEnvironment::InitLinks() {
  Environment::InitLinks();
  events.gp.SetBaseType(&TA_TimeEvent_Group);
}

void TimeEnvironment::Copy_(const TimeEnvironment& cp) {
  interpolate = cp.interpolate;
}


//////////////////////////
//  FreqTimeEnv 	//
//////////////////////////

void FreqTimeEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP;		// group is typically used with time
  sample_type = RANDOM;
  events.SetBaseType(&TA_FreqTimeEvent);
  events.gp.SetBaseType(&TA_FreqTimeEvent_Group);
}

void FreqTimeEnv::InitLinks() {
  TimeEnvironment::InitLinks();
  taBase::Own(list, this);
  events.gp.SetBaseType(&TA_FreqTimeEvent_Group);
}

void FreqTimeEnv::Copy_(const FreqTimeEnv& cp) {
  n_sample = cp.n_sample;
  freq_level = cp.freq_level;
  sample_type = cp.sample_type;
  list = cp.list;
}

int FreqTimeEnv::EventCount() {
  if(freq_level == EVENT) {
    return list.size;
  }
  return TimeEnvironment::EventCount();
}

Event* FreqTimeEnv::GetEvent(int i) {
  if(freq_level == EVENT) {
    return (Event*)events.Leaf(list.SafeEl(i));
  }
  return TimeEnvironment::GetEvent(i);
}

int FreqTimeEnv::GroupCount() {
  if(freq_level == GROUP) {
    return list.size;
  }
  return TimeEnvironment::GroupCount();
}

Event_Group* FreqTimeEnv::GetGroup(int i) {
  if(freq_level == GROUP) {
    return (Event_Group*)events.SafeLeafGp(list.SafeEl(i));
  }
  return TimeEnvironment::GetGroup(i);
}


//////////////////////////
//     Probability 	//
//////////////////////////

void ProbPattern::Initialize() {
  prob = 1.0f;
  applied = false;
}

void ProbPattern::Copy_(const ProbPattern& cp) {
  prob = cp.prob;
}

void ProbPattern::InitLinks() {
  Pattern::InitLinks();
  Event* e = GET_MY_OWNER(Event);
  if((e != NULL) && (e->spec.spec != NULL) &&
     (e->spec.spec->InheritsFrom(TA_ProbEventSpec))) {
    prob = ((ProbEventSpec*)e->spec.spec)->default_prob;
  }
}

void ProbPatternSpec_Group::UpdateAfterEdit() {
  PatternSpec_Group::UpdateAfterEdit();

  last_pat = -1;
}

void ProbPatternSpec_Group::CutLinks() {
  PatternSpec_Group::CutLinks();

  last_pat = -1;
}

void ProbPatternSpec_Group::Initialize() {
  last_pat = -1;
}

void ProbEventSpec::Initialize() {
  patterns.gp.el_typ = &TA_ProbPatternSpec_Group;
  default_prob = .25f;
}


//////////////////////////
// 	XY Patterns	//
//////////////////////////

void XYPatternSpec::Initialize() {
  pattern_type = &TA_XYPattern;
  wrap = false;
  apply_background = false;
  background_value = 0.0f;
}

//////////////////////////
// 	XY Subset	//
//////////////////////////

void XYSubPatternSpec::Initialize() {
  pattern_type = &TA_XYSubPattern;
  wrap = false;
}

//////////////////////////
//     GroupPattern	//
//////////////////////////

void GroupPatternSpec::Initialize() {
  sub_geom.SetGtEq(1);
  TDCoord& gg = gp_geom;
  gg = geom / sub_geom;
  trans_apply = false;
}

void GroupPatternSpec::UpdateAfterEdit() {
  PatternSpec::UpdateAfterEdit();
  sub_geom.SetGtEq(1);		// no dividing by zero!
  TDCoord& gg = gp_geom;
  gg = geom / sub_geom;
}

int GroupPatternSpec::FlatToValueIdx(int index) {
  int mx = gp_geom.x * sub_geom.x;
  int y = index / mx;
  int x = index % mx;
  TwoDCoord gpc, sbc;
  gpc.y = y / sub_geom.y;
  sbc.y = y % sub_geom.y;
  gpc.x = x / sub_geom.x;
  sbc.x = x % sub_geom.x;
  return CoordToValueIdx(gpc, sbc);
}

int GroupPatternSpec::CoordToValueIdx(const TwoDCoord& gpc, const TwoDCoord& sbc) {
  int gpno = gpc.y * gp_geom.x + gpc.x;
  int rval = gpno * (sub_geom.x * sub_geom.y);
  rval += sbc.y * sub_geom.x + sbc.x;
  return rval;
}

int GroupPatternSpec::ValueToFlatIdx(int index) {
  int mx = sub_geom.x * sub_geom.y;
  int gpno = index / mx;
  int sbno = index % mx;
  TwoDCoord gpc, sbc;
  gpc.y = gpno / gp_geom.x;
  gpc.x = gpno % gp_geom.x;
  sbc.y = sbno / sub_geom.x;
  sbc.x = sbno % sub_geom.x;
  return CoordToFlatIdx(gpc, sbc);
}

int GroupPatternSpec::CoordToFlatIdx(const TwoDCoord& gpc, const TwoDCoord& sbc) {
  int x = gpc.x * sub_geom.x + sbc.x;
  int y = gpc.y * sub_geom.y + sbc.y;
  return (y * gp_geom.x * sub_geom.x) + x;
}

float GroupPatternSpec::Value(Pattern* pat, int index) {
  return pat->value.FastEl(FlatToValueIdx(index));
}

int GroupPatternSpec::Flag(PatUseFlags flag_type, Pattern* pat, int index) {
  int idx = FlatToValueIdx(index);
  return PatternSpec::Flag(flag_type, pat, idx);
}


//////////////////////////
//  Project		//
//////////////////////////

void Project::Initialize() {
  specs.SetBaseType(&TA_BaseSpec);
  environments.SetBaseType(&TA_Environment);
  processes.SetBaseType(&TA_SchedProcess);
  //nn logs.SetBaseType(&TA_TextLog);
  scripts.SetBaseType(&TA_Script);
}

void Project::InitLinks() {
  inherited::InitLinks();
  taBase::Own(specs, this);
  taBase::Own(environments, this);
  taBase::Own(processes, this);
  taBase::Own(logs, this);
  taBase::Own(scripts, this);
}

void Project::CutLinks() {
  deleting = true;
  scripts.CutLinks();
  logs.CutLinks();
  processes.CutLinks();
  environments.CutLinks();
  specs.CutLinks();
  inherited::CutLinks();
}

void Project::Copy_(const Project& cp) {
  specs = cp.specs;
  environments = cp.environments;
  processes = cp.processes;
  logs = cp.logs;
  scripts = cp.scripts;
}

void Project::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  //TODO: here is maybe where we can trap having loaded legacy, and convert
}

void Project::ConvertToV4() {
  int ch = taMisc::Choice("This will convert the legacy v3.x project to v4.x format. The new project will have the old name with a _v4 suffix. Do you want to continue?", "Yes", "No");
  if (ch != 0) return;
  if (ConvertToV4_impl())
    taMisc::Choice("The conversion was successful!", "Ok");
  else
    taMisc::Choice("The conversion failed -- see console for mesesages", "Ok");
}

bool Project::ConvertToV4_impl() {
  if(networks.size == 0) {
    taMisc::Error("No network found: cannot convert project!");
    return false;
  }
  Network* net = (Network*)networks[0];
  if(net->InheritsFrom(&TA_LeabraNetwork)) {
    ConvertToV4_Leabra();
  }
  else {
    taMisc::Error("Unable to convert non-Leabra projects at this point, sorry!");
    return false;
  }
}

bool Project::ConvertToV4_Enviros(ProjectBase* nwproj) {
  for(int ei=0; ei < environments.size; ei++) {
    Environment* env = environments[ei];
    DataTable* dt = nwproj->data.NewEl(1,&TA_DataTable);
    dt->name = env->name;
    String_Data* nmcol = dt->NewColString("name");
    int st_pat_col = 1;		// starting pattern column
    String_Data* gpcol = NULL;
    if(env->events.gp.size > 0) {
      gpcol = dt->NewColString("group");
      st_pat_col++;
    }
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
      dt->SetValAsString(ev->name, 0, -1); // last row
      if(gpcol) {
	Event_Group* eg = (Event_Group*)ev->GetOwner();
	if(!eg->name.empty())
	  dt->SetValAsString(eg->name, 1, -1); // last row
      }
      for(int pi=0; pi < ev->patterns.size; pi++) {
	Pattern* pat = (Pattern*)ev->patterns[pi];
	taMatrix* mat = dt->GetValAsMatrix(st_pat_col + pi, -1);
	for(int vi=0; vi<pat->value.size; vi++) {
	  mat->SetFmVar_Flat(pat->value[vi], vi);
	}
      }
    }
  }
  return true;
}

bool Project::ConvertToV4_Leabra() {
  PDPRoot* root = (PDPRoot*)tabMisc::root;
  LeabraProject* nwproj = (LeabraProject*)root->projects.NewEl(1, &TA_LeabraProject);

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

  ConvertToV4_Enviros(nwproj);

  DataTable* mon_data = (DataTable*)nwproj->data.NewEl(1,&TA_DataTable);
  mon_data->name = "mon_data";

  nwproj->programs.prog_lib.NewProgramFmName("LeabraStdTrain", &(nwproj->programs));

  // todo: copy network params from processes
  // todo: make a standard leabra process for each process group

  FOR_ITR_EL(Network, net, networks., ni) {
    net->Build();
    net->Connect();
  }

  // browse the new project:
  DataBrowser* brows = DataBrowser::New(nwproj, NULL);
  if (!brows) return false;
  brows->ViewWindow();
  return true;
}
