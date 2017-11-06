
#define DYN_EL STATE_CLASS(PFCDynEl)

void STATE_CLASS(PFCUnitSpec)::AllocDyns(int ns) {
  if(alloc_dyns >= ns) return;
  if(dyns_m) {
    FreeDyns();
  }
  dyns_m = new DYN_EL[ns];
  alloc_dyns = ns;
}

void STATE_CLASS(PFCUnitSpec)::FreeDyns() {
  if(dyns_m) {
    delete [] dyns_m;
    dyns_m = NULL;
  }
  alloc_dyns = 0;
  n_dyns = 0;
}

float STATE_CLASS(PFCUnitSpec)::InitDynVal(int row) {
  if(row >= n_dyns) return 1.0f;
  DYN_EL& dyn = dyns_m[row];
  return dyn.init;
}
  
float STATE_CLASS(PFCUnitSpec)::UpdtDynVal(int row, float time_step) {
  if(row >= n_dyns) return 1.0f;
  DYN_EL& dyn = dyns_m[row];
  float val = dyn.init;
  if(dyn.rise_tau > 0 && dyn.decay_tau > 0) {
    if(time_step >= dyn.rise_tau) {
      val = 1.0f - ((time_step - dyn.rise_tau) / dyn.decay_tau);
    }
    else {
      val = dyn.init + (1.0f - dyn.init) * (time_step / dyn.rise_tau);
    }
  }
  else if(dyn.rise_tau > 0) {
    val = dyn.init + (1.0f - dyn.init) * (time_step / dyn.rise_tau);
  }
  else if(dyn.decay_tau > 0) {
    val = dyn.init - dyn.init * (time_step / dyn.decay_tau);
  }
  if(val > 1.0f) val = 1.0f;
  if(val < 0.001f) val = 0.001f; // non-zero indicates gated..
  return val;
}

float STATE_CLASS(PFCUnitSpec)::Compute_NetinExtras
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float& net_syn) {
  
  float net_ex = inherited::Compute_NetinExtras(u, net, thr_no, net_syn);
  if(!deep.IsSuper()) {
    return net_ex;
  }
  if(u->thal_cnt < 0.0f) {      // not even close to maintaining..
    return net_ex;
  }
  
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  LEABRA_UNGP_STATE* ugd = (LEABRA_UNGP_STATE*)u->GetOwnUnGp(net);
  if(u->thal_cnt == 0.0f) {     // just gated -- only maint if nothing else
    if(ugd->netin_raw.max < 0.05f) { //
      net_ex += maint.s_mnt_max * u->deep_mod_net;
    }
  }
  else {                        // maintaining
    const float eff_netin_max = fminf(ugd->netin_raw.max, maint.mnt_net_max);
    const float netin_factor = 1.0f - (eff_netin_max / maint.mnt_net_max);
    const float mnt_net = maint.s_mnt_min +
      netin_factor * (maint.s_mnt_max - maint.s_mnt_min);
    net_ex += mnt_net * u->deep_mod_net;
  }
  return net_ex;
}

void STATE_CLASS(PFCUnitSpec)::Compute_PFCGating
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  if(!deep.IsSuper()) {
    STATE_CLASS(LeabraUnitSpec)::SaveGatingAct(u, net, thr_no); // we save like regular -- don't get gating inputs
    return;
  }

  if(u->thal_gate == 0.0f) {    // no gating!
    return;
  }
  // note: gating actually happend on prior cycle -- we were sent thal values in Act_Post on the prior cycle

  if(gate.out_gate && gate.out_q1only) {
    if(net->quarter > 1) return;
  }
  
  u->act_g = GetRecAct(u);      // save activation at point of gating
  
  if(u->thal > gate.gate_thr) { // new gating signal -- reset counter
    if(u->thal_cnt >= 1.0f) { // already maintaining
      if(maint.clear > 0.0f) {
        DecayState(u, net, thr_no, maint.clear);
      }
    }
    u->thal_cnt = 0.0f;        // this is the "just gated" signal
    if(gate.out_gate) {         // time to clear out maint
      ClearOtherMaint(u, net, thr_no); 
    }
  }

  // test for over-duration maintenance -- allow for active gating to override
  if(u->thal_cnt >= maint.max_mnt) {
    u->thal_cnt = -1.0f;
  }
}

void STATE_CLASS(PFCUnitSpec)::Compute_DeepRaw(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;

  // NOTE: only super does anything here -- this is where the gating is detected and updated
  if(!deep.IsSuper()) return;
  
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);

  float thr_cmp = lgpd->acts_raw.avg +
    deep.raw_thr_rel * (lgpd->acts_raw.max - lgpd->acts_raw.avg);
  thr_cmp = fmaxf(thr_cmp, deep.raw_thr_abs);
  float draw = 0.0f;
  if(u->act_raw >= thr_cmp) {
    draw = u->act_raw;
  }

  float thal_eff = 0.0f;
  if(u->thal_cnt >= 0.0f) {     // gated or maintaining
    thal_eff = fmaxf(u->thal, gate.mnt_thal);
  }
  
  u->deep_raw = thal_eff * draw;
}

void STATE_CLASS(PFCUnitSpec)::GetThalCntFromSuper
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  // look for layer we recv a deep context con from, that is also a PFCUnitSpec SUPER
  const int nrg = u->NRecvConGps(net); 
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->IsDeepCtxtCon()) continue;
    LEABRA_LAYER_STATE* fmlay = (LEABRA_LAYER_STATE*)recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* fmus = (LEABRA_UNIT_SPEC_CPP*)fmlay->GetUnitSpec(net);
    if(fmus->GetStateSpecType() != LEABRA_NETWORK_STATE::T_PFCUnitSpec) continue;
    if(!fmus->deep.IsSuper() || recv_gp->size == 0) continue;
    LEABRA_UNIT_STATE* suv = (LEABRA_UNIT_STATE*)recv_gp->UnState(0,net); // get first connection
    u->thal_cnt = suv->thal_cnt; // all super guys in same stripe should have same thal_cnt
  }
}  


void STATE_CLASS(PFCUnitSpec)::Send_DeepCtxtNetin
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  if(u->thal_cnt < 0.0f) {      // not maintaining or just gated -- zero!
    u->deep_raw = 0.0f;
    u->deep_ctxt = 0.0f;
    u->thal_cnt -= 1.0f;        // decrement count -- optional
  }
  else {
    u->thal_cnt += 1.0f;          // we are maintaining, update count for all
  }

  if(deep.IsSuper()) {
    if(u->thal_cnt < 0.0f) return; // optimization: don't send if not maintaining!
  }

  inherited::Send_DeepCtxtNetin(u, net, thr_no);
}


void STATE_CLASS(PFCUnitSpec)::Compute_DeepStateUpdt
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  if(maint.use_dyn && deep.IsDeep() && u->thal_cnt >= 0) { // update dynamics!
    LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
    int unidx = u->ungp_un_idx;
    int dyn_row = unidx % n_dyns;
    if(u->thal_cnt <= 1.0f) { // first gating -- should only ever be 1.0 here..
      u->misc_1 = u->deep_ctxt; // record gating ctxt
      u->deep_ctxt *= InitDynVal(dyn_row);
    }
    else {
      u->deep_ctxt = u->misc_1 * UpdtDynVal(dyn_row, (u->thal_cnt-1.0f));
    }
  }

  inherited::Compute_DeepStateUpdt(u, net, thr_no);
}

void STATE_CLASS(PFCUnitSpec)::ClearOtherMaint
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  LEABRA_UNGP_STATE* ugd = (LEABRA_UNGP_STATE*)u->GetOwnUnGp(net);
  if(ugd->acts_eq.max < 0.1f)   // we can't clear anyone if nobody in our group is active!
    return;
  
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)send_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    for(int j=0;j<send_gp->size; j++) {
      LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)send_gp->UnState(j,net);
      if(su->thal_cnt >= 1.0f) { // important!  only for established maint, not just gated!
        STATE_CLASS(PFCUnitSpec)* mus = (STATE_CLASS(PFCUnitSpec)*)su->GetUnitSpec(net);
        su->thal_cnt = -1.0f; // terminate!
        if(mus->maint.clear > 0.0f) {
          DecayState(su, net, thr_no, mus->maint.clear); // note: thr_no is WRONG here! but shouldn't matter..
        }
      }
    }
  }
}

