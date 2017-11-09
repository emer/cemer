// this is included directly in LeabraExtraLayerSpecs_cpp / _cuda
// {


void STATE_CLASS(DecodeTwoDValLayerSpec)::ReadValue_ugp
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    if(u->NRecvConGps(net) == 0) continue;
    CON_STATE* cg = u->RecvConState(net, 0);
    if(cg->NotActive()) continue;
    LEABRA_UNIT_STATE* su = (LEABRA_UNIT_STATE*)cg->UnState(0, net);
    u->net = su->net;
    u->act = su->act;
    u->act_eq = su->act_eq;
    u->act_nd = su->act_nd;
  }
  inherited::ReadValue_ugp(lay, net, gpidx);
}

