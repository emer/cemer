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
#include <DataTable>
#include <taMisc>
#include <tabMisc>

#ifdef CUDA_COMPILE
#include "LeabraConSpec_cuda.h"
#endif

TA_BASEFUNS_CTORS_DEFN(LeabraNetTiming);
TA_BASEFUNS_CTORS_DEFN(LeabraCudaSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraTimes);
TA_BASEFUNS_CTORS_DEFN(LeabraNetStats);
TA_BASEFUNS_CTORS_DEFN(LeabraNetMisc);
TA_BASEFUNS_CTORS_DEFN(RelNetinSched);
TA_BASEFUNS_CTORS_DEFN(LeabraNetwork);


void LeabraCudaSpec::Initialize() {
#ifdef CUDA_COMPILE
  on = true;
#else
  on = false;
#endif
  get_wts = false;
  min_threads = 32;
  max_threads = 1024;
  cons_per_thread = 2;
  timers_on = false;
}

void LeabraTimes::Initialize() {
  quarter = 25;
  cycle_qtr = true;
  time_inc = 0.001f;

  minus = 3 * quarter;
  plus = quarter;
  total_cycles = minus + plus;
}

void LeabraTimes::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  minus = 3 * quarter;
  plus = quarter;
  total_cycles = minus + plus;
}

void LeabraNetStats::Initialize() {
  trg_max_act_crit = 0.5f;
  off_errs = true;
  on_errs = true;
  cos_err_lrn_thr = -1.0f;
}

void LeabraNetMisc::Initialize() {
  ti = false;
  deep5b_cons = false;
  diff_scale_p = false;
  dwt_norm = false;
  lay_gp_inhib = false;
  inhib_cons = false;
}

void RelNetinSched::Initialize() {
  on = true;
  trl_skip = 10;
  epc_skip = 10;
}

void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);
  net_timing.SetBaseType(&TA_LeabraNetTiming);

  unit_vars_type = &TA_LeabraUnitVars;
  con_group_type = &TA_LeabraConGroup;

  unlearnable_trial = false;

  quarter = 0;
  phase = MINUS_PHASE;

  tot_cycle = 0;

  rt_cycles = 0.0f;

  send_pct = 0.0f;
  send_pct_n = send_pct_tot = 0;

  trg_max_act = 0.0f;

  ext_rew = 0.0f;
  ext_rew_avail = false;

  norm_err = 0.0f;

  cos_err = 0.0f;
  cos_err_prv = 0.0f;
  cos_err_vs_prv = 0.0f;

  cos_diff = 0.0f;

  trial_cos_diff = 0.0f;

  init_netins_cycle_stat = false;
  thrs_send_d5bnet_tmp = NULL;  // todo: need to free this thing too!

#ifdef CUDA_COMPILE
  cudai = new LeabraConSpecCuda;
#endif
}

void LeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_LeabraCon;
  prjn->con_spec.SetBaseType(&TA_LeabraConSpec);
}

void LeabraNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  times.UpdateAfterEdit_NoGui();

  if(TestWarning(!lstats.off_errs && !lstats.on_errs, "UAE", "can't have both lstats.off_errs and lstats.on_errs be off (no err would be computed at all) -- turned both back on")) {
    lstats.on_errs = true;
    lstats.off_errs = true;
  }
}

void LeabraNetwork::BuildNullUnit() {
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new LeabraUnit, this);
  }
}

///////////////////////////////////////////////////////////////////////
//      General Init functions

void LeabraNetwork::Init_Counters() {
  inherited::Init_Counters();
  quarter = 0;
  phase = MINUS_PHASE;
  tot_cycle = 0;
}

void LeabraNetwork::Init_Stats() {
  inherited::Init_Stats();
  trg_max_act = 0.0f;

  rt_cycles = 0.0f;
  avg_cycles.ResetAvg();

  minus_output_name = "";

  send_pct_n = send_pct_tot = 0;
  send_pct = 0.0f;
  avg_send_pct.ResetAvg();

  ext_rew = 0.0f;
  ext_rew_avail = false;
  avg_ext_rew.ResetAvg();

  norm_err = 0.0f;
  avg_norm_err.ResetAvg();

  cos_err = 0.0f;
  cos_err_prv = 0.0f;
  cos_err_vs_prv = 0.0f;
  avg_cos_err.ResetAvg();
  avg_cos_err_prv.ResetAvg();
  avg_cos_err_vs_prv.ResetAvg();

  cos_diff = 0.0f;
  avg_cos_diff.ResetAvg();

  trial_cos_diff = 0.0f;
  avg_trial_cos_diff.ResetAvg();
}

void LeabraNetwork::Init_Acts() {
  NET_THREAD_CALL(LeabraNetwork::Init_Acts_Thr);

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Init_Acts_Layer(this);
  }
}

void LeabraNetwork::Init_Netins() {
  NET_THREAD_CALL(LeabraNetwork::Init_Netins_Thr);
}

void LeabraNetwork::Init_Netins_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Init_Netins(uv, this, thr_no);
  }
}

void LeabraNetwork::DecayState(float decay) {
  tmp_arg1 = decay;
  NET_THREAD_CALL(LeabraNetwork::DecayState_Thr);
}

void LeabraNetwork::DecayState_Thr(int thr_no) {
  float decay = tmp_arg1;
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->DecayState(uv, this, thr_no, decay);
  }
}

void LeabraNetwork::CheckInhibCons() {
  net_misc.inhib_cons = false;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->CheckInhibCons(this);
  }
}

void LeabraNetwork::Build() {
  CheckInhibCons();
  if(net_misc.inhib_cons) {
    SetNetFlag(NETIN_PER_PRJN);	// inhib cons use per-prjn inhibition
  }
  inherited::Build();

#ifdef CUDA_COMPILE
  Cuda_BuildUnits_Threads();
#endif
}

#ifdef NUMA_COMPILE
void LeabraNetwork::AllocSendNetinTmp_Thr(int thr_no) {
  inherited::AllocSendNetinTmp_Thr(thr_no);
}
#endif

void LeabraNetwork::AllocSendNetinTmp() {
  // temporary storage for sender-based netinput computation
  if(n_units_built == 0 || threads.n_threads == 0) return;

  net_aligned_malloc((void**)&thrs_send_d5bnet_tmp, n_thrs_built * sizeof(float*));

#ifndef NUMA_COMPILE
  for(int i=0; i<n_thrs_built; i++) {
    net_aligned_malloc((void**)&thrs_send_d5bnet_tmp[i],
                       n_units_built * sizeof(float));
  }
#endif

  inherited::AllocSendNetinTmp();
}

void LeabraNetwork::InitSendNetinTmp_Thr(int thr_no) {
  inherited::InitSendNetinTmp_Thr(thr_no);
  memset(thrs_send_d5bnet_tmp[thr_no], 0, n_units_built * sizeof(float));
}


///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LeabraNetwork::Trial_Init() {
  unlearnable_trial = false;
  Trial_Init_Counters();
  Trial_Init_Specs();

  Trial_Init_Unit(); // performs following at unit-level
  //  Trial_Init_SRAvg();
  //  Trial_DecayState();
  //  Trial_NoiseInit(); 
  Trial_Init_Layer();
}

void LeabraNetwork::Trial_Init_Counters() {
  cycle = 0;
  quarter = 0;
  phase = MINUS_PHASE;
  rt_cycles = -1;          // signal that nothing has been recorded
}

void LeabraNetwork::Trial_Init_Specs() {
  net_misc.ti = false;
  net_misc.deep5b_cons = false;
  net_misc.dwt_norm = false;
  net_misc.lay_gp_inhib = false;
  net_misc.diff_scale_p = false;
  net_misc.lrate_updtd = false;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Trial_Init_Specs(this);
  }
  if(net_misc.lrate_updtd) {
    taMisc::Info("cur_lrate updated at epoch:", String(epoch));
#ifdef CUDA_COMPILE
    Cuda_UpdateConParams();     // push new params to cuda
#endif
  }
}

void LeabraNetwork::Trial_Init_Unit() {
  // todo: we have to do this non-threaded for now because of the noise generator problem.
  // (using LOOP instead of CALL)
  NET_THREAD_LOOP(LeabraNetwork::Trial_Init_Unit_Thr);
}

void LeabraNetwork::Trial_Init_Unit_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Trial_Init_Unit(uv, this, thr_no);
  }
}

void LeabraNetwork::Trial_Init_Layer() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Trial_Init_Layer(this);
  }
}

///////////////////////////////////////////////////////////////////////
//      QuarterInit -- at start of settling

void LeabraNetwork::Quarter_Init() {
  // todo: consolidate the 2 unit-level threaded guys into one..

  Quarter_Init_Counters();
  Quarter_Init_Unit();           // do chunk of following unit-level functions:
//   Quarter_Init_TargFlags();
//   Compute_NetinScale();              // compute net scaling
  Quarter_Init_Layer();

  Compute_NetinScale_Senders(); // second phase after recv-based NetinScale
  // put it after Quarter_Init_Layer to allow for mods to netin scale in that guy..

  Compute_HardClamp();          // clamp all hard-clamped input acts: not easily threadable
}

void LeabraNetwork::Quarter_Init_Counters() {
  // set the phase according to the counter 0,1,2 = MINUS, 3 = PLUS
  if(quarter <= 2)
    phase = MINUS_PHASE;
  else
    phase = PLUS_PHASE;
}

void LeabraNetwork::Quarter_Init_Unit() {
#if 1                           // todo: try this out
  NET_THREAD_CALL(LeabraNetwork::Quarter_Init_Unit_Thr);
#else
  for(int i=1; i<n_units_built; i++) {
    int thr_no = UnThr(i);
    LeabraUnitVars* uv = (LeabraUnitVars*)UnUnitVars(i);
    ((LeabraUnitSpec*)uv->unit_spec)->Quarter_Init_Unit(uv, this, thr_no);
  }
#endif
}

void LeabraNetwork::Quarter_Init_Unit_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Quarter_Init_Unit(uv, this, thr_no);
  }
}

void LeabraNetwork::Quarter_Init_Layer() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Quarter_Init_Layer(this);
  }
}

void LeabraNetwork::Quarter_Init_TargFlags() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  NET_THREAD_CALL(LeabraNetwork::Quarter_Init_TargFlags_Thr);

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Quarter_Init_TargFlags_Layer(this);
  }
}

void LeabraNetwork::Quarter_Init_TargFlags_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Quarter_Init_TargFlags(uv, this, thr_no);
  }
}

void LeabraNetwork::Compute_NetinScale() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NetinScale_Thr);
}

void LeabraNetwork::Compute_NetinScale_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_NetinScale(uv, this, thr_no);
  }
}

void LeabraNetwork::Compute_NetinScale_Senders() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NetinScale_Senders_Thr);
}

void LeabraNetwork::Compute_NetinScale_Senders_Thr(int thr_no) {
  // NOTE: this IS called by default -- second phase of Quarter_Init_Unit
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LeabraConGroup* scg = (LeabraConGroup*)ThrSendConGroup(thr_no, i);
    if(scg->NotActive()) {
      scg->scale_eff = 0.0f;
      continue;
    }
    LeabraConGroup* rcg = (LeabraConGroup*)scg->UnCons(0, this);
    scg->scale_eff = rcg->scale_eff;
  }
}

void LeabraNetwork::Compute_HardClamp() {
  NET_THREAD_CALL(LeabraNetwork::Compute_HardClamp_Thr);

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_HardClamp_Layer(this);
  }
}

void LeabraNetwork::Compute_HardClamp_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_HardClamp(uv, this, thr_no);
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
  Quarter_Init_Layer();
  Quarter_Init_TargFlags();
  Compute_HardClamp();
}

////////////////////////////////////////////////////////////////
//      Cycle_Run

void LeabraNetwork::Cycle_Run() {
  NET_THREAD_CALL(LeabraNetwork::Cycle_Run_Thr);
}

void LeabraNetwork::Cycle_Run_Thr(int thr_no) {
  int tot_cyc = 1;
  if(times.cycle_qtr)
    tot_cyc = times.quarter;
  for(int cyc = 0; cyc < tot_cyc; cyc++) {
    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->netin.StartTimer(true); // reset
    Send_Netin_Thr(thr_no);
    threads.SyncSpin(thr_no);
    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->netin.EndIncrAvg();

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->netin_integ.StartTimer(true); // reset
    Compute_NetinInteg_Thr(thr_no);
    threads.SyncSpin(thr_no);
    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->netin_integ.EndIncrAvg();

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->inhib.StartTimer(true); // reset
    Compute_Inhib_Thr(thr_no);
    threads.SyncSpin(thr_no);
    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->inhib.EndIncrAvg();

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->act.StartTimer(true); // reset
    Compute_Act_Thr(thr_no);
    threads.SyncSpin(thr_no);
    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->act.EndIncrAvg();

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->act_post.StartTimer(true); // reset
    Compute_Act_Post_Thr(thr_no);
    threads.SyncSpin(thr_no);
    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->act_post.EndIncrAvg();

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->cycstats.StartTimer(true); // reset
    if(thr_no == 0) {
      Compute_CycleStats_Pre();
    }

    Compute_CycleStats_Layer_Thr(thr_no);
    
    if(thr_no == 0) {
      Compute_CycleStats_Post();
    }

    threads.SyncSpin(thr_no);
    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->cycstats.EndIncrAvg();

    if(thr_no == 0)
      Cycle_IncrCounters();
  }
}

void LeabraNetwork::Cycle_IncrCounters() {
  cycle++;
  tot_cycle++;
  time += times.time_inc;
}

///////////////////////////////////////////////////////
//      Cycle Stage 1: netinput


void LeabraNetwork::Send_Netin_Thr(int thr_no) {
// #ifdef CUDA_COMPILE
//   Cuda_Send_Netin();
//   return;
// #endif

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Send_NetinDelta(uv, this, thr_no);
  }
  if(send_pct_tot > 0) {        // only avail for one-threaded calls
    send_pct = (float)send_pct_n / (float)send_pct_tot;
    avg_send_pct.Increment(send_pct);
  }
}

void LeabraNetwork::Compute_NetinInteg_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_NetinInteg(uv, this, thr_no);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition

void LeabraNetwork::Compute_Inhib_Thr(int thr_no) {
  // note: only running on thr_no == 0 right now -- may be best overall to avoid
  // messy cache stuff, to just keep it on 0
  if(thr_no == 0) {
    FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
      if(lay->lesioned()) continue;
      lay->Compute_Inhib(this);
    }
    if(net_misc.lay_gp_inhib)
      Compute_Inhib_LayGp();
  }
}

void LeabraNetwork::Compute_Inhib_LayGp() {
  if(!net_misc.lay_gp_inhib || layers.gp.size == 0) return;
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
        lay->i_val.laygp_g_i = lay_gp_g_i;
        lay->i_val.g_i = MAX(lay->i_val.laygp_g_i, lay->i_val.g_i);

        if(lay->unit_groups) {
          lay->Compute_LayInhibToGps(this);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation

void LeabraNetwork::Compute_Act_Post_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_Act_Post(uv, this, thr_no);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraNetwork::Compute_CycleStats_Pre() {
  // stats are never threadable due to updating at higher levels
  trg_max_act = 0.0f;
  init_netins_cycle_stat = false;
}

void LeabraNetwork::Compute_CycleStats_Layer_Thr(int thr_no) {
  if(thr_no == 0) {
    FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
      if(lay->lesioned()) continue;
      lay->Compute_CycleStats(this);
    }
  }
}

void LeabraNetwork::Compute_CycleStats_Post() {
  Compute_OutputName();
  Compute_RTCycles();
  // todo: eliminate this if possible -- just kinda hacky..
  if(init_netins_cycle_stat) {
    Init_Netins();
  }
}

void LeabraNetwork::Compute_OutputName() {
  output_name = "";             // this will be updated by layer
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_OutputName(this);
  }
}

void LeabraNetwork::Compute_RTCycles() {
  if(phase != MINUS_PHASE) return;
  if(rt_cycles > 0) return;  // already set
  if(trg_max_act > lstats.trg_max_act_crit)
    rt_cycles = cycle;
}

///////////////////////////////////////////////////////////////////////
//      Quarter Final

void LeabraNetwork::Quarter_Final() {
  Quarter_Final_Pre();
  NET_THREAD_CALL(LeabraNetwork::Quarter_Final_Unit_Thr);
  Quarter_Final_Layer();
  Quarter_Compute_dWt();
  Quarter_Final_Counters();
}

void LeabraNetwork::Quarter_Final_Pre() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Quarter_Final_Pre(this);
  }
}

void LeabraNetwork::Quarter_Final_Unit_Thr(int thr_no) {
  if(net_misc.ti) {
    Send_TICtxtNetin_Thr(thr_no);
  }
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Quarter_Final(uv, this, thr_no);
  }
}

void LeabraNetwork::Quarter_Final_Layer() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Quarter_Final_Layer(this);
  }
  if(quarter == 3) {            // last quarter
    Compute_CosDiff();
    Compute_AvgActDiff();
    Compute_TrialCosDiff();
  }
}

void LeabraNetwork::Quarter_Compute_dWt() {
  if(train_mode == TEST)
    return;

  Compute_dWt();                // always call -- let units sort it out..
}

void LeabraNetwork::Quarter_Final_Counters() {
  quarter++;                    // always shows +1 at end of quarter
}

///////////////////////////////////////////////////////////////////////
//      LeabraTI context updating

void LeabraNetwork::Send_TICtxtNetin_Thr(int thr_no) {
#ifdef CUDA_COMPILE
  Cuda_Send_TICtxtNetin();
  const int nu = units_flat.size;
  // always need to roll up the netinput into unit vals
  for(int i=1;i<nu;i++) {   // 0 = dummy idx
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    u->Send_TICtxtNetin_Post(this);
  }
  return;
#endif

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Send_TICtxtNetin(uv, this, thr_no);
  }

  threads.SyncSpin(thr_no);

  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Send_TICtxtNetin_Post(uv, this, thr_no);
  }
}

void LeabraNetwork::ClearTICtxt() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->ClearTICtxt(this);
  }
}



///////////////////////////////////////////////////////////////////////
//      Trial Update and Final

void LeabraNetwork::Trial_Final() {
  Compute_AbsRelNetin();
}

///////////////////////////////////////////////////////////////////////
//      Learning

void LeabraNetwork::Compute_dWt_Layer_pre() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_dWt_Layer_pre(this);
  }
}

void LeabraNetwork::Compute_dWt_vecvars() {
  // todo: fix this to work per thread -- but just not sure how much benefit it will have..
  float* avg_s = UnVecVar(AVG_S);
  float* avg_m = UnVecVar(AVG_M);
  float* avg_l = UnVecVar(AVG_L);
  float* thal =  UnVecVar(THAL);
  float* cos_diff_lmix =  UnVecVar(COS_DIFF_LMIX);
  float* act_q0 =  UnVecVar(ACT_Q0);
  // float* act_m = UnVecVar(ACT_M);
  // float* act_p = UnVecVar(ACT_P);

  // non-threaded for now..
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    avg_s[i] = un->avg_s();
    avg_m[i] = un->avg_m();
    avg_l[i] = un->avg_l();
    thal[i] = un->thal();
    act_q0[i] = un->act_q0();

    LeabraLayer* rlay = un->own_lay();
    cos_diff_lmix[i] = rlay->cos_diff_avg_lmix;

    // act_m[i] = un->act_m;
    // act_p[i] = un->act_p;
  }
}


void LeabraNetwork::Compute_dWt() {
  if(lstats.cos_err_lrn_thr > -1.0f) {		  // note: requires computing err before calling this!
    if(cos_err < lstats.cos_err_lrn_thr) return; // didn't make threshold
  }

  Compute_dWt_Layer_pre();
  // Compute_dWt_vecvars(); // todo

#ifdef CUDA_COMPILE
  if(!net_misc.dwt_norm) {      // todo: add other checks here for non-std dwts etc
    Cuda_Compute_dWt();
    return;
  }
#endif

  NET_THREAD_CALL(LeabraNetwork::Compute_dWt_Thr);

}

void LeabraNetwork::Compute_dWt_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->dwt.StartTimer(true); // reset

  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LeabraConGroup* scg = (LeabraConGroup*)ThrSendConGroup(thr_no, i);
    if(scg->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)scg->con_spec;
    if(!cs->Quarter_LearnNow(this->quarter)) return;
    cs->Compute_dWt(scg, this, thr_no);
  }

  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->dwt.EndIncrAvg();
}

void LeabraNetwork::Compute_dWt_Norm_Thr(int thr_no) {
  if(!net_misc.dwt_norm) return;
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    LeabraConGroup* rcg = (LeabraConGroup*)ThrRecvConGroup(thr_no, i);
    if(rcg->NotActive()) continue;
    ((LeabraConSpec*)rcg->con_spec)->Compute_dWt_Norm(rcg, this, thr_no);
  }
}

void LeabraNetwork::Compute_Weights_impl() {
#ifdef CUDA_COMPILE
  Cuda_Compute_Weights();
  return;
#endif

  NET_THREAD_CALL(LeabraNetwork::Compute_Weights_Thr);
}

///////////////////////////////////////////////////////////////////////
//      Stats

void LeabraNetwork::LayerAvgAct(DataTable* report_table, LeabraLayerSpec* lay_spec) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!report_table) {
    report_table = proj->GetNewAnalysisDataTable(name + "_LayerAvgAct", true);
  }
  int idx;
  report_table->StructUpdate(true);
  report_table->ResetData();
  DataCol* ln = report_table->FindMakeColName("layer", idx, VT_STRING);
  DataCol* lsn = report_table->FindMakeColName("layer_spec", idx, VT_STRING);
  DataCol* actm = report_table->FindMakeColName("acts_m_avg", idx, VT_FLOAT);
  DataCol* actp = report_table->FindMakeColName("acts_p_avg", idx, VT_FLOAT);
  DataCol* init = report_table->FindMakeColName("avg_act_init", idx, VT_FLOAT);

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    if(lay_spec != NULL) {
      if(ls != lay_spec) continue;
    }
    report_table->AddBlankRow();
    ln->SetValAsString(lay->name, -1);
    lsn->SetValAsString(ls->name, -1);
    actm->SetValAsFloat(lay->acts_m_avg, -1);
    actp->SetValAsFloat(lay->acts_p_avg, -1);
    init->SetValAsFloat(ls->avg_act.init, -1);
  }
  report_table->StructUpdate(false);
  tabMisc::DelayedFunCall_gui(report_table, "BrowserSelectMe");
}

void LeabraNetwork::Set_ExtRew(bool avail, float ext_rew_val) {
  ext_rew_avail = avail;
  ext_rew = ext_rew_val;
}

void LeabraNetwork::Compute_ExtRew() {
  // assumes any ext rew computation has happened before this point, and set the
  // network ext_rew and ext_rew_avail flags appropriately
  if(ext_rew_avail) {
    avg_ext_rew.Increment(ext_rew);
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

    if(lstats.cos_err_lrn_thr > -1.0f) {
      if(cos_err < lstats.cos_err_lrn_thr) return; // didn't make threshold - don't add to global
    }
    avg_norm_err.Increment(norm_err);
  }
  else {
    norm_err = 0.0f;
  }
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
    if(net_misc.ti && lay_vals > 0) {
      cosvp += l->cos_err_prv;
      cosvsp += l->cos_err_vs_prv;
      n_lays++;
    }
  }
  if(n_lays > 0) {
    cosv /= (float)n_lays;
    cos_err = cosv;
    avg_cos_err.Increment(cos_err);

    if(net_misc.ti) {
      cosvp /= (float)n_lays;
      cos_err_prv = cosvp;
      avg_cos_err_prv.Increment(cos_err_prv);

      cosvsp /= (float)n_lays;
      cos_err_vs_prv = cosvsp;
      avg_cos_err_vs_prv.Increment(cos_err_vs_prv);
    }
  }
  else {
    cos_err = 0.0f;
    cos_err_prv = 0.0f;
    cos_err_vs_prv = 0.0f;
  }
  return cosv;
}

float LeabraNetwork::Compute_CosDiff() {
  float cosv = 0.0f;
  int n_lays = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    float lcosv = l->Compute_CosDiff(this);
    if(!l->HasExtFlag(UnitVars::TARG | UnitVars::COMP)) {
      cosv += lcosv;
      n_lays++;
    }
  }
  if(n_lays > 0) {
    cosv /= (float)n_lays;

    cos_diff = cosv;
    avg_cos_diff.Increment(cos_diff);
  }
  else {
    cos_diff = 0.0f;
  }
  return cosv;
}

float LeabraNetwork::Compute_AvgActDiff() {
  float adiff = 0.0f;
  int n_lays = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    float ladiff = l->Compute_AvgActDiff(this);
    if(!l->HasExtFlag(UnitVars::TARG | UnitVars::COMP)) {
      adiff += ladiff;
      n_lays++;
    }
  }
  if(n_lays > 0) {
    adiff /= (float)n_lays;

    avg_act_diff = adiff;
    avg_avg_act_diff.Increment(avg_act_diff);
  }
  else {
    avg_act_diff = 0.0f;
  }
  return adiff;
}

float LeabraNetwork::Compute_TrialCosDiff() {
  float cosv = 0.0f;
  int n_lays = 0;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, layers) {
    if(l->lesioned()) continue;
    float lcosv = l->Compute_TrialCosDiff(this);
    if(!l->HasExtFlag(UnitVars::TARG | UnitVars::COMP)) {
      cosv += lcosv;
      n_lays++;
    }
  }
  if(n_lays > 0) {
    cosv /= (float)n_lays;

    trial_cos_diff = cosv;
    avg_trial_cos_diff.Increment(trial_cos_diff);
  }
  else {
    trial_cos_diff = 0.0f;
  }
  return cosv;
}

void LeabraNetwork::Compute_TrialStats() {
  taMisc::Warning("do not call Compute_TrialStats() anymore -- it is obsolete -- use Compute_PhaseStats or Compute_MinusStats / Compute_PlusStats for more appropriate stats computation at the right time");
}

void LeabraNetwork::Compute_PhaseStats() {
  if(quarter == 3)
    Compute_MinusStats();
  else if(quarter == 4)
    Compute_PlusStats();
}

void LeabraNetwork::Compute_MinusStats() {
  minus_output_name = output_name; // grab and hold..
  if(rt_cycles < 0) // never reached target
    rt_cycles = cycle;       // set to current cyc -- better for integrating
  avg_cycles.Increment(rt_cycles);

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->minus_output_name = lay->output_name;
  }
}

void LeabraNetwork::Compute_PlusStats() {
  if(unlearnable_trial) {
    sse = 0.0f;                 // ignore errors..
    norm_err = 0.0f;
    cos_err = 0.0f;
    cos_err_prv = 0.0f;
    cos_err_vs_prv = 0.0f;
  }
  else {
    Compute_SSE(stats.sse_unit_avg, stats.sse_sqrt);
    if(stats.prerr)
      Compute_PRerr();
    Compute_NormErr();
    Compute_CosErr();
    Compute_ExtRew();
  }
}

void LeabraNetwork::Compute_AbsRelNetin() {
  // always get layer-level netin max / avg values
  // decision of whether to run prjn-level is done by layers
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

void LeabraNetwork::Compute_AvgCycles() {
  avg_cycles.GetAvg_Reset();
}

void LeabraNetwork::Compute_AvgSendPct() {
  avg_send_pct.GetAvg_Reset();
}

void LeabraNetwork::Compute_AvgExtRew() {
  avg_ext_rew.GetAvg_Reset();
}

void LeabraNetwork::Compute_AvgNormErr() {
  avg_norm_err.GetAvg_Reset();
}

void LeabraNetwork::Compute_AvgCosErr() {
  avg_cos_err.GetAvg_Reset();

  if(net_misc.ti) {
    avg_cos_err_prv.GetAvg_Reset();
    avg_cos_err_vs_prv.GetAvg_Reset();
  }
}

void LeabraNetwork::Compute_AvgCosDiff() {
  avg_cos_diff.GetAvg_Reset();
}

void LeabraNetwork::Compute_AvgAvgActDiff() {
  avg_avg_act_diff.GetAvg_Reset();
}

void LeabraNetwork::Compute_AvgTrialCosDiff() {
  avg_trial_cos_diff.GetAvg_Reset();
}


void LeabraNetwork::Compute_EpochStats() {
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
  Compute_AvgNormErr();
  Compute_AvgCosErr();
  Compute_AvgCosDiff();
  Compute_AvgAvgActDiff();
  Compute_AvgTrialCosDiff();
  Compute_AvgExtRew();
  Compute_AvgSendPct();
  Compute_AvgAbsRelNetin();
}

String LeabraNetwork::MemoryReport(bool print) {

  int consz = sizeof(LeabraCon) + 3 * sizeof(int);
  // 3 = send idx, 2 recv idx's

  String constr;
  constr.convert((float)n_cons);
  return constr;

  // todo: move to Network, add raw n_recv,send cons_cnt

  // String report = name + " memory report:\n";
  // report << "number of units:       " << n_units << "\n"
  //        << "    bytes per unit:    " << sizeof(LeabraUnit) << "\n"
  //        << "    total unit memory: " << taMisc::GetSizeString(n_units * sizeof(LeabraUnit)) << "\n"
  //        << "number of connections: " << constr << "\n"
  //        << "    bytes per con+idx: " << consz << "\n"
  //        << "    total con memory:  " << taMisc::GetSizeString(n_send_cons_cnt * sizeof(float) + n_recv_cons_cnt * sizeof(int)) << "\n" 
  //        << "       owned (send):   " << taMisc::GetSizeString(n_send_cons_cnt * sizeof(float)) << "\n" 
  //        << "       ptr (recv):     " << taMisc::GetSizeString(n_recv_cons_cnt * sizeof(int)) << "\n";
  // if(print)
  //   taMisc::Info(report);
  // return report;
}

#ifdef CUDA_COMPILE

void LeabraNetwork::Cuda_BuildUnits_Threads() {
  if(taMisc::is_loading)        // ignore all the loading-time ones
    return;

  int send_net_max_prj = 1;
  if(NetinPerPrjn()) {
    send_net_max_prj = max_prjns;
  }
  cudai->AllocCudaArrays
    (n_units, cuda.min_threads, cuda.max_threads, cuda.cons_per_thread, 
     own_cons_max_size, own_cons_avg_size, own_cons_cnt, ptr_cons_cnt,
     own_units_x_cons, ptr_units_x_cons, own_cons_mem, ptr_cons_mem,
     send_netin_tmp.el, send_net_max_prj, send_d5bnet_tmp.el, unit_vec_vars.el);

  if(n_units == 0 || own_units_x_cons == 0) return;

  taMisc::Info("CUDA using:", String(cudai->n_threads), "threads for avg number of cons:",
               String(own_cons_avg_size));

  const int nu = units_flat.size;

  int uncn = 0;
  for(int i=1;i<nu;i++) {     // 0 = dummy idx
    Unit* un = units_flat[i];
    bool first = true;
    for(int p=0;p<un->send.size;p++) {
      SendCons* sc = un->send[p];
      if(!sc->PrjnIsActive()) continue;
      if(first) {
        cudai->unit_starts_h[i] = uncn;
        first = false;
      }
      cudai->units_h[uncn] = i;
      cudai->con_mem_idxs_h[uncn] = sc->mem_idx;
      cudai->con_allocs_h[uncn] = sc->alloc_size;
      cudai->con_sizes_h[uncn] = sc->size;
      cudai->con_recv_idxs_h[uncn] = sc->recv_idx();

      ++uncn;
    }
  }

  cudai->UpdateUnitsXCons();
  cudai->OwnCons_HostToDevice(true); // sync
  Cuda_UpdateConParams();
}

void LeabraNetwork::Cuda_UpdateConParams() {
  const int nu = units_flat.size;

  bool first = true;
  int uncn = 0;
  for(int i=1;i<nu;i++) {     // 0 = dummy idx
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    for(int p=0;p<un->send.size;p++) {
      LeabraConGroup* sc = (LeabraConGroup*)un->send[p];
      if(!sc->PrjnIsActive()) continue;
      LeabraConSpec* cs = (LeabraConSpec*)sc->GetConSpec();

      cudai->ConParam_h(uncn, LeabraConSpecCuda::S_MIX) = cs->xcal.s_mix;
      cudai->ConParam_h(uncn, LeabraConSpecCuda::M_MIX) = cs->xcal.m_mix;
      cudai->ConParam_h(uncn, LeabraConSpecCuda::THR_L_MIX) = cs->xcal.thr_l_mix;
      cudai->ConParam_h(uncn, LeabraConSpecCuda::THR_MAX) = cs->xcal.thr_max;
      cudai->ConParam_h(uncn, LeabraConSpecCuda::CUR_LRATE) = cs->cur_lrate;

      if(first) {
        cudai->wt_sig_fun_h = cs->wt_sig_fun.el;
        first = false;
      }

      ++uncn;
    }
  }

  cudai->UpdateConParams();
}


void LeabraNetwork::Cuda_Send_Netin() {
  if(cuda.timers_on)
    cuda_send_netin_time.StartRun(true); // using averaging, so reset used..

  const int nu = units_flat.size;
  int cur_snd = 0;
  for(int i=1; i<nu; i++) {
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    float act_ts = u->act;
    if(us->syn_delay.on) {
      if(!u->act_buf)
        us->Init_ActBuff(u);
      act_ts = u->act_buf->CircSafeEl(0); // get first logical element..
    }

    if(act_ts > us->opt_thresh.send) {
      float act_delta = act_ts - u->act_sent;
      if(fabsf(act_delta) > us->opt_thresh.delta) {
        int uncn = cudai->unit_starts_h[i];
        for(int g=0; g<u->send.size; g++) {
          LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
          if(send_gp->NotActive()) continue;
          LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
          LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
          if(!tol->hard_clamped && cs->DoesStdNetin()) {
            // note: all other netin types REQUIRE a CUDA impl because the weights
            // only live (updated) on the GPU device..
            cudai->cur_units_x_cons_h[cur_snd] = uncn;
            cudai->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
            cur_snd++;
          }
          uncn++;               // needs to track all
        }
        u->act_sent = act_ts;
      }
    }
    else if(u->act_sent > us->opt_thresh.send) {
      float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
      int uncn = cudai->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(!tol->hard_clamped && cs->DoesStdNetin()) {
          cudai->cur_units_x_cons_h[cur_snd] = uncn;
          cudai->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
      u->act_sent = 0.0f;         // now it effectively sent a 0..
    }
  }
  cudai->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cudai->Send_NetinDelta();
  }

  if(net_misc.deep5b_cons) {
    Cuda_Send_Deep5bNetin();
  }

  if(cuda.timers_on) {
    cuda_send_netin_time.EndRun();
    cuda_send_netin_time.IncrAvg();
  }
}

void LeabraNetwork::Cuda_Send_Deep5bNetin() {
  const int nu = units_flat.size;
  int cur_snd = 0;
  for(int i=1; i<nu; i++) {
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(!us->Quarter_Deep5bNow(quarter)) continue;
    float act_ts = u->act;
    // note: no delay for 5b
    // if(us->syn_delay.on) {
    //   if(!u->act_buf)
    //     us->Init_ActBuff(u);
    //   act_ts = u->act_buf->CircSafeEl(0); // get first logical element..
    // }

    if(act_ts > us->opt_thresh.send) {
      float act_delta = act_ts - u->d5b_sent;
      if(fabsf(act_delta) > us->opt_thresh.delta) {
        int uncn = cudai->unit_starts_h[i];
        for(int g=0; g<u->send.size; g++) {
          LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
          if(send_gp->NotActive()) continue;
          LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
          LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
          if(!tol->hard_clamped && cs->IsDeep5bCon()) {
            cudai->cur_units_x_cons_h[cur_snd] = uncn;
            cudai->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
            cur_snd++;
          }
          uncn++;               // needs to track all
        }
        u->d5b_sent = act_ts;
      }
    }
    else if(u->d5b_sent > us->opt_thresh.send) {
      float act_delta = - u->d5b_sent; // un-send the last above-threshold activation to get back to 0
      int uncn = cudai->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(!tol->hard_clamped && cs->IsDeep5bCon()) {
          cudai->cur_units_x_cons_h[cur_snd] = uncn;
          cudai->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
      u->d5b_sent = 0.0f;         // now it effectively sent a 0..
    }
  }
  cudai->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cudai->Send_Deep5bNetinDelta();
  }
}

void LeabraNetwork::Cuda_Send_TICtxtNetin() {
  const int nu = units_flat.size;
  int cur_snd = 0;
  for(int i=1; i<nu; i++) {
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(!us->Quarter_SendTICtxtNow(quarter))
      continue;
    
    float act_ts = u->act_eq;
    if(us->cifer_d5b.on) {
      act_ts *= us->cifer_d5b.ti_5b_c;
      act_ts += us->cifer_d5b.ti_5b * u->deep5b;
    }

    if(act_ts > us->opt_thresh.send) {
      int uncn = cudai->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsTICtxtCon()) {
          cudai->cur_units_x_cons_h[cur_snd] = uncn;
          cudai->send_net_acts_h[cur_snd] = act_ts * send_gp->scale_eff;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
    }
  }
  cudai->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cudai->Send_TICtxtNetin();
  }
}

void LeabraNetwork::Cuda_Compute_dWt() {
  if(cuda.timers_on)
    cuda_compute_dwt_time.StartRun(true); // using averaging, so reset used..

  const int nu = units_flat.size;
  int cur_snd = 0;
  for(int i=1; i<nu; i++) {
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(us->Quarter_LearnNow(quarter)) {
      if(u->avg_s >= us->opt_thresh.xcal_lrn || u->avg_m >= us->opt_thresh.xcal_lrn) {
        int uncn = cudai->unit_starts_h[i];
        for(int g=0; g<u->send.size; g++) {
          LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
          if(send_gp->NotActive()) continue;
          LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
          if(cs->DoesStdDwt()) {
            // exclude non-standard here -- def need for TICtxt for example!
            // requires a whole separate duplication of this process for each type
            cudai->cur_units_x_cons_h[cur_snd] = uncn;
            cur_snd++;
          }
          uncn++;               // needs to track all
        }
      }
      // bias weights are separate from CUDA
      LeabraLayer* lay = u->own_lay();
      LeabraConSpec* bspc = ((LeabraConSpec*)us->bias_spec.SPtr());
      bspc->B_Compute_dWt(&u->bias, u, lay);
    }
  }
  cudai->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cudai->Compute_dWt(true);      // sync -- need to do this so compute_weights (or tictxt) is ok..
  }

  if(net_misc.ti) {
    Cuda_Compute_dWt_TICtxt();
  }

  if(cuda.timers_on)
    cuda_compute_dwt_time.EndRun();

  if(cur_snd > 0) {
    if(cuda.timers_on)
      cuda_compute_dwt_time.StartWait(true);
    if(cuda.get_wts)
      cudai->OwnCons_DeviceToHost(true); // sync
    if(cuda.timers_on)
      cuda_compute_dwt_time.EndWait();
  }

  if(cuda.timers_on)
    cuda_compute_dwt_time.IncrAvg();
}

void LeabraNetwork::Cuda_Compute_dWt_TICtxt() {
  if(!net_misc.ti) return;

  const int nu = units_flat.size;
  int cur_snd = 0;
  for(int i=1; i<nu; i++) {
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(us->Quarter_LearnNow(quarter)) {
      int uncn = cudai->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsTICtxtCon()) {
          cudai->cur_units_x_cons_h[cur_snd] = uncn;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
    }
  }
  cudai->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cudai->Compute_dWt_TICtxt(true);      // sync -- need to do it for compute_weights
  }
}

void LeabraNetwork::Cuda_Compute_Weights() {
  if(cuda.timers_on)
    cuda_compute_wt_time.StartRun(true); // using averaging, so reset used..

  // bias weights are separate from CUDA
  const int nu = units_flat.size;
  for(int i=1; i<nu; i++) {
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    LeabraConSpec* bspc = ((LeabraConSpec*)us->bias_spec.SPtr());
    bspc->B_Compute_Weights(&u->bias, u);
  }

  cudai->Compute_Weights(true);      // sync -- todo: make this a param

  if(cuda.timers_on)
    cuda_compute_wt_time.EndRun();
  if(cuda.get_wts) {
    if(cuda.timers_on)
      cuda_compute_wt_time.StartWait(true);
    cudai->OwnCons_DeviceToHost(true); // sync
    if(cuda.timers_on)
      cuda_compute_wt_time.EndWait();
  }

  if(cuda.timers_on)
    cuda_compute_wt_time.IncrAvg();
}

void LeabraNetwork::Cuda_ConStateToHost() {
  cudai->OwnCons_DeviceToHost(true); // sync
}

void LeabraNetwork::Cuda_ConStateToDevice() {
  cudai->OwnCons_HostToDevice(true); // sync
}

String LeabraNetwork::Cuda_MemoryReport(bool print) {
  int64_t h_size = 0;
  int64_t d_size = 0;

  h_size += cudai->own_units_x_cons * sizeof(int); // units_h
  d_size += cudai->own_units_x_cons * sizeof(int);

  h_size += cudai->own_units_x_cons * sizeof(bigint); // con_mem_idxs_h 
  d_size += cudai->own_units_x_cons * sizeof(bigint);

  h_size += cudai->own_units_x_cons * sizeof(int); // con_allocs_h
  d_size += cudai->own_units_x_cons * sizeof(int);

  h_size += cudai->own_units_x_cons * sizeof(int); // con_sizes_h
  d_size += cudai->own_units_x_cons * sizeof(int);

  h_size += cudai->own_units_x_cons * sizeof(int); // con_recv_idxs_h
  d_size += cudai->own_units_x_cons * sizeof(int);

  h_size += (cudai->n_units+1) * sizeof(int); // unit_starts_h 

  h_size += cudai->own_units_x_cons * LeabraConSpecCuda::N_CON_PARAMS * sizeof(float); // con_params_h
  d_size += cudai->own_units_x_cons * LeabraConSpecCuda::N_CON_PARAMS * sizeof(float);

  d_size += 10002 * sizeof(float); // wt_sig_fun_d

  h_size += cudai->own_units_x_cons * sizeof(int); // cur_units_x_cons_h
  d_size += cudai->own_units_x_cons * sizeof(int);

  h_size += cudai->own_units_x_cons * sizeof(float); // send_net_acts_h
  d_size += cudai->own_units_x_cons * sizeof(float);

  d_size += (cudai->n_units+1) * cudai->send_net_max_prjns * sizeof(float); // send_netin_tmp_d

  d_size += (cudai->n_units+1) * sizeof(float); // send_d5bnet_tmp_d

  d_size += (cudai->n_units+1) * N_VEC_VARS * sizeof(float); // unit_vec_vars_d

  int64_t d_con_size = cudai->own_cons_cnt * sizeof(float); // own_cons_mem_d
  int64_t h_con_size = own_cons_cnt * sizeof(float); // own_cons_mem_h
  int64_t h_ptr_size = ptr_cons_cnt * sizeof(float); // ptr_cons_mem_h

  String report = "CUDA memory report:\n";
  report << "total device memory: " << taMisc::GetSizeString(d_size + d_con_size) << "\n"
         << "        connections: " << taMisc::GetSizeString(d_con_size) << "\n"
         << "        overhead:    " << taMisc::GetSizeString(d_size) << "\n"
         << " host   overhead:    " << taMisc::GetSizeString(h_size) << "\n"
         << " host   connections: " << taMisc::GetSizeString(h_con_size + h_ptr_size) << "\n"
         << " number of units:    " << n_units << "\n"
         << " number of cons:     " << taMisc::GetSizeString(n_cons) << "\n";

  if(print)
    taMisc::Info(report);
  return report;
}

String LeabraNetwork::Cuda_TimingReport(bool print) {
  String report = "CUDA timing report:\n\
numbers are average microseconds per call of a given type\n\n";
  report << "send netin:  " << cuda_send_netin_time.ReportAvg(1.0e6) << "\n";
  report << "compute dwt: " << cuda_compute_dwt_time.ReportAvg(1.0e6) << "\n";
  report << "compute wt:  " << cuda_compute_wt_time.ReportAvg(1.0e6) << "\n";
  if(print)
    taMisc::Info(report);
  return report;
}

#else // NO CUDA_COMPILE

void LeabraNetwork::Cuda_ConStateToHost() {
}

void LeabraNetwork::Cuda_ConStateToDevice() {
}

String LeabraNetwork::Cuda_MemoryReport(bool print) {
  taMisc::Info("CUDA not compiled!");
  return "";
}

String LeabraNetwork::Cuda_TimingReport(bool print) {
  taMisc::Info("CUDA not compiled!");
  return "";
}

#endif // CUDA_COMPILE
