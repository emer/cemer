// Copyright 2014, Regents of the University of Colorado,
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

#include "cs_v3_compat.h"

TA_BASEFUNS_CTORS_DEFN(V3CsProject);

TA_BASEFUNS_CTORS_DEFN(CsCycle);

TA_BASEFUNS_CTORS_DEFN(CsSettle);

TA_BASEFUNS_CTORS_DEFN(CsTrial);

TA_BASEFUNS_CTORS_DEFN(CsSample);

TA_BASEFUNS_CTORS_DEFN(CsMaxDa);

TA_BASEFUNS_CTORS_DEFN(CsDistStat);

TA_BASEFUNS_CTORS_DEFN(CsTIGstat);

TA_BASEFUNS_CTORS_DEFN(CsTargStat);

TA_BASEFUNS_CTORS_DEFN(CsGoodStat);

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
  EmergentRoot* root = (EmergentRoot*)tabMisc::root;
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
  CsNetwork* new_net = (CsNetwork*)nwproj->networks[0];

//   if(grouped_data) {
//     Program* epc = progs->FindName("CsEpoch");
//     if(epc) {
//       epc->LoadFromProgLib(Program_Group::prog_lib.FindName("CsEpochGpData"));
//     }
//   }

  ConvertToV4_DefaultApplyInputs(nwproj);

  ConvertToV4_Edits(nwproj);

  CsCycle* old_cycle = (CsCycle*)processes.FindLeafType(&TA_CsCycle);
  if(old_cycle && new_net) {
    new_net->update_mode = (CsNetwork::UpdateMode)old_cycle->update_mode;
    new_net->n_updates = old_cycle->n_updates;
  }

  CsSettle* old_settle = (CsSettle*)processes.FindLeafType(&TA_CsSettle);
  if(old_settle && new_net) {
    new_net->cycle_max = old_settle->cycle.max;
    new_net->deterministic = old_settle->deterministic;
    new_net->start_stats = old_settle->start_stats;
    new_net->between_phases = (CsNetwork::StateInit)old_settle->between_phases;
  }

  CsTrial* old_trial = (CsTrial*)processes.FindLeafType(&TA_CsTrial);
  if(old_trial && new_net) {
    new_net->trial_init = (CsNetwork::StateInit)old_trial->trial_init;
  }
  return true;
}

