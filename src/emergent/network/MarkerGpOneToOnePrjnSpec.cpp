// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(MarkerGpOneToOnePrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) { 

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  
  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->gp_geom_x, send_lay->gp_geom_y);

  int ru_nunits;
  int su_nunits;
  if(recv_lay->HasUnitGroups()) {
    ru_nunits = recv_lay->un_geom_n;
  }
  else {
    ru_nunits = recv_lay->n_units;
  }
  if(send_lay->HasUnitGroups()) {
    su_nunits = send_lay->un_geom_n;
  }
  else {
    su_nunits = send_lay->n_units;
  }

  int s_alloc = (ru_nunits / su_nunits) + 1;

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = recv_lay->gp_geom_n - recv_start;
  max_n = MIN(recv_lay->gp_geom_n - recv_start, max_n);
  max_n = MIN(send_lay->gp_geom_n - send_start, max_n);
  max_n = MAX(1, max_n);        // lower limit of 1
  for(int i=0; i<max_n; i++) {
    int rgpidx = i + recv_start;
    int sgpidx = i + send_start;

    if(!make_cons) {
      recv_lay->RecvConsPreAlloc(net, prjn, 1);
      send_lay->SendConsPreAlloc(net, prjn, s_alloc);
    }
    else {
      for(int rui=0; rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        if(ru->lesioned()) continue;
        int sui = rui % su_nunits;
        UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
        if(su->lesioned()) continue;
        if(self_con || (ru != su))
          ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

