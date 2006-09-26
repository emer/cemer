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


#include "leabra_v3_compat.h"

#include "ta_viewer.h"

//////////////////////////
// 	Max Da Stat	//
//////////////////////////

void LeabraMaxDa::Initialize() {
  settle_proc = NULL;
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
  net_agg.op = Aggregate::AVG;
  subtr_inhib = false;
}

////////////////////////////////
// 	LeabraSharpStat        //
////////////////////////////////

void LeabraSharpStat::Initialize() {
  min_layer = &TA_LeabraLayer;
  net_agg.op = Aggregate::AVG;
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
  min_con_group = &TA_LeabraCon_Group;
  net_agg.op = Aggregate::LAST;
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
  PDPRoot* root = (PDPRoot*)tabMisc::root;
  LeabraProject* nwproj = (LeabraProject*)root->projects.NewEl(1, &TA_LeabraProject);

  ConvertToV4_Nets(nwproj);
  ConvertToV4_Enviros(nwproj);

  DataTable* mon_data = (DataTable*)nwproj->data.NewEl(1,&TA_DataTable);
  mon_data->name = "mon_data";

  nwproj->programs.prog_lib.NewProgramFmName("LeabraAll_Std", &(nwproj->programs));

  ConvertToV4_DefaultApplyInputs(nwproj);

  // todo: copy network params from processes
  // todo: make a standard leabra process for each process group

  return true;
}

