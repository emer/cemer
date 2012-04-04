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

#ifdef TA_GUI
# include "css_qt.h"
# include "ta_qt.h"
# include "ta_qtdata.h" // for taiObjChooser
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"
# include "ta_datatable_qtso.h"
# include "netstru_qtso.h"
# include "ta_gui.h"

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
  taMisc::view_colors->FindMakeViewColor("ConSpec", "Emergent Connection Spec", true, "LimeGreen");
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

/////////////////////////////////////////////////
//              StdNetWizDlg

void StdNetWizDlg::Initialize() {
  Dlg1 = NULL;
  Dlg2 = NULL;
  net_config = NULL;
  n_layers = 3;
}

void StdNetWizDlg::NewNetwork() {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if(proj) {
    network = (Network*)proj->networks.New(1);
  }
  Dlg1->Revert();
  taMisc::Info("New network created:", network->name);
}

void StdNetWizDlg::NLayersFmNetwork() {
  if(!network) {
    return;
  }
  n_layers = network->layers.leaves;
  Dlg1->Revert();
  taMisc::Info("Read n_layers from network:", String(n_layers), " network = ", network->name);
}

void StdNetWizDlg::ConfigOneLayer(int lay_no, const String& nm, const String& typ) {
  net_config->SetVal(nm, "Name", lay_no);
  net_config->SetVal(typ, "Type", lay_no);
  net_config->SetVal(5, "Size_X", lay_no);
  net_config->SetVal(5, "Size_Y", lay_no);
}

// configure default structure for a new network
void StdNetWizDlg::NewNetDefaultConfig() {
  // local variables
  int n_lays;  n_lays = 0;
  Layer* lay;  lay = NULL;
  int i;  i = 0;
  n_lays = net_config->rows;
  if(n_lays > 0) {
    ConfigOneLayer(0, "Input", "INPUT");
  }
  if(n_lays > 1) {
    ConfigOneLayer(1, "Hidden", "HIDDEN");
  }
  if(n_lays == 3) {
    ConfigOneLayer(2, "Output", "TARGET");
  }
  if(n_lays > 2) {
    ConfigOneLayer(n_lays-1, "Output", "TARGET");
    for(i=2; i<n_lays-1; i++) {
      ConfigOneLayer(i, "Hidden_" + String(i-1), "HIDDEN");
    }
  }
}

void StdNetWizDlg::AddNewLayerRow() {
  net_config->AddBlankRow();
  Dlg2->Apply();
}

void StdNetWizDlg::RefreshLayerList() {
  Dlg2->Apply();
}

bool StdNetWizDlg::DoDialog() {
  int new_net;  new_net = 0;
  String curow;
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  String mypath = GetPath();

  Dlg1 = new taGuiDialog;
  taBase::Own(Dlg1, this);
  Dlg2 = new taGuiDialog;
  taBase::Own(Dlg2, this);
  net_config = new DataTable;
  n_layers = 3;

  bool rval = false;

  Dlg1->Reset();
  Dlg1->prompt = "Network Wizard Step 1 of 2: Select Network and Number of Layers";
  Dlg1->win_title = "Network Wizard Step 1 of 2";
  Dlg1->AddWidget("main", "", "");
  Dlg1->AddVBoxLayout("mainv", "", "main", "");
  curow = "instr";
  Dlg1->AddHBoxLayout(curow, "mainv", "", "");
  Dlg1->AddLabel("Instructions", "main", curow, "label=Please select a network (or make a new one) and enter the number of layers.\nYou will then be able to configure each layer in the next dialog (press OK to continue).;");
  Dlg1->AddSpace(20, "mainv");
  curow = "netsel";
  Dlg1->AddHBoxLayout(curow, "mainv", "", "");
  Dlg1->AddObjectPtr((taBaseRef*)&network, &TA_Network, "Network", "main", curow, "tooltip=select the network to configure;");
  Dlg1->AddSpace(20, curow);
  Dlg1->AddPushButton("NewNetwork", "main", curow, "mdlg.NewNetwork()", "label=New Network; tooltip=press this button to create a new network to configure;");
  Dlg1->AddSpace(20, "mainv");
  curow = "nlayers";
  Dlg1->AddHBoxLayout(curow, "mainv", "", "");
  Dlg1->AddLabel("nlaylbl", "main", curow, "label=N Layers: ;");
  Dlg1->AddIntField(&n_layers, "n_layers", "main", curow, "tooltip=enter the number of layers to create here\n you will be able to change this later too;");
  Dlg1->AddStretch(curow);
  Dlg1->AddPushButton("NLayersFmNetwork", "main", curow, "mdlg.NLayersFmNetwork()", "label=Get N Layers From Network; tooltip=get the number of layers from the existing network;");

  Dlg1->FixAllUrl("mdlg.", mypath); // update all the urls

  int drval = Dlg1->PostDialog(true);
  if(drval == 0 || network.ptr() == NULL) {
    goto cleanup;
  }

  /*******************************************************************
  // == Dialog 2 ==
  *******************************************************************/
  if(network->layers.leaves == 0) {
    new_net = true;
  }
  network->NetStructToTable(net_config);
  net_config->RemoveCol("SendPrjns");
  net_config->EnforceRows(n_layers);
  if(new_net) {
    NewNetDefaultConfig();
  }
  Dlg2->Reset();
  Dlg2->prompt = "Network Wizard Step 2 of 2: Enter Layer Names, Sizes, and Projections (Prjns)";
  Dlg2->win_title = "Network Wizard Step 2 of 2";
  Dlg2->SetSize(900, 600);
  Dlg2->AddWidget("main", "", "", "");
  Dlg2->AddVBoxLayout("mainv", "", "main", "");
  curow = "instr";
  Dlg2->AddHBoxLayout(curow, "mainv", "", "");
  Dlg2->AddLabel("LabelTest", "main", curow, "label=Enter the names of the network layers, optionally a Group for the layers to live in, and the number of units in X, Y dimensions per layer\nand the receiving projections (layer names, separated by spaces) to connect from\nHold the mouse over the colum names for more detailed information (e.g., for Type options);");
  Dlg2->AddSpace(20, "mainv");
  curow = "dtable";
  Dlg2->AddHBoxLayout(curow, "mainv", "", "");
  Dlg2->AddDataTable(net_config, "NetworkStructure", "main", curow, "max_width=850; max_height=500;");
  curow = "buttons";
  Dlg2->AddHBoxLayout(curow, "mainv", "", "");
  Dlg2->AddToolButton("AddLayer", "main", curow, "mdlg.AddNewLayerRow()", "label=Add New Layer; tooltip=Add a new layer row to layer list (above)\n use context menu on selected rows to delete (press Refresh Layer List afterwards);");
  Dlg2->AddSpace(20, curow);
  Dlg2->AddToolButton("RefreshList", "main", curow, "mdlg.RefreshLayerList()", "label=Refresh Layer List; tooltip=updates the list of layers and their settings (above)\n if you used one of the context menu actions to insert or remove layers.\n the display updating does not work automatically in this dialog;");

  Dlg2->FixAllUrl("mdlg.", mypath); // update all the urls

  drval = Dlg2->PostDialog(true);
  if(drval == 0) {
    goto cleanup;
  }

  /*******************************************************************
  // == now build the network to spec ==
  *******************************************************************/
  network->NetStructFmTable(net_config);
  network->Build();
  network->LayerZPos_Unitize();
  network->FindMakeView();
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(network, "BrowserExpandAll");
    tabMisc::DelayedFunCall_gui(network, "BrowserSelectMe");
  }
  if(proj) {
    proj->undo_mgr.SaveUndo(network.ptr(), "Wizard::StdNetwork after -- actually saves network specifically");
  }
  rval = true;

 cleanup:
  taBase::unRefDone(Dlg1);
  taBase::unRefDone(Dlg2);
  delete net_config;

  Initialize();
  return rval;
}

//////////////////////////
//   Wizard             //
//////////////////////////

void Wizard::Initialize() {
}

void Wizard::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

void Wizard::RenderWizDoc() {
  RenderWizDoc_header();
  RenderWizDoc_impl();
  RenderWizDoc_footer();
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  String my_path = GetPath(NULL, proj);
  String projpath = proj->GetPath();
  wiz_doc.text.gsub("<this>", my_path); // shortcut for functions
  wiz_doc.text.gsub("<proj>", projpath); // shortcut for functions
  wiz_doc.UpdateText();
}

void Wizard::RenderWizDoc_impl() {
  wiz_doc.text = RenderWizDoc_intro();
  wiz_doc.text += RenderWizDoc_network();
  wiz_doc.text += RenderWizDoc_data();
  wiz_doc.text += RenderWizDoc_program();
}

String Wizard::RenderWizDoc_intro() {
  return String("\n= Emergent Wizard =\n\
This is the Emergent Wizard -- select from the options listed below.\n\n\
There are also other wizards available as programs -- use the [[<proj>.programs.NewFromLib()|programs NewFromLib]] button to create new programs from the available library, and search for the wizard keyword.\n\n\
== Standard Default Configuration ==\n\n\
Selecting these options in sequence will configure a standard project to the point where a network can be trained in a standard way.\n\n\
* [[<this>.StdEverything()|Standard Everything]] -- does the following steps one after the other:\n\
:* [[<this>.StdNetwork()|Standard Network]] -- generate or configure a standard network, specifying number of layers, layer names, sizes, types, and connectivity.\n\
:* [[<this>.StdData()|Standard Data]] -- generate or configure standard input and output data tables, based on existing network configuration -- these provide external input to the network and record statistics from the network as it runs.\n\
:* [[<this>.StdProgs()|Standard Programs]] -- install standard programs for running the network -- these programs coordinate presentation of input data and recording of output data.\n");
}

String Wizard::RenderWizDoc_network() {
  return String("\n== Network ==\n\
* [[<this>.StdNetwork()|Standard Network]] -- generate or configure a standard network, specifying number of layers, layer names, sizes, types, and connectivity.\n\
\n=== Specialized Networks ===\n\
* [[<this>.RetinaProcNetwork()|Retina Processing Network]] -- configures input layers of network to accept image processing inputs ('''NOTE: currently nonfunctional''').\n");
}

String Wizard::RenderWizDoc_data() {
  return String("\n== Data Tables ==\n\
* [[<this>.StdData()|Standard Data]] -- click this to generate or configure standard input and output data tables, based on existing network configuration -- these provide external input to the network and record statistics from the network as it runs -- calls these two functions:\n\
:* [[<this>.StdInputData()|Standard Input Data]] -- generate or configure standard input data, based on existing network configuration (creates columns for each input/output layer).\n\
:* [[<this>.StdOutputData()|Standard Output Data]] -- make standard set of output data (monitoring network performance) -- this just creates empty datatables in OutputData subgroup with names that standard programs look for.\n\
\n=== Data Utility Functions ===\n\
* [[<this>.UpdateInputDataFmNet()|Update Input Data From Network]] -- just update an existing input data datatable from a given network so that it has all the appropriate columns for the corresponding layers -- also calls Update Layer Writers.\n\
* [[<this>.UpdateLayerWriters()|Update Layer Writers]] -- update LayerWriter configuration in ApplyInputs programs to fit any changes in the network or data table -- only affects LayerWriters that are already configured to use the given network and data table.\n");
}

String Wizard::RenderWizDoc_program() {
  return String("\n== Programs ==\n\
* [[<this>.StdProgs()|Standard Programs]] -- install standard programs for running the network.\n\
* [[<this>.TestProgs()|Testing Programs]] -- create a standard set of testing programs for testing the network -- these can be configured to be called periodically during training\n");
}

////////////////////////////////////////////
//              Wiz Code: Network


bool Wizard::StdEverything() {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if(!proj) return false;
  bool rval = false;
  if(StdNetwork()) {
    Network* net = proj->networks.SafeEl(0);
    if(net) {
      if(StdData(net)) {
        rval = StdProgs();
      }
    }
  }
  return rval;
}


bool Wizard::StdNetwork() {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if(proj->networks.size == 0)  // make a new one for starters always
    proj->networks.New(1);
  bool rval = std_net_dlg.DoDialog();
  return rval;
}

bool Wizard::RetinaProcNetwork(RetinaProc* retina_spec, Network* net) {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if(!net) {
    if(TestError(!proj, "RetinaProcNetwork", "network is NULL and could not find project owner to make a new one -- aborting!")) return false;
    net = proj->GetNewNetwork();
    if(TestError(!net, "RetinaProcNetwork", "network is NULL and could not make a new one -- aborting!")) return false;
  }
  if(TestError(!retina_spec, "RetinaProcNetwork", "retina_spec is NULL -- you need to create and configure this in advance, as it is then used to configure the network!")) return false;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::RetinaProcNetwork before -- actually saves network specifically");
  }

  // todo: fixme
  net->StructUpdate(true);
  for(int i=0;i<retina_spec->regions.size; i++) {
    VisRegionSpecBase* sp = retina_spec->regions[i];
//     Layer* on_lay = net->FindMakeLayer(sp->name + "_on");
//     on_lay->un_geom.x = sp->spacing.output_size.x;
//     on_lay->un_geom.y = sp->spacing.output_size.y;
//     on_lay->un_geom.n = sp->spacing.output_units;
//     on_lay->layer_type = Layer::INPUT;
//     Layer* off_lay = net->FindMakeLayer(sp->name + "_off");
//     off_lay->un_geom.x = sp->spacing.output_size.x;
//     off_lay->un_geom.y = sp->spacing.output_size.y;
//     off_lay->un_geom.n = sp->spacing.output_units;
//     off_lay->layer_type = Layer::INPUT;
  }
  net->StructUpdate(false);
  if(taMisc::gui_active) {
    tabMisc::DelayedFunCall_gui(net, "BrowserExpandAll");
    tabMisc::DelayedFunCall_gui(net, "BrowserSelectMe");
  }
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::RetinaProcNetwork after -- actually saves network specifically");
  }

  return true;
}

//////////////////////////////////
//      Enviro Wizard           //
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
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
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

  UpdateLayerWriters(net, data_table);

  return true;
}

bool Wizard::UpdateLayerWriters(Network* net, DataTable* data_table) {
  if(TestError(!data_table || !net, "UpdateLayerWriters",
               "must specify both a network and a data table")) return false;
  TypeDef* td = &TA_LayerWriter;
  for(int i=0; i<td->tokens.size; i++) {
    LayerWriter* lw = (LayerWriter*)td->tokens.FastEl(i);
    if(!lw) continue;
    if(!(lw->network.ptr() == net && lw->data.ptr() == data_table)) continue;
    lw->AutoConfig(true);
  }
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
//      Progs Wizard            //
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
            pcall->target = first_guy;      pcall->UpdateArgs();
            pcall->desc = "call testing program";
          }
          else {
            ProgramCall* pcall = (ProgramCall*)loopel->loop_code.New(1, &TA_ProgramCall);
            pcall->SetProgFlag(ProgEl::NEW_EL);
            pcall->target = first_guy;      pcall->UpdateArgs();
            pcall->desc = "call testing program";
          }
        }
      }
      else {
        ProgramCall* pcall = (ProgramCall*)call_test_from->prog_code.New(1, &TA_ProgramCall);
        pcall->SetProgFlag(ProgEl::NEW_EL);
        pcall->target = first_guy;      pcall->UpdateArgs();
        pcall->desc = "call testing program";
      }
    }
  }
  return (Program_Group*)pg;
}

bool Wizard::FixOldProgs() {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  FOREACH_ELEM_IN_GROUP(Program, prg, proj->programs) {
    if(prg->name.contains("Apply") || prg->name.contains("Input")) {
      prg->SetProgFlag(Program::NO_STOP_STEP);
      prg->short_nm = "AplyIn";
    }
    else if(prg->name.contains("Mon")) {
      prg->SetProgFlag(Program::NO_STOP_STEP);
      if(prg->name.contains("Trial"))
        prg->short_nm = "TrlMon";
      else if(prg->name.contains("Epoch"))
        prg->short_nm = "EpcMon";
    }
    else if(prg->name.contains("Save")) {
      prg->SetProgFlag(Program::NO_STOP_STEP);
      prg->short_nm = "SvWts";
    }
    else if(prg->name.contains("Startup")) {
      prg->SetProgFlag(Program::NO_STOP_STEP);
      prg->short_nm = "Start";
    }
  }
  return true;
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
  FOREACH_ELEM_IN_GROUP(Stat, sst, tproc->loop_stats) {
    if(!sst->InheritsFrom(&TA_SE_Stat) || (sst->time_agg.from == NULL)) continue;
    Stat* fst = sst->time_agg.from;
    if(fst->GetMySchedProc() == cve) {
      ((SE_Stat*)sst)->se.stopcrit.flag = true; // stop on this one
    }
  }
  return cve;
}

//////////////////////////////////
//      Stats Wizard            //
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
  taBase::SetPointer((taBase**)&cte->stat, stat);
  if(cte->data_env == NULL) {
    Environment* env = pdpMisc::GetNewEnv(proj, &TA_Environment);
    taBase::SetPointer((taBase**)&cte->data_env, env);
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
  FOREACH_ELEM_IN_GROUP(Process, pr, *tgp) {
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
  taBase::SetPointer((taBase**)&ddep->data_env, data_env);
  ddep->pat_no = pat_no;
  ddep->disp_type = disp_type;
  ddep->C_Code();               // run it!
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
  taBase::SetPointer((taBase**)&rfs->layer, recv_layer);
  if(send_layer != NULL)
    rfs->rf_layers.LinkUnique(send_layer);
  if(send2_layer != NULL)
    rfs->rf_layers.LinkUnique(send2_layer);
  if(rfs->data_env == NULL) {
    Environment* env = pdpMisc::GetNewEnv(proj, &TA_Environment);
    taBase::SetPointer((taBase**)&rfs->data_env, env);
  }
  rfs->InitRFVals();
  Process_Group* reset_gp = disp_proc->GetProcGroup(SchedProcess::INIT_PROCS);
  UnitActRFStatResetProc* rrf = (UnitActRFStatResetProc*)reset_gp->FindMakeProc(NULL, &TA_UnitActRFStatResetProc);
  taBase::SetPointer((taBase**)&rrf->unit_act_rf_stat, rfs);
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
  taBase::SetPointer((taBase**)&rts->layer, lay);
  rts->act_thresh = thresh;
  rts->max_act.stopcrit.flag = true;
  rts->UpdateAfterEdit();
  taMisc::DelayedMenuUpdate(proc);
}

void Wizard::AddCountersToTest(SchedProcess* te_proc, SchedProcess* tr_proc) {
  if((te_proc == NULL) || (tr_proc == NULL)) return;
  Stat_Group* sgp = (Stat_Group*)&(te_proc->final_stats);
  ProcCounterStat* st = (ProcCounterStat*)sgp->FindMakeStat(&TA_ProcCounterStat);
  taBase::SetPointer((taBase**)&st->proc, tr_proc);
  st->UpdateAfterEdit();
  taMisc::DelayedMenuUpdate(te_proc);
}

void Wizard::GetStatsFromProc(SchedProcess* sproc, SchedProcess* tproc, SchedProcess::StatLoc tloc, Aggregate::Operator agg_op) {
  if((sproc == NULL) || (tproc == NULL)) return;
  FOREACH_ELEM_IN_GROUP(Stat, sst, sproc->loop_stats) {
    Stat_Group* tgp = tproc->GetStatGroup(sst->GetTypeDef(), tloc);
    Stat* trst = tgp->FindAggregator(sst, agg_op);
    if(trst == NULL)
      tproc->MakeAggregator(sst, tloc, agg_op);
  }
  FOREACH_ELEM_IN_GROUP(Stat, sst, sproc->final_stats) {
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
  taBase::SetPointer((taBase**)&rproc->time_ctr_stat, st);
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
//  ProjectBase         //
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

void ProjectBase::InitLinks_post() {
  if(!taMisc::is_loading) {     // only if not loading!
    taWizard* wiz = wizards.SafeEl(0);
    if (!wiz) {
      wiz = (Wizard*)wizards.New(1, wizards.el_typ);
    }
  }
  inherited::InitLinks_post();
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

void ProjectBase::Dump_Load_post() {
  //  inherited::Dump_Load_post(); -- don't do this -- need to do in correct order
  taFBase::Dump_Load_post();          // parent of taProject
  if(taMisc::is_undo_loading) return; // none of this.
  OpenProjectLog();
  DoView();
  taVersion v502(5, 0, 2);
  if(taMisc::loading_version < v502) { // fix old programs for < 5.0.2
    Wizard* wiz = (Wizard*)wizards.SafeEl(0);
    if(wiz) wiz->FixOldProgs();
  }
  AutoBuildNets();
  setDirty(false);              // nobody should start off dirty!
  if(!taMisc::interactive) {
    bool startup_run = programs.RunStartupProgs();      // run startups as last step..
    if(!taMisc::gui_active && startup_run) taiMC_->Quit();
  }
}

void ProjectBase::SaveRecoverFile_strm(ostream& strm) {
  // really there is no compelling reason to force it to save units if project otherwise does not
//   FOREACH_ELEM_IN_GROUP(Network, net, networks) {
//     net->SetNetFlag(Network::SAVE_UNITS_FORCE); // force to save units for recover file!
//   }
  Save_strm(strm);
//   FOREACH_ELEM_IN_GROUP(Network, net, networks) {
//     net->ClearNetFlag(Network::SAVE_UNITS_FORCE);
//   }
}

void ProjectBase::AutoBuildNets() {
  FOREACH_ELEM_IN_GROUP(Network, net, networks) {
    if(net->auto_build == Network::NO_BUILD) continue;
    if(taMisc::gui_active && (net->auto_build == Network::PROMPT_BUILD)) {
      int chs = taMisc::Choice("Build network: " + net->name, "Yes", "No");
      if(chs == 1) continue;
    }
    taMisc::Info("Network:",net->name,"auto building");
    net->Build();
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
//  EmergentRoot        //
//////////////////////////

void EmergentRoot::Initialize() {
  projects.SetBaseType(&TA_ProjectBase); //note: must actually be one of the descendants
}

#ifdef TA_GUI
taBase* EmergentRoot::Browse(const char* init_path) {
  if(!taMisc::gui_active) return NULL;

  taBase* iob = this;
  MemberDef* md;
  if(init_path != NULL) {
    String ip = init_path;
    iob = FindFromPath(ip, md);
    if(!iob) iob = this;
  }

  taiObjChooser* chs = new taiObjChooser((taBase*)iob, "Browse for Object", false, NULL);
  chs->Choose();
  taBase* retv = chs->sel_obj();
  delete chs;
  return retv;
}
#endif

void EmergentRoot::About() {
  STRING_BUF(info, 2048);
  info += "Emergent Info\n";
  info += "This is the Emergent software package, version: ";
  info += version;
  info += " (" + taMisc::svn_rev + ")\n";
  info += "http://grey.colorado.edu/emergent\n";
  info += "\n";
  info +=
    "Copyright (c) 1995-2011, Regents of the University of Colorado, "
    "Carnegie Mellon University, Princeton University.\n"
    "\n"
    "Emergent is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.\n"
    "\n"
    "Emergent is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.\n"
    "\n"
    "Note that the taString class was derived from the GNU String class "
    "Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and "
    "is covered by the GNU General Public License, see ta_string.h\n"
    "\n"
    "The talairach.nii file is from the Talairach Daemon software. "
    "For more information, see http://www.talairach.org\n";

  if(taMisc::interactive)
    taMisc::Choice(info, "Ok");
  else
    taMisc::Info(info);
}
