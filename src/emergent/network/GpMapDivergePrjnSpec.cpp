// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(GpMapDivergePrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  if(!recv_lay->HasUnitGroups()) {
    net->StateError("GpMapDivergePrjnSpec::Connect_impl requires recv layer to have unit groups!");
    return;
  }

  if((send_lay->un_geom_x != recv_lay->un_geom_x) ||
     (send_lay->un_geom_y != recv_lay->un_geom_y)) {
    net->StateError("GpMapDivergePrjnSpec::Connect_impl requires sending layer unit geometry to match receiving layer unit geometry, i.e., within each unit group!");
    return;
  }

  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  int n_ru_gps = recv_lay->gp_geom_n;

  int alloc_no = n_ru_gps;      // number of cons per recv unit

  if(!make_cons) {
    send_lay->SendConsPreAlloc(net, prjn, alloc_no);
    recv_lay->RecvConsPreAlloc(net, prjn, 1);
    return;
  }
  
  for(int sui = 0; sui < send_lay->n_units; sui++) {
    UNIT_STATE* su = send_lay->GetUnitState(net, sui);
    TAVECTOR2I ruc;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++) {
        int rgpidx = recv_lay->GetGpIdxFmXY(ruc.x, ruc.y);
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, sui);
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

