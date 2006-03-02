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


bool Process_Group::Close_Child(TAPtr obj) {
  SchedProcess* sp = GET_MY_OWNER(SchedProcess);
  if (sp != NULL) {
//obs    winbMisc::DelayedMenuUpdate(sp);
  }
  return Remove(obj);		// otherwise just nuke it
}

//////////////////////////
//   Process_MGroup	//
//////////////////////////

bool Process_MGroup::Close_Child(TAPtr obj) {
  if(!obj->InheritsFrom(&TA_SchedProcess)) {
    return taGroup<Process>::Close_Child(obj);
  }
  SchedProcess* sp = (SchedProcess*)obj;
  bool rval = true;
  // do a clean removal of object from hierarchy instead of nixing entire set
  if(sp->super_proc != NULL) {
    sp->super_proc->RemoveSubProc();
  }
  else if(sp->sub_proc != NULL) {
    sp->sub_proc->RemoveSuperProc();
  }
  else
    rval = Remove(obj);		// otherwise just nuke it
  taMisc::DelayedMenuUpdate(this);
  return rval;
}

bool Process_MGroup::DuplicateEl(TAPtr obj) {
return taGroup<Process>::DuplicateEl(obj);/*obs  if(!obj->InheritsFrom(&TA_SchedProcess)) {
    return taGroup<Process>::DuplicateEl(obj);
  }
  SchedProcess* sp = (SchedProcess*)obj;
  bool rval = taGroup<Process>::DuplicateEl(obj); // first get the new guy
  if(!rval) return rval;
  SchedProcess* np = (SchedProcess*)Peek();
  np->DuplicateElHook(sp);
  taMisc::DelayedMenuUpdate(this);
  return rval; */
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
  Init();
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
//  StatVal_List     	//
//////////////////////////

void StatVal_List::NameStatVals(const char* nm, const char* opts, bool is_string) {
  if(size <= 0)    return;
  StatVal* sv = (StatVal*)FastEl(0);
  if(is_string)
    sv->SetStringVecNm(nm, size);
  else
    sv->SetFloatVecNm(nm, size);
  sv->AddDispOption(opts);
  int i;
  for(i=1; i<size; i++) {
    StatVal* sv = (StatVal*)FastEl(i);
    if(is_string)
      sv->SetStringName(nm);
    else
      sv->SetName(nm);
    sv->AddDispOption(opts);
  }
}

bool StatVal_List::HasStopCrit() {
  int i;
  for(i=0; i<size; i++) {
    StatVal* sv = (StatVal*)FastEl(i);
    if(sv->stopcrit.flag)
      return true;
  }
  return false;
}

void StatVal_List::Init() {
  int i;
  for(i=0; i<size; i++)
    ((StatVal*)FastEl(i))->Init();
}

void StatVal_List::InitStat(float value) {
  int i;
  for(i=0; i<size; i++)
    ((StatVal*)FastEl(i))->InitStat(value);
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

void AggStat::UpdateAfterEdit() {
  StatValAgg::UpdateAfterEdit();

  if(owner == NULL)
    return;

  if(from != NULL) {
    if(type_safe && !owner->InheritsFrom(from->GetTypeDef())) {
      taMisc::Error("Incompatible Aggregation of type:",from->GetTypeDef()->name,
	       "into type:",owner->GetTypeDef()->name);
      from = NULL;
    }
  }
  FindRealStat();
  owner->UpdateAfterEdit();
}

void AggStat::SetFrom(Stat* frm) {
  taBase::SetPointer((TAPtr*)&from, frm);
}

void AggStat::FindRealStat() {
  if(owner == NULL) {
    real_stat = NULL;
    return;
  }

  Stat* tmp = (Stat*)owner;
  while ((tmp->time_agg.from != NULL) && (tmp->time_agg.from != tmp))
    tmp = tmp->time_agg.from;

  real_stat = tmp;
}

String AggStat::AppendAggName(const char* nm) const {
  String rval = nm;
  if(from == NULL)
    return rval;
  rval += ":";
  rval += GetAggName();
  return rval;
}

String AggStat::PrependAggName(const char* nm) const {
  if(from == NULL)
    return (String)nm;
  String rval = GetAggName();
  rval += "_";
  rval += nm;
  return rval;
}


//////////////////////////
//  	Stat     	//
//////////////////////////

static String stat_name_agg_sv(Stat* stat, StatVal* fm, StatVal* to) {
  String nm = fm->name;
  int pos;
  if(((pos = nm.index('<')) != -1) && ((pos = nm.index('>',pos+1)) != -1))
    nm = nm.after(pos);
  String fmt;
  if(!nm.empty() && ((nm[0] == '|') || (nm[0] == '$'))) {
    fmt = nm[0]; nm = nm.after(0);
  }
  if(stat->time_agg.from->time_agg.from != NULL) { // already an aggregator, remove time
    String rmv = stat->time_agg.from->time_agg.GetAggName();
    if(nm.before('_') == rmv) nm = nm.after('_');
  }
  nm = stat->time_agg.PrependAggName(nm);
  nm = fmt + nm;
  to->disp_opts = fm->disp_opts;
  to->is_string = fm->is_string;
  return nm;
}


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
  NameStatVals();
  Init();
}

void Stat::CutLinks() {
  DeleteAggregates();
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
  FindOwnAggFrom(cp);
}

void Stat::UpdateAfterEdit() {
  Process::UpdateAfterEdit();
  time_agg.FindRealStat();
  Stat* rstat = time_agg.real_stat;
  if((time_agg.from != NULL) && (rstat != NULL)) { // if aggregator
    net_agg = rstat->net_agg;	// always copy stuff from real stat
    taBase::SetPointer((TAPtr*)&layer, rstat->layer);
    mod.flag = rstat->mod.flag;
  }
  else {
    if(net_agg.op != Aggregate::COPY) {
      copy_vals.Reset();
      n_copy_vals = 0;
    }
  }
  NameStatVals();
  has_stop_crit = HasStopCrit();
  String altnm = AltTypeName();
  if((own_proc != NULL) && (name.contains(GetTypeDef()->name) || name.contains(altnm))) {
    String nw_nm = own_proc->name + "_" + altnm;
    name = time_agg.PrependAggName(net_agg.PrependAggName(nw_nm));
    if(layer != NULL)
      name += String("_") + layer->name;
    else if((time_agg.real_stat != NULL) && (time_agg.real_stat->layer != NULL))
      name += String("_") + time_agg.real_stat->layer->name;
  }
  UpdateAggregates();		// update our aggreagators too!
}

void Stat::DeleteAggregates() {
  if(own_proc == NULL) return;
  SchedProcess* sproc= (SchedProcess *) own_proc->super_proc;
  Stat*		stat_to_agg = this;

  while((sproc != NULL) && (sproc->InheritsFrom(&TA_SchedProcess))) {
    Stat* next_stat_to_agg = NULL;
    int i;
    for(i=sproc->loop_stats.size-1; i>=0; i--) {
      Stat* ast = (Stat*)sproc->loop_stats.FastEl(i);
      if(ast->time_agg.from == stat_to_agg) {
	if(next_stat_to_agg == NULL) // get the first one (most likely to be big agg)
	  next_stat_to_agg = ast;
	else
	  ast->DeleteAggregates(); // be thorough anyway (a little recursion can't hurt)
	sproc->loop_stats.Remove(i);
      }
    }
    if(next_stat_to_agg != NULL)
      stat_to_agg = next_stat_to_agg;
    sproc = (SchedProcess *) sproc->super_proc;
  }
}

bool Stat::HasStopCrit() {
  if((n_copy_vals > 0) && copy_vals.HasStopCrit())
    return true;
  TypeDef* td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++){
    MemberDef* md = td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal)) {
      if(((StatVal*) md->GetOff(this))->stopcrit.flag)
	return true;
    }
    else if(md->type->InheritsFrom(&TA_StatVal_List)) {
      if(((StatVal_List*) md->GetOff(this))->HasStopCrit())
	return true;
    }
  }
  return false;
}

float Stat::InitStatVal() {
  if(time_agg.from != NULL)
    return time_agg.InitAggVal();
  return net_agg.InitAggVal();
}

void Stat::InitStat_impl() {
  net_agg.Init();  
  copy_vals.InitStat(InitStatVal());
}

// implementors of subtypes should replace this general function with one
// that specifically calls InitStat() on StatVal objects directly and:
// InitStat_impl();

void Stat::InitStat() {
  float init_val = InitStatVal();
  TypeDef*   td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++) {
    MemberDef* md=td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal))
      ((StatVal*) md->GetOff(this))->InitStat(init_val);
    else if(md->type->InheritsFrom(&TA_StatVal_List))
      ((StatVal_List*) md->GetOff(this))->InitStat(init_val);
  }
  InitStat_impl();
}

void Stat::Init_impl() {
  time_agg.Init();  
  InitStat();	
  n_copy_vals = 0;
}

// implementors of subtypes should replace this general function with
// specific (Init and Crit)

// Init() specifically calls Init() on StatVal objects directly, and:
// Init_impl();

void Stat::Init() {
  if(loop_init == NO_INIT) return;
  TypeDef*   td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++){
    MemberDef* md = td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal))
      ((StatVal*) md->GetOff(this))->Init();
    else if(md->type->InheritsFrom(&TA_StatVal_List))
      ((StatVal_List*) md->GetOff(this))->Init();
  }
  Init_impl();
}

void Stat::NameStatVals() {
  Stat* rstat = time_agg.real_stat;
  TypeDef* td = GetTypeDef();
  if(time_agg.from != NULL) {	// aggregator, just copy from previous values..
    if(time_agg.op == Aggregate::COPY) {
      // reset any statval_lists because we're not using them!
      int i;
      for(i=TA_Stat.members.size; i<td->members.size;i++){
	MemberDef* md=td->members.FastEl(i);
	if(md->type->InheritsFrom(&TA_StatVal_List)) {
	  StatVal_List* svl = (StatVal_List*)md->GetOff(this);
	  svl->Reset();
	}
      }
      // but names of copy_vals are made as we construct them, so do nothing else here
    }
    else {			// all non-copy time agg: name the stats from previous guys
      if(time_agg.from->copy_vals.size > 0) { // agging from a copy stat
	// reset any statval_lists because we're not using them!
	int i;
	for(i=TA_Stat.members.size; i<td->members.size;i++){
	  MemberDef* md=td->members.FastEl(i);
	  if(md->type->InheritsFrom(&TA_StatVal_List)) {
	    StatVal_List* svl = (StatVal_List*)md->GetOff(this);
	    svl->Reset();
	  }
	}
	copy_vals.EnforceSize(time_agg.from->copy_vals.size); // always have same size as real stat
	n_copy_vals = copy_vals.size;
	int mx = MIN(time_agg.from->copy_vals.size, copy_vals.size);
	for(i=0; i<mx; i++) {
	  StatVal* tosv = (StatVal*)copy_vals.FastEl(i);
	  StatVal* fmsv = (StatVal*)time_agg.from->copy_vals.FastEl(i);
	  String nm = stat_name_agg_sv(this, fmsv, tosv);
	  if((i==0) && (copy_vals.size > 1)) // only for vec > 1
	    tosv->SetFloatVecNm(nm, copy_vals.size);
	  else {
	    tosv->name = nm;
	    tosv->vec_n = 0;
	  }
	}
      }
      else {			// agging from a regular stat
	copy_vals.Reset();
	n_copy_vals = 0;
	int i;
	for(i=TA_Stat.members.size; i<td->members.size;i++){
	  MemberDef* md=td->members.FastEl(i);
	  if(md->type->InheritsFrom(&TA_StatVal)) {
	    StatVal* fm = (StatVal*) md->GetOff((void*)time_agg.from);
	    StatVal* to = (StatVal*) md->GetOff((void*)this);
	    String nm = stat_name_agg_sv(this, fm, to);
	    to->name = nm;
	  }
	  else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	    StatVal_List* fm = (StatVal_List *) md->GetOff((void*)time_agg.from);
	    StatVal_List* to = (StatVal_List *) md->GetOff((void*)this);
	    to->EnforceSize(fm->size);
	    int j;
	    for(j=0;j<to->size;j++) {
	      StatVal* fmsv = (StatVal*)fm->FastEl(j);
	      StatVal* tosv = (StatVal*)to->FastEl(j);
	      String nm = stat_name_agg_sv(this, fmsv, tosv);
	      if((j==0) && (to->size > 1)) // only for vec > 1
		tosv->SetFloatVecNm(nm, to->size);
	      else {
		tosv->name = nm;
		tosv->vec_n = 0;
	      }
	    }
	  }
	}
      }
    }
  }
  else {			// non-aggregator: name the raw stats according to variable
    String misc_nm;
    if((rstat != NULL) && (rstat->layer != NULL)) {
      misc_nm = rstat->layer->name;
      if(misc_nm.length() > 4)
	misc_nm = misc_nm.before(4);
    }
    if(net_agg.op != Aggregate::COPY) {
      copy_vals.Reset();
      n_copy_vals = 0;
    }
    int i;
    for(i=TA_Stat.members.size; i<td->members.size;i++){
      MemberDef* md=td->members.FastEl(i);
      String nm;
      if(!misc_nm.empty())
	nm = misc_nm + "_" + md->name;
      else
	nm = md->name;
      String ag_nm = net_agg.PrependAggName(nm);
      if(md->type->InheritsFrom(&TA_StatVal)) {
	StatVal* sv = (StatVal*)md->GetOff(this);
	sv->SetName(ag_nm);
      }
      else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	StatVal_List* svl = (StatVal_List*)md->GetOff(this);
	svl->NameStatVals(ag_nm, "", false);
      }
      if(net_agg.op == Aggregate::COPY) {
	copy_vals.NameStatVals(ag_nm, "", false);
      }
    }
  }
}

void Stat::UpdateAggregates() {
  Stat* aggr = FindAggregator();
  if(aggr != NULL) {
    aggr->UpdateAfterEdit();
    aggr->UpdateAggregates();
  }
}

Stat* Stat::FindAggregator() {
  if(own_proc == NULL) return NULL;
  SchedProcess* sproc= (SchedProcess *) own_proc->super_proc;
  Stat*		stat_to_agg = this;

  while((sproc != NULL) && (sproc->InheritsFrom(&TA_SchedProcess))) {
    int i;
    for(i=sproc->loop_stats.size-1; i>=0; i--) {
      Stat* ast = (Stat*)sproc->loop_stats.FastEl(i);
      if(ast->time_agg.from == stat_to_agg)
	return ast;
    }
    sproc = (SchedProcess *) sproc->super_proc;
  }
  return NULL;
}

bool Stat::FindOwnAggFrom(const Stat& cp) {
  if((cp.time_agg.from == NULL) || (cp.own_proc == NULL) || (own_proc == NULL)) return true;
  if(time_agg.from != NULL) return false;	// if we're already set, don't do anything
  // find out if from is in sub proc stats somewhere
  if((cp.own_proc->sub_proc == NULL) || (own_proc->sub_proc == NULL))
    return false;// some kind of bizzare agg from outside of hierarch
  int lfidx;
  lfidx = cp.own_proc->sub_proc->final_stats.FindLeaf(cp.time_agg.from);
  if(lfidx >= 0) {
    if(own_proc->sub_proc->final_stats.leaves >= lfidx) return false; // not avail!
    Stat* st = (Stat*)own_proc->sub_proc->final_stats.Leaf(lfidx);
    if(st->GetTypeDef() != GetTypeDef()) return false;
    taBase::SetPointer((TAPtr*)&(time_agg.from),st);
    return true;
  }
  lfidx = cp.own_proc->sub_proc->loop_stats.FindLeaf(cp.time_agg.from);
  if(lfidx >= 0) {
    if(own_proc->sub_proc->loop_stats.leaves >= lfidx) return false; // not avail!
    Stat* st = (Stat*)own_proc->sub_proc->loop_stats.Leaf(lfidx);
    if(st->GetTypeDef() != GetTypeDef()) return false;
    taBase::SetPointer((TAPtr*)&(time_agg.from),st);
    return true;
  }
  return false;
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

bool Stat_Group::nw_itm_def_arg = false;

Stat* Stat_Group::FindMakeStat(TypeDef* td, const char* nm, bool& nw_itm) {
  nw_itm = false;
  if(td == NULL) return NULL;
  Stat* st = (Stat*)FindType(td);
  if(st != NULL) {
    if((nm == NULL) || (st->name == nm)) return st;
  }
  nw_itm = true;
  st = (Stat*)NewEl(1, td);
  if(nm != NULL) st->name = nm;
  taMisc::DelayedMenuUpdate(GetOwner());
  return st;
}

Stat* Stat_Group::FindAggregator(Stat* of_stat, Aggregate::Operator agg_op) {
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, this->, i) {
    if(st->time_agg.from == of_stat) {
      if(agg_op == Aggregate::DEFAULT) return st;
      if(st->time_agg.op == agg_op) return st;
    }
  }
  return NULL;
}

MonitorStat* Stat_Group::FindMonitor(TAPtr of_obj, const char* of_var) {
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, this->, i) {
    if((st->time_agg.from != NULL) || !st->InheritsFrom(&TA_MonitorStat)) continue;
    MonitorStat* mst = (MonitorStat*)st;
    if(mst->objects.size != 1) continue;
    if(mst->objects[0] == of_obj) {
      if(of_var == NULL) return mst;
      if(mst->variable == of_var) return mst;
    }
  }
  return NULL;
}

MonitorStat* Stat_Group::FindMakeMonitor(TAPtr of_obj, const char* of_var, bool& nw_itm) {
  nw_itm = false;
  MonitorStat* mst = FindMonitor(of_obj, of_var);
  if(mst != NULL) return mst;
  nw_itm = true;
  mst = (MonitorStat*)NewEl(1, &TA_MonitorStat);
  mst->SetObject(of_obj);
  mst->SetVariable(of_var);
  taMisc::DelayedMenuUpdate(GetOwner());
  return mst;
}

void MonitorStat::NameStatVals() {
  if(time_agg.from != NULL) {
    Stat::NameStatVals();
  }
  // otherwise just use current values!
}

//////////////////////////
// 	SE_Stat		//
//////////////////////////

void SE_Stat::Initialize() {
  tolerance = 0.0f;
}

void SE_Stat::NameStatVals() {
  Stat::NameStatVals();
  se.AddDispOption("MIN=0");
  se.AddDispOption("TEXT");
}

void SE_Stat::InitStat() {
  float init_val = InitStatVal();
  se.InitStat(init_val);
  InitStat_impl();
}

void SE_Stat::Init() {
  if(loop_init == NO_INIT) return;
  se.Init();
  Init_impl();
}


///////////////////////////
// 	Monitor Stat	 //
///////////////////////////

static StatVal* get_new_mon_val(MonitorStat* st, String nm) {
  StatVal* sv = NULL;
  if(st->mon_vals.size > st->n_copy_vals) {
    sv = (StatVal*)st->mon_vals.FastEl(st->n_copy_vals++);
    if(!sv->name.contains(nm)) {
      sv->val = 0.0f;
      sv->str_val = "";
      sv->stopcrit.Initialize();
      sv->disp_opts = "";
      sv->is_string = false;
      sv->vec_n = 0;
    }
  }
  else {
    st->n_copy_vals++;
    sv = (StatVal*)st->mon_vals.New(1);
  }
  sv->name = nm;
  return sv;
}

String MonitorStat::GetObjName(TAPtr obj) {
  String nm = obj->GetName();
  if(!nm.empty()) {
    if(nm.length() > 4) nm = nm.before(4);
    return nm;
  }
  if(obj->InheritsFrom(TA_Unit)) {
    Unit* u = (Unit*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if(lay != NULL) {
      nm = lay->name;
      if(nm.length() > 4) nm = nm.before(4);
      int index = ((Unit_Group*)u->owner)->Find(u);
      nm += String("[") + String(index) + "]";
      return nm;
    }
  }
  else if(obj->InheritsFrom(TA_Unit_Group)) {
    Unit_Group* ug = (Unit_Group*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if(lay != NULL) {
      nm = lay->name;
      if(nm.length() > 4) nm = nm.before(4);
      nm += ".un";
      if(ug != &(lay->units)) {
	int index = lay->units.gp.Find(ug);
	if(index >= 0)
	  nm += String("[") + String(index) + "]";
      }
      return nm;
    }
  }
  return "no_nm";
}

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

void MonitorStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  if((owner == NULL) || (own_proc == NULL) || (time_agg.real_stat == NULL))    return;
  MonitorStat* rstat = (MonitorStat*)time_agg.real_stat;
  if(rstat->variable.empty())
    return;
  if(time_agg.from != NULL) {
    variable = rstat->variable;
    objects.RemoveAll();
    ptrs.RemoveAll();
    members.RemoveAll();
  }
  else {
    ScanObjects();
  }
  // add variable and object name, if relevant
  String altnm = AltTypeName();
  if(name.contains(GetTypeDef()->name) || name.contains(altnm)) {
    name += String("_") + variable;
    if(rstat->objects.size > 0) {
      String objnm = GetObjName(rstat->objects.FastEl(0));
      if(!objnm.empty())
	name += String("_") + objnm;
    }
  }
}

void MonitorStat::InitStat() {
  mon_vals.InitStat(InitStatVal());
  InitStat_impl();
}

void MonitorStat::Init() {
  if(loop_init == NO_INIT) return;
  mon_vals.Init();
  Init_impl();
}

void MonitorStat::ScanConGroup(Con_Group* mcg, char* varname, Projection* p) {
  Con_Group* cg;
  int i;
  MemberDef* md;
  FOR_ITR_GP(Con_Group, cg, mcg->, i) {
    if((p != NULL) && (cg->prjn != p)) continue;
    md = cg->el_typ->members.FindNameR(varname);
    if(md == NULL) continue;
    String unitname;
    if(net_agg.op==Aggregate::COPY) {
      Unit* u = GET_OWNER(cg,Unit);
      if(u == NULL) continue;
      unitname = GetObjName(u) + String("[") + String(i) + "]";
    }
    int j;
    for(j=0; j<cg->size; j++) {
      Connection* c = (Connection *) cg->Cn(j);
      ptrs.Link(c);
      members.Add(md);
      if(net_agg.op==Aggregate::COPY) {
	String valname = unitname + "[" + String(j) + "]." + String(varname);
	get_new_mon_val(this, valname);
      }
    }
  }
}

void MonitorStat::ScanUnit(Unit* u, Projection* p) {
  TAPtr ths = u;
  void* temp;
  MemberDef* md = NULL;
  String varname = variable;
  if(variable.contains('.')) {
    varname = variable.before('.');
    // for selected projections:
    if(varname=="r") varname = "recv";
    else if(varname=="s") varname = "send";
    md = u->FindMembeR(varname,temp);
    if((md == NULL) || (temp == NULL)) return;
    if(md->type->ptr == 1) {
      ths = *((TAPtr*)temp);
      if(ths == NULL) return;
    }
    else
      ths = (TAPtr) temp;
    varname = variable.after('.');
    if(ths->InheritsFrom(&TA_Con_Group)) {
      ScanConGroup((Con_Group *) ths,varname,p);
      return;
    }
    // otherwise it must be bias or someother sub object
  }
  if(ths == NULL) return;
  md = ths->FindMember((const char*)varname);
  if(md == NULL) return;
  members.Add(md);
  ptrs.Link(ths);
  if(net_agg.op == Aggregate::COPY) {
    String valname = GetObjName(u) + String(".") + variable;
    get_new_mon_val(this, valname);
  }
}

void MonitorStat::ScanProjection(Projection* p) {
  int temp=0;
  MemberDef* md = p->FindMember((char *) variable,temp);
  if(md != NULL) {
    if(net_agg.op == Aggregate::COPY) {
      String valname = GetObjName(p) + String(".") + variable;
      get_new_mon_val(this, valname);
    }
    ptrs.Link(p);
    members.Add(md);
    return;
  }
  Layer* lay = NULL;
  if(variable.before('.') == "r") lay = p->layer;
  else if(variable.before('.') == "s") lay = p->from;
  if(lay == NULL) {
    taMisc::Error("MonitorStat Projection does not have layer's set or",
		   "selected variable does not apply to connections");
    return;
  }
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, lay->units., i)
    ScanUnit(u, p);
}

void MonitorStat::ScanLayer(Layer* lay) {
  TAPtr ths = lay;
  MemberDef* md = NULL;
  String varname = variable;
  if(variable.contains('.')) {
    varname = variable.before('.');
    md = lay->FindMember((const char*)varname);
    if(md != NULL) {
      if(md->type->ptr == 1)
	ths = *((TAPtr*)md->GetOff((void*)lay));
      else
	ths = (TAPtr) md->GetOff((void*)lay);
      varname = variable.after('.');
    }
    else			// variable not there, go to units
      ths = NULL;
  }
  if(ths != NULL) {
    // now search from 'ths'
    md = ths->FindMember((const char*)varname);
    if(md != NULL) {
      if(net_agg.op == Aggregate::COPY) {
	String valname = GetObjName(lay) + String(".") + variable;
	get_new_mon_val(this, valname);
      }
      ptrs.Link(ths);
      members.Add(md);
      return;
    }
  }
  // the default is to scan the units for the variable
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, lay->units., i)
    ScanUnit(u);
  if(mon_vals.size > 1) {
    StatVal* sv = (StatVal *) mon_vals.FastEl(0);
    sv->disp_opts = "";
    String xgeom = String("GEOM_X=") + String(lay->geom.x);
    String ygeom = String("GEOM_Y=") + String(lay->geom.y);
    sv->AddDispOption(xgeom);
    sv->AddDispOption(ygeom);
  }
}

void MonitorStat::ScanUnitGroup(Unit_Group* ug) {
  TAPtr ths = ug;
  MemberDef* md = NULL;
  String varname = variable;
  if(variable.contains('.')) {
    varname = variable.before('.');
    md = ug->FindMember((const char*)varname);
    if(md != NULL) {
      if(md->type->ptr == 1)
	ths = *((TAPtr*)md->GetOff((void*)ug));
      else
	ths = (TAPtr) md->GetOff((void*)ug);
      varname = variable.after('.');
    }
    else			// variable not there, go to units
      ths = NULL;
  }
  if(ths != NULL) {
    // now search from 'ths'
    md = ths->FindMember((const char*)varname);
    if(md != NULL) {
      if(net_agg.op == Aggregate::COPY) {
	String valname = GetObjName(ug) + String(".") + variable;
	get_new_mon_val(this, valname);
      }
      ptrs.Link(ths);
      members.Add(md);
      return;
    }
  }
  // the default is to scan the units for the variable
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, ug->, i)
    ScanUnit(u);
  if(mon_vals.size > 1) {
    StatVal* sv = (StatVal *) mon_vals.FastEl(0);
    sv->disp_opts = "";
    String xgeom = String("GEOM_X=") + String(ug->geom.x);
    String ygeom = String("GEOM_Y=") + String(ug->geom.y);
    sv->AddDispOption(xgeom);
    sv->AddDispOption(ygeom);
  }
}

void MonitorStat::ScanObjects() {
  if((time_agg.real_stat == NULL) || (time_agg.from != NULL))
    return;
  MonitorStat* rstat = (MonitorStat*)time_agg.real_stat;
  n_copy_vals = 0;		// use this as a counter for mon_vals!
  tabMisc::NotifyEdits(&mon_vals);
  ptrs.RemoveAll();
  members.RemoveAll();
  if(rstat->objects.size == 0) return;
  int i;
  for(i=0;i<rstat->objects.size;i++){
    TAPtr obj = rstat->objects.FastEl(i);
    if(obj == NULL)
      continue;
    if(obj->InheritsFrom(&TA_Unit)) ScanUnit((Unit*) obj);
    else if(obj->InheritsFrom(&TA_Layer)) ScanLayer((Layer*) obj);
    else if(obj->InheritsFrom(&TA_Unit_Group)) ScanUnitGroup((Unit_Group*) obj);
    else if(obj->InheritsFrom(&TA_Projection))
      ScanProjection((Projection*) obj);
    else {			// could be any type of object
      int temp = 0;
      MemberDef* md = obj->FindMember((char *) variable,temp);
      if(md != NULL) {
	if(net_agg.op == Aggregate::COPY) {
	  String valname = GetObjName(obj) + String(".") + variable;
	  get_new_mon_val(this, valname);
	}
	ptrs.Link(obj);
	members.Add(md);
      }
    }
  }
  if(net_agg.op != Aggregate::COPY) {
    mon_vals.EnforceSize(1);
    String nm = variable;
    String objnm = GetObjName(rstat->objects.FastEl(0));
    if(!objnm.empty())
      nm += String("_") + objnm;
    nm = net_agg.PrependAggName(nm);
    get_new_mon_val(this, nm);
  }
  else {
    mon_vals.EnforceSize(n_copy_vals);
    if(mon_vals.size > 0) {
      StatVal* sv = (StatVal*)mon_vals.FastEl(0);
      String nm = sv->name;
      sv->SetFloatVecNm(nm, mon_vals.size);
    }
  }
  n_copy_vals = 0;
}

void MonitorStat::SetVariable(const char* varnm){
  variable = varnm;
  UpdateAfterEdit();
}

void MonitorStat::SetObject(TAPtr obj){
  objects.Reset();
  objects.Link(obj);
  UpdateAfterEdit();
}

void MonitorStat::AddObject(TAPtr obj){
  objects.Link(obj);
  UpdateAfterEdit();
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

void SchedProcess::SetDefaultName() {
  // set name to be shorter version of original (no Process or type names)
  if(taMisc::not_constr || taMisc::in_init)
    return;
  TypeDef* td = GetTypeDef();
  int tok = td->tokens.Find((void *)this);
  String nw_nm = td->name;
  if(nw_nm.contains("Process"))
    nw_nm.gsub("Process","");
  else if(nw_nm.contains("Proc"))
    nw_nm.gsub("Proc", "");
  else {
    int pos = nw_nm.length()-1;
    int c = nw_nm[pos];
    while(!isupper(c) && (pos > 0)) c = nw_nm[--pos];
    if(pos > 0)
      nw_nm = nw_nm.from(pos);
  }
  name = nw_nm + "_" + String(tok);
}

void SchedProcess::CutLinks() {
  RemoveFromLogs();		// logs have a pointer to us in them too
//nn  RemoveFromDisplays();		// displays have a pointer to us in them too
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
    Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
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

void SchedProcess::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();

  taLeafItr i;
  Process* pr;
  FOR_ITR_EL(Process, pr, final_stats., i) {
    pr->CopyPNEPtrs(network, environment);
    pr->UpdateAfterEdit();
  }
  FOR_ITR_EL(Process, pr, loop_stats., i) {
    pr->CopyPNEPtrs(network, environment);
    pr->UpdateAfterEdit();
  }
  FOR_ITR_EL(Process, pr, init_procs., i) {
    if(pr->GetOwner(GetTypeDef()) == this) { // only if we own the process
      pr->CopyPNEPtrs(network, environment);
      pr->UpdateAfterEdit();
  }
  }
  FOR_ITR_EL(Process, pr, loop_procs., i) {
    if(pr->GetOwner(GetTypeDef()) == this) { // only if we own the process
      pr->CopyPNEPtrs(network, environment);
      pr->UpdateAfterEdit();
    }
  }
  FOR_ITR_EL(Process, pr, final_procs., i) {
    if(pr->GetOwner(GetTypeDef()) == this) { // only if we own the process
      pr->CopyPNEPtrs(network, environment);
      pr->UpdateAfterEdit();
    }
  }

  if(!cntr_items.el_typ->InheritsFrom(TA_DataItem))
    cntr_items.el_typ = &TA_DataItem;
  GetCntrDataItems();

  CreateSubProcs(false);
  UpdateLogUpdaters();
}

void SchedProcess::Init_impl() {
  SetDefaultPNEPtrs();
/*nn  if(cntr != NULL)
    *cntr = 0;
  
  int c = 0;
  if(super_proc != NULL)
    c = super_proc->GetCounter() + 1; // add 1 to simulate increment of parent log..
*/  
  Stat* st;
  taLeafItr i;
/*nn  FOR_ITR_EL(Stat, st, final_stats., i) {
    if(st->mod.flag && (((c - st->mod.off) % (st->mod.m)) == 0))
      st->Init();
  }*/
  FOR_ITR_EL(Stat, st, loop_stats., i)
    st->Init();
  Process* proc;
  FOR_ITR_EL(Process, proc, init_procs., i) {
    if(!proc->InheritsFrom(TA_SchedProcess))
      proc->Init();
  }
  FOR_ITR_EL(Process, proc, loop_procs., i) {
    if(!proc->InheritsFrom(TA_SchedProcess))
      proc->Init();
  }
  FOR_ITR_EL(Process, proc, final_procs., i) {
    if(!proc->InheritsFrom(TA_SchedProcess))
      proc->Init();
  }
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

void SchedProcess::GetCntrDataItems() {
  if(cntr == NULL) return;
  if(!cntr_items.el_typ->InheritsFrom(TA_DataItem))
    cntr_items.el_typ = &TA_DataItem;
  if(cntr_items.size < 1)
    cntr_items.EnforceSize(1);
  DataItem* it = (DataItem*)cntr_items.FastEl(0);
  it->SetNarrowName(cntr->name);
}

void SchedProcess::RemoveFromLogs() {
  PDPLog* lg;
  taLeafItr i;
  FOR_ITR_EL(PDPLog, lg, logs., i) {
    if(lg->log_proc.Remove(this)) {
//      lg->SyncLogViewUpdaters();
    }
  }
}

void SchedProcess::UpdateLogUpdaters() {
  // make sure the logs have us in them too...
/*maybe obs  PDPLog* plog;
  taLeafItr i;
  FOR_ITR_EL(PDPLog, plog, logs., i) {
    if(plog->log_proc.LinkUnique(this))
      plog->SyncLogViewUpdaters();
    if(plog->cur_proc != this) {
      if(taMisc::is_loading || taMisc::is_duplicating)
	plog->cur_proc = this;
      else {
	GenLogData();
//TODO:OBS	plog->NewHead(log_data, this);
      }
    }
  } */
}

void SchedProcess::CreateSubProcs(bool update) {
  if(sub_proc_type == NULL)
    return;

  if((sub_proc == NULL) || (sub_proc->GetTypeDef() != sub_proc_type)) {
    Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
    if(gp == NULL)
      return;

    if(sub_proc != NULL) {
      gp->RemoveLeaf(sub_proc);
      taBase::DelPointer((TAPtr*)&sub_proc);
    }

    sub_proc = (SchedProcess*)taBase::MakeToken(sub_proc_type);
    taBase::Ref(sub_proc);	// give it an extra reference for us
    sub_proc->super_proc = this;
    sub_proc->CopyPNEPtrs(network, environment);
    gp->Add(sub_proc); // InitLinks called with super_proc in place
    taBase::SetPointer((TAPtr*)&(step.proc), sub_proc);	// default is to step lower guy.
    if(!taMisc::is_loading && (network != NULL) &&
       (sub_proc->InheritsFrom(TA_TrialProcess) ||
	sub_proc->InheritsFrom(TA_SettleProcess)))
    {
      // auto add updater to network
/*TODO      if(network->views().count() > 0)
	((NetView*)network->views.FastEl(0))->AddUpdater(sub_proc);*/
    }
  }

  if(sub_proc != NULL) {
    sub_proc->super_proc = this;
    sub_proc->CopyPNEPtrs(network, environment);
    sub_proc->UpdateAfterEdit();
  }

  if(update)
    UpdateAfterEdit();
}

void SchedProcess::RemoveSuperProc() {
  Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
  if((gp == NULL) || (super_proc == NULL))
    return;

  SchedProcess* prv_super = super_proc;	// cache orig
  SchedProcess* super_super = prv_super->super_proc;
  UnLinkFmSuperProc();		// disconnect me from that one!
  if(super_super != NULL) {
    super_super->LinkSubProc(this); // i am now its sub proc
    super_super->SetAggsAfterRmvSubProc(); // the super-super proc now has a new sub-proc
  }
  gp->RemoveEl(prv_super);	// get rid of it!
  taMisc::DelayedMenuUpdate(gp);
}

void SchedProcess::RemoveSubProc() {
  Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
  if((gp == NULL) || (sub_proc == NULL))
    return;

  SchedProcess* prv_sub = sub_proc;	// cache orig
  SchedProcess* sub_sub = prv_sub->sub_proc;
  prv_sub->UnLinkSubProc();	// disconnect sub-sub from sub process, otherwise it will take it all down with it
  UnLinkSubProc();		// disconnect me from that one!
  if(sub_sub != NULL) {
    LinkSubProc(sub_sub); 	// i am now its super proc
    SetAggsAfterRmvSubProc(); 	// i need to point to new sub_sub proc aggs
  }
  gp->RemoveEl(prv_sub);	// get rid of it!
  taMisc::DelayedMenuUpdate(gp);
}

void SchedProcess::UnLinkSubProc() {
  if(sub_proc != NULL)
    sub_proc->super_proc = NULL;
  sub_proc_type = NULL;
  taBase::SetPointer((TAPtr*)&(sub_proc), NULL);
}

void SchedProcess::UnLinkFmSuperProc() {
  if(super_proc != NULL)
    super_proc->UnLinkSubProc();
  super_proc = NULL;
}

void SchedProcess::LinkSubProc(SchedProcess* proc) {
  if(proc == NULL) {
    UnLinkSubProc();
    return;
  }
  // warning -- does nothing to preserve sub_proc's current super-proc!
  sub_proc_type = proc->GetTypeDef();
  taBase::SetPointer((TAPtr*)&(sub_proc), proc);
  sub_proc->super_proc = this;
  sub_proc->CopyPNEPtrs(network, environment);
}

void SchedProcess::LinkSuperProc(SchedProcess* proc) {
  if(proc == NULL) {
    UnLinkFmSuperProc();
    return;
  }
  // warning: does nothing to preserve super-proc's current sub-proc!
  super_proc = proc;
  super_proc->CopyPNEPtrs(network, environment);
  super_proc->LinkSubProc(this);
}

void SchedProcess::GetAggsFmSuperProc() {
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, super_proc->loop_stats., i) {
    if(st->time_agg.from == NULL) continue; // ignore non-aggs
    Stat* nag = (Stat*)st->Clone(); // clone original one
    loop_stats.Add(nag);  	// i get it
    taBase::SetPointer((TAPtr*)&(nag->time_agg.from), st->time_agg.from); // get agg.from from this one
    taBase::SetPointer((TAPtr*)&(st->time_agg.from), nag); // super now aggs from me
    nag->UpdateAfterEdit();	// update everything on the new aggregator..
  }
}

void SchedProcess::SetAggsAfterRmvSubProc() {
  int lf;
  for(lf = loop_stats.leaves-1; lf>=0; lf--) {
    Stat* st = (Stat*)loop_stats.Leaf(lf);
    if(st->time_agg.from == NULL) continue; // ignore non-aggs
    Stat* prv_frm = st->time_agg.from;	// this is stat on old sub proc
    if(prv_frm->time_agg.from == NULL)	{ // this is not itself an agg
      loop_stats.RemoveLeaf(lf); // what i'm aggregating is dissappearing, so die!
      continue;
    }
    Stat* nw_frm = prv_frm->time_agg.from; // this is who i now want to agg from
    taBase::SetPointer((TAPtr*)&(st->time_agg.from), nw_frm);
    nw_frm->UpdateAfterEdit();	// update everything on the new aggregator..
  }
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

void TrialProcess::UpdateAfterEdit() {
  SchedProcess::UpdateAfterEdit();
  epoch_proc = (EpochProcess*)FindSuperProc(&TA_EpochProcess);
  if((epoch_proc != NULL) && epoch_proc->InheritsFrom(TA_SequenceEpoch))
    taBase::SetPointer((TAPtr*)&enviro_group,
			((SequenceEpoch*)epoch_proc)->cur_event_gp);
  else if(environment != NULL)
    taBase::SetPointer((TAPtr*)&enviro_group, &(environment->events));
}

void TrialProcess::Init_impl() {
  SchedProcess::Init_impl();
  if(epoch_proc != NULL)
    taBase::SetPointer((TAPtr*)&cur_event, epoch_proc->cur_event);
  if((epoch_proc != NULL) && epoch_proc->InheritsFrom(TA_SequenceEpoch))
    taBase::SetPointer((TAPtr*)&enviro_group,
			((SequenceEpoch*)epoch_proc)->cur_event_gp);
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

void EpochProcess::UpdateAfterEdit() {
  SchedProcess::UpdateAfterEdit();
  if(environment != NULL)
    taBase::SetPointer((TAPtr*)&enviro_group, &(environment->events));
  if(batch_n < 1) batch_n = 1;
  batch_n_eff = batch_n;
  if(dmem_nprocs < 1) dmem_nprocs = 1;
//  GetCurEvent();
}

void EpochProcess::Init_impl() {
  SchedProcess::Init_impl();
  if((environment == NULL) || (network == NULL))
    return;

//nn  environment->InitEvents();
//nn  environment->UnSetLayers();

  trial.val = 0;

//  GetEventList();
// GetCurEvent();
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

void SequenceProcess::UpdateAfterEdit() {
  SchedProcess::UpdateAfterEdit();
  sequence_epoch = (SequenceEpoch*)FindSuperProc(&TA_SequenceEpoch);
  if(environment != NULL)
    taBase::SetPointer((TAPtr*)&enviro_group, &(environment->events));
  if(sequence_epoch != NULL)
    taBase::SetPointer((TAPtr*)&cur_event_gp, sequence_epoch->cur_event_gp);
}

void SequenceProcess::Init_impl() {
  SchedProcess::Init_impl();
  if((environment == NULL) || (network == NULL) || (sequence_epoch == NULL))
    return;
  taBase::SetPointer((TAPtr*)&cur_event_gp, sequence_epoch->cur_event_gp);
  if(cur_event_gp == NULL)
    return;

  GetEventList();
  tick.val = 0;
}

void SequenceProcess::GetEventList() {
  event_list.Reset();
//NN??  tick.max = cur_event_gp->EventCount();
  int i;
  for(i=0; i<tick.max; i++)
    event_list.Add(i);
  if(order == PERMUTED)
    event_list.Permute();
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

void SequenceEpoch::UpdateAfterEdit() {
  EpochProcess::UpdateAfterEdit();
}

void SequenceEpoch::Init_impl() {
  SchedProcess::Init_impl();
  if((environment == NULL) || (network == NULL))
    return;

//  environment->InitEvents();
//  environment->UnSetLayers();

  trial.val = 0;

  GetEventList();
}

void SequenceEpoch::GetEventList() {
  event_list.Reset();
//NN??  trial.max = environment->GroupCount();
  if(trial.max == 0) trial.max = 1; // just present all the events themselves!
  int i;
  for(i=0; i<trial.max; i++)
    event_list.Add(i);
  if(order == PERMUTED)
    event_list.Permute();
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

void NEpochProcess::UpdateAfterEdit() {
  SchedProcess::UpdateAfterEdit();
  epoch_proc = (EpochProcess*)FindSubProc(&TA_EpochProcess);
  if(network != NULL)
    epoch = network->epoch;
}

void NEpochProcess::Init_impl() {
  inherited::Init_impl();
  if(network != NULL)
    epoch = network->epoch;
}

void NEpochProcess::GetCntrDataItems() {
  if(cntr_items.size < 2)
    cntr_items.EnforceSize(2);
  SchedProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetNarrowName("Epoch");
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

void TrainProcess::UpdateAfterEdit() {
  SchedProcess::UpdateAfterEdit();
  epoch_proc = (EpochProcess*)FindSubProc(&TA_EpochProcess);
  if(network != NULL)
    epoch.val = network->epoch;
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

void PatternSpec::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if(n_vals == 0)
    n_vals = geom.x * geom.y;
  geom.FitNinXY(n_vals);

  if((name.empty() || name.contains(GetTypeDef()->name)) && !layer_name.empty())
    name = layer_name;

  value_names.EnforceSize(n_vals);
  global_flags.EnforceSize(n_vals);

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


BaseSpec_MGroup* EventSpec_SPtr::GetSpecGroup() {
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

void Environment::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

int Environment::GroupCount() {
  if(events.gp.size == 0)
    return 0;
  if(events.leaf_gp == NULL) events.InitLeafGp();
  return events.leaf_gp->size;
}

Event_MGroup* Environment::GetGroup(int i) {
  if(events.gp.size == 0)
    return NULL;
  if(events.leaf_gp == NULL) events.InitLeafGp();
  return (Event_MGroup*)events.leaf_gp->SafeEl(i);
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
  UpdateReCompile();
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
  Event_MGroup::InitLinks();
  taBase::Own(list, this);
  fenv = GET_MY_OWNER(FreqEnv);
}

void FreqEvent_Group::CutLinks() {
  fenv = NULL;
  Event_MGroup::CutLinks();
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

Event_MGroup* FreqEnv::GetGroup(int i) {
  if((freq_level == GROUP) || (freq_level == GROUP_EVENT)) {
    if(events.leaf_gp == NULL) events.InitLeafGp();
    return (Event_MGroup*)events.leaf_gp->SafeEl(list.SafeEl(i));
  }
  return Environment::GetGroup(i);
}


//////////////////////////
//  TimeEvent_MGroup 	//
//////////////////////////

void TimeEvent_MGroup::Initialize() {
  end_time = 1.0f;
  interpolate = USE_ENVIRO;
  SetBaseType(&TA_TimeEvent);
}

void TimeEvent_MGroup::Copy_(const TimeEvent_MGroup& cp) {
  interpolate = cp.interpolate;
  end_time = cp.end_time;
}

void TimeEvent_MGroup::UpdateAfterEdit() {
  Event_MGroup::UpdateAfterEdit();
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
  events.gp.SetBaseType(&TA_TimeEvent_MGroup);
  interpolate = CONSTANT;
}

void TimeEnvironment::InitLinks() {
  Environment::InitLinks();
  events.gp.SetBaseType(&TA_TimeEvent_MGroup);
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

Event_MGroup* FreqTimeEnv::GetGroup(int i) {
  if(freq_level == GROUP) {
    if(events.leaf_gp == NULL) events.InitLeafGp();
    return (Event_MGroup*)events.leaf_gp->SafeEl(list.SafeEl(i));
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
  environments.SetBaseType(&TA_Environment);
  processes.SetBaseType(&TA_SchedProcess);
}

void Project::InitLinks() {
  inherited::InitLinks();
  taBase::Own(environments, this);
  taBase::Own(processes, this);
}

void Project::CutLinks() {
  deleting = true;
  processes.CutLinks();
  environments.CutLinks();
  inherited::CutLinks();
}

void Project::Copy_(const Project& cp) {
  environments = cp.environments;
  processes = cp.processes;
}

void Project::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  //TODO: here is maybe where we can trap having loaded legacy, and convert
}
