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


#include "ta_base.h"
#include "ta_dump.h"
#include "ta_css.h"
#ifdef TA_GUI
# include "css_qt.h"
# include "ta_qt.h"
# include "ta_qtdata.h" // for taiObjChooser
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"
# include "netstru_qtso.h"

# include <qapplication.h>
# include <QWidgetList>
#endif

#include <time.h>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif


//////////////////////////////////
//	TypeDefault_MGroup	//
//////////////////////////////////

int TypeDefault_MGroup::Dump_Load_Value(istream& strm, TAPtr par) {
  Reset();			// get rid of any existing defaults before loading
  return inherited::Dump_Load_Value(strm, par);
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
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
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
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if (cond == NULL) {
    cond = pdpMisc::GetNewConduit(proj);
  }
  if (cond == NULL) return;
  if (net == NULL) {
    net = pdpMisc::GetDefNetwork(GET_MY_OWNER(ProjectBase));
  }
  if (net == NULL) return; // TODO, maybe create net
  cond->InitFromNetwork(net);
}

void Wizard::StdEnv(Environment* env, int n_events) {
  if(env == NULL) {
    env = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase));
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
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
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
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
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
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
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
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
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
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
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


//////////////////////////
//  ProjectBase		//
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
bool ProjectBase::nw_itm_def_arg = false;


void ProjectBase::Initialize() {
  defaults.SetBaseType(&TA_TypeDefault);
  wizards.SetBaseType(&TA_Wizard);
  specs.SetBaseType(&TA_BaseSpec);
  networks.SetBaseType(&TA_Network);
//nn  logs.SetBaseType(&TA_TextLog);
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

void ProjectBase::InitLinks() {
  taBase::Own(defaults, this);
  taBase::Own(wizards, this);
  taBase::Own(specs, this);
  taBase::Own(networks, this);
  taBase::Own(data, this);
  taBase::Own(logs, this);
  taBase::Own(programs, this);
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

void ProjectBase::CutLinks() {
  deleting = true;
/*TODO: equiv for Programs
  SchedProcess* sp;
  taLeafItr li;
  FOR_ITR_EL(SchedProcess, sp, processes., li) {
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
  programs.CutLinks();
  logs.CutLinks();
  data.CutLinks();
  networks.CutLinks();
  specs.CutLinks();
  defaults.CutLinks();

  view_colors.CutLinks();
  the_colors.CutLinks();
}

void ProjectBase::Copy_(const ProjectBase& cp) {
  // delete things first, to avoid dangling refs etc.
  scripts.Reset();
  
  defaults = cp.defaults;
  specs = cp.specs;
  networks = cp.networks;
  data = cp.data;
  logs = cp.logs;
  programs = cp.programs;
  scripts = cp.scripts;
#ifdef TA_GUI
  edits = cp.edits;
  viewers = cp.viewers;
#endif
  view_colors = cp.view_colors;
  the_colors = cp.the_colors;
}

void ProjectBase::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();

  UpdateColors();
}

BaseSpec_MGroup* ProjectBase::FindMakeSpecGp(const char* nm, bool& nw_itm) {
  BaseSpec_MGroup* gp = (BaseSpec_MGroup*)specs.gp.FindName(nm);
  nw_itm = false;
  if(gp == NULL) {
    gp = (BaseSpec_MGroup*)specs.gp.New(1);
    gp->name = nm;
    nw_itm = true;
  }
  return gp;
}

void ProjectBase::GetDefaultColors() {
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

const iColor* ProjectBase::GetObjColor(TypeDef* td) {
#ifdef TA_GUI
  if(view_colors.size != COLOR_COUNT) {
    view_colors.Reset();
    GetDefaultColors();
  }
  if(the_colors.size != COLOR_COUNT)
    UpdateColors();
  if(td->InheritsFrom(TA_ConSpec))
    return the_colors.FastEl(ProjectBase::CON_SPEC)->color();
  else if(td->InheritsFrom(TA_UnitSpec))
    return the_colors.FastEl(ProjectBase::UNIT_SPEC)->color();
  else if(td->InheritsFrom(TA_ProjectionSpec))
    return the_colors.FastEl(ProjectBase::PRJN_SPEC)->color();
  else if(td->InheritsFrom(TA_LayerSpec))
    return the_colors.FastEl(ProjectBase::LAYER_SPEC)->color();
  else if(td->InheritsFrom(TA_Network))
    return the_colors.FastEl(ProjectBase::NETWORK)->color();
  else if(td->InheritsFrom(TA_Environment))
    return the_colors.FastEl(ProjectBase::ENVIRONMENT)->color();
  else if(td->InheritsFrom(TA_PDPLog))
    return the_colors.FastEl(ProjectBase::PDPLOG)->color();
  else if(td->InheritsFrom(TA_SchedProcess))
    return the_colors.FastEl(ProjectBase::SCHED_PROC)->color();
  else if(td->InheritsFrom(TA_Stat))
    return the_colors.FastEl(ProjectBase::STAT_PROC)->color();
  else if(td->InheritsFrom(TA_Process))
    return the_colors.FastEl(ProjectBase::OTHER_PROC)->color();
  else if(td->InheritsFrom(TA_taGroup_impl))
    return the_colors.FastEl(ProjectBase::GEN_GROUP)->color();
  else if(td->InheritsFrom(TA_Wizard))
    return the_colors.FastEl(ProjectBase::WIZARD)->color();
#endif
  return NULL;
}

const iColor* ProjectBase::GetObjColor(ViewColors vc) {
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

int ProjectBase::Load(istream& strm, TAPtr par) {
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

void ProjectBase::LoadDefaults() {
  fstream def;
  // use a user-provided default 
  //TODO: this may not be a great idea in the v4 integrated runtime
  //TODO: the entire handling of defaults in v4 needs to be redone
  String def_file;
  if (!pdpMisc::user_spec_def.empty())
    def_file = pdpMisc::user_spec_def;
  else
    def_file = defaults_file;
  if (!def_file.empty() && cssProgSpace::GetFile(def, def_file)) {
    defaults.Load(def);
    def.close(); def.clear();
    //    cerr << "Successfully loaded default file: " << pdpMisc::root->default_file << "\n";
  } else if (!defaults_str.empty()) {
    std::string def_str = defaults_str.chars();
    std::istringstream sdef(def_str);
    sdef.seekg(0, ios::beg);
    defaults.Load(sdef);
    if(taMisc::dmem_proc == 0)
      cerr << "Using standard pre-compiled defaults file\n";
  } else {
    taMisc::Warning("no default file or defaults were available - created objects will not automatically be of correct type");
  }
}

void ProjectBase::MakeDefaultWiz(bool auto_opn) {
  Wizard* wiz = (Wizard*)wizards.New(1, wizards.el_typ);
  if(auto_opn) {
    wiz->auto_open = true;
//TEMP    wiz->ThreeLayerNet();
    wiz->Edit();
  }
}

#ifdef TA_GUI
pdpDataViewer* ProjectBase::NewViewer() {
  pdpDataViewer* vwr = NULL;
  vwr = pdpDataViewer::New(this);
  viewers.Add(vwr);
  return vwr;
}
#endif
#ifdef TA_GUI
void ProjectBase::OpenNetworkViewer(Network* net) {
  if (!net) return;

  // network objects
  pdpDataViewer* vwr = NewViewer();
  NetView* nv = NetView::New(vwr, net);
  nv->BuildAll();

  vwr->ViewWindow();
}
#endif
int ProjectBase::SaveAs(ostream& strm, TAPtr par, int indent) {
#ifdef TA_GUI
  if (use_sim_log) {
    UpdateSimLog();
  }
#endif
  return inherited::SaveAs(strm, par, indent);
}

int ProjectBase::Save(ostream& strm, TAPtr par, int indent) {
  if (taMisc::gui_active) {
    taMisc::Busy();
  }
  ++taMisc::is_saving;
  dumpMisc::path_tokens.Reset();
  strm << "// ta_Dump File v2.0\n";   // be sure to check version with Load
  int rval = Dump_Save_Path(strm, par, indent);
  if (rval == false) 
     goto exit;
  strm << " {\n";

  // save defaults within project save as first item
  defaults.Dump_Save_Path(strm, par, indent+1);
  strm << " { ";
  if (defaults.Dump_Save_PathR(strm, par, indent+2))
    taMisc::indent(strm, indent+1, 1);
  strm << "  };\n";
  defaults.Dump_Save_impl(strm, par, indent+1);
//nn,already in _impl  defaults.Dump_SaveR(strm, par, indent+1);

  if (Dump_Save_PathR(strm, par, indent+1))
    taMisc::indent(strm, indent, 1);
  strm << "};\n";
  Dump_Save_impl(strm, par, indent);
//nn,already in _impl  Dump_SaveR(strm, par, indent);
  rval = true;
  
exit:
  --taMisc::is_saving;
  dumpMisc::path_tokens.Reset();
  if (taMisc::gui_active)  taMisc::DoneBusy();
  return rval;
}

bool ProjectBase::SetFileName(const String& val) {
  prev_file_nm = GetFileName();
  return inherited::SetFileName(val);
}

void ProjectBase::UpdateColors() {
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
void ProjectBase::UpdateSimLog() {
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
  int rval = taGroup<ProjectBase>::Load(strm, par);
  if (rval) {
    ProjectBase* p;
    taLeafItr i;
    FOR_ITR_EL(ProjectBase, p, this->, i) {
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
  projects.SetBaseType(&TA_ProjectBase); //note: must actually be one of the descendants
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
    }
  } else {
    projects.RemoveAll();
  }
  colorspecs.RemoveAll();
  CutLinks();
  if (we_are_root) {
    taiMiscCore::RunPending();
    taiMiscCore::Quit();
  }
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
  //cssiSession::quitting = true;
  taiMiscCore::Quit();
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
  ProjectBase* pr;
  FOR_ITR_EL(ProjectBase, pr, projects., i) {
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
    wn_pos = &(((ProjectBase*)projects.Leaf(0))->root_win_pos);

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
    wn_pos = &(((ProjectBase*)projects.Leaf(0))->root_win_pos);
  wn_pos->GetWinPos();
} */

#ifdef DEBUG
//#include <string.h>
void TestObj::Initialize() {
  // just zero all the pod data
  size_t _size = ((intptr_t)(&intptr) + sizeof(intptr) - (intptr_t)(&b));
  memset(&b, 0, _size);
  
  v_b = false;
  v_c = '\0';
  v_i.setInt(0);
  v_i_ro.setInt(0x7fffffff);
  v_ui.setUInt(0U);
  v_i64.setInt64(0LL);
  v_u64.setUInt64(0ULL);
  v_d.setDouble(0.0);
  v_str.setString(_nilString);
  s_ptr = NULL;
  v_ptr.setPtr(NULL);
  s_tab = NULL;
  v_tab.setBase(NULL);
  s_mat = NULL;
  v_mat.setMatrix(NULL);
  s_own_tab = NULL;
  v_own_tab.setBase(NULL);
}

void TestObj::InitLinks() {
  inherited::InitLinks();
  UpdateTypeDefVars();
}

void TestObj::CutLinks() {
  taBase* own_tab = v_own_tab.toBase();
  if (own_tab) {
    own_tab->CutLinks();
    v_own_tab.setBase(NULL); // should delete
  }
  if (s_own_tab) {
    s_own_tab->CutLinks();
    taBase::DelPointer(&s_own_tab);
  }
  v_tab.setBase(NULL);
  v_mat.setMatrix(NULL);
  inherited::CutLinks();
}

void TestObj::UpdateAfterEdit() {
  UpdateTypeDefVars();
  inherited::UpdateAfterEdit();
}

void TestObj::UpdateTypeDefVars() {
  MemberDef* md;
  TypeDef* mt = GetTypeDef();
  md = mt->members.SafeEl(mt->members.Find("s_ptr"));
  if (md != NULL) {
    typ_s_ptr = md->type->name;
  }
  md = mt->members.SafeEl(mt->members.Find("s_tab"));
  if (md != NULL) {
    typ_s_tab = md->type->name;
  }
  md = mt->members.SafeEl(mt->members.Find("s_mat"));
  if (md != NULL) {
    typ_s_mat = md->type->name;
  }
  void* dummy;
  TypeDef* typ;
  v_ptr.GetRepInfo(typ, dummy);
  typ_v_ptr = typ->name; 
  v_tab.GetRepInfo(typ, dummy);
  typ_v_tab = typ->name; 
  v_mat.GetRepInfo(typ, dummy);
  typ_v_mat = typ->name; 
}

void TestObj::InitObj() {
  b = true;
  c = 'a';
  sc = -128;
  uc = 255;
  byt = 254; //note: s/b same as an unsigned char
  sh = -32768;
  ssh = 32766;
  ush = 65535;
  i = 2147483647;
  si = -2147483647;
  s = 2147483645;
  ui = 4294967295U;
  u = 4294967294U;
  l = 2147483647L;
  sl = 2147483646L;
  ul = 4294967295UL;
  i64 = 9223372036854775807LL;
  ll = 9223372036854775806LL;
  sll = 9223372036854775805LL;
  u64 = 18446744073709551615ULL;
  ull = 18446744073709551614ULL;
  intptr = (sizeof(int) == sizeof(intptr_t)) ? i : (intptr_t)i64;

  v_b = true;
  v_c = 'a';
  v_i = 0x7fffffff;
  v_ui = 0xffffffffU;
  v_i64 = 0x7fffffffffffffffLL;
  v_u64 = 0xffffffffffffffffULL;
  v_d = 1.23e45;
  v_str = "the rain in spain";
  v_ptr = (void*)this;
  v_ptr = s_ptr;
  taBase::SetPointer(&s_tab, this->GetOwner());
  v_tab = s_tab;
  // find a matrix in the owner
  taList_impl* own_lst = GET_MY_OWNER(taList_impl);
  if (own_lst != NULL) {
    taBase* it;
    taListItr itr;
    FOR_ITR_EL(taBase, it, own_lst->, itr) {
      if (it->InheritsFrom(&TA_taMatrix)) {
        taMatrix* mat = (taMatrix*)it;
        // initialize it if not initialized
        if (mat->size == 0) {
          mat->SetGeom(2, 2, 3);
          mat->SetFmStr_Flat("0", 0);
          mat->SetFmStr_Flat("1", 1);
          mat->SetFmStr_Flat("2", 2);
          mat->SetFmStr_Flat("3", 3);
          mat->SetFmStr_Flat("4", 4);
          mat->SetFmStr_Flat("5", 5);
        }
        taBase::SetPointer((taBase**)&s_mat, mat);
        v_mat.setMatrix(s_mat);
        break;
      }
    }
  }
  if (!taMisc::is_loading) {
    if (s_own_tab == NULL) {
      taBase::SetPointer(&s_own_tab, new taNBase());
      s_own_tab->SetName("s_owned_taBase");
      taBase::Own(s_own_tab, this);
    }
    if (v_own_tab.isNull()) {
      v_own_tab = new taNBase();
      v_own_tab.toBase()->SetName("v_owned_taBase");
      taBase::Own(v_own_tab.toBase(), this);
    }
  }
  UpdateAfterEdit();
}


bool TestObj::TestMethod1(
    const Variant&	v, 
    int64_t		i64,
    long long		ll,
    signed long long	sll,
    uint64_t		u64,
    unsigned long long	ull,
    bool		b,
    char		c,
    signed char		sc,
    unsigned char	uc,
    byte		byt,
    short		sh,
    signed short	ssh,
    unsigned short	ush,
    int			i,
    signed int		si,
    signed		s,
    unsigned int	ui,
    unsigned		u,
    long		l,
    signed long		sl,
    unsigned long	ul,
    taBase*		s_tab
) 
{
  this->v =v; 
  this->i64 = i64;
  this->ll = ll;
  this->sll = sll;
  this->u64 = u64;
  this->ull = ull;
  this->b = b; // #DEF_false
  this->c = c;
  this->sc = sc;
  this->uc = uc;
  this->byt = byt; //note: s/b same as an unsigned char
  this->sh = sh;
  this->ssh = ssh;
  this->ush = ush;
  this->i = i; // #DEF_0
  this->si = si;
  this->s = s;
  this->ui = ui;
  this->u = u;
  this->l = l;
  this->sl = sl;
  this->ul = ul;
  taBase::SetPointer(&(this->s_tab), s_tab);
  UpdateAfterEdit();
  return b;
}

bool TestObj::TestMethod_ProblemDefs(
    char		c,
    unsigned int	ui,
    unsigned		u,
    long		l,
    signed long		sl,
    unsigned long	ul,
    int64_t		i64,
    long long		ll,
    signed long long	sll,
    uint64_t		u64,
    unsigned long long	ull,
    const Variant&	v, 
    taBase*		s_tab
) 
{
  this->c = c;
  this->ui = ui;
  this->u = u;
  this->l = l;
  this->sl = sl;
  this->ul = ul;
  this->i64 = i64;
  this->ll = ll;
  this->sll = sll;
  this->u64 = u64;
  this->ull = ull;
  this->v =v; 
  taBase::SetPointer(&(this->s_tab), s_tab);
  UpdateAfterEdit();
  return true;
}

void TestObj2::Initialize() {
  int_val = 0;
}

void TestObj2::InitLinks() {
  inherited::InitLinks();
  taBase::Own(test_obj, this);
  if (!taMisc::is_loading) {
      if (test_obj.s_own_tab == NULL) {
      taBase::SetPointer(&test_obj.s_own_tab, new taNBase());
      test_obj.s_own_tab->SetName("par_created_s_owned_taBase");
      taBase::Own(test_obj.s_own_tab, &test_obj);
    }
  }

}

void TestObj2::CutLinks() {
  test_obj.CutLinks();
  inherited::CutLinks();
}

void TestObj2::Copy_(const TestObj2& cp) {
  int_val = cp.int_val;
  test_obj = cp.test_obj;
}

void TestObj2::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}


void TestObj3::Initialize() {
  i = 1;
  s_own_tab = NULL;
  f = 2.3f;
}

void TestObj3::InitLinks() {
  inherited::InitLinks();
  if (!taMisc::is_loading) {
    if (s_own_tab == NULL) {
      taBase::SetPointer(&s_own_tab, new taNBase());
      s_own_tab->SetName("s_owned_taBase");
      taBase::Own(s_own_tab, this);
    }
  }

}

void TestObj3::CutLinks() {
  if (s_own_tab) {
    s_own_tab->CutLinks();
    taBase::DelPointer(&s_own_tab);
  }
  inherited::CutLinks();
}

void TestObj3::Copy_(const TestObj3& cp) {
  i = cp.i;
  if (s_own_tab && cp.s_own_tab) 
    s_own_tab->Copy(*(cp.s_own_tab));
  f = cp.f;
}

void TestObj3::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

#include "tdgeometry.h"

void TestOwnObj::Initialize() {
  ft_own = NULL;
}

void TestOwnObj::InitLinks() {
  inherited::InitLinks();
  if (!taMisc::is_loading) {
    if (ft_own == NULL) {
      taBase::SetPointer((taBase**)&ft_own, (taBase*)new FloatTransform());
      taBase::Own(ft_own, this);
    }
  } else {
    if (ft_own) {
      taBase::Own(ft_own, this);
    }
  }
}

void TestOwnObj::CutLinks() {
  if (ft_own) {
    ft_own->CutLinks();
    taBase::DelPointer((taBase**)&ft_own);
  }
  inherited::CutLinks();
}

void TestOwnObj::Copy_(const TestOwnObj& cp) {
  ft_inst = cp.ft_inst;
  if (ft_own && cp.ft_own) 
    ft_own->Copy(*(cp.ft_own));
}

void TestOwnObj::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}


 
void TestOwnedObj::InitLinks() {
  inherited::InitLinks();
  taBase::OwnPointer((TAPtr*)&o1, new TestOwnedObj_taBase, this);
  taBase::OwnPointer((TAPtr*)&o2, new TestOwnedObj_taOBase, this); 
  taBase::OwnPointer((TAPtr*)&o3, new TestOwnedObj_taBase_inline, this); 
  taBase::OwnPointer((TAPtr*)&o4, new TestOwnedObj_taOBase_inline, this); 
  
  taBase::OwnPointer((TAPtr*)&o5, new TestOwnedObj_taBase, this);
  o5->i = 5; o5->s = "s5";
  taBase::OwnPointer((TAPtr*)&o6, new TestOwnedObj_taOBase, this); 
  o6->i = 6; o6->s = "s6";
  taBase::OwnPointer((TAPtr*)&o7, new TestOwnedObj_taBase_inline, this); 
  o7->i = 7; o7->s = "s7";
  taBase::OwnPointer((TAPtr*)&o8, new TestOwnedObj_taOBase_inline, this); 
  o8->i = 8; o8->s = "s8";
  
  // following are not owned
  taBase::SetPointer((TAPtr*)&o9, new TestOwnedObj_taOBase); 
  o9->i = 9; o9->s = "s9";
  taBase::SetPointer((TAPtr*)&oA, new TestOwnedObj_taOBase_inline); 
  oA->i = 10; oA->s = "sA";
  
  taBase::SetPointer((TAPtr*)&oB, new TestOwnedObj_taOBase); 
  oB->i = 11; oB->s = "sB";
  taBase::SetPointer((TAPtr*)&oC, new TestOwnedObj_taOBase_inline); 
  oC->i = 12; oC->s = "sC";
  
}

void TestOwnedObj::CutLinks() {
  taBase::DelPointer((TAPtr*)&oC);
  taBase::DelPointer((TAPtr*)&oB);
  taBase::DelPointer((TAPtr*)&oA);
  taBase::DelPointer((TAPtr*)&o9);
  
  if (o8) {o8->CutLinks(); taBase::DelPointer((TAPtr*)&o8);}
  if (o7) {o7->CutLinks(); taBase::DelPointer((TAPtr*)&o7);}
  if (o6) {o6->CutLinks(); taBase::DelPointer((TAPtr*)&o6);}
  if (o5) {o5->CutLinks(); taBase::DelPointer((TAPtr*)&o5);}
  if (o4) {o4->CutLinks(); taBase::DelPointer((TAPtr*)&o4);}
  if (o3) {o3->CutLinks(); taBase::DelPointer((TAPtr*)&o3);}
  if (o2) {o2->CutLinks(); taBase::DelPointer((TAPtr*)&o2);}
  if (o1) {o1->CutLinks(); taBase::DelPointer((TAPtr*)&o1);}
  inherited::CutLinks();
}

#endif
