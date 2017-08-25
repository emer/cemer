// Copyright 2017, Regents of the University of Colorado,
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
#include <MSNConSpec>

#include <taProject>
#include <DataTable>
#include <Program>
#include <taMisc>
#include <tabMisc>

// #ifdef CUDA_COMPILE
// #include "LeabraConSpec_cuda.h"
// #endif

TA_BASEFUNS_CTORS_DEFN(LeabraNetTiming);
TA_BASEFUNS_CTORS_DEFN(LeabraTimes);
TA_BASEFUNS_CTORS_DEFN(LeabraNetStats);
TA_BASEFUNS_CTORS_DEFN(LeabraNetMisc);
TA_BASEFUNS_CTORS_DEFN(LeabraNetDeep);
TA_BASEFUNS_CTORS_DEFN(RelNetinSched);
TA_BASEFUNS_CTORS_DEFN(LeabraNetwork);


void LeabraTimes::Initialize() {
  quarter = 25;
  cycle_qtr = true;
  deep_cyc = 5;
  time_inc = 0.001f;
  wt_bal_int = 10;

  minus = 3 * quarter;
  plus = quarter;
  total_cycles = minus + plus;
  thal_gate_cycle = -2;
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
  agg_unlearnable = false;
  wt_bal = false;
}

void LeabraNetMisc::Initialize() {
  spike = false;
  bias_learn = false;
  trial_decay = false;
  diff_scale_p = false;
  diff_scale_q1 = false;
  wt_bal = false;
  lay_gp_inhib = false;
  inhib_cons = false;
}

void LeabraNetDeep::Initialize() {
  on = false;
  ctxt = false;
  raw_net = false;
  mod_net = false;
  raw_qtr = QNULL;
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

  n_lay_stats = N_LeabraThrLayStats;

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
  net_sd = 0.0f;
  hog_pct = 0.0f;
  dead_pct = 0.0f;

  unit_vec_vars = NULL;
  thrs_send_deeprawnet_tmp = NULL;
  thrs_send_deepmodnet_tmp = NULL;
  thrs_lay_avg_max_vals = NULL;
  thrs_ungp_avg_max_vals = NULL;
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

  BuildLeabraThreadMem();

  // taMisc::Info("sizeof:", String(sizeof(LeabraUnitVars)), ".size:",
  //              String(unit_vars_size));
}

void LeabraNetwork::BuildLeabraThreadMem() {
  net_aligned_malloc((void**)&unit_vec_vars, n_thrs_built * sizeof(float*));
  net_aligned_malloc((void**)&thrs_lay_avg_max_vals, n_thrs_built * sizeof(char*));
  net_aligned_malloc((void**)&thrs_ungp_avg_max_vals, n_thrs_built * sizeof(char*));

  for(int i=0; i<n_thrs_built; i++) {
    net_aligned_malloc((void**)&unit_vec_vars[i], n_units_built
                       * N_VEC_VARS * sizeof(float));
    net_aligned_malloc((void**)&thrs_lay_avg_max_vals[i], n_layers_built
                       * N_AM_VARS * sizeof(AvgMaxValsRaw));
    net_aligned_malloc((void**)&thrs_ungp_avg_max_vals[i], n_ungps_built
                       * N_AM_VARS * sizeof(AvgMaxValsRaw));
  }

  NET_THREAD_CALL(LeabraNetwork::InitLeabraThreadMem_Thr);
}

void LeabraNetwork::InitLeabraThreadMem_Thr(int thr_no) {
  memset(unit_vec_vars[thr_no], 0, n_units_built * N_VEC_VARS * sizeof(float));
  memset(thrs_lay_avg_max_vals[thr_no], 0, n_layers_built * N_AM_VARS *
         sizeof(AvgMaxValsRaw));
  memset(thrs_ungp_avg_max_vals[thr_no], 0, n_ungps_built * N_AM_VARS *
         sizeof(AvgMaxValsRaw));
}

void LeabraNetwork::FreeUnitConGpThreadMem() {
  inherited::FreeUnitConGpThreadMem();

  if(!unit_vec_vars) return;

  for(int i=0; i<n_thrs_built; i++) {
    net_free((void**)&thrs_send_deeprawnet_tmp[i]);
    net_free((void**)&thrs_send_deepmodnet_tmp[i]);
    net_free((void**)&unit_vec_vars[i]);
    net_free((void**)&thrs_lay_avg_max_vals[i]);
    net_free((void**)&thrs_ungp_avg_max_vals[i]);
  }
  net_free((void**)&thrs_send_deeprawnet_tmp);
  net_free((void**)&thrs_send_deepmodnet_tmp);
  net_free((void**)&unit_vec_vars);
  net_free((void**)&thrs_lay_avg_max_vals);
  net_free((void**)&thrs_ungp_avg_max_vals);
}

void LeabraNetwork::BuildNullUnit() {
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new LeabraUnit, this);
  }
}

void LeabraNetwork::AllocSendNetinTmp() {
  // note: not calling Network: version -- need to update based on that!
  // temporary storage for sender-based netinput computation
  if(n_units_built == 0 || threads.n_threads == 0) return;

  net_aligned_malloc((void**)&thrs_send_netin_tmp, n_thrs_built * sizeof(float*));
  net_aligned_malloc((void**)&thrs_send_deeprawnet_tmp, n_thrs_built * sizeof(float*));
  net_aligned_malloc((void**)&thrs_send_deepmodnet_tmp, n_thrs_built * sizeof(float*));

  for(int i=0; i<n_thrs_built; i++) {
    if(NetinPerPrjn()) {
      net_aligned_malloc((void**)&thrs_send_netin_tmp[i],
                         n_units_built * max_prjns * sizeof(float));
    }
    else {
      net_aligned_malloc((void**)&thrs_send_netin_tmp[i],
                         n_units_built * sizeof(float));
    }
    net_aligned_malloc((void**)&thrs_send_deeprawnet_tmp[i],
                       n_units_built * sizeof(float));
    net_aligned_malloc((void**)&thrs_send_deepmodnet_tmp[i],
                       n_units_built * sizeof(float));
  }

  NET_THREAD_CALL(Network::InitSendNetinTmp_Thr);
}

void LeabraNetwork::InitSendNetinTmp_Thr(int thr_no) {
  InitCycleNetinTmp_Thr(thr_no);
  InitDeepRawNetinTmp_Thr(thr_no);
  // just in case deep flags are off - cycle uses those..
  memset(thrs_send_deepmodnet_tmp[thr_no], 0, n_units_built * sizeof(float));
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

  avg_act_diff = 0.0f;
  avg_avg_act_diff.ResetAvg();
  
  trial_cos_diff = 0.0f;
  avg_trial_cos_diff.ResetAvg();

  net_sd = 0.0f;
  avg_net_sd.ResetAvg();

  hog_pct = 0.0f;
  dead_pct = 0.0f;
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

void LeabraNetwork::Init_AdaptInhib() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Init_AdaptInhib(this);
  }
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

void LeabraNetwork::ResetSynTR() {
  NET_THREAD_CALL(LeabraNetwork::ResetSynTR_Thr);
}

void LeabraNetwork::ResetSynTR_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->ResetSynTR(uv, this, thr_no);
  }
}


///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LeabraNetwork::Trial_Init() {
  if(TestError(!IsBuiltIntact(), "Trial_Init",
               "Network is not built or is not intact -- must Build first")) {
    return;
  }
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
  times.thal_gate_cycle = -2;
  cycle = 0;
  quarter = 0;
  phase = MINUS_PHASE;
  rt_cycles = -1;          // signal that nothing has been recorded
  total_trials++;          // this is when we increment it!
}

void LeabraNetwork::Trial_Init_Specs() {
  net_misc.spike = false;
  net_misc.bias_learn = false;
  net_misc.trial_decay = false;
  net_misc.diff_scale_p = false;
  net_misc.diff_scale_q1 = false;
  net_misc.wt_bal = false;
  net_misc.lay_gp_inhib = false;
  net_misc.lrate_updtd = false;

  deep.on = false;
  deep.ctxt = false;
  deep.raw_net = false;
  deep.mod_net = false;
  deep.raw_qtr = LeabraNetDeep::QNULL;
  
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Trial_Init_Specs(this);
  }
  if(net_misc.lrate_updtd) {
    taMisc::Info("cur_lrate updated at epoch:", String(epoch));
// #ifdef CUDA_COMPILE
//     Cuda_UpdateConParams();     // push new params to cuda
// #endif
  }

  if(!deep.on) {
    TestWarning(deep.ctxt, "Trial_Init_Specs",
                "DeepCtxtConSpec's were found, but deep.on is not set -- no deep_ctxt will be computed!");
    TestWarning(deep.raw_net, "Trial_Init_Specs",
                "SendDeepRawConSpec's were found, but deep.on is not set -- no deep_raw_net will be computed!");
    TestWarning(deep.mod_net, "Trial_Init_Specs",
                "SendDeepModConSpec's were found, but deep.on is not set -- no deep_mod_net or deep_mod will be computed!");
  }
}

void LeabraNetwork::Trial_Init_Unit() {
  NET_THREAD_CALL(LeabraNetwork::Trial_Init_Unit_Thr);
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
  if(times.cycle_qtr) {
    if(Program::step_mode && Program::cur_step_prog.ptr() != NULL) {
      if(Program::cur_step_prog->name.contains("Cycle")) {
        times.cycle_qtr = false;
        taMisc::Info("Note: turned off times.cycle_qtr in network:",
                     name, "to step by single cycles -- this will slow down precessing significantly, so turn it back on if you want maximum speed later.");
      }
    }
  }
  
  Quarter_Init_Counters();
  Quarter_Init_Layer();
  Quarter_Init_Unit();           // do chunk of following unit-level functions:
//   Quarter_Init_TargFlags();
//   Compute_NetinScale();       // compute net scaling
//   Compute_HardClamp();        // clamp all hard-clamped input acts

  Quarter_Init_Deep();

  Compute_NetinScale_Senders(); // second phase after recv-based NetinScale
  // put it after Quarter_Init_Layer to allow for mods to netin scale in that guy..

  // also, super important to do this AFTER the Quarter_Init_Unit call so net is still
  // around for functions that use the previous value of it
  // NOTE: *everyone* has to init netins when scales change across quarters, because any existing netin has already been weighted at the previous scaled -- no way to rescale that aggregate -- just have to start over..
  if((phase == LeabraNetwork::PLUS_PHASE && net_misc.diff_scale_p) ||
     (quarter == 1 && net_misc.diff_scale_q1)) {
    Init_Netins();
  }
  
  Compute_HardClamp_Layer();
}

void LeabraNetwork::Quarter_Init_Counters() {
  // set the phase according to the counter 0,1,2 = MINUS, 3 = PLUS
  if(quarter <= 2)
    phase = MINUS_PHASE;
  else
    phase = PLUS_PHASE;
}

void LeabraNetwork::Quarter_Init_Unit() {
  NET_THREAD_CALL(LeabraNetwork::Quarter_Init_Unit_Thr);
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
    if(scg->NotActive()) continue;
    LeabraConGroup* rcg = (LeabraConGroup*)scg->UnCons(0, this);
    scg->scale_eff = rcg->scale_eff;
  }
}

void LeabraNetwork::Quarter_Init_Deep() {
  if(!deep.on) return;
  if(!deep.Quarter_DeepRawPrevQtr(quarter)) return; // nobody doing it now..

  NET_THREAD_CALL(LeabraNetwork::Quarter_Init_Deep_Thr);
}

void LeabraNetwork::Quarter_Init_Deep_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);

  if(deep.ctxt) {
    for(int i=0; i<nu; i++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
      if(uv->lesioned()) continue;
      ((LeabraUnitSpec*)uv->unit_spec)->Quarter_Init_Deep(uv, this, thr_no);
    }
    threads.SyncSpin(thr_no, 0);
    
    for(int i=0; i<nu; i++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
      if(uv->lesioned()) continue;
      ((LeabraUnitSpec*)uv->unit_spec)->Send_DeepCtxtNetin(uv, this, thr_no);
    }
    threads.SyncSpin(thr_no, 1);

    for(int i=0; i<nu; i++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
      if(uv->lesioned()) continue;
      ((LeabraUnitSpec*)uv->unit_spec)->Compute_DeepCtxt(uv, this, thr_no);
    }
    threads.SyncSpin(thr_no, 2);
    InitDeepRawNetinTmp_Thr(thr_no);

    Compute_DeepCtxtStats_Thr(thr_no);
    threads.SyncSpin(thr_no, 0);
    if(thr_no == 0) {
      Compute_DeepCtxtStats_Post();
    }
    threads.SyncSpin(thr_no, 1);
  }

  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_DeepStateUpdt(uv, this, thr_no);
  }
}

void LeabraNetwork::InitDeepRawNetinTmp_Thr(int thr_no) {
  memset(thrs_send_deeprawnet_tmp[thr_no], 0, n_units_built * sizeof(float));
}

void LeabraNetwork::Compute_DeepCtxtStats_Thr(int thr_no) {
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    AvgMaxValsRaw* am_ctxt = ThrLayAvgMax(thr_no, li, AM_DEEP_RAW);
    am_ctxt->InitVals();
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_ctxt->UpdtVals(uv->deep_ctxt, flat_idx); 
    }
  }

  // then unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    AvgMaxValsRaw* am_ctxt = ThrUnGpAvgMax(thr_no, li, AM_DEEP_RAW);
    am_ctxt->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_ctxt->UpdtVals(uv->deep_ctxt, flat_idx); 
    }
  }
}

void LeabraNetwork::Compute_DeepCtxtStats_Post() {
  // integrate all the data from thread-specific guys

  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    // if(lay->hard_clamped)
    // continue;
    AvgMaxVals& am_deep_ctxt = lay->am_deep_ctxt;
    am_deep_ctxt.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_ctxt = ThrLayAvgMax(i, li, AM_DEEP_RAW);
      am_deep_ctxt.UpdtFmAvgMaxRaw(*am_ctxt);
    }
    am_deep_ctxt.CalcAvg();
  }

  // then by unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    int ugidx = ActiveUnGp(li);
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(ugidx);
    AvgMaxVals& am_deep_ctxt = gpd->am_deep_ctxt;
    am_deep_ctxt.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_ctxt = ThrUnGpAvgMax(i, li, AM_DEEP_RAW);
      am_deep_ctxt.UpdtFmAvgMaxRaw(*am_ctxt);
    }
    am_deep_ctxt.CalcAvg();
  }
}

void LeabraNetwork::Compute_HardClamp() {
  // NOT called by default -- done in Quarter_Init_Unit
  NET_THREAD_CALL(LeabraNetwork::Compute_HardClamp_Thr);

  Compute_HardClamp_Layer();
}

void LeabraNetwork::Compute_HardClamp_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_HardClamp(uv, this, thr_no);
  }
}

void LeabraNetwork::Compute_HardClamp_Layer() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Compute_HardClamp_Layer(this);
  }
}

void LeabraNetwork::ExtToComp() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->ExtToComp(this);
  }

  NET_THREAD_CALL(LeabraNetwork::ExtToComp_Thr);
}

void LeabraNetwork::ExtToComp_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->ExtToComp(uv, this, thr_no);
  }
}

void LeabraNetwork::TargExtToComp() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->TargExtToComp(this);
  }

  NET_THREAD_CALL(LeabraNetwork::TargExtToComp_Thr);
}

void LeabraNetwork::TargExtToComp_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->TargExtToComp(uv, this, thr_no);
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
    Send_Netin_Thr(thr_no);
    threads.SyncSpin(thr_no, 0);

    Compute_NetinInteg_Thr(thr_no);
    threads.SyncSpin(thr_no, 1);

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->netin_stats.StartTimer(true); // reset

    Compute_NetinStats_Thr(thr_no);
    if(deep.mod_net) {
      Compute_DeepModStats_Thr(thr_no);
    }
    threads.SyncSpin(thr_no, 2);
    if(thr_no == 0) {
      Compute_NetinStats_Post();
      if(deep.mod_net) {
        Compute_DeepModStats_Post();
      }
    }
    threads.SyncSpin(thr_no, 0);

    InitCycleNetinTmp_Thr(thr_no);

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->netin_stats.EndIncrAvg();

    Compute_Inhib_Thr(thr_no);
    threads.SyncSpin(thr_no, 1);

    Compute_Act_Thr(thr_no);
    threads.SyncSpin(thr_no, 2);

    if(thr_no == 0) {
      Compute_CycleStats_Pre(); // prior to act post!
    }
    threads.SyncSpin(thr_no, 0);

    Compute_Act_Post_Thr(thr_no);
    threads.SyncSpin(thr_no, 1);

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->cycstats.StartTimer(true); // reset

    Compute_CycleStats_Thr(thr_no);
    threads.SyncSpin(thr_no, 2);
    
    if(thr_no == 0) {
      Compute_CycleStats_Post();
    }
    threads.SyncSpin(thr_no, 0);

    if(deep.on && deep.Quarter_DeepRawNow(quarter)) {
      int qtrcyc = cycle % times.quarter;
      if(qtrcyc % times.deep_cyc == 0) {
        Compute_DeepRaw_Thr(thr_no);
      }
    }
    threads.SyncSpin(thr_no, 1);

    if(threads.get_timing)
      ((LeabraNetTiming*)net_timing[thr_no])->cycstats.EndIncrAvg();

    if(thr_no == 0)
      Cycle_IncrCounters();
  }
}

void LeabraNetwork::Cycle_IncrCounters() {
  cycle++;
  tot_cycle++;
  time = (float)((int)round(time / times.time_inc) + 1) * times.time_inc;
  // crazy hoops to make sure that time is in precise increments of time_inc -- otherwise
  // can get significant drift..
}

///////////////////////////////////////////////////////
//      Cycle Stage 1: netinput


void LeabraNetwork::Send_Netin_Thr(int thr_no) {
// #ifdef CUDA_COMPILE
//   Cuda_Send_Netin();
//   return;
// #endif

  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->netin.StartTimer(true); // reset

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

  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->netin.EndIncrAvg();
}

void LeabraNetwork::Compute_NetinInteg_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->netin_integ.StartTimer(true); // reset

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_NetinInteg(uv, this, thr_no);
  }

  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->netin_integ.EndIncrAvg();
}

void LeabraNetwork::Compute_NetinStats_Thr(int thr_no) {
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    if(lay->hard_clamped)
      continue;
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    float net_thr = ls->inhib_misc.net_thr;
    if(ls->inhib_misc.thr_rel) {
      net_thr *= lay->netin.max; // from previous cycle
    }
    AvgMaxValsRaw* am_net = ThrLayAvgMax(thr_no, li, AM_NET);
    am_net->InitVals();
    AvgMaxValsRaw* am_net_raw = ThrLayAvgMax(thr_no, li, AM_NET_RAW);
    am_net_raw->InitVals();
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned() || uv->net < net_thr) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_net->UpdtVals(uv->net, flat_idx); 
      am_net_raw->UpdtVals(uv->net_raw, flat_idx); 
    }
  }

  // then unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    if(lay->hard_clamped)
      continue;
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    float net_thr = ls->inhib_misc.net_thr;
    if(ls->inhib_misc.thr_rel) {
      net_thr *= lay->netin.max; // from previous cycle
    }
    AvgMaxValsRaw* am_net = ThrUnGpAvgMax(thr_no, li, AM_NET);
    am_net->InitVals();
    AvgMaxValsRaw* am_net_raw = ThrUnGpAvgMax(thr_no, li, AM_NET_RAW);
    am_net_raw->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned() || uv->net < net_thr) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_net->UpdtVals(uv->net, flat_idx); 
      am_net_raw->UpdtVals(uv->net_raw, flat_idx); 
    }
  }
}

void LeabraNetwork::Compute_NetinStats_Post() {
  // integrate all the data from thread-specific guys

  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    if(lay->hard_clamped)
      continue;
    AvgMaxVals& netin = lay->netin;
    netin.InitVals();
    AvgMaxVals& netin_raw = lay->netin_raw;
    netin_raw.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_net = ThrLayAvgMax(i, li, AM_NET);
      netin.UpdtFmAvgMaxRaw(*am_net);
      AvgMaxValsRaw* am_net_raw = ThrLayAvgMax(i, li, AM_NET_RAW);
      netin_raw.UpdtFmAvgMaxRaw(*am_net_raw);
    }
    netin.CalcAvg();
    if(phase == MINUS_PHASE) {
      lay->netin_m = netin;
    }
    netin_raw.CalcAvg();
  }

  // then by unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    if(lay->hard_clamped)
      continue;
    int ugidx = ActiveUnGp(li);
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(ugidx);
    AvgMaxVals& netin = gpd->netin;
    netin.InitVals();
    AvgMaxVals& netin_raw = gpd->netin_raw;
    netin_raw.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_net = ThrUnGpAvgMax(i, li, AM_NET);
      netin.UpdtFmAvgMaxRaw(*am_net);
      AvgMaxValsRaw* am_net_raw = ThrUnGpAvgMax(i, li, AM_NET_RAW);
      netin_raw.UpdtFmAvgMaxRaw(*am_net_raw);
    }
    netin.CalcAvg();
    if(phase == MINUS_PHASE) {
      gpd->netin_m = netin;
    }
    netin_raw.CalcAvg();
  }
}

void LeabraNetwork::Compute_DeepModStats_Thr(int thr_no) {
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    AvgMaxValsRaw* am_net = ThrLayAvgMax(thr_no, li, AM_DEEP_NET);
    am_net->InitVals();
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_net->UpdtVals(uv->deep_mod_net, flat_idx); 
    }
  }

  // then unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    AvgMaxValsRaw* am_net = ThrUnGpAvgMax(thr_no, li, AM_DEEP_NET);
    am_net->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_net->UpdtVals(uv->deep_mod_net, flat_idx); 
    }
  }
}

void LeabraNetwork::Compute_DeepModStats_Post() {
  // integrate all the data from thread-specific guys

  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    // if(lay->hard_clamped)
    // continue;
    AvgMaxVals& am_deep_net = lay->am_deep_mod_net;
    am_deep_net.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_net = ThrLayAvgMax(i, li, AM_DEEP_NET);
      am_deep_net.UpdtFmAvgMaxRaw(*am_net);
    }
    am_deep_net.CalcAvg();
  }

  // then by unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    int ugidx = ActiveUnGp(li);
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(ugidx);
    AvgMaxVals& am_deep_net = gpd->am_deep_mod_net;
    am_deep_net.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_net = ThrUnGpAvgMax(i, li, AM_DEEP_NET);
      am_deep_net.UpdtFmAvgMaxRaw(*am_net);
    }
    am_deep_net.CalcAvg();
  }
}

void LeabraNetwork::InitCycleNetinTmp_Thr(int thr_no) {
  if(NetinPerPrjn()) {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * max_prjns * sizeof(float));
  }
  else {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * sizeof(float));
  }
  if(deep.mod_net) {
    memset(thrs_send_deepmodnet_tmp[thr_no], 0, n_units_built * sizeof(float));
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition

void LeabraNetwork::Compute_Inhib_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->inhib.StartTimer(true); // reset

  // note: only running on thr_no == 0 right now -- may be best overall to avoid
  // messy cache stuff, to just keep it on 0
  if(thr_no == 0) {
    if(net_misc.lay_gp_inhib)
      Compute_Inhib_LayGp();
    FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
      if(lay->lesioned()) continue;
      lay->Compute_Inhib(this);
    }
  }
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->inhib.EndIncrAvg();
}

void LeabraNetwork::Compute_Inhib_LayGp() {
  if(!net_misc.lay_gp_inhib || layers.gp.size == 0) return;
  for(int lgi = 0; lgi < layers.gp.size; lgi++) {
    Layer_Group* lg = (Layer_Group*)layers.gp[lgi];
    if(lg->size <= 1) continue;

    // use first layer as initial data for layer group
    LeabraLayer* lay0 = (LeabraLayer*)lg->FastEl(0);
    LeabraLayerSpec* laysp0 = (LeabraLayerSpec*)lay0->spec.SPtr();
    if(!laysp0->lay_gp_inhib.on) continue;
    
    lay0->laygp_data.netin.InitVals();
    lay0->laygp_data.acts.InitVals();
    for(int li = 0; li < lg->size; li++) {
      LeabraLayer* lay = (LeabraLayer*)lg->FastEl(li);
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;

      lay0->laygp_data.netin.UpdtFmAvgMax(lay->netin);
      lay0->laygp_data.acts.UpdtFmAvgMax(lay->acts);
    }

    lay0->laygp_data.netin.CalcAvg();
    lay0->laygp_data.acts.CalcAvg();
    laysp0->Compute_Inhib_impl(lay0, &lay0->laygp_data, this, laysp0->lay_gp_inhib);
    
    for(int li = 1; li < lg->size; li++) {
      LeabraLayer* lay = (LeabraLayer*)lg->FastEl(li);
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;
      
      lay->laygp_data = lay0->laygp_data;
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation

void LeabraNetwork::Compute_Act_Thr(int thr_no) {
  if(net_misc.spike) {
    Compute_Act_Spike_Thr(thr_no);
  }
  else {
    Compute_Act_Rate_Thr(thr_no);
  }
}

void LeabraNetwork::Compute_Act_Rate_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->act.StartTimer(true); // reset

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_Act_Rate(uv, this, thr_no);
  }

  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->act.EndIncrAvg();
}

void LeabraNetwork::Compute_Act_Spike_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->act.StartTimer(true); // reset

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_Act_Spike(uv, this, thr_no);
  }

  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->act.EndIncrAvg();
}

void LeabraNetwork::Compute_Act_Post_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->act_post.StartTimer(true); // reset

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_Act_Post(uv, this, thr_no);
  }

  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->act_post.EndIncrAvg();
}

void LeabraNetwork::ThalGatedNow() {
  times.thal_gate_cycle = cycle; // record current cycle
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraNetwork::Compute_CycleStats_Pre() {
  // stats are never threadable due to updating at higher levels
  trg_max_act = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_CycleStats_Pre(this);
  }
}

void LeabraNetwork::Compute_CycleStats_Thr(int thr_no) {
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  // this is when we should update clamped layers
  
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;

    AvgMaxValsRaw* am_act = ThrLayAvgMax(thr_no, li, AM_ACT);
    am_act->InitVals();
    AvgMaxValsRaw* am_act_raw = ThrLayAvgMax(thr_no, li, AM_ACT_RAW);
    am_act_raw->InitVals();
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_act->UpdtVals(uv->act, flat_idx); 
      am_act_raw->UpdtVals(uv->act_raw, flat_idx);
    }
  }

  // then unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    AvgMaxValsRaw* am_act = ThrUnGpAvgMax(thr_no, li, AM_ACT);
    am_act->InitVals();
    AvgMaxValsRaw* am_act_raw = ThrUnGpAvgMax(thr_no, li, AM_ACT_RAW);
    am_act_raw->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_act->UpdtVals(uv->act, flat_idx); 
      am_act_raw->UpdtVals(uv->act_raw, flat_idx); 
    }
  }
  
  if(net_misc.spike) {
    Compute_ActEqStats_Thr(thr_no); // act_eq requred separately for spiking
  }
}

void LeabraNetwork::Compute_ActEqStats_Thr(int thr_no) {
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  // this is when we should update clamped layers
  
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    AvgMaxValsRaw* am_act_eq = ThrLayAvgMax(thr_no, li, AM_ACT_EQ);
    am_act_eq->InitVals();
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_act_eq->UpdtVals(uv->act_eq, flat_idx);
    }
  }

  // then unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    AvgMaxValsRaw* am_act_eq = ThrUnGpAvgMax(thr_no, li, AM_ACT_EQ);
    am_act_eq->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_act_eq->UpdtVals(uv->act_eq, flat_idx);
    }
  }
}

void LeabraNetwork::Compute_CycleStats_Post() {
  // integrate all the data from thread-specific guys
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  // this is when we should update clamped layers

  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    AvgMaxVals& acts = lay->acts;
    AvgMaxVals& acts_eq = lay->acts_eq;
    AvgMaxVals& acts_raw = lay->acts_raw;
    acts.InitVals();
    acts_eq.InitVals();
    acts_raw.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_act = ThrLayAvgMax(i, li, AM_ACT);
      acts.UpdtFmAvgMaxRaw(*am_act);
      if(net_misc.spike) {
        AvgMaxValsRaw* am_act_eq = ThrLayAvgMax(i, li, AM_ACT_EQ);
        acts_eq.UpdtFmAvgMaxRaw(*am_act_eq);
      }
      else {
        acts_eq.UpdtFmAvgMaxRaw(*am_act); // use act!
      }
      AvgMaxValsRaw* am_act_raw = ThrLayAvgMax(i, li, AM_ACT_RAW);
      acts_raw.UpdtFmAvgMaxRaw(*am_act_raw);
    }
    acts.CalcAvg();
    acts_eq.CalcAvg();
    acts_raw.CalcAvg();
    if(lay->HasExtFlag(UnitVars::TARG)) {
      trg_max_act = fmaxf(trg_max_act, acts_eq.max);
    }
    if(lay->Iconified()) {
      lay->icon_value = acts_eq.avg;
    }
  }

  // then by unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    int ugidx = ActiveUnGp(li);
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(ugidx);
    AvgMaxVals& acts = gpd->acts;
    AvgMaxVals& acts_eq = gpd->acts_eq;
    AvgMaxVals& acts_raw = gpd->acts_raw;
    acts.InitVals();
    acts_eq.InitVals();
    acts_raw.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_act = ThrUnGpAvgMax(i, li, AM_ACT);
      acts.UpdtFmAvgMaxRaw(*am_act);
      if(net_misc.spike) {
        AvgMaxValsRaw* am_act_eq = ThrUnGpAvgMax(i, li, AM_ACT_EQ);
        acts_eq.UpdtFmAvgMaxRaw(*am_act_eq);
      }
      else {
        acts_eq.UpdtFmAvgMaxRaw(*am_act); // use act!
      }
      AvgMaxValsRaw* am_act_raw = ThrUnGpAvgMax(i, li, AM_ACT_RAW);
      acts_raw.UpdtFmAvgMaxRaw(*am_act_raw);
    }
    acts.CalcAvg();
    acts_eq.CalcAvg();
    acts_raw.CalcAvg();
  }

  Compute_OutputName();
  Compute_RTCycles();
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

void LeabraNetwork::Compute_GcIStats_Thr(int thr_no) {
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    AvgMaxValsRaw* am_un_g_i = ThrLayAvgMax(thr_no, li, AM_UN_G_I);
    am_un_g_i->InitVals();

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_un_g_i->UpdtVals(uv->gc_i, flat_idx);
    }
  }

  // then unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    AvgMaxValsRaw* am_un_g_i = ThrUnGpAvgMax(thr_no, li, AM_UN_G_I);
    am_un_g_i->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_un_g_i->UpdtVals(uv->gc_i, flat_idx);
    }
  }
}

void LeabraNetwork::Compute_GcIStats_Post() {
  // integrate all the data from thread-specific guys
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    AvgMaxVals& un_g_i = lay->un_g_i;
    un_g_i.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_un_g_i = ThrLayAvgMax(i, li, AM_UN_G_I);
      un_g_i.UpdtFmAvgMaxRaw(*am_un_g_i);
    }

    un_g_i.CalcAvg();
  }

  // then by unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    if(lay->hard_clamped && !updt_clamped)
      continue;
    int ugidx = ActiveUnGp(li);
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(ugidx);
    AvgMaxVals& un_g_i = lay->un_g_i;
    un_g_i.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_un_g_i = ThrUnGpAvgMax(i, li, AM_UN_G_I);
      un_g_i.UpdtFmAvgMaxRaw(*am_un_g_i);
    }

    un_g_i.CalcAvg();
  }
}

///////////////////////////////////////////////////////////////////////
//      DeepLeabra updates

void LeabraNetwork::Compute_DeepRaw_Thr(int thr_no) {
  // already tested that this is the time to call us, including quarter..
  
  const int nu = ThrNUnits(thr_no);

  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Compute_DeepRaw(uv, this, thr_no);
  }
  threads.SyncSpin(thr_no, 0);

  if(deep.raw_net) {
    for(int i=0; i<nu; i++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
      if(uv->lesioned()) continue;
      ((LeabraUnitSpec*)uv->unit_spec)->Send_DeepRawNetin(uv, this, thr_no);
    }
    threads.SyncSpin(thr_no, 1);

    for(int i=0; i<nu; i++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
      if(uv->lesioned()) continue;
      ((LeabraUnitSpec*)uv->unit_spec)->DeepRawNetin_Integ(uv, this, thr_no);
    }
    threads.SyncSpin(thr_no, 2);
    InitDeepRawNetinTmp_Thr(thr_no);
  }

  Compute_DeepRawStats_Thr(thr_no);
  threads.SyncSpin(thr_no, 0);
  if(thr_no == 0) {
    Compute_DeepRawStats_Post();
  }
}

void LeabraNetwork::Compute_DeepRawStats_Thr(int thr_no) {
  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    AvgMaxValsRaw* am_raw = ThrLayAvgMax(thr_no, li, AM_DEEP_RAW);
    am_raw->InitVals();
    AvgMaxValsRaw* am_net = ThrLayAvgMax(thr_no, li, AM_DEEP_NET);
    am_net->InitVals();
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_raw->UpdtVals(uv->deep_raw, flat_idx); 
      am_net->UpdtVals(uv->deep_raw_net, flat_idx); 
    }
  }

  // then unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    AvgMaxValsRaw* am_raw = ThrUnGpAvgMax(thr_no, li, AM_DEEP_RAW);
    am_raw->InitVals();
    AvgMaxValsRaw* am_net = ThrUnGpAvgMax(thr_no, li, AM_DEEP_NET);
    am_net->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = ThrUnitIdx(thr_no, ui); // note: max_i is now in flat_idx units
      am_raw->UpdtVals(uv->deep_raw, flat_idx); 
      am_net->UpdtVals(uv->deep_raw_net, flat_idx); 
    }
  }
}

void LeabraNetwork::Compute_DeepRawStats_Post() {
  // integrate all the data from thread-specific guys

  // first go by layers
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    // if(lay->hard_clamped)
    // continue;
    AvgMaxVals& am_deep_raw = lay->am_deep_raw;
    am_deep_raw.InitVals();
    AvgMaxVals& am_deep_net = lay->am_deep_raw_net;
    am_deep_net.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_raw = ThrLayAvgMax(i, li, AM_DEEP_RAW);
      am_deep_raw.UpdtFmAvgMaxRaw(*am_raw);
      AvgMaxValsRaw* am_net = ThrLayAvgMax(i, li, AM_DEEP_NET);
      am_deep_net.UpdtFmAvgMaxRaw(*am_net);
    }
    am_deep_raw.CalcAvg();
    am_deep_net.CalcAvg();
  }

  // then by unit groups
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveUnGpLayer(li);
    // if(lay->hard_clamped)
    //   continue;
    int ugidx = ActiveUnGp(li);
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(ugidx);
    AvgMaxVals& am_deep_raw = gpd->am_deep_raw;
    am_deep_raw.InitVals();
    AvgMaxVals& am_deep_net = gpd->am_deep_raw_net;
    am_deep_net.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      AvgMaxValsRaw* am_raw = ThrUnGpAvgMax(i, li, AM_DEEP_RAW);
      am_deep_raw.UpdtFmAvgMaxRaw(*am_raw);
      AvgMaxValsRaw* am_net = ThrUnGpAvgMax(i, li, AM_DEEP_NET);
      am_deep_net.UpdtFmAvgMaxRaw(*am_net);
    }
    am_deep_raw.CalcAvg();
    am_deep_net.CalcAvg();
  }
}

void LeabraNetwork::ClearDeepActs() {
  NET_THREAD_CALL(LeabraNetwork::ClearDeepActs_Thr);
}

void LeabraNetwork::ClearDeepActs_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->ClearDeepActs(uv, this, thr_no);
  }
}

void LeabraNetwork::ClearMSNTrace() {
  NET_THREAD_CALL(LeabraNetwork::ClearMSNTrace_Thr);
}

void LeabraNetwork::ClearMSNTrace_Thr(int thr_no) {
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LeabraConGroup* scg = (LeabraConGroup*)ThrSendConGroup(thr_no, i);
    if(scg->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)scg->con_spec;
    if(!cs->InheritsFrom(&TA_MSNConSpec)) continue;
    MSNConSpec* mscs = (MSNConSpec*)cs;
    mscs->ClearMSNTrace(scg, this, thr_no);
  }
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
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((LeabraUnitSpec*)uv->unit_spec)->Quarter_Final(uv, this, thr_no);
  }
  if(quarter == 3) {
    Compute_CosDiff_Thr(thr_no); // more efficient to lump here -- must come before Compute_dWt because cos_diff_avg_lrn is used 
  }
}

void LeabraNetwork::Quarter_Final_Layer() {
  if(quarter == 3) {
    Compute_CosDiff_Agg();      // aggregate from Unit_Thr
  }
  
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Quarter_Final_Layer(this);
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

void LeabraNetwork::Compute_dWt_VecVars_Thr(int thr_no) {
  float* avg_s = UnVecVar(thr_no, AVG_S);
  float* avg_m = UnVecVar(thr_no, AVG_M);
  float* avg_l = UnVecVar(thr_no, AVG_L);
  float* avg_l_lrn = UnVecVar(thr_no, AVG_L_LRN);
  // float* deep =  UnVecVar(thr_no, DEEP);
// #ifdef CUDA_COMPILE
//   float* act_q0 =  UnVecVar(thr_no, ACT_Q0);
// #endif

  // each thread copies all unit vars into their *own* thread-local mem in unit_vec_vars
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnitVars* u = (LeabraUnitVars*)UnUnitVars(i);
    avg_s[i] = u->avg_s_eff;    // key!
    avg_m[i] = u->avg_m;
    avg_l[i] = u->avg_l;
    avg_l_lrn[i] = u->avg_l_lrn;
    // deep[i] = u->deep_lrn;
// #ifdef CUDA_COMPILE
//     act_q0[i] = u->act_q0;
//     LeabraUnit* un = (LeabraUnit*)UnFmIdx(i);
//     LeabraLayer* rlay = un->own_lay();
// #endif
  }
}


void LeabraNetwork::Compute_dWt() {
  Compute_dWt_Layer_pre();

// #ifdef CUDA_COMPILE
//   Cuda_Compute_dWt();
//   return;
// #endif

  NET_THREAD_CALL(LeabraNetwork::Compute_dWt_Thr);
}

void LeabraNetwork::Compute_dWt_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->dwt.StartTimer(true); // reset

  // note: not currently using
  // Compute_dWt_VecVars_Thr(thr_no);
  // threads.SyncSpin(thr_no);

  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LeabraConGroup* scg = (LeabraConGroup*)ThrSendConGroup(thr_no, i);
    if(scg->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)scg->con_spec;
    if(!cs->Quarter_LearnNow(this->quarter)) continue;
    cs->Compute_dWt(scg, this, thr_no);
  }

  if(net_misc.bias_learn) {
    const int nu = ThrNUnits(thr_no);
    for(int i=0; i<nu; i++) {
      UnitVars* uv = ThrUnitVars(thr_no, i);
      if(uv->lesioned()) continue;
      uv->unit_spec->Compute_dWt(uv, this, thr_no); // just bias weights
    }
  }
  
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->dwt.EndIncrAvg();
}

void LeabraNetwork::Compute_Weights() {
#ifdef DMEM_COMPILE
  DMem_SumDWts(dmem_trl_comm.comm);
#endif

// #ifdef CUDA_COMPILE
//   Cuda_Compute_Weights();
//   return;
// #endif
  
  NET_THREAD_CALL(LeabraNetwork::Compute_Weights_Thr);
  
  if(net_misc.wt_bal && (total_trials % times.wt_bal_int == 0)) {
    NET_THREAD_CALL(LeabraNetwork::Compute_WtBal_Thr);
    if(lstats.wt_bal) {
      Compute_WtBalStats();
    }
  }

  SaveWeights_ClusterRunTerm();
}

void LeabraNetwork::Compute_Weights_Thr(int thr_no) {
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->wt.StartTimer(true); // reset

  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LeabraConGroup* scg = (LeabraConGroup*)ThrSendConGroup(thr_no, i);
    if(scg->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)scg->con_spec;
    cs->Compute_Weights(scg, this, thr_no);
  }

  if(net_misc.bias_learn) {
    const int nu = ThrNUnits(thr_no);
    for(int i=0; i<nu; i++) {
      UnitVars* uv = ThrUnitVars(thr_no, i);
      if(uv->lesioned()) continue;
      uv->unit_spec->Compute_Weights(uv, this, thr_no); // just bias weights
    }
  }
  
  if(threads.get_timing)
    ((LeabraNetTiming*)net_timing[thr_no])->wt.EndIncrAvg();
}

void LeabraNetwork::Compute_WtBal_Thr(int thr_no) {
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    LeabraConGroup* rcg = (LeabraConGroup*)ThrRecvConGroup(thr_no, i);
    if(rcg->NotActive()) continue;
    ((LeabraConSpec*)rcg->con_spec)->Compute_WtBal(rcg, this, thr_no);
  }
}

void LeabraNetwork::Compute_WtBalStats() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP_NESTED(LeabraPrjn, p, lay->projections) {
      if(p->NotActive()) continue;
      p->wt_avg_max = 0.0f;
      p->wt_avg_avg = 0.0f;
    }
    int denom = 0;
    FOREACH_ELEM_IN_GROUP_NESTED(LeabraUnit, u, lay->units) {
      if(u->lesioned()) continue;
      denom++;
      FOREACH_ELEM_IN_GROUP_NEST2(LeabraPrjn, p, lay->projections) {
        if(p->NotActive()) continue;
        LeabraConGroup* cg = (LeabraConGroup*)u->RecvConGroup(p->recv_idx);
        p->wt_avg_max = fmaxf(p->wt_avg_max, cg->wt_avg);
        p->wt_avg_avg += cg->wt_avg;
      }
    }
    if(denom > 0) {
      float norm = 1.0f / denom;
      FOREACH_ELEM_IN_GROUP_NEST2(LeabraPrjn, p, lay->projections) {
        if(p->NotActive()) continue;
        p->wt_avg_avg *= norm;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Stats

void LeabraNetwork::LayerAvgAct(DataTable* report_table, LeabraLayerSpec* lay_spec) {
  taProject* proj = GetMyProj();
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
    init->SetValAsFloat(ls->avg_act.targ_init, -1);
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

bool LeabraNetwork::AggPerfStats() {
  if(!unlearnable_trial) return true;
  return lstats.agg_unlearnable;
}

void LeabraNetwork::Compute_NormErr() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NormErr_Thr);
  Compute_NormErr_Agg();
}

void LeabraNetwork::Compute_NormErr_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    Layer* lay = ActiveLayer(li);
    if(!lay->HasExtFlag(UnitVars::COMP_TARG))
      continue;
    if(lay->layer_type == Layer::HIDDEN)
      continue;

    float lay_nerr = 0.0f;    float lay_trg_n = 0.0f;
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    bool targ_active = false;
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      lay_nerr += ((LeabraUnitSpec*)uv->unit_spec)->Compute_NormErr
        (uv, this, thr_no, targ_active);
      if(targ_active) lay_trg_n += 1.0f;
    }
    ThrLayStats(thr_no, li, 0, NORMERR) = lay_nerr;
    ThrLayStats(thr_no, li, 1, NORMERR) = lay_trg_n;
  }
}

void LeabraNetwork::Compute_NormErr_Agg() {
  float nerr_sum = 0.0f;
  float nerr_avail = 0.0f;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    lay->Compute_MaxErr(this);  // do this here as well..
    float nerr = lay->Compute_NormErr(this);
    if(nerr >= 0.0f) {
      nerr_avail += 1.0f;
      nerr_sum += nerr;
    }
  }
  if(nerr_avail > 0.0f) {
    norm_err = nerr_sum / nerr_avail; // normalize contribution across layers
    if(AggPerfStats())
      avg_norm_err.Increment(norm_err);
  }
  else {
    norm_err = 0.0f;
  }
}

float LeabraNetwork::Compute_CosErr() {
  NET_THREAD_CALL(LeabraNetwork::Compute_CosErr_Thr);
  return Compute_CosErr_Agg();
}

void LeabraNetwork::Compute_CosErr_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    Layer* lay = ActiveLayer(li);
    if(!lay->HasExtFlag(UnitVars::COMP_TARG))
      continue;
    if(lay->layer_type == Layer::HIDDEN)
      continue;

    float cosv = 0.0f;  float cosvp = 0.0f;  float ssm = 0.0f;
    float ssp = 0.0f;   float sst = 0.0f;    float nvals = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      if(!uv->HasExtFlag(UnitVars::COMP_TARG)) continue;
      nvals += 1.0f;
      cosv += uv->targ * uv->act_m;
      ssm += uv->act_m * uv->act_m;
      sst += uv->targ * uv->targ;
      if(net_misc.deep) {
        cosvp += uv->targ * uv->act_q0;
        ssp += uv->act_q0 * uv->act_q0;
      }
    }
    ThrLayStats(thr_no, li, 0, COSERR) = cosv;
    ThrLayStats(thr_no, li, 1, COSERR) = cosvp;
    ThrLayStats(thr_no, li, 2, COSERR) = ssm;
    ThrLayStats(thr_no, li, 3, COSERR) = ssp;
    ThrLayStats(thr_no, li, 4, COSERR) = sst;
    ThrLayStats(thr_no, li, 5, COSERR) = nvals;
  }
}

float LeabraNetwork::Compute_CosErr_Agg() {
  float cosv = 0.0f;
  float cosvp = 0.0f;
  float cosvsp = 0.0f;
  int n_lays = 0;
  int lay_vals = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    cosv += l->Compute_CosErr(this, lay_vals);
    if(lay_vals > 0) {
      n_lays++;
      if(net_misc.deep) {
        cosvp += l->cos_err_prv;
        cosvsp += l->cos_err_vs_prv;
      }
    }
  }
  if(n_lays > 0) {
    cosv /= (float)n_lays;
    cos_err = cosv;
    if(AggPerfStats())
      avg_cos_err.Increment(cos_err);

    if(net_misc.deep) {
      cosvp /= (float)n_lays;
      cos_err_prv = cosvp;
      if(AggPerfStats())
        avg_cos_err_prv.Increment(cos_err_prv);

      cosvsp /= (float)n_lays;
      cos_err_vs_prv = cosvsp;
      if(AggPerfStats())
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
  NET_THREAD_CALL(LeabraNetwork::Compute_CosDiff_Thr);
  return Compute_CosDiff_Agg();
}
    
void LeabraNetwork::Compute_CosDiff_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    const float avg_m = lay->acts_m.avg;
    const float avg_p = lay->acts_p.avg;

    float cosv = 0.0f;  float ssm = 0.0f;  float sst = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const float act_p = (uv->act_p - avg_p); // zero mean!
      const float act_m = (uv->act_m - avg_m);
      cosv += act_p * act_m;
      ssm += act_m * act_m;
      sst += act_p * act_p;
    }
    ThrLayStats(thr_no, li, 0, COSDIFF) = cosv;
    ThrLayStats(thr_no, li, 1, COSDIFF) = ssm;
    ThrLayStats(thr_no, li, 2, COSDIFF) = sst;
  }
}

float LeabraNetwork::Compute_CosDiff_Agg() {
  float cosv = 0.0f;
  int n_lays = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    float lcosv = l->Compute_CosDiff(this);
    if(!l->HasExtFlag(UnitVars::COMP_TARG)) {
      cosv += lcosv;
      n_lays++;
    }
  }
  // second step for sharing cos_diff-based lrate_mod
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    l->Compute_CosDiff_post(this);
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
  NET_THREAD_CALL(LeabraNetwork::Compute_AvgActDiff_Thr);
  return Compute_AvgActDiff_Agg();
}

void LeabraNetwork::Compute_AvgActDiff_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    Layer* lay = ActiveLayer(li);

    float adiff = 0.0f;  float nd = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      adiff += uv->act_dif;
      nd += 1.0f;
    }

    ThrLayStats(thr_no, li, 0, AVGACTDIFF) = adiff;
    ThrLayStats(thr_no, li, 1, AVGACTDIFF) = nd;
  }
}

float LeabraNetwork::Compute_AvgActDiff_Agg() {
  float adiff = 0.0f;
  int n_lays = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    float ladiff = l->Compute_AvgActDiff(this);
    if(!l->HasExtFlag(UnitVars::COMP_TARG)) {
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
  NET_THREAD_CALL(LeabraNetwork::Compute_TrialCosDiff_Thr);
  return Compute_TrialCosDiff_Agg();
}
    
void LeabraNetwork::Compute_TrialCosDiff_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    const float avg_p = lay->acts_p.avg;
    const float avg_q0 = lay->acts_p_avg; // use running average -- best we've got
    
    float cosv = 0.0f;  float ssm = 0.0f;  float sst = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const float act_p = uv->act_p - avg_p; // zero mean
      const float act_q0 = uv->act_q0 - avg_q0;
      cosv += act_p * act_q0;
      ssm += act_q0 * act_q0;
      sst += act_p * act_p;
    }

    ThrLayStats(thr_no, li, 0, TRIALCOSDIFF) = cosv;
    ThrLayStats(thr_no, li, 1, TRIALCOSDIFF) = ssm;
    ThrLayStats(thr_no, li, 2, TRIALCOSDIFF) = sst;
  }
}

float LeabraNetwork::Compute_TrialCosDiff_Agg() {
  float cosv = 0.0f;
  const int nlay = n_layers_built;
  int n_lays = 0;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    float lcosv = l->Compute_TrialCosDiff(this);
    if(!l->HasExtFlag(UnitVars::COMP_TARG)) {
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

void LeabraNetwork::Compute_ActMargin() {
  NET_THREAD_CALL(LeabraNetwork::Compute_ActMargin_Thr);
  Compute_ActMargin_Agg();
}
    
void LeabraNetwork::Compute_ActMargin_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    const float low_thr = lay->margin_low_thr;
    const float med_thr = lay->margin_med_thr;
    const float hi_thr = lay->margin_hi_thr;
    
    float low_avg = 0.0f;  float hi_avg = 0.0f;  float med_avg = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      const float v_m_eq = uv->v_m_eq;
      if(v_m_eq >= low_thr) {
        low_avg += 1.0f;
        if(v_m_eq > hi_thr)
          hi_avg += 1.0f;
        else if(v_m_eq <= med_thr)
          med_avg += 1.0f;
      }
    }

    ThrLayStats(thr_no, li, 0, ACTMARGIN) = low_avg;
    ThrLayStats(thr_no, li, 1, ACTMARGIN) = med_avg;
    ThrLayStats(thr_no, li, 2, ACTMARGIN) = hi_avg;
  }
}

void LeabraNetwork::Compute_ActMargin_Agg() {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    l->Compute_ActMargin(this);
  }
}

float LeabraNetwork::Compute_NetSd() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NetSd_Thr);
  return Compute_NetSd_Agg();
}
    
void LeabraNetwork::Compute_NetSd_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    float net_avg = lay->netin.avg;

    bool ugp = lay->HasUnitGpInhib();
    
    float var = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      float netsb;
      if(ugp) {
        net_avg = lay->UnGpDataUn(uv->Un(this, thr_no))->netin.avg;
      }
      netsb = (uv->net - net_avg);
      var += netsb * netsb;
    }
    ThrLayStats(thr_no, li, 0, NETSD) = var;
  }
}

float LeabraNetwork::Compute_NetSd_Agg() {
  float var = 0.0f;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    float lvar = l->Compute_NetSd(this);
    if(!l->HasExtFlag(UnitVars::EXT)) {
      var += lvar;
    }
  }
  net_sd = sqrt(var);
  avg_net_sd.Increment(net_sd);
  return net_sd;
}

void LeabraNetwork::Compute_HogDeadPcts() {
  NET_THREAD_CALL(LeabraNetwork::Compute_HogDeadPcts_Thr);
  Compute_HogDeadPcts_Agg();
}
    
void LeabraNetwork::Compute_HogDeadPcts_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* lay = (LeabraLayer*)ActiveLayer(li);
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();

    float hog = 0.0f;  float dead = 0.0f;  float nu = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      if(uv->act_avg > laysp->lstats.hog_thr) {
        hog += 1.0f;
      }
      else if(uv->act_avg < laysp->lstats.dead_thr) {
        dead += 1.0f;
      }
      nu += 1.0f;
    }

    ThrLayStats(thr_no, li, 0, HOGDEAD) = hog;
    ThrLayStats(thr_no, li, 1, HOGDEAD) = dead;
    ThrLayStats(thr_no, li, 2, HOGDEAD) = nu;
  }
}

void LeabraNetwork::Compute_HogDeadPcts_Agg() {
  float hog = 0.0f;
  float dead = 0.0f;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LeabraLayer* l = (LeabraLayer*)ActiveLayer(li);
    l->Compute_HogDeadPcts(this);
    hog += l->hog_pct;
    dead += l->dead_pct;
  }
  if(nlay > 0) {
    hog_pct = hog / (float)nlay;
    dead_pct = dead / (float)nlay;
  }
  else {
    hog_pct = 0.0f;
    dead_pct = 0.0f;
  }
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

  Compute_NetSd(); // todo: combine as in plus if more than one

  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->minus_output_name = lay->output_name;
  }
}

void LeabraNetwork::Compute_PlusStats_Thr(int thr_no) {
  Compute_SSE_Thr(thr_no);
  if(stats.prerr)
    Compute_PRerr_Thr(thr_no);
  Compute_NormErr_Thr(thr_no);
  Compute_CosErr_Thr(thr_no);
  // Compute_CosDiff_Thr(thr_no);
  // cosdiff must be computed prior to Compute_dWt, in Quarter_Final
  Compute_AvgActDiff_Thr(thr_no);
  Compute_TrialCosDiff_Thr(thr_no);
  Compute_ActMargin_Thr(thr_no);
  //  Compute_HogDeadPcts_Thr(thr_no);  // only in epoch
}

void LeabraNetwork::Compute_PlusStats_Agg() {
  Compute_SSE_Agg();
  if(stats.prerr)
    Compute_PRerr_Agg();
  Compute_NormErr_Agg();
  Compute_CosErr_Agg();
  // Compute_CosDiff_Agg();
  // cosdiff must be computed prior to Compute_dWt, in Quarter_Final
  Compute_AvgActDiff_Agg();
  Compute_TrialCosDiff_Agg();
  Compute_ActMargin_Agg();
  //  Compute_HogDeadPcts_Agg();  // only in epoch
}

void LeabraNetwork::Compute_PlusStats() {
  NET_THREAD_CALL(LeabraNetwork::Compute_PlusStats_Thr); // do all threading at once
  Compute_PlusStats_Agg();
  Compute_ExtRew();
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

  if(net_misc.deep) {
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

void LeabraNetwork::Compute_AvgNetSd() {
  avg_net_sd.GetAvg_Reset();
}

void LeabraNetwork::Compute_EpochStats() {
  Compute_EpochWeights();
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
  Compute_AvgNormErr();
  Compute_AvgCosErr();
  Compute_AvgCosDiff();
  Compute_AvgAvgActDiff();
  Compute_AvgTrialCosDiff();
  Compute_AvgNetSd();
  Compute_AvgExtRew();
  Compute_AvgSendPct();
  Compute_AvgAbsRelNetin();
  Compute_HogDeadPcts();
}

void LeabraNetwork::Compute_EpochWeights() {
  NET_THREAD_CALL(LeabraNetwork::Compute_EpochWeights_Thr);
}

void LeabraNetwork::Compute_EpochWeights_Thr(int thr_no) {
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LeabraConGroup* scg = (LeabraConGroup*)ThrSendConGroup(thr_no, i);
    if(scg->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)scg->con_spec;
    cs->Compute_EpochWeights(scg, this, thr_no);
  }
}

static String pct_val_out(float val, float sum) {
  String rval;
  rval = taMisc::FormatValue(val, 7, 3) + " " 
    + taMisc::FormatValue(100.0f * (val / sum), 7, 3);
  return rval;
}

String LeabraNetwork::TimingReport(DataTable& dt, bool print) {
  if(!HasNetFlag(BUILT)) {
    String rval = "Network not built yet!";
    if(print)
      taMisc::Info(rval);
    return rval;
  }

  int idx;
  DataCol* thc = dt.FindMakeColName("thread", idx, VT_INT);
  DataCol* stat = dt.FindMakeColName("stat", idx, VT_STRING);
  DataCol* rca = dt.FindMakeColName("run_avg", idx, VT_FLOAT);
  DataCol* rcs = dt.FindMakeColName("run_sum", idx, VT_FLOAT);
  float rescale_val = 1.0e6;        // how many microseconds
  float rescale = rescale_val;      // effective

  LeabraNetTiming* net_tm = (LeabraNetTiming*)net_timing.Peek();
  net_tm->netin.avg_used.ResetSum();
  net_tm->act.avg_used.ResetSum();
  net_tm->dwt.avg_used.ResetSum();
  net_tm->wt.avg_used.ResetSum();
  net_tm->netin_integ.avg_used.ResetSum();
  net_tm->netin_stats.avg_used.ResetSum();
  net_tm->inhib.avg_used.ResetSum();
  net_tm->act_post.avg_used.ResetSum();
  net_tm->cycstats.avg_used.ResetSum();

  float tot_time = 0.0f;
  float wait_time_avg = 0.0f;
  float wait_time_sum = 0.0f;
  for(int i=0; i<net_timing.size; i++) {
    LeabraNetTiming* tm = (LeabraNetTiming*)net_timing[i];
    if(i < n_thrs_built) {
      net_tm->netin.avg_used.IncrementAvg(tm->netin.avg_used.sum);
      net_tm->act.avg_used.IncrementAvg(tm->act.avg_used.sum);
      net_tm->dwt.avg_used.IncrementAvg(tm->dwt.avg_used.sum);
      net_tm->wt.avg_used.IncrementAvg(tm->wt.avg_used.sum);
      net_tm->netin_integ.avg_used.IncrementAvg(tm->netin_integ.avg_used.sum);
      net_tm->netin_stats.avg_used.IncrementAvg(tm->netin_stats.avg_used.sum);
      net_tm->inhib.avg_used.IncrementAvg(tm->inhib.avg_used.sum);
      net_tm->act_post.avg_used.IncrementAvg(tm->act_post.avg_used.sum);
      net_tm->cycstats.avg_used.IncrementAvg(tm->cycstats.avg_used.sum);
    }
    else {
      rescale = 1.0f;
      tot_time += tm->netin.avg_used.avg;
      tot_time += tm->act.avg_used.avg;
      tot_time += tm->dwt.avg_used.avg;
      tot_time += tm->wt.avg_used.avg;
      tot_time += tm->netin_integ.avg_used.avg;
      tot_time += tm->netin_stats.avg_used.avg;
      tot_time += tm->inhib.avg_used.avg;
      tot_time += tm->act_post.avg_used.avg;
      tot_time += tm->cycstats.avg_used.avg;
    }

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("netin_time", -1);
    rca->SetValAsFloat(tm->netin.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->netin.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("netin_integ_time", -1);
    rca->SetValAsFloat(tm->netin_integ.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->netin_integ.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("netin_stats_time", -1);
    rca->SetValAsFloat(tm->netin_stats.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->netin_stats.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("inhib_time", -1);
    rca->SetValAsFloat(tm->inhib.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->inhib.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("act_time", -1);
    rca->SetValAsFloat(tm->act.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->act.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("act_post_time", -1);
    rca->SetValAsFloat(tm->act_post.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->act_post.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("cycstats_time", -1);
    rca->SetValAsFloat(tm->cycstats.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->cycstats.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("dwt_time", -1);
    rca->SetValAsFloat(tm->dwt.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->dwt.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("wt_time", -1);
    rca->SetValAsFloat(tm->wt.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->wt.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("sync_time", -1);

    if(i < n_thrs_built) {
      TimeUsedHR& wt = ((NetworkThreadTask*)threads.tasks[i])->wait_time;
      rca->SetValAsFloat(wt.avg_used.avg * rescale, -1);
      rcs->SetValAsFloat(wt.avg_used.sum, -1);
      wait_time_avg += wt.avg_used.avg;
      wait_time_sum += wt.avg_used.sum;
    }
    else {
      wait_time_avg /= (float)n_thrs_built;
      wait_time_sum /= (float)n_thrs_built;

      rca->SetValAsFloat(wait_time_avg * rescale_val, -1);
      rcs->SetValAsFloat(wait_time_sum, -1);
    }
  }

  String report = name + " timing report:\n";
  report << "function       time     percent \n"
         << "netin:         " << pct_val_out(net_tm->netin.avg_used.avg, tot_time) << "\n"
         << "netin_integ:   " << pct_val_out(net_tm->netin_integ.avg_used.avg, tot_time) << "\n"
         << "netin_stats:   " << pct_val_out(net_tm->netin_stats.avg_used.avg, tot_time) << "\n"
         << "inhib:         " << pct_val_out(net_tm->inhib.avg_used.avg, tot_time) << "\n"
         << "act:           " << pct_val_out(net_tm->act.avg_used.avg, tot_time) << "\n"
         << "act_post:      " << pct_val_out(net_tm->act_post.avg_used.avg, tot_time) << "\n"
         << "cycstats:      " << pct_val_out(net_tm->cycstats.avg_used.avg, tot_time) << "\n"
         << "dwt:           " << pct_val_out(net_tm->dwt.avg_used.avg, tot_time) << "\n"
         << "wt:            " << pct_val_out(net_tm->wt.avg_used.avg, tot_time) << "\n"
         << "    total:     " << taMisc::FormatValue(tot_time, 7, 3) << "\n"
         << "sync wait:     " << taMisc::FormatValue(wait_time_sum, 7, 3) << "\n"
    ;

  if(print)
    taMisc::Info(report);
  return report;
}


// #ifdef CUDA_COMPILE
#if 0 // not enabled for now..

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

      cuda_net->ConParam_h(uncn, LeabraConSpecCuda::S_MIX) = cs->xcal.s_mix;
      cuda_net->ConParam_h(uncn, LeabraConSpecCuda::M_MIX) = cs->xcal.m_mix;
      cuda_net->ConParam_h(uncn, LeabraConSpecCuda::THR_L_MIX) = cs->xcal.thr_l_mix;
      // cuda_net->ConParam_h(uncn, LeabraConSpecCuda::THR_MAX) = cs->xcal.thr_max;
      cuda_net->ConParam_h(uncn, LeabraConSpecCuda::CUR_LRATE) = cs->cur_lrate;

      if(first) {
        cuda_net->wt_sig_fun_h = cs->wt_sig_fun.el;
        first = false;
      }

      ++uncn;
    }
  }

  cuda_net->UpdateConParams();
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
        int uncn = cuda_net->unit_starts_h[i];
        for(int g=0; g<u->send.size; g++) {
          LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
          if(send_gp->NotActive()) continue;
          LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
          LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
          if(!tol->hard_clamped && cs->DoesStdNetin()) {
            // note: all other netin types REQUIRE a CUDA impl because the weights
            // only live (updated) on the GPU device..
            cuda_net->cur_units_x_cons_h[cur_snd] = uncn;
            cuda_net->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
            cur_snd++;
          }
          uncn++;               // needs to track all
        }
        u->act_sent = act_ts;
      }
    }
    else if(u->act_sent > us->opt_thresh.send) {
      float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
      int uncn = cuda_net->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(!tol->hard_clamped && cs->DoesStdNetin()) {
          cuda_net->cur_units_x_cons_h[cur_snd] = uncn;
          cuda_net->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
      u->act_sent = 0.0f;         // now it effectively sent a 0..
    }
  }
  cuda_net->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cuda_net->Send_NetinDelta();
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
        int uncn = cuda_net->unit_starts_h[i];
        for(int g=0; g<u->send.size; g++) {
          LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
          if(send_gp->NotActive()) continue;
          LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
          LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
          if(!tol->hard_clamped && cs->IsDeep5bCon()) {
            cuda_net->cur_units_x_cons_h[cur_snd] = uncn;
            cuda_net->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
            cur_snd++;
          }
          uncn++;               // needs to track all
        }
        u->d5b_sent = act_ts;
      }
    }
    else if(u->d5b_sent > us->opt_thresh.send) {
      float act_delta = - u->d5b_sent; // un-send the last above-threshold activation to get back to 0
      int uncn = cuda_net->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(!tol->hard_clamped && cs->IsDeep5bCon()) {
          cuda_net->cur_units_x_cons_h[cur_snd] = uncn;
          cuda_net->send_net_acts_h[cur_snd] = act_delta * send_gp->scale_eff;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
      u->d5b_sent = 0.0f;         // now it effectively sent a 0..
    }
  }
  cuda_net->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cuda_net->Send_Deep5bNetinDelta();
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
      int uncn = cuda_net->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsTICtxtCon()) {
          cuda_net->cur_units_x_cons_h[cur_snd] = uncn;
          cuda_net->send_net_acts_h[cur_snd] = act_ts * send_gp->scale_eff;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
    }
  }
  cuda_net->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cuda_net->Send_TICtxtNetin();
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
        int uncn = cuda_net->unit_starts_h[i];
        for(int g=0; g<u->send.size; g++) {
          LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
          if(send_gp->NotActive()) continue;
          LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
          if(cs->DoesStdDwt()) {
            // exclude non-standard here -- def need for TICtxt for example!
            // requires a whole separate duplication of this process for each type
            cuda_net->cur_units_x_cons_h[cur_snd] = uncn;
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
  cuda_net->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cuda_net->Compute_dWt(true);      // sync -- need to do this so compute_weights (or tictxt) is ok..
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
      cuda_net->OwnCons_DeviceToHost(true); // sync
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
      int uncn = cuda_net->unit_starts_h[i];
      for(int g=0; g<u->send.size; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsTICtxtCon()) {
          cuda_net->cur_units_x_cons_h[cur_snd] = uncn;
          cur_snd++;
        }
        uncn++;               // needs to track all
      }
    }
  }
  cuda_net->cur_units_x_cons_n = cur_snd;

  if(cur_snd > 0) {
    cuda_net->Compute_dWt_TICtxt(true);      // sync -- need to do it for compute_weights
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

  cuda_net->Compute_Weights(true);      // sync -- todo: make this a param

  if(cuda.timers_on)
    cuda_compute_wt_time.EndRun();
  if(cuda.get_wts) {
    if(cuda.timers_on)
      cuda_compute_wt_time.StartWait(true);
    cuda_net->OwnCons_DeviceToHost(true); // sync
    if(cuda.timers_on)
      cuda_compute_wt_time.EndWait();
  }

  if(cuda.timers_on)
    cuda_compute_wt_time.IncrAvg();
}


#else // NO CUDA_COMPILE


#endif // CUDA_COMPILE
