// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(PatchUnitSpec)::Send_DAShunt(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float snd_val = u->act_eq;
  if(u->act_eq < opt_thresh.send) {
    snd_val = 0.0f;
  }
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LEABRA_UNIT_STATE*)send_gp->UnState(j,net))->shunt = snd_val;
    }
  }
}

