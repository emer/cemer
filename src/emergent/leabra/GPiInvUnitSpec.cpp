// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(GPiInvUnitSpec)::Compute_NetinRaw(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  int nt = net->n_thrs_built;
  int flat_idx = u->flat_idx;

  // note: REQUIRES NetinPerPrjn!  Set automatically in CheckConfig
  float go_in = 0.0f;
  float nogo_in = 0.0f;
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_LAYER_STATE* fmlay = (LEABRA_LAYER_STATE*) recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)fmlay->GetUnitSpec(net);

    float g_nw_nt = 0.0f;
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
      g_nw_nt += ndval;
    }

    recv_gp->net_raw += g_nw_nt;

    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_MSNUnitSpec) {
      LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)fmlay->GetUnitState(net, 0);
      if(su->HasUnitFlag(LEABRA_UNIT_STATE::DORSAL)) {
        if(su->HasUnitFlag(LEABRA_UNIT_STATE::D1R)) {
          go_in += recv_gp->net_raw;
        }
        else { // D2R
          nogo_in += recv_gp->net_raw;
        }
      }
      else { // ventral
        if(su->HasUnitFlag(LEABRA_UNIT_STATE::APPETITIVE)) {
          if(su->HasUnitFlag(LEABRA_UNIT_STATE::D1R)) {
            go_in += recv_gp->net_raw;
          }
          else { // D2R
            nogo_in += recv_gp->net_raw;
          }
        }
        else {                  // aversive
          if(su->HasUnitFlag(LEABRA_UNIT_STATE::D2R)) { // FLIPPED!!!
            go_in += recv_gp->net_raw;
          }
          else { // D1R
            nogo_in += recv_gp->net_raw;
          }
        }
      }
    }
    else { // non-MSNUnitSpec inputs.. just go with the name..
      if(fmlay->LayerNameContains("NoGo") || fmlay->LayerNameContains("D2")) {
        nogo_in += recv_gp->net_raw;
      }
      else {
        go_in += recv_gp->net_raw;
      }
    }
  }

  float gpi_net = 0.0f;
  gpi_net = gpi.tot_gain * (go_in - gpi.nogo * nogo_in);
  gpi_net = fmaxf(gpi_net, 0.0f);
  u->net_raw = gpi_net;
}

void STATE_CLASS(GPiInvUnitSpec)::Send_Thal(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  bool gate_now = Quarter_GateNow(net->quarter);
  int qtr_cyc = net->QuarterCycle();

  float snd_val = 0.0f;
  float gate_val = 0.0f;
  if(net->quarter == 0 && qtr_cyc <= 1) { // reset
    u->thal_cnt = 0.0f;
  }

  if(gate_now && qtr_cyc == gate.gate_cyc) {
    gate_val = 1.0f;            // gated!
    if(gate.updt_net) {
      if(u->lay_un_idx == 0) { // choose only first unit to do this -- not based on threads b/c we don't know for sure that thread 0 will be represented here!
        net->ThalGatedNow();   // record 
      }
    }
    if(gpi.thr_act) {
      if(u->act_eq <= gpi.gate_thr) u->act_eq = 0.0f;
      snd_val = u->act_eq;
    }
    else {
      snd_val = (u->act_eq > gpi.gate_thr ? u->act_eq : 0.0f);
    }

    if(snd_val > 0.0f && gpi.min_thal > gpi.gate_thr) {
      if(gpi.min_thal == 1.0f) {
        snd_val = 1.0f;
      }
      else {
        snd_val = gpi.min_thal + (snd_val - gpi.gate_thr) * gpi.thal_rescale;
      }
    }
    u->thal_cnt = snd_val;      // save gating value!
  }
  else {
    snd_val = u->thal_cnt;
  }
  
  u->thal = snd_val;            // record what we send, always
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)send_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LEABRA_UNIT_STATE*)send_gp->UnState(j,net))->thal = snd_val;
      ((LEABRA_UNIT_STATE*)send_gp->UnState(j,net))->thal_gate = gate_val;
    }
  }
}


