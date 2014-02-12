// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "StdNetWizDlg.h"
#include <taGuiDialog>
#include <DataTable>
#include <ProjectBase>

#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(StdNetWizDlg);



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
  network->UpdateAfterEdit();   // update any special settings..
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
