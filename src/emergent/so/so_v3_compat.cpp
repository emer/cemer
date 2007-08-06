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


#include "so_v3_compat.h"

//////////////////////////////////
// 	V3SoProject		//
//////////////////////////////////

bool V3SoProject::ConvertToV4_impl() {
  EmergentRoot* root = (EmergentRoot*)tabMisc::root;
  SoProject* nwproj = (SoProject*)root->projects.NewEl(1, &TA_SoProject);

  nwproj->desc = desc1 + "\n" + desc2 + "\n" + desc3 + "\n" + desc4;

  SoWizard* lwiz = (SoWizard*)nwproj->wizards[0];

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
  nwproj->programs.prog_lib.NewProgramFmName("SoAll_Std", &(nwproj->programs));
//   Program_Group* progs = (Program_Group*)nwproj->programs.gp[0];
//   SoNetwork* new_net = (SoNetwork*)nwproj->networks[0];

//   if(grouped_data) {
//     Program* epc = progs->FindName("SoEpoch");
//     if(epc) {
//       epc->LoadFromProgLib(Program_Group::prog_lib.FindName("SoEpochGpData"));
//     }
//   }

  ConvertToV4_DefaultApplyInputs(nwproj);

  ConvertToV4_Edits(nwproj);

  return true;
}

