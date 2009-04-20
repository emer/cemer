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

// 

#include "emergent_project.h"


#include "ta_base.h"
#include "ta_dump.h"
#include "css_ta.h"
#include "netdata.h"
#include "ta_program_els.h"

#include "ta_program.h"
#include "ta_imgproc.h"

// #include "ta_fontspec.h"

#ifdef TA_GUI
# include "css_qt.h"
# include "ta_qt.h"
# include "ta_qtdata.h" // for taiObjChooser
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"
# include "ta_datatable_qtso.h"
# include "netstru_qtso.h"
//# include "ta_seledit.h"
//# include "ta_qtviewer.h"

# include <qapplication.h>
# include <QWidgetList>
#endif


#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

/* colors from v3.2
"VioletRed1";"Network";
"DarkOliveGreen3";"Env";
"yellow";"SchedProc";
"LightSteelBlue2";"StatGroup";
"wheat";"SubProcGroup";
"SlateBlue1";"Stat";
"gold";"Process";
"burlywood2";"Log";
"aquamarine";"Agg Highlite";
"grey64";"Group";
"grey75";"Inactive";
"red";"Stopping Stat";
"SlateBlue3";"Agging Stat";
"SpringGreen";"ConSpec";
"violet";"UnitSpec";
"orange";"PrjnSpec";
"MediumPurple1";"LayerSpec";
"azure";"Wizard";
*/

static void emergent_viewcolor_init() {
  if(!taMisc::view_colors) {
    taRootBase::Startup_InitViewColors();
  }
  taMisc::view_colors->FindMakeViewColor("Network", "Emergent Neural network", true, "VioletRed1");
  taMisc::view_colors->FindMakeViewColor("ConSpec", "Emergent Connection Spec", true, "SpringGreen");
  taMisc::view_colors->FindMakeViewColor("Connection", "Emergent Connection", true, "SpringGreen");
  taMisc::view_colors->FindMakeViewColor("UnitSpec", "Emergent Unit Spec", true, "violet");
  taMisc::view_colors->FindMakeViewColor("Unit", "Emergent Unit", true, "violet");
  taMisc::view_colors->FindMakeViewColor("ProjectionSpec", "Emergent Projection Spec", true, "orange");
  taMisc::view_colors->FindMakeViewColor("Projection", "Emergent Projection", true, "orange");
  taMisc::view_colors->FindMakeViewColor("LayerSpec", "Emergent Layer Spec", true, "MediumPurple1");
  taMisc::view_colors->FindMakeViewColor("Layer", "Emergent Layer", true, "MediumPurple1");
}

void emergent_project_init() {
  emergent_viewcolor_init();
  taMisc::default_app_install_folder_name = "Emergent";
}

// module initialization
InitProcRegistrar mod_init_emergent_project(emergent_project_init);


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
  default_net_type = &TA_Network;
}

void Wizard::InitLinks() {
  inherited::InitLinks();
  taBase::Own(layer_cfg, this);
  layer_cfg.SetSize(n_layers);
}

void Wizard::CutLinks() {
  layer_cfg.RemoveAll();
  inherited::CutLinks();
}

void Wizard::UpdateAfterEdit() {
  layer_cfg.SetSize(n_layers);
  inherited::UpdateAfterEdit();
}

bool Wizard::ThreeLayerNet() {
  n_layers = 3;
  layer_cfg.SetSize(n_layers);
  layer_cfg[0]->name = "Input";
  layer_cfg[0]->io_type = LayerWizEl::INPUT;
  layer_cfg[0]->DataChanged(DCR_ITEM_UPDATED);
  layer_cfg[1]->name = "Hidden";
  layer_cfg[1]->io_type = LayerWizEl::HIDDEN;
  layer_cfg[1]->DataChanged(DCR_ITEM_UPDATED);
  layer_cfg[2]->name = "Output";
  layer_cfg[2]->io_type = LayerWizEl::OUTPUT;
  layer_cfg[2]->DataChanged(DCR_ITEM_UPDATED);
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(&layer_cfg, "BrowserExpandAll");
  }
  return true;
}

bool Wizard::MultiLayerNet(int n_inputs, int n_hiddens, int n_outputs) {
  n_layers = n_inputs + n_hiddens + n_outputs;
  layer_cfg.SetSize(n_layers);
  int i;
  for(i=0;i<n_inputs;i++) {
    LayerWizEl* lel = layer_cfg[i];
    lel->name = "Input";
    if(n_inputs > 1) lel->name += "_" + String(i);
    lel->io_type = LayerWizEl::INPUT;
    lel->DataChanged(DCR_ITEM_UPDATED);
  }
  for(;i<n_inputs + n_hiddens;i++) {
    LayerWizEl* lel = layer_cfg[i];
    lel->name = "Hidden";
    if(n_hiddens > 1) lel->name += "_" + String(i-n_inputs);
    lel->io_type = LayerWizEl::HIDDEN;
    lel->DataChanged(DCR_ITEM_UPDATED);
  }
  for(;i<n_layers;i++) {
    LayerWizEl* lel = layer_cfg[i];
    lel->name = "Output";
    if(n_outputs > 1) lel->name += "_" + String(i-(n_inputs+n_hiddens));
    lel->io_type = LayerWizEl::OUTPUT;
    lel->DataChanged(DCR_ITEM_UPDATED);
  }
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(&layer_cfg, "BrowserExpandAll");
  }
  return true;
}

bool Wizard::StdNetwork(TypeDef* net_type, Network* net) {
  if(!net) {
    ProjectBase* proj = GET_MY_OWNER(ProjectBase);
    if(TestError(!proj, "StdNetwork", "network is NULL and could not find project owner to make a new one -- aborting!")) return false;
    net = proj->GetNewNetwork(net_type);
    if(TestError(!net, "StdNetwork", "network is NULL and could not make a new one -- aborting!")) return false;
  }
  net->StructUpdate(true);
  layer_cfg.SetSize(n_layers);
  net->layers.SetSize(n_layers);
  int i;
  int n_hid_layers = 0;
  for(i=0;i<layer_cfg.size;i++) {
    LayerWizEl* el = (LayerWizEl*)layer_cfg[i];
    if(el->io_type == LayerWizEl::HIDDEN) n_hid_layers++;
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
	  lay->pos.x = prv->pos.x + prv->un_geom.x + 1;
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
	    lay->pos.x = prv->pos.x + prv->un_geom.x + 1;
	  }
	}
      }
    }
    lay->un_geom.n = el->n_units;
    lay->un_geom.FitN(lay->un_geom.n);
    if(lay->un_geom.x * lay->un_geom.y != lay->un_geom.n) {
      lay->un_geom.n_not_xy = true;
    }
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
  net->LayerZPos_Unitize();
  net->Build();
  net->StructUpdate(false);
  net->FindMakeView();
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(net, "BrowserExpandAll");
    tabMisc::DelayedFunCall_gui(net, "BrowserSelectMe");
  }
  return true;
}

bool Wizard::RetinaSpecNetwork(RetinaSpec* retina_spec, Network* net) {
  if(!net) {
    ProjectBase* proj = GET_MY_OWNER(ProjectBase);
    if(TestError(!proj, "RetinaSpecNetwork", "network is NULL and could not find project owner to make a new one -- aborting!")) return false;
    net = proj->GetNewNetwork();
    if(TestError(!net, "RetinaSpecNetwork", "network is NULL and could not make a new one -- aborting!")) return false;
  }
  if(TestError(!retina_spec, "RetinaSpecNetwork", "retina_spec is NULL -- you need to create and configure this in advance, as it is then used to configure the network!")) return false;

  net->StructUpdate(true);
  for(int i=0;i<retina_spec->dogs.size; i++) {
    DoGRetinaSpec* sp = retina_spec->dogs[i];
    Layer* on_lay = net->FindMakeLayer(sp->name + "_on");
    on_lay->un_geom.x = sp->spacing.output_size.x;
    on_lay->un_geom.y = sp->spacing.output_size.y;
    on_lay->un_geom.n = sp->spacing.output_units;
    on_lay->layer_type = Layer::INPUT;
    Layer* off_lay = net->FindMakeLayer(sp->name + "_off");
    off_lay->un_geom.x = sp->spacing.output_size.x;
    off_lay->un_geom.y = sp->spacing.output_size.y;
    off_lay->un_geom.n = sp->spacing.output_units;
    off_lay->layer_type = Layer::INPUT;
  }
  net->StructUpdate(false);
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(net, "BrowserExpandAll");
    tabMisc::DelayedFunCall_gui(net, "BrowserSelectMe");
  }
  return true;
}

//////////////////////////////////
// 	Enviro Wizard		//
//////////////////////////////////

bool Wizard::StdData(Network* net, DataTable* data_table, int n_patterns, bool group) {
  StdOutputData();
  StdInputData(net, data_table, n_patterns, group);
  return true;
}

bool Wizard::StdInputData(Network* net, DataTable* data_table, int n_patterns, bool group) {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if(!data_table) {
    data_table = proj->GetNewInputDataTable("StdInputData");
  }
  if(!net) {
    net = proj->GetDefNetwork();
  }
  if(TestError(!net, "StdInputData", "could not find network to get data config from!"))
    return false;
  data_table->StructUpdate(true);
  if(group) {
    int gp_idx = 0;
    data_table->FindMakeColName("Group", gp_idx, DataTable::VT_STRING, 0);
  }
  int nm_idx = 0;
  data_table->FindMakeColName("Name", nm_idx, DataTable::VT_STRING, 0);

  UpdateInputDataFmNet(net, data_table);

  if(n_patterns > 0)
    data_table->AddRows(n_patterns);
  data_table->StructUpdate(false);
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(data_table, "BrowserSelectMe");
  }
  return true;
}

bool Wizard::UpdateInputDataFmNet(Network* net, DataTable* data_table) {
  if(TestError(!data_table || !net, "UpdateInputDataFmNet",
	       "must specify both a network and a data table")) return false;
  data_table->StructUpdate(true);
  taLeafItr li;
  Layer* lay;
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    if(lay->layer_type == Layer::HIDDEN) continue;
    int lay_idx = 0;
    //DataCol* ld = 
    if(lay->unit_groups) {
      data_table->FindMakeColName
	(lay->name, lay_idx, DataTable::VT_FLOAT, 4,
	 MAX(lay->un_geom.x,1), MAX(lay->un_geom.y,1),
	 MAX(lay->gp_geom.x,1), MAX(lay->gp_geom.y,1));
    }
    else {
      data_table->FindMakeColName
	(lay->name, lay_idx, DataTable::VT_FLOAT, 2,
	 MAX(lay->un_geom.x,1), MAX(lay->un_geom.y,1));
    }
  }
  data_table->StructUpdate(false);
//   if(taMisc::gui_active) {
//     tabMisc::DelayedFunCall_gui(data_table, "BrowserSelectMe");
//   }
  return true;
}

bool Wizard::StdOutputData() {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  // DataTable* trl_data 
  proj->GetNewOutputDataTable("TrialOutputData");
  DataTable* epc_data =
    proj->GetNewOutputDataTable("EpochOutputData");
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(epc_data, "BrowserSelectMe");
  }
  return true;
}

//////////////////////////////////
// 	Progs Wizard		//
//////////////////////////////////

bool Wizard::StdProgs() {
  TestError(true, "StdProgs", "This must be redefined in algorithm-specific project!",
	     "Just call StdProgs_impl with name of std program from prog lib");
  return false;
}

bool Wizard::TestProgs(Program* call_test_from, bool call_in_loop, int call_modulus) {
  TestError(true, "TestProgs", "This must be redefined in algorithm-specific project!",
	     "Just call StdProgs_impl with name of std program from prog lib");
  return false;
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

  Program_Group* pg = (Program_Group*)rval;
  Program* apin = pg->FindName("ApplyInputs");
  if(apin) {
    LayerWriter* lw = (LayerWriter*)apin->objs.FindType(&TA_LayerWriter);
    if(lw) {
      lw->AutoConfig();
    }
  }
  Program* first_guy = pg->Leaf(0);
  if(first_guy)
    tabMisc::DelayedFunCall_gui(first_guy, "BrowserSelectMe");
  return (Program_Group*)pg;
}

Program_Group* Wizard::TestProgs_impl(const String& prog_nm, Program* call_test_from,
				      bool call_in_loop, int call_modulus) {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);

  // make testing output data tables -- used by the test programs
  DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("OutputData");
  if(dgp->size < 4) {
    proj->GetNewOutputDataTable("TrialTestOutputData", true); // msg = true
    proj->GetNewOutputDataTable("EpochTestOutputData", true);
  }

  taBase* rval = proj->programs.NewFromLibByName(prog_nm);
  if(!rval) return NULL;
  if(!rval->InheritsFrom(&TA_Program_Group)) {
    taMisc::Error("Wizard::TestProgs_impl program named:", prog_nm,
		  "is not a group of programs -- invalid for this function");
    return NULL;
  }

  Program_Group* pg = (Program_Group*)rval;
  Program* apin = pg->FindName("ApplyInputsTest");
  if(apin) {
    LayerWriter* lw = (LayerWriter*)apin->objs.FindType(&TA_LayerWriter);
    if(lw) {
      lw->AutoConfig();
    }
  }
  Program* first_guy = pg->Leaf(0);
  if(first_guy) {
    tabMisc::DelayedFunCall_gui(first_guy, "BrowserSelectMe");
    if(call_test_from) {
      if(call_in_loop) {
	Loop* loopel = (Loop*)call_test_from->prog_code.FindType(&TA_Loop);
	if(!TestWarning(!loopel, "TestProgs", "Loop program element not found -- cannot call test program within loop!")) {
	  if(call_modulus > 1) {
	    IfElse* ife = (IfElse*)loopel->loop_code.New(1, &TA_IfElse);
	    ife->SetProgFlag(ProgEl::NEW_EL);
	    ife->cond.SetExpr("network.epoch % " + String(call_modulus) + " == 0");
	    ife->desc = "only call every n epochs -- make sure epochs is appropriate, and its also a good idea to make n a variable";

	    ProgramCall* pcall = (ProgramCall*)ife->true_code.New(1, &TA_ProgramCall);
	    pcall->SetProgFlag(ProgEl::NEW_EL);
	    pcall->target = first_guy;	    pcall->UpdateArgs();
	    pcall->desc = "call testing program";
	  }
	  else {
	    ProgramCall* pcall = (ProgramCall*)loopel->loop_code.New(1, &TA_ProgramCall);
	    pcall->SetProgFlag(ProgEl::NEW_EL);
	    pcall->target = first_guy;	    pcall->UpdateArgs();
	    pcall->desc = "call testing program";
	  }
	}
      }
      else {
	ProgramCall* pcall = (ProgramCall*)call_test_from->prog_code.New(1, &TA_ProgramCall);
	pcall->SetProgFlag(ProgEl::NEW_EL);
	pcall->target = first_guy;	pcall->UpdateArgs();
	pcall->desc = "call testing program";
      }
    }
  }
  return (Program_Group*)pg;
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
}

void ProjectBase::InitLinks_impl() {
  inherited::InitLinks_impl();
  taBase::Own(networks, this);

  // make default groups for different types of data
  data.FindMakeGpName("InputData");
  data.FindMakeGpName("OutputData");
  data.FindMakeGpName("AnalysisData");
}

void ProjectBase::CutLinks_impl() {
  // do base first, esp. to nuke viewers before the networks
  inherited::CutLinks_impl();
  networks.CutLinks();
}

void ProjectBase::Copy_(const ProjectBase& cp) {
  networks = cp.networks;

  UpdatePointers_NewPar((taBase*)&cp, this); // update all the pointers!
}

void ProjectBase::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

void ProjectBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ProjectBase::Dump_Load_pre() {
  inherited::Dump_Load_pre();
  if(taMisc::is_undo_loading) return; // none of this.
  // see comment in ta_project.cpp 
//   networks.Reset();
}

void ProjectBase::Dump_Load_post() {
  inherited::Dump_Load_post();
  AutoBuildNets();
}

void ProjectBase::SaveRecoverFile_strm(ostream& strm) {
  Network* net;
  taLeafItr i;
  FOR_ITR_EL(Network, net, networks., i) {
    net->SetNetFlag(Network::SAVE_UNITS_FORCE); // force to save units for recover file!
  }
  Save_strm(strm);
  FOR_ITR_EL(Network, net, networks., i) {
    net->ClearNetFlag(Network::SAVE_UNITS_FORCE);
  }
}

void ProjectBase::AutoBuildNets() {
  Network* net;
  taLeafItr i;
  FOR_ITR_EL(Network, net, networks., i) {
    if(net->auto_build == Network::NO_BUILD) continue;
    if(taMisc::gui_active && (net->auto_build == Network::PROMPT_BUILD)) {
      int chs = taMisc::Choice("Build network: " + net->name, "Yes", "No");
      if(chs == 1) continue;
    }
    taMisc::Info("Network:",net->name,"auto building");
    net->Build();
  }
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
    wiz->EditPanel(true);
  }
}

Network* ProjectBase::GetNewNetwork(TypeDef* typ) {
  Network* rval = (Network*)networks.New(1, typ);
// #ifdef TA_GUI
//   taiMisc::RunPending();
// #endif
  return rval;
}

Network* ProjectBase::GetDefNetwork() {
  return (Network*)networks.DefaultEl();
}


//////////////////////////
//  EmergentRoot	//
//////////////////////////

void EmergentRoot::Initialize() {
  projects.SetBaseType(&TA_ProjectBase); //note: must actually be one of the descendants
}

#ifdef TA_GUI
TAPtr EmergentRoot::Browse(const char* init_path) {
  if(!taMisc::gui_active) return NULL;

  TAPtr iob = this;
  MemberDef* md;
  if(init_path != NULL) {
    String ip = init_path;
    iob = FindFromPath(ip, md);
    if(!iob) iob = this;
  }

  taiObjChooser* chs = new taiObjChooser((TAPtr)iob, "Browse for Object", false, NULL);
  chs->Choose();
  TAPtr retv = chs->sel_obj();
  delete chs;
  return retv;
}
#endif

void EmergentRoot::About() {
  STRING_BUF(info, 2048);
  info += "Emergent Info\n";
  info += "This is the Emergent software package, version: ";
  info += version;
  info += "\n\n";
  info += "WWW Page: http://grey.colorado.edu/emergent\n";
  info += "\n\n";

  info += "Copyright (c) 1995-2006, Regents of the University of Colorado,\n\
Carnegie Mellon University, Princeton University.\n\
 \n\
Emergent is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\
 \n\
Emergent is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
 \n\
Note that the taString class was derived from the GNU String class\n\
Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and\n\
is covered by the GNU General Public License, see ta_string.h\n";

  taMisc::Choice(info, "Ok");
}


