// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in LeabraNetworkState.cpp, _cpp.cpp, _cuda.cpp

#define LEABRA_AVG_MAX STATE_CLASS(LeabraAvgMax)

////////////////////////////////////////////////////////////////////////
//              Build

void LEABRA_NETWORK_STATE::AllocLeabraStatsMem() {
  NetStateMalloc((void**)&thrs_ungp_avg_max_vals, n_thrs_built * sizeof(char*));

  for(int i=0; i<n_thrs_built; i++) {
    NetStateMalloc((void**)&thrs_ungp_avg_max_vals[i], n_ungps_built
                       * N_AM_VARS * sizeof(LEABRA_AVG_MAX));
  }
}

void LEABRA_NETWORK_STATE::InitLeabraThreadMem_Thr(int thr_no) {
  memset(thrs_ungp_avg_max_vals[thr_no], 0, n_ungps_built * N_AM_VARS *
         sizeof(LEABRA_AVG_MAX));
}

void LEABRA_NETWORK_STATE::FreeStateMem() {
  inherited::FreeStateMem();

  if(!thrs_ungp_avg_max_vals) return;
  
  for(int i=0; i<n_thrs_built; i++) {
    NetStateFree((void**)&thrs_send_deeprawnet_tmp[i]);
    NetStateFree((void**)&thrs_send_deepmodnet_tmp[i]);
    NetStateFree((void**)&thrs_ungp_avg_max_vals[i]);
  }
  NetStateFree((void**)&thrs_send_deeprawnet_tmp);
  NetStateFree((void**)&thrs_send_deepmodnet_tmp);
  NetStateFree((void**)&thrs_ungp_avg_max_vals);
}


void LEABRA_NETWORK_STATE::AllocSendNetinTmpState() {
  // note: not calling Network: version -- need to update based on that!
  // temporary storage for sender-based netinput computation
  if(n_units_built == 0 || n_thrs_built == 0) return;

  NetStateMalloc((void**)&thrs_send_netin_tmp, n_thrs_built * sizeof(float*));
  NetStateMalloc((void**)&thrs_send_deeprawnet_tmp, n_thrs_built * sizeof(float*));
  NetStateMalloc((void**)&thrs_send_deepmodnet_tmp, n_thrs_built * sizeof(float*));

  for(int i=0; i<n_thrs_built; i++) {
    if(NetinPerPrjn()) {
      NetStateMalloc((void**)&thrs_send_netin_tmp[i],
                         n_units_built * max_prjns * sizeof(float));
    }
    else {
      NetStateMalloc((void**)&thrs_send_netin_tmp[i],
                         n_units_built * sizeof(float));
    }
    NetStateMalloc((void**)&thrs_send_deeprawnet_tmp[i],
                       n_units_built * sizeof(float));
    NetStateMalloc((void**)&thrs_send_deepmodnet_tmp[i],
                       n_units_built * sizeof(float));
  }
}

void LEABRA_NETWORK_STATE::InitSendNetinTmp_Thr(int thr_no) {
  InitCycleNetinTmp_Thr(thr_no);
  InitDeepRawNetinTmp_Thr(thr_no);
  // just in case deep flags are off - cycle uses those..
  memset(thrs_send_deepmodnet_tmp[thr_no], 0, n_units_built * sizeof(float));
}


LAYER_SPEC_CPP* LEABRA_NETWORK_STATE::NewLayerSpec(int spec_type) const {
  switch(spec_type) {
  case T_LeabraLayerSpec:
    return new STATE_CLASS_CPP(LeabraLayerSpec)();
  case T_ScalarValLayerSpec:
    return new STATE_CLASS_CPP(ScalarValLayerSpec)();
  case T_ExtRewLayerSpec:
    return new STATE_CLASS_CPP(ExtRewLayerSpec)();
  case T_TwoDValLayerSpec:
    return new STATE_CLASS_CPP(TwoDValLayerSpec)();
  case T_DecodeTwoDValLayerSpec:
    return new STATE_CLASS_CPP(DecodeTwoDValLayerSpec)();
  // case T_MotorForceLayerSpec:
  //   return new STATE_CLASS_CPP(MotorForceLayerSpec)();
  // case T_LeabraV1LayerSpec:
  //   return new STATE_CLASS_CPP(LeabraV1LayerSpec)();
  // case T_GPiSoftMaxLayerSpec:
  //   return new STATE_CLASS_CPP(GPiSoftMaxLayerSpec)();
  // case T_SubiculumLayerSpec:
  //   return new STATE_CLASS_CPP(SubiculumLayerSpec)();
  }
  StateError("NewLayerSpec, type not recognized");
  return new STATE_CLASS_CPP(LeabraLayerSpec)();
}

UNIT_SPEC_CPP* LEABRA_NETWORK_STATE::NewUnitSpec(int spec_type) const {
  switch(spec_type) {
  case T_LeabraUnitSpec:
    return new STATE_CLASS_CPP(LeabraUnitSpec)();
  case T_LayerActUnitSpec:
    return new STATE_CLASS_CPP(LayerActUnitSpec)();
  case T_DeepCopyUnitSpec:
    return new STATE_CLASS_CPP(DeepCopyUnitSpec)();
  case T_ThalSendUnitSpec:
    return new STATE_CLASS_CPP(ThalSendUnitSpec)();
  case T_PoolInputsUnitSpec:
    return new STATE_CLASS_CPP(PoolInputsUnitSpec)();

  case T_TDRewPredUnitSpec:
    return new STATE_CLASS_CPP(TDRewPredUnitSpec)();
  case T_TDRewIntegUnitSpec:
    return new STATE_CLASS_CPP(TDRewIntegUnitSpec)();
  case T_TDDeltaUnitSpec:
    return new STATE_CLASS_CPP(TDDeltaUnitSpec)();

  case T_BasAmygUnitSpec:
    return new STATE_CLASS_CPP(BasAmygUnitSpec)();
  case T_BLAmygUnitSpec:
    return new STATE_CLASS_CPP(BLAmygUnitSpec)();
  case T_CElAmygUnitSpec:
    return new STATE_CLASS_CPP(CElAmygUnitSpec)();
  case T_PPTgUnitSpec:
    return new STATE_CLASS_CPP(PPTgUnitSpec)();
  case T_LHbRMTgUnitSpec:
    return new STATE_CLASS_CPP(LHbRMTgUnitSpec)();
  case T_VTAUnitSpec:
    return new STATE_CLASS_CPP(VTAUnitSpec)();
  case T_DRNUnitSpec:
    return new STATE_CLASS_CPP(DRNUnitSpec)();
  case T_ClampDaUnitSpec:
    return new STATE_CLASS_CPP(ClampDaUnitSpec)();
  case T_BFCSUnitSpec:
    return new STATE_CLASS_CPP(BFCSUnitSpec)();

  case T_D1D2UnitSpec:
    return new STATE_CLASS_CPP(D1D2UnitSpec)();
  case T_MSNUnitSpec:
    return new STATE_CLASS_CPP(MSNUnitSpec)();
  case T_TANUnitSpec:
    return new STATE_CLASS_CPP(TANUnitSpec)();
  case T_PatchUnitSpec:
    return new STATE_CLASS_CPP(PatchUnitSpec)();
  case T_GPiInvUnitSpec:
    return new STATE_CLASS_CPP(GPiInvUnitSpec)();
  case T_InvertUnitSpec:
    return new STATE_CLASS_CPP(InvertUnitSpec)();
  case T_PFCUnitSpec:
    return new STATE_CLASS_CPP(PFCUnitSpec)();

  case T_ECoutUnitSpec:
    return new STATE_CLASS_CPP(ECoutUnitSpec)();
  case T_CA1UnitSpec:
    return new STATE_CLASS_CPP(CA1UnitSpec)();
  case T_CerebGranuleUnitSpec:
    return new STATE_CLASS_CPP(CerebGranuleUnitSpec)();
  case T_LeabraMultCopyUnitSpec:
    return new STATE_CLASS_CPP(LeabraMultCopyUnitSpec)();
  case T_LeabraContextUnitSpec:
    return new STATE_CLASS_CPP(LeabraContextUnitSpec)();
  }
  StateError("NewUnitSpec, type not recognized");
  return new STATE_CLASS_CPP(LeabraUnitSpec)();
}

CON_SPEC_CPP* LEABRA_NETWORK_STATE::NewConSpec(int spec_type) const {
  switch(spec_type) {
  case T_LeabraConSpec:
    return new STATE_CLASS_CPP(LeabraConSpec)();
  case T_LeabraBiasSpec:
    return new STATE_CLASS_CPP(LeabraBiasSpec)();

  case T_DeepCtxtConSpec:
    return new STATE_CLASS_CPP(DeepCtxtConSpec)();
  case T_SendDeepRawConSpec:
    return new STATE_CLASS_CPP(SendDeepRawConSpec)();
  case T_SendDeepModConSpec:
    return new STATE_CLASS_CPP(SendDeepModConSpec)();

  case T_MarkerConSpec:
    return new STATE_CLASS_CPP(MarkerConSpec)();
  case T_LeabraLimPrecConSpec:
    return new STATE_CLASS_CPP(LeabraLimPrecConSpec)();
    
  case T_LeabraDeltaConSpec:
    return new STATE_CLASS_CPP(LeabraDeltaConSpec)();
  case T_DaDeltaConSpec:
    return new STATE_CLASS_CPP(DaDeltaConSpec)();
  case T_DaHebbConSpec:
    return new STATE_CLASS_CPP(DaHebbConSpec)();
  case T_CHLConSpec:
    return new STATE_CLASS_CPP(CHLConSpec)();

  case T_TDRewPredConSpec:
    return new STATE_CLASS_CPP(TDRewPredConSpec)();
  case T_LatAmygConSpec:
    return new STATE_CLASS_CPP(LatAmygConSpec)();
  case T_BasAmygConSpec:
    return new STATE_CLASS_CPP(BasAmygConSpec)();
  case T_BLAmygConSpec:
    return new STATE_CLASS_CPP(BLAmygConSpec)();
  case T_CElAmygConSpec:
    return new STATE_CLASS_CPP(CElAmygConSpec)();
  case T_MSNConSpec:
    return new STATE_CLASS_CPP(MSNConSpec)();

  case T_HippoEncoderConSpec:
    return new STATE_CLASS_CPP(HippoEncoderConSpec)();
  case T_CerebPfPcConSpec:
    return new STATE_CLASS_CPP(CerebPfPcConSpec)();
  }
  StateError("NewConSpec, type not recognized");
  return new STATE_CLASS_CPP(LeabraConSpec)();
}



///////////////////////////////////////////////////////////////////////
//      General Init functions


void LEABRA_NETWORK_STATE::Init_Counters_State() {
  inherited::Init_Counters_State();
  quarter = 0;
  phase = MINUS_PHASE;
  tot_cycle = 0;
}

void LEABRA_NETWORK_STATE::Init_Weights_Layers() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Init_Weights_Layer(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Init_Stats() {
  inherited::Init_Stats();
  trg_max_act = 0.0f;

  rt_cycles = 0.0f;
  avg_cycles.ResetAvg();

  // minus_output_name = "";

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

void LEABRA_NETWORK_STATE::Init_Stats_Layers() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Init_Stats(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Init_AdaptInhib() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Init_AdaptInhib(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Init_Acts_Layers() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Init_Acts_Layer(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Init_Netins_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Init_Netins(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::DecayState_Thr(int thr_no) {
  float decay = tmp_arg1;
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->DecayState(uv, this, thr_no, decay);
  }
}

void LEABRA_NETWORK_STATE::ResetSynTR_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->ResetSynTR(uv, this, thr_no);
  }
}

///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LEABRA_NETWORK_STATE::Trial_Init_Counters() {
  times.thal_gate_cycle = -2;
  cycle = 0;
  quarter = 0;
  phase = MINUS_PHASE;
  rt_cycles = -1;          // signal that nothing has been recorded
  total_trials++;          // this is when we increment it!
}

void LEABRA_NETWORK_STATE::Trial_Init_Specs() {
  net_misc.spike = false;
  net_misc.bias_learn = false;
  net_misc.trial_decay = false;
  net_misc.diff_scale_p = false;
  net_misc.diff_scale_q1 = false;
  net_misc.wt_bal = false;
  net_misc.lay_gp_inhib = false;

  deep.on = false;
  deep.ctxt = false;
  deep.raw_net = false;
  deep.mod_net = false;
  deep.raw_qtr = STATE_CLASS_CPP(LeabraNetDeep)::QNULL;

  for(int i=0; i < n_layer_specs_built; i++) {
    LEABRA_LAYER_SPEC_CPP* ls = (LEABRA_LAYER_SPEC_CPP*)GetLayerSpec(i);
    ls->Trial_Init_Specs(this);
  }
  for(int i=0; i < n_unit_specs_built; i++) {
    LEABRA_UNIT_SPEC_CPP* ls = (LEABRA_UNIT_SPEC_CPP*)GetUnitSpec(i);
    ls->Trial_Init_Specs(this);
  }
  for(int i=0; i < n_con_specs_built; i++) {
    LEABRA_CON_SPEC_CPP* ls = (LEABRA_CON_SPEC_CPP*)GetConSpec(i);
    ls->Trial_Init_Specs(this);
  }
}


void LEABRA_NETWORK_STATE::Trial_Init_Unit_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Trial_Init_Unit(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Trial_Init_Layers() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Trial_Init_Layer(lay, this);
  }
}


///////////////////////////////////////////////////////////////////////
//      QuarterInit -- at start of settling


void LEABRA_NETWORK_STATE::Quarter_Init_Counters() {
  // set the phase according to the counter 0,1,2 = MINUS, 3 = PLUS
  if(quarter <= 2)
    phase = MINUS_PHASE;
  else
    phase = PLUS_PHASE;
}

void LEABRA_NETWORK_STATE::Quarter_Init_Layers() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Quarter_Init_Layer(lay, this);
    ls->Compute_HardClamp_Layer(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Compute_HardClamp_Layers() {
  // not called by default!
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_HardClamp_Layer(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Quarter_Init_Unit_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Quarter_Init_Unit(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Quarter_Init_TargFlags_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Quarter_Init_TargFlags(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Quarter_Init_TargFlags_Layers() {
  // not called by default!
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Quarter_Init_TargFlags_Layer(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Compute_NetinScale() {
  const int nl = n_layers_built;
  for(int li=0; li < nl; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;

    LEABRA_UNIT_SPEC_CPP* us = lay->GetUnitSpec(this);
    us->Compute_NetinScale(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Quarter_Init_Deep_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);

  if(deep.ctxt) {
    for(int i=0; i<nu; i++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->Quarter_Init_Deep(uv, this, thr_no);
    }
    ThreadSyncSpin(thr_no, 0);
    
    for(int i=0; i<nu; i++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->Send_DeepCtxtNetin(uv, this, thr_no);
    }
    ThreadSyncSpin(thr_no, 1);

    for(int i=0; i<nu; i++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->Compute_DeepCtxt(uv, this, thr_no);
    }
    ThreadSyncSpin(thr_no, 2);
    InitDeepRawNetinTmp_Thr(thr_no);

    Compute_DeepCtxtStats_Thr(thr_no);
    ThreadSyncSpin(thr_no, 0);
    if(thr_no == 0) {
      Compute_DeepCtxtStats_Post();
    }
    ThreadSyncSpin(thr_no, 1);
  }

  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_DeepStateUpdt(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::InitDeepRawNetinTmp_Thr(int thr_no) {
  // todo: does memset work in cuda?
  memset(thrs_send_deeprawnet_tmp[thr_no], 0, n_units_built * sizeof(float));
}

void LEABRA_NETWORK_STATE::Compute_HardClamp_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_HardClamp(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Compute_DeepCtxtStats_Thr(int thr_no) {
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    if(gpd->lesioned(this))
      continue;
    LEABRA_AVG_MAX* am_ctxt = ThrUnGpAvgMax(thr_no, li, AM_DEEP_RAW);
    am_ctxt->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = uv->flat_idx;
      am_ctxt->UpdtVals(uv->deep_ctxt, flat_idx); 
    }
  }
}

void LEABRA_NETWORK_STATE::Compute_DeepCtxtStats_Post() {
  // integrate all the data from thread-specific guys
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    if(gpd->lesioned(this))
      continue;
    LEABRA_AVG_MAX& am_deep_ctxt = gpd->am_deep_ctxt;
    am_deep_ctxt.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      LEABRA_AVG_MAX* am_ctxt = ThrUnGpAvgMax(i, li, AM_DEEP_RAW);
      am_deep_ctxt.UpdtFmAvgMax(*am_ctxt);
    }
    am_deep_ctxt.CalcAvg();
  }
}

void LEABRA_NETWORK_STATE::Quarter_Init_Layer_Post() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Quarter_Init_Layer_Post(lay, this);
  }
}

void LEABRA_NETWORK_STATE::ExtToComp_Layers() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->ExtToComp(lay, this);
  }
}

void LEABRA_NETWORK_STATE::ExtToComp_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->ExtToComp(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::TargExtToComp_Layers() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->TargExtToComp(lay, this);
  }
}

void LEABRA_NETWORK_STATE::TargExtToComp_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->TargExtToComp(uv, this, thr_no);
  }
}


////////////////////////////////////////////////////////////////
//      Cycle_Run


void LEABRA_NETWORK_STATE::Cycle_Run_Thr(int thr_no) {
  int tot_cyc = 1;
  if(times.cycle_qtr)
    tot_cyc = times.quarter;
  for(int cyc = 0; cyc < tot_cyc; cyc++) {
    Send_Netin_Thr(thr_no);
    ThreadSyncSpin(thr_no, 0);

    Compute_NetinInteg_Thr(thr_no);
    ThreadSyncSpin(thr_no, 1);

    StartTimer(NT_NETIN_STATS, thr_no);

    Compute_NetinStats_Thr(thr_no);
    if(deep.mod_net) {
      Compute_DeepModStats_Thr(thr_no);
    }
    ThreadSyncSpin(thr_no, 2);
    if(thr_no == 0) {
      Compute_NetinStats_Post();
      if(deep.mod_net) {
        Compute_DeepModStats_Post();
      }
    }
    ThreadSyncSpin(thr_no, 0);

    InitCycleNetinTmp_Thr(thr_no);

    EndTimer(NT_NETIN_STATS, thr_no);

    if(thr_no == 0) {
      Compute_Inhib();
    }
    ThreadSyncSpin(thr_no, 1);

    Compute_Act_Thr(thr_no);
    ThreadSyncSpin(thr_no, 2);

    if(thr_no == 0) {
      Compute_CycleStats_Pre(); // prior to act post!
    }
    ThreadSyncSpin(thr_no, 0);

    Compute_Act_Post_Thr(thr_no);
    ThreadSyncSpin(thr_no, 1);

    StartTimer(NT_CYCLE_STATS, thr_no);

    Compute_CycleStats_Thr(thr_no);
    ThreadSyncSpin(thr_no, 2);
    
    if(thr_no == 0) {
      Compute_CycleStats_Post();
    }
    ThreadSyncSpin(thr_no, 0);

    if(deep.on && deep.Quarter_DeepRawNow(quarter)) {
      int qtrcyc = cycle % times.quarter;
      if(qtrcyc % times.deep_cyc == 0) {
        Compute_DeepRaw_Thr(thr_no);
      }
    }
    ThreadSyncSpin(thr_no, 1);

    EndTimer(NT_CYCLE_STATS, thr_no);

    if(thr_no == 0) {
      Cycle_IncrCounters();
    }
  }
}

void LEABRA_NETWORK_STATE::Cycle_IncrCounters() {
  cycle++;
  tot_cycle++;
  time = (float)((int)round(time / times.time_inc) + 1) * times.time_inc;
  // crazy hoops to make sure that time is in precise increments of time_inc -- otherwise
  // can get significant drift..
}



///////////////////////////////////////////////////////
//      Cycle Stage 1: netinput

void LEABRA_NETWORK_STATE::Send_Netin_Thr(int thr_no) {
  StartTimer(NT_NETIN, thr_no);

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Send_NetinDelta(uv, this, thr_no);
  }
  if(send_pct_tot > 0) {        // only avail for one-threaded calls
    send_pct = (float)send_pct_n / (float)send_pct_tot;
    avg_send_pct.Increment(send_pct);
  }

  EndTimer(NT_NETIN, thr_no);
}

void LEABRA_NETWORK_STATE::Compute_NetinInteg_Thr(int thr_no) {
  StartTimer(NT_NETIN_INTEG, thr_no);

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_NetinInteg(uv, this, thr_no);
  }

  EndTimer(NT_NETIN_INTEG, thr_no);
}

void LEABRA_NETWORK_STATE::Compute_NetinStats_Thr(int thr_no) {
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    LEABRA_LAYER_STATE* lay = gpd->GetLayerState(this);
    if(lay->lesioned() || lay->hard_clamped)
      continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);
    float net_thr = ls->inhib_misc.net_thr;
    if(ls->inhib_misc.thr_rel) {
      net_thr *= lgpd->netin.max; // from previous cycle
    }
    LEABRA_AVG_MAX* am_net = ThrUnGpAvgMax(thr_no, li, AM_NET);
    am_net->InitVals();
    LEABRA_AVG_MAX* am_net_raw = ThrUnGpAvgMax(thr_no, li, AM_NET_RAW);
    am_net_raw->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned() || uv->net < net_thr) continue;
      const int flat_idx = uv->flat_idx;
      am_net->UpdtVals(uv->net, flat_idx); 
      am_net_raw->UpdtVals(uv->net_raw, flat_idx); 
    }
  }
}

void LEABRA_NETWORK_STATE::Compute_NetinStats_Post() {
  // integrate all the data from thread-specific guys
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    LEABRA_LAYER_STATE* lay = gpd->GetLayerState(this);
    if(lay->lesioned() || lay->hard_clamped)
      continue;
    LEABRA_AVG_MAX& netin = gpd->netin;
    netin.InitVals();
    LEABRA_AVG_MAX& netin_raw = gpd->netin_raw;
    netin_raw.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      LEABRA_AVG_MAX* am_net = ThrUnGpAvgMax(i, li, AM_NET);
      netin.UpdtFmAvgMax(*am_net);
      LEABRA_AVG_MAX* am_net_raw = ThrUnGpAvgMax(i, li, AM_NET_RAW);
      netin_raw.UpdtFmAvgMax(*am_net_raw);
    }
    netin.CalcAvg();
    if(phase == MINUS_PHASE) {
      gpd->netin_m = netin;
    }
    netin_raw.CalcAvg();
  }
}

void LEABRA_NETWORK_STATE::Compute_DeepModStats_Thr(int thr_no) {
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    if(gpd->lesioned(this))
      continue;
    LEABRA_AVG_MAX* am_net = ThrUnGpAvgMax(thr_no, li, AM_DEEP_NET);
    am_net->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = uv->flat_idx;
      am_net->UpdtVals(uv->deep_mod_net, flat_idx); 
    }
  }
}

void LEABRA_NETWORK_STATE::Compute_DeepModStats_Post() {
  // integrate all the data from thread-specific guys

  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    if(gpd->lesioned(this))
      continue;
    LEABRA_AVG_MAX& am_deep_net = gpd->am_deep_mod_net;
    am_deep_net.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      LEABRA_AVG_MAX* am_net = ThrUnGpAvgMax(i, li, AM_DEEP_NET);
      am_deep_net.UpdtFmAvgMax(*am_net);
    }
    am_deep_net.CalcAvg();
  }
}

void LEABRA_NETWORK_STATE::InitCycleNetinTmp_Thr(int thr_no) {
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

void LEABRA_NETWORK_STATE::Compute_Inhib() {
  StartTimer(NT_INHIB, 0);

  // note: only running on thr_no == 0 right now -- may be best overall to avoid
  // messy cache stuff, to just keep it on 0
  if(net_misc.lay_gp_inhib){
    Compute_Inhib_LayGp();
  }
  
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_Inhib(lay, this);
  }
  
  EndTimer(NT_INHIB, 0);
}

void LEABRA_NETWORK_STATE::Compute_Inhib_LayGp() {
  if(!net_misc.lay_gp_inhib) return;
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay0 = GetLayerState(li);
    if(lay0->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* laysp0 = lay0->GetLayerSpec(this);
    if(!laysp0->lay_gp_inhib.on) continue;
    if(lay0->laygp_lay0_idx != lay0->layer_idx || lay0->laygp_n <= 1)
      continue; // no actual groups..

    const int mx = lay0->laygp_n;
    const int l0 = lay0->laygp_lay0_idx;
    
    lay0->laygp_netin.InitVals();
    lay0->laygp_acts_eq.InitVals();
    for(int gi = 0; gi < mx; gi++) {
      LEABRA_LAYER_STATE* lay = GetLayerState(l0 + gi);
      LEABRA_LAYER_SPEC_CPP* laysp = lay->GetLayerSpec(this);
      if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;
      LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);

      lay0->laygp_netin.UpdtFmAvgMax(lgpd->netin);
      lay0->laygp_acts_eq.UpdtFmAvgMax(lgpd->acts);
    }

    lay0->laygp_netin.CalcAvg();
    lay0->laygp_acts_eq.CalcAvg();
    laysp0->Compute_Inhib_FfFb
      (lay0, this, lay0->laygp_i_val, lay0->laygp_netin.avg, lay0->laygp_netin.max,
       lay0->laygp_acts_eq.avg, laysp0->lay_gp_inhib);
    
    for(int gi = 0; gi < mx; gi++) {
      LEABRA_LAYER_STATE* lay = GetLayerState(l0 + gi);
      LEABRA_LAYER_SPEC_CPP* laysp = lay->GetLayerSpec(this);
      if(lay->lesioned() || !laysp->lay_gp_inhib.on) continue;
      
      lay->laygp_i_val = lay0->laygp_i_val;
      lay->laygp_netin = lay0->laygp_netin;
      lay->laygp_acts_eq = lay0->laygp_acts_eq;
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation

void LEABRA_NETWORK_STATE::Compute_Act_Thr(int thr_no) {
  StartTimer(NT_ACT, thr_no);
  
  if(net_misc.spike) {
    Compute_Act_Spike_Thr(thr_no);
  }
  else {
    Compute_Act_Rate_Thr(thr_no);
  }
  
  EndTimer(NT_ACT, thr_no);
}

void LEABRA_NETWORK_STATE::Compute_Act_Rate_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_Act_Rate(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Compute_Act_Spike_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_Act_Spike(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Compute_Act_Post_Thr(int thr_no) {
  StartTimer(NT_ACT_POST, thr_no);

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_Act_Post(uv, this, thr_no);
  }

  EndTimer(NT_ACT_POST, thr_no);
}

void LEABRA_NETWORK_STATE::ThalGatedNow() {
  times.thal_gate_cycle = cycle; // record current cycle
}


///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LEABRA_NETWORK_STATE::Compute_CycleStats_Pre() {
  trg_max_act = 0.0f;
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_CycleStats_Pre(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Compute_CycleStats_Thr(int thr_no) {
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  // this is when we should update clamped layers
  
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    LEABRA_LAYER_STATE* lay = gpd->GetLayerState(this);
    if(lay->lesioned()) continue;
    if(lay->hard_clamped && !updt_clamped)
      continue;
    LEABRA_AVG_MAX* am_act = ThrUnGpAvgMax(thr_no, li, AM_ACT);
    am_act->InitVals();
    LEABRA_AVG_MAX* am_act_raw = ThrUnGpAvgMax(thr_no, li, AM_ACT_RAW);
    am_act_raw->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = uv->flat_idx;
      am_act->UpdtVals(uv->act, flat_idx); 
      am_act_raw->UpdtVals(uv->act_raw, flat_idx); 
    }
  }
  
  if(net_misc.spike) {
    Compute_ActEqStats_Thr(thr_no); // act_eq requred separately for spiking
  }
}

void LEABRA_NETWORK_STATE::Compute_ActEqStats_Thr(int thr_no) {
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  // this is when we should update clamped layers
  
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    LEABRA_LAYER_STATE* lay = gpd->GetLayerState(this);
    if(lay->lesioned()) continue;
    if(lay->hard_clamped && !updt_clamped)
      continue;
    LEABRA_AVG_MAX* am_act_eq = ThrUnGpAvgMax(thr_no, li, AM_ACT_EQ);
    am_act_eq->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = uv->flat_idx;
      am_act_eq->UpdtVals(uv->act_eq, flat_idx);
    }
  }
}

void LEABRA_NETWORK_STATE::Compute_CycleStats_Post() {
  // integrate all the data from thread-specific guys
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  // this is when we should update clamped layers

  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    LEABRA_LAYER_STATE* lay = gpd->GetLayerState(this);
    if(lay->lesioned()) continue;
    if(lay->hard_clamped && !updt_clamped)
      continue;
    LEABRA_AVG_MAX& acts = gpd->acts;
    LEABRA_AVG_MAX& acts_eq = gpd->acts_eq;
    LEABRA_AVG_MAX& acts_raw = gpd->acts_raw;
    acts.InitVals();
    acts_eq.InitVals();
    acts_raw.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      LEABRA_AVG_MAX* am_act = ThrUnGpAvgMax(i, li, AM_ACT);
      acts.UpdtFmAvgMax(*am_act);
      if(net_misc.spike) {
        LEABRA_AVG_MAX* am_act_eq = ThrUnGpAvgMax(i, li, AM_ACT_EQ);
        acts_eq.UpdtFmAvgMax(*am_act_eq);
      }
      else {
        acts_eq.UpdtFmAvgMax(*am_act); // use act!
      }
      LEABRA_AVG_MAX* am_act_raw = ThrUnGpAvgMax(i, li, AM_ACT_RAW);
      acts_raw.UpdtFmAvgMax(*am_act_raw);
    }
    acts.CalcAvg();
    acts_eq.CalcAvg();
    acts_raw.CalcAvg();
    if(gpd->ungp_idx == lay->ungp_idx) { // main layer group
      if(lay->HasExtFlag(LAYER_STATE::TARG)) {
        trg_max_act = fmaxf(trg_max_act, acts_eq.max);
      }
      if(lay->Iconified()) {
        lay->icon_value = acts_eq.avg;
      }
    }
  }

  // Compute_OutputName();  this is now computed in LeabraNetwork::Cycle_Run() -- after end..
  Compute_RTCycles();
}

void LEABRA_NETWORK_STATE::Compute_RTCycles() {
  if(phase != MINUS_PHASE) return;
  if(rt_cycles > 0) return;  // already set
  if(trg_max_act > lstats.trg_max_act_crit)
    rt_cycles = cycle;
}

void LEABRA_NETWORK_STATE::Compute_GcIStats_Thr(int thr_no) {
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    LEABRA_LAYER_STATE* lay = gpd->GetLayerState(this);
    if(lay->lesioned()) continue;
    if(lay->hard_clamped && !updt_clamped)
      continue;
    LEABRA_AVG_MAX* am_un_g_i = ThrUnGpAvgMax(thr_no, li, AM_UN_G_I);
    am_un_g_i->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = uv->flat_idx;
      am_un_g_i->UpdtVals(uv->gc_i, flat_idx);
    }
  }
}

void LEABRA_NETWORK_STATE::Compute_GcIStats_Post() {
  // integrate all the data from thread-specific guys
  const bool updt_clamped = (cycle == 0 || cycle == 3 * times.quarter);
  
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    LEABRA_LAYER_STATE* lay = gpd->GetLayerState(this);
    if(lay->lesioned()) continue;
    if(lay->hard_clamped && !updt_clamped)
      continue;
    LEABRA_AVG_MAX& un_g_i = gpd->un_g_i;
    un_g_i.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      LEABRA_AVG_MAX* am_un_g_i = ThrUnGpAvgMax(i, li, AM_UN_G_I);
      un_g_i.UpdtFmAvgMax(*am_un_g_i);
    }

    un_g_i.CalcAvg();
  }
}

///////////////////////////////////////////////////////////////////////
//      DeepLeabra updates

void LEABRA_NETWORK_STATE::Compute_DeepRaw_Thr(int thr_no) {
  // already tested that this is the time to call us, including quarter..
  
  const int nu = ThrNUnits(thr_no);

  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_DeepRaw(uv, this, thr_no);
  }
  ThreadSyncSpin(thr_no, 0);

  if(deep.raw_net) {
    for(int i=0; i<nu; i++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->Send_DeepRawNetin(uv, this, thr_no);
    }
    ThreadSyncSpin(thr_no, 1);

    for(int i=0; i<nu; i++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->DeepRawNetin_Integ(uv, this, thr_no);
    }
    ThreadSyncSpin(thr_no, 2);
    InitDeepRawNetinTmp_Thr(thr_no);
  }

  Compute_DeepRawStats_Thr(thr_no);
  ThreadSyncSpin(thr_no, 0);
  if(thr_no == 0) {
    Compute_DeepRawStats_Post();
  }
}

void LEABRA_NETWORK_STATE::Compute_DeepRawStats_Thr(int thr_no) {
  // first go by layers
  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    if(gpd->lesioned(this))
      continue;
    LEABRA_AVG_MAX* am_raw = ThrUnGpAvgMax(thr_no, li, AM_DEEP_RAW);
    am_raw->InitVals();
    LEABRA_AVG_MAX* am_net = ThrUnGpAvgMax(thr_no, li, AM_DEEP_NET);
    am_net->InitVals();
    
    const int ust = ThrUnGpUnStart(thr_no, li);
    const int ued = ThrUnGpUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      const int flat_idx = uv->flat_idx;
      am_raw->UpdtVals(uv->deep_raw, flat_idx); 
      am_net->UpdtVals(uv->deep_raw_net, flat_idx); 
    }
  }
}

void LEABRA_NETWORK_STATE::Compute_DeepRawStats_Post() {
  // integrate all the data from thread-specific guys

  const int nugs = n_ungps_built;
  for(int li = 0; li < nugs; li++) {
    LEABRA_UNGP_STATE* gpd = GetUnGpState(li);
    if(gpd->lesioned(this))
      continue;
    LEABRA_AVG_MAX& am_deep_raw = gpd->am_deep_raw;
    am_deep_raw.InitVals();
    LEABRA_AVG_MAX& am_deep_net = gpd->am_deep_raw_net;
    am_deep_net.InitVals();

    for(int i=0; i < n_thrs_built; i++) {
      LEABRA_AVG_MAX* am_raw = ThrUnGpAvgMax(i, li, AM_DEEP_RAW);
      am_deep_raw.UpdtFmAvgMax(*am_raw);
      LEABRA_AVG_MAX* am_net = ThrUnGpAvgMax(i, li, AM_DEEP_NET);
      am_deep_net.UpdtFmAvgMax(*am_net);
    }
    am_deep_raw.CalcAvg();
    am_deep_net.CalcAvg();
  }
}

void LEABRA_NETWORK_STATE::ClearDeepActs_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->ClearDeepActs(uv, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::ClearMSNTrace_Thr(int thr_no) {
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LEABRA_CON_STATE* scg = ThrSendConState(thr_no, i);
    if(scg->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = scg->GetConSpec(this);
    if(cs->GetStateSpecType() != LEABRA_NETWORK_STATE::T_MSNConSpec) continue;
    STATE_CLASS(MSNConSpec)* mscs = (STATE_CLASS(MSNConSpec)*)cs;
    mscs->ClearMSNTrace(scg, this, thr_no);
  }
}

///////////////////////////////////////////////////////////////////////
//	Quarter Final

void LEABRA_NETWORK_STATE::Quarter_Final_Pre() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Quarter_Final_Pre(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Quarter_Final_Unit_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Quarter_Final(uv, this, thr_no);
  }
  if(quarter == 3) {
    Compute_CosDiff_Thr(thr_no); // more efficient to lump here -- must come before Compute_dWt because cos_diff_avg_lrn is used 
  }
}

void LEABRA_NETWORK_STATE::Quarter_Final_Layers() {
  if(quarter == 3) {
    Compute_CosDiff_Agg();      // aggregate from Unit_Thr
  }
  
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Quarter_Final_Layer(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Quarter_Final_Counters() {
  quarter++;                    // always shows +1 at end of quarter
}


///////////////////////////////////////////////////////////////////////
//      Trial Update and Final

void LEABRA_NETWORK_STATE::Compute_AbsRelNetin() {
  // always get layer-level netin max / avg values
  // decision of whether to run prjn-level is done by layers
  for(int li = 0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_AbsRelNetin(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Compute_AvgAbsRelNetin() {
  for(int li = 0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_AvgAbsRelNetin(lay, this);
  }
}

///////////////////////////////////////////////////////////////////////
//	Learning


void LEABRA_NETWORK_STATE::Compute_dWt_Layer_pre() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_dWt_Layer_pre(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Compute_dWt_Thr(int thr_no) {
  StartTimer(NT_DWT, thr_no);

  // note: not currently using
  // Compute_dWt_VecVars_Thr(thr_no);
  // ThreadSyncSpin(thr_no);

  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LEABRA_CON_STATE* scg = ThrSendConState(thr_no, i);
    if(scg->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = scg->GetConSpec(this);
    if(!cs->Quarter_LearnNow(this->quarter)) continue;
    cs->Compute_dWt(scg, this, thr_no);
  }

  if(net_misc.bias_learn) {
    const int nu = ThrNUnits(thr_no);
    for(int i=0; i<nu; i++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->Compute_dWt(uv, this, thr_no); // just bias weights
    }
  }
  
  EndTimer(NT_DWT, thr_no);
}

void LEABRA_NETWORK_STATE::Compute_Weights_Thr(int thr_no) {
  StartTimer(NT_WT, thr_no);

  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LEABRA_CON_STATE* scg = ThrSendConState(thr_no, i);
    if(scg->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = scg->GetConSpec(this);
    cs->Compute_Weights(scg, this, thr_no);
  }

  if(net_misc.bias_learn) {
    const int nu = ThrNUnits(thr_no);
    for(int i=0; i<nu; i++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, i);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->Compute_Weights(uv, this, thr_no); // just bias weights
    }
  }
  
  EndTimer(NT_WT, thr_no);
}

void LEABRA_NETWORK_STATE::Compute_WtBal_Thr(int thr_no) {
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    LEABRA_CON_STATE* rcg = ThrRecvConState(thr_no, i);
    if(rcg->NotActive()) continue;
    rcg->GetConSpec(this)->Compute_WtBal(rcg, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Compute_WtBalStats() {
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    const int n_prj = lay->n_recv_prjns;
    for(int pj=0; pj<n_prj; pj++) {
      LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(this, pj);
      if(!prjn->IsActive(this)) continue;
      prjn->wt_avg_max = 0.0f;
      prjn->wt_avg_avg = 0.0f;
    }
    int denom = 0;

    const int ust = lay->units_flat_idx;
    const int ued = lay->units_flat_idx + lay->n_units;
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = GetUnitState(ui);
      if(uv->lesioned()) continue;
      denom++;
      for(int pj=0; pj<n_prj; pj++) {
        LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(this, pj);
        if(!prjn->IsActive(this)) continue;
        LEABRA_CON_STATE* cg = uv->RecvConState(this, prjn->recv_idx);
        prjn->wt_avg_max = fmaxf(prjn->wt_avg_max, cg->wt_avg);
        prjn->wt_avg_avg += cg->wt_avg;
      }
    }
    if(denom > 0) {
      float norm = 1.0f / (float)denom;
      for(int pj=0; pj<n_prj; pj++) {
        LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(this, pj);
        if(!prjn->IsActive(this)) continue;
        prjn->wt_avg_avg *= norm;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////
//      Stats

void LEABRA_NETWORK_STATE::Set_ExtRew(bool avail, float ext_rew_val) {
  ext_rew_avail = avail;
  ext_rew = ext_rew_val;
}

void LEABRA_NETWORK_STATE::Compute_ExtRew() {
  // assumes any ext rew computation has happened before this point, and set the
  // network ext_rew and ext_rew_avail flags appropriately
  if(ext_rew_avail) {
    avg_ext_rew.Increment(ext_rew);
  }
}

float LEABRA_NETWORK_STATE::Compute_SSE_Layer(LAYER_STATE* llay, int& n_vals, bool unit_avg, bool sqrt) {
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)llay;
  LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
  return ls->Compute_SSE(lay, this, n_vals, unit_avg, sqrt);
}

void LEABRA_NETWORK_STATE::Compute_NormErr_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG))
      continue;
    if(lay->layer_type == LAYER_STATE::HIDDEN)
      continue;

    float lay_nerr = 0.0f;    float lay_trg_n = 0.0f;
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    bool targ_active = false;
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      LEABRA_UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      lay_nerr += us->Compute_NormErr(uv, this, thr_no, targ_active);
      if(targ_active) lay_trg_n += 1.0f;
    }
    ThrLayStats(thr_no, li, 0, NORMERR) = lay_nerr;
    ThrLayStats(thr_no, li, 1, NORMERR) = lay_trg_n;
  }
}

void LEABRA_NETWORK_STATE::Compute_NormErr_Agg() {
  float nerr_sum = 0.0f;
  float nerr_avail = 0.0f;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_MaxErr(lay, this);  // do this here as well..
    float nerr = ls->Compute_NormErr(lay, this);
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

void LEABRA_NETWORK_STATE::Compute_CosErr_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG))
      continue;
    if(lay->layer_type == LAYER_STATE::HIDDEN)
      continue;

    float cosv = 0.0f;  float cosvp = 0.0f;  float ssm = 0.0f;
    float ssp = 0.0f;   float sst = 0.0f;    float nvals = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      if(!uv->HasExtFlag(UNIT_STATE::COMP_TARG)) continue;
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

float LEABRA_NETWORK_STATE::Compute_CosErr_Agg() {
  float cosv = 0.0f;
  float cosvp = 0.0f;
  float cosvsp = 0.0f;
  int n_lays = 0;
  int lay_vals = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    cosv += ls->Compute_CosErr(lay, this, lay_vals);
    if(lay_vals > 0) {
      n_lays++;
      if(net_misc.deep) {
        cosvp += lay->cos_err_prv;
        cosvsp += lay->cos_err_vs_prv;
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

void LEABRA_NETWORK_STATE::Compute_CosDiff_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);
    
    const float avg_m = lgpd->acts_m.avg;
    const float avg_p = lgpd->acts_p.avg;

    float cosv = 0.0f;  float ssm = 0.0f;  float sst = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
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

float LEABRA_NETWORK_STATE::Compute_CosDiff_Agg() {
  float cosv = 0.0f;
  int n_lays = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    float lcosv = ls->Compute_CosDiff(lay, this);
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) {
      cosv += lcosv;
      n_lays++;
    }
  }
  // second step for sharing cos_diff-based lrate_mod
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_CosDiff_post(lay, this);
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

void LEABRA_NETWORK_STATE::Compute_AvgActDiff_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;

    float adiff = 0.0f;  float nd = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      adiff += uv->act_dif;
      nd += 1.0f;
    }

    ThrLayStats(thr_no, li, 0, AVGACTDIFF) = adiff;
    ThrLayStats(thr_no, li, 1, AVGACTDIFF) = nd;
  }
}

float LEABRA_NETWORK_STATE::Compute_AvgActDiff_Agg() {
  float adiff = 0.0f;
  int n_lays = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    float ladiff = ls->Compute_AvgActDiff(lay, this);
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) {
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

void LEABRA_NETWORK_STATE::Compute_TrialCosDiff_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);

    const float avg_p = lgpd->acts_p.avg;
    const float avg_q0 = lgpd->acts_p_avg; // use running average -- best we've got
    
    float cosv = 0.0f;  float ssm = 0.0f;  float sst = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
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

float LEABRA_NETWORK_STATE::Compute_TrialCosDiff_Agg() {
  float cosv = 0.0f;
  const int nlay = n_layers_built;
  int n_lays = 0;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    float lcosv = ls->Compute_TrialCosDiff(lay, this);
    if(!lay->HasExtFlag(LAYER_STATE::COMP_TARG)) {
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

void LEABRA_NETWORK_STATE::Compute_ActMargin_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    // LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);

    const float low_thr = lay->margin.low_thr;
    const float med_thr = lay->margin.med_thr;
    const float hi_thr = lay->margin.hi_thr;
    
    float low_avg = 0.0f;  float hi_avg = 0.0f;  float med_avg = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
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

void LEABRA_NETWORK_STATE::Compute_ActMargin_Agg() {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_ActMargin(lay, this);
  }
}

void LEABRA_NETWORK_STATE::Compute_RTCycles_Agg() {
  if(rt_cycles < 0) // never reached target
    rt_cycles = cycle;       // set to current cyc -- better for integrating
  avg_cycles.Increment(rt_cycles);
}

void LEABRA_NETWORK_STATE::Compute_NetSd_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);
    float net_avg = lgpd->netin.avg;

    bool ugp = ls->HasUnitGpInhib(lay);
    
    float var = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      float netsb;
      if(ugp) {
        LEABRA_UNGP_STATE* gpd = uv->GetOwnUnGp(this);
        net_avg = gpd->netin.avg;
      }
      netsb = (uv->net - net_avg);
      var += netsb * netsb;
    }
    ThrLayStats(thr_no, li, 0, NETSD) = var;
  }
}

float LEABRA_NETWORK_STATE::Compute_NetSd_Agg() {
  float var = 0.0f;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    float lvar = ls->Compute_NetSd(lay, this);
    if(!lay->HasExtFlag(LAYER_STATE::EXT)) {
      var += lvar;
    }
  }
  net_sd = sqrtf(var);
  avg_net_sd.Increment(net_sd);
  return net_sd;
}

void LEABRA_NETWORK_STATE::Compute_HogDeadPcts_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);

    float hog = 0.0f;  float dead = 0.0f;  float nu = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      LEABRA_UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      if(uv->act_avg > ls->lstats.hog_thr) {
        hog += 1.0f;
      }
      else if(uv->act_avg < ls->lstats.dead_thr) {
        dead += 1.0f;
      }
      nu += 1.0f;
    }

    ThrLayStats(thr_no, li, 0, HOGDEAD) = hog;
    ThrLayStats(thr_no, li, 1, HOGDEAD) = dead;
    ThrLayStats(thr_no, li, 2, HOGDEAD) = nu;
  }
}

void LEABRA_NETWORK_STATE::Compute_HogDeadPcts_Agg() {
  float hog = 0.0f;
  float dead = 0.0f;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LEABRA_LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
    ls->Compute_HogDeadPcts(lay, this);
    hog += lay->hog_pct;
    dead += lay->dead_pct;
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

void LEABRA_NETWORK_STATE::Compute_PlusStats_Thr(int thr_no) {
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

void LEABRA_NETWORK_STATE::Compute_PlusStats_Agg() {
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


/////////////////////////////////////////////////////
//              EpochStats

void LEABRA_NETWORK_STATE::Compute_AvgCycles() {
  avg_cycles.GetAvg_Reset();
}

void LEABRA_NETWORK_STATE::Compute_AvgSendPct() {
  avg_send_pct.GetAvg_Reset();
}

void LEABRA_NETWORK_STATE::Compute_AvgExtRew() {
  avg_ext_rew.GetAvg_Reset();
}

void LEABRA_NETWORK_STATE::Compute_AvgNormErr() {
  avg_norm_err.GetAvg_Reset();
}

void LEABRA_NETWORK_STATE::Compute_AvgCosErr() {
  avg_cos_err.GetAvg_Reset();

  if(net_misc.deep) {
    avg_cos_err_prv.GetAvg_Reset();
    avg_cos_err_vs_prv.GetAvg_Reset();
  }
}

void LEABRA_NETWORK_STATE::Compute_AvgCosDiff() {
  avg_cos_diff.GetAvg_Reset();
}

void LEABRA_NETWORK_STATE::Compute_AvgAvgActDiff() {
  avg_avg_act_diff.GetAvg_Reset();
}

void LEABRA_NETWORK_STATE::Compute_AvgTrialCosDiff() {
  avg_trial_cos_diff.GetAvg_Reset();
}

void LEABRA_NETWORK_STATE::Compute_AvgNetSd() {
  avg_net_sd.GetAvg_Reset();
}


void LEABRA_NETWORK_STATE::Compute_EpochWeights_Thr(int thr_no) {
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    LEABRA_CON_STATE* scg = ThrSendConState(thr_no, i);
    if(scg->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = scg->GetConSpec(this);
    cs->Compute_EpochWeights(scg, this, thr_no);
  }
}

void LEABRA_NETWORK_STATE::Compute_EpochStats_Layer(LAYER_STATE* llay) {
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)llay;
  LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(this);
  ls->Compute_EpochStats(lay, this);
}

