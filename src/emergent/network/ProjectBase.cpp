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

#include "ProjectBase.h"
#include <Wizard>

#include <taiMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ProjectBase);


using namespace std;


void ProjectBase::Initialize() {
  // up-class a few of the bases
  wizards.SetBaseType(&TA_Wizard);
  // now the rest
}

void ProjectBase::InitLinks_impl() {
  inherited::InitLinks_impl();
  taBase::Own(networks, this);

  networks.name = "networks";

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

  networks.name = "networks";
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

