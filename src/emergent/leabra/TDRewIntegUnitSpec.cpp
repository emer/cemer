// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(TDRewIntegUnitSpec)::Compute_TDRewInteg
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float rew_pred_val = 0.0f;
  float ext_rew_val = 0.0f;

  const int nrg = u->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* slay = (LEABRA_LAYER_STATE*)recv_gp->GetSendLayer(net);
    LEABRA_LAYER_SPEC_CPP* sls = (LEABRA_LAYER_SPEC_CPP*)slay->GetLayerSpec(net);
    LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)slay->GetUnitSpec(net);
    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_TDRewPredUnitSpec) {
      rew_pred_val = slay->GetTotalActEq(net);
    }
    else if(sls->GetStateSpecType() == LEABRA_NETWORK_STATE::T_ExtRewLayerSpec) {
      LEABRA_UNIT_STATE* eru = (LEABRA_UNIT_STATE*)slay->GetUnitState(net, 0);
      ext_rew_val = eru->misc_1; // readout is in misc_1
    }
  }

  float new_val;
  if(!Quarter_DeepRawNow(net->quarter)) { // plus phase marker..
    new_val = rew_pred_val; // no discount in minus phase!!!  should only reflect previous V^(t)
  }
  else {
    if(rew_integ.max_r_v) {
      new_val = fmaxf(rew_integ.discount * rew_pred_val, ext_rew_val);
    }
    else {
      new_val = rew_integ.discount * rew_pred_val + ext_rew_val; // now discount new rewpred!
    }
  }

  u->ext = new_val;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

