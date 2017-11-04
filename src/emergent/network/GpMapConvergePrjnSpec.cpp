// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(GpMapConvergePrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  if(!send_lay->HasUnitGroups()) {
    net->StateError("GpMapConvergePrjnSpec::Connect_impl requires send layer to have unit groups!");
    return;
  }

  if((send_lay->un_geom_x != recv_lay->un_geom_x) ||
     (send_lay->un_geom_y != recv_lay->un_geom_y)) {
    net->StateError("GpMapConvergePrjnSpec::Connect_impl requires sending layer unit geometry to match receiving layer unit geometry, i.e., within each unit group!");
    return;
  }

  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->gp_geom_x, send_lay->gp_geom_y);
  int n_su_gps = send_lay->gp_geom_n;

  int alloc_no = n_su_gps;      // number of cons per recv unit

  // pre-alloc senders -- only 1
  if(!make_cons) {
    send_lay->SendConsPreAlloc(net, prjn, 1);
    recv_lay->RecvConsPreAlloc(net, prjn, alloc_no);
    return;
  }

  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    TAVECTOR2I suc;
    for(suc.y = 0; suc.y < su_geo.y; suc.y++) {
      for(suc.x = 0; suc.x < su_geo.x; suc.x++) {
        int sgpidx = send_lay->GetGpIdxFmXY(suc.x, suc.y);
        UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, rui);
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

