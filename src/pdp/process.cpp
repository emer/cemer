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



// processes.cc

#include "process.h"
#include "pdpshell.h"
#include "sched_proc.h"
#include "ta_css.h"

#ifdef TA_GUI
#include <qdialog.h>
#include "ta_qt.h"
#include "ta_qtgroup.h"
#include "css_qt.h"
#include "pdp_qtso.h"
#endif

#ifdef TA_GUI
//////////////////////////
// 	CtrlPanelData	//
//////////////////////////

void CtrlPanelData::Initialize() {
  ctrl_panel = NULL;
  active = false;
  lft = 0.0f;
  top = 0.0f;
}

void CtrlPanelData::Copy_(const CtrlPanelData& cp) {
  lft = cp.lft;
  top = cp.top;
}

void CtrlPanelData::UpdateAfterEdit() {
  taBase::UpdateAfterEdit();
  if(taMisc::gui_active && active && (ctrl_panel == NULL) && (owner != NULL)) {
    ((Process*)owner)->ControlPanel();
  }
  SetWinPos();
}

void CtrlPanelData::GetPanel() {
  if(!taMisc::gui_active || (owner == NULL))
    return;
  ctrl_panel = (ProcessDialog*)taiMisc::FindEdit((void*)owner, owner->GetTypeDef());
  if((ctrl_panel != NULL) && (ctrl_panel->CtrlPanel()))
    active = true;
  else {
    active = false;
    ctrl_panel = NULL;
  }
}

void CtrlPanelData::GetWinPos() {
  if(!taMisc::gui_active) return;
  GetPanel();
  if(ctrl_panel == NULL)	return;
//  Dialog* win = ctrl_panel->dialog;
  QWidget* win = ctrl_panel->widget(); //NOTE: could be a panel!!!
  active = true;
  lft = win->x();
  top = win->y();
}

void CtrlPanelData::ScriptWinPos(ostream& strm) {
  if(!taMisc::gui_active || (owner == NULL)) return;
  GetPanel();
  if(ctrl_panel == NULL)	return;
  String temp = owner->GetPath() + ".ctrl_panel.Place("
    + String(lft) + ", " +  String(top) + ");\n";
  if (taMisc::record_script != NULL)  taMisc::RecordScript(temp);
  else   strm << temp;
}


void CtrlPanelData::SetWinPos() {
  if(!taMisc::gui_active || !active || (ctrl_panel == NULL) || ((lft == 0.0f) && (top == 0.0f)))
    return;
  GetPanel();
  if(ctrl_panel == NULL)	return;
/*TODO  float newleft, newtop;
  ivDisplay* dsp = ivSession::instance()->default_display();
  ivTopLevelWindow* win = ((NoBlockDialog*)ctrl_panel->dialog)->win;
  newleft =    MIN(lft,dsp->width() - win->width());
  newtop =  MIN(top,dsp->height() - win->height() - 20.0f); // allow for window decor
  lft = newleft;
  top = newtop;
  win->move(lft + taMisc::window_decor_offset_x, top + taMisc::window_decor_offset_y); */
}

void CtrlPanelData::Place(float left, float top) {
  lft = left;
  top = top;
  SetWinPos();
}

void CtrlPanelData::Revert() {
  if(!taMisc::gui_active || !active || (ctrl_panel == NULL))
    return;
  ctrl_panel->GetImage();
}
#endif // TA_GUI
//////////////////////////
// 	Process		//
//////////////////////////

void Process::Initialize() {
  type=C_CODE;
  project = NULL;
  network = NULL;
  environment = NULL;
  min_network = &TA_Network;
  min_layer = &TA_Layer;
  min_unit = &TA_Unit;
  min_con_group = &TA_Con_Group;
  min_con = &TA_Connection;
}

void Process::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(mod, this);
  taBase::Own(rndm_seed, this);
  taBase::Own(log_data, this);
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
  if((taMisc::gui_active) && !taMisc::is_loading) { // for linked sub-process
    ctrl_panel.GetPanel();
    if(ctrl_panel.ctrl_panel != NULL)
      ctrl_panel.ctrl_panel->Cancel();
  }
#endif
  // un-set any pointers to this object!
  Project* proj = GET_MY_OWNER(Project);
  if((proj != NULL) && !proj->deleting) {
    in_repl = true;
    taMisc::ReplaceAllPtrs(GetTypeDef(), (void*)this, NULL);
    in_repl = false;
  }
  taNBase::CutLinks();
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
  min_network = cp.min_network;
  min_layer = cp.min_layer;
  min_unit = cp.min_unit;
  min_con_group = cp.min_con_group;
  min_con = cp.min_con;
}

void Process::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  if((!script_file->fname.empty() && script_file->file_selected) || !script_string.empty())
    type = SCRIPT;
  if(type == SCRIPT) {
    if((script == NULL) || (script->run == cssEl::Waiting)
       || (script->run == cssEl::Stopping))
      UpdateReCompile();
  }
}

void Process::CopyPNEPtrs(Network* net, Environment* env) {
  taBase::SetPointer((TAPtr*)&network, net);
  taBase::SetPointer((TAPtr*)&environment, env);
}

void Process::SetEnv(Environment* env) {
  taBase::SetPointer((TAPtr*)&environment, env);
  UpdateAfterEdit();
}

void Process::SetNet(Network* net) {
  taBase::SetPointer((TAPtr*)&network, net);
  UpdateAfterEdit();
}

void Process::NewInit() {
  NewSeed();
  Init();
}

void Process::ReInit() {
  OldSeed();
  Init();
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

void Process::NewSeed() {
  rndm_seed.NewSeed();
#ifdef DMEM_COMPILE
  DMem_SyncSameNetSeeds();
#endif
}

void Process::OldSeed() {
  rndm_seed.OldSeed();
#ifdef DMEM_COMPILE
  DMem_SyncSameNetSeeds();
#endif
}

#ifdef DMEM_COMPILE
void Process::DMem_SyncSameNetSeeds() {
  if((taMisc::dmem_nprocs <= 1) || (network == NULL) || (network->dmem_nprocs_actual <= 1))
    return;

  // just blast the first guy to all members of the same communicator
  DMEM_MPICALL(MPI_Bcast(rndm_seed.seed.el, MTRnd::N, MPI_LONG, 0, network->dmem_share_units.comm),
	       "Process::SyncSameNetSeeds", "Bcast");
  rndm_seed.OldSeed();		// then get my seed!
}

void Process::DMem_SyncAllSeeds() {
  if(taMisc::dmem_nprocs <= 1)
    return;

  // just blast the first guy to all members of the same communicator
  DMEM_MPICALL(MPI_Bcast(rndm_seed.seed.el, MTRnd::N, MPI_LONG, 0, MPI_COMM_WORLD),
	       "Process::SyncAllSeeds", "Bcast");
  rndm_seed.OldSeed();		// then get my seed!
}
#endif

void Process::Init() {
  rndm_seed.GetCurrent();
}

void Process::Run() {
  TimeUsed start;  start.rec = time_used.rec;
  start.GetTimes();
  if(!RunScript())
    C_Code();			// else run the C_Code
  time_used.GetUsed(start);
}

void Process::Run_gui() {
  Run();
}

LogData& Process::GenLogData(LogData* ld) {
  if(ld == NULL)
    return log_data;
  return *ld;
}

void Process::LoadScript(const char* nm) {
  ScriptBase::LoadScript(nm);
  type = SCRIPT;
}

bool Process::RunScript() {
  if(type == C_CODE) return false;
  return ScriptBase::RunScript();
}

#ifdef TA_GUI
void Process::ControlPanel(float left, float top) {
  if(!taMisc::gui_active) return;
  taiProcess* iep = (taiProcess*)GetTypeDef()->ie;
  if(iep->run_ie == NULL) {
    iep->run_ie = new taiProcessRunBox(GetTypeDef());
  }
  const iColor* bgclr = GetEditColorInherit();
  iep->run_ie->Edit((void*)this, false, bgclr);
  ctrl_panel.GetPanel();
  if((left != 0.0f) && (top != 0.0f)) {
    ctrl_panel.Place(left, top);
  }
}
#endif
void Process::CheckError(TAPtr ck, TypeDef* td) {
  if(ck == NULL) {
    taMisc::Error("NULL object found in check of type:", td->name);
  }
  else {
    taMisc::Error("In process:", name, ", incorrect type of obj:", ck->GetPath(),
		   "of type:", ck->GetTypeDef()->name,
		   "should be at least:", td->name);
  }
}

// cache the types already checked to avoid redundant checking!
static TypeDef* proc_checked_network;
static TypeDef* proc_checked_layer;
static TypeDef* proc_checked_unit;
static TypeDef* proc_checked_con_group;
static TypeDef* proc_checked_con;

void Process::CheckResetCache() {
  proc_checked_network = NULL;
  proc_checked_layer = NULL;
  proc_checked_unit = NULL;
  proc_checked_con_group = NULL;
  proc_checked_con = NULL;
}

bool Process::CheckNetwork() {
  int n_checked = 0;
  if((proc_checked_network != NULL) &&
     proc_checked_network->InheritsFrom(min_network)) n_checked++;
  else proc_checked_network = min_network;
  if((proc_checked_layer != NULL) &&
     proc_checked_layer->InheritsFrom(min_layer)) n_checked++;
  else proc_checked_layer = min_layer;
  if((proc_checked_unit != NULL) &&
     proc_checked_unit->InheritsFrom(min_unit)) n_checked++;
  else proc_checked_unit = min_unit;
  if((proc_checked_con_group != NULL) &&
     proc_checked_con_group->InheritsFrom(min_con_group)) n_checked++;
  else proc_checked_con_group = min_con_group;
  if((proc_checked_con != NULL) &&
     proc_checked_con->InheritsFrom(min_con)) n_checked++;
  else proc_checked_con = min_con;
  if(n_checked == 5)
    return true;		// allready been checked!

  if((network==NULL) || !network->InheritsFrom(min_network)) {
    CheckError(network, min_network);
    return false;
  }
  if(!network->CheckTypes()) return false;
  if(network->CheckBuild()) {
    taMisc::Error("In process:", GetName(), ", Network:",network->GetName(),
		  "Needs the 'Build' command to be run");
    return false;
  }
  if(network->CheckConnect()) {
    taMisc::Error("In process:", GetName(), ", Network:",network->GetName(),
		  "Needs the 'Connect' command to be run");
    return false;
  }

  Layer *layer;
  taLeafItr i;
  FOR_ITR_EL(Layer, layer, network->layers., i) {
    if(!CheckLayer(layer))
      return false;
  }

  // make sure network is fully updated..
  network->UpdtAfterNetMod();
  return true;
}

bool Process::CheckLayer(Layer* ck) {
  if(ck->lesion)	return true; // don't care about lesioned layers
  if((ck == NULL) || !ck->InheritsFrom(min_layer)) {
    CheckError(ck, min_layer);
    return false;
  }
  Unit* unit;
  taLeafItr i;
  FOR_ITR_EL(Unit, unit, ck->units., i) {
    if(!CheckUnit(unit))
      return false;
  }
  return true;
}

bool Process::CheckUnit(Unit* ck) {
  if((ck == NULL) || !ck->InheritsFrom(min_unit)) {
    CheckError(ck, min_unit);
    return false;
  }
  Con_Group* cg;
  int i;
  FOR_ITR_GP(Con_Group, cg, ck->recv., i) {
    if(!CheckConGroup(cg))      	return false;
    if(!cg->CheckOtherIdx_Recv()) 	return false;
  }
  FOR_ITR_GP(Con_Group, cg, ck->send., i) {
    if(!CheckConGroup(cg))      	return false;
    if(!cg->CheckOtherIdx_Send()) 	return false;
  }
  return true;
}
bool Process::CheckConGroup(Con_Group* ck) {
  if((ck==NULL) || !ck->InheritsFrom(min_con_group)) {
    CheckError(ck, min_con_group);
    return false;
  }
  if(!ck->el_typ->InheritsFrom(min_con)) {
    CheckError(ck, min_con);
    return false;
  }
  if(ck->spec.spec == NULL) {
    CheckError(ck->spec.spec, NULL);
  }
  return true;
}

SchedProcess* Process::GetMySchedProc() {
  if(InheritsFrom(TA_SchedProcess))
    return (SchedProcess*)this;
  return GET_MY_OWNER(SchedProcess);
}

SchedProcess* Process::GetMySProcOfType(TypeDef* proc_type) {
  SchedProcess* sp = GetMySchedProc();
  if(sp == NULL) return sp;
  return sp->FindProcOfType(proc_type);
}

TrialProcess* Process::GetMyTrialProc() {
  return (TrialProcess*)GetMySProcOfType(&TA_TrialProcess);
}

EpochProcess* Process::GetMyEpochProc() {
  return (EpochProcess*)GetMySProcOfType(&TA_EpochProcess);
}

Event* Process::GetMyCurEvent() {
  TrialProcess* tp = GetMyTrialProc();
  if(tp == NULL) return NULL;
  return tp->cur_event;
}

Event* Process::GetMyNextEvent() {
  EpochProcess* ep = GetMyEpochProc();
  if(ep == NULL) return NULL;
  return ep->GetMyNextEvent();
}

Event_MGroup* Process::GetMyCurEventGp() {
  SequenceProcess* sp = (SequenceProcess*)GetMySProcOfType(&TA_SequenceProcess);
  if(sp == NULL) {
    EpochProcess* ep = GetMyEpochProc();
    if(ep == NULL) return NULL;
    return ep->enviro_group;
  }
  return sp->cur_event_gp;
}

//////////////////////////
//    Process_Group	//
//////////////////////////

bool Process_Group::nw_itm_def_arg = false;

bool Process_Group::Close_Child(TAPtr obj) {
  SchedProcess* sp = GET_MY_OWNER(SchedProcess);
  if (sp != NULL) {
//obs    winbMisc::DelayedMenuUpdate(sp);
  }
  return Remove(obj);		// otherwise just nuke it
}

Process* Process_Group::FindMakeProc(const char* nm, TypeDef* td, bool& nw_itm) {
  Process* gp = NULL;
  if(nm != NULL)
    gp = (Process*)FindLeafName(nm);
  else if(td != NULL)
    gp = (Process*)FindLeafType(td);
  nw_itm = false;
  if(gp == NULL) {
    gp = (Process*)NewEl(1, td);
    if(nm != NULL)
      gp->name = nm;
    nw_itm = true;
  }
  return gp;
}

//////////////////////////
//    Process_MGroup	//
//////////////////////////

bool Process_MGroup::nw_itm_def_arg = false;

void Process_MGroup::Initialize() {
  SetBaseType(&TA_Process);
//  SetAdapter(new Process_MGroupAdapter(this));
}

/*obs void Process_MGroup::GetAllWinPos() {
  if(!taMisc::gui_active) return;
  taLeafItr li;
  Process* ta;
  FOR_ITR_EL(Process, ta, this->, li) {
    ta->ctrl_panel.GetWinPos();
  }
}

void Process_MGroup::ScriptAllWinPos() {
  if(!taMisc::gui_active) return;
  taLeafItr li;
  Process* ta;
  FOR_ITR_EL(Process, ta, this->, li) {
    ta->ctrl_panel.ScriptWinPos();
  }
} */

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
  if(!obj->InheritsFrom(&TA_SchedProcess)) {
    return taGroup<Process>::DuplicateEl(obj);
  }
  SchedProcess* sp = (SchedProcess*)obj;
  bool rval = taGroup<Process>::DuplicateEl(obj); // first get the new guy
  if(!rval) return rval;
  SchedProcess* np = (SchedProcess*)Peek();
  np->DuplicateElHook(sp);
  taMisc::DelayedMenuUpdate(this);
  return rval;
}

int Process_MGroup::ReplaceEnvPtrs(Environment* old_ev, Environment* new_ev) {
  int nchg = 0;
  taLeafItr li;
  Process* ta;
  FOR_ITR_EL(Process, ta, this->, li) {
    if(ta->environment == old_ev) {
      taBase::SetPointer((TAPtr*)&ta->environment, new_ev);
      if(new_ev == NULL)
	taMisc::Error("*** Note: set environment pointer to NULL in process:",ta->GetPath());
      else
	taMisc::Error("*** Note: replaced environment pointer to:", old_ev->GetName(),
		      "in process:", ta->GetPath(), "with pointer to:", new_ev->GetName());
      nchg++;
    }
    ta->UpdateAfterEdit();	// other things might have changed
  }
  return nchg;
}

int Process_MGroup::ReplaceNetPtrs(Network* old_net, Network* new_net) {
  int nchg = 0;
  taLeafItr li;
  Process* ta;
  FOR_ITR_EL(Process, ta, this->, li) {
    if(ta->network == old_net) {
      taBase::SetPointer((TAPtr*)&(ta->network), new_net);
      if(new_net == NULL)
	taMisc::Error("*** Note: set network pointer to NULL in process:",ta->GetPath());
      else
	taMisc::Error("*** Note: replaced network pointer to:", old_net->GetName(),
		      "in process:", ta->GetPath(), "with pointer to:", new_net->GetName());
      nchg++;
    }
    ta->UpdateAfterEdit();	// other things might have changed
  }
  return nchg;
}

#ifdef TA_GUI
void Process_MGroup::ControlPanel_mc(taiMenuEl* sel) {
/*TODO  if(win_owner == NULL) return;
  if((sel != NULL) && (sel->usr_data != NULL)) {
    Process* itm = (Process*)sel->usr_data;
    itm->ControlPanel();
    if(taiMisc::record_script != NULL) {
      *taiMisc::record_script << itm->GetPath() << "->ControlPanel();" << endl;
    }
  } */
}
#endif
/*obs void Process_MGroup::GenMenu_impl(taiMenu* menu) {
  PDPMGroup::GenMenu_impl(menu);

  taiTypeHier* s_typ_list = new taiTypeHier(menu, &TA_Stat, NULL, NULL, NULL);
  taiTypeHier* p_typ_list = new taiTypeHier(menu, &TA_Process, NULL, NULL, NULL);

  // todo: warning, this menu won't work for over-max-menu!
  mc.member = SLOT(ControlPanel_mc(taiMenuEl*));
  menu->AddSep();
  taiMenu* sub = menu->AddSubMenu("Control Panel");


  // don't put in the groups or the stats processes within menu
  itm_list->SetFlag(taiData::flgNoInGroup | taiData::flgNoList);
  itm_list->GetMenu(sub, &mc);

  delete s_typ_list;
  delete p_typ_list;
} */

Process* Process_MGroup::FindMakeProc(const char* nm, TypeDef* td, bool& nw_itm) {
  Process* gp = NULL;
  if(nm != NULL)
    gp = (Process*)FindLeafName(nm);
  else if(td != NULL)
    gp = (Process*)FindLeafType(td);
  nw_itm = false;
  if(gp == NULL) {
    gp = (Process*)NewEl(1, td);
    if(nm != NULL)
      gp->name = nm;
    nw_itm = true;
  }
  return gp;
}

