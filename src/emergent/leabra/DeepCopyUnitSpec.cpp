// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(DeepCopyUnitSpec)::Compute_ActFmSource
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {

  LEABRA_CON_STATE* cg = (LEABRA_CON_STATE*)u->RecvConStateSafe(net, 0);
  LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->UnState(0, net);
  LEABRA_LAYER_STATE* fmlay = (LEABRA_LAYER_STATE*)cg->GetSendLayer(net);
  if(fmlay->lesioned()) {
    u->act = 0.0f;
    return;
  }
  float var = 0.0f;
  switch(deep_var) {
  case DEEP_RAW:
    var = su->deep_raw;
    break;
  case DEEP_MOD:
    var = su->deep_mod;
    break;
  case DEEP_LRN:
    var = su->deep_lrn;
    break;
  case DEEP_CTXT:
    var = su->deep_ctxt;
    break;
  }
  u->act = var;
  u->act_eq = u->act_nd = u->act;
  u->da = 0.0f;
  // u->AddToActBuf(syn_delay); // todo:

  if(deep.on && Quarter_DeepRawNow(net->quarter)) {
    Compute_DeepRaw(u, net, thr_no);
  }
}
