// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(ClampDaUnitSpec)::Send_Da(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  const float snd_val = u->act;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      if(da_val == DA_P) {
        send_gp->UnState(j,net)->da_p = snd_val;
      }
      else {
        send_gp->UnState(j,net)->da_n = snd_val;
      }
    }
  }
}


