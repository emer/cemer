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


#include "bp_v3_compat.h"

//////////////////////////
//  	Procs		//
//////////////////////////

void BpTrial::Initialize() {
//nn  min_unit = &TA_BpUnit;
//  min_con_group = &TA_BpCon_Group;
//  min_con = &TA_BpCon;
  bp_to_inputs = false;
}


//////////////////////////
// 	CE_Stat		//
//////////////////////////

void CE_Stat::Initialize() {
  tolerance = 0.0f;
}

void CE_Stat::NameStatVals() {
  Stat::NameStatVals();
  ce.AddDispOption("MIN=0");
}

void CE_Stat::InitStat() {
  float init_val = InitStatVal();
  ce.InitStat(init_val);
  InitStat_impl();
}

void CE_Stat::Init() {
  ce.Init();
  Init_impl();
}


/////////////////////////////////
//        NormDotProd_Stat     //
/////////////////////////////////

void NormDotProd_Stat::Initialize(){
//  ndp = 0.0f;
  net_agg.op = Aggregate::AVG;
}


void NormDotProd_Stat::NameStatVals() {
  Stat::NameStatVals();
  ndp.AddDispOption("MIN=0");
}

void NormDotProd_Stat::InitStat() {
  float init_val = InitStatVal();
  ndp.InitStat(init_val);
  InitStat_impl();
}

void NormDotProd_Stat::Init() {
  ndp.Init();
  Init_impl();
}


////////////////////////////
//        VecCor_Stat     //
////////////////////////////

void VecCor_Stat::Initialize(){
//nn  vcor = 0.0f;
  dp = l1 = l2 =  0.0f;
}


void VecCor_Stat::NameStatVals() {
  Stat::NameStatVals();
  vcor.AddDispOption("MIN=0");
}

void VecCor_Stat::InitStat() {
  float init_val = InitStatVal();
  vcor.InitStat(init_val);
  dp = l1 = l2 =  0.0f;
  InitStat_impl();
}

void VecCor_Stat::Init() {
  vcor.Init();
  Init_impl();
}


////////////////////////////////
//        NormVecLen_Stat     //
////////////////////////////////

void NormVecLen_Stat::Initialize(){
//nn  nvl = 0.0f;
  net_agg.op = Aggregate::AVG;
}


void NormVecLen_Stat::NameStatVals() {
  Stat::NameStatVals();
  nvl.AddDispOption("MIN=0");
}

void NormVecLen_Stat::InitStat() {
  float init_val = InitStatVal();
  nvl.InitStat(init_val);
  InitStat_impl();
}

void NormVecLen_Stat::Init() {
  nvl.Init();
  Init_impl();
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

/*nn  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon; */
}

void RBpTrial::InitLinks() {
  TrialProcess::InitLinks();
}

void RBpTrial::UpdateAfterEdit() {
  BpTrial::UpdateAfterEdit();
//  GetUnitBufSize(false);
  time_win_ticks = (int)ceil((float)time_window / dt);
  bp_gap_ticks = (int)ceil((float)bp_gap / dt);
}

void RBpTrial::Init_impl() {
  BpTrial::Init_impl();
  bp_performed = false;
}



//////////////////////////
// 	RBpSE_Stat	//
//////////////////////////

void RBpSE_Stat::Initialize() {
//nn  min_unit = &TA_RBpUnit;
}


//////////////////////////////////////////
//	Almeida-Pineda Algorithm	//
//////////////////////////////////////////


//////////////////////////
// 	CycleProcess	//
//////////////////////////

void APBpCycle::Initialize() {
  sub_proc_type = NULL;
  apbp_settle = NULL;
  apbp_trial = NULL;

/*nn  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon; */
}

void APBpCycle::CutLinks() {
  CycleProcess::CutLinks();
  apbp_settle = NULL;
  apbp_trial = NULL;
}

void APBpCycle::UpdateAfterEdit() {
  CycleProcess::UpdateAfterEdit();
  apbp_settle = (APBpSettle*)FindSuperProc(&TA_APBpSettle);
  apbp_trial = (APBpTrial*)FindSuperProc(&TA_APBpTrial);
}


//////////////////////////
// 	SettleProcess	//
//////////////////////////

void APBpSettle::Initialize() {
  sub_proc_type = &TA_APBpCycle;
  apbp_trial = NULL;
  cycle.max = 50;

/*nn  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon; */
}

void APBpSettle::InitLinks() {
  SettleProcess::InitLinks();
}

void APBpSettle::CutLinks() {
  SettleProcess::CutLinks();
  apbp_trial = NULL;
}

void APBpSettle::UpdateAfterEdit() {
  SettleProcess::UpdateAfterEdit();
  apbp_trial = (APBpTrial*)FindSuperProc(&TA_APBpTrial);
}

void APBpSettle::Init_impl() {
  SettleProcess::Init_impl();
  if((apbp_trial == NULL) || (network == NULL))
    return;

  if((apbp_trial->cur_event != NULL) && apbp_trial->cur_event->InheritsFrom(TA_DurEvent))
    cycle.SetMax((int)((DurEvent*)apbp_trial->cur_event)->duration);

}

//////////////////////////
// 	TrialProcess	//
//////////////////////////

void APBpTrial::Initialize() {
  sub_proc_type = &TA_APBpSettle;
  phase = ACT_PHASE;
  phase_no.SetMax(2);
  no_bp_stats = true;
  no_bp_test = true;
  trial_init = INIT_STATE;

/*nn  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon; */
}

void APBpTrial::InitLinks() {
  TrialProcess::InitLinks();
  taBase::Own(phase_no, this);
}

void APBpTrial::Copy_(const APBpTrial& cp) {
  phase_no = cp.phase_no;
  phase = cp.phase;
  trial_init = cp.trial_init;
  no_bp_stats = cp.no_bp_stats;
  no_bp_test = cp.no_bp_test;
}

void APBpTrial::Init_impl() {
  if(network == NULL)	return;
  TrialProcess::Init_impl();
  phase_no.SetMax(2);
  bool is_testing = false;
  if(no_bp_test && (epoch_proc != NULL) &&
     (epoch_proc->wt_update == EpochProcess::TEST))
  {
    phase_no.SetMax(1);		// just do one loop (the minus phase)
    is_testing = true;
  }
  phase = ACT_PHASE;

}


//////////////////////////
// 	Stats		//
//////////////////////////

void APBpMaxDa_De::Initialize() {
  net_agg.op = Aggregate::MAX;
  da_de.stopcrit.flag = true;	// defaults
  da_de.stopcrit.val = .01f;
  da_de.stopcrit.cnt = 1;
  da_de.stopcrit.rel = CritParam::LESSTHANOREQUAL;
  has_stop_crit = true;
  net_agg.op = Aggregate::MAX;
  loop_init = INIT_START_ONLY;

//nn  min_unit = &TA_RBpUnit;
}

void APBpMaxDa_De::InitStat() {
  da_de.InitStat(InitStatVal());
  InitStat_impl();
}

void APBpMaxDa_De::Init() {
  da_de.Init();
  Init_impl();
}

