// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(TANUnitSpec)::Compute_PlusPhase_Netin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float max_send_act = 0.0f;
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    const int sz = recv_gp->size;
    for(int i=0; i< sz; i++) {
      // receiving from act_eq must happen outside of Compute_Act stage!
      const float act_eq = recv_gp->UnState(i,net)->act_eq;
      max_send_act = fmaxf(act_eq, max_send_act);
    }
  }
  u->ext = max_send_act;
  u->net = u->ext;
}

void STATE_CLASS(TANUnitSpec)::Send_ACh(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  const float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      send_gp->UnState(j,net)->ach = snd_val;
    }
  }
}


