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



// PDPShell based on C^c C super script language

#include "pdpshell.h"


//#include "spec.h"
//#include "sched_proc.h"
//#include "pdplog.h"
//#include "net_qt.h"
//#include "pdp_qt.h"

//obs #include "procs_extra.h"
//obs #include "enviro_extra.h"

#include "ta_base.h"
#include "ta_dump.h"
#include "ta_css.h"
#ifdef TA_GUI
  #include "css_qt.h"
  #include "ta_qt.h"
  #include "ta_qtdata.h" // for taiObjChooser
  #include "ta_qtdialog.h"
  #include "ta_qttype_def.h"
  #include "netstru_qtso.h"

  #include <qapplication.h>
  //#include <qevent.h>
  #include <qwidgetlist.h>

#endif

#include <time.h>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif
/*obs
static ivOptionDesc PDP_options[] = {
    { NULL }
};

static ivPropertyData PDP_defs[] = {
  {"PDP++*gui", "sgimotif"},
  {"PDP++*PopupWindow*overlay", "true"},
  {"PDP++*PopupWindow*saveUnder", "on"},
  {"PDP++*TransientWindow*saveUnder", "on"},
  {"PDP++*double_buffered",	"on"},
  {"PDP++*flat",		"#c0c4d3"},
  {"PDP++*background",  	"#70c0d8"},
  {"PDP++*name*flat",		"#70c0d8"},
  {"PDP++*apply_button*flat",	"#c090b0"},
  {"PDP++*FieldEditor*background", "white"},
  {"PDP++*FileChooser*filter", 	"on"},
  {"PDP++*FileChooser.rows", 	"20"},
  {"PDP++*FileChooser.width", 	"300"},
  {"PDP++*taivObjChooser.width", "300"},
  {"PDP++*taivObjChooser.rows",	"20"},
  {"PDP++*PaletteButton*minimumWidth", "72.0"},
  {"PDP++*PushButton*minimumWidth", "72.0"},
  {"PDP++*TaIVButton*SmallWidth", "46.0"},
  {"PDP++*TaIVButton*MediumWidth", "72.0"},
  {"PDP++*TaIVButton*BigWidth", "115.0"},
  {"PDP++*toggleScale",		"1.5"},
#ifndef CYGWIN
  {"PDP++*font",		"*-helvetica-medium-r-*-*-10*"},
  {"PDP++*name*font",		"*-helvetica-medium-r-*-*-10*"},
  {"PDP++*title*font",		"*-helvetica-bold-r-*-*-10*"},
  {"PDP++*small_menu*font",	"*-helvetica-medium-r-*-*-10*"},
  {"PDP++*small_submenu*font",	"*-helvetica-medium-r-*-*-10*"},
  {"PDP++*big_menu*font",	"*-helvetica-medium-r-*-*-12*"},
  {"PDP++*big_submenu*font",	"*-helvetica-medium-r-*-*-12*"},
  {"PDP++*big_menubar*font",	"*-helvetica-bold-r-*-*-14*"},
  {"PDP++*big_italic_menubar*font","*-helvetica-bold-o-*-*-14*"},
#else
  {"PDP++*font",		"*Arial*medium*--12*"},
  {"PDP++*name*font",		"*Arial*medium*--12*"},
  {"PDP++*title*font",		"*Arial*bold*--12*"},
  {"PDP++*small_menu*font",	"*Arial*medium*--12*"},
  {"PDP++*small_submenu*font",	"*Arial*medium*--12*"},
  {"PDP++*big_menu*font",	"*Arial*medium*--12*"},
  {"PDP++*big_submenu*font",	"*Arial*medium*--12*"},
  {"PDP++*big_menubar*font",	"*Arial*bold*--14*"},
  {"PDP++*big_italic_menubar*font","*Arial*italic*--14*"},
  // following are def'd in smf_kit.cpp
  {"PDP++*MenuBar*font", 	"*Arial*bold*--12*"},
  {"PDP++*MenuItem*font", 	"*Arial*bold*--12*"},
  {"PDP++*MenuBar*font", 	"*Arial*bold*--12*"},
  {"PDP++*MenuItem*font", 	"*Arial*bold*--12*"},
  // this sets the scaling of the windows to 1.25 -- much closer to unix pdp sizing
  {"PDP++*mswin_scale",		"1.25"},
#endif
  { NULL }
};
*/


//////////////////////////////////
//	TypeDefault_Group	//
//////////////////////////////////

int TypeDefault_MGroup::Dump_Load_Value(istream& strm, TAPtr par) {
  Reset();			// get rid of any existing defaults before loading
  return taGroup<TypeDefault>::Dump_Load_Value(strm, par);
}
#ifdef TA_GUI
//////////////////////////////////
//	SelectEdit_MGroup	//
//////////////////////////////////

void SelectEdit_MGroup::AutoEdit() {
  taLeafItr i;
  SelectEdit* se;
  FOR_ITR_EL(SelectEdit, se, this->, i) {
    if(se->config.auto_edit)
      se->Edit();
  }
}
#endif

//////////////////////////
//    LayerWizEl	//
//////////////////////////

void LayerWizEl::Initialize() {
  n_units = 10;
}


//////////////////////////
//   Wizard		//
//////////////////////////

void Wizard::Initialize() {
  auto_open = false;
  n_layers = 3;
  layer_cfg.SetBaseType(&TA_LayerWizEl);
  connectivity = FEEDFORWARD;
  event_type = &TA_Event;
}

void Wizard::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(layer_cfg, this);
  layer_cfg.EnforceSize(n_layers);
}

void Wizard::CutLinks() {
  layer_cfg.RemoveAll();
  taNBase::CutLinks();
}

void Wizard::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  layer_cfg.EnforceSize(n_layers);
}
/*TEMP
void Wizard::ThreeLayerNet() {
  n_layers = 3;
  layer_cfg.EnforceSize(n_layers);
  ((LayerWizEl*)layer_cfg[0])->name = "Input";
  ((LayerWizEl*)layer_cfg[0])->io_type = LayerWizEl::INPUT;
  ((LayerWizEl*)layer_cfg[1])->name = "Hidden";
  ((LayerWizEl*)layer_cfg[1])->io_type = LayerWizEl::HIDDEN;
  ((LayerWizEl*)layer_cfg[2])->name = "Output";
  ((LayerWizEl*)layer_cfg[2])->io_type = LayerWizEl::OUTPUT;
}

void Wizard::MultiLayerNet(int n_inputs, int n_hiddens, int n_outputs) {
  n_layers = n_inputs + n_hiddens + n_outputs;
  layer_cfg.EnforceSize(n_layers);
  int i;
  for(i=0;i<n_inputs;i++) {
    ((LayerWizEl*)layer_cfg[i])->name = "Input";
    if(n_inputs > 1) ((LayerWizEl*)layer_cfg[i])->name += "_" + String(i);
    ((LayerWizEl*)layer_cfg[i])->io_type = LayerWizEl::INPUT;
  }
  for(;i<n_inputs + n_hiddens;i++) {
    ((LayerWizEl*)layer_cfg[i])->name = "Hidden";
    if(n_hiddens > 1) ((LayerWizEl*)layer_cfg[i])->name += "_" + String(i-n_inputs);
    ((LayerWizEl*)layer_cfg[i])->io_type = LayerWizEl::HIDDEN;
  }
  for(;i<n_layers;i++) {
    ((LayerWizEl*)layer_cfg[i])->name = "Output";
    if(n_outputs > 1) ((LayerWizEl*)layer_cfg[i])->name += "_" + String(i-(n_inputs+n_hiddens));
    ((LayerWizEl*)layer_cfg[i])->io_type = LayerWizEl::OUTPUT;
  }
}

void Wizard::StdNetwork(Network* net) {
  Project* proj = GET_MY_OWNER(Project);
  if(net == NULL)
    net = pdpMisc::GetNewNetwork(proj);
  if(net == NULL) return;
  layer_cfg.EnforceSize(n_layers);
  net->layers.EnforceSize(n_layers);
  int i;
  int n_hid_layers = 0;
  for(i=0;i<layer_cfg.size;i++) {
    if(((LayerWizEl*)layer_cfg[i])->io_type == LayerWizEl::HIDDEN) n_hid_layers++;
  }
  for(i=0;i<layer_cfg.size;i++) {
    LayerWizEl* el = (LayerWizEl*)layer_cfg[i];
    Layer* lay = (Layer*)net->layers[i];
    if(lay->name != el->name) {
      lay->name = el->name;
      if(el->io_type == LayerWizEl::INPUT) {
        lay->layer_type = Layer::INPUT;
	lay->pos.z = 0;
	if(i > 0) {
	  Layer* prv = (Layer*)net->layers[i-1];
	  lay->pos.x = prv->pos.x + prv->geom.x + 1;
	}
      }
      else if(el->io_type == LayerWizEl::HIDDEN) {
	if(i > 0) {
	  Layer* prv = (Layer*)net->layers[i-1];
	  lay->pos.z = prv->pos.z + 1;
	}
      }
      else {			// OUTPUT
        lay->layer_type = (Layer::LayerType)(Layer::OUTPUT | Layer::TARGET);
	lay->pos.z = n_hid_layers + 1;
	if(i > 0) {
	  LayerWizEl* prvel = (LayerWizEl*)layer_cfg[i-1];
	  if(prvel->io_type == LayerWizEl::OUTPUT) {
	    Layer* prv = (Layer*)net->layers[i-1];
	    lay->pos.x = prv->pos.x + prv->geom.x + 1;
	  }
	}
      }
    }
    lay->n_units = el->n_units;
    lay->UpdateAfterEdit();
  }
  int hid_ctr = 0;
  for(i=0;i<layer_cfg.size;i++) {
    LayerWizEl* el = (LayerWizEl*)layer_cfg[i];
    Layer* lay = (Layer*)net->layers[i];
    if(el->io_type == LayerWizEl::INPUT) continue;
    if(el->io_type == LayerWizEl::HIDDEN) {
      for(int j=0;j<layer_cfg.size;j++) {
	LayerWizEl* fmel = (LayerWizEl*)layer_cfg[j];
	Layer* fm = net->FindLayer(fmel->name);
	if(hid_ctr == 0) {
	  if(fmel->io_type == LayerWizEl::INPUT)
	    net->FindMakePrjn(lay, fm);
	}
	else {
	  if(i==0) continue;
	  LayerWizEl* prvel = (LayerWizEl*)layer_cfg[i-1];
	  Layer* fm = net->FindLayer(prvel->name);
	  net->FindMakePrjn(lay, fm);
	  if(connectivity == BIDIRECTIONAL)
	    net->FindMakePrjn(fm, lay);
	}
      }
      hid_ctr++;
    }
    else {			// OUTPUT
      for(int j=layer_cfg.size-1;j>=0;j--) {
	LayerWizEl* fmel = (LayerWizEl*)layer_cfg[j];
	Layer* fm = net->FindLayer(fmel->name);
	if(fmel->io_type == LayerWizEl::HIDDEN) {
	  net->FindMakePrjn(lay, fm);
	  if(connectivity == BIDIRECTIONAL)
	    net->FindMakePrjn(fm, lay);
	  break;
	}
	else if((n_hid_layers == 0) && (fmel->io_type == LayerWizEl::INPUT)) {
	  net->FindMakePrjn(lay, fm);
	}
      }
    }
  }
  net->Build();
  net->Connect();
  taMisc::DelayedMenuUpdate(net);
}


//////////////////////////////////
// 	Enviro Wizard		//
//////////////////////////////////

void Wizard::StdConduit(NetConduit* cond, Network* net) {
  Project* proj = GET_MY_OWNER(Project);
  if (cond == NULL) {
    cond = pdpMisc::GetNewConduit(proj);
  }
  if (cond == NULL) return;
  if (net == NULL) {
    net = pdpMisc::GetDefNetwork(GET_MY_OWNER(Project));
  }
  if (net == NULL) return; // TODO, maybe create net
  cond->InitFromNetwork(net);
}

void Wizard::StdEnv(Environment* env, int n_events) {
  if(env == NULL) {
    env = pdpMisc::GetNewEnv(GET_MY_OWNER(Project));
  }
  if(env == NULL) return;
  layer_cfg.EnforceSize(n_layers);
  EventSpec* es = env->GetAnEventSpec();
  int n_io_layers = 0;
  int i;
  for(i=0;i<layer_cfg.size;i++) {
    if(((LayerWizEl*)layer_cfg[i])->io_type != LayerWizEl::HIDDEN) n_io_layers++;
  }
  es->patterns.EnforceSize(n_io_layers);
  int pctr = 0;
  for(i=0;i<layer_cfg.size;i++) {
    LayerWizEl* el = (LayerWizEl*)layer_cfg[i];
    if(el->io_type == LayerWizEl::HIDDEN) continue;
    PatternSpec* ps = (PatternSpec*)es->patterns[pctr];
    ps->name = el->name;
    ps->layer_name = el->name;
    ps->to_layer = PatternSpec::LAY_NAME;
    if(el->io_type == LayerWizEl::INPUT)
      ps->type = PatternSpec::INPUT;
    else
      ps->type = PatternSpec::TARGET;
    ps->SetToLayer();
    pctr++;
  }
  if(event_type->InheritsFrom(env->events.el_base))
    env->events.el_typ = event_type;
  if(n_events > 0) {
    env->events.EnforceSize(n_events);
  }
  taMisc::DelayedMenuUpdate(env);
}

void Wizard::UpdateEnvFmNet(Environment* env) {
  if(env == NULL) return;
  env->UpdateAllEventSpecs();
}

void Wizard::SequenceEvents(Environment* env, int n_seqs, int events_per_seq) {
  if(env == NULL) {
    taMisc::Error("SequenceEvents: must have basic constructed environment first");
    return;
  }
  if(env->events.size > 0) {
    env->events.EnforceSize(0);	// no top-level dudes
  }
  if(event_type->InheritsFrom(env->events.el_base))
    env->events.el_typ = event_type;
  env->events.gp.EnforceSize(n_seqs);
  int i;
  for(i=0;i<n_seqs;i++) {
    Event_MGroup* egp = (Event_MGroup*)env->events.gp[i];
    egp->el_typ = event_type;
    egp->EnforceSize(events_per_seq);
  }
  taMisc::DelayedMenuUpdate(env);
}

void Wizard::TimeSeqEvents(TimeEnvironment* env, int n_seqs, int events_per_seq, float start_time, float time_inc) {
  if(env == NULL) {
    taMisc::Error("TimeSeqEvents: must have basic constructed environment first");
    return;
  }
  if(env->events.size > 0) {
    env->events.EnforceSize(0);
  }
  if(event_type->InheritsFrom(env->events.el_base))
    env->events.el_typ = event_type;
  env->events.gp.EnforceSize(n_seqs);
  int i;
  for(i=0;i<n_seqs;i++) {
    TimeEvent_MGroup* egp = (TimeEvent_MGroup*)env->events.gp[i];
    egp->EnforceSize(events_per_seq);
    if(!egp->InheritsFrom(TA_TimeEvent_MGroup)) continue;
    egp->RegularlySpacedTimes(start_time, time_inc);
  }
  taMisc::DelayedMenuUpdate(env);
}

//////////////////////////////////
// 	Proc Wizard		//
//////////////////////////////////

void Wizard::StdProcs() {
  Project* proj = GET_MY_OWNER(Project);
  BatchProcess* batch = (BatchProcess*)pdpMisc::FindMakeProc(proj, "Batch_0", &TA_BatchProcess);
  batch->CreateSubProcs();
#ifdef TA_GUI
  batch->ControlPanel();
#endif
}

void Wizard::NetAutoSave(SchedProcess* proc, bool just_weights) {
  if(proc == NULL) return;
  if(just_weights)
    proc->final_procs.FindMakeProc(NULL, &TA_SaveWtsProc);
  else
    proc->final_procs.FindMakeProc(NULL, &TA_SaveNetsProc);
  taMisc::DelayedMenuUpdate(proc);
}

EpochProcess* Wizard::AutoTestProc(SchedProcess* tproc, Environment* tenv) {
  if((tproc == NULL) || (tenv == NULL)) return NULL;
  Project* proj = GET_MY_OWNER(Project);
  EpochProcess* cve = (EpochProcess*)pdpMisc::FindMakeProc(proj, "TestEpoch", &TA_EpochProcess);
  cve->CreateSubProcs();
  cve->SetEnv(tenv);
  cve->wt_update = EpochProcess::TEST;
  cve->order = EpochProcess::SEQUENTIAL;
  tproc->loop_procs.Link(cve);
  GetStatsFromProc(cve, tproc, SchedProcess::LOOP_STATS, Aggregate::LAST);
  EpochProcess* trepc = tproc->GetMyEpochProc();
  if(trepc != NULL)
    GetStatsFromProc(cve, trepc, SchedProcess::FINAL_STATS, Aggregate::LAST);
  AddCountersToTest(cve, tproc);
  return cve;
}

EpochProcess* Wizard::CrossValidation(SchedProcess* tproc, Environment* tenv) {
  if((tproc == NULL) || (tenv == NULL)) return NULL;
  EpochProcess* cve = AutoTestProc(tproc, tenv);
  Stat* sst;
  taLeafItr i;
  FOR_ITR_EL(Stat, sst, tproc->loop_stats.,i) {
    if(!sst->InheritsFrom(&TA_SE_Stat) || (sst->time_agg.from == NULL)) continue;
    Stat* fst = sst->time_agg.from;
    if(fst->GetMySchedProc() == cve) {
      ((SE_Stat*)sst)->se.stopcrit.flag = true;	// stop on this one
    }
  }
  return cve;
}

void Wizard::ToSequenceEvents(SchedProcess* proc) {
  if(proc == NULL) return;
  SchedProcess* epc = proc->GetMyEpochProc();
  if(epc == NULL) {
    taMisc::Error("Error: no epoch process found in hierarchy!");
    return;
  }
  if(epc->InheritsFrom(&TA_SequenceEpoch)) return; // already done!
  epc->AddSubProc(&TA_SequenceProcess);
  epc->ChangeMyType(&TA_SequenceEpoch);
}

void Wizard::NoSequenceEvents(SchedProcess* proc) {
  if(proc == NULL) return;
  SchedProcess* epc = proc->GetMyEpochProc();
  if(epc == NULL) {
    taMisc::Error("Error: no epoch process found in hierarchy!");
    return;
  }
  if(!epc->InheritsFrom(&TA_SequenceEpoch)) return; // already done!
  epc->RemoveSubProc();
  epc->ChangeMyType(&TA_EpochProcess);
}

//////////////////////////////////
// 	Stats Wizard		//
//////////////////////////////////

MonitorStat* Wizard::RecordLayerValues(SchedProcess* proc, SchedProcess::StatLoc loc, Layer* lay, const char* var) {
  if(proc == NULL) return NULL;
  Stat_Group* tgp = proc->GetStatGroup(&TA_MonitorStat, loc);
  MonitorStat* mst = tgp->FindMakeMonitor(lay, var);
  taMisc::DelayedMenuUpdate(proc);
  return mst;
}

CopyToEnvStat* Wizard::SaveValuesInDataEnv(MonitorStat* stat) {
  if(stat == NULL) return NULL;
  Project* proj = GET_MY_OWNER(Project);
  Stat_Group* sgp = (Stat_Group*)stat->owner;
  int idx = sgp->Find(stat);
  CopyToEnvStat* cte;
  if(sgp->size <= idx+1) {
    cte = (CopyToEnvStat*)sgp->NewEl(1, &TA_CopyToEnvStat);
  }
  else {
    Stat* nst = (Stat*)sgp->FastEl(idx+1);
    if(nst->InheritsFrom(&TA_CopyToEnvStat))
      cte = (CopyToEnvStat*)nst;
    else {
      cte = (CopyToEnvStat*)sgp->NewEl(1, &TA_CopyToEnvStat);
      sgp->MoveAfter(stat, cte);
    }
  }
  taBase::SetPointer((TAPtr*)&cte->stat, stat);
  if(cte->data_env == NULL) {
    Environment* env = pdpMisc::GetNewEnv(proj, &TA_Environment);
    taBase::SetPointer((TAPtr*)&cte->data_env, env);
  }
  cte->InitEnv();
  taMisc::DelayedMenuUpdate(stat->GetMySchedProc());
  return cte;
}

DispDataEnvProc* Wizard::AutoAnalyzeDataEnv(Environment* data_env, int pat_no,
					    DispDataEnvProc::DispType disp_type,
					    SchedProcess* proc, SchedProcess::ProcLoc loc) {
  if((proc == NULL) || (data_env == NULL)) return NULL;
  Process_Group* tgp = proc->GetProcGroup(loc);
  DispDataEnvProc* ddep = NULL;
  taLeafItr i;
  Process* pr;
  FOR_ITR_EL(Process, pr, tgp->, i) {
    if(!pr->InheritsFrom(TA_DispDataEnvProc)) continue;
    DispDataEnvProc* dp = (DispDataEnvProc*)pr;
    if((dp->data_env == data_env) && (dp->disp_type == disp_type) && (dp->pat_no == pat_no)) {
      ddep = dp;
      break;
    }
  }
  if(ddep == NULL) {
    ddep = (DispDataEnvProc*)tgp->NewEl(1, &TA_DispDataEnvProc);
  }
  taBase::SetPointer((TAPtr*)&ddep->data_env, data_env);
  ddep->pat_no = pat_no;
  ddep->disp_type = disp_type;
  ddep->C_Code();		// run it!
  taMisc::DelayedMenuUpdate(proc);
  return ddep;
}

DispDataEnvProc* Wizard::AnalyzeNetLayer(SchedProcess* rec_proc, SchedProcess::StatLoc rec_loc,
			     Layer* lay, const char* var,
			     DispDataEnvProc::DispType disp_type, SchedProcess* anal_proc,
			     SchedProcess::ProcLoc anal_loc) {
  MonitorStat* mst = RecordLayerValues(rec_proc, rec_loc, lay, var);
  if(mst == NULL) return NULL;
  CopyToEnvStat* cte = SaveValuesInDataEnv(mst);
  if(cte == NULL) return NULL;
  DispDataEnvProc* ddep = AutoAnalyzeDataEnv(cte->data_env, 0, disp_type, anal_proc, anal_loc);
  return ddep;
}

UnitActRFStat* Wizard::ActBasedReceptiveField(SchedProcess* rec_proc, SchedProcess::StatLoc rec_loc,
					      Layer* recv_layer, Layer* send_layer, Layer* send2_layer,
					      SchedProcess* disp_proc, SchedProcess::ProcLoc disp_loc)
{
  if((rec_proc == NULL) || (disp_proc == NULL)) return NULL;
  Project* proj = GET_MY_OWNER(Project);
  Stat_Group* tgp = rec_proc->GetStatGroup(&TA_UnitActRFStat, rec_loc);
  UnitActRFStat* rfs = (UnitActRFStat*)tgp->FindMakeStat(&TA_UnitActRFStat);
  taBase::SetPointer((TAPtr*)&rfs->layer, recv_layer);
  if(send_layer != NULL)
    rfs->rf_layers.LinkUnique(send_layer);
  if(send2_layer != NULL)
    rfs->rf_layers.LinkUnique(send2_layer);
  if(rfs->data_env == NULL) {
    Environment* env = pdpMisc::GetNewEnv(proj, &TA_Environment);
    taBase::SetPointer((TAPtr*)&rfs->data_env, env);
  }
  rfs->InitRFVals();
  Process_Group* reset_gp = disp_proc->GetProcGroup(SchedProcess::INIT_PROCS);
  UnitActRFStatResetProc* rrf = (UnitActRFStatResetProc*)reset_gp->FindMakeProc(NULL, &TA_UnitActRFStatResetProc);
  taBase::SetPointer((TAPtr*)&rrf->unit_act_rf_stat, rfs);
  AutoAnalyzeDataEnv(rfs->data_env, 0, DispDataEnvProc::RAW_DATA_GRID, disp_proc, disp_loc);
  if(send2_layer != NULL)
    AutoAnalyzeDataEnv(rfs->data_env, 1, DispDataEnvProc::RAW_DATA_GRID, disp_proc, disp_loc);
  taMisc::DelayedMenuUpdate(rec_proc);
  return rfs;
}

DispNetWeightsProc* Wizard::DisplayNetWeights(Layer* recv_layer, Layer* send_layer,
      SchedProcess* proc, SchedProcess::ProcLoc loc) {
  if((proc == NULL) || (recv_layer == NULL) || (send_layer == NULL)) return NULL;
  Process_Group* tgp = proc->GetProcGroup(loc);
  DispNetWeightsProc* dnw = (DispNetWeightsProc*)tgp->FindMakeProc(NULL, &TA_DispNetWeightsProc);
  dnw->recv_layer_nm = recv_layer->name;
  dnw->send_layer_nm = send_layer->name;
  dnw->C_Code();
  taMisc::DelayedMenuUpdate(proc);
  return dnw;
}

void Wizard::StopOnActThresh(SchedProcess* proc, Layer* lay, float thresh) {
  if((proc == NULL) || (lay == NULL)) return;
  Stat_Group* sgp = (Stat_Group*)&(proc->loop_stats);
  ActThreshRTStat* rts = (ActThreshRTStat*)sgp->FindMakeStat(&TA_ActThreshRTStat);
  taBase::SetPointer((TAPtr*)&rts->layer, lay);
  rts->act_thresh = thresh;
  rts->max_act.stopcrit.flag = true;
  rts->UpdateAfterEdit();
  taMisc::DelayedMenuUpdate(proc);
}

void Wizard::AddCountersToTest(SchedProcess* te_proc, SchedProcess* tr_proc) {
  if((te_proc == NULL) || (tr_proc == NULL)) return;
  Stat_Group* sgp = (Stat_Group*)&(te_proc->final_stats);
  ProcCounterStat* st = (ProcCounterStat*)sgp->FindMakeStat(&TA_ProcCounterStat);
  taBase::SetPointer((TAPtr*)&st->proc, tr_proc);
  st->UpdateAfterEdit();
  taMisc::DelayedMenuUpdate(te_proc);
}

void Wizard::GetStatsFromProc(SchedProcess* sproc, SchedProcess* tproc, SchedProcess::StatLoc tloc, Aggregate::Operator agg_op) {
  if((sproc == NULL) || (tproc == NULL)) return;
  Stat* sst;
  taLeafItr i;
  FOR_ITR_EL(Stat, sst, sproc->loop_stats.,i) {
    Stat_Group* tgp = tproc->GetStatGroup(sst->GetTypeDef(), tloc);
    Stat* trst = tgp->FindAggregator(sst, agg_op);
    if(trst == NULL)
      tproc->MakeAggregator(sst, tloc, agg_op);
  }
  FOR_ITR_EL(Stat, sst, sproc->final_stats.,i) {
    Stat_Group* tgp = tproc->GetStatGroup(sst->GetTypeDef(), tloc);
    Stat* trst = tgp->FindAggregator(sst, agg_op);
    if(trst == NULL)
      tproc->MakeAggregator(sst, tloc, agg_op);
  }
  taMisc::DelayedMenuUpdate(tproc);
}

TimeCounterStat* Wizard::AddTimeCounter(SchedProcess* inc_proc, SchedProcess::StatLoc loc, SchedProcess* reset_proc) {
  if((inc_proc == NULL) || (reset_proc == NULL)) return NULL;
  Stat_Group* igp = inc_proc->GetStatGroup(&TA_TimeCounterStat, loc);
  TimeCounterStat* st = (TimeCounterStat*)igp->FindMakeStat(&TA_TimeCounterStat);
  Process_Group* rgp = (Process_Group*)&(reset_proc->init_procs);
  TimeCounterStatResetProc* rproc =
    (TimeCounterStatResetProc*)rgp->FindMakeProc(NULL, &TA_TimeCounterStatResetProc);
  taBase::SetPointer((TAPtr*)&rproc->time_ctr_stat, st);
  taMisc::DelayedMenuUpdate(inc_proc);
  taMisc::DelayedMenuUpdate(reset_proc);
  return st;
}


void Wizard::LogProcess(SchedProcess* proc, TypeDef* log_type) {
  if((proc == NULL) || (log_type == NULL)) return;
  Project* proj = GET_MY_OWNER(Project);
  String nm = proc->name + "_" + log_type->name;
  PDPLog* plog = pdpMisc::FindMakeLog(proj, nm, log_type);
  if(plog->log_proc.size == 0)
    plog->AddUpdater(proc);
  taMisc::DelayedMenuUpdate(proj);
}

void Wizard::StdLogs(SchedProcess* proc) {
  if(proc == NULL) return;
  SchedProcess* epc = proc->GetMyEpochProc();
  if(epc != NULL) {
    LogProcess(epc, &TA_GraphLog);

    SchedProcess* bat = epc->FindSuperProc(&TA_BatchProcess);
    if(bat != NULL) {
      LogProcess(bat, &TA_TextLog);
    }
  }
  SchedProcess* trl = proc->GetMyTrialProc();
  if(trl != NULL) {
    LogProcess(trl, &TA_TextLog);
  }
}
*/
//////////////////////////////////
// 	Wizard_MGroup		//
//////////////////////////////////

void Wizard_MGroup::AutoEdit() {
  Wizard* wz;
  taLeafItr i;
  FOR_ITR_EL(Wizard, wz, this->, i) {
    if (wz->auto_open)
      wz->Edit();
  }
}


///////////////////////////
//	Project		//
//////////////////////////
#ifdef TA_GUI
class SimLogEditDialog : public taiEditDataHost {
public:
  bool	ShowMember(MemberDef* md) {
    bool rval = (md->ShowMember(show) && (md->im != NULL));
    if(!rval) return rval;
    if(!(md->name.contains("desc") || (md->name == "use_sim_log") || (md->name == "save_rmv_units")
	 || (md->name == "prev_file_nm"))) return false;
    return true;
  }

  override void	Constr_Methods_impl() { }	// suppress methods

  SimLogEditDialog(void* base, TypeDef* tp, bool read_only_,
  	bool modal_) : taiEditDataHost(base, tp, read_only_, modal_) { };
};
#endif
bool Project::nw_itm_def_arg = false;


void Project::Initialize() {
  defaults.SetBaseType(&TA_TypeDefault);
  wizards.SetBaseType(&TA_Wizard);
  wizards.el_typ = pdpMisc::def_wizard;
  specs.SetBaseType(&TA_BaseSpec);
  networks.SetBaseType(&TA_Network);
  net_writers.SetBaseType(&TA_NetWriter);
  net_readers.SetBaseType(&TA_NetReader);
  environments.SetBaseType(&TA_Environment);
  processes.SetBaseType(&TA_SchedProcess);
  logs.SetBaseType(&TA_TextLog);
  scripts.SetBaseType(&TA_Script);
#ifdef TA_GUI
  edits.SetBaseType(&TA_SelectEdit);
  //TODO: viewer
#endif
  save_rmv_units = false;
  use_sim_log = true;

//TODO  editor = NULL;
  view_colors.SetBaseType(&TA_RGBA);
  the_colors.SetBaseType(&TA_TAColor);
  mnu_updating = false;
  deleting = false;
}

void Project::InitLinks() {
  taBase::Own(defaults, this);
  taBase::Own(wizards, this);
  taBase::Own(specs, this);
  taBase::Own(networks, this);
  taBase::Own(net_writers, this);
  taBase::Own(net_readers, this);
  taBase::Own(data, this);
  taBase::Own(environments, this);
  taBase::Own(processes, this);
  taBase::Own(logs, this);
  taBase::Own(scripts, this);
#ifdef TA_GUI
  taBase::Own(edits, this);
  taBase::Own(viewers, this);
#ifdef DEBUG
  taBase::Own(test_objs, this);	// just for testing, for any kind of objs
#endif
#endif
  taBase::Own(the_colors, this);
  taBase::Own(view_colors, this);

  if(taMisc::is_loading)
    view_colors.Reset();		// kill existing colors
  else
    GetDefaultColors();

  LoadDefaults();

  if(!taMisc::is_loading) {
    MakeDefaultWiz(true);	// make default and edit it
  }
  else {
    MakeDefaultWiz(false);	// make default and don't edit it
  }

  inherited::InitLinks();
}

void Project::CutLinks() {
  deleting = true;
  SchedProcess* sp;
  taLeafItr li;
/*TODO: equiv for Programs  FOR_ITR_EL(SchedProcess, sp, processes., li) {
    if(sp->running)
      sp->Stop();
  } */
//TODO  if(editor != NULL) { delete editor; editor = NULL; }
  inherited::CutLinks();	// close windows, etc
#ifdef TA_GUI
#ifdef DEBUG
  test_objs.CutLinks();	// just for testing, for any kind of objs
#endif
  viewers.CutLinks();
  edits.CutLinks();
#endif
  scripts.CutLinks();
  logs.CutLinks();
  processes.CutLinks();
  environments.CutLinks();
  data.CutLinks();
  net_readers.CutLinks();
  net_writers.CutLinks();
  networks.CutLinks();
  specs.CutLinks();
  defaults.CutLinks();

  view_colors.CutLinks();
  the_colors.CutLinks();
}

void Project::Copy_(const Project& cp) {
  defaults = cp.defaults;
  specs = cp.specs;
  networks = cp.networks;
  net_writers = cp.net_writers;
  net_readers = cp.net_readers;
  data = cp.data;
  environments = cp.environments;
  processes = cp.processes;
  logs = cp.logs;
  scripts = cp.scripts;
#ifdef TA_GUI
  edits = cp.edits;
  viewers = cp.viewers;
#endif
  view_colors = cp.view_colors;
  the_colors = cp.the_colors;
}

void Project::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();

  UpdateColors();
}

BaseSpec_MGroup* Project::FindMakeSpecGp(const char* nm, bool& nw_itm) {
  BaseSpec_MGroup* gp = (BaseSpec_MGroup*)specs.gp.FindName(nm);
  nw_itm = false;
  if(gp == NULL) {
    gp = (BaseSpec_MGroup*)specs.gp.New(1);
    gp->name = nm;
    nw_itm = true;
  }
  return gp;
}

void Project::GetDefaultColors() {
  view_colors.EnforceSize(COLOR_COUNT);
  view_colors[TEXT]->name = "black";
  view_colors[TEXT]->desc = "Text";
  view_colors[BACKGROUND]->r = .752941f;
  view_colors[BACKGROUND]->g = .768627f;
  view_colors[BACKGROUND]->b = .827451f;
  view_colors[BACKGROUND]->desc = "Background";
  view_colors[NETWORK]->name = "VioletRed1";
  view_colors[NETWORK]->desc = "Network";
  view_colors[ENVIRONMENT]->name = "DarkOliveGreen3";
  view_colors[ENVIRONMENT]->desc = "Env";
  view_colors[SCHED_PROC]->name = "yellow";
  view_colors[SCHED_PROC]->desc = "SchedProc";
  view_colors[STAT_GROUP]->name = "LightSteelBlue2";
  view_colors[STAT_GROUP]->desc = "StatGroup";
  view_colors[SUBPROC_GROUP]->name = "wheat";
  view_colors[SUBPROC_GROUP]->desc = "SubProcGroup";
  view_colors[STAT_PROC]->name = "SlateBlue1";
  view_colors[STAT_PROC]->desc = "Stat";
  view_colors[OTHER_PROC]->name = "gold";
  view_colors[OTHER_PROC]->desc = "Process";
  view_colors[PDPLOG]->name = "burlywood2";
  view_colors[PDPLOG]->desc = "Log";
  view_colors[STAT_AGG]->name = "aquamarine";
  view_colors[STAT_AGG]->desc = "Agg Highlite";
  view_colors[GEN_GROUP]->name = "grey64";
  view_colors[GEN_GROUP]->desc = "Group";
  view_colors[INACTIVE]->name = "grey75";
  view_colors[INACTIVE]->desc = "Inactive";
  view_colors[STOP_CRIT]->name = "red";
  view_colors[STOP_CRIT]->desc = "Stopping Stat";
  view_colors[AGG_STAT]->name = "SlateBlue3";
  view_colors[AGG_STAT]->desc = "Agging Stat";

  view_colors[CON_SPEC]->name = "SpringGreen";
  view_colors[CON_SPEC]->desc = "ConSpec";
  view_colors[UNIT_SPEC]->name = "violet";
  view_colors[UNIT_SPEC]->desc = "UnitSpec";
  view_colors[PRJN_SPEC]->name = "orange";
  view_colors[PRJN_SPEC]->desc = "PrjnSpec";
  view_colors[LAYER_SPEC]->name = "MediumPurple1";
  view_colors[LAYER_SPEC]->desc = "LayerSpec";
  view_colors[WIZARD]->name = "azure";
  view_colors[WIZARD]->desc = "Wizard";

  UpdateColors();
}

const iColor* Project::GetObjColor(TypeDef* td) {
#ifdef TA_GUI
  if(view_colors.size != COLOR_COUNT) {
    view_colors.Reset();
    GetDefaultColors();
  }
  if(the_colors.size != COLOR_COUNT)
    UpdateColors();
  if(td->InheritsFrom(TA_ConSpec))
    return the_colors.FastEl(Project::CON_SPEC)->color();
  else if(td->InheritsFrom(TA_UnitSpec))
    return the_colors.FastEl(Project::UNIT_SPEC)->color();
  else if(td->InheritsFrom(TA_ProjectionSpec))
    return the_colors.FastEl(Project::PRJN_SPEC)->color();
  else if(td->InheritsFrom(TA_LayerSpec))
    return the_colors.FastEl(Project::LAYER_SPEC)->color();
  else if(td->InheritsFrom(TA_Network))
    return the_colors.FastEl(Project::NETWORK)->color();
  else if(td->InheritsFrom(TA_Environment))
    return the_colors.FastEl(Project::ENVIRONMENT)->color();
  else if(td->InheritsFrom(TA_PDPLog))
    return the_colors.FastEl(Project::PDPLOG)->color();
  else if(td->InheritsFrom(TA_SchedProcess))
    return the_colors.FastEl(Project::SCHED_PROC)->color();
  else if(td->InheritsFrom(TA_Stat))
    return the_colors.FastEl(Project::STAT_PROC)->color();
  else if(td->InheritsFrom(TA_Process))
    return the_colors.FastEl(Project::OTHER_PROC)->color();
  else if(td->InheritsFrom(TA_taGroup_impl))
    return the_colors.FastEl(Project::GEN_GROUP)->color();
  else if(td->InheritsFrom(TA_Wizard))
    return the_colors.FastEl(Project::WIZARD)->color();
#endif
  return NULL;
}

const iColor* Project::GetObjColor(ViewColors vc) {
#ifdef TA_GUI
  if(view_colors.size != COLOR_COUNT) {
    view_colors.Reset();
    GetDefaultColors();
  }
  if(the_colors.size != COLOR_COUNT)
    UpdateColors();
  TAColor* tac = the_colors.SafeEl(vc);
  if (tac != NULL)
    return tac->color();
#endif
  return NULL;
}

int Project::Load(istream& strm, TAPtr par) {
  int rval = inherited::Load(strm, par); // load-em-up
  if (rval) {	 // don't do this as a dump_load_value cuz we need an updateafteredit..
    if (taMisc::gui_active) {
      pdpMisc::post_load_opr.Link(&wizards);
      pdpMisc::post_load_opr.Link(&scripts);
#ifdef TA_GUI
      pdpMisc::post_load_opr.Link(&edits);
#endif
    } else {
      wizards.AutoEdit();
      scripts.AutoRun();
#ifdef TA_GUI
      edits.AutoEdit();
#endif
    }
  }
  return rval;
}

void Project::LoadDefaults() {
  bool loading = taMisc::is_loading; // cache is loading flag
  fstream def;
  if(!pdpMisc::root->default_file.empty() && cssProgSpace::GetFile(def, pdpMisc::root->default_file)) {
    defaults.Load(def);
    def.close(); def.clear();
    //    cerr << "Successfully loaded default file: " << pdpMisc::root->default_file << "\n";
  }
  else if(pdpMisc::defaults_str != NULL) {
    std::string def_str = pdpMisc::defaults_str;
    std::istringstream sdef(def_str);
    sdef.seekg(0, ios::beg);
    defaults.Load(sdef);
    if(taMisc::dmem_proc == 0)
      cerr << "Using standard pre-compiled defaults file\n";
  }
  else {
    taMisc::Error("Warning: no default file was available - created objects will not automatically be of correct type");
  }
  taMisc::is_loading = loading;
}

void Project::MakeDefaultWiz(bool auto_opn) {
  Wizard* wiz = (Wizard*)wizards.New(1, pdpMisc::def_wizard);
  if(auto_opn) {
    wiz->auto_open = true;
//TEMP    wiz->ThreeLayerNet();
    wiz->Edit();
  }
}

#ifdef TA_GUI
pdpDataViewer* Project::NewViewer() {
  pdpDataViewer* vwr = NULL;
  vwr = pdpDataViewer::New(this);
  viewers.Add(vwr);
  return vwr;
}
#endif
#ifdef TA_GUI
void Project::OpenNetworkViewer(Network* net) {
  if (!net) return;

  // network objects
  pdpDataViewer* vwr = NewViewer();
  NetView* nv = NetView::New(vwr, net);
  nv->BuildAll();

  vwr->ViewWindow();
}
#endif
int Project::SaveAs(ostream& strm, TAPtr par, int indent) {
#ifdef TA_GUI
  if (use_sim_log) {
    UpdateSimLog();
  }
#endif
  return inherited::SaveAs(strm, par, indent);
}

int Project::Save(ostream& strm, TAPtr par, int indent) {
  if(taMisc::gui_active) {
    taMisc::Busy();
  }
  taMisc::is_saving = true;
  if (save_rmv_units) {
    for(int i=0; i< networks.size; i++) {
      Network* net = (Network*)networks[i];
      net->RemoveUnits();
    }
  }
  dumpMisc::path_tokens.Reset();
  strm << "// ta_Dump File v1.0\n";   // be sure to check version with Load
  int rval = Dump_Save_Path(strm, par, indent);
  if(rval == false) {
    if(taMisc::gui_active)  taMisc::DoneBusy();
    return rval;
  }
  strm << " {\n";

  // save defaults within project save as first item
  defaults.Dump_Save_Path(strm, par, indent+1);
  strm << " { ";
  if(defaults.Dump_Save_PathR(strm, par, indent+2))
    taMisc::indent(strm, indent+1, 1);
  strm << "  };\n";
  defaults.Dump_Save_impl(strm, par, indent+1);
  defaults.Dump_SaveR(strm, par, indent+1);

  if(Dump_Save_PathR(strm, par, indent+1))
    taMisc::indent(strm, indent, 1);
  strm << "};\n";
  Dump_Save_impl(strm, par, indent);
  Dump_SaveR(strm, par, indent);
  taMisc::is_saving = false;
  dumpMisc::path_tokens.Reset();
  if(taMisc::gui_active)  taMisc::DoneBusy();
  return true;
}

bool Project::SetFileName(const String& val) {
  prev_file_nm = GetFileName();
  return inherited::SetFileName(val);
}

void Project::UpdateColors() {
  if(view_colors.size != COLOR_COUNT) {
    view_colors.Reset();
    GetDefaultColors();
  }
  the_colors.EnforceSize(COLOR_COUNT);
  if(taMisc::gui_active) {
    int i;
    for(i=0;i<COLOR_COUNT;i++) {
      view_colors[i]->UpdateAfterEdit();
      the_colors[i]->SetColor(view_colors[i]);
    }
  }
}
#ifdef TA_GUI
void Project::UpdateSimLog() {
  SimLogEditDialog* dlg = new SimLogEditDialog(this, GetTypeDef(), false, true);
  dlg->Constr("Update simulation log (SimLog) for this project, storing the name of the project and the description as entered here.  Click off use_sim_log if you are not using this feature");
  if(dlg->Edit(true) && use_sim_log) {
    time_t tmp = time(NULL);
    String tstamp = ctime(&tmp);
    tstamp = tstamp.before('\n');

    String user;
    char* user_c = getenv("USER");
    if(user_c != NULL) user = user_c;
    char* host_c = getenv("HOSTNAME");
    if(host_c != NULL) user += String("@") + String(host_c);

    fstream fh;
    fh.open("SimLog", ios::out | ios::app);
    fh << endl << endl;
    fh << file_name << " <- " << prev_file_nm << "\t" << tstamp << "\t" << user << endl;
    if(!desc1.empty()) fh << "\t" << desc1 << endl;
    if(!desc2.empty()) fh << "\t" << desc2 << endl;
    if(!desc3.empty()) fh << "\t" << desc3 << endl;
    if(!desc4.empty()) fh << "\t" << desc4 << endl;
    fh.close(); fh.clear();
  }
}
#endif
//////////////////////////
//   Project_MGroup	//
//////////////////////////

int Project_MGroup::Load(istream& strm, TAPtr par) {
/*obs  if ((ta_file != NULL) && !ta_file->dir.empty() && (ta_file->dir != ".")) {
    // change directories to where the project was loaded!
    ta_file->GetDir();
    chdir(ta_file->dir);
    String fnm = ta_file->fname.after('/',-1);
    ta_file->dir = "";
    ta_file->fname = fnm;
  } */
  int rval = taGroup<Project>::Load(strm, par);
  if (rval) {
    Project* p;
    taLeafItr i;
    FOR_ITR_EL(Project, p, this->, i) {
      if (taMisc::gui_active) {
	pdpMisc::post_load_opr.Link(&(p->wizards));
	pdpMisc::post_load_opr.Link(&(p->scripts));
#ifdef TA_GUI
	pdpMisc::post_load_opr.Link(&(p->edits));
#endif
      } else {
	p->wizards.AutoEdit();
	p->scripts.AutoRun();
#ifdef TA_GUI
	p->edits.AutoEdit();
#endif
      }
    }
  }
  return rval;
}


//////////////////////////////////////////
//	PDPRoot: structural root	//
//////////////////////////////////////////

void PDPRoot::Initialize() {
  version_no = taMisc::version_no;
  SetName("root");
  projects.SetBaseType(&TA_Project);
  projects.SetName("projects");
  colorspecs.SetBaseType(&TA_ColorScaleSpec);
  projects.colorspecs = &colorspecs;
}

void PDPRoot::Destroy() {
  bool we_are_root = (tabMisc::root == this); // 'true' if we are the one and only root app object
  if (taMisc::app == (IApp*)this) taMisc::app = NULL;
  if (we_are_root) {
    tabMisc::root = NULL; //TODO: maybe for cleanness we should do a SetPointer thingy, since we set it that way...
#ifdef DMEM_COMPILE
    if(taMisc::dmem_nprocs > 1) {
      taMisc::RecordScript(".Quit();\n");
    } else
#endif
    {
      projects.RemoveAll();
      cssMisc::Top->ExitShell();	// just exit from top-level shell
    }
  } else {
    projects.RemoveAll();
  }
  colorspecs.RemoveAll();
  CutLinks();
#ifdef TA_GUI
  if (we_are_root && taMisc::gui_active) {
    cssiSession::RunPending();
      if(cssiSession::WaitProc != NULL) 	// then its safe to do some work
	(*cssiSession::WaitProc)();
    cssiSession::Quit();	    // then quit..
  }
#endif
}

void PDPRoot::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(projects, this);
  taBase::Own(colorspecs, this);
}

void PDPRoot::CutLinks() {
  projects.CutLinks();
  colorspecs.CutLinks();
  taNBase::CutLinks();
}

void PDPRoot::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
}
#ifdef TA_GUI
TAPtr PDPRoot::Browse(const char* init_path) {
  if(!taMisc::gui_active) return NULL;

  TAPtr iob = this;
  if(init_path != NULL) {
    String ip = init_path;
    iob = FindFromPath(ip);
    if(iob == NULL) iob = this;
  }

  taiObjChooser* chs = new taiObjChooser((TAPtr)iob, "Browse for Object", false, NULL);
  chs->Choose();
  TAPtr retv = chs->sel_obj();
  delete chs;
  return retv;
}
#endif
void PDPRoot::Info() {
  String info = "PDP Info\n";
  info += "This is the PDP++ software package, version: ";
  info += version_no + "\n\n";
  info += "Mailing List:       http://www.cnbc.cmu.edu/PDP++/pdp-discuss.html\n";
  info += "Bug Reports:        pdp++bugreport@cnbc.cmu.edu\n";
  info += "WWW Page:           http://www.cnbc.cmu.edu/PDP++/PDP++.html\n";
  info += "Anonymous FTP Site: ftp://grey.colorado.edu/pub/oreilly/pdp++/\n";
  info += "                    or ftp://cnbc.cmu.edu/pub/pdp++/\n";
  info += "\n\n";

  info += "Copyright (C) 1995-2004 Randall C. O'Reilly, Chadley K. Dawson,\n\
                    James L. McClelland, and Carnegie Mellon University\n\
 \n\
Permission to use, copy, and modify this software and its documentation\n\
for any purpose other than distribution-for-profit is hereby granted\n\
without fee, provided that the above copyright notice and this permission\n\
notice appear in all copies of the software and related documentation\n\
 \n\
Permission to distribute the software or modified or extended versions\n\
thereof on a not-for-profit basis is explicitly granted, under the\n\
above conditions. HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE\n\
OR MODIFIED OR EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED\n\
EXCEPT BY PRIOR ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS\n\
 \n\
Note that the taString class, which is derived from the GNU String class\n\
is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and\n\
is covered by the GNU General Public License, see ta_string.h\n\
The iv_graphic library and some iv_misc classes were derived from the\n\
InterViews morpher example and other InterViews code, which is\n\
Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University\n\
Copyright (C) 1991 Silicon Graphics, Inc\n\
 \n\
THE SOFTWARE IS PROVIDED 'AS-IS' AND WITHOUT WARRANTY OF ANY KIND\n\
EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY\n\
WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE\n\
\n\
IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY\n\
SPECIAL INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,\n\
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,\n\
WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY\n\
OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE\n\
OF THIS SOFTWARE";

  taMisc::Choice(info, "Ok");
}

void PDPRoot::LoadConfig() {
  String home_dir = getenv("HOME");
  String cfgfn = home_dir + "/.pdpconfig";
  fstream strm;
  strm.open(cfgfn, ios::in);
  if(!strm.bad() && !strm.eof())
    Load(strm);
  strm.close(); strm.clear();
  ((taMisc*)TA_taMisc.GetInstance())->LoadConfig();
}

void PDPRoot::SaveConfig() {
  if (projects.size > 0) {
    taMisc::Error("Error: You cannot save the ~/.pdpconfig file with a project file loaded -- remove the project(s) then try again!");
    return;
  }
  String home_dir = getenv("HOME");
  String cfgfn = home_dir + "/.pdpconfig";
  fstream strm;
  strm.open(cfgfn, ios::out);
  Save(strm);
  strm.close(); strm.clear();
  ((taMisc*)TA_taMisc.GetInstance())->SaveConfig();
}

void PDPRoot::Settings() {
#ifdef TA_GUI
  if(taMisc::gui_active)
//    TA_taMisc.ie->Edit(TA_taMisc.GetInstance(),window);
    TA_taMisc.ie->Edit(TA_taMisc.GetInstance());
#endif
}

void PDPRoot::Quit() {
#ifdef TA_GUI
  cssiSession::quitting = true;
  if (taiMisc::main_window) taiMisc::main_window->close();
//obs  window->close(); //TODO: causing an exception, after the confirm dialog and window closing, but closing directly causes no prob
//  QApplication::postEvent(window, new QCloseEvent());
//TODO: do we need to do something for no-gui compile????
#endif
}

/*obs void PDPRoot::SetWinName() {
  if(!taMisc::gui_active) return;
  if(window == NULL)    return;
  //  String nw_name = String(ivSession::instance()->classname()) + ": Root";
  // if we give it a common name, windows will be grouped together under KDE!
  String nw_name = String(taiM->classname());
  if(nw_name == win_name) return;
  win_name = nw_name;
  window->setCaption(win_name);
} */

/* void PDPRoot::WindowClosing(bool& cancel) {
  int chs = 0;
  QWidgetList* tlw = NULL;
  taiM->MainWindowClosing(cancel);
  if (cancel) goto exit;


//note: following was commented out
 no -- use the projects mechanism below
  //Need to close all the other windows first -- user can cancel
  tlw = QApplication::topLevelWidgets(); //note: must be deleted
  for (int i = tlw->count() - 1;  i >= 0; --i) {
    QWidget* w = tlw->at(i);
    if (w == this->window) continue; // don't close ourself!
    if (!w->close()) {
      cancel = true;
      break;
    }
    taiMisc::RunPending();
  }
  delete tlw;

  if (cancel) goto exit;

// end commenting out

exit:
  if (cancel) quitting = false;
}*/

void PDPRoot::SaveAll() {
//#ifdef TA_GUI
  taLeafItr i;
  Project* pr;
  FOR_ITR_EL(Project, pr, projects., i) {
    taFiler* taf = pr->GetFileDlg();
    if (taf) {
      taRefN::Ref(taf);
      ostream* strm = taf->Save();
      if ((strm != NULL) && strm->good()) {
        taMisc::RecordScript(pr->GetPath() + ".Save(" + taf->fname + ");\n");
        pr->SetFileName(taf->fname);
        DMEM_GUI_RUN_IF {
          pr->Save(*strm);
        }
      }
      taRefN::unRef(taf); //don't do Done in case supplier isn't using refcounts to manage lifetime
    } // taf
  }
//#endif
}

/*obs??
void PDPRoot::GetWindow() {
  IconGlyph* ig =
    new IconGlyph(new ivBackground(win_box, wkit->background()),NULL,this);
  ig->SetIconify = winbMisc::SetIconify;
  ig->ScriptIconify= winbMisc::ScriptIconify;
  window = new ivApplicationWindow(ig);
  ivHandler* delh = new PDPRootWMDeleteHandler(this);
  window->wm_delete(delh);
  ig->SetWindow(window);
} */

/*obsbool PDPRoot::ThisMenuFilter(MethodDef* md) {
  if((md->name == "Load") || (md->name == "Save") || (md->name == "SaveAs") ||
     (md->name == "Close") || (md->name == "Print") || (md->name == "Print_Data") ||
     (md->name == "GetAllWinPos") || (md->name == "ScriptAllWinPos") ||
     (md->name == "SetWinPos") || (md->name == "CopyFrom") || (md->name == "CopyTo") ||
     (md->name == "DuplicateMe") || (md->name == "ChangeMyType") ||
     (md->name == "SelectForEdit") || (md->name == "SelectFunForEdit"))
    return false;
  return true;
} */
/*
void PDPRoot::SetWinPos(float left, float top, float width, float height) {
  WinGeometry* wn_pos = &win_pos;
  if(projects.leaves > 0)
    wn_pos = &(((Project*)projects.Leaf(0))->root_win_pos);

  bool was_set = false;
  if((left != -1.0f) && (top != -1.0f)) {
    wn_pos->lft = left;
    wn_pos->top = top;
    was_set = true;
  }
  if((width != -1.0f) && (height != -1.0f)) {
    wn_pos->wd = width;
    wn_pos->ht = height;
    was_set = true;
  }
  if(was_set) {
    wn_pos->UpdateAfterEdit();	// puts it on the update_winpos list
  }
  else {
    wn_pos->SetWinPos();	// does it now
  }
}

void PDPRoot::GetWinPos() {
  WinGeometry* wn_pos = &win_pos;
  if(projects.leaves > 0)
    wn_pos = &(((Project*)projects.Leaf(0))->root_win_pos);
  wn_pos->GetWinPos();
} */

#ifdef DEBUG
//#include <string.h>
void TestObj::Initialize() {
  // just zero all the pod data
  size_t _size = ((intptr_t)(&intptr) + sizeof(intptr) - (intptr_t)(&b));
  memset(&b, 0, _size);
  
  v_b = false;
  v_c = 'a';
  v_i = 0x7fffffff;
  v_ui = 0xffffffffU;
  v_i64 = 0x7fffffffffffffffLL;
  v_u64 = 0xffffffffffffffffULL;
  v_d = 1.23e456;
  v_str = "the rain in spain";
  v_ptr = (void*)this;
  v_tab = this;
  v_mat = (taMatrix*)NULL;
}
#endif
