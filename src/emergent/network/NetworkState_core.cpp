// this is the .cpp implementation companion to NetworkState_core.h -- it is included in _cpp and
// _cuda files to implement code that cannot otherwise be inlined in NetworkState_core, which cannot
// for example include any other State files because it is a key dependency for all of them

void NETWORK_STATE::AllocSpecMem() {
  NetStateMalloc((void**)&layer_specs, n_layer_specs_built * sizeof(void*));
  NetStateMalloc((void**)&unit_specs, n_unit_specs_built * sizeof(void*));
  NetStateMalloc((void**)&con_specs, n_con_specs_built * sizeof(void*));
}


LAYER_SPEC* NETWORK_STATE::NewLayerSpec(int spec_type) const {
  return new LAYER_SPEC;
}

UNIT_SPEC* NETWORK_STATE::NewUnitSpec(int spec_type) const {
  return new UNIT_SPEC;
}

CON_SPEC* NETWORK_STATE::NewConSpec(int spec_type) const {
  return new CON_SPEC;
}

void NETWORK_STATE::AllocLayUnitMem() {
  NetStateMalloc((void**)&layers_mem, n_layers_built * layer_state_size);
  NetStateMalloc((void**)&prjns_mem, n_prjns_built * prjn_state_size);
  NetStateMalloc((void**)&ungps_mem, n_ungps_built * ungp_state_size);
  NetStateMalloc((void**)&ungp_lay_idxs, n_ungps_built * sizeof(int));
    
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

void NETWORK_STATE::AllocConGpMem() {
  for(int i=0; i<n_thrs_built; i++) {
    NetStateMalloc((void**)&thrs_recv_cgp_mem[i], thrs_n_recv_cgps[i] *
                   con_state_size);
    NetStateMalloc((void**)&thrs_send_cgp_mem[i], thrs_n_send_cgps[i] *
                   con_state_size);
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


void NETWORK_STATE::AllocConsStateMem() {
#ifdef DMEM_COMPILE
  all_dmem_sum_dwts_size = 0;
#endif
  for(int thr_no=0; thr_no<n_thrs_built; thr_no++) {
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

void NETWORK_STATE::InitSendNetinTmp_Thr(int thr_no) {
  if(NetinPerPrjn()) {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * max_prjns * sizeof(float));
  }
  else {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * sizeof(float));
  }
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
  NetStateFree((void**)&units_thrs);
    
  NetStateFree((void**)&ungp_lay_idxs);
  NetStateFree((void**)&layers_mem);
  NetStateFree((void**)&prjns_mem);
  NetStateFree((void**)&ungps_mem);
}


void NETWORK_STATE::Initialize_core()  {
  max_thr_n_units = 0;
  layers_mem = NULL;    prjns_mem = NULL;    ungps_mem = NULL;  ungp_lay_idxs = NULL;
    
  units_thrs = NULL;    units_thr_un_idxs = NULL;    thrs_n_units = NULL;
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

/////////////////////////////////////////////////////////
//              Main compute algorithm code!

void NETWORK_STATE::Init_InputData_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC* us = uv->GetUnitSpec(this);
    us->Init_InputData(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_InputData_Layer() {
  for(int i=0; i < n_layers_built; i++) {
    LAYER_STATE* lay = GetLayerState(i);
    lay->Init_InputData(this);
  }
}

void NETWORK_STATE::Init_Acts_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC* us = uv->GetUnitSpec(this);
    us->Init_Acts(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_dWt_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      CON_STATE* rcg = ThrRecvConState(thr_no, i);
      if(rcg->NotActive()) continue;
      CON_SPEC* cs = rcg->GetConSpec(this);
      cs->Init_dWt(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      CON_STATE* scg = ThrSendConState(thr_no, i);
      if(scg->NotActive()) continue;
      CON_SPEC* cs = scg->GetConSpec(this);
      cs->Init_dWt(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC* us = uv->GetUnitSpec(this);
    us->Init_dWt(uv, this, thr_no);
  }
}

void NETWORK_STATE::Init_Weights_Layer() {
  for(int i=0; i < n_layers_built; i++) {
    LAYER_STATE* lay = GetLayerState(i);
    lay->Init_Weights_Layer(this);
  }
}

void NETWORK_STATE::Init_Stats_Layer() {
  for(int i=0; i < n_layers_built; i++) {
    LAYER_STATE* lay = GetLayerState(i);
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
    UNIT_SPEC* us = uv->GetUnitSpec(this);
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

void NETWORK_STATE::Compute_Act_Thr(int thr_no) {
  // if(threads.get_timing)
  //   net_timing[thr_no]->act.StartTimer(true); // reset

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UNIT_STATE* uv = ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    UNIT_SPEC* us = uv->GetUnitSpec(this);
    us->Compute_Act(uv, this, thr_no);
  }

  // if(threads.get_timing)
  //   net_timing[thr_no]->act.EndIncrAvg();
}

void NETWORK_STATE::Compute_NetinAct_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    LAYER_STATE* lay = GetLayerState(li);
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      UNIT_STATE* uv = ThrUnitState(thr_no, ui);
      if(uv->lesioned()) continue;
      UNIT_SPEC* us = uv->GetUnitSpec(this);
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
    UNIT_SPEC* us = uv->GetUnitSpec(this);
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
    UNIT_SPEC* us = uv->GetUnitSpec(this);
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
      UNIT_SPEC* us = uv->GetUnitSpec(this);
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
      UNIT_SPEC* us = uv->GetUnitSpec(this);
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
  for(int i=0; i < n_layers_built; i++) {
    LAYER_STATE* lay = GetLayerState(i);
    lay->Compute_EpochStats(this);
  }
}

ConState_cpp* NETWORK_STATE::FindRecipRecvCon(int& con_idx, NetworkState_cpp* net,
                                             UnitState_cpp* su, UnitState_cpp* ru) {
  LayerState_cpp* ru_lay = ru->GetOwnLayer(net);
  LayerState_cpp* su_lay = su->GetOwnLayer(net);
  PrjnState_cpp* prj = su_lay->FindPrjnFrom(net, ru_lay->layer_idx);
  if(!prj) return NULL;
  ConState_cpp* rcg = su->RecvConState(net, prj->recv_idx);
  if(!rcg) return NULL;
  con_idx = rcg->FindConFromIdx(ru->flat_idx);
  if(con_idx >= 0) return rcg;
  return NULL;
}

ConState_cpp* NETWORK_STATE::FindRecipSendCon(int& con_idx, NetworkState_cpp* net,
                                              UnitState_cpp* ru, UnitState_cpp* su) {
  LayerState_cpp* ru_lay = ru->GetOwnLayer(net);
  LayerState_cpp* su_lay = su->GetOwnLayer(net);
  PrjnState_cpp* prj = su_lay->FindPrjnFrom(net, ru_lay->layer_idx);
  if(!prj) return NULL;
  ConState_cpp* scg = ru->SendConState(net, prj->send_idx);
  if(!scg) return NULL;
  con_idx = scg->FindConFromIdx(su->flat_idx);
  if(con_idx >= 0) return scg;
  return NULL;
}  



