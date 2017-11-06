// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(BFCSUnitSpec)::Compute_ACh(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float sum_delta = 0.0f;
  int sum_n = 0;
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    LEABRA_LAYER_STATE* fmlay = (LEABRA_LAYER_STATE*)recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)fmlay->GetUnitSpec(net);
    if(cs->IsMarkerCon()) {
      float avg_del = 0.0f;
      if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_CElAmygUnitSpec) {
        for(int j=0;j<recv_gp->size; j++) {
          LEABRA_UNIT_STATE* suv = (LEABRA_UNIT_STATE*)recv_gp->UnState(j,net);
          float del = suv->act_eq - suv->act_q0; // trial level delta
          if(del < 0.0f) del = 0.0f;             // positive rectification!?
          avg_del += fabsf(del);
        }
        avg_del *= ach.cea_gain;
      }
      else {                    // assume vs, within-trial delta
        for(int j=0;j<recv_gp->size; j++) {
          LEABRA_UNIT_STATE* suv = (LEABRA_UNIT_STATE*)recv_gp->UnState(j,net);
          float del = suv->act_eq * suv->da_p; // act * dopamine!
          // if(del < 0.0f) del = 0.0f;             // positive rectification!
          avg_del += fabsf(del);
        }
        avg_del *= ach.vs_gain;
      }
      avg_del /= (float)recv_gp->size;
      sum_delta += avg_del;
      sum_n++;
    }
  }

  float avg_delta = 0.0f;
  if(sum_n > 0) {
    avg_delta = sum_delta / (float)sum_n;
  }

  u->misc_2 = avg_delta;
  u->misc_1 += ach.dt * (avg_delta - u->misc_1); // time integrate
  u->ach = u->misc_1;

  // if(da.rec_data) {
  //   // lay->SetUserData("negpv", negpv, false); // false=no update
  // }
}

void STATE_CLASS(BFCSUnitSpec)::Send_ACh(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float snd_val = u->misc_1;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = (LEABRA_CON_STATE*)u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LEABRA_UNIT_STATE*)send_gp->UnState(j,net))->ach = snd_val;
    }
  }
}

