// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(BasAmygUnitSpec)::Init_Weights
  (UNIT_STATE* uv, NETWORK_STATE* net, int thr_no)  {

  inherited::Init_Weights(uv, net, thr_no);
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
  if(dar == D1R) {
    u->SetExtFlag(LEABRA_UNIT_STATE::D1R);
    u->ClearExtFlag(LEABRA_UNIT_STATE::D2R);
  }
  else {
    u->SetExtFlag(LEABRA_UNIT_STATE::D2R);
    u->ClearExtFlag(LEABRA_UNIT_STATE::D1R);
  }
  if(acq_ext == ACQ) {
    u->SetExtFlag(LEABRA_UNIT_STATE::ACQUISITION);
    u->ClearExtFlag(LEABRA_UNIT_STATE::EXTINCTION);
  }
  else {
    u->SetExtFlag(LEABRA_UNIT_STATE::EXTINCTION);
    u->ClearExtFlag(LEABRA_UNIT_STATE::ACQUISITION);
  }
  if(valence == APPETITIVE) {
    u->SetExtFlag(LEABRA_UNIT_STATE::APPETITIVE);
    u->ClearExtFlag(LEABRA_UNIT_STATE::AVERSIVE);
  }
  else {
    u->SetExtFlag(LEABRA_UNIT_STATE::AVERSIVE);
    u->ClearExtFlag(LEABRA_UNIT_STATE::APPETITIVE);
  }
}

void STATE_CLASS(BasAmygUnitSpec)::Compute_DeepMod
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {

  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)u->GetOwnLayer(net);
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  if(deep.SendDeepMod()) {
    u->deep_lrn = u->deep_mod = u->act;      // record what we send!
  }
  else if(deep.IsTRC()) {
    u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
    if(trc.thal_gate) {
      u->net *= u->thal;
    }
  }
  // must be SUPER units at this point
  // else if(lay->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet
  else if(u->deep_mod_net <= deep.mod_thr) { // not enough yet
    u->deep_lrn = 0.0f;    // default is 0!
    u->deep_mod = 1.0f;
  }
  else {
    u->deep_lrn = u->deep_mod_net / lgpd->am_deep_mod_net.max;
    u->deep_mod = 1.0f;                               // do not modulate with deep_mod!
  }
}

float STATE_CLASS(BasAmygUnitSpec)::Compute_DaModNetin
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, float& net_syn) {
  float da_val = u->da_p * net_syn;
  if(dar == D2R)
    da_val = -da_val;           // flip the sign
  if(net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
    return da_mod.plus * da_val;
  }
  else {                      // MINUS_PHASE
    return da_mod.minus * da_val;
  }
}

