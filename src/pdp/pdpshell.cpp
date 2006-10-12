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

// 

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
# include "pdplog_qtso.h" // TEMP, until moved

# include <qapplication.h>
# include <QWidgetList>
#endif

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif


//////////////////////////
//    LayerWizEl	//
//////////////////////////

void LayerWizEl::Initialize() {
  n_units = 25;
}


//////////////////////////
//   Wizard		//
//////////////////////////

void Wizard::Initialize() {
  n_layers = 3;
  layer_cfg.SetBaseType(&TA_LayerWizEl);
  connectivity = FEEDFORWARD;
  event_type = &TA_Event;
}

void Wizard::InitLinks() {
  inherited::InitLinks();
  taBase::Own(layer_cfg, this);
  layer_cfg.EnforceSize(n_layers);
}

void Wizard::CutLinks() {
  layer_cfg.RemoveAll();
  inherited::CutLinks();
}

void Wizard::UpdateAfterEdit() {
  layer_cfg.EnforceSize(n_layers);
  inherited::UpdateAfterEdit();
}

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
        lay->layer_type = Layer::TARGET;
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
  net->LayerZPos_Auto();
  net->ShowInViewer();
  net->Build();
  net->Connect();
//obs  taMisc::DelayedMenuUpdate(net);
}


//////////////////////////////////
// 	Enviro Wizard		//
//////////////////////////////////

void Wizard::StdInputData(Network* net, DataTable* data_table, int n_patterns, bool group) {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if(!data_table) {
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("InputData");
    data_table = dgp->NewEl(1, &TA_DataTable); // todo: should be in InputData
    data_table->name = "StdInputData";
  }
  if(!net) {
    net = pdpMisc::GetDefNetwork(GET_MY_OWNER(ProjectBase));
  }
  if(!net) return;
  if(group) {
    int gp_idx = 0;
    data_table->FindMakeColName("Group", gp_idx, DataTable::VT_STRING, 0);
  }
  int nm_idx = 0;
  data_table->FindMakeColName("Name", nm_idx, DataTable::VT_STRING, 0);

  UpdateInputDataFmNet(net, data_table);

  data_table->AddRow(n_patterns);
}

void Wizard::UpdateInputDataFmNet(Network* net, DataTable* data_table) {
  if(!data_table || !net) return;
  taLeafItr li;
  Layer* lay;
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    if(lay->layer_type == Layer::HIDDEN) continue;
    int lay_idx = 0;
    DataArray_impl* ld = data_table->FindMakeColName
      (lay->name, lay_idx, DataTable::VT_FLOAT, 2,
       MAX(lay->geom.x,1), MAX(lay->geom.y,1));
  }
}

void Wizard::StdOutputData() {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("OutputData");
  DataTable* trl_data = dgp->NewEl(1, &TA_DataTable);
  DataTable* epc_data = dgp->NewEl(1, &TA_DataTable);
  trl_data->SetName("TrialOutputData");
  epc_data->SetName("EpochOutputData");
}

//////////////////////////////////
// 	Progs Wizard		//
//////////////////////////////////

void Wizard::StdProgs() {
  taMisc::Error("This must be redefined in algorithm-specific project!",
		"Just call StdProgs_impl with name of std program from proglib");
}

Program_Group* Wizard::StdProgs_impl(const String& prog_nm) {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  taBase* rval = proj->programs.NewFromLibByName(prog_nm);
  if(!rval) return NULL;
  if(!rval->InheritsFrom(&TA_Program_Group)) {
    taMisc::Error("Wizard::StdProgs_impl program named:", prog_nm,
		  "is not a group of programs -- invalid for this function");
    return NULL;
  }
  return (Program_Group*)rval;
}

/* TEMP

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

//////////////////////////
//  ProjectBase		//
//////////////////////////

void ProjectBase::Initialize() {
  // up-class a few of the bases
  wizards.SetBaseType(&TA_Wizard);
  // now the rest
  save_rmv_units = true; // default is not to save units
}

void ProjectBase::InitLinks_impl() {
  inherited::InitLinks_impl();
  taBase::Own(networks, this);
  taBase::Own(the_colors, this);
  taBase::Own(view_colors, this);

  if (taMisc::is_loading)
    view_colors.Reset();		// kill existing colors
  else
    GetDefaultColors();

  // make default groups for different types of data
  data.FindMakeGpName("InputData");
  data.FindMakeGpName("OutputData");
  data.FindMakeGpName("AnalysisData");
}

void ProjectBase::CutLinks_impl() {
  // do base first, esp. to nuke viewers before the networks
  inherited::CutLinks_impl();
  view_colors.CutLinks();
  the_colors.CutLinks();
  networks.CutLinks();
}

void ProjectBase::Copy_(const ProjectBase& cp) {
  networks = cp.networks;

  view_colors = cp.view_colors;
  the_colors = cp.the_colors;
  UpdatePointers_NewPar((taBase*)&cp, this); // update all the pointers!
}

void ProjectBase::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();

  UpdateColors();
}

DataTable_Group* ProjectBase::analysisDataGroup() {
  DataTable_Group* rval = (DataTable_Group*)data.FindMakeGpName("AnalysisData");
  return rval;
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
  else if (td->InheritsFrom(TA_Network) ||
    td->InheritsFrom(TA_Layer) ||
    td->InheritsFrom(TA_Unit_Group) ||
    td->InheritsFrom(TA_Projection) ||
    td->InheritsFrom(TA_Connection) ||
    td->InheritsFrom(TA_Con_Group) ||
    td->InheritsFrom(TA_Unit) ||
    td->InheritsFrom(TA_Network_Group)
    ) return the_colors.FastEl(ProjectBase::NETWORK)->color();
  else if(td->InheritsFrom(TA_Environment))
    return the_colors.FastEl(ProjectBase::ENVIRONMENT)->color();
  else if (td->InheritsFrom(TA_TableView)) //note: formerly logs
    return the_colors.FastEl(ProjectBase::PDPLOG)->color();
  else if(td->InheritsFrom(TA_SchedProcess))
    return the_colors.FastEl(ProjectBase::SCHED_PROC)->color();
  else if(td->InheritsFrom(TA_Stat))
    return the_colors.FastEl(ProjectBase::STAT_PROC)->color();
  else if(td->InheritsFrom(TA_Process))
    return the_colors.FastEl(ProjectBase::OTHER_PROC)->color();
  else if(td->InheritsFrom(TA_taGroup_impl))
    return the_colors.FastEl(ProjectBase::GEN_GROUP)->color();
  else if(td->InheritsFrom(TA_taWizard)) //note: all Wizards, even ta ones
    return the_colors.FastEl(ProjectBase::WIZARD)->color();
#endif
  return NULL;
}

const iColor* ProjectBase::GetObjColor(int/*ViewColors*/ vc) {
#ifdef TA_GUI
  if (view_colors.size != COLOR_COUNT) {
    view_colors.Reset();
    GetDefaultColors();
  }
  if (the_colors.size != COLOR_COUNT)
    UpdateColors();
  TAColor* tac = the_colors.SafeEl(vc);
  if (tac)
    return tac->color();
#endif
  return NULL;
}

void ProjectBase::AssertDefaultWiz(bool auto_opn) {
  taWizard* wiz = wizards.SafeEl(0);
//TODO: need a better wizard making api -- factor out the make routine
  if (!wiz) {
    wiz = (Wizard*)wizards.New(1, wizards.el_typ);
  }
  if(auto_opn) {
    wiz->auto_open = true;
    ((Wizard*)wiz)->ThreeLayerNet();
    wiz->Edit();
  }
}

void ProjectBase::NewGraphView(DataTable* dt, T3DataViewFrame* fr)
{
  if (!dt) {
    taMisc::Error("You must specify a DataTable");
    return;
  }
  //note: even if fr specified, need to insure it is right proj for table
  ProjectBase* proj = (ProjectBase*)dt->GetOwner(&TA_ProjectBase);
  
  if (fr) {
    if (proj != fr->GetOwner(&TA_ProjectBase)) {
      taMisc::Error("The viewer you specified is not in the same Project as the table.");
      return;
    }
  } else {
    MainWindowViewer* vw = MainWindowViewer::GetDefaultProjectBrowser(proj);
    if (!vw) return; // shouldn't happen
    T3DataViewer* t3vw = (T3DataViewer*)vw->FindFrameByType(&TA_T3DataViewer);
    if (!t3vw) return; // shouldn't happen
    fr = t3vw->NewT3DataViewFrame();
  }
  GraphTableView::NewGraphTableView(dt, fr); // discard result
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

//////////////////////////
//  PDPRoot		//
//////////////////////////

void PDPRoot::Initialize() {
  projects.SetBaseType(&TA_ProjectBase); //note: must actually be one of the descendants
  colorspecs.SetBaseType(&TA_ColorScaleSpec);
}

void PDPRoot::Destroy() {
  CutLinks();
}

void PDPRoot::InitLinks() {
  inherited::InitLinks();
  taBase::Own(colorspecs, this);
}

void PDPRoot::CutLinks() {
  colorspecs.CutLinks();
  inherited::CutLinks();
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

const iColor* PDPRoot::GetObjColor(taBase* inst) {
  if (!inst) return NULL;
  ProjectBase* proj = (ProjectBase*)inst->GetOwner(&TA_ProjectBase);
  if (proj) return proj->GetObjColor(inst->GetTypeDef());
  else return NULL;
} 

void PDPRoot::Info() {
  STRING_BUF(info, 2048);
  info += "PDP Info\n";
  info += "This is the PDP++ software package, version: ";
  info += version_no;
  info += "\n\n";
  info += "Mailing List:       http://psych.colorado.edu/~oreilly/PDP++/pdp-discuss.html\n";
  info += "WWW Page:           http://psych.colorado.edu/~oreilly/PDP++/PDP++.html\n";
  info += "Anonymous FTP Site: ftp://grey.colorado.edu/pub/oreilly/pdp++/\n";
  info += "\n\n";

  info += "Copyright (c) 1995-2006, Regents of the University of Colorado,\n\
Carnegie Mellon University, Princeton University.\n\
 \n\
TA/PDP++ is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\
 \n\
TA/PDP++ is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
 \n\
Note that the taString class was derived from the GNU String class\n\
Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and\n\
is covered by the GNU General Public License, see ta_string.h\n";

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
    pr->Save_File(); // does SaveAs if no filename
/*    taFiler* taf = pr->GetFileDlg();
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
  */
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

