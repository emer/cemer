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

#include "EmergentRoot.h"
#include <iDialogObjChooser>
#include <iToolBoxDockViewer>
#include <ViewColor_List>
#include <KeyBindings_List>
#include <KeyBindings>
#include <iBaseClipWidgetAction>
#include <ToolBoxRegistrar>

#include <NetCounterInit>
#include <NetCounterIncr>
#include <NetUpdateView>
#include <WtInitPrompt>

#include <NetDataLoop>
#include <NetGroupedDataLoop>

//#include <Network>
//#include <Layer>
//#include <LayerSpec>
//#include <UnitSpec>
//#include <ProjectionSpec>
//#include <ConSpec>

#include <LeabraNetwork>
#include <LeabraLayer>
#include <LeabraLayerSpec>
#include <LeabraUnitSpec>
#include <ProjectionSpec>
#include <LeabraConSpec>

eTypeDef_Of(ProjectBase);

#include <taMisc>
#include <tabMisc>

#include <iProgramToolBar>

TA_BASEFUNS_CTORS_DEFN(EmergentRoot);

taTypeDef_Of(Network);
taTypeDef_Of(ConSpec);


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

void EmergentRoot::Initialize() {
  projects.SetBaseType(&TA_ProjectBase); //note: must actually be one of the descendants
}

void PDPProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("Network"); //note: need to keep it short
  NetCounterInit* init = new NetCounterInit;
  taRootBase::instance()->templates.Add(init);
  NetCounterIncr* incr = new NetCounterIncr;
  taRootBase::instance()->templates.Add(incr);
  NetUpdateView* view = new NetUpdateView;
  taRootBase::instance()->templates.Add(view);
  WtInitPrompt* prompt = new WtInitPrompt;
  taRootBase::instance()->templates.Add(prompt);
  NetDataLoop* loop = new NetDataLoop;
  taRootBase::instance()->templates.Add(loop);
  
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetCounterInit);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetCounterIncr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetUpdateView);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_WtInitPrompt);
  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetDataLoop);
  
  int the_new_sec = tb->AssertSection("New"); //note: need to keep it short
  
//  Network* net = new Network;
//  taRootBase::instance()->templates.Add(net);
//  iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_Network);
//  
//  Layer* layer = new Layer;
//  taRootBase::instance()->templates.Add(layer);
//  iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_Layer);
//  
//  LayerSpec* layer_spec = new LayerSpec;
//  taRootBase::instance()->templates.Add(layer_spec);
//  iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LayerSpec);
//  
//  UnitSpec* unit_spec = new UnitSpec;
//  taRootBase::instance()->templates.Add(unit_spec);
//  iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_UnitSpec);
//  
//  ProjectionSpec* proj_spec = new ProjectionSpec;
//  taRootBase::instance()->templates.Add(proj_spec);
//  iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_ProjectionSpec);
//  
//  ConSpec* con_spec = new ConSpec;
//  taRootBase::instance()->templates.Add(con_spec);
//  iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_ConSpec);
//  
//  tb->AddSeparator(the_new_sec);

  {  // scope to reuse names
    LeabraNetwork* net = new LeabraNetwork;
    taRootBase::instance()->templates.Add(net);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraNetwork);
    
    LeabraLayer* leabra_layer = new LeabraLayer;
    taRootBase::instance()->templates.Add(leabra_layer);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraLayer);
    
    LeabraLayerSpec* layer_spec = new LeabraLayerSpec;
    taRootBase::instance()->templates.Add(layer_spec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraLayerSpec);
    
    LeabraUnitSpec* unit_spec = new LeabraUnitSpec;
    taRootBase::instance()->templates.Add(unit_spec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraUnitSpec);
    
    ProjectionSpec* proj_spec = new ProjectionSpec;
    taRootBase::instance()->templates.Add(proj_spec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_ProjectionSpec);
    
    LeabraConSpec* con_spec = new LeabraConSpec;
    taRootBase::instance()->templates.Add(con_spec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraConSpec);
  }
}

ToolBoxRegistrar emergent_ptb(PDPProgramToolBoxProc);

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

  iDialogObjChooser* chs = new iDialogObjChooser((taBase*)iob, "Browse for Object", false, NULL);
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
  info += "https://grey.colorado.edu/emergent\n";
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
