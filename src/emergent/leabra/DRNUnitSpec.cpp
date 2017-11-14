// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {


void STATE_CLASS(DRNUnitSpec)::Compute_Se(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float pospv = 0.0f;
  int   pospv_n  = 0;
  float negpv = 0.0f;
  int   negpv_n  = 0;
  float posstate = 0.0f;
  int   posstate_n = 0;
  float negstate = 0.0f;
  int   negstate_n = 0;

  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_LAYER_STATE* from = recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us = from->GetUnitSpec(net);
    LEABRA_UNGP_STATE* lgpd = from->GetLayUnGpState(net);

    const float act_avg = lgpd->acts_eq.avg;
    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_VTAUnitSpec) {
      continue;                 // skip -- just read from dav
    }
    else {
      if(from->LayerNameContains("Pos")) {
        if(from->LayerNameContains("State")) {
          posstate += act_avg;
          posstate_n++;
        }
        else {                    // PV
          pospv += act_avg;
          pospv_n++;
        }
      }
      else {
        if(from->LayerNameContains("State")) {
          negstate += act_avg;
          negstate_n++;
        }
        else {                    // PV
          negpv += act_avg;
          negpv_n++;
        }
      }
    }
  }

  if(pospv_n > 0) pospv /= (float)pospv_n;
  if(negpv_n > 0) negpv /= (float)negpv_n;
  if(posstate_n > 0) posstate /= (float)posstate_n;
  if(negstate_n > 0) negstate /= (float)negstate_n;

  if(se.sub_pos) {
    negpv -= pospv;
    negstate -= posstate;
  }

  float incd = se.se_inc_dt * (1.0f - u->sev);
  float dapd = 0.0f;
  if(u->da_p > 0.0f) {
    dapd = se.da_pos_dt * u->da_p * (se.se_base - u->sev);
  }
  float dand = 0.0f;
  if(u->da_p < 0.0f) {
    dand = se.da_neg_dt * -u->da_p * (1.0f - u->sev);
  }
  float negpvd = se.se_pv_dt * (negpv - u->sev);
  float negstated = se.se_state_dt * (negstate - u->sev);

  u->sev += incd + dapd + dand + negpvd + negstated;

  if(u->sev < se.se_base) u->sev = se.se_base;
  if(u->sev > 1.0f) u->sev = 1.0f;

  u->ext = u->sev;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

void STATE_CLASS(DRNUnitSpec)::Send_Se(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  const float snd_val = se.se_out_gain * u->sev;
  const int nsg = u->NSendConGps(net); 
  for(int g=0; g<nsg; g++) {
    LEABRA_CON_STATE* send_gp = u->SendConState(net, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      send_gp->UnState(j,net)->sev = snd_val;
    }
  }
}

