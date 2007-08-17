// Copyright, 1995-2005, Regents of the University of Colorado,
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


#include "bp_v3_compat.h"

//////////////////////////
//  	Procs		//
//////////////////////////

void BpTrial::Initialize() {
  min_unit = &TA_BpUnit;
  min_con_group = &TA_BpRecvCons;
  min_con = &TA_BpCon;
  bp_to_inputs = false;
}

void CE_Stat::Initialize() {
  tolerance = 0.0f;
}

void NormDotProd_Stat::Initialize(){
//  ndp = 0.0f;
  net_agg.op = Aggregate::MEAN;
}

void VecCor_Stat::Initialize(){
//nn  vcor = 0.0f;
  dp = l1 = l2 =  0.0f;
}

void NormVecLen_Stat::Initialize(){
//nn  nvl = 0.0f;
  net_agg.op = Aggregate::MEAN;
}


//////////////////////////
//  	RBpTrial	//
//////////////////////////

void RBpTrial::Initialize() {
  time = 0.0f;
  dt = 1.0f;
  time_window = 10;
  bp_gap = 1;
  real_time = false;
  time_win_ticks = 10;
  bp_gap_ticks = 1;
  bp_performed = false;
  log_counter = true;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpRecvCons;
  min_con = &TA_BpCon;
}

void RBpSE_Stat::Initialize() {
  min_unit = &TA_RBpUnit;
}


//////////////////////////////////////////
//	Almeida-Pineda Algorithm	//
//////////////////////////////////////////


void APBpCycle::Initialize() {
  sub_proc_type = NULL;
  apbp_settle = NULL;
  apbp_trial = NULL;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpRecvCons;
  min_con = &TA_BpCon;
}

void APBpSettle::Initialize() {
  sub_proc_type = &TA_APBpCycle;
  apbp_trial = NULL;
  cycle.max = 50;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpRecvCons;
  min_con = &TA_BpCon;
}

void APBpTrial::Initialize() {
  sub_proc_type = &TA_APBpSettle;
  phase = ACT_PHASE;
  phase_no.SetMax(2);
  no_bp_stats = true;
  no_bp_test = true;
  trial_init = INIT_STATE;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpRecvCons;
  min_con = &TA_BpCon;
}

void APBpMaxDa_De::Initialize() {
  net_agg.op = Aggregate::MAX;
  da_de.stopcrit.flag = true;	// defaults
  da_de.stopcrit.val = .01f;
  da_de.stopcrit.cnt = 1;
  da_de.stopcrit.rel = CritParam::LESSTHANOREQUAL;
  has_stop_crit = true;
  net_agg.op = Aggregate::MAX;
  loop_init = INIT_START_ONLY;

  min_unit = &TA_RBpUnit;
}


//////////////////////////////////
// 	V3BpProject		//
//////////////////////////////////

bool V3BpProject::ConvertToV4_impl() {
  EmergentRoot* root = (EmergentRoot*)tabMisc::root;
  BpProject* nwproj = (BpProject*)root->projects.NewEl(1, &TA_BpProject);

  nwproj->desc = desc1 + "\n" + desc2 + "\n" + desc3 + "\n" + desc4;

  BpWizard* lwiz = (BpWizard*)nwproj->wizards[0];

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
  nwproj->programs.prog_lib.NewProgramFmName("BpAll_Std", &(nwproj->programs));
//   Program_Group* progs = (Program_Group*)nwproj->programs.gp[0];
//   BpNetwork* new_net = (BpNetwork*)nwproj->networks[0];

//   if(grouped_data) {
//     Program* epc = progs->FindName("BpEpoch");
//     if(epc) {
//       epc->LoadFromProgLib(Program_Group::prog_lib.FindName("BpEpochGpData"));
//     }
//   }

  ConvertToV4_DefaultApplyInputs(nwproj);

  ConvertToV4_Edits(nwproj);

  return true;
}

