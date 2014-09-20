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

TA_BASEFUNS_CTORS_DEFN(LeabraPhases);
TA_BASEFUNS_CTORS_DEFN(LeabraNetStats);
TA_BASEFUNS_CTORS_DEFN(LeabraNetMisc);
TA_BASEFUNS_CTORS_DEFN(RelNetinSched);
TA_BASEFUNS_CTORS_DEFN(LeabraNetwork);


//////////////////////////
//      Network         //
//////////////////////////

void LeabraPhases::Initialize() {
  minus = 50;
  plus = 20;
  no_plus_test = false;
  time_inc = 0.001f;

  total_cycles = minus + plus;
}

void LeabraPhases::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
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

  unlearnable_trial = false;

  phase = MINUS_PHASE;
  phase_no = 0;
  phase_max = 2;

  ct_cycle = 0;
  tot_cycle = 0;

  minus_cycles = 0.0f;

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
  phases.UpdateAfterEdit_NoGui();

  if(TestWarning(phases.plus <= 0, "UAE",
                 "can't have phases.plus <= 0 -- must have at least some plus phase cycles -- see no_plus_test option for not running plus during testing -- setting plus to default 20 cycles")) {
    phases.plus = 20;
  }

  if(TestWarning(!lstats.off_errs && !lstats.on_errs, "UAE", "can't have both lstats.off_errs and lstats.on_errs be off (no err would be computed at all) -- turned both back on")) {
    lstats.on_errs = true;
    lstats.off_errs = true;
  }

  if(net_misc.ti) {
    if(TestWarning(phases.no_plus_test, "UAE", 
                   "can't have no_plus_test in TI mode -- turning it off")) {
      phases.no_plus_test = false;
    }
    if(TestWarning(phases.minus <= 0, "UAE", 
                   "must have minus phase cycles in TI mode -- setting minus = 50")) {
      phases.minus = 50;
    }
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
  phase_no = 0;
  tot_cycle = 0;
}

void LeabraNetwork::Init_Stats() {
  inherited::Init_Stats();
  trg_max_act = 0.0f;

  minus_cycles = 0.0f;
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

void LeabraNetwork::Init_Weights() {
  inherited::Init_Weights();
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

void LeabraNetwork::CheckInhibCons() {
  net_misc.inhib_cons = false;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->CheckInhibCons(this);
  }
}

void LeabraNetwork::BuildUnits_Threads() {
  CheckInhibCons();
  if(net_misc.inhib_cons) {
    SetNetFlag(NETIN_PER_PRJN);	// inhib cons use per-prjn inhibition
  }
  inherited::BuildUnits_Threads();

  active_layer_idx.Reset();
  for(int i=0;i<layers.leaves; i++) {
    LeabraLayer* l = (LeabraLayer*)layers.Leaf(i);
    if(l->lesioned()) continue;
    LeabraLayerSpec* ls = (LeabraLayerSpec*)l->GetLayerSpec();
    if(l->layer_type == Layer::INPUT && ls->clamp.hard)
      continue;                 // not active for our purposes..
    active_layer_idx.Add(i);
  }

  lthreads.InitAll();
}

void LeabraNetwork::BuildUnits_Threads_send_netin_tmp() {
  // temporary storage for sender-based netinput computation
  if(units_flat.size > 0 && lthreads.n_threads > 0) {
    // note: n_threads should always be > 0, so in general we have this buffer around
    // in all cases
    if(NetinPerPrjn()) {
      send_netin_tmp.SetGeom(3, units_flat.size, max_prjns, lthreads.n_threads);
    }
    else {
      send_netin_tmp.SetGeom(2, units_flat.size, lthreads.n_threads);
    }
    send_netin_tmp.InitVals(0.0f);

    unit_vec_vars.SetGeom(2, units_flat.size, N_VEC_VARS);
  }
}

///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LeabraNetwork::Trial_Init() {
  unlearnable_trial = false;
  cycle = -1;
  Trial_Init_Phases();
  Trial_Init_Specs();

  Trial_Init_Unit(); // performs following at unit-level
//   Trial_DecayState();
  Trial_NoiseInit();            // run for kpos case
  //  Trial_Init_SRAvg();
  Trial_Init_Layer();
}

void LeabraNetwork::Trial_Init_Phases() {
  phase = MINUS_PHASE;
  phase_no = 0;

  bool no_plus_testing = IsNoPlusTesting();

  if(phases.minus > 0) {
    if(no_plus_testing)
      phase_max=1;
    else
      phase_max=2;
  }
  else {
    phase_max=1;
    phase = PLUS_PHASE;
  }
}

void LeabraNetwork::Trial_Init_Specs() {
  net_misc.ti = false;
  net_misc.dwt_norm = false;
  net_misc.lay_gp_inhib = false;
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Trial_Init_Specs(this);
  }
}

void LeabraNetwork::Trial_Init_Unit() {
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Trial_Init_Unit(this, -1);
  }
  // ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Trial_Init_Unit);
  // threads.Run(&un_call, -1.0f); // -1 = always run localized
}

void LeabraNetwork::Trial_NoiseInit() {
  // FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
  //   if(lay->lesioned()) continue;
  //   lay->Trial_NoiseInit(this);
  // }
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
  if(phase == MINUS_PHASE)
    minus_cycles = -1;          // signal that nothing has been recorded

  Settle_Init_Unit();           // do chunk of following unit-level functions:

//   Settle_Init_TargFlags();
//   Settle_DecayState();
//   Compute_NetinScale();              // compute net scaling

  // these could have accumulated netin deltas for clamped layers..
  send_netin_tmp.InitVals(0.0f); // reset for next time around

  Compute_HardClamp();          // clamp all hard-clamped input acts: not easily threadable
  cycle = tmp_cycle;
}

void LeabraNetwork::Settle_Init_Unit() {
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Settle_Init_Unit(this, -1);
  }
  // ThreadUnitCall un_call((ThreadUnitMethod)(LeabraUnitMethod)&LeabraUnit::Settle_Init_Unit);
  // threads.Run(&un_call, -1.0f); // -1 = always run localized

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
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->Settle_Init_TargFlags(this);
  }
}

void LeabraNetwork::Settle_DecayState() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Settle_DecayState(this, -1);
  }
}

void LeabraNetwork::Compute_NetinScale() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Compute_NetinScale(this, -1);
  }
}

void LeabraNetwork::Compute_NetinScale_Senders() {
  // NOTE: this IS called by default -- second phase of Settle_Init_Unit
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Compute_NetinScale_Senders(this, -1);
  }
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

  if(lthreads.CanRun()) {
    lthreads.Run(LeabraThreadMgr::RUN_CYCLE);
  }
  else {
    Send_Netin();
    Compute_NetinInteg();

    Compute_Inhib();

    Compute_Act();

    Compute_CycleStats_Pre();
    Compute_CycleStats_Layer();
    Compute_CycleStats_Post();

    Compute_MidMinus();           // check for mid-minus and run if so (PBWM)

    Cycle_IncrCounters();
  }
}

void LeabraNetwork::Cycle_IncrCounters() {
  cycle++;
  ct_cycle++;
  tot_cycle++;
  time += phases.time_inc;
}

///////////////////////////////////////////////////////
//      Cycle Stage 1: netinput


void LeabraNetwork::Send_Netin() {
  // always use delta mode!
  // no threads -- only threaded version supported is lthreads, due to need to
  // use correct thread roll-up numbers, etc
  send_pct_n = send_pct_tot = 0;
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Send_NetinDelta(this, -1);
  }
  if(send_pct_tot > 0) {        // only avail for non-threaded calls
    send_pct = (float)send_pct_n / (float)send_pct_tot;
    avg_send_pct.Increment(send_pct);
  }
}

void LeabraNetwork::Compute_NetinInteg() {
  // non-threaded
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Compute_NetinInteg(this, -1);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition

void LeabraNetwork::Compute_Inhib() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_Inhib(this);
  }
  if(net_misc.lay_gp_inhib)
    Compute_Inhib_LayGp();
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

void LeabraNetwork::Compute_Act() {
  // non-threaded
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Compute_Act(this, -1);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraNetwork::Compute_CycleStats_Pre() {
  // stats are never threadable due to updating at higher levels
  trg_max_act = 0.0f;
  init_netins_cycle_stat = false;
}

void LeabraNetwork::Compute_CycleStats_Layer() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_CycleStats(this);
  }
}

void LeabraNetwork::Compute_CycleStats_Post() {
  Compute_OutputName();
  Compute_MinusCycles();
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

void LeabraNetwork::Compute_MinusCycles() {
  if(phase != MINUS_PHASE) return;
  if(minus_cycles > 0) return;  // already set
  if(trg_max_act > lstats.trg_max_act_crit)
    minus_cycles = cycle;
}


///////////////////////////////////////////////////////////////////////
//      Cycle Optional Misc

void LeabraNetwork::Compute_MidMinus() {
  // if(mid_minus_cycle <= 0) return;
  // if(ct_cycle == mid_minus_cycle) {
  //   FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
  //     if(lay->lesioned())       continue;
  //     lay->Compute_MidMinus(this);
  //   }
  // }
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
  if(net_misc.ti) {
    TI_CtxtUpdate();
  }
  if(phase == LeabraNetwork::PLUS_PHASE) {
    Compute_CosDiff();
    Compute_AvgActDiff();
    Compute_TrialCosDiff();
  }
}

void LeabraNetwork::Settle_Compute_dWt() {
  if(train_mode == TEST)
    return;

  if(phases.minus > 0) {
    if(phase_no == 1) {
      Compute_dWt();
    }
  }
  else {                        // plus only
    Compute_dWt();
  }
}


///////////////////////////////////////////////////////////////////////
//      LeabraTI final plus phase context updating

void LeabraNetwork::TI_CtxtUpdate() {
  bool do_updt = (phase_no == 1); // only do in plus phase

  // todo: only doing deep act at end of every phase -- if need to update TI context
  // then need to delegate entirely to unit level..
  TI_Compute_Deep5bAct();

  if(do_updt) {
    TI_Send_Netins();
    TI_Compute_CtxtAct();
  }
}

void LeabraNetwork::TI_Compute_Deep5bAct() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(!lay->lesioned())
      lay->TI_Compute_Deep5bAct(this);
  }
}

void LeabraNetwork::TI_Send_Netins() {
  if(lthreads.CanRun()) {
    lthreads.Run(LeabraThreadMgr::RUN_TI_NETS);
  }
  else {
    TI_Send_Deep5bNetin();
    TI_Send_CtxtNetin();
  }
}

void LeabraNetwork::TI_Send_Deep5bNetin() {
  send_pct_n = send_pct_tot = 0;

  // non-threaded
  const int nu = units_flat.size;
  for(int i=1; i<nu; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->TI_Send_Deep5bNetin(this, -1);
  }

  // always need to roll up the netinput into unit vals -- thread or not
  for(int i=1;i<nu;i++) {   // 0 = dummy idx
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    u->TI_Send_Deep5bNetin_Post(this);
  }
}

void LeabraNetwork::TI_Send_CtxtNetin() {
  send_pct_n = send_pct_tot = 0;

  // non-threaded
  const int nu = units_flat.size;
  for(int i=1; i<nu; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->TI_Send_CtxtNetin(this, -1);
  }
  // always need to roll up the netinput into unit vals
  for(int i=1;i<nu;i++) {   // 0 = dummy idx
    LeabraUnit* u = (LeabraUnit*)units_flat[i];
    u->TI_Send_CtxtNetin_Post(this);
  }
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
  bool no_plus_testing = IsNoPlusTesting();

  if(phases.minus > 0) {
    phase = PLUS_PHASE;
  }
  else { // plus only
    // nop -- should be done!
  }
}

void LeabraNetwork::Trial_Final() {
  EncodeState();
  Compute_SelfReg_Trial();
  Compute_AbsRelNetin();
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

void LeabraNetwork::Compute_dWt_Layer_pre() {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_dWt_Layer_pre(this);
  }
}

void LeabraNetwork::Compute_dWt_vecvars() {
  float* avg_s = UnVecVar(AVG_S);
  float* avg_m = UnVecVar(AVG_M);
  float* avg_l = UnVecVar(AVG_L);
  float* thal =  UnVecVar(THAL);

  // non-threaded for now..
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    avg_s[i] = un->avg_s;
    avg_m[i] = un->avg_m;
    avg_l[i] = un->avg_l;
    thal[i] = un->thal;
  }
}


void LeabraNetwork::Compute_dWt() {
  if(lstats.cos_err_lrn_thr > -1.0f) {		  // note: requires computing err before calling this!
    if(cos_err < lstats.cos_err_lrn_thr) return; // didn't make threshold
  }

  Compute_dWt_Layer_pre();
  Compute_dWt_vecvars();

  if(lthreads.CanRun()) {
    lthreads.Run(LeabraThreadMgr::RUN_DWT); // does both dwt and dwt_norm
  }
  else {
    // non-threaded
    for(int i=1; i<units_flat.size; i++) {
      LeabraUnit* un = (LeabraUnit*)units_flat[i];
      un->Compute_dWt(this, -1);
    }
    Compute_dWt_Norm();
  }
}

void LeabraNetwork::Compute_dWt_Norm() {
  if(!net_misc.dwt_norm) return;
  // non-threaded
  for(int i=1; i<units_flat.size; i++) {
    LeabraUnit* un = (LeabraUnit*)units_flat[i];
    un->Compute_dWt_Norm(this, -1);
  }
}

void LeabraNetwork::Compute_Weights_impl() {
  if(lthreads.CanRun()) {
    lthreads.Run(LeabraThreadMgr::RUN_WT);
  }
  else {
    // non-threaded
    for(int i=1; i<units_flat.size; i++) {
      LeabraUnit* un = (LeabraUnit*)units_flat[i];
      un->Compute_Weights(this, -1);
    }
  }
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
    if(!l->HasExtFlag(Unit::TARG | Unit::COMP)) {
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
    if(!l->HasExtFlag(Unit::TARG | Unit::COMP)) {
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
    if(!l->HasExtFlag(Unit::TARG | Unit::COMP)) {
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
  bool no_plus_testing = IsNoPlusTesting();

  if(phases.minus > 0) {
    if(no_plus_testing) {
      Compute_MinusStats();
      Compute_PlusStats();
    }
    else {
      if(phase_no == 0)
        Compute_MinusStats();
      else
        Compute_PlusStats();
    }
  }
  else {                        // plus only
    Compute_MinusStats();
    Compute_PlusStats();
  }
}

void LeabraNetwork::Compute_MinusStats() {
  minus_output_name = output_name; // grab and hold..
  if(minus_cycles < 0) // never reached target
    minus_cycles = cycle;       // set to current cyc -- better for integrating
  avg_cycles.Increment(minus_cycles);
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
