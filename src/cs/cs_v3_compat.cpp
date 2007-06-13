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

#include "cs_v3_compat.h"

void CsCycle::Initialize() {
  update_mode = SYNCHRONOUS;
  n_updates = 10;
}

void CsSettle::Initialize() {
  between_phases = INIT_STATE;
  n_units = 0;
  deterministic = false;
  start_stats = 50;
}

void CsTrial::Initialize() {
  phase = MINUS_PHASE;
  no_plus_stats = true;
  no_plus_test = true;
}

void CsSample::Initialize() {
}

void CsMaxDa::Initialize() {
}

void CsDistStat::Initialize() {
  tolerance = 0.25f;
  n_updates = 0;
}

void CsTIGstat::Initialize() {
}


void CsTargStat::Initialize() {
}

void CsGoodStat::Initialize() {
  use_netin = false;
  netin_hrmny = 0;
}


//////////////////////////////////
// 	V3CsProject		//
//////////////////////////////////

bool V3CsProject::ConvertToV4_impl() {
  PDPRoot* root = (PDPRoot*)tabMisc::root;
  CsProject* nwproj = (CsProject*)root->projects.NewEl(1, &TA_CsProject);

  nwproj->desc = desc1 + "\n" + desc2 + "\n" + desc3 + "\n" + desc4;

  CsWizard* lwiz = (CsWizard*)nwproj->wizards[0];

  lwiz->StdOutputData();	// need this for std procs

  ConvertToV4_Nets(nwproj);
  ConvertToV4_Enviros(nwproj);
  ConvertToV4_ProcScripts(nwproj);
  ConvertToV4_Scripts(nwproj);

  DataTable_Group* dgp = (DataTable_Group*)nwproj->data.FindMakeGpName("InputData");

  bool grouped_data = false;
  if(dgp->size > 1) {
    DataTable* first_env = (DataTable*)dgp->FastEl(0);
    if(first_env->data.FindName("Group"))
      grouped_data = true;
  }

  // process the new programs
  nwproj->programs.prog_lib.NewProgramFmName("CsAll_Std", &(nwproj->programs));
//   Program_Group* progs = (Program_Group*)nwproj->programs.gp[0];
//   CsNetwork* new_net = (CsNetwork*)nwproj->networks[0];

//   if(grouped_data) {
//     Program* epc = progs->FindName("CsEpoch");
//     if(epc) {
//       epc->LoadFromProgLib(Program_Group::prog_lib.FindName("CsEpochGpData"));
//     }
//   }

  ConvertToV4_DefaultApplyInputs(nwproj);

  ConvertToV4_Edits(nwproj);

  return true;
}

