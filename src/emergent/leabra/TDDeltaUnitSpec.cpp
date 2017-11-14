// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(TDDeltaUnitSpec)::Compute_TD
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(!Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
    u->da_p = 0.0f;
    u->ext = u->da_p;
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    return;
  }
  float rew_integ_minus = 0.0f;
  float rew_integ_cur = 0.0f;
  const int nrg = u->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* slay = recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us = slay->GetUnitSpec(net);
    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_TDRewIntegUnitSpec) {
      LEABRA_UNIT_STATE* su = slay->GetUnitState(net, 0);
      rew_integ_minus = su->act_m;
      rew_integ_cur = su->act_eq;
      break;
    }
  }
  float delta = rew_integ_cur - rew_integ_minus;
  u->da_p = delta;
  u->ext = u->da_p;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
}

void STATE_CLASS(TDDeltaUnitSpec)::Send_TD(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      send_gp->UnState(j,net)->da_p = snd_val;
    }
  }
}

