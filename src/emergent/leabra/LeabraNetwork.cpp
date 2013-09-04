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

#include "LeabraNetwork.h"
#include <taProject>


//////////////////////////
//      Network         //
//////////////////////////

void LeabraNetMisc::Initialize() {
  cyc_syn_dep = false;
  syn_dep_int = 20;
}

void CtTrialTiming::Initialize() {
  use = false;			// has to be false for old projects, to get CHL default right -- Ct learn rules will automatically turn on anyway
  minus = 50;
  plus = 20;
  inhib = 1;
  n_avg_only_epcs = 0;

  total_cycles = minus + plus + inhib;
  inhib_start = minus + plus;
}

void CtTrialTiming::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  total_cycles = minus + plus + inhib;
  inhib_start = minus + plus;
}

void CtSRAvgSpec::Initialize() {
  manual = false;
  start = 30;
  end = 1;
  interval = 1;
  plus_s_st = 19;
  plus_s_only = false;
  force_con = false;
}

void CtSineInhibMod::Initialize() {
  start = 30;
  duration = 20;
  n_pi = 2.0f;
  burst_i = 0.0f;
  trough_i = 0.0f;
}

void CtFinalInhibMod::Initialize() {
  start = 20;
  end = 25;
  inhib_i = 0.0f;
}

void CtLrnTrigSpec::Initialize() {
  plus_lrn_cyc = -1;
  davg_dt = 0.1f;
  davg_s_dt = 0.05f;
  davg_m_dt = 0.03f;
  davg_l_dt = 0.0005f;
  thr_min = 0.0f;
  thr_max = 0.5f;
  loc_max_cyc = 8;
  loc_max_dec = 0.01f;
  lrn_delay = 40;
  lrn_refract = 100;
  davg_l_init = 0.0f;
  davg_max_init = 0.001f;

  davg_time = 1.0f / davg_dt;
  davg_s_time = 1.0f / davg_s_dt;
  davg_m_time = 1.0f / davg_m_dt;
  davg_l_time = 1.0f / davg_l_dt;

  lrn_delay_inc = 1.0f / MAX(1.0f, (float)lrn_delay);
  lrn_refract_inc = 1.0f / MAX(1.0f, (float)lrn_refract);
}

void CtLrnTrigSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  davg_time = 1.0f / davg_dt;
  davg_s_time = 1.0f / davg_s_dt;
  davg_m_time = 1.0f / davg_m_dt;
  davg_l_time = 1.0f / davg_l_dt;

  lrn_delay_inc = 1.0f / MAX(1.0f, (float)lrn_delay);
  lrn_refract_inc = 1.0f / MAX(1.0f, (float)lrn_refract);
}

void CtLrnTrigVals::Initialize() {
  davg = 0.0f;
  davg_s = 0.0f;
  davg_m = 0.0f;
  davg_smd = 0.0f;
  davg_l = 0.0f;
  davg_max = 0.001f;
  cyc_fm_inc = 0;
  cyc_fm_dec = 0;
  loc_max = 0.001f;
  lrn_max = 0.0f;
  lrn_trig = 0.0f;
  lrn = 0;

  Init_Stats();
}

void CtLrnTrigVals::Init_Stats() {
  lrn_min = 0.0f;
  lrn_min_cyc = 0.0f;
  lrn_min_thr = 0.0f;

  lrn_plus = 0.0f;
  lrn_plus_cyc = 0.0f;
  lrn_plus_thr = 0.0f;

  lrn_noth = 0.0f;
  lrn_noth_cyc = 0.0f;
  lrn_noth_thr = 0.0f;

  Init_Stats_Sums();
}

void CtLrnTrigVals::Init_Stats_Sums() {
  lrn_stats_n = 0;

  lrn_min_sum = 0.0f;
  lrn_min_cyc_sum = 0.0f;
  lrn_min_thr_sum = 0.0f;

  lrn_plus_sum = 0.0f;
  lrn_plus_cyc_sum = 0.0f;
  lrn_plus_thr_sum = 0.0f;

  lrn_noth_sum = 0.0f;
  lrn_noth_cyc_sum = 0.0f;
  lrn_noth_thr_sum = 0.0f;
}

void LeabraNetwork::GraphInhibMod(bool flip_sign, DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_InhibMod", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* cyc_col = graph_data->FindMakeColName("ct_cycle", idx, VT_INT);
  DataCol* imod_col = graph_data->FindMakeColName("inhib_mod", idx, VT_FLOAT);
//   imod->SetUserData("MIN", 0.0f);
//   imod->SetUserData("MAX", 1.0f);

  float bi = ct_sin_i.burst_i;
  float ti = ct_sin_i.trough_i;
  float ii = ct_fin_i.inhib_i;

  for(int cyc = 0; cyc < ct_time.total_cycles; cyc++) {
    float imod = ct_sin_i.GetInhibMod(cyc, bi, ti) +
      ct_fin_i.GetInhibMod(cyc - ct_time.inhib_start, ii);
    if(flip_sign) imod *= -1.0f;
    graph_data->AddBlankRow();
    cyc_col->SetValAsInt(cyc, -1);
    imod_col->SetValAsFloat(imod, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}


void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);

  learn_rule = CTLEABRA_XCAL;
  ti_mode = false;
  prv_learn_rule = -1;
  phase_order = MINUS_PLUS;
  no_plus_test = true;
  sequence_init = DO_NOTHING;
  phase = MINUS_PHASE;
  nothing_phase = false;
  phase_no = 0;
  phase_max = 2;

  ct_cycle = 0;
  time_inc = 1.0f;              // just a simple counter by default

  cycle_max = 60;
  mid_minus_cycle = -1;
  min_cycles = 15;
  min_cycles_phase2 = 35;
  dwt_norm_enabled = false;

  thread_flags = TF_ALL;

  minus_cycles = 0.0f;
  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;

  send_pct = 0.0f;
  send_pct_n = send_pct_tot = 0;
  avg_send_pct = 0.0f;
  avg_send_pct_sum = 0.0f;
  avg_send_pct_n = 0;

  maxda_stopcrit = .005f;
  maxda = 0.0f;

  trg_max_act_stopcrit = 1.0f;  // disabled
  trg_max_act = 0.0f;

  ext_rew = 0.0f;
  ext_rew_avail = false;
  norew_val = 0.5f;
  avg_ext_rew = 0.0f;
  pvlv_pvi = 0.0f;
  pvlv_pvr = 0.0f;
  pvlv_lve = 0.0f;
  pvlv_lvi = 0.0f;
  pvlv_nv = 0.0f;
  pvlv_dav = 0.0f;
  pvlv_tonic_da = 0.0f;
  pv_detected = false;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;

  on_errs = true;
  off_errs = true;
  unlearnable_trial = false;

  norm_err = 0.0f;
  avg_norm_err = 1.0f;
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;

  cos_err_lrn_thr = -1.0f;
  cos_err = 0.0f;
  cos_err_prv = 0.0f;
  cos_err_vs_prv = 0.0f;
  avg_cos_err = 1.0f;
  avg_cos_err_sum = 0.0f;
  avg_cos_err_n = 0;
  avg_cos_err_prv = 1.0f;
  avg_cos_err_prv_sum = 0.0f;
  avg_cos_err_vs_prv = 1.0f;
  avg_cos_err_vs_prv_sum = 0.0f;

  inhib_cons_used = false;
  init_netins_cycle_stat = false;
}

void LeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_LeabraCon;
  prjn->recvcons_type = &TA_LeabraRecvCons;
  prjn->sendcons_type = &TA_LeabraSendCons;
  prjn->con_spec.SetBaseType(&TA_LeabraConSpec);
}

void LeabraNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(learn_rule != LEABRA_CHL)
    ct_time.use = true;		// has to be true for these guys

  ct_time.UpdateAfterEdit_NoGui();
  ct_lrn_trig.UpdateAfterEdit_NoGui();

  if(TestWarning(ct_sravg.plus_s_st >= ct_time.plus, "UAE",
               "ct_sravg.plus_s_st is higher than ct_time.plus (# of cycles in plus phase)"
               "just set it to plus-2")) {
    ct_sravg.plus_s_st = ct_time.plus -2;
  }

//   if(TestWarning(learn_rule != CTLEABRA_CAL && ct_sravg.interval == 5, "UAE",
//             "ct_sravg.interval should be 1 for all algorithms *EXCEPT* CTLEABRA_CAL -- I just set it to 1 for you.  Also, while I'm at it, I set n_avg_only_epcs = 0 as well, because that is the new default")) {
//     ct_sravg.interval = 1;
//     ct_time.n_avg_only_epcs = 0;
//   }
//   if(TestWarning(learn_rule == CTLEABRA_CAL && ct_sravg.interval == 1, "UAE",
//             "ct_sravg.interval should be > 1 for CTLEABRA_CAL -- I just set it to 5 for you")) {
//     ct_sravg.interval = 5;
//   }

  if(TestWarning(!off_errs && !on_errs, "UAE", "can't have both off_errs and on_errs be off (no err would be computed at all) -- turned both back on")) {
    on_errs = true;
    off_errs = true;
  }

  if(prv_learn_rule == -1) {
    prv_learn_rule = learn_rule;
  }
  else if(prv_learn_rule != learn_rule) {
    SetLearnRule();
    prv_learn_rule = learn_rule;
  }
}

void LeabraNetwork::BuildNullUnit() {
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new LeabraUnit, this);
  }
}

///////////////////////////////////////////////////////////////////////
//      General Init functions

void LeabraNetwork::Init_Acts() {
  inherited::Init_Acts();
}

void LeabraNetwork::Init_Counters() {
  inherited::Init_Counters();
  phase = MINUS_PHASE;
  nothing_phase = false;
  phase_no = 0;
}

void LeabraNetwork::Init_Stats() {
  inherited::Init_Stats();
  maxda = 0.0f;
  trg_max_act = 0.0f;

  minus_cycles = 0.0f;
  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;

  minus_output_name = "";

  send_pct_n = send_pct_tot = 0;
  send_pct = 0.0f;
  avg_send_pct = 0.0f;
  avg_send_pct_sum = 0.0f;
  avg_send_pct_n = 0;

  ext_rew = 0.0f;
  ext_rew_avail = false;
  norew_val = 0.5f;
  avg_ext_rew = 0.0f;
  pvlv_pvi = 0.0f;
  pvlv_pvr = 0.0f;
  pvlv_lve = 0.0f;
  pvlv_lvi = 0.0f;
  pvlv_nv = 0.0f;
  pvlv_dav = 0.0f;
  pvlv_tonic_da = 0.0f;
  pv_detected = false;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;

  norm_err = 0.0f;
  avg_norm_err = 1.0f;
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;

  cos_err = 0.0f;
  cos_err_prv = 0.0f;
  cos_err_vs_prv = 0.0f;
  avg_cos_err = 1.0f;
  avg_cos_err_sum = 0.0f;
  avg_cos_err_n = 0;
  avg_cos_err_prv = 1.0f;
  avg_cos_err_prv_sum = 0.0f;
  avg_cos_err_vs_prv = 1.0f;
  avg_cos_err_vs_prv_sum = 0.0f;

  lrn_trig.Init_Stats();
}

void LeabraNetwork::Init_Sequence() {
  inherited::Init_Sequence();
  if(sequence_init == INIT_STATE) {
    Init_Acts();
  }
  else if(sequence_init == DECAY_STATE) {
    Trial_DecayState();
  }
}

void LeabraNetwork::Init_Weights() {
  inherited::Init_Weights();
  sravg_vals.InitVals();

  lrn_trig.davg = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_s = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_m = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_smd = 0.0f;
  lrn_trig.davg_l = ct_lrn_trig.davg_l_init;
  lrn_trig.davg_max = ct_lrn_trig.davg_max_init;

  lrn_trig.cyc_fm_inc = 0;
  lrn_trig.cyc_fm_dec = 0;
  lrn_trig.loc_max = 0.0f;
  lrn_trig.lrn_max = 0.0f;

  lrn_trig.lrn_trig = 0.0f;
  lrn_trig.lrn = 0;
}

void LeabraNetwork::Init_Netins() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Init_Netins(this);
  }
}

void LeabraNetwork::DecayState(float decay) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->DecayState(this, decay);
  }
}

void LeabraNetwork::SetLearnRule_ConSpecs(BaseSpec_Group* spgp) {
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *spgp) {
    if(bs->InheritsFrom(&TA_LeabraConSpec)) {
      ((LeabraConSpec*)bs)->SetLearnRule(this);
    }
    SetLearnRule_ConSpecs(&bs->children); // recurse
  }
}

void LeabraNetwork::SetLearnRule() {
  if(learn_rule == LEABRA_CHL) {
    if(phase_order == MINUS_PLUS_NOTHING) {
      phase_order = MINUS_PLUS;
    }
    maxda_stopcrit = 0.005f;
    min_cycles = 15;
    min_cycles_phase2 = 35;
    cycle_max = 60;
    ct_time.use = false;
  }
  else {
//     if(phase_order == MINUS_PLUS) {
//       phase_order = MINUS_PLUS_NOTHING;
//     }

    if(learn_rule == CTLEABRA_CAL) {
      ct_sravg.interval = 5;
    }
    else {
      ct_sravg.interval = 1;
    }

    maxda_stopcrit = -1;
    min_cycles = 0;
    min_cycles_phase2 = 0;
    ct_time.use = true;
  }

  SetLearnRule_ConSpecs(&specs);
  // set all my specs -- otherwise it looks weird in hierarchy for unused parent specs

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    lay->SetLearnRule(this);
  }
}

void LeabraNetwork::CheckInhibCons() {
  inhib_cons_used = false;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->CheckInhibCons(this);
  }
}

void LeabraNetwork::BuildUnits_Threads() {
  CheckInhibCons();
  if(inhib_cons_used) {
    SetNetFlag(NETIN_PER_PRJN);	// inhib cons use per-prjn inhibition
  }
  inherited::BuildUnits_Threads();
}

///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LeabraNetwork::Trial_Init() {
  unlearnable_trial = false;
  cycle = -1;
  Trial_Init_Phases();
  SetCurLrate();

  Trial_Init_Unit(); // performs following at unit-level
//   Trial_DecayState();
  Trial_NoiseInit();            // run for kpos case
//   Trial_Init_SRAvg();
  Trial_Init_Layer();
}

void LeabraNetwork::Trial_Init_Phases() {
  phase = MINUS_PHASE;
  nothing_phase = false;
  phase_no = 0;

  bool no_plus_testing = false;
  if(no_plus_test && (train_mode == TEST)) {
    no_plus_testing = true;
  }

  switch(phase_order) {
  case MINUS_PLUS:
    if(no_plus_testing)
      phase_max=1;
    else
      phase_max=2;
    break;
  case PLUS_MINUS:
    if(no_plus_testing)
      phase_max=1;              // just do minus
    else {
      phase_max=2;
      phase = PLUS_PHASE;
    }
    break;
  case PLUS_ONLY:
    phase_max=1;
    phase = PLUS_PHASE;
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    phase_max=3;        // ignore no_plus_test here -- just turn PLUS into extra MINUS
    break;
  case PLUS_NOTHING:
    phase_max=2;        // ignore no_plus_test here -- just turn PLUS into MINUS
    break;
  }
}

void LeabraNetwork::SetCurLrate() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->SetCurLrate(this, epoch);
  }
}

void LeabraNetwork::Trial_Init_Unit() {
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Trial_Init_Unit);
  if(thread_flags & TRIAL_INIT)
    threads.Run(&un_call, .2f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  sravg_vals.InitVals();        // reset sravg vals, after Trial_Init_SRAvg!
}

void LeabraNetwork::Trial_NoiseInit() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Trial_NoiseInit(this);
  }
}

void LeabraNetwork::Trial_DecayState() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Trial_DecayState(this);
  }
}

void LeabraNetwork::Trial_Init_SRAvg() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Trial_Init_SRAvg(this);
  }
  sravg_vals.InitVals();        // reset sravg vals, after Trial_Init_SRAvg!
}

void LeabraNetwork::Trial_Init_Layer() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Trial_Init_Layer(this);
  }
}


///////////////////////////////////////////////////////////////////////
//      SettleInit -- at start of settling

void LeabraNetwork::Settle_Init() {
  int tmp_cycle = cycle;
  cycle = -2;                   // special signal for settle init

  Settle_Init_CtTimes();
  Compute_Active_K();           // compute here because could depend on pat_n

  Settle_Init_Unit();           // do chunk of following unit-level functions:

//   Settle_Init_TargFlags();
//   Settle_DecayState();
//   Compute_NetinScale();              // compute net scaling

  Compute_HardClamp();          // clamp all hard-clamped input acts: not easily threadable
  cycle = tmp_cycle;
}

void LeabraNetwork::Settle_Init_CtTimes() {
  if(!ct_time.use) return;

  switch(phase_no) {
  case 0:
    cycle_max = ct_time.minus;
    break;
  case 1:
    cycle_max = ct_time.plus;
    break;
  case 2:
    cycle_max = ct_time.inhib;
    break;
  }
}

void LeabraNetwork::Compute_Active_K() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_Active_K(this);        // this gets done at the outset..
  }
}

void LeabraNetwork::Settle_Init_Unit() {
  if(nothing_phase) {
    if(phase_order != MINUS_PLUS_MINUS) {
      TargExtToComp();          // all external input is now 'comparison' = nothing!
    }
  }

  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Settle_Init_Unit);
  if(thread_flags & SETTLE_INIT)
    threads.Run(&un_call, .1f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  Settle_Init_Layer();

  Compute_NetinScale_Senders(); // second phase after recv-based NetinScale
  // put it after Settle_Init_Layer to allow for mods to netin scale in that guy..
}

void LeabraNetwork::Settle_Init_Layer() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Settle_Init_Layer(this);
  }
}

void LeabraNetwork::Settle_Init_TargFlags() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  if(nothing_phase) {
    if(phase_order != MINUS_PLUS_MINUS) {
      TargExtToComp();          // all external input is now 'comparison' = nothing!
    }
  }

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Settle_Init_TargFlags(this);
  }
}

void LeabraNetwork::Settle_DecayState() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Settle_DecayState);
  threads.Run(&un_call, .1f);   // lowest number -- not for real use so doesn't matter
}

void LeabraNetwork::Compute_NetinScale() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_NetinScale);
  threads.Run(&un_call, .1f);   // lowest number -- not for real use so doesn't matter
}

void LeabraNetwork::Compute_NetinScale_Senders() {
  // NOTE: this IS called by default -- second phase of Settle_Init_Unit
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_NetinScale_Senders);
  threads.Run(&un_call, .1f);   // lowest number -- minimal computation
}

void LeabraNetwork::Compute_HardClamp() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_HardClamp(this);
  }
}

void LeabraNetwork::ExtToComp() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->ExtToComp(this);
  }
}

void LeabraNetwork::TargExtToComp() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->TargExtToComp(this);
  }
}

void LeabraNetwork::NewInputData_Init() {
  Settle_Init_Layer();
  Settle_Init_TargFlags();
  Compute_HardClamp();
}

////////////////////////////////////////////////////////////////
//      Cycle_Run

void LeabraNetwork::Cycle_Run() {
  // ct_cycle is pretty useful anyway
  if(phase_no == 0 && cycle == 0) // detect start of trial
    ct_cycle = 0;

  Send_Netin();
  Compute_ExtraNetin();         // layer level extra netinput for special algorithms
  Compute_NetinInteg();
  Compute_NetinStats();

  Compute_Inhib();
  Compute_ApplyInhib();

  Compute_Act();
  Compute_CycleStats();

  Compute_CycSynDep();

  Compute_SRAvg_State();
  Compute_SRAvg();              // note: only ctleabra variants do con-level compute here
  Compute_XCalC_dWt();
  Compute_MidMinus();           // check for mid-minus and run if so (PBWM)

  ct_cycle++;
  time += time_inc;                     // always increment time..
}

///////////////////////////////////////////////////////
//      Cycle Stage 1: netinput

void LeabraNetwork::Send_Netin() {
  // always use delta mode!
  send_pct_n = send_pct_tot = 0;
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Send_NetinDelta);
  if(thread_flags & NETIN)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  // now need to roll up the netinput into unit vals
  const int nu = units_flat.size;
  for(int i=1;i<nu;i++) {   // 0 = dummy idx
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    u->Send_NetinDelta_Post(this);
  }

  if(NetinPerPrjn() || threads.using_threads) {
    send_netin_tmp.InitVals(0.0f); // reset for next time around
  }

#ifdef DMEM_COMPILE
  dmem_share_units.Sync(3);
#endif
  if(send_pct_tot > 0) {        // only avail for non-threaded calls
    send_pct = (float)send_pct_n / (float)send_pct_tot;
    avg_send_pct_sum += send_pct;
    avg_send_pct_n++;
  }
}

void LeabraNetwork::Compute_ExtraNetin() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_ExtraNetin(this);
  }
}

void LeabraNetwork::Compute_NetinInteg() {
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_NetinInteg);
  if(thread_flags & NETIN_INTEG)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_NetinStats() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_NetinStats(this);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition

void LeabraNetwork::Compute_Inhib() {
  bool do_lay_gp = false;
  if(layers.gp.size == 0) do_lay_gp = true; // falsely set to true to prevent further checking
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_Inhib(this);
    if(!do_lay_gp) {
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      if(laysp->lay_gp_inhib.on) do_lay_gp = true;
    }
  }
  if(layers.gp.size == 0) do_lay_gp = false; // now override anything
  if(do_lay_gp) {
    for(int lgi = 0; lgi < layers.gp.size; lgi++) {
      Layer_Group* lg = (Layer_Group*)layers.gp[lgi];
      float lay_gp_g_i = 0.0f;
      for(int li = 0; li < lg->size; li++) {
        LeabraLayer* lay = (LeabraLayer*)lg->FastEl(li);
        LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
        if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;
        float lay_val = laysp->lay_gp_inhib.gp_g * lay->i_val.g_i;
        lay_gp_g_i = MAX(lay_val, lay_gp_g_i);
      }
      if(lay_gp_g_i > 0.0f) {   // actually got something
        for(int li = 0; li < lg->size; li++) {
          LeabraLayer* lay = (LeabraLayer*)lg->FastEl(li);
          LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
          if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;
          lay->i_val.gp_g_i = lay_gp_g_i;
          lay->i_val.g_i = MAX(lay->i_val.gp_g_i, lay->i_val.g_i);

          if(lay->unit_groups) {
            lay->Compute_LayInhibToGps(this);
          }
        }
      }
    }
  }
}

void LeabraNetwork::Compute_ApplyInhib() {
  // this is not threadable due to interactions with inhib vals at layer level
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_ApplyInhib(this);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation

void LeabraNetwork::Compute_Act() {
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_Act);
  if(thread_flags & ACT)
    threads.Run(&un_call, 0.4f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraNetwork::Compute_CycleStats() {
  // stats are never threadable due to updating at higher levels
  output_name = "";             // this will be updated by layer
  maxda = 0.0f;         // initialize
  trg_max_act = 0.0f;
  init_netins_cycle_stat = false;

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_CycleStats(this);
  }
  if(init_netins_cycle_stat) {
    Init_Netins();
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Optional Misc

void LeabraNetwork::Compute_CycSynDep() {
  if(!net_misc.cyc_syn_dep) return;
  if(ct_cycle % net_misc.syn_dep_int != 0) return;

  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_CycSynDep);
  threads.Run(&un_call, 0.6f); // todo: this # is an estimate -- not tested yet -- no flag for it
}

void LeabraNetwork::Compute_MidMinus() {
  if(mid_minus_cycle <= 0) return;
  if(ct_cycle == mid_minus_cycle) {
    FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
      if(lay->lesioned())       continue;
      lay->Compute_MidMinus(this);
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Settle Final

void LeabraNetwork::Settle_Final() {
  // all weight changes take place here for consistency!
  PostSettle_Pre();
  PostSettle();
  Settle_Compute_dWt();
}

void LeabraNetwork::PostSettle_Pre() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->PostSettle_Pre(this);
  }
}

void LeabraNetwork::PostSettle() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->PostSettle(this);
  }
  if(ti_mode) {
    TI_CtxtUpdate();
  }
}

void LeabraNetwork::Settle_Compute_dWt() {
  if(train_mode == TEST)
    return;

  switch(phase_order) {
  case MINUS_PLUS:
    if(phase_no == 0) {
      Compute_dWt_FirstMinus();
    }
    // note: no break -- intentional fallthrough..
  case PLUS_MINUS:
  case PLUS_NOTHING:
    if(phase_no == 1) {
      Compute_dWt_FirstPlus();
      AdaptKWTAPt();
    }
    break;
  case PLUS_ONLY:
    Compute_dWt_FirstPlus();
    AdaptKWTAPt();
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    if(phase_no == 0) {
      Compute_dWt_FirstMinus();
    }
    else if(phase_no == 1) {
      Compute_dWt_FirstPlus();
    }
    else if(phase_no == 2) {
      Compute_dWt_Nothing();
      AdaptKWTAPt();
    }
    break;
  }
}

void LeabraNetwork::AdaptKWTAPt() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->AdaptKWTAPt(this);
  }
}


///////////////////////////////////////////////////////////////////////
//      LeabraTI final plus phase context updating

void LeabraNetwork::TI_CtxtUpdate() {
  bool do_updt = false;
  no_plus_test = false;         // never viable to do that with TI!
  if(phase_order == MINUS_PLUS) {
    if(phase_no == 1)
      do_updt = true;
  }
  else {
    TestError(true, "LeabraTI_CtxtUpdate",
              "LeabraTI ti_on mode is only compatible with phase_order = MINUS_PLUS -- I just set it");
    phase_order = MINUS_PLUS;
  }
  if(do_updt) {
    TI_Send_CtxtNetin();
    TI_Compute_CtxtAct();
  }
}

void LeabraNetwork::TI_Send_CtxtNetin() {
  send_pct_n = send_pct_tot = 0;
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::TI_Send_CtxtNetin);
  if(thread_flags & NETIN)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized

  // now need to roll up the netinput into unit vals
  const int nu = units_flat.size;
  for(int i=1;i<nu;i++) {   // 0 = dummy idx
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    u->TI_Send_CtxtNetin_Post(this);
  }

  if(threads.using_threads) {
    send_netin_tmp.InitVals(0.0f); // reset for next time around
  }
  // todo: need this??
// #ifdef DMEM_COMPILE
//   dmem_share_units.Sync(3);
// #endif
}

void LeabraNetwork::TI_Compute_CtxtAct() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->TI_Compute_CtxtAct(this);
  }
}

void LeabraNetwork::TI_ClearContext() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->TI_ClearContext(this);
  }
}



///////////////////////////////////////////////////////////////////////
//      Trial Update and Final

void LeabraNetwork::Trial_UpdatePhase() {
  if(phase_no == phase_max) return; // done!

  // this assumes that phase_no > 0 -- called after updating phase_no
  nothing_phase = false;
  bool no_plus_testing = false;
  if(no_plus_test && (train_mode == TEST)) {
    no_plus_testing = true;
  }

  switch(phase_order) {
  case MINUS_PLUS:
    phase = PLUS_PHASE;
    break;
  case PLUS_MINUS:
    phase = MINUS_PHASE;
    break;
  case PLUS_ONLY:
    // nop
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    // diff between these two is in Settle_Init_Decay: TargExtToComp()
    if(phase_no == 1) {
      if(no_plus_testing)
        phase = MINUS_PHASE;    // another minus
      else
        phase = PLUS_PHASE;
    }
    else {
      phase = MINUS_PHASE;
      nothing_phase = true;
    }
    break;
  case PLUS_NOTHING:
    phase = MINUS_PHASE;
    nothing_phase = true;
    break;
  }
}

void LeabraNetwork::Trial_Final() {
  EncodeState();
  Compute_SelfReg_Trial();
}

void LeabraNetwork::EncodeState() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->EncodeState(this);
  }
}

void LeabraNetwork::Compute_SelfReg_Trial() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_SelfReg_Trial(this);
  }
}

///////////////////////////////////////////////////////////////////////
//      Learning

void LeabraNetwork::Compute_SRAvg_State() {
  if(!ct_sravg.manual) {
    sravg_vals.state = CtSRAvgVals::NO_SRAVG;
    int eff_int = ct_sravg.interval;

    if(ct_sravg.plus_s_only) {
      if(phase == LeabraNetwork::PLUS_PHASE && cycle >= ct_sravg.plus_s_st) {
	if((ct_time.plus - ct_sravg.plus_s_st) < eff_int) {
	  eff_int = 1;              // make sure you get short-time/plus phase info!
	}
	if((ct_cycle - ct_sravg.start) % eff_int == 0) {
	  sravg_vals.state = CtSRAvgVals::SRAVG_S; // s-only
	}
      }
      else if(phase == LeabraNetwork::MINUS_PHASE) { // m only in minus phase
	if((ct_cycle >= ct_sravg.start) &&
	   (ct_cycle < (ct_time.inhib_start + ct_sravg.end)) &&
	   ((ct_cycle - ct_sravg.start) % eff_int == 0)) {
	  sravg_vals.state = CtSRAvgVals::SRAVG_M;
	}
      }
    }
    else {
      if(phase == LeabraNetwork::PLUS_PHASE && cycle >= ct_sravg.plus_s_st) {
	if((ct_time.plus - ct_sravg.plus_s_st) < eff_int) {
	  eff_int = 1;              // make sure you get short-time/plus phase info!
	}
	if((ct_cycle - ct_sravg.start) % eff_int == 0) {
	  sravg_vals.state = CtSRAvgVals::SRAVG_SM; // always do M by default
	}
      }
      else {
	if((ct_cycle >= ct_sravg.start) &&
	   (ct_cycle < (ct_time.inhib_start + ct_sravg.end)) &&
	   ((ct_cycle - ct_sravg.start) % eff_int == 0)) {
	  sravg_vals.state = CtSRAvgVals::SRAVG_M;
	}
      }
    }
  }
  
  // now go through layers and let them update their own state values -- usu just
  // copy from network..
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_SRAvg_State(this);
  }
}

void LeabraNetwork::Compute_SRAvg() {
  // update the network-level sravg_vals, even though these should not generally be used
  if(sravg_vals.state == CtSRAvgVals::SRAVG_M ||
     sravg_vals.state == CtSRAvgVals::SRAVG_SM) {
    sravg_vals.m_sum += 1.0f;
    sravg_vals.m_nrm = 1.0f / sravg_vals.m_sum;
  }

  if(sravg_vals.state == CtSRAvgVals::SRAVG_S ||
     sravg_vals.state == CtSRAvgVals::SRAVG_SM) {
    sravg_vals.s_sum += 1.0f;
    sravg_vals.s_nrm = 1.0f / sravg_vals.s_sum;
  }

  // first go through layers and let them update
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_SRAvg_Layer(this);
  }

  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_SRAvg);
  if(thread_flags & SRAVG)
    threads.Run(&un_call, 0.9f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_XCalC_dWt() {
  if(learn_rule != CTLEABRA_XCAL_C) return;

  bool do_lrn = false;

  float tmp_davg = 0.0f;
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    tmp_davg += fabsf(un->davg);
  }
  lrn_trig.davg = tmp_davg / (float)units_flat.size;
  lrn_trig.davg_s += ct_lrn_trig.davg_s_dt * (lrn_trig.davg - lrn_trig.davg_s);
  lrn_trig.davg_m += ct_lrn_trig.davg_m_dt * (lrn_trig.davg_s - lrn_trig.davg_m);
  float nw_smd = lrn_trig.davg_s - lrn_trig.davg_m;
  float d_smd = nw_smd - lrn_trig.davg_smd;
  lrn_trig.davg_smd = nw_smd;

  lrn_trig.davg_l += ct_lrn_trig.davg_l_dt * (lrn_trig.davg_smd - lrn_trig.davg_l);
  if(lrn_trig.davg_smd >= lrn_trig.davg_max) {
    lrn_trig.davg_max = lrn_trig.davg_smd;
  }
  else {
    lrn_trig.davg_max += ct_lrn_trig.davg_l_dt * (lrn_trig.davg_l - lrn_trig.davg_max);
  }

  if(d_smd > 0.0f) {
    lrn_trig.loc_max = lrn_trig.davg_smd; // indicate local max
    lrn_trig.cyc_fm_inc = 0;
    lrn_trig.cyc_fm_dec++;
  }
  else {
    lrn_trig.cyc_fm_dec = 0;
    lrn_trig.cyc_fm_inc++;
  }

  float maxldif = lrn_trig.davg_max - lrn_trig.davg_l;
  float thr_min_eff = lrn_trig.davg_l +  ct_lrn_trig.thr_min * maxldif;
  float thr_max_eff = lrn_trig.davg_l +  ct_lrn_trig.thr_max * maxldif;

  float loc_dec_norm = (lrn_trig.loc_max - lrn_trig.davg_smd) / maxldif;

  if(lrn_trig.lrn_trig > 0.0f) {        // already met thresh, counting up to learn time
    lrn_trig.lrn_trig += ct_lrn_trig.lrn_delay_inc;
    if(lrn_trig.lrn_trig >= 1.0f) {
      do_lrn = true;
      lrn_trig.lrn_trig = -1.0f;  // begin refractory period
    }
  }
  else if(lrn_trig.lrn_trig < 0.0f) {   // already learned, counting down to refract
    lrn_trig.lrn_trig += ct_lrn_trig.lrn_refract_inc;
    if(lrn_trig.lrn_trig >= 0.0f) {
      lrn_trig.lrn_trig = 0.0f; // reset -- eligible for learning again
    }
  }
  else if((lrn_trig.cyc_fm_dec == 0) && // going down
          (lrn_trig.cyc_fm_inc == ct_lrn_trig.loc_max_cyc) && // x amount from inc
          (loc_dec_norm >= ct_lrn_trig.loc_max_dec) && // x inc
          (lrn_trig.loc_max >= thr_min_eff && lrn_trig.loc_max <= thr_max_eff)) { // max in range
    lrn_trig.lrn_max = lrn_trig.loc_max;
    lrn_trig.lrn_trig = ct_lrn_trig.lrn_delay_inc; // start counting
  }

  if(ct_lrn_trig.plus_lrn_cyc > 0) {
    // fake it override!
    do_lrn = (ct_cycle == (ct_time.minus + ct_lrn_trig.plus_lrn_cyc));
  }

  if(do_lrn) {
    // this is all the std code from Compute_dWt_FirstPlus
    Compute_dWt_Layer_pre();
    lrn_trig.lrn = 1;
    ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_FirstPlus);
    if(thread_flags & DWT)
      threads.Run(&un_call, 0.6f);
    else
      threads.Run(&un_call, -1.0f); // -1 = always run localized

    Compute_dWt_Norm();

    lrn_trig.lrn_stats_n++;
    if(phase_no == 0) {
      lrn_trig.lrn_min_sum += 1.0f;
      lrn_trig.lrn_min_cyc_sum += cycle;
      lrn_trig.lrn_min_thr_sum += lrn_trig.lrn_max;
    }
    else if(phase_no == 1) {
      lrn_trig.lrn_plus_sum += 1.0f;
      lrn_trig.lrn_plus_cyc_sum += cycle;
      lrn_trig.lrn_plus_thr_sum += lrn_trig.lrn_max;
    }
    else {
      lrn_trig.lrn_noth_sum += 1.0f;
      lrn_trig.lrn_noth_cyc_sum += cycle;
      lrn_trig.lrn_noth_thr_sum += lrn_trig.lrn_max;
    }
  }
  else {
    lrn_trig.lrn = 0;
  }
}

void LeabraNetwork::Compute_dWt_Layer_pre() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_dWt_Layer_pre(this);
  }
}

void LeabraNetwork::Compute_dWt_FirstMinus() {
  if(learn_rule == CTLEABRA_XCAL_C) return; // done separately
  if(learn_rule == LeabraNetwork::CTLEABRA_XCAL && epoch < ct_time.n_avg_only_epcs) {
    return; // no learning while gathering data!
  }

  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_FirstMinus);
  if(thread_flags & DWT)
    threads.Run(&un_call, 0.6f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
  //  Compute_dWt_Norm(); // assume first plus will handle this.. 
}

void LeabraNetwork::Compute_dWt_FirstPlus() {
  if(learn_rule == CTLEABRA_XCAL_C) return; // done separately
  if(learn_rule == LeabraNetwork::CTLEABRA_XCAL && epoch < ct_time.n_avg_only_epcs) {
    return; // no learning while gathering data!
  }

  if(cos_err_lrn_thr > -1.0f) {		  // note: requires computing err before calling this!
    if(cos_err < cos_err_lrn_thr) return; // didn't make threshold
  }

  Compute_dWt_Layer_pre();
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_FirstPlus);
  if(thread_flags & DWT)
    threads.Run(&un_call, 0.6f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
  Compute_dWt_Norm();
}

void LeabraNetwork::Compute_dWt_Nothing() {
  if(learn_rule == CTLEABRA_XCAL_C) return; // done separately
  if(learn_rule == LeabraNetwork::CTLEABRA_XCAL && epoch < ct_time.n_avg_only_epcs) {
    return; // no learning while gathering data!
  }
  Compute_dWt_Layer_pre();
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_Nothing);
  if(thread_flags & DWT)
    threads.Run(&un_call, 0.6f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
  Compute_dWt_Norm();
}

void LeabraNetwork::Compute_dWt_Norm() {
  if(!dwt_norm_enabled) return;
  if(learn_rule == LeabraNetwork::CTLEABRA_XCAL && epoch < ct_time.n_avg_only_epcs) {
    return; // no learning while gathering data!
  }
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_dWt_Norm);
  if(thread_flags & DWT)
    threads.Run(&un_call, 0.6f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_Weights_impl() {
  ThreadUnitCall un_call(&Unit::Compute_Weights);
  if(thread_flags & WEIGHTS)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Compute_StableWeights() {
  ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Compute_StableWeights);
  if(thread_flags & WEIGHTS)
    threads.Run(&un_call, 1.0f);
  else
    threads.Run(&un_call, -1.0f); // -1 = always run localized
}


///////////////////////////////////////////////////////////////////////
//      Stats

void LeabraNetwork::Compute_ExtRew() {
  // assumes any ext rew computation has happened before this point, and set the
  // network ext_rew and ext_rew_avail flags appropriately
  if(ext_rew_avail) {
    avg_ext_rew_sum += ext_rew;
    avg_ext_rew_n++;
  }
}

void LeabraNetwork::Compute_NormErr() {
  float nerr_sum = 0.0f;
  float nerr_avail = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    float nerr = lay->Compute_NormErr(this);
    if(nerr >= 0.0f) {
      nerr_avail += 1.0f;
      nerr_sum += nerr;
    }
  }
  if(nerr_avail > 0.0f) {
    norm_err = nerr_sum / nerr_avail; // normalize contribution across layers

    if(cos_err_lrn_thr > -1.0f) {
      if(cos_err < cos_err_lrn_thr) return; // didn't make threshold - don't add to global
    }
    avg_norm_err_sum += norm_err;
    avg_norm_err_n++;
  }
  else {
    norm_err = 0.0f;
  }
}

float LeabraNetwork::Compute_M2SSE(bool unit_avg, bool sqrt) {
  float sse = 0.0f;
  int n_vals = 0;
  int lay_vals = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    sse += l->Compute_M2SSE(this, lay_vals);
    n_vals += lay_vals;
  }
  if(unit_avg && n_vals > 0)
    sse /= (float)n_vals;
  if(sqrt)
    sse = sqrtf(sse);
  return sse;
}

float LeabraNetwork::Compute_M2SSE_Recon(bool unit_avg, bool sqrt) {
  float sse = 0.0f;
  int n_vals = 0;
  int lay_vals = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned() || l->layer_type != Layer::INPUT) continue;
    sse += l->Compute_M2SSE(this, lay_vals);
    n_vals += lay_vals;
  }
  if(unit_avg && n_vals > 0)
    sse /= (float)n_vals;
  if(sqrt)
    sse = sqrtf(sse);
  return sse;
}

float LeabraNetwork::Compute_CosErr() {
  float cosv = 0.0f;
  float cosvp = 0.0f;
  float cosvsp = 0.0f;
  int n_lays = 0;
  int lay_vals = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    cosv += l->Compute_CosErr(this, lay_vals);
    if(ti_mode && lay_vals > 0) {
      cosvp += l->cos_err_prv;
      cosvsp += l->cos_err_vs_prv;
      n_lays++;
    }
  }
  if(n_lays > 0) {
    cosv /= (float)n_lays;
    cos_err = cosv;
    avg_cos_err_sum += cos_err;
    avg_cos_err_n++;

    if(ti_mode) {
      cosvp /= (float)n_lays;
      cos_err_prv = cosvp;
      avg_cos_err_prv_sum += cos_err_prv;

      cosvsp /= (float)n_lays;
      cos_err_vs_prv = cosvsp;
      avg_cos_err_vs_prv_sum += cos_err_vs_prv;
    }
  }
  else {
    cos_err = 0.0f;
    cos_err_prv = 0.0f;
    cos_err_vs_prv = 0.0f;
  }
  return cosv;
}

float LeabraNetwork::Compute_M2CosErr() {
  float cosv = 0.0f;
  int n_lays = 0;
  int lay_vals = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    cosv += l->Compute_M2CosErr(this, lay_vals);
    if(lay_vals > 0)
      n_lays++;
  }
  if(n_lays > 0)
    cosv /= (float)n_lays;
  return cosv;
}

float LeabraNetwork::Compute_M2CosErr_Recon() {
  float cosv = 0.0f;
  int n_lays = 0;
  int lay_vals = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned() || l->layer_type != Layer::INPUT) continue;
    cosv += l->Compute_M2CosErr(this, lay_vals);
    if(lay_vals > 0)
      n_lays++;
  }
  if(n_lays > 0)
    cosv /= (float)n_lays;
  return cosv;
}

float LeabraNetwork::Compute_CosDiff() {
  float cosv = 0.0f;
  int n_lays = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    cosv += l->Compute_CosDiff(this);
    n_lays++;
  }
  if(n_lays > 0)
    cosv /= (float)n_lays;
  return cosv;
}

float LeabraNetwork::Compute_CosDiff2() {
  float cosv = 0.0f;
  int n_lays = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    cosv += l->Compute_CosDiff2(this);
    n_lays++;
  }
  if(n_lays > 0)
    cosv /= (float)n_lays;
  return cosv;
}

void LeabraNetwork::Compute_MinusCycles() {
  minus_cycles = cycle;
  avg_cycles_sum += minus_cycles;
  avg_cycles_n++;
}

bool LeabraNetwork::Compute_TrialStats_Test() {
  bool is_time = false;

  bool no_plus_testing = false;
  if(no_plus_test && (train_mode == TEST)) {
    no_plus_testing = true;
  }

  switch(phase_order) {
  case MINUS_PLUS:
    if(phase_no == 0) is_time = true;
    break;
  case PLUS_MINUS:
    if(no_plus_testing)
      is_time = true;
    else {
      if(phase_no == 1) is_time = true;
    }
    break;
  case PLUS_ONLY:
    is_time = true;
    break;
  case MINUS_PLUS_NOTHING:
  case MINUS_PLUS_MINUS:
    if(no_plus_testing) {
      if(phase_no == 1) is_time = true;
    }
    else {
      if(phase_no == 0) is_time = true;
    }
    break;
  case PLUS_NOTHING:
    if(phase_no == 1) is_time = true;
    break;
  }

  return is_time;
}

void LeabraNetwork::Compute_TrialStats() {
  if(unlearnable_trial) {
    sse = 0.0f;                 // ignore errors..
    norm_err = 0.0f;
    cos_err = 0.0f;
    cos_err_prv = 0.0f;
    cos_err_vs_prv = 0.0f;
  }
  else {
    Compute_SSE(sse_unit_avg, sse_sqrt);
    if(compute_prerr)
      Compute_PRerr();
    Compute_NormErr();
    Compute_CosErr();
  }
  Compute_MinusCycles();
  minus_output_name = output_name; // grab and hold..
}

void LeabraNetwork::Compute_AbsRelNetin() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_AbsRelNetin(this);
  }
}

void LeabraNetwork::Compute_AvgAbsRelNetin() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_AvgAbsRelNetin(this);
  }
}

void LeabraNetwork::Compute_TrgRelNetin() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_TrgRelNetin(this);
  }
}

void LeabraNetwork::Compute_AdaptRelNetin() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_AdaptRelNetin(this);
  }
}

void LeabraNetwork::Compute_AdaptAbsNetin() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_AdaptAbsNetin(this);
  }
}

void LeabraNetwork::Compute_AvgCycles() {
  if(avg_cycles_n > 0) {
    avg_cycles = avg_cycles_sum / (float)avg_cycles_n;
  }
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;
}

void LeabraNetwork::Compute_AvgSendPct() {
  if(avg_send_pct_n > 0) {
    avg_send_pct = avg_send_pct_sum / (float)avg_send_pct_n;
  }
  avg_send_pct_sum = 0.0f;
  avg_send_pct_n = 0;
}

void LeabraNetwork::Compute_AvgExtRew() {
  if(avg_ext_rew_n > 0) {
    avg_ext_rew = avg_ext_rew_sum / (float)avg_ext_rew_n;
  }
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::Compute_AvgNormErr() {
  if(avg_norm_err_n > 0) {
    avg_norm_err = avg_norm_err_sum / (float)avg_norm_err_n;
  }
  avg_norm_err_sum = 0.0f;
  avg_norm_err_n = 0;
}

void LeabraNetwork::Compute_AvgCosErr() {
  if(avg_cos_err_n > 0) {
    avg_cos_err = avg_cos_err_sum / (float)avg_cos_err_n;
  }
  avg_cos_err_sum = 0.0f;

  if(ti_mode) {
    if(avg_cos_err_n > 0) {
      avg_cos_err_prv = avg_cos_err_prv_sum / (float)avg_cos_err_n;
    }
    avg_cos_err_prv_sum = 0.0f;

    if(avg_cos_err_n > 0) {
      avg_cos_err_vs_prv = avg_cos_err_vs_prv_sum / (float)avg_cos_err_n;
    }
    avg_cos_err_vs_prv_sum = 0.0f;
  }

  avg_cos_err_n = 0;
}

void LeabraNetwork::Compute_CtLrnTrigAvgs() {
  if(lrn_trig.lrn_stats_n > 0) {
    float ltrign = (float)lrn_trig.lrn_stats_n;
    if(lrn_trig.lrn_min_sum > 0.0f) {
      lrn_trig.lrn_min_cyc = lrn_trig.lrn_min_cyc_sum / lrn_trig.lrn_min_sum;
      lrn_trig.lrn_min_thr = lrn_trig.lrn_min_thr_sum / lrn_trig.lrn_min_sum;
    }
    else {
      lrn_trig.lrn_min_cyc = 0.0f;
      lrn_trig.lrn_min_thr = 0.0f;
    }
    lrn_trig.lrn_min = lrn_trig.lrn_min_sum / ltrign;

    if(lrn_trig.lrn_plus_sum > 0.0f) {
      lrn_trig.lrn_plus_cyc = lrn_trig.lrn_plus_cyc_sum / lrn_trig.lrn_plus_sum;
      lrn_trig.lrn_plus_thr = lrn_trig.lrn_plus_thr_sum / lrn_trig.lrn_plus_sum;
    }
    else {
      lrn_trig.lrn_plus_cyc = 0.0f;
      lrn_trig.lrn_plus_thr = 0.0f;
    }
    lrn_trig.lrn_plus = lrn_trig.lrn_plus_sum / ltrign;

    if(lrn_trig.lrn_noth_sum > 0.0f) {
      lrn_trig.lrn_noth_cyc = lrn_trig.lrn_noth_cyc_sum / lrn_trig.lrn_noth_sum;
      lrn_trig.lrn_noth_thr = lrn_trig.lrn_noth_thr_sum / lrn_trig.lrn_noth_sum;
    }
    else {
      lrn_trig.lrn_noth_cyc = 0.0f;
      lrn_trig.lrn_noth_thr = 0.0f;
    }
    lrn_trig.lrn_noth = lrn_trig.lrn_noth_sum / ltrign;
  }
  lrn_trig.Init_Stats_Sums();
}

void LeabraNetwork::Compute_EpochStats() {
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
  Compute_AvgNormErr();
  Compute_AvgCosErr();
  Compute_AvgExtRew();
  Compute_AvgSendPct();
  Compute_CtLrnTrigAvgs();
}
