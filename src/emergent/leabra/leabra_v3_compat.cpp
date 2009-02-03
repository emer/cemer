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


#include "leabra_v3_compat.h"

#include "ta_viewer.h"

//////////////////////////
// 	Max Da Stat	//
//////////////////////////

void LeabraMaxDa::Initialize() {
  min_layer = &TA_LeabraLayer;
  min_unit = &TA_LeabraUnit;
  net_agg.op = Aggregate::MAX;
  has_stop_crit = true;
  loop_init = INIT_START_ONLY;

  da_type = INET_DA;
  inet_scale = 1.0f;
  lay_avg_thr = .01f;

  da.stopcrit.flag = true;	// defaults
  da.stopcrit.val = .005f;
  da.stopcrit.cnt = 1;
  da.stopcrit.rel = CritParam::LESSTHANOREQUAL;

  trg_max_act.stopcrit.flag = false;	// defaults
  trg_max_act.stopcrit.val = .85f;
  trg_max_act.stopcrit.cnt = 1;
  trg_max_act.stopcrit.rel = CritParam::GREATERTHANOREQUAL;
}


//////////////////////////
//  	Ae Stat		//
//////////////////////////

void LeabraSE_Stat::Initialize() {
  targ_or_comp = Unit::COMP_TARG;
  trial_proc = NULL;
  tolerance = .5;
  no_off_err = false;
}


////////////////////////////////
// 	LeabraGoodStat        //
////////////////////////////////

void LeabraGoodStat::Initialize() {
  min_layer = &TA_LeabraLayer;
  min_unit = &TA_LeabraUnit;
  net_agg.op = Aggregate::MEAN;
  subtr_inhib = false;
}

////////////////////////////////
// 	LeabraSharpStat        //
////////////////////////////////

void LeabraSharpStat::Initialize() {
  min_layer = &TA_LeabraLayer;
  net_agg.op = Aggregate::MEAN;
}

//////////////////////////
// 	WrongOnStat	//
//////////////////////////

void WrongOnStat::Initialize() {
  threshold = .5f;
  trg_lay = NULL;
}


////////////////////////////////////////
// 	LeabraPrjnRelNetinStat        //
////////////////////////////////////////

void LeabraPrjnRelNetinStat::Initialize() {
  min_layer = &TA_LeabraLayer;
  min_con_group = &TA_LeabraRecvCons;
  net_agg.op = Aggregate::GROUP;
  recv_act_thr = .1f;
}

//////////////////////////////////
// 	ExtRew_Stat		//
//////////////////////////////////

void ExtRew_Stat::Initialize() {
}

void PhaseOrderEventSpec::Initialize() {
  phase_order = MINUS_PLUS;
}

//////////////////////////////////
// 	V3LeabraProject		//
//////////////////////////////////

bool V3LeabraProject::ConvertToV4_impl() {
  EmergentRoot* root = (EmergentRoot*)tabMisc::root;
  LeabraProject* nwproj = (LeabraProject*)root->projects.NewEl(1, &TA_LeabraProject);

  nwproj->desc = desc1 + "\n" + desc2 + "\n" + desc3 + "\n" + desc4;

  LeabraWizard* lwiz = (LeabraWizard*)nwproj->wizards[0];

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
  nwproj->programs.prog_lib.NewProgramFmName("LeabraAll_Std", &(nwproj->programs));
  Program_Group* progs = (Program_Group*)nwproj->programs.gp[0];
  LeabraNetwork* new_net = (LeabraNetwork*)nwproj->networks[0];

  if(grouped_data) {
    Program* epc = progs->FindName("LeabraEpoch");
    if(epc) {
      epc->LoadFromProgLib(Program_Group::prog_lib.FindName("LeabraEpochGpData"));
    }
  }

  ConvertToV4_DefaultApplyInputs(nwproj);

  ConvertToV4_Edits(nwproj);

  LeabraSettle* old_settle = (LeabraSettle*)processes.FindLeafType(&TA_LeabraSettle);
  if(old_settle && new_net) {
    new_net->cycle_max = old_settle->cycle.max;
  }

  LeabraTrial* old_trial = (LeabraTrial*)processes.FindLeafType(&TA_LeabraTrial);
  if(old_trial && new_net) {
    new_net->phase_order = (LeabraNetwork::PhaseOrder)old_trial->phase_order;
    new_net->no_plus_test = old_trial->no_plus_test;
  }

  // todo: make a standard leabra process for each process group

  return true;
}

