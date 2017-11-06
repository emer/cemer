// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(ECoutUnitSpec)::ClampFromECin(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(deep.on) {
    Compute_DeepMod(u, net, thr_no);
  }
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)recv_gp->UnState(0, net);
    float inval = su->act_eq;
    u->act = clamp_range.Clip(inval);
    if(deep.on) {
      u->act *= u->deep_mod;
    }
    u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;
    //    u->AddToActBuf(syn_delay);
  }
}

float STATE_CLASS(ECoutUnitSpec)::Compute_SSE(UNIT_STATE* ru, NETWORK_STATE* rnet, int thr_no, bool& has_targ) {
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ru;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)rnet;
  
  float uerr = u->act_p - u->act_q1;
  if(fabsf(uerr) >= sse_tol) {
    u->misc_1 = uerr * uerr;
  }
  else {
    u->misc_1 = 0.0f;
  }

  return inherited::Compute_SSE(ru, rnet, thr_no, has_targ);
}
