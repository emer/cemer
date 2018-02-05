// Copyright 2013-2018, Regents of the University of Colorado,
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
#include <ToolBoxDockViewer>

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

#include <ProjectionSpec>

#include <LeabraNetwork>
#include <LeabraLayer>
#include <LeabraLayerSpec>
#include <LeabraUnitSpec>
#include <LeabraConSpec>

#include <BpNetwork>
#include <BpLayer>
#include <BpUnitSpec>
#include <BpConSpec>

eTypeDef_Of(ProjectBase);

#include <taMisc>
#include <tabMisc>

#include <iProgramToolBar>

TA_BASEFUNS_CTORS_DEFN(EmergentRoot);

eTypeDef_Of(Network);
eTypeDef_Of(ConSpec);


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
  taRootBase* root = taRootBase::instance();

  // these are all automatically added to root->templates from ProgEl
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetCounterInit);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetCounterIncr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetUpdateView);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_WtInitPrompt);
  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_NetDataLoop);
  
  int the_new_sec = tb->AssertSection("New"); //note: need to keep it short

  // add this for all cases:
  {
    taBase* check = root->templates.FindType(&TA_ProjectionSpec);
    if(!check) {
      root->templates.Add(new ProjectionSpec);
      TA_ProjectionSpec.tokens.name_idx_start++;
    }
  }
  
  taProject* proj = NULL;
  if(tb->viewer()) {
    proj = tb->viewer()->GetMyProj();
  }
  if(!proj) {
    taBase* check = root->templates.FindType(&TA_Network);
    if(!check) {
      root->templates.Add(new Network);
      TA_Network.tokens.name_idx_start++;
      root->templates.Add(new Layer);
      TA_Layer.tokens.name_idx_start++;
      root->templates.Add(new UnitSpec);
      TA_UnitSpec.tokens.name_idx_start++;
      root->templates.Add(new ConSpec);
      TA_ConSpec.tokens.name_idx_start++;
    }
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_Network);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_Layer);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_UnitSpec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_ConSpec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_ProjectionSpec);
  }
  else if(proj->GetTypeDef()->InheritsFromName("LeabraProject")) {
    taBase* check = root->templates.FindType(&TA_LeabraNetwork);
    if(!check) {
      root->templates.Add(new LeabraNetwork);
      TA_LeabraNetwork.tokens.name_idx_start++;
      root->templates.Add(new LeabraLayer);
      TA_LeabraLayer.tokens.name_idx_start++;
      root->templates.Add(new LeabraLayerSpec);
      TA_LeabraLayerSpec.tokens.name_idx_start++;
      root->templates.Add(new LeabraUnitSpec);
      TA_LeabraUnitSpec.tokens.name_idx_start++;
      root->templates.Add(new LeabraConSpec);
      TA_LeabraConSpec.tokens.name_idx_start++;
    }
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraNetwork);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraLayer);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraLayerSpec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraUnitSpec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_LeabraConSpec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_ProjectionSpec);
  }
  else if(proj->GetTypeDef()->InheritsFromName("BpProject")) {
    taBase* check = root->templates.FindType(&TA_BpNetwork);
    if(!check) {
      root->templates.Add(new BpNetwork);
      TA_BpNetwork.tokens.name_idx_start++;
      root->templates.Add(new BpLayer);
      TA_BpLayer.tokens.name_idx_start++;
      root->templates.Add(new BpUnitSpec);
      TA_BpUnitSpec.tokens.name_idx_start++;
      root->templates.Add(new BpConSpec);
      TA_BpConSpec.tokens.name_idx_start++;
    }
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_BpNetwork);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_BpLayer);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_BpUnitSpec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_BpConSpec);
    iProgramToolBar::ptbp_add_widget(tb, the_new_sec, &TA_ProjectionSpec);
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
