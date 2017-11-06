// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(InvertUnitSpec)::Compute_ActFmSource(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)u->RecvConState(net, 0);
  LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->UnState(0, net);
  LEABRA_LAYER_STATE* fmlay = (LEABRA_LAYER_STATE*)cg->GetSendLayer(net);
  if(fmlay->lesioned()) {
    u->act = 1.0f;
    return;
  }
  u->act = 1.0f - su->act_eq;
  u->act_eq = u->act_nd = u->act;
  u->da = 0.0f;
  // u->AddToActBuf(syn_delay); // todo
}


