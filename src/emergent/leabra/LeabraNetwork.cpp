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
// #include <MSNConSpec>

#include <taProject>
#include <DataTable>
#include <Program>
#include <taMisc>
#include <tabMisc>

// #ifdef CUDA_COMPILE
// #include "LeabraConSpec_cuda.h"
// #endif

TA_BASEFUNS_CTORS_DEFN(LeabraAvgMax);
TA_BASEFUNS_CTORS_DEFN(LeabraMarginVals);
TA_BASEFUNS_CTORS_DEFN(LeabraInhibVals);

TA_BASEFUNS_CTORS_DEFN(LeabraNetTiming);
TA_BASEFUNS_CTORS_DEFN(LeabraTimes);
TA_BASEFUNS_CTORS_DEFN(LeabraNetStats);
TA_BASEFUNS_CTORS_DEFN(LeabraNetMisc);
TA_BASEFUNS_CTORS_DEFN(LeabraNetDeep);
TA_BASEFUNS_CTORS_DEFN(RelNetinSched);
TA_BASEFUNS_CTORS_DEFN(LeabraNetwork);


void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);
  net_timing.SetBaseType(&TA_LeabraNetTiming);

  Initialize_net_core();
}

NetworkState_cpp* LeabraNetwork::NewNetworkState() const {
  return new LeabraNetworkState_cpp;
}

TypeDef* LeabraNetwork::NetworkStateType() const { return &TA_LeabraNetworkState_cpp; }
TypeDef* LeabraNetwork::LayerStateType() const { return &TA_LeabraLayerState_cpp; }
TypeDef* LeabraNetwork::PrjnStateType() const { return &TA_LeabraPrjnState_cpp; }
TypeDef* LeabraNetwork::UnGpStateType() const { return &TA_LeabraUnGpState_cpp; }
TypeDef* LeabraNetwork::UnitStateType() const { return &TA_LeabraUnitState_cpp; }
TypeDef* LeabraNetwork::ConStateType() const { return &TA_LeabraConState_cpp; }


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

  // taMisc::Info("sizeof:", String(sizeof(LeabraUnitState_cpp)), ".size:",
  //              String(unit_vars_size));
}

void LeabraNetwork::BuildLeabraThreadMem() {
  LeabraNetState()->AllocLeabraStatsMem();
  
  NET_THREAD_CALL(LeabraNetwork::InitLeabraThreadMem_Thr);
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

  Init_Acts_Layer();
}

void LeabraNetwork::Init_Netins() {
  NET_THREAD_CALL(LeabraNetwork::Init_Netins_Thr);
}

void LeabraNetwork::DecayState(float decay) {
  LeabraNetState()->tmp_arg1 = decay;
  NET_THREAD_CALL(LeabraNetwork::DecayState_Thr);
}

void LeabraNetwork::ResetSynTR() {
  NET_THREAD_CALL(LeabraNetwork::ResetSynTR_Thr);
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
    if(!lay->lesioned()) {
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      laysp->Trial_Init_Specs(lay, this);
    }
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

void LeabraNetwork::Quarter_Init_Unit() {
  NET_THREAD_CALL(LeabraNetwork::Quarter_Init_Unit_Thr);
}

void LeabraNetwork::Quarter_Init_TargFlags() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  NET_THREAD_CALL(LeabraNetwork::Quarter_Init_TargFlags_Thr);

  Quarter_Init_TargFlags_Layer();
}

void LeabraNetwork::Compute_NetinScale() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NetinScale_Thr);
}

void LeabraNetwork::Compute_NetinScale_Senders() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NetinScale_Senders_Thr);
}

void LeabraNetwork::Quarter_Init_Deep() {
  if(!deep.on) return;
  if(!deep.Quarter_DeepRawPrevQtr(quarter)) return; // nobody doing it now..

  NET_THREAD_CALL(LeabraNetwork::Quarter_Init_Deep_Thr);
}


void LeabraNetwork::Compute_HardClamp() {
  // NOT called by default -- done in Quarter_Init_Unit
  NET_THREAD_CALL(LeabraNetwork::Compute_HardClamp_Thr);

  Compute_HardClamp_Layer();
}

void LeabraNetwork::ExtToComp() {
  ExtToComp_Layer();

  NET_THREAD_CALL(LeabraNetwork::ExtToComp_Thr);
}

void LeabraNetwork::TargExtToComp() {
  TargExtToComp_Layer();

  NET_THREAD_CALL(LeabraNetwork::TargExtToComp_Thr);
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


///////////////////////////////////////////////////////
//      Cycle Stage 1: netinput



void LeabraNetwork::Compute_Inhib_LayGp() {
  if(!net_misc.lay_gp_inhib || layers.gp.size == 0) return;
  for(int lgi = 0; lgi < layers.gp.size; lgi++) {
    Layer_Group* lg = (Layer_Group*)layers.gp[lgi];
    if(lg->size <= 1) continue;

    // use first layer as initial data for layer group
    LeabraLayer* lay0 = (LeabraLayer*)lg->FastEl(0);
    LeabraLayerSpec* laysp0 = (LeabraLayerSpec*)lay0->spec.SPtr();
    if(!laysp0->lay_gp_inhib.on) continue;
    
    // lay0->laygp_data.netin.InitVals();
    // lay0->laygp_data.acts.InitVals();
    for(int li = 0; li < lg->size; li++) {
      LeabraLayer* lay = (LeabraLayer*)lg->FastEl(li);
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;

      // lay0->laygp_data.netin.UpdtFmAvgMax(lay->netin);
      // lay0->laygp_data.acts.UpdtFmAvgMax(lay->acts);
    }

    // lay0->laygp_data.netin.CalcAvg();
    // lay0->laygp_data.acts.CalcAvg();
    // laysp0->Compute_Inhib_impl(lay0, &lay0->laygp_data, this, laysp0->lay_gp_inhib);
    
    for(int li = 1; li < lg->size; li++) {
      LeabraLayer* lay = (LeabraLayer*)lg->FastEl(li);
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;
      
      // lay->laygp_data = lay0->laygp_data;
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation



///////////////////////////////////////////////////////////////////////
//      Cycle Stats


void LeabraNetwork::Compute_OutputName() {
  output_name = "";             // this will be updated by layer
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    // lay->Compute_OutputName(this);
  }
}


///////////////////////////////////////////////////////////////////////
//      DeepLeabra updates

void LeabraNetwork::ClearDeepActs() {
  NET_THREAD_CALL(LeabraNetwork::ClearDeepActs_Thr);
}

void LeabraNetwork::ClearMSNTrace() {
  NET_THREAD_CALL(LeabraNetwork::ClearMSNTrace_Thr);
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


// void LeabraNetwork::Compute_dWt_VecVars_Thr(int thr_no) {
//   float* avg_s = UnVecVar(thr_no, AVG_S);
//   float* avg_m = UnVecVar(thr_no, AVG_M);
//   float* avg_l = UnVecVar(thr_no, AVG_L);
//   float* avg_l_lrn = UnVecVar(thr_no, AVG_L_LRN);
//   // float* deep =  UnVecVar(thr_no, DEEP);
// // #ifdef CUDA_COMPILE
// //   float* act_q0 =  UnVecVar(thr_no, ACT_Q0);
// // #endif

//   // each thread copies all unit vars into their *own* thread-local mem in unit_vec_vars
//   for(int i=1; i<units_flat.size; i++) {
//     LeabraUnitState_cpp* u = (LeabraUnitState_cpp*)UnUnitState(i);
//     avg_s[i] = u->avg_s_eff;    // key!
//     avg_m[i] = u->avg_m;
//     avg_l[i] = u->avg_l;
//     avg_l_lrn[i] = u->avg_l_lrn;
//     // deep[i] = u->deep_lrn;
// // #ifdef CUDA_COMPILE
// //     act_q0[i] = u->act_q0;
// //     LeabraUnit* un = (LeabraUnit*)UnFmIdx(i);
// //     LeabraLayer* rlay = un->own_lay();
// // #endif
//   }
// }


void LeabraNetwork::Compute_dWt() {
  Compute_dWt_Layer_pre();

// #ifdef CUDA_COMPILE
//   Cuda_Compute_dWt();
//   return;
// #endif

  NET_THREAD_CALL(LeabraNetwork::Compute_dWt_Thr);
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
    LEABRA_UNGP_STATE* lgpd = (LEABRA_UNGP_STATE*)lay->GetLayUnGpState(net_state);
    report_table->AddBlankRow();
    ln->SetValAsString(lay->name, -1);
    lsn->SetValAsString(ls->name, -1);
    actm->SetValAsFloat(lgpd->acts_m_avg, -1);
    actp->SetValAsFloat(lgpd->acts_p_avg, -1);
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

void LeabraNetwork::Compute_NormErr() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NormErr_Thr);
  Compute_NormErr_Agg();
}

float LeabraNetwork::Compute_CosErr() {
  NET_THREAD_CALL(LeabraNetwork::Compute_CosErr_Thr);
  return Compute_CosErr_Agg();
}

float LeabraNetwork::Compute_CosDiff() {
  NET_THREAD_CALL(LeabraNetwork::Compute_CosDiff_Thr);
  return Compute_CosDiff_Agg();
}
    
float LeabraNetwork::Compute_AvgActDiff() {
  NET_THREAD_CALL(LeabraNetwork::Compute_AvgActDiff_Thr);
  return Compute_AvgActDiff_Agg();
}

float LeabraNetwork::Compute_TrialCosDiff() {
  NET_THREAD_CALL(LeabraNetwork::Compute_TrialCosDiff_Thr);
  return Compute_TrialCosDiff_Agg();
}
    
void LeabraNetwork::Compute_ActMargin() {
  NET_THREAD_CALL(LeabraNetwork::Compute_ActMargin_Thr);
  Compute_ActMargin_Agg();
}
    
float LeabraNetwork::Compute_NetSd() {
  NET_THREAD_CALL(LeabraNetwork::Compute_NetSd_Thr);
  return Compute_NetSd_Agg();
}
    
void LeabraNetwork::Compute_HogDeadPcts() {
  NET_THREAD_CALL(LeabraNetwork::Compute_HogDeadPcts_Thr);
  Compute_HogDeadPcts_Agg();
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

void LeabraNetwork::Compute_PlusStats() {
  NET_THREAD_CALL(LeabraNetwork::Compute_PlusStats_Thr); // do all threading at once
  Compute_PlusStats_Agg();
  Compute_ExtRew();
}

void LeabraNetwork::Compute_AbsRelNetin() {
  // always get layer-level netin max / avg values
  // decision of whether to run prjn-level is done by layers
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    laysp->Compute_AbsRelNetin(lay, this);
  }
}

void LeabraNetwork::Compute_AvgAbsRelNetin() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    laysp->Compute_AvgAbsRelNetin(lay, this);
  }
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
      LeabraConState_cpp* sc = (LeabraConState_cpp*)un->send[p];
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
          LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->send.FastEl(g);
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
        LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->send.FastEl(g);
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
          LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->send.FastEl(g);
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
        LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->send.FastEl(g);
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
        LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->send.FastEl(g);
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
          LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->send.FastEl(g);
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
        LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->send.FastEl(g);
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
