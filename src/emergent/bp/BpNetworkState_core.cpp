// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in BpNetworkState.cpp, _cpp.cpp, _cuda.cpp

void BP_NETWORK_STATE::Compute_NetinAct_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    bool has_softmax = false;
    for(int ui = ust; ui < ued; ui++) {
      UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      BP_UNIT_SPEC* us = (BP_UNIT_SPEC*)uv->GetUnitSpec(this);
      if(us->act_fun == BP_UNIT_SPEC::SOFTMAX) {
        if(!has_softmax) has_softmax = true;
        us->Compute_Netin(uv, this, thr_no);
      }
      else {
        us->Compute_NetinAct(uv, this, thr_no);
      }
    }
    ThreadSyncSpin(thr_no, li % 3);   // need to sync for each layer!

    if(has_softmax) {           //
      if(thr_no == 0) {         // main thread has to do everything
        const int ustl = lay->units_flat_idx;
        const int uedl = lay->units_flat_idx + lay->n_units;
        float sum = 0.0f;
        for(int ui = ustl; ui < uedl; ui++) {
          BP_UNIT_STATE* uv = (BP_UNIT_STATE*)ThrUnitState(thr_no, ui);
          if(uv->lesioned()) continue;
          sum += uv->misc1;
        }
        for(int ui = ustl; ui < uedl; ui++) {
          BP_UNIT_STATE* uv = (BP_UNIT_STATE*)ThrUnitState(thr_no, ui);
          if(uv->lesioned()) continue;
          BP_UNIT_SPEC* us = (BP_UNIT_SPEC*)uv->GetUnitSpec(this);
          uv->err = sum;        // temp store
          us->Compute_Act(uv, this, thr_no);
        }
      }
      ThreadSyncSpin(thr_no, (li-1) % 3);   // sync again!
    }
  }
}

void BP_NETWORK_STATE::Compute_dEdA_dEdNet_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = nlay-1; li >= 0; li--) { // go in reverse order!
    LAYER_STATE* lay = GetLayerState(li);
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      BP_UNIT_STATE* uv = (BP_UNIT_STATE*)ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      ((BP_UNIT_SPEC*)uv->GetUnitSpec(this))->Compute_dEdA_dEdNet(uv, this, thr_no);
    }
    ThreadSyncSpin(thr_no, li % 3);   // need to sync for each layer!
  }
}

void BP_NETWORK_STATE::Compute_Error_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    if(lay->lesioned() || !(lay->ext_flag & UNIT_STATE::TARG))
      continue;
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      BP_UNIT_STATE* uv = (BP_UNIT_STATE*)ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      BP_UNIT_SPEC* us = (BP_UNIT_SPEC*)uv->GetUnitSpec(this);
      uv->dEdA = 0.0f;           // must reset -- error is incremental!
      us->Compute_Error(uv, this, 0);
    }
  }
}

void BP_NETWORK_STATE::Compute_dWt_Thr(int thr_no) {
  // if(threads.get_timing)
  //   net_timing[thr_no]->dwt.StartTimer(true); // reset

  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i);
    if(rcg->NotActive()) continue;
    rcg->GetConSpec(this)->Compute_dWt(rcg, this, thr_no);
  }
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    uv->GetUnitSpec(this)->Compute_dWt(uv, this, thr_no);
  }
  
  // if(threads.get_timing)
  //   net_timing[thr_no]->dwt.EndIncrAvg();
}

void BP_NETWORK_STATE::Compute_Weights_Thr(int thr_no) {
  // if(threads.get_timing)
  //   net_timing[thr_no]->wt.StartTimer(true); // reset

  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    CON_STATE* rcg = ThrRecvConState(thr_no, i);
    if(rcg->NotActive()) continue;
    rcg->GetConSpec(this)->Compute_Weights(rcg, this, thr_no);
  }
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    uv->GetUnitSpec(this)->Compute_Weights(uv, this, thr_no);
  }
  
  // if(threads.get_timing)
  //   net_timing[thr_no]->wt.EndIncrAvg();
}

void BP_NETWORK_STATE::Trial_Run_Thr(int thr_no) {
  Compute_NetinAct_Thr(thr_no);

  ThreadSyncSpin(thr_no, 0);
  
  if(train_mode == TRAIN) {
    Compute_dEdA_dEdNet_Thr(thr_no);
    ThreadSyncSpin(thr_no, 1);
    Compute_dWt_Thr(thr_no);
  }
  else {
    Compute_Error_Thr(thr_no);  // for display, stats purposes only..
  }
  // note: no need to sync here b/c end of thread run always syncs
}

