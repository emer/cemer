// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(ThalSendUnitSpec)::Send_Thal(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  const float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      float& thal = ((LEABRA_UNIT_STATE*)send_gp->UnState(j,net))->thal;
      if(snd_val > thal)        // max..  thal was reset in Send_NetinDelta
        thal = snd_val;
    }
  }
}


