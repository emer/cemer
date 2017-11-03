// this is the .cpp implementation companion to NetworkState_core.h -- it is included in _cpp and
// _cuda files to implement code that cannot otherwise be inlined in NetworkState_core, which cannot
// for example include any other State files because it is a key dependency for all of them


/////////////////////////////////////////////////////////
//              Main compute algorithm code!


void NETWORK_STATE::Init_InputData_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Init_InputData(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_InputData_Layer() {
  for(int i=0; i < n_layers_built; i++) {
    LAYER_STATE* lay = GetLayerState(i);
    if(lay->lesioned()) continue;
    lay->Init_InputData(this);
  }
}

void NETWORK_STATE::Init_Acts_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Init_Acts(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_dWt_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive()) continue;
      CON_SPEC_CPP* cs = rcg->GetConSpec(this);
      cs->Init_dWt(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC_CPP* cs = scg->GetConSpec(this);
      cs->Init_dWt(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Init_dWt(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_Weights_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive() || rcg->Sharing()) continue;
      PRJN_STATE* prjn = rcg->GetPrjnState(this);
      PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(this);
      CON_SPEC_CPP* cs = rcg->GetConSpec(this);
      if(pspec->init_wts) {
        pspec->Init_Weights_Prjn(prjn, this, thr_no, rcg);
      }
      else {
        cs->Init_Weights(rcg, this, thr_no);
      }
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      PRJN_STATE* prjn = scg->GetPrjnState(this);
      PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(this);
      CON_SPEC_CPP* cs = scg->GetConSpec(this);
      if(pspec->init_wts) continue; // do with recv's below
      cs->Init_Weights(scg, this, thr_no);
    }
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive()) continue;
      PRJN_STATE* prjn = rcg->GetPrjnState(this);
      PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(this);
      if(pspec->init_wts) {
        pspec->Init_Weights_Prjn(prjn, this, thr_no, rcg);
      }
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Init_Weights(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_Weights_1Thr() {
  for(int ui=1; ui < n_units_built; ui++) {
    UNIT_STATE* uv = GetUnitState(ui);
    if(uv->lesioned()) continue;
    int thr_no = uv->thread_no;
    
    if(RecvOwnsCons()) {
      const int nrcg = UnNRecvConGps(ui);
      for(int i=0; i<nrcg; i++) {
        CON_STATE* rcg = RecvConState(ui, i);
        if(rcg->NotActive() || rcg->Sharing()) continue;
        PRJN_STATE* prjn = rcg->GetPrjnState(this);
        PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(this);
        CON_SPEC_CPP* cs = rcg->GetConSpec(this);
        if(pspec->init_wts) {
          pspec->Init_Weights_Prjn(prjn, this, thr_no, rcg);
        }
        else {
          cs->Init_Weights(rcg, this, thr_no);
        }
      }
    }
    else {
      const int nscg = UnNSendConGps(ui);
      for(int i=0; i<nscg; i++) {
        CON_STATE* scg = SendConState(ui, i);
        if(scg->NotActive()) continue;
        PRJN_STATE* prjn = scg->GetPrjnState(this);
        PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(this);
        if(pspec->init_wts) continue; // do with recv's below
        CON_SPEC_CPP* cs = scg->GetConSpec(this);
        cs->Init_Weights(scg, this, thr_no);
      }
      const int nrcg = UnNRecvConGps(ui);
      for(int i=0; i<nrcg; i++) {
        CON_STATE* rcg = RecvConState(ui, i);
        if(rcg->NotActive()) continue;
        PRJN_STATE* prjn = rcg->GetPrjnState(this);
        PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(this);
        if(pspec->init_wts) {
          pspec->Init_Weights_Prjn(prjn, this, thr_no, rcg);
        }
      }
    }
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Init_Weights(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_Weights_renorm_Thr(int thr_no) {
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i);
    if(rcg->NotActive()) continue;
    PRJN_STATE* prjn = rcg->GetPrjnState(this);
    PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(this);
    pspec->Init_Weights_renorm(prjn, this, thr_no, rcg);
  }
}

void NETWORK_STATE::Init_Weights_sym_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive()) continue;
      CON_SPEC_CPP* cs = rcg->GetConSpec(this);
      cs->Init_Weights_sym_r(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC_CPP* cs = scg->GetConSpec(this);
      cs->Init_Weights_sym_s(scg, this, thr_no);
    }
  }
}

void NETWORK_STATE::Init_Weights_post_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive()) continue;
      CON_SPEC_CPP* cs = rcg->GetConSpec(this);
      cs->Init_Weights_post(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC_CPP* cs = scg->GetConSpec(this);
      cs->Init_Weights_post(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Init_Weights_post(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_Weights_Layer() {
  for(int i=0; i < n_layers_built; i++) {
    LAYER_STATE* lay = GetLayerState(i);
    if(lay->lesioned()) continue;
    lay->Init_Weights_Layer(this);
  }
}

void NETWORK_STATE::Init_Counters() {
  // this is one you do not reinit: loops over inits!
//   batch = 0;
  epoch = 0;
  group = 0;
  // group_name = "";
  trial = 0;
  // trial_name = "";
  tick = 0;
  cycle = 0;
  time = 0.0f;
  total_trials = 0;

  Init_Counters_State();
}

void NETWORK_STATE::Init_Counters_State() {
  
}

void NETWORK_STATE::Init_Stats() {
  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse.ResetAvg();
  cnt_err = 0.0f;
  cur_cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  sum_prerr.InitVals();
  epc_prerr.InitVals();
  
  // output_name = "";

  // also call at the layer level
  Init_Stats_Layer();
}

void NETWORK_STATE::Init_Stats_Layer() {
  for(int i=0; i < n_layers_built; i++) {
    LAYER_STATE* lay = GetLayerState(i);
    if(lay->lesioned()) continue;
    lay->Init_Stats();
  }
}

void NETWORK_STATE::Compute_Netin_Thr(int thr_no) {
  // if(threads.get_timing)
  //   net_timing[thr_no]->netin.StartTimer(true); // reset

  // unit-level, as separate pass -- this initializes net
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_Netin(uv, this, thr_no);
  }
  // assuming recv owns -- otherwise use Send
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i);
    if(rcg->NotActive()) continue;
    CON_SPEC* cs = rcg->GetConSpec(this);
    cs->Compute_Netin(rcg, this, thr_no);
  }

  // if(threads.get_timing)
  //   net_timing[thr_no]->netin.EndIncrAvg();
}

void NETWORK_STATE::Send_Netin_Thr(int thr_no) {
  // typically the whole point of using sender based net input is that you
  // want to check here if the sending unit's activation is above some threshold
  // so you don't send if it isn't above that threshold..
  // this isn't implemented here though.
  // if(threads.get_timing)
  //   net_timing[thr_no]->netin.StartTimer(true); // reset

  if(NetinPerPrjn()) {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC* cs = scg->GetConSpec(this);
      cs->Send_Netin_PerPrjn(scg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC* cs = scg->GetConSpec(this);
      cs->Send_Netin(scg, this, thr_no);
    }
  }

  // if(threads.get_timing)
  //   net_timing[thr_no]->netin.EndIncrAvg();
}

void NETWORK_STATE::Send_Netin_Integ() {
  // now need to roll up the netinput into unit vals
  const int nu = n_units_built;
  const int nt = n_thrs_built;
  if(NetinPerPrjn()) {
    for(int i=1;i<nu;i++) {     // 0 = dummy idx
      float nw_nt = 0.0f;
      for(int p=0;p<UnNRecvConGps(i);p++) {
        for(int j=0;j<nt;j++) {
          float& ntmp = thrs_send_netin_tmp[nt][p * n_units_built + i];
          nw_nt += ntmp;
          ntmp = 0.0f; // reset immediately
        }
      }
      UnitState_cpp* uv = GetUnitState(i);
      uv->GetUnitSpec(this)->Compute_SentNetin(uv, this, nw_nt);
    }
  }
  else {
    for(int i=1;i<nu;i++) {     // 0 = dummy idx
      UnitState_cpp* uv = GetUnitState(i);
      float nw_nt = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ntmp = thrs_send_netin_tmp[nt][i];
        nw_nt += ntmp;
        ntmp = 0.0f;
      }
      uv->GetUnitSpec(this)->Compute_SentNetin(uv, this, nw_nt);
    }
  }
}

void NETWORK_STATE::Compute_Act_Thr(int thr_no) {
  // if(threads.get_timing)
  //   net_timing[thr_no]->act.StartTimer(true); // reset

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_Act(uv, this, thr_no);
  }

  // if(threads.get_timing)
  //   net_timing[thr_no]->act.EndIncrAvg();
}

void NETWORK_STATE::Compute_NetinAct_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      us->Compute_NetinAct(uv, this, thr_no);
    }
    ThreadSyncSpin(thr_no, li % 3);   // need to sync for each layer!
  }
}

void NETWORK_STATE::Compute_dWt_Thr(int thr_no) {
  // if(threads.get_timing)
  //   net_timing[thr_no]->dwt.StartTimer(true); // reset

  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive()) continue;
      CON_SPEC* cs = rcg->GetConSpec(this);
      cs->Compute_dWt(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC* cs = scg->GetConSpec(this);
      cs->Compute_dWt(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_dWt(uv, this, thr_no);
  }

  // if(threads.get_timing)
  //   net_timing[thr_no]->dwt.EndIncrAvg();
}

void NETWORK_STATE::Compute_Weights_Thr(int thr_no) {
  // if(threads.get_timing)
  //   net_timing[thr_no]->wt.StartTimer(true); // reset

  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive()) continue;
      CON_SPEC* cs = rcg->GetConSpec(this);
      cs->Compute_Weights(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC* cs = scg->GetConSpec(this);
      cs->Compute_Weights(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
    us->Compute_Weights(uv, this, thr_no);
  }

  // if(threads.get_timing)
  //   net_timing[thr_no]->wt.EndIncrAvg();
}

void NETWORK_STATE::Compute_SSE_Thr(int thr_no) {
  // gather all the raw data for sse computation
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    if(!lay->HasExtFlag(UNIT_STATE::COMP_TARG))
      continue;
    if(lay->layer_type == LAYER_STATE::HIDDEN)
      continue;

    float lay_sse = 0.0f;   float lay_n = 0.0f;
    
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    bool has_targ = false;
    for(int ui = ust; ui < ued; ui++) {
      UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      lay_sse += us->Compute_SSE(uv, this, thr_no, has_targ);
      if(has_targ) lay_n += 1.0f;
    }
    ThrLayStats(thr_no, li, 0, SSE) = lay_sse;
    ThrLayStats(thr_no, li, 1, SSE) = lay_n;
  }
}

void NETWORK_STATE::Compute_SSE_Agg(bool unit_avg, bool sqrt) {
  sse = 0.0f;
  int n_vals = 0;
  int lay_vals = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    sse += lay->Compute_SSE(this, lay_vals, unit_avg, sqrt);
    n_vals += lay_vals;
  }
  if(unit_avg && n_vals > 0)
    sse /= (float)n_vals;
  if(sqrt)
    sse = sqrtf(sse);
  avg_sse.Increment(sse);
  if(sse > stats.cnt_err_tol)
    cur_cnt_err += 1.0f;
}

void NETWORK_STATE::Compute_PRerr_Thr(int thr_no) {
  // gather all the raw data for prerr computation
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    if(!lay->HasExtFlag(UNIT_STATE::COMP_TARG))
      continue;
    if(lay->layer_type == LAYER_STATE::HIDDEN)
      continue;

    float true_pos = 0.0f; float false_pos = 0.0f; float false_neg = 0.0f;
    float true_neg = 0.0f; float lay_n = 0.0f;

    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      UNIT_SPEC_CPP* us = uv->GetUnitSpec(this);
      bool has_targ = us->Compute_PRerr(uv, this, thr_no, true_pos, false_pos, false_neg, true_neg);
      if(has_targ) lay_n += 1.0f;
    }
    ThrLayStats(thr_no, li, 0, PRERR) = true_pos;
    ThrLayStats(thr_no, li, 1, PRERR) = false_pos;
    ThrLayStats(thr_no, li, 2, PRERR) = false_neg;
    ThrLayStats(thr_no, li, 3, PRERR) = true_neg;
    ThrLayStats(thr_no, li, 4, PRERR) = lay_n;
  }
}

void NETWORK_STATE::Compute_PRerr_Agg() {
  prerr.InitVals();
  int n_vals = 0;
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    int lay_vals = lay->Compute_PRerr(this);
    if(lay_vals > 0) {
      prerr.IncrVals(lay->prerr);
    }
    n_vals += lay_vals;
  }
  if(n_vals > 0) {
    sum_prerr.IncrVals(prerr);
    prerr.ComputePR();
    sum_prerr.ComputePR();
  }
}

void NETWORK_STATE::Compute_EpochStats_Layer() {
  for(int li=0; li < n_layers_built; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    lay->Compute_EpochStats(this);
  }
}

void NETWORK_STATE::Compute_EpochSSE() {
  sum_sse = avg_sse.sum;
  cnt_err = cur_cnt_err;
  if(avg_sse.n > 0) {
    pct_err = cnt_err / (float)avg_sse.n;
    pct_cor = 1.0f - pct_err;
  }
  avg_sse.GetAvg_Reset();

  cur_cnt_err = 0.0f;
}

void NETWORK_STATE::Compute_EpochPRerr() {
  epc_prerr = sum_prerr;
  epc_prerr.ComputePR();        // make sure, in case of dmem summing
  sum_prerr.InitVals();         // reset!
}

CON_STATE* NETWORK_STATE::FindRecipRecvCon(int& con_idx, UNIT_STATE* su, UNIT_STATE* ru) {
  LAYER_STATE* ru_lay = ru->GetOwnLayer(this);
  LAYER_STATE* su_lay = su->GetOwnLayer(this);
  PRJN_STATE* prj = su_lay->FindRecvPrjnFrom(this, ru_lay->layer_idx);
  if(!prj) return NULL;
  CON_STATE* rcg = su->RecvConState(this, prj->recv_idx);
  if(!rcg) return NULL;
  con_idx = rcg->FindConFromIdx(ru->flat_idx);
  if(con_idx >= 0) return rcg;
  return NULL;
}

CON_STATE* NETWORK_STATE::FindRecipSendCon(int& con_idx, UNIT_STATE* ru, UNIT_STATE* su) {
  LAYER_STATE* ru_lay = ru->GetOwnLayer(this);
  LAYER_STATE* su_lay = su->GetOwnLayer(this);
  PRJN_STATE* prj = su_lay->FindRecvPrjnFrom(this, ru_lay->layer_idx);
  if(!prj) return NULL;
  CON_STATE* scg = ru->SendConState(this, prj->send_idx);
  if(!scg) return NULL;
  con_idx = scg->FindConFromIdx(su->flat_idx);
  if(con_idx >= 0) return scg;
  return NULL;
}  

LAYER_STATE* NETWORK_STATE::FindLayerName(const char* lay_name) const {
  for(int li=0; li < n_layers_built; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    if(lay->LayerNameIs(lay_name))
      return lay;
  }
  return NULL;
}

PRJN_STATE* NETWORK_STATE::FindRecvPrjnFrom(LAYER_STATE* recv_lay, LAYER_STATE* send_lay) {
  return recv_lay->FindRecvPrjnFromLay(this, send_lay);
}



///////////////////////////////////////////////////////////////////////////
//                      Build Infrastructure


void NETWORK_STATE::AllocSpecMem() {
  NetStateMalloc((void**)&layer_specs, n_layer_specs_built * sizeof(void*));
  NetStateMalloc((void**)&prjn_specs, n_prjn_specs_built * sizeof(void*));
  NetStateMalloc((void**)&unit_specs, n_unit_specs_built * sizeof(void*));
  NetStateMalloc((void**)&con_specs, n_con_specs_built * sizeof(void*));
}


LAYER_SPEC_CPP* NETWORK_STATE::NewLayerSpec(int spec_type) const {
  return new LAYER_SPEC_CPP;
}

PRJN_SPEC_CPP* NETWORK_STATE::NewPrjnSpec(int spec_type) const {
  switch(spec_type) {
  case T_ProjectionSpec:
    return new STATE_CLASS_CPP(ProjectionSpec)();
  case T_FullPrjnSpec:
    return new STATE_CLASS_CPP(FullPrjnSpec)();
  case T_OneToOnePrjnSpec:
    return new STATE_CLASS_CPP(OneToOnePrjnSpec)();
  case T_GpOneToOnePrjnSpec:
    return new STATE_CLASS_CPP(GpOneToOnePrjnSpec)();
  case T_MarkerGpOneToOnePrjnSpec:
    return new STATE_CLASS_CPP(MarkerGpOneToOnePrjnSpec)();
  case T_GpMapConvergePrjnSpec:
    return new STATE_CLASS_CPP(GpMapConvergePrjnSpec)();
  case T_GpMapDivergePrjnSpec:
    return new STATE_CLASS_CPP(GpMapDivergePrjnSpec)();
  case T_RandomPrjnSpec:
    return new STATE_CLASS_CPP(RandomPrjnSpec)();
  case T_UniformRndPrjnSpec:
    return new STATE_CLASS_CPP(UniformRndPrjnSpec)();
  case T_PolarRndPrjnSpec:
    return new STATE_CLASS_CPP(PolarRndPrjnSpec)();
  case T_TesselPrjnSpec:
    return new STATE_CLASS_CPP(TesselPrjnSpec)();
  case T_GpTesselPrjnSpec:
    return new STATE_CLASS_CPP(GpTesselPrjnSpec)();
  // case T_SymmetricPrjnSpec:
  //   return new STATE_CLASS_CPP(SymmetricPrjnSpec)();
  case T_TiledGpRFPrjnSpec:
    return new STATE_CLASS_CPP(TiledGpRFPrjnSpec)();
  case T_TiledGpRFOneToOnePrjnSpec:
    return new STATE_CLASS_CPP(TiledGpRFOneToOnePrjnSpec)();
  case T_TiledGpRFOneToOneWtsPrjnSpec:
    return new STATE_CLASS_CPP(TiledGpRFOneToOneWtsPrjnSpec)();
  case T_TiledSubGpRFPrjnSpec:
    return new STATE_CLASS_CPP(TiledSubGpRFPrjnSpec)();
  // case T_TiledRFPrjnSpec:
  //   return new STATE_CLASS_CPP(TiledRFPrjnSpec)();
  // case T_TiledNovlpPrjnSpec:
  //   return new STATE_CLASS_CPP(TiledNovlpPrjnSpec)();
  // case T_TiledDivGpRFPrjnSpec:
  //   return new STATE_CLASS_CPP(TiledDivGpRFPrjnSpec)();
  // case T_TiledGpMapConvergePrjnSpec:
  //   return new STATE_CLASS_CPP(TiledGpMapConvergePrjnSpec)();
  case T_GaussRFPrjnSpec:
    return new STATE_CLASS_CPP(GaussRFPrjnSpec)();
  // case T_GradientWtsPrjnSpec:
  //   return new STATE_CLASS_CPP(GradientWtsPrjnSpec)();
  // case T_PFCPrjnSpec:
  //   return new STATE_CLASS_CPP(PFCPrjnSpec)();
  // case T_BgPfcPrjnSpec:
  //   return new STATE_CLASS_CPP(BgPfcPrjnSpec)();
  // case T_ConPoolPrjnSpec:
  //   return new STATE_CLASS_CPP(ConPoolPrjnSpec)();
  // case T_SmallWorldPrjnSpec:
  //   return new STATE_CLASS_CPP(SmallWorldPrjnSpec)();
  // case T_ScalarValSelfPrjnSpec:
  //   return new STATE_CLASS_CPP(ScalarValSelfPrjnSpec)();  
  // case T_SaliencyPrjnSpec:
  //   return new STATE_CLASS_CPP(SaliencyPrjnSpec)();
  // case T_CerebConj2PrjnSpec:
  //   return new STATE_CLASS_CPP(CerebConj2PrjnSpec)();  
  // case T_RowColPrjnSpec:
  //   return new STATE_CLASS_CPP(RowColPrjnSpec)();
  // case T_TopoWtsPrjnSpec:
  //   return new STATE_CLASS_CPP(TopoWtsPrjnSpec)();
  // case T_:
  //   return new STATE_CLASS_CPP()();
  // case T_:
  //   return new STATE_CLASS_CPP()();
  }    
  StateError("NewPrjnSpec, type not recognized");
  return new STATE_CLASS_CPP(ProjectionSpec)();
}

UNIT_SPEC_CPP* NETWORK_STATE::NewUnitSpec(int spec_type) const {
  return new UNIT_SPEC_CPP;
}

CON_SPEC_CPP* NETWORK_STATE::NewConSpec(int spec_type) const {
  return new CON_SPEC_CPP;
}


void NETWORK_STATE::AllocLayerUnitMem() {
  NetStateMalloc((void**)&layers_mem, n_layers_built * layer_state_size);
  NetStateMalloc((void**)&prjns_mem, n_prjns_built * prjn_state_size);
  NetStateMalloc((void**)&ungps_mem, n_ungps_built * ungp_state_size);
  NetStateMalloc((void**)&ungp_lay_idxs, n_ungps_built * sizeof(int));
  NetStateMalloc((void**)&lay_send_prjns, n_prjns_built * sizeof(int));
    
  NetStateMalloc((void**)&units_lays, n_units_built * sizeof(int));
  NetStateMalloc((void**)&units_ungps, n_units_built * sizeof(int));
  NetStateMalloc((void**)&units_thrs, n_units_built * sizeof(int));
  NetStateMalloc((void**)&units_thr_un_idxs, n_units_built * sizeof(int));
  NetStateMalloc((void**)&thrs_n_units, n_thrs_built * sizeof(int));
  NetStateMalloc((void**)&thrs_unit_idxs, n_thrs_built * sizeof(int*));
  NetStateMalloc((void**)&thrs_lay_unit_idxs, n_thrs_built * sizeof(int*));
  NetStateMalloc((void**)&thrs_ungp_unit_idxs, n_thrs_built * sizeof(int*));
  NetStateMalloc((void**)&thrs_lay_stats, n_thrs_built * sizeof(float*));
  NetStateMalloc((void**)&thrs_units_mem, n_thrs_built * sizeof(char*));

  NetStateMalloc((void**)&units_n_recv_cgps, n_units_built * sizeof(int));
  NetStateMalloc((void**)&units_n_send_cgps, n_units_built * sizeof(int));

  NetStateMalloc((void**)&thrs_units_n_recv_cgps, n_thrs_built * sizeof(int*));
  NetStateMalloc((void**)&thrs_units_n_send_cgps, n_thrs_built * sizeof(int*));

  NetStateMalloc((void**)&thrs_n_recv_cgps, n_thrs_built * sizeof(int));
  NetStateMalloc((void**)&thrs_n_send_cgps, n_thrs_built * sizeof(int));

  NetStateMalloc((void**)&thrs_recv_cgp_mem, n_thrs_built * sizeof(char*));
  NetStateMalloc((void**)&thrs_send_cgp_mem, n_thrs_built * sizeof(char*));

  NetStateMalloc((void**)&thrs_recv_cgp_start, n_thrs_built * sizeof(int*));
  NetStateMalloc((void**)&thrs_send_cgp_start, n_thrs_built * sizeof(int*));

  max_thr_n_units = (n_units_built / n_thrs_built) + 2;

  for(int i=0; i<n_thrs_built; i++) {
    NetStateMalloc((void**)&thrs_unit_idxs[i], max_thr_n_units * sizeof(int));
    NetStateMalloc((void**)&thrs_units_mem[i], max_thr_n_units * unit_state_size);
    NetStateMalloc((void**)&thrs_lay_unit_idxs[i], 2 * n_layers_built * sizeof(int));
    NetStateMalloc((void**)&thrs_ungp_unit_idxs[i], 2 * n_ungps_built * sizeof(int));
    NetStateMalloc((void**)&thrs_lay_stats[i], n_lay_stats * n_layers_built * n_lay_stats_vars * sizeof(float));

    NetStateMalloc((void**)&thrs_units_n_recv_cgps[i], max_thr_n_units * sizeof(int));
    NetStateMalloc((void**)&thrs_units_n_send_cgps[i], max_thr_n_units * sizeof(int));

    NetStateMalloc((void**)&thrs_recv_cgp_start[i], max_thr_n_units * sizeof(int));
    NetStateMalloc((void**)&thrs_send_cgp_start[i], max_thr_n_units * sizeof(int));
  }

  n_recv_cgps = 0;
  n_send_cgps = 0;

  for(int i=0; i<n_thrs_built; i++) {
    thrs_n_units[i] = 0;
    thrs_n_recv_cgps[i] = 0;
    thrs_n_send_cgps[i] = 0;
  }
}

void NETWORK_STATE::InitUnitIdxs() {
  n_recv_cgps = 0;
  n_send_cgps = 0;
  for(int i=1; i< n_units_built; i++) {
    int thr_no = (i-1) % n_thrs_built; // just increment sequentialy 0,1,2..n_threads-1
    units_thrs[i] = thr_no;
    int thr_un_idx = thrs_n_units[thr_no]++;
    units_thr_un_idxs[i] = thr_un_idx;
    LAYER_STATE* lay = LayerStateForUn(i);
    units_n_recv_cgps[i] = lay->n_recv_prjns;
    units_n_send_cgps[i] = lay->n_send_prjns;
    n_recv_cgps += lay->n_recv_prjns;
    n_send_cgps += lay->n_send_prjns;
  }
}

void NETWORK_STATE::InitUnitState_Thr(int thr_no) {
  for(int i=0; i< 2*n_layers_built; i++) {
    thrs_lay_unit_idxs[thr_no][i] = -1;
  }
  for(int i=0; i< 2*n_ungps_built; i++) {
    thrs_ungp_unit_idxs[thr_no][i] = -1;
  }
  for(int i=0; i< n_lay_stats*n_layers_built; i++) {
    thrs_lay_stats[thr_no][i] = 0.0f;
  }

  int thr_un_idx = 0;
  int prv_lay_ungp_idx = -1;
  int prv_sub_ungp_idx = -1;
  int prv_lay_idx = -1;
  for(int ui=1; ui < n_units_built; ui++) {
    int th = units_thrs[ui];
    if(th != thr_no) continue;

    int lay_idx = units_lays[ui];
    int ungp_idx = units_ungps[ui];
    LAYER_STATE* lay = GetLayerState(lay_idx);
    UNGP_STATE* ugst = GetUnGpState(ungp_idx);
    
    if(lay_idx != prv_lay_idx) {        // new layer -- update 
      thrs_lay_unit_idxs[thr_no][lay_idx * 2] = thr_un_idx; // start
      if(prv_lay_idx >= 0) {
        thrs_lay_unit_idxs[thr_no][prv_lay_idx * 2 + 1] = thr_un_idx; // end of prev
      }
      if(prv_lay_ungp_idx >= 0) {
        thrs_ungp_unit_idxs[thr_no][prv_lay_ungp_idx * 2 + 1] = thr_un_idx; // end of prev
      }
      if(prv_sub_ungp_idx >= 0) {
        thrs_ungp_unit_idxs[thr_no][prv_sub_ungp_idx * 2 + 1] = thr_un_idx; // end of prev
      }
      prv_lay_idx = lay_idx;
      ungp_idx = lay->ungp_idx;                               // new layer ungp idx
      prv_lay_ungp_idx = ungp_idx;
      thrs_ungp_unit_idxs[thr_no][ungp_idx * 2] = thr_un_idx; // start
      prv_sub_ungp_idx = -1;
    }

    if(lay->n_ungps > 0) {      // sub unit groups
      if(ungp_idx != prv_sub_ungp_idx) {
        thrs_ungp_unit_idxs[thr_no][ungp_idx * 2] = thr_un_idx; // start
        if(prv_sub_ungp_idx >= 0) {
          thrs_ungp_unit_idxs[thr_no][prv_sub_ungp_idx * 2 + 1] = thr_un_idx; // end of prev
        }
        prv_sub_ungp_idx = ungp_idx;
      }
    }

    thrs_unit_idxs[thr_no][thr_un_idx] = ui;
    thrs_units_n_recv_cgps[thr_no][thr_un_idx] = lay->n_recv_prjns;
    thrs_units_n_send_cgps[thr_no][thr_un_idx] = lay->n_send_prjns;

    thrs_recv_cgp_start[thr_no][thr_un_idx] = thrs_n_recv_cgps[thr_no];
    thrs_send_cgp_start[thr_no][thr_un_idx] = thrs_n_send_cgps[thr_no];

    thrs_n_recv_cgps[thr_no] += lay->n_recv_prjns;
    thrs_n_send_cgps[thr_no] += lay->n_send_prjns;

    // initialize unitstate
    UNIT_SPEC_CPP* us = lay->GetUnitSpec(this);
    UNIT_STATE* uv = GetUnitState(ui);
    int lay_un_idx = ui - lay->units_flat_idx;
    int ungp_un_idx = ui - ugst->units_flat_idx;
    int gp_idx = ugst->layer_gp_idx;
    
    uv->Initialize_core(ui, lay_un_idx, gp_idx, ungp_un_idx, thr_no, thr_un_idx,
                        lay->layer_idx, ugst->ungp_idx, us->spec_idx);
    if(us) {
      us->Init_UnitState(uv, this, thr_no);  // initialze -- causes this thread to own mem
    }
    
    thr_un_idx++;
  }
  if(prv_lay_idx >= 0) {
    thrs_lay_unit_idxs[thr_no][prv_lay_idx * 2 + 1] = thr_un_idx; // end of prev
  }
  if(prv_lay_ungp_idx >= 0) {
    thrs_ungp_unit_idxs[thr_no][prv_lay_ungp_idx * 2 + 1] = thr_un_idx; // end of prev
  }
  if(prv_sub_ungp_idx >= 0) {
    thrs_ungp_unit_idxs[thr_no][prv_sub_ungp_idx * 2 + 1] = thr_un_idx; // end of prev
  }
}

void NETWORK_STATE::LayoutUnits() {
  for(int li=0; li < n_layers_built; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    lay->LayoutUnits(this);
  }
}

void NETWORK_STATE::AllocConStateMem() {
  for(int i=0; i<n_thrs_built; i++) {
    NetStateMalloc((void**)&thrs_recv_cgp_mem[i], thrs_n_recv_cgps[i] *
                   con_state_size);
    NetStateMalloc((void**)&thrs_send_cgp_mem[i], thrs_n_send_cgps[i] *
                   con_state_size);
  }
}
  
void NETWORK_STATE::InitConState_Thr(int thr_no) {
  // note: cannot just go over ThrsNRecvConGps(thr_no); b/c ConState_cpp doesn't have units etc yet

  int rcg_flags = ConState_cpp::RECV_CONS | ConState_cpp::IS_ACTIVE;
  int scg_flags = ConState_cpp::CG_0 | ConState_cpp::IS_ACTIVE;
  if(RecvOwnsCons()) {
    rcg_flags |= ConState_cpp::OWN_CONS;
  }
  else {
    scg_flags |= ConState_cpp::OWN_CONS;
  }

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i < nu; i++) {
    int flt_idx = ThrUnitIdx(thr_no, i);
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    LAYER_STATE* lay = uv->GetOwnLayer(this);

    for(int j=0; j < lay->n_recv_prjns; j++) {
      PRJN_STATE* prjn = lay->GetRecvPrjnState(this, j);
      CON_STATE* rcg = ThrUnRecvConState(thr_no, i, j);
      rcg->Initialize_core
        (flt_idx, i, prjn->con_spec_idx, rcg_flags, prjn->prjn_idx, prjn->n_con_vars, prjn->send_idx); 
    }
    
    for(int j=0; j < lay->n_send_prjns; j++) {
      PRJN_STATE* prjn = lay->GetSendPrjnState(this, j);
      CON_STATE* scg = ThrUnSendConState(thr_no, i, j);
      scg->Initialize_core
        (flt_idx, i, prjn->con_spec_idx, scg_flags, prjn->prjn_idx, prjn->n_con_vars, prjn->recv_idx);
    }
  }
}


void NETWORK_STATE::AllocSendNetinTmpState() {
  // temporary storage for sender-based netinput computation
  if(n_units_built == 0 || n_thrs_built == 0) return;

  NetStateMalloc((void**)&thrs_send_netin_tmp, n_thrs_built * sizeof(float*));

  for(int i=0; i<n_thrs_built; i++) {
    if(NetinPerPrjn()) {
      NetStateMalloc((void**)&thrs_send_netin_tmp[i],
                     n_units_built * max_prjns * sizeof(float));
    }
    else {
      NetStateMalloc((void**)&thrs_send_netin_tmp[i],
                     n_units_built * sizeof(float));
    }
  }
}

void NETWORK_STATE::InitSendNetinTmp_Thr(int thr_no) {
  if(NetinPerPrjn()) {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * max_prjns * sizeof(float));
  }
  else {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * sizeof(float));
  }
}

void NETWORK_STATE::Connect_Sizes() {
  // go in reverse order so that symmetric prjns can be made in
  // response to receiver-based projections
  for(int li=n_layers_built-1; li >= 0; li--) {
    LAYER_STATE* lay = GetLayerState(li);
    lay->Connect_Sizes(this);
  }
}

void NETWORK_STATE::AllocConsCountStateMem() {
  NetStateMalloc((void**)&thrs_recv_cons_cnt, n_thrs_built * sizeof(int64_t));
  NetStateMalloc((void**)&thrs_send_cons_cnt, n_thrs_built * sizeof(int64_t));

  NetStateMalloc((void**)&thrs_recv_cons_mem, n_thrs_built * sizeof(float*));
  NetStateMalloc((void**)&thrs_send_cons_mem, n_thrs_built * sizeof(float*));

  NetStateMalloc((void**)&thrs_own_cons_max_size, n_thrs_built * sizeof(int));
  NetStateMalloc((void**)&thrs_own_cons_tot_size, n_thrs_built * sizeof(int64_t));
  NetStateMalloc((void**)&thrs_own_cons_tot_size_nonshared, n_thrs_built * sizeof(int64_t));
  NetStateMalloc((void**)&thrs_own_cons_avg_size, n_thrs_built * sizeof(int));
  NetStateMalloc((void**)&thrs_own_cons_max_vars, n_thrs_built * sizeof(int));
  NetStateMalloc((void**)&thrs_pct_cons_vec_chunked, n_thrs_built * sizeof(float));

  NetStateMalloc((void**)&thrs_tmp_chunks, n_thrs_built * sizeof(int*));
  NetStateMalloc((void**)&thrs_tmp_not_chunks, n_thrs_built * sizeof(int*));
  NetStateMalloc((void**)&thrs_tmp_con_mem, n_thrs_built * sizeof(float*));
}


void NETWORK_STATE::Connect_AllocSizes_Thr(int thr_no) {
  thrs_recv_cons_cnt[thr_no] = 0;
  thrs_send_cons_cnt[thr_no] = 0;
  thrs_own_cons_max_size[thr_no] = 0;
  thrs_own_cons_max_vars[thr_no] = 0;
  int64_t ocsum = 0;
  int ocn = 0;

  // recv cons
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i); // guaranteed to be active..
    if(rcg->OwnCons()) {
      thrs_recv_cons_cnt[thr_no] += rcg->OwnMemReq();
      thrs_own_cons_max_size[thr_no] = MAX(thrs_own_cons_max_size[thr_no],
                                           rcg->alloc_size);
      thrs_own_cons_max_vars[thr_no] = MAX(thrs_own_cons_max_vars[thr_no],
                                           rcg->NConVars());
      ocsum += rcg->alloc_size;
      ocn++;
    }
    else {
      thrs_recv_cons_cnt[thr_no] += rcg->PtrMemReq();
    }
  }

  // send cons
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    CON_STATE* scg = ThrSendConState(thr_no, i); // guaranteed to be active..
    if(scg->OwnCons()) {
      thrs_send_cons_cnt[thr_no] += scg->OwnMemReq();
      thrs_own_cons_max_size[thr_no] = MAX(thrs_own_cons_max_size[thr_no],
                                           scg->alloc_size);
      thrs_own_cons_max_vars[thr_no] = MAX(thrs_own_cons_max_vars[thr_no],
                                           scg->NConVars());
      ocsum += scg->alloc_size;
      ocn++;
    }
    else {
      thrs_send_cons_cnt[thr_no] += scg->PtrMemReq();
    }
  }

  thrs_own_cons_tot_size[thr_no] = ocsum;
  thrs_own_cons_tot_size_nonshared[thr_no] = ocsum; // assume all nonshared for now..
  // see CountNonSharedRecvCons_Thr later..
  if(ocn > 0) {
    thrs_own_cons_avg_size[thr_no] = round((float)ocsum / (float)ocn);
  }
}

void NETWORK_STATE::Connect_Alloc_Thr(int thr_no) {
  // then dole it out to the units..
  int64_t thrs_recv_cons_idx = 0;
  int64_t thrs_send_cons_idx = 0;

  // recv cons
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i); // guaranteed to be active..
    rcg->SetMemStart(this, thrs_recv_cons_idx);
    thrs_recv_cons_idx += rcg->MemReq();
  }

  // send cons
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    CON_STATE* scg = ThrSendConState(thr_no, i); // guaranteed to be active..
    scg->SetMemStart(this, thrs_send_cons_idx);
    thrs_send_cons_idx += scg->MemReq();
  }
}

void NETWORK_STATE::CacheMemStart_Thr(int thr_no) {
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i);
    rcg->CacheMemStart(this, thr_no);
  }
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    CON_STATE* scg = ThrSendConState(thr_no, i);
    scg->CacheMemStart(this, thr_no);
  }
}

void NETWORK_STATE::Connect_Cons() {
  // go in reverse order so that symmetric prjns can be made in
  // response to receiver-based projections
  for(int li=n_layers_built-1; li >= 0; li--) {
    LAYER_STATE* lay = GetLayerState(li);
    lay->Connect_Cons(this);
  }
}

void NETWORK_STATE::Connect_VecChunk_Thr(int thr_no) {
  float pct_chunked = 0.0f;
  int   ncg = 0;
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i); // guaranteed to be active..
      rcg->VecChunk_RecvOwns
        (this, thrs_tmp_chunks[thr_no], thrs_tmp_not_chunks[thr_no],
         thrs_tmp_con_mem[thr_no]);
      pct_chunked += rcg->VecChunkPct();
      ncg++;
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i); // guaranteed to be active..
      scg->VecChunk_SendOwns
        (this, thrs_tmp_chunks[thr_no], thrs_tmp_not_chunks[thr_no],
         thrs_tmp_con_mem[thr_no]);
      pct_chunked += scg->VecChunkPct();
      ncg++;
    }
  }

  if(ncg > 0) {
    thrs_pct_cons_vec_chunked[thr_no] = (pct_chunked / (float)ncg);
  }
  else {
    thrs_pct_cons_vec_chunked[thr_no] = 0.0f;
  }    
}

void NETWORK_STATE::AllocConnectionMem() {
#ifdef DMEM_COMPILE
  all_dmem_sum_dwts_size = 0;
#endif
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    if(thrs_recv_cons_cnt[thr_no] > 0) {
      NetStateMalloc((void**)&thrs_recv_cons_mem[thr_no],
                     thrs_recv_cons_cnt[thr_no] * sizeof(float));
    }
    else {
      thrs_recv_cons_mem[thr_no] = 0;
    }
    if(thrs_send_cons_cnt[thr_no] > 0) {
      NetStateMalloc((void**)&thrs_send_cons_mem[thr_no],
                     thrs_send_cons_cnt[thr_no] * sizeof(float));
    }
    else {
      thrs_send_cons_mem[thr_no] = 0;
    }
    if(thrs_own_cons_max_size[thr_no] > 0) {
      NetStateMalloc((void**)&thrs_tmp_chunks[thr_no],
                     thrs_own_cons_max_size[thr_no] * sizeof(int));
      NetStateMalloc((void**)&thrs_tmp_not_chunks[thr_no],
                     thrs_own_cons_max_size[thr_no] * sizeof(int));
      NetStateMalloc((void**)&thrs_tmp_con_mem[thr_no],
                     thrs_own_cons_max_size[thr_no] * (thrs_own_cons_max_vars[thr_no] + 1)
                     * sizeof(float));
    }
    else {
      thrs_tmp_chunks[thr_no] = 0;
      thrs_tmp_not_chunks[thr_no] = 0;
      thrs_tmp_con_mem[thr_no] = 0;
    }
#ifdef DMEM_COMPILE
    all_dmem_sum_dwts_size += thrs_own_cons_tot_size[thr_no] + thrs_n_units[thr_no];
#endif
  }

#ifdef DMEM_COMPILE
  NetStateMalloc((void**)&thrs_dmem_sum_dwts_send, n_thrs_built * sizeof(float*));
  NetStateMalloc((void**)&thrs_dmem_sum_dwts_recv, n_thrs_built * sizeof(float*));

  for(int thr_no=0; thr_no<n_thrs_built; thr_no++) {
    int64_t szal = thrs_own_cons_tot_size[thr_no] + thrs_n_units[thr_no];
    if(szal > 0) {
      NetStateMalloc((void**)&thrs_dmem_sum_dwts_send[thr_no],
                     szal * sizeof(float));
      NetStateMalloc((void**)&thrs_dmem_sum_dwts_recv[thr_no],
                     szal * sizeof(float));
    }
    else {
      thrs_dmem_sum_dwts_send[thr_no] = 0;
      thrs_dmem_sum_dwts_recv[thr_no] = 0;
    }
  }
#endif
}

void NETWORK_STATE::Connect_UpdtActives_Thr(int thr_no) {
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i);
    rcg->UpdtIsActive(this);
  }
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    CON_STATE* scg = ThrSendConState(thr_no, i);
    scg->UpdtIsActive(this);
  }
}

void NETWORK_STATE::CountCons() {
  n_units = n_units_built;
  n_cons = 0;
  pct_cons_vec_chunked = 0.0f;
  for(int i=0; i<n_thrs_built; i++) {
    n_cons += thrs_own_cons_tot_size[i];
    pct_cons_vec_chunked += thrs_pct_cons_vec_chunked[i];
  }
  if(n_thrs_built > 0) {
    pct_cons_vec_chunked /= (float)n_thrs_built;
  }
  max_prjns = 1;
  for(int li=0; li < n_layers_built; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    max_prjns = MAX(lay->n_recv_prjns, max_prjns);
  }

  if(RecvOwnsCons()) {
    for(int i=0; i < n_thrs_built; i++) {
      CountNonSharedRecvCons_Thr(i);
    }
  }
  // cannot share any sending connections!
}

void NETWORK_STATE::CountNonSharedRecvCons_Thr(int thr_no) {
  int64_t ocsum_nonshared = 0;
  // recv cons only..
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i); // guaranteed to be active..
    if(!rcg->Sharing()) {
      ocsum_nonshared += rcg->alloc_size;
    }
  }
  thrs_own_cons_tot_size_nonshared[thr_no] = ocsum_nonshared;
}

void NETWORK_STATE::FreeConMem() {
  if(!thrs_recv_cons_mem) return; // nothing allocated yet -- otherwise assume EVERYTHING is

  for(int i=0; i<n_thrs_built; i++) {
    // go in reverse order: cons to units..
    NetStateFree((void**)&thrs_send_netin_tmp[i]);

    NetStateFree((void**)&thrs_tmp_chunks[i]);
    NetStateFree((void**)&thrs_tmp_not_chunks[i]);
    NetStateFree((void**)&thrs_tmp_con_mem[i]);

    NetStateFree((void**)&thrs_recv_cons_mem[i]);
    NetStateFree((void**)&thrs_send_cons_mem[i]);

#ifdef DMEM_COMPILE    
    NetStateFree((void**)&thrs_dmem_sum_dwts_send[i]);
    NetStateFree((void**)&thrs_dmem_sum_dwts_recv[i]);
#endif
  }

  // first all the doubly-allocated by-thread guys from above
  NetStateFree((void**)&thrs_send_netin_tmp);

  NetStateFree((void**)&thrs_tmp_chunks);
  NetStateFree((void**)&thrs_tmp_not_chunks);
  NetStateFree((void**)&thrs_tmp_con_mem);

  NetStateFree((void**)&thrs_recv_cons_mem);
  NetStateFree((void**)&thrs_send_cons_mem);

  // now go back and get the rest
  NetStateFree((void**)&thrs_own_cons_max_size);
  NetStateFree((void**)&thrs_own_cons_tot_size);
  NetStateFree((void**)&thrs_own_cons_tot_size_nonshared);
  NetStateFree((void**)&thrs_own_cons_avg_size);
  NetStateFree((void**)&thrs_own_cons_max_vars);
  NetStateFree((void**)&thrs_pct_cons_vec_chunked);

  NetStateFree((void**)&thrs_recv_cons_cnt);
  NetStateFree((void**)&thrs_send_cons_cnt);

#ifdef DMEM_COMPILE
  all_dmem_sum_dwts_size = 0;
  
  NetStateFree((void**)&thrs_dmem_sum_dwts_send);
  NetStateFree((void**)&thrs_dmem_sum_dwts_recv);
#endif
}


void NETWORK_STATE::FreeStateMem() {
  if(!units_thrs) return; // nothing allocated yet -- otherwise assume EVERYTHING is

  FreeConMem();           // this must go first!

  for(int i=0; i<n_thrs_built; i++) {
    // go in reverse order: con gps to units..
    NetStateFree((void**)&thrs_recv_cgp_start[i]);
    NetStateFree((void**)&thrs_send_cgp_start[i]);

    NetStateFree((void**)&thrs_recv_cgp_mem[i]);
    NetStateFree((void**)&thrs_send_cgp_mem[i]);

    NetStateFree((void**)&thrs_units_n_recv_cgps[i]);
    NetStateFree((void**)&thrs_units_n_send_cgps[i]);

    NetStateFree((void**)&thrs_unit_idxs[i]);
    NetStateFree((void**)&thrs_lay_unit_idxs[i]);
    NetStateFree((void**)&thrs_ungp_unit_idxs[i]);
    NetStateFree((void**)&thrs_lay_stats[i]);
    NetStateFree((void**)&thrs_units_mem[i]);
  }

  // first all the doubly-allocated by-thread guys from above
  NetStateFree((void**)&thrs_recv_cgp_start);
  NetStateFree((void**)&thrs_send_cgp_start);

  NetStateFree((void**)&thrs_recv_cgp_mem);
  NetStateFree((void**)&thrs_send_cgp_mem);

  NetStateFree((void**)&thrs_units_n_recv_cgps);
  NetStateFree((void**)&thrs_units_n_send_cgps);

  NetStateFree((void**)&thrs_unit_idxs);
  NetStateFree((void**)&thrs_lay_unit_idxs);
  NetStateFree((void**)&thrs_ungp_unit_idxs);
  NetStateFree((void**)&thrs_lay_stats);
  NetStateFree((void**)&thrs_units_mem);

  // now go back and get the rest
  NetStateFree((void**)&thrs_n_recv_cgps);
  NetStateFree((void**)&thrs_n_send_cgps);

  NetStateFree((void**)&units_n_recv_cgps);
  NetStateFree((void**)&units_n_send_cgps);

  NetStateFree((void**)&thrs_n_units);
  NetStateFree((void**)&units_thr_un_idxs);
  NetStateFree((void**)&units_lays);
  NetStateFree((void**)&units_ungps);
  NetStateFree((void**)&units_thrs);
    
  NetStateFree((void**)&lay_send_prjns);
  NetStateFree((void**)&ungp_lay_idxs);
  NetStateFree((void**)&layers_mem);
  NetStateFree((void**)&prjns_mem);
  NetStateFree((void**)&ungps_mem);
}


void NETWORK_STATE::Initialize_core()  {
  max_thr_n_units = 0; net_owner = NULL;
  layers_mem = NULL;    prjns_mem = NULL;    ungps_mem = NULL;  ungp_lay_idxs = NULL;
  lay_send_prjns = NULL;
    
  units_lays = NULL;  units_ungps = NULL; units_thrs = NULL; units_thr_un_idxs = NULL;
  thrs_n_units = NULL;
  thrs_unit_idxs = NULL;    thrs_units_mem = NULL;    thrs_lay_unit_idxs = NULL;
  thrs_ungp_unit_idxs = NULL;    thrs_lay_stats = NULL;
  n_lay_stats = N_NetThrLayStats; // derived classes with new stats need to overwrite
  n_lay_stats_vars = 6;

  units_n_recv_cgps = NULL;    units_n_send_cgps = NULL;    n_recv_cgps = 0;    n_send_cgps = 0;

  thrs_units_n_recv_cgps = NULL;    thrs_units_n_send_cgps = NULL;    thrs_n_recv_cgps = NULL;
  thrs_n_send_cgps = NULL;
    
  thrs_recv_cgp_mem = NULL;    thrs_send_cgp_mem = NULL;    thrs_recv_cgp_start = NULL;
  thrs_send_cgp_start = NULL;
    
  thrs_recv_cons_cnt = NULL;    thrs_send_cons_cnt = NULL;    thrs_recv_cons_mem = NULL;
  thrs_send_cons_mem = NULL;

  thrs_own_cons_max_size = NULL;    thrs_own_cons_tot_size = NULL;
  thrs_own_cons_tot_size_nonshared = NULL;    thrs_own_cons_avg_size = NULL;
  thrs_own_cons_max_vars = NULL;    thrs_pct_cons_vec_chunked = NULL;
  pct_cons_vec_chunked = 0.0f;    thrs_tmp_chunks = NULL;
  thrs_tmp_not_chunks = NULL;    thrs_tmp_con_mem =  NULL;

  thrs_send_netin_tmp = NULL;

  dmem_nprocs = 1;
  dmem_proc = 0;
#ifdef DMEM_COMPILE
  all_dmem_sum_dwts_size = 0;
  thrs_dmem_sum_dwts_send = NULL;
  thrs_dmem_sum_dwts_recv = NULL;
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

