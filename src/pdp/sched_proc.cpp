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



// sched_proc.cc


#include "sched_proc.h"
#include "procs_extra.h"
#include "pdpshell.h"

#ifdef TA_GUI
#include "css_qt.h"
#include "ta_qt.h"
#include "ta_qtdialog.h"
#endif

#include <ctype.h>

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

TAPtr 	StepParams::GetOwner() const	{ return (taBase*)owner; }
TAPtr	StepParams::GetOwner(TypeDef* tp) const { return taBase::GetOwner(tp); }
TAPtr 	StepParams::SetOwner(TAPtr ta)	{ owner = (SchedProcess*)ta; return ta; }

void StepParams::Copy_(const StepParams& cp) {
  n = cp.n;
}


//////////////////////////
// 	SchedProcess	//
//////////////////////////

bool SchedProcess::stop_flag = false;
bool SchedProcess::stepping = false;
bool SchedProcess::bailing = false;
bool SchedProcess::running = false;

void SchedProcess::Initialize() {
  re_init=true;
  sub_proc_type = NULL;
  cntr = NULL;
  super_proc = NULL;
  sub_proc = NULL;
  im_step_proc = false;
  im_run_proc = false;
  log_loop = false;
  log_counter = false;
  loop_stats.SetBaseType(&TA_Stat);
  final_stats.SetBaseType(&TA_Stat);
  init_procs.SetBaseType(&TA_Process);
  loop_procs.SetBaseType(&TA_Process);
  final_procs.SetBaseType(&TA_Process);
//TODO  displays.SetBaseType(&TA_PDPView);
  logs.SetBaseType(&TA_PDPLog);
  cntr_items.SetBaseType(&TA_DataItem);
}

void SchedProcess::InitLinks() {
  Process::InitLinks();
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

void SchedProcess::InitAllLogs() {
  if(project == NULL)
    return;
  taLeafItr i;
  PDPLog* lg;
  FOR_ITR_EL(PDPLog, lg, project->logs., i)
    lg->Clear();
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

void SchedProcess::Destroy() {
  CutLinks();
}

void SchedProcess::CutLinks() {
  RemoveFromLogs();		// logs have a pointer to us in them too
  RemoveFromDisplays();		// displays have a pointer to us in them too
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
  Process::CutLinks();
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

//TODO  displays.BorrowUnique(cp.displays);	// don't replicate because we don't own them...
  UpdateDisplayUpdaters();	// let the new displays know about us.

  // don't do any of the following here, only for duplicateEl:
  //  logs.BorrowUnique(cp.logs);  // don't have two procs to go the same logs
  //  UpdateLogUpdaters();

  //  sub_proc_type = cp.sub_proc_type;
  //  super_proc = cp.super_proc;
  //  taBase::SetPointer((TAPtr*)&sub_proc, cp.sub_proc);
}

void SchedProcess::UpdateAfterEdit() {
  Process::UpdateAfterEdit();

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
  UpdateDisplayUpdaters();
}

////////////////////////////////////////////////////////
//	C_Code: the master function!
////////////////////////////////////////////////////////

void SchedProcess::C_Code() {
  bool stop_crit = false;	// a stopping criterion was reached
  bool stop_force = false;	// either the Stop button was pressed or our Step level was reached

  if(re_init) {			// if its time to re-initialize, then do it
    Init();			// this sets the counters to zero, etc.
    InitProcs();		// this runs any initialization processes
  }

  do {
    Loop();			// user defined code goes here
    if(!bailing) {
      UpdateCounters();		// update the counters (before logging)
      LoopProcs();		// check/run loop procs (use mod based on counter)
      LoopStats();		// udpate in-loop stats
      if(log_loop)
	UpdateLogs();		// generate log output and update logs
      UpdateState();		// update process state vars

      stop_crit = Crit();   	// check if stopping criterion was reached
      if(!stop_crit) {		// if at critera, going to quit anyway, so don't
	stop_force = StopCheck(); // check for stopping (either by Stop button or Step)
      }
    }
  }
  while(!bailing && !stop_crit && !stop_force);
  // loop until we reach criterion (e.g. ctr > max) or are forcibly stopped

  if(stop_crit) {		// we stopped because we reached criterion
    Final();			// user defined code at end of loop
    FinalProcs();		// call the final procs
    FinalStats();		// final statistics after looping
    if(!log_loop)
      UpdateLogs();		// if not logging in loop, logging at end
    UpdateDisplays();		// update displays after the loop
    SetReInit(true);		// made it through the loop, so Init next time
    FinalStepCheck();		// always stop at end if i'm the step process
  }
  else {			// we we're forcibly stopped for some reason
    bailing = true;		// now we're bailing out of all further processing
  }
}

////////////////////////////////////////////////////////
//	Main user interface code for running
////////////////////////////////////////////////////////

void SchedProcess::NewInit() {
  if(running) {
    taMisc::Choice("A process is running; Stop first, then you can NewInit.", "Ok");
    return;		// already running
  }
  if(!CheckAllTypes())		// do this before any actions in Init_force
    return;
  NewSeed();
  Init_force();
  OldSeed();		// then restore seed again to start over from same place
}

void SchedProcess::ReInit() {
  if(running) {
    taMisc::Choice("A process is running; Stop first, then you can ReInit.", "Ok");
    return;		// already running
  }
  if(!CheckAllTypes())		// do this before any actions in Init_force
    return;
  OldSeed();
  Init_force();
  OldSeed();		// then restore seed again to start over from same place
}

void SchedProcess::Run_gui() {
  if(running) {
    taMisc::Choice("A process is already running.", "Ok");
    return;		// already running
  }
  stop_flag = false;
  bailing = false;
  running = true;
  im_run_proc = true;
  NotifyNetObjects();
  NotifyProcDialogs();
  Run();
  im_run_proc = false;
  stop_flag = false;
  stepping = false;
  bailing = false;
  running = false;
  NotifyProcDialogs();
}

void SchedProcess::Step() {
  if(running) {
    taMisc::Choice("A process is already running.", "Ok");
    return;		// already running
  }
/*TODO  if((network != NULL) && (network->views().count() > 0) && (step.proc != NULL)) {
    ((NetView*)network->views.DefaultEl())->AddUpdater(step.proc);
  } */
  SchedProcess* sp = this;
  // based on the sub_proc, not the proc itself.
  // set step flags of all containing procs
  while((sp != NULL) && (this != step.proc) && (sp->sub_proc != step.proc)) {
    sp->im_step_proc = false;
    sp = sp->sub_proc;
  }
  if(sp == NULL) return;
  sp->im_step_proc = true;	// flag the step process
  sp->step.n = step.n;		// copy step n down here, where it is actually used

  stepping = true;		// actually doing a step
  Run_gui();
}

void SchedProcess::Stop() {
  if(!running) {
    taMisc::Choice("No running process to Stop.", "Ok");
    return;		// already running
  }
  stop_flag = true;
}

void SchedProcess::GoTo(int goto_val) {
  Init_force();			// first initialize (gets all sub-guys inited too)
  if(cntr != NULL)
    cntr->val = goto_val;
  SetReInit(false);		// don't init again!
  UpdateState();		// then update state for new val
}

void SchedProcess::StepUp() {
  if((step.proc == NULL) || (step.proc->super_proc == NULL)) return;
/*TODO  if((network != NULL) && (network->views().count() > 0)) {
    ((NetView*)network->views.DefaultEl())->RemoveUpdater(step.proc);
    ((NetView*)network->views.DefaultEl())->AddUpdater(step.proc->super_proc);
  }*/
  taBase::SetPointer((TAPtr*)&(step.proc), step.proc->super_proc);
}

void SchedProcess::StepDn() {
  if((step.proc == NULL) || (step.proc->sub_proc == NULL)) return;
  taBase::SetPointer((TAPtr*)&(step.proc), step.proc->sub_proc);
/*TODO  if((network != NULL) && (network->views().count() > 0)) {
    ((NetView*)network->views.DefaultEl())->AddUpdater(step.proc);
  } */
}

void SchedProcess::NotifyNetObjects() {
#ifdef TA_GUI
  if(!taiMisc::gui_active)    return;
  int i;
  for(i=taiMisc::active_edits.size-1; i>=0; i--) {
    taiEditDataHost* dlg = taiMisc::active_edits.FastEl(i);
    if((dlg->state != taiDataHost::ACTIVE) || (dlg->cur_base == NULL) ||
       (dlg->typ == NULL))
      continue;
    if(dlg->typ->InheritsFrom(TA_Network) || dlg->typ->InheritsFrom(TA_Layer) ||
       dlg->typ->InheritsFrom(TA_Projection) || dlg->typ->InheritsFrom(TA_Unit) ||
       dlg->typ->InheritsFrom(TA_Connection) || dlg->typ->InheritsFrom(TA_Con_Group))
//obs      dlg->SetRevert();
      dlg->Changed();
  }
#endif
}

void SchedProcess::NotifyProcDialogs() {
#ifdef TA_GUI
  if(!taiMisc::gui_active)    return;
  int i;
  for(i=taiMisc::active_edits.size-1; i>=0; i--) {
    taiEditDataHost* dlg = taiMisc::active_edits.FastEl(i);
    if((dlg->state != taiDataHost::ACTIVE) || (dlg->cur_base == NULL) ||
       (dlg->typ == NULL)) {
      continue;
    }
    if(dlg->typ->InheritsFrom(TA_SchedProcess)) {
      dlg->Revert_force();
    }
  }
  taiMisc::RunPending();
#endif
}

////////////////////////////////////////////////////////
//	Initialization
////////////////////////////////////////////////////////

void SchedProcess::Init() {
  rndm_seed.GetCurrent();
  Init_impl();
  SetReInit(false);
  if(sub_proc != NULL)		// set sub-procs to be re-inited when run
    sub_proc->Init_flag();
  taLeafItr i;
  SchedProcess* proc;
  FOR_ITR_EL(SchedProcess, proc, init_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_flag();
  }
  FOR_ITR_EL(SchedProcess, proc, loop_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_flag();
  }
  FOR_ITR_EL(SchedProcess, proc, final_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_flag();
  }
}

void SchedProcess::InitProcs() {
  int c = 0;
  if(super_proc != NULL)
    c = super_proc->GetCounter();
  taLeafItr i;
  Process* p;
  FOR_ITR_EL(Process, p, init_procs., i) {
    if(p->mod.flag && (((c - p->mod.off) % (p->mod.m)) == 0))
      p->Run();
  }
}

void SchedProcess::Init_impl() {
  SetDefaultPNEPtrs();
  if(cntr != NULL)
    *cntr = 0;
#ifdef TA_GUI
  if(taMisc::gui_active)
    ctrl_panel.Revert();
#endif
  int c = 0;
  if(super_proc != NULL)
    c = super_proc->GetCounter() + 1; // add 1 to simulate increment of parent log..
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, final_stats., i) {
    if(st->mod.flag && (((c - st->mod.off) % (st->mod.m)) == 0))
      st->Init();
  }
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

void SchedProcess::Init_flag() {
  SetReInit(true);
  if(sub_proc != NULL)		// set sub-procs to be re-inited when run
    sub_proc->Init_flag();
  taLeafItr i;
  SchedProcess* proc;
  FOR_ITR_EL(SchedProcess, proc, init_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_flag();
  }
  FOR_ITR_EL(SchedProcess, proc, loop_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_flag();
  }
  FOR_ITR_EL(SchedProcess, proc, final_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_flag();
  }
}

void SchedProcess::Init_force() {
  SetReInit(true);		// make sure init flag is set at this point
  Init_impl();
  if(sub_proc != NULL)		// force re-init of sub-procs
    sub_proc->Init_force();
  taLeafItr i;
  SchedProcess* proc;
  FOR_ITR_EL(SchedProcess, proc, init_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_force();
  }
  FOR_ITR_EL(SchedProcess, proc, loop_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_force();
  }
  FOR_ITR_EL(SchedProcess, proc, final_procs., i) {
    if(proc->InheritsFrom(TA_SchedProcess))
      proc->Init_force();
  }
  Init_step_flags();
}

void SchedProcess::Init_step_flags() {
  SchedProcess* sp = this;
  while(sp != NULL) {		// reset the step flags
    sp->im_step_proc = false;
    sp->im_run_proc = false;
    sp = sp->sub_proc;
  }
  stepping = false;
}


////////////////////////////////////////////////////////
//	Inside the Loop
////////////////////////////////////////////////////////

void SchedProcess::Loop() {
  if(sub_proc != NULL) sub_proc->Run();
}

void SchedProcess::UpdateCounters() {
  if(cntr) cntr->Inc();
}

void SchedProcess::LoopProcs() {
  int c = GetCounter();
  taLeafItr i;
  Process* p;
  FOR_ITR_EL(Process, p, loop_procs., i) {
    if(p->mod.flag && (((c - p->mod.off) % (p->mod.m)) == 0))
      p->Run();
  }
}

void SchedProcess::LoopStats() {
  int c = GetCounter();
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, loop_stats., i) {
    if(st->mod.flag && (((c - st->mod.off) % (st->mod.m)) == 0)) {
      if((st->time_agg.from == NULL) && (st->loop_init == Stat::INIT_IN_LOOP)) st->Init();
      st->Run();
    }
  }
}

void SchedProcess::UpdateLogs() {
  if(logs.leaves == 0)
    return;
  GenLogData();			// use our own log_data
  if(log_data.items.size == 0)	// no data this time around
    return;
  PDPLog* lg;
  taLeafItr i;
  FOR_ITR_EL(PDPLog, lg, logs., i) {
    lg->NewData(log_data,this);
  }
  log_data.Reset();		// reset to prevent dangling pointers
}

void SchedProcess::UpdateState() {
#ifdef TA_GUI
  if(taMisc::gui_active)
    ctrl_panel.Revert();
#endif
}

bool SchedProcess::Crit() {
  if((cntr != NULL) && (cntr->Crit()))
    return true;		// reached criterion by counter, bail

  int lc = GetCounter();	// loop counter
  int fc = 0;			// final counter
  if(super_proc != NULL)
    fc = super_proc->GetCounter() + 1; // add 1 to simulate increment of parent log..

  taLeafItr i;
  Stat* st;
  FOR_ITR_EL(Stat, st, final_stats., i) {
    if(st->mod.flag && (((fc - st->mod.off) % (st->mod.m)) == 0) && st->Crit())
      return true;
  }
  FOR_ITR_EL(Stat, st, loop_stats., i) {
    if(st->mod.flag && (((lc - st->mod.off) % (st->mod.m)) == 0) && st->Crit())
      return true;
  }
  return false;
}

bool SchedProcess::StopCheck() {
  if(!running) return false;

  // first handle events
  HandleEvents();

  // check for a stop call
  if(stop_flag) return true;

  if(!stepping || !im_step_proc) return false;

  if(step.n <= 0) step.n = 1;

  if((sub_proc == NULL) || (GetCounter() % step.n == 0)) {
    im_step_proc = false;	// clear
    stop_flag = true;		// as if we hit stop button
    return true;
  }
  return false;
}

bool SchedProcess::HandleEvents() {
#ifdef DMEM_COMPILE
  if (taMisc::dmem_nprocs > 1) {
    if (taMisc::dmem_proc == 0) { // process the gui stuff
      // following is pdpMisc::WaitProc minus the DMem_WaitProc call!
      cssiSession::RunPending();
      bool cont = false;
      do {
	if(taMisc::gui_active) {
	  taiMisc::OpenWindows();
	  cont = taiMisc::WaitProc();
	}
	else {
	  cont = tabMisc::WaitProc();
	}
      } while(cont);

      pdpMisc::DMem_WaitProc(true);	// true = send a stop command to subprocs upon exiting!
    }
    else {
      pdpMisc::DMem_SubEventLoop(); // I'm a sub-proc: get commands until stop!
    }
    return 0;
  }
#endif

#ifdef TA_GUI
  if(!taMisc::gui_active)
    return false;

  while(pdpMisc::WaitProc());

  int rval = cssiSession::RunPending();
  return (bool)rval;
#else
//TODO: may need to call the session if we use it for Qt4
    return false;
#endif
}

////////////////////////////////////////////////////////
//	After the Loop
////////////////////////////////////////////////////////

void SchedProcess::FinalProcs() {
  int c = 0;
  if(super_proc != NULL)
    c = super_proc->GetCounter() + 1; // add 1 to simulate increment of parent log..
  taLeafItr i;
  Process* p;
  FOR_ITR_EL(Process, p, final_procs., i) {
    if(p->mod.flag && (((c - p->mod.off) % (p->mod.m)) == 0))
      p->Run();
  }
}

void SchedProcess::FinalStats() {
  int c = 0;
  if(super_proc != NULL)
    c = super_proc->GetCounter() + 1; // add 1 to simulate increment of parent log..
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, final_stats., i) {
    if(st->mod.flag && (((c - st->mod.off) % (st->mod.m)) == 0))
      st->Run();
  }
}

void SchedProcess::UpdateDisplays() {
#ifdef TA_GUI
  if(!taMisc::gui_active)
    return;

  ctrl_panel.Revert();

/*TODO  WinView* wv;
  taLeafItr i;
  FOR_ITR_EL(WinView, wv, displays., i) {
    wv->UpdateDisplay(this);
  } */

//TEMP:
  network->UpdateAllViews();
#endif
}

bool SchedProcess::FinalStepCheck() {
  if(!stepping || !im_step_proc) return false;
  im_step_proc = false;		// clear
  stop_flag = true;		// as if we hit stop button
  return true;
}

////////////////////////////////////////////////////////
//	Log generation
////////////////////////////////////////////////////////

void SchedProcess::GetCntrDataItems() {
  if(cntr == NULL) return;
  if(!cntr_items.el_typ->InheritsFrom(TA_DataItem))
    cntr_items.el_typ = &TA_DataItem;
  if(cntr_items.size < 1)
    cntr_items.EnforceSize(1);
  DataItem* it = (DataItem*)cntr_items.FastEl(0);
  it->SetNarrowName(cntr->name);
}

void SchedProcess::GenCntrLog(LogData* ld, bool gen) {
  if(super_proc) {
    super_proc->GenCntrLog(ld, true);
  }
  if(cntr && gen) {
    if(cntr_items.size < 1)
      GetCntrDataItems();
    ld->AddFloat(cntr_items.FastEl(0), (float)cntr->val);
  }
}

LogData& SchedProcess::GenLogData(LogData* ld) {
  if(ld == NULL)
    ld = &log_data;
  int lc = GetCounter();	// loop counter
  int fc = 0;			// final counter
  if(super_proc != NULL)
    fc = super_proc->GetCounter() + 1; // add 1 to simulate increment of parent log..
  ld->Reset();
  GenCntrLog(ld, log_counter);
  bool no_stats = true;
  if((loop_stats.leaves == 0) && (final_stats.leaves == 0))
    no_stats = false;		// always generate in this case
  Stat* st;
  taLeafItr i;
  // do 2 passes, first pass is to see if there are any data to be had:
  FOR_ITR_EL(Stat, st, loop_stats., i) {
    if(!st->log_stat || !st->mod.flag)
      continue;
    if(log_loop) {
      if(((lc - st->mod.off) % (st->mod.m)) == 0) {
	no_stats = false;
	break;
      }
    }
    else {
      no_stats = false;
      break;
    }
  }
  if(!log_loop) {
    FOR_ITR_EL(Stat, st, final_stats., i) {
      if(!st->log_stat || !st->mod.flag)
	continue;
      if(((fc - st->mod.off) % (st->mod.m)) == 0) {
	no_stats = false;
	break;
      }
    }
  }
  if(no_stats) {
    ld->Reset();
    return *ld;
  }
  // second pass - if data is being produced, then generate data from all logs!
  FOR_ITR_EL(Stat, st, loop_stats., i) {
    if(!st->log_stat || !st->mod.flag)
      continue;
    st->GenLogData(ld);
  }
  if(!log_loop) {
    FOR_ITR_EL(Stat, st, final_stats., i) {
      if(!st->log_stat || !st->mod.flag)
	continue;
      st->GenLogData(ld);
    }
  }
  return *ld;
}

////////////////////////////////////////////////////////
//	Actions menu: logs/updaters
////////////////////////////////////////////////////////

void SchedProcess::InitMyLogs() {
  PDPLog* lg;
  taLeafItr i;
  FOR_ITR_EL(PDPLog, lg, logs., i)
    lg->Clear();
}

// InitAllLogs in pdpshell.cc

void SchedProcess::InitNetwork() {
  if(network == NULL)
    return;
  network->InitWtState();
}

void SchedProcess::InitAll() {
  Init_force();
  InitNetwork();
  InitAllLogs();
}

void SchedProcess::RemoveFromLogs() {
  PDPLog* lg;
  taLeafItr i;
  FOR_ITR_EL(PDPLog, lg, logs., i) {
    if(lg->log_proc.Remove(this)) {
      lg->SyncLogViewUpdaters();
    }
  }
}

void SchedProcess::RemoveFromDisplays() {
/*TODO  WinView* wv;
  taLeafItr i;
  FOR_ITR_EL(WinView, wv, displays., i)
    wv->updaters.Remove(this); */
}

bool SchedProcess::CheckAllTypes() {
  CheckResetCache();		// reset the cache first
  return CheckAllTypes_impl();
}

bool SchedProcess::CheckAllTypes_impl() {
  if(!CheckNetwork())
    return false;

  // go depth-first on type checking to get most sensitive processes first
  // then get more generic things like stats, which will already have been
  // dealt with by inheritance
  if(sub_proc != NULL) {
    if(!sub_proc->CheckAllTypes_impl())
      return false;
  }

  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, loop_stats., i) {
    if(!st->CheckNetwork())
      return false;
  }
  FOR_ITR_EL(Stat, st, final_stats., i) {
    if(!st->CheckNetwork())
      return false;
  }
  Process* proc;
  FOR_ITR_EL(Process, proc, init_procs., i) {
    if(!proc->CheckNetwork())
      return false;
  }
  FOR_ITR_EL(Process, proc, loop_procs., i) {
    if(!proc->CheckNetwork())
      return false;
  }
  FOR_ITR_EL(Process, proc, final_procs., i) {
    if(!proc->CheckNetwork())
      return false;
  }

  return true;
}

void SchedProcess::UpdateLogUpdaters() {
  // make sure the logs have us in them too...
  PDPLog* plog;
  taLeafItr i;
  FOR_ITR_EL(PDPLog, plog, logs., i) {
    if(plog->log_proc.LinkUnique(this))
      plog->SyncLogViewUpdaters();
    if(plog->cur_proc != this) {
      if(taMisc::is_loading || taMisc::is_duplicating)
	plog->cur_proc = this;
      else {
	GenLogData();
	plog->NewHead(log_data, this);
      }
    }
  }
}

void SchedProcess::UpdateDisplayUpdaters() {
/*TODO  WinView* wv;
  taLeafItr i;
  FOR_ITR_EL(WinView, wv, displays., i)
    wv->updaters.LinkUnique(this); */
}


////////////////////////////////////////////////////////
//	Finding Processes
////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////
//	Structure Manipulation
////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////
// NEW_FUN
////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////
// Structure menu
////////////////////////////////////////////////////////

void SchedProcess::MoveToSubGp(const char* gp_name) {
  Process_MGroup* own_gp = (Process_MGroup*)GetOwner(&TA_Process_MGroup);
  Process_MGroup* nw_gp = (Process_MGroup*)own_gp->NewGp(1);
  nw_gp->taList_impl::name = gp_name;
  SchedProcess* cp = this;
  while(cp != NULL) {
    nw_gp->Transfer(cp);
    cp = cp->sub_proc;
  }
  taMisc::DelayedMenuUpdate(own_gp);
}

void SchedProcess::ChangeNameSuffix(const char* new_name_sufx) {
  SchedProcess* cp = this;
  while(cp != NULL) {
    if(cp->name.contains('_'))
      cp->name = cp->name.through('_') + new_name_sufx;
    cp = cp->sub_proc;
  }
  taMisc::DelayedMenuUpdate(this);
}

void SchedProcess::AddSuperProc(TypeDef* proc_type) {
  Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
  if((gp == NULL) || (proc_type == NULL) || !proc_type->InheritsFrom(&TA_SchedProcess))
    return;

  SchedProcess* prv_super = super_proc;	// cache orig
  SchedProcess* nw_proc = (SchedProcess*)taBase::MakeToken(proc_type);
  if(nw_proc == NULL) return;
  LinkSuperProc(nw_proc);
  int myidx = gp->FindEl(this);
  gp->Insert(nw_proc, myidx);	// initlinks called here, put in my place!
  if(prv_super != NULL) {
    prv_super->LinkSubProc(nw_proc); // insert into hierarchy
    super_proc->GetAggsFmSuperProc(); // new super gets aggs from its super!
  }
  else {
    // go through my stats and create aggregates of things that are already aggregators
    Stat* st;
    taLeafItr i;
    FOR_ITR_EL(Stat, st, loop_stats., i) {
      if(st->time_agg.from == NULL) continue; // ignore non-aggs
      st->CreateAggregates();
    }
  }
  taMisc::DelayedMenuUpdate(gp);
}

void SchedProcess::AddSubProc(TypeDef* proc_type) {
  Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
  if((gp == NULL) || (proc_type == NULL) || !proc_type->InheritsFrom(&TA_SchedProcess))
    return;

  SchedProcess* prv_sub = sub_proc;	// cache orig
  SchedProcess* nw_proc = (SchedProcess*)taBase::MakeToken(proc_type);
  if(nw_proc == NULL) return;
  LinkSubProc(nw_proc);
  int myidx = gp->FindEl(this);
  gp->Insert(nw_proc, myidx+1);	// initlinks called here, put in after me
  if(prv_sub != NULL) {
    nw_proc->LinkSubProc(prv_sub); 	// insert into hierarchy
  }
  sub_proc->GetAggsFmSuperProc(); // sub gets aggs from me!
  taMisc::DelayedMenuUpdate(gp);
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

////////////////////////////////////////////////////////
//	Create new accessory procs
////////////////////////////////////////////////////////

Stat* SchedProcess::NewStat(TypeDef* typ, StatLoc loc, int num, bool create_aggs) {
  if(typ == NULL) return NULL;

  Stat_Group* stat_gp = GetStatGroup(typ, loc);
  Stat* rval = (Stat*) stat_gp->New(num, typ);
  if(create_aggs) {
    int i;
    for(i = stat_gp->size -1;i >= (int) (stat_gp->size - num); i--) {
      stat_gp->FastEl(i)->CallFun("CreateAggregates");
    }
  }
  UpdateAfterEdit();	// update the process..
  taMisc::DelayedMenuUpdate(this);
  return rval;
}

Process* SchedProcess::NewProcess(TypeDef* typ, ProcLoc loc, int num) {
  if(typ == NULL) return NULL;

  Process_Group* prgp = GetProcGroup(loc);
  Process* rval = (Process*) prgp->New(num, typ);
  UpdateAfterEdit();	// update the process..
  taMisc::DelayedMenuUpdate(this);
  return rval;
}

SchedProcess* SchedProcess::NewSchedProc(TypeDef* typ, SchedProcLoc loc) {
  if(typ == NULL) return NULL;

  SchedProcess* rval;
  if(loc == SUPER_PROC) {
    AddSuperProc(typ);
    rval = super_proc;
  }
  else {
    AddSubProc(typ);
    rval = sub_proc;
  }
  return rval;
}

////////////////////////////////////////////////////////
//	helper functions for accessory proc groups
////////////////////////////////////////////////////////

// returns the default process to create given type of stat in
SchedProcess* SchedProcess::Default_StatProc(TypeDef* stat_td, Process_MGroup* procs) {
  TypeDef* tp = &TA_TrialProcess;
  String tp_nm = stat_td->OptionAfter("COMPUTE_IN_");
  if(!tp_nm.empty())
    tp = taMisc::types.FindName(tp_nm);
  if(tp == NULL)
    tp = &TA_TrialProcess;	// not a bad bet, in general..
  int idx;
  SchedProcess* sp = (SchedProcess*)procs->FindLeafType(tp, idx);
  if(sp == NULL)
    return (SchedProcess*)procs->Leaf(procs->leaves-1); // last one, when in doubt..
  SchedProcess* next_sp = (SchedProcess*)procs->Leaf(idx+1);
  if((next_sp != NULL) && (next_sp->InheritsFrom(tp)))	// if two in a row of same type, return lowest (2nd)
    return next_sp;
  return sp;
}

// returns the default stat group within sched proc to create stat in
Stat_Group* SchedProcess::Default_StatGroup(TypeDef* stat_td, SchedProcess* proc) {
  Stat_Group* stat_gp = &(proc->loop_stats);

  // figure out if it should be in loop or final stats of process
  if(((proc->sub_proc == NULL) || stat_td->HasOption("FINAL_STAT")) &&
     !stat_td->HasOption("LOOP_STAT"))
  {
    stat_gp = &(proc->final_stats);
  }
  return stat_gp;
}

Stat_Group* SchedProcess::GetStatGroup(TypeDef* stat_td, StatLoc loc) {
  Stat_Group* stat_gp;
  if(loc == DEFAULT)
    stat_gp = SchedProcess::Default_StatGroup(stat_td, this);
  else if(loc == LOOP_STATS)
    stat_gp = &(loop_stats);
  else
    stat_gp = &(final_stats);
  return stat_gp;
}

Process_Group* SchedProcess::GetProcGroup(ProcLoc loc) {
  Process_Group* prgp;
  if(loc == INIT_PROCS)
    prgp = &(init_procs);
  else if(loc == LOOP_PROCS)
    prgp = &(loop_procs);
  else
    prgp = &(final_procs);
  return prgp;
}

Stat* SchedProcess::MakeAggregator(Stat* of_stat, StatLoc in_loc, Aggregate::Operator agg_op) {
  Stat_Group* sgp = GetStatGroup(of_stat->GetTypeDef(), in_loc);
  Stat* nag = (Stat*)of_stat->Clone(); // clone original one
  // make sure it doesn't inherit any scripts when making an agg..
  nag->SetScript("");
  nag->script_string = "";
  nag->type = C_CODE;
  sgp->Add(nag);
  nag->time_agg.op = Stat::GetAggOpForProc(this, agg_op);
  taBase::SetPointer((TAPtr*)&(nag->time_agg.from), of_stat);
  nag->time_agg.UpdateAfterEdit();
  nag->UpdateAfterEdit();
  return nag;
}

////////////////////////////////////
// 	Hooks

void SchedProcess::ReplacePointersHook(TAPtr old) {
  // go through entire hierarchy and replace any pointer to old guy with me

  SchedProcess* spold = (SchedProcess*)old;

  String olcnm = spold->name;
  spold->SetDefaultName();
  String oldnm = spold->name;
  spold->name = olcnm;
  oldnm = oldnm.before('_');

  if(olcnm.contains(oldnm))
    SetDefaultName();		// return to the default for new guy!

  // copy does not by default get the sub and super proc ptrs -- we get them now:
  sub_proc_type = spold->sub_proc_type;
  taBase::SetPointer((TAPtr*)&sub_proc, spold->sub_proc);
  super_proc = spold->super_proc;
  // nor does it get the logs, get these too
  logs.BorrowUnique(spold->logs);  // don't have two procs to go the same logs
  // now copy aggregators from original cuz they don't otherwise work right..
  {
    Stat* st;
    Stat* cpst;
    taLeafItr i;
    taLeafItr cpi;
    for(st = (Stat*)loop_stats.FirstEl(i), cpst = (Stat*)spold->loop_stats.FirstEl(cpi);
	st && cpst;
	st = (Stat*)loop_stats.NextEl(i), cpst = (Stat*)spold->loop_stats.NextEl(cpi)) {
      if(st->GetTypeDef() == cpst->GetTypeDef()) {
	taBase::SetPointer((TAPtr*)&(st->time_agg.from),cpst->time_agg.from);
      }
    }
    for(st = (Stat*)final_stats.FirstEl(i), cpst = (Stat*)spold->final_stats.FirstEl(cpi);
	st && cpst;
	st = (Stat*)final_stats.NextEl(i), cpst = (Stat*)spold->final_stats.NextEl(cpi)) {
      if(st->GetTypeDef() == cpst->GetTypeDef()) {
	taBase::SetPointer((TAPtr*)&(st->time_agg.from),cpst->time_agg.from);
      }
    }
  }

  // make sure this guy doesn't delete everything below it!
  spold->sub_proc_type = NULL;
  taBase::SetPointer((TAPtr*)&(spold->sub_proc), NULL);
  // and disconnect from above
  spold->super_proc = NULL;

  // and now connect us up with our new folks:
  if(sub_proc != NULL) {
    sub_proc->super_proc = this;
  }
  if(super_proc != NULL) {	// sp is my super-proc
    super_proc->LinkSubProc(this);
  }

  Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
  if(gp == NULL) return;
  Process_MGroup* prv_gp = gp;
  while(gp != NULL) {		// get highest level of process mgroup!
    prv_gp = gp;
    gp = GET_OWNER(gp, Process_MGroup);
  }

  String old_path = old->GetPath();

  // now go through the procs and find any pointers to old!
  Process* proc;
  taLeafItr pi;
  FOR_ITR_EL(Process, proc, prv_gp->, pi) {
    if(!proc->InheritsFrom(&TA_SchedProcess)) continue;
    SchedProcess* sp = (SchedProcess*)proc;
    Stat* st;
    taLeafItr i;
    FOR_ITR_EL(Stat, st, sp->final_stats., i) {
      Stat* fm = st->time_agg.from;
      if(fm == NULL) continue;
      if(GET_OWNER(fm, SchedProcess) == spold) { // points to stat owned by old guy
	String nw_pth = fm->GetPath();
	nw_pth = nw_pth.after(old_path);
	Stat* myst = (Stat*)FindFromPath(nw_pth);
	if(myst != NULL) {
	  taBase::SetPointer((TAPtr*)&(st->time_agg.from), myst); // update pointer
	  st->UpdateAfterEdit();
	}
      }
    }
    FOR_ITR_EL(Stat, st, sp->loop_stats., i) {
      Stat* fm = st->time_agg.from;
      if(fm == NULL) continue;
      if(GET_OWNER(fm, SchedProcess) == spold) { // points to stat owned by old guy
	String nw_pth = fm->GetPath();
	nw_pth = nw_pth.after(old_path);
	Stat* myst = (Stat*)FindFromPath(nw_pth);
	if(myst != NULL) {
	  taBase::SetPointer((TAPtr*)&(st->time_agg.from), myst); // update pointer
	  st->UpdateAfterEdit();
	}
      }
    }
    // replace any links pointing to me!

    int ri;
    for(ri=0;ri<sp->init_procs.size; ri++) { // only works for top-level..
      Process* prc = (Process*)sp->init_procs.FastEl(ri);
      if(prc == spold) sp->init_procs.ReplaceLink(ri, this);
    }
    for(ri=0;ri<sp->loop_procs.size; ri++) { // only works for top-level..
      Process* prc = (Process*)sp->loop_procs.FastEl(ri);
      if(prc == spold) sp->loop_procs.ReplaceLink(ri, this);
    }
    for(ri=0;ri<sp->final_procs.size; ri++) { // only works for top-level..
      Process* prc = (Process*)sp->final_procs.FastEl(ri);
      if(prc == spold) sp->final_procs.ReplaceLink(ri, this);
    }
    // don't forget about the step process!

    if(sp->step.proc == spold) {
      taBase::SetPointer((TAPtr*)&(sp->step.proc), this);
    }

    tabMisc::DelayedUpdateAfterEdit(sp);	// update this guy to get any new pointers!
  }
  taMisc::DelayedMenuUpdate(prv_gp);
  tabMisc::DelayedUpdateAfterEdit(this);
  Process::ReplacePointersHook(old);
}

void SchedProcess::DuplicateElHook(SchedProcess* cp) {
  sub_proc_type = cp->sub_proc_type;
  if(sub_proc_type != NULL) {
    CreateSubProcs(false);
    if((sub_proc != NULL) && (cp->sub_proc != NULL)) {
      sub_proc->UnSafeCopy(cp->sub_proc);
      sub_proc->DuplicateElHook(cp->sub_proc);
    }
  }
  bool all_ok = true;
  Stat* st;
  Stat* cpst;
  taLeafItr i;
  taLeafItr cpi;
  for(st = (Stat*)loop_stats.FirstEl(i), cpst = (Stat*)cp->loop_stats.FirstEl(cpi);
      st && cpst;
      st = (Stat*)loop_stats.NextEl(i), cpst = (Stat*)cp->loop_stats.NextEl(cpi)) {
    if(st->GetTypeDef() == cpst->GetTypeDef()) {
      bool ok = st->FindOwnAggFrom(*cpst);
      if(!ok) all_ok = false;
    }
    else
      all_ok = false;
  }
  for(st = (Stat*)final_stats.FirstEl(i), cpst = (Stat*)cp->final_stats.FirstEl(cpi);
      st && cpst;
      st = (Stat*)final_stats.NextEl(i), cpst = (Stat*)cp->final_stats.NextEl(cpi)) {
    if(st->GetTypeDef() == cpst->GetTypeDef()) {
      bool ok = st->FindOwnAggFrom(*cpst);
      if(!ok) all_ok = false;
    }
    else
      all_ok = false;
  }
//    if(!all_ok) {
//      taMisc::Error("Warning: All time_agg.from aggregators on stats were not correctly set for duplicated object:",
//  		  GetPath());
//    }
  tabMisc::DelayedUpdateAfterEdit(this);
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
  if(tp != NULL) {
    CyclesToSettle* cs = (CyclesToSettle*)tp->loop_stats.FindType(&TA_CyclesToSettle);
    if(cs != NULL)
      cs->settle = this;
  }
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

bool TrialProcess::CheckAllTypes_impl() {
  if(!SchedProcess::CheckAllTypes_impl()) return false; // failed basic test
  return network->CheckConfig(this);
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
  dmem_nprocs_actual = 1;
#ifdef DMEM_COMPILE
  epc_gp = -1;
  epc_comm = (MPI_Comm)MPI_COMM_SELF;
#endif
}

void EpochProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(event_list, this);
  taBase::Own(trial, this);
}

void EpochProcess::CutLinks() {
#ifdef DMEM_COMPILE
  if(epc_comm != MPI_COMM_SELF) {
    DMEM_MPICALL(MPI_Comm_free((MPI_Comm*)&epc_comm), "EpochProcess::CutLinks", "epc Comm_free");
    DMEM_MPICALL(MPI_Group_free((MPI_Group*)&epc_gp), "EpochProcess::CutLinks", "epc Group_free");
  }
#endif
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
  GetCurEvent();
#ifdef DMEM_COMPILE
  if(network != NULL) {
    dmem_nprocs_actual = taMisc::dmem_nprocs / network->dmem_nprocs_actual;
    dmem_nprocs_actual = MIN(dmem_nprocs_actual, dmem_nprocs);
    if(dmem_nprocs_actual < 1) dmem_nprocs_actual = 1;
    if(dmem_nprocs_actual > 1) {
      if(dmem_nprocs_actual * network->dmem_nprocs_actual != taMisc::dmem_nprocs) {
	taMisc::Error("EpochProcess: Error -- total number of processes not an even multiple of net dmem_nprocs_actual:",
		      String(network->dmem_nprocs_actual));
      }
      if(wt_update == ON_LINE) {
	taMisc::Error("*** Warning: EpochProcess: ON_LINE mode does not work in distributed computing across epochs, switching to SMALL_BATCH");
	wt_update = SMALL_BATCH;
	batch_n = 1;
	batch_n_eff = 1;
      }
      if(wt_update == SMALL_BATCH) {
	batch_n_eff = batch_n / dmem_nprocs_actual;
	if(batch_n_eff <= 0) batch_n_eff = 1;
      }
    }
    AllocProcs();
  }
#endif
}

void EpochProcess::Init_impl() {
  SchedProcess::Init_impl();
  if((environment == NULL) || (network == NULL))
    return;

  environment->InitEvents();
  environment->UnSetLayers();

  trial.val = 0;

#ifdef DMEM_COMPILE
  AllocProcs();
  if(dmem_nprocs_actual > 1) {
    int myepc = taMisc::dmem_proc / network->dmem_nprocs_actual;
    trial.val = myepc;
  }
#endif

  GetEventList();
  GetCurEvent();
}

void EpochProcess::GetEventList() {
  event_list.Reset();
  trial.max = environment->EventCount();
  int i;
  for(i=0; i<trial.max; i++)
    event_list.Add(i);
  if(order == PERMUTED)
    event_list.Permute();
}

void EpochProcess::GetCurEvent() {
  if(environment == NULL) return;

  if(trial.max != environment->EventCount()) {
    GetEventList();
  }
  if(trial.val >= trial.max) {
    taBase::SetPointer((TAPtr*)&cur_event, NULL);
    return;
  }

  if((order == PERMUTED) || (order == SEQUENTIAL)) {
    if(trial.val >= event_list.size)
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
    else
      taBase::SetPointer((TAPtr*)&cur_event, environment->GetEvent(event_list[trial.val]));
  }
  else {
    if(trial.max > 0) {
      long evnt_no = MTRnd::genrand_int32() % trial.max;
      taBase::SetPointer((TAPtr*)&cur_event, environment->GetEvent((int)evnt_no));
    }
    else {
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
    }
  }
}

Event* EpochProcess::GetMyNextEvent() {
  if(environment == NULL) return NULL;
  if(trial.val + 1 >= trial.max) return NULL;

  if((order == PERMUTED) || (order == SEQUENTIAL)) {
    if(trial.val + 1 >= event_list.size)
      return NULL;
    return environment->GetEvent(event_list[trial.val+1]);
  }
  else {
    taMisc::Error("*** EpochProcess::GetMyNextEvent: cannot get next event for RANDOM event order!");
    return NULL;
  }
}

////////////////////////////////////////
// 0 1 2 3 4 5 6 7 8 9    event
// a b a b a b a b a b    proc = 2
// 0 0 1 1 2 2 3 3 4 4    mytrl batch_n
//     x x     x x    x x         2
//         x x        x x         3
//             x x    x x         4
//                 x x            5


void EpochProcess::Loop_UpdateWeights() {
  if((wt_update == TEST) || (wt_update == BATCH)) return; // nothing to do!
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual <= 1) {
#endif
    if(wt_update == ON_LINE)
      network->UpdateWeights();
    else if(((trial.val + 1) % batch_n_eff) == 0)
      network->UpdateWeights();
    return;
#ifdef DMEM_COMPILE
  }
  if(wt_update == ON_LINE)
    network->UpdateWeights();

  int mytrl = trial.val / dmem_nprocs_actual;

  if(((mytrl + 1) % batch_n_eff) == 0) { // time to synchronize!
    //    cerr << "proc: " << taMisc::dmem_proc << " loop sync on trl: " << mytrl << endl;
    DMem_UpdateWeights();
  }
#endif
}

void EpochProcess::Final_UpdateWeights() {
  if(wt_update == TEST) return; // nothing to do!
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual <= 1) {
#endif
    if(wt_update == BATCH)
      network->UpdateWeights();
    else if((wt_update == SMALL_BATCH) && ((trial.val % batch_n_eff) != 0))
      network->UpdateWeights();	// we didn't do this just before end of trial..
    return;
#ifdef DMEM_COMPILE
  }
  int mytrl = trial.val / dmem_nprocs_actual;
  int rmndr = trial.val % dmem_nprocs_actual;

  if(wt_update == BATCH)
    DMem_UpdateWeights();
  else if((mytrl % batch_n_eff) != 0) { // we didn't do this just before end of trial..
//     cerr << "proc: " << taMisc::dmem_proc << " final sync on trl: " << mytrl << endl;
    DMem_UpdateWeights();
  }
  else if(rmndr != 0) {
    // not an even distribution of events across processors: it is possible that
    // other procs are running one more event, and then doing an update weight
    int myepc = taMisc::dmem_proc / network->dmem_nprocs_actual;
    if((myepc >= rmndr) && (((mytrl + 1) % batch_n_eff) == 0)) {
      // i'm beyond the remainder, and other guys are going to update next step!
//       cerr << "proc: " << taMisc::dmem_proc << " final sync on trl: " << mytrl
// 	   << " rmndr: " << rmndr << endl;
      DMem_UpdateWeights();
    }
  }
#endif
}

void EpochProcess::Loop() {
  if((network == NULL) || (cur_event == NULL)) return;

  if(trial.max != environment->EventCount()) { // something changed!
    GetEventList();
    if(trial.val >= trial.max) {
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
      return;
    }
    GetCurEvent();		// get the new event
  }
  if(trial.val >= trial.max)
    return;
  if(sub_proc != NULL)
    sub_proc->Run();
  if(!bailing)
    Loop_UpdateWeights();
}

void EpochProcess::UpdateState() {
  SchedProcess::UpdateState();
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual > 1) {
    trial.val += dmem_nprocs_actual - 1;	// already incremented by 1, so add epc - 1
    if(trial.val > trial.max) trial.val = trial.max;
  }
#endif
  GetCurEvent();
}

bool EpochProcess::Crit() {
  bool b = SchedProcess::Crit();
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual > 1) {
    if(b && !((cntr != NULL) && cntr->Crit())) { // something else made us stop!
      taMisc::Error("*** Error: Cannot use stop criteria on stats under dmem across events in EpochProcess");
      return false;
    }
  }
#endif
  if(b || (cur_event == NULL))
    return true;
  return false;
}

void EpochProcess::Final() {
  if(network == NULL) return;
  Final_UpdateWeights();
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual > 1) {
    int n_stats = MIN(dmem_nprocs_actual, trial.val);// number of procs w/ stats
    SyncLoopStats(this, (MPI_Comm)epc_comm, n_stats);
  }
#endif
}

void EpochProcess::GetCntrDataItems() {
  if(cntr_items.size < 2)
    cntr_items.EnforceSize(2);
  SchedProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetStringName("Event");
}

void EpochProcess::GenCntrLog(LogData* ld, bool gen) {
  SchedProcess::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 2)
      GetCntrDataItems();
    if(cur_event != NULL)
      ld->AddString(cntr_items.FastEl(1), cur_event->name);
    else
      ld->AddString(cntr_items.FastEl(1), "n/a");
  }
}

void EpochProcess::NewSeed() {
  rndm_seed.NewSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}

void EpochProcess::OldSeed() {
  rndm_seed.OldSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}

#ifdef DMEM_COMPILE

void EpochProcess::DMem_UpdateWeights() {
  if(wt_update == ON_LINE)
    network->DMem_SyncWts(epc_comm, false);
  else {
    network->DMem_SyncWts(epc_comm, true);	// sum the dwts
    network->UpdateWeights();
  }
}

void EpochProcess::AllocProcs() {
  int epcsz = 0; MPI_Comm_size((MPI_Comm)epc_comm, &epcsz);
  if(epcsz == dmem_nprocs_actual) return;

  // outer-loop is epochs, inner loop is networks
  // e0:   e1:
  // n0 n1 n0 n1
  // 0  1  2  3
  int mynet = taMisc::dmem_proc % network->dmem_nprocs_actual;

  if(dmem_nprocs_actual > 1) {
    MPI_Group worldgp;
    DMEM_MPICALL(MPI_Comm_group(MPI_COMM_WORLD, &worldgp), "EpochProcess::AllocProcs", "Comm_group");

    if(epc_comm != MPI_COMM_SELF) {
      DMEM_MPICALL(MPI_Comm_free((MPI_Comm*)&epc_comm), "EpochProcess::AllocProcs", "epc Comm_free");
      DMEM_MPICALL(MPI_Group_free((MPI_Group*)&epc_gp), "EpochProcess::AllocProcs", "epc Group_free");
    }

    int epc_ranks[dmem_nprocs_actual];
    for(int i = 0;i<dmem_nprocs_actual; i++)
      epc_ranks[i] = mynet + (i * network->dmem_nprocs_actual);
    DMEM_MPICALL(MPI_Group_incl(worldgp, dmem_nprocs_actual, epc_ranks, (MPI_Group*)&epc_gp),
		       "EpochProcess::AllocProcs", "epc Group_incl");
    DMEM_MPICALL(MPI_Comm_create(MPI_COMM_WORLD, (MPI_Group)epc_gp, (MPI_Comm*)&epc_comm),
		       "EpochProcess::AllocProcs", "epc Comm_create");
  }
  else {
    dmem_nprocs_actual = 1;
    epc_comm = MPI_COMM_SELF;
    epc_gp = -1;
  }
}

void EpochProcess::SyncLoopStats(SchedProcess* sp, MPI_Comm ec, int n_stats) {
  static float_Array values;
  static float_Array results;

  int np = 0; MPI_Comm_size(ec, &np);
  if(np <= 1) return;
  int this_proc = 0; MPI_Comm_rank(ec, &this_proc);

  values.size = 0;
  Stat* st;
  taLeafItr si;
  FOR_ITR_EL(Stat, st, sp->loop_stats., si) {
    if(!st->mod.flag || (st->time_agg.from == NULL) || (st->time_agg.n_updt == 0))
      continue; // only loop stats, with actual data
    if(st->n_copy_vals > 0) {
      for(int i=0;i<st->copy_vals.size;i++) {
	StatVal* sv = (StatVal*)st->copy_vals.FastEl(i);
	values.Add(sv->val);
      }
    }
    else {
      TypeDef* td = st->GetTypeDef();
      int m;
      for(m=TA_Stat.members.size; m<td->members.size; m++) {
	MemberDef* md = td->members.FastEl(m);
	if(md->type->InheritsFrom(&TA_StatVal)) {
	  StatVal* sv = (StatVal*)md->GetOff(st);
	  if(!sv->is_string)
	    values.Add(sv->val);
	}
	else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	  StatVal_List* svl = (StatVal_List*)md->GetOff(st);
	  for(int i=0;i<svl->size;i++) {
	    StatVal* sv = (StatVal*)svl->FastEl(i);
	    values.Add(sv->val);
	  }
	}
      }
    }
  }

  if(values.size == 0) return;
  results.EnforceSize(values.size * np);

  DMEM_MPICALL(MPI_Allgather(values.el, values.size, MPI_FLOAT,
			     results.el, values.size, MPI_FLOAT, ec),
	       "EpochProcess::SyncLoopStats", "Allgather");

  int vidx = 0;
  FOR_ITR_EL(Stat, st, sp->loop_stats., si) {
    if(!st->mod.flag || (st->time_agg.from == NULL) || (st->time_agg.n_updt == 0))
      continue; // only loop stats, with actual data
    bool is_cnt = false;
    if(st->time_agg.op == Aggregate::COUNT) {
      is_cnt = true;
      st->time_agg.op = Aggregate::SUM;	// switch to SUM for COUNT -- get it right..
    }
    if(st->n_copy_vals > 0) {
      for(int k=0;k<n_stats;k++) { // note: going to n_stats here, not np!
	if(k == this_proc) continue;
	for(int i=0;i<st->copy_vals.size;i++) {
	  StatVal* sv = (StatVal*)st->copy_vals.FastEl(i);
	  StatVal nsv = *sv;
	  nsv.val = results.FastEl(k * values.size + vidx + i);
	  st->time_agg.ComputeAggNoUpdt(sv, &nsv);
	}
	st->time_agg.IncUpdt();
      }
      vidx += st->copy_vals.size;
    }
    else {
      TypeDef* td = st->GetTypeDef();
      int m;
      for(m=TA_Stat.members.size; m<td->members.size; m++) {
	MemberDef* md = td->members.FastEl(m);
	if(md->type->InheritsFrom(&TA_StatVal)) {
	  StatVal* sv = (StatVal*)md->GetOff(st);
	  if(sv->is_string) continue;
	  for(int k=0;k<n_stats;k++) { // note: n_stats
	    if(k == this_proc) continue;
	    StatVal nsv = *sv;
	    nsv.val = results.FastEl(k * values.size + vidx);
	    st->time_agg.ComputeAgg(sv, &nsv);
	  }
	  vidx++;
	}
	else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	  StatVal_List* svl = (StatVal_List*)md->GetOff(st);

	  for(int k=0;k<n_stats;k++) { // note: n_stats
	    if(k == this_proc) continue;
	    for(int i=0;i<svl->size;i++) {
	      StatVal* sv = (StatVal*)svl->FastEl(i);
	      StatVal nsv = *sv;
	      nsv.val = results.FastEl(k * values.size + vidx + i);
	      st->time_agg.ComputeAggNoUpdt(sv, &nsv);
	    }
	  }
	  vidx += svl->size;
	}
      }
    }
    if(is_cnt) st->time_agg.op = Aggregate::COUNT;
  }
}

#endif

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
  GetCurEvent();
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

#ifdef DMEM_COMPILE
  if(sequence_epoch->dmem_nprocs_actual > 1) {
    if((sequence_epoch->wt_update != EpochProcess::BATCH) &&
       (sequence_epoch->small_batch == SequenceEpoch::EVENT)) {
      int myepc = taMisc::dmem_proc / network->dmem_nprocs_actual;
      tick.val = myepc;
    }
  }
#endif

  GetCurEvent();
  InitNetState();
}

void SequenceProcess::GetEventList() {
  event_list.Reset();
  tick.max = cur_event_gp->EventCount();
  int i;
  for(i=0; i<tick.max; i++)
    event_list.Add(i);
  if(order == PERMUTED)
    event_list.Permute();
}

void SequenceProcess::GetCurEvent() {
  if((cur_event_gp == NULL) || (sequence_epoch == NULL))
    return;

  if(tick.max != cur_event_gp->EventCount()) {
    GetEventList();
  }
  if(tick.val >= tick.max) {
    taBase::SetPointer((TAPtr*)&cur_event, NULL);
    return;
  }

  if((order == PERMUTED) || (order == SEQUENTIAL)) {
    if(tick.val >= event_list.size)
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
    else
      taBase::SetPointer((TAPtr*)&cur_event, (Event*)cur_event_gp->GetEvent(event_list[tick.val]));
  }
  else {
    if(tick.max > 0) {
      long evnt_no = MTRnd::genrand_int32() % tick.max;
      taBase::SetPointer((TAPtr*)&cur_event, (Event*)cur_event_gp->GetEvent((int)evnt_no));
    }
    else {
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
    }
  }
  // copy cur_event to epoch so that trial process can access it there..
  taBase::SetPointer((TAPtr*)&(sequence_epoch->cur_event), cur_event);
}

Event* SequenceProcess::GetMyNextEvent() {
  if((cur_event_gp == NULL) || (sequence_epoch == NULL))
    return NULL;

  if(tick.val + 1 >= tick.max) return NULL;

  if((order == PERMUTED) || (order == SEQUENTIAL)) {
    if(tick.val + 1 >= event_list.size)
      return NULL;
    return cur_event_gp->GetEvent(event_list[tick.val+1]);
  }
  else {
    taMisc::Error("*** SequenceProcess::GetMyNextEvent: cannot get next event for RANDOM event order!");
    return NULL;
  }
}

#ifdef DMEM_COMPILE

void SequenceProcess::DMem_UpdateWeights() {
  if(sequence_epoch->wt_update == EpochProcess::ON_LINE)
    network->DMem_SyncWts(sequence_epoch->epc_comm, false);
  else {
    network->DMem_SyncWts(sequence_epoch->epc_comm, true);	// sum the dwts
    network->UpdateWeights();
  }
}

#endif

void SequenceProcess::Loop_UpdateWeights() {
  if(sequence_epoch->wt_update == EpochProcess::TEST) return; // nothing to do!
  if((sequence_epoch->wt_update == EpochProcess::BATCH) ||
     ((sequence_epoch->wt_update == EpochProcess::SMALL_BATCH) &&
      (sequence_epoch->small_batch == SequenceEpoch::SEQUENCE)))
    return; // nothing for batch mode to do!
#ifdef DMEM_COMPILE
  if(sequence_epoch->dmem_nprocs_actual <= 1) {
#endif
    if(sequence_epoch->wt_update == EpochProcess::ON_LINE)
      network->UpdateWeights();
    else if(((tick.val + 1) % sequence_epoch->batch_n_eff) == 0)
      network->UpdateWeights();
    return;
#ifdef DMEM_COMPILE
  }
  if(sequence_epoch->wt_update == EpochProcess::ON_LINE) {
    network->UpdateWeights();
    if(sequence_epoch->small_batch == SequenceEpoch::SEQUENCE) return;
  }

  int mytrl = tick.val / sequence_epoch->dmem_nprocs_actual;

  if(((mytrl + 1) % sequence_epoch->batch_n_eff) == 0) // time to synchronize!
    DMem_UpdateWeights();
#endif
}

void SequenceProcess::Final_UpdateWeights() {
  if(sequence_epoch->wt_update == EpochProcess::TEST) return; // nothing to do!
  // only for small_batch/online, event mode..
  if((sequence_epoch->wt_update == EpochProcess::BATCH) ||
     (sequence_epoch->wt_update == EpochProcess::ON_LINE) ||
     (sequence_epoch->small_batch == SequenceEpoch::SEQUENCE)) return;
#ifdef DMEM_COMPILE
  if(sequence_epoch->dmem_nprocs_actual <= 1) {
#endif
    if((sequence_epoch->wt_update == EpochProcess::SMALL_BATCH) &&
       (tick.val % sequence_epoch->batch_n_eff) != 0)
      network->UpdateWeights();	// we didn't do this just before end of trial..
    return;
#ifdef DMEM_COMPILE
  }
  int mytrl = tick.val / sequence_epoch->dmem_nprocs_actual;
  int rmndr = tick.val % sequence_epoch->dmem_nprocs_actual;

  if((mytrl % sequence_epoch->batch_n_eff) != 0) // we didn't do this just before end of trial..
    DMem_UpdateWeights();
  else if(rmndr != 0) {
    // not an even distribution of events across processors: it is possible that
    // other procs are running one more event, and then doing an update weight
    int myepc = taMisc::dmem_proc / network->dmem_nprocs_actual;
    if((myepc >= rmndr) && (((mytrl + 1) % sequence_epoch->batch_n_eff) == 0)) {
      // i'm beyond the remainder, and other guys are going to update next step!
      DMem_UpdateWeights();
    }
  }
#endif
}

void SequenceProcess::Loop() {
  if((network == NULL) || (cur_event_gp == NULL) || (sequence_epoch == NULL) || (cur_event == NULL))
    return;
  if(tick.max != cur_event_gp->EventCount()) {
    GetEventList();
    if(tick.val >= tick.max) {
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
      return;
    }
    GetCurEvent();
  }
  if(tick.val >= tick.max)
    return;
  if(sub_proc != NULL)
    sub_proc->Run();
  if(!bailing)
    Loop_UpdateWeights();
}

void SequenceProcess::UpdateState() {
  SchedProcess::UpdateState();
#ifdef DMEM_COMPILE
  if((sequence_epoch != NULL) && (sequence_epoch->wt_update != EpochProcess::BATCH) &&
     (sequence_epoch->small_batch == SequenceEpoch::EVENT)) {
    if(sequence_epoch->dmem_nprocs_actual > 1) {
      tick.val += sequence_epoch->dmem_nprocs_actual - 1; // already incremented by 1, so add epc - 1
      if(tick.val > tick.max) tick.val = tick.max;
    }
  }
#endif
  GetCurEvent();
}

bool SequenceProcess::Crit() {
  bool b = SchedProcess::Crit();
#ifdef DMEM_COMPILE
  if((sequence_epoch != NULL) && (sequence_epoch->wt_update != EpochProcess::BATCH) &&
     (sequence_epoch->small_batch == SequenceEpoch::EVENT)) {
    if(sequence_epoch->dmem_nprocs_actual > 1) {
      if(b && !((cntr != NULL) && cntr->Crit())) { // something else made us stop!
	taMisc::Error("*** Error: Cannot use stop criteria on stats under dmem across events in SequenceProcess");
	return false;
      }
    }
  }
#endif
  if(b || (cur_event_gp == NULL) || (cur_event == NULL))
    return true;
  return false;
}

void SequenceProcess::Final() {
  if((network == NULL) || (sequence_epoch == NULL))
    return;
  Final_UpdateWeights();
#ifdef DMEM_COMPILE
  if(sequence_epoch->dmem_nprocs_actual > 1) {
    if((sequence_epoch != NULL) &&
       (sequence_epoch->wt_update != EpochProcess::BATCH) &&
       (sequence_epoch->small_batch == SequenceEpoch::EVENT)) {
      int n_stats = MIN(sequence_epoch->dmem_nprocs_actual, tick.val);// number of procs w/ stats
      EpochProcess::SyncLoopStats(this, (MPI_Comm)sequence_epoch->epc_comm, n_stats);
    }
  }
#endif
}

void SequenceProcess::InitNetState() {
  if(sequence_init == INIT_STATE)
    network->InitState();
  else if(sequence_init == MODIFY_STATE)
    network->ModifyState();
}

void SequenceProcess::GetCntrDataItems() {
  if(cntr_items.size < 2)
    cntr_items.EnforceSize(2);
  SchedProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetStringName("Event");
}

void SequenceProcess::GenCntrLog(LogData* ld, bool gen) {
  SchedProcess::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 2)
      GetCntrDataItems();
    if(cur_event != NULL)
      ld->AddString(cntr_items.FastEl(1), cur_event->name);
    else
      ld->AddString(cntr_items.FastEl(1), "n/a");
  }
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
  GetCurEvent();
}

void SequenceEpoch::Init_impl() {
  SchedProcess::Init_impl();
  if((environment == NULL) || (network == NULL))
    return;

  environment->InitEvents();
  environment->UnSetLayers();

  trial.val = 0;

#ifdef DMEM_COMPILE
  AllocProcs();
  if(dmem_nprocs_actual > 1) {
    if((wt_update == BATCH) || (small_batch == SEQUENCE)) {
      int myepc = taMisc::dmem_proc / network->dmem_nprocs_actual;
      trial.val = myepc;
    }
  }
#endif

  GetEventList();
  GetCurEvent();
}

void SequenceEpoch::GetEventList() {
  event_list.Reset();
  trial.max = environment->GroupCount();
  if(trial.max == 0) trial.max = 1; // just present all the events themselves!
  int i;
  for(i=0; i<trial.max; i++)
    event_list.Add(i);
  if(order == PERMUTED)
    event_list.Permute();
}

void SequenceEpoch::GetCurEvent() {
  if(environment == NULL) {
    taBase::SetPointer((TAPtr*)&cur_event_gp, NULL);
    return;
  }

  if(environment->GroupCount() == 0) {
    trial.max = 1;
    // group = master event list
    taBase::SetPointer((TAPtr*)&cur_event_gp, &(environment->events));
    return;
  }

  if(trial.max != environment->GroupCount()) {
    GetEventList();
  }
  if(trial.val >= trial.max) {
    taBase::SetPointer((TAPtr*)&cur_event_gp, NULL);
    return;
  }

  if((order == PERMUTED) || (order == SEQUENTIAL)) {
    if(trial.val >= event_list.size)
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
    else
      taBase::SetPointer((TAPtr*)&cur_event_gp, environment->GetGroup(event_list[trial.val]));
  }
  else {
    if(trial.max > 0) {
      long evnt_no = MTRnd::genrand_int32() % trial.max;
      taBase::SetPointer((TAPtr*)&cur_event_gp, environment->GetGroup((int)evnt_no));
    }
    else {
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
    }
  }
}

Event* SequenceEpoch::GetMyNextEvent() {
  SequenceProcess* seqpr = (SequenceProcess*)FindSubProc(&TA_SequenceProcess);
  if(seqpr == NULL) return NULL;
  return seqpr->GetMyNextEvent();
}

void SequenceEpoch::Loop_UpdateWeights() {
  if((wt_update == TEST) || (wt_update == BATCH) || (small_batch == EVENT)) return; // nothing to do!
  // on_line, small_batch event mode is dealth with entirely in SequenceProcess
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual <= 1) {
#endif
    if((wt_update == SMALL_BATCH) && ((trial.val + 1) % batch_n_eff) == 0)
      network->UpdateWeights();
    return;
#ifdef DMEM_COMPILE
  }
  int mytrl = trial.val / dmem_nprocs_actual;

  if(((mytrl + 1) % batch_n_eff) == 0)
    DMem_UpdateWeights();
#endif
}

void SequenceEpoch::Final_UpdateWeights() {
  if(wt_update == TEST) return;
  if((wt_update != BATCH) && (small_batch == EVENT)) return; // nothing for event mode to do
  EpochProcess::Final_UpdateWeights();
}

void SequenceEpoch::Loop() {
  if(cur_event_gp == NULL) return;

  if((trial.max != environment->GroupCount()) &&
      !((trial.max == 1) && (environment->GroupCount() == 0))) {
    GetEventList();
    if(trial.val >= trial.max) {
      taBase::SetPointer((TAPtr*)&cur_event_gp, NULL);
      return;
    }
    GetCurEvent();
  }
  if(trial.val >= trial.max)
    return;
  if(sub_proc != NULL)
    sub_proc->Run();
  if(!bailing)
    Loop_UpdateWeights();
}

bool SequenceEpoch::Crit() {
  bool b = SchedProcess::Crit();
#ifdef DMEM_COMPILE
  if((wt_update == BATCH) || (small_batch == SEQUENCE)) {
    if(dmem_nprocs_actual > 1) {
      if(b && !((cntr != NULL) && cntr->Crit())) { // something else made us stop!
	taMisc::Error("*** Error: Cannot use stop criteria on stats under dmem across events in SequenceEpoch");
	return false;
      }
    }
  }
#endif
  if(b || (cur_event_gp == NULL))
    return true;
  return false;
}

void SequenceEpoch::Final() {
  if(network == NULL) return;
  Final_UpdateWeights();
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual > 1) {
    if(!((wt_update == SMALL_BATCH) && (small_batch == EVENT))) {
      int n_stats = MIN(dmem_nprocs_actual, trial.val);// number of procs w/ stats
      SyncLoopStats(this, (MPI_Comm)epc_comm, n_stats);
    }
  }
#endif
}
void SequenceEpoch::UpdateState() {
  SchedProcess::UpdateState();
#ifdef DMEM_COMPILE
  if(dmem_nprocs_actual > 1) {
    if(small_batch == SEQUENCE)
      trial.val += dmem_nprocs_actual - 1;	// already incremented by 1, so add epc - 1
    if(trial.val > trial.max) trial.val = trial.max;
  }
#endif
  GetCurEvent();
}

void SequenceEpoch::GetCntrDataItems() {
  if(cntr_items.size < 2)
    cntr_items.EnforceSize(2);
  SchedProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetStringName("EventGp");
}

void SequenceEpoch::GenCntrLog(LogData* ld, bool gen) {
  SchedProcess::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 2)
      GetCntrDataItems();
    if((cur_event_gp != NULL) && (!cur_event_gp->taList_impl::name.empty()))
      ld->AddString(cntr_items.FastEl(1), cur_event_gp->taList_impl::name);
    else
      ld->AddString(cntr_items.FastEl(1), "n/a");
  }
}

//////////////////////////////////
// 	InteractiveEpoch	//
//////////////////////////////////

void InteractiveEpoch::Initialize() {
  order = SEQUENTIAL;
  last_trial_val = -1;
}

void InteractiveEpoch::UpdateAfterEdit() {
  EpochProcess::UpdateAfterEdit();
}

void InteractiveEpoch::GetEventList() {
  event_list.Reset();
  // do not reset: max is fixed.
  //  trial.max = environment->EventCount();
}

void InteractiveEpoch::GetCurEvent() {
  if(environment == NULL) return;

  if((trial.val == last_trial_val) && (cur_event != NULL)) return;

  Event* nxt_ev = environment->GetNextEvent();
  taBase::SetPointer((TAPtr*)&cur_event, nxt_ev);
  if(cur_event == NULL)
    trial.max = trial.val;	// we're done
  else if(trial.max <= trial.val)
    trial.max = trial.val + 1;	// keep ahead of the game
  last_trial_val = trial.val;
}

void InteractiveEpoch::Loop() {
  if((network == NULL) || (cur_event == NULL)) return;
  if(trial.val >= trial.max)
    return;
  if(sub_proc != NULL)
    sub_proc->Run();
  if(!bailing)
    Loop_UpdateWeights();
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
  SchedProcess::Init_impl();
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

void NEpochProcess::GenCntrLog(LogData* ld, bool gen) {
  SchedProcess::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 2)
      GetCntrDataItems();
    ld->AddFloat(cntr_items.FastEl(1), (float)epoch);
  }
}

void NEpochProcess::NewSeed() {
  rndm_seed.NewSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}

void NEpochProcess::OldSeed() {
  rndm_seed.OldSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}

void NEpochProcess::UpdateCounters() {
  SchedProcess::UpdateCounters();
  if(network == NULL)	return;
  if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST))
    network->epoch++;
  epoch = network->epoch;
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

void TrainProcess::InitAll() {
  Init_force();
//  InitNetwork();   // this is done by the init_force anyway..
  InitAllLogs();
}

void TrainProcess::Init_impl() {
  if(network != NULL) {		// always init network before counter!
    if(network->re_init) {
      network->InitWtState();	// this is what it means to init a train process..
    }
  }
  SchedProcess::Init_impl();
}

void TrainProcess::SetReInit(bool ri_val) {
  SchedProcess::SetReInit(ri_val);
  if(network != NULL)
    network->re_init = ri_val;
}

void TrainProcess::UpdateCounters() {
  SchedProcess::UpdateCounters();
  if(network == NULL)	return;
  if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST))
    network->epoch++;
  epoch.val = network->epoch;
}

void TrainProcess::GetCntrDataItems() {
  SchedProcess::GetCntrDataItems();
}

void TrainProcess::GenCntrLog(LogData* ld, bool gen) {
  SchedProcess::GenCntrLog(ld, gen);
}

void TrainProcess::NewSeed() {
  rndm_seed.NewSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}

void TrainProcess::OldSeed() {
  rndm_seed.OldSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}

void TrainProcess::Run_gui() {
  SchedProcess::Run_gui();
  SetReInit(false); 	// never re-init automatically from gui (only by way of newinit, etc)
}

void TrainProcess::Step() {
  SchedProcess::Step();
  SetReInit(false); 	// never re-init automatically from gui (only by way of newinit, etc)
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

void BatchProcess::NewSeed() {
  rndm_seed.NewSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}

void BatchProcess::OldSeed() {
  rndm_seed.OldSeed();
#ifdef DMEM_COMPILE
  DMem_SyncAllSeeds();
#endif
}
