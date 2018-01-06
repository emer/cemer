// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

bool STATE_CLASS(LeabraContextUnitSpec)::ShouldUpdateNow
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {

  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  bool do_update = lay->HasLayerFlag(LAYER_STATE::LAY_FLAG_1);
  if(!do_update) {              // check other criteria
    switch (update_criteria) {
    case UC_TRIAL:
      do_update = true;
      break;
    case UC_MANUAL:
      break; // weren't triggered, so that's it
    case UC_N_TRIAL: {
      // do modulo the trial, adding offset -- add 1 so first trial is not trigger
      do_update = (((net->trial + n_spec.n_offs + 1) % n_spec.n_trials) == 0);
    } break;
    }
  }
  return do_update;
}

void STATE_CLASS(LeabraContextUnitSpec)::Compute_Context
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  LEABRA_CON_STATE* cg = u->RecvConState(net, 0);
  LEABRA_UNIT_STATE* su = cg->UnState(0, net);
  LEABRA_LAYER_STATE* fmlay = cg->GetSendLayer(net);
  if(fmlay->lesioned()) {
    u->act = 0.0f;
  }
  else if(net->cycle == 0) {
    bool up = ShouldUpdateNow(u, net, thr_no);
    if(up) {
      u->act = updt.fm_prv * u->act_q0 + updt.fm_hid * su->act_q0; // compute new value
    }
    else {
      u->act = u->act_q0;       // keep previous
    }
  }
  u->act_eq = u->act_nd = u->act;
  u->da = 0.0f;
  // u->AddToActBuf(syn_delay); // todo:

  if(deep.on && Quarter_DeepRawNow(net->quarter)) {
    Compute_DeepRaw(u, net, thr_no);
  }
}


