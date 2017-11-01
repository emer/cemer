// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(GpOneToOnePrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int n_recv = recv_lay->n_units - recv_start;
  int n_send = send_lay->un_geom_n - send_start;

  // special case where we map units to groups, only if no offsets..
  if(send_lay->HasUnitGroups() && recv_lay->n_units == send_lay->gp_geom_n) {
    Connect_RecvUnitsSendGps(prjn, net, make_cons);
    return;
  }
  else if(recv_lay->HasUnitGroups() && send_lay->n_units == recv_lay->gp_geom_n) {
    Connect_SendUnitsRecvGps(prjn, net, make_cons);
    return;
  }

  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  int r_st = recv_start;
  int s_st = send_start;

  int max_n = n_conns;
  if(recv_lay->HasUnitGroups()) {
    if(n_conns < 0) 
      max_n = recv_lay->gp_geom_n - recv_start;
    max_n = MIN(recv_lay->gp_geom_n - recv_start, max_n);
  }
  else {
    max_n = 1; 
    r_st = 0;
  }
  if(send_lay->HasUnitGroups()) {
    max_n = MIN(send_lay->gp_geom_n - send_start, max_n);
  }
  else {
    max_n = 1;
    s_st = 0;
  }
  max_n = MAX(1, max_n);        // lower limit of 1

  for(int i=0; i<max_n; i++) {  // loop over group index
    int rgpidx = i + r_st;
    int sgpidx = i + s_st;

    if(!make_cons) {
      // pre-allocate connections
      for(int rui=0; rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        if(ru) {
          ru->RecvConsPreAlloc(net, prjn, su_nunits);
        }
      }
      for(int sui=0; sui < su_nunits; sui++) {
        UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
        if(su) {
          su->SendConsPreAlloc(net, prjn, ru_nunits);
        }
      }
    }
    else {
      for(int rui=0; rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        if(!ru) continue;
        for(int sui=0; sui < su_nunits; sui++) {
          UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
          if(!su) continue;
          if(self_con || (ru != su))
            ru->ConnectFrom(net, su, prjn);
        }
      }
    }
  }
}

void STATE_CLASS(GpOneToOnePrjnSpec)::Connect_RecvUnitsSendGps(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int su_nunits = send_lay->un_geom_n;

  int max_n = n_conns;
  if(max_n < 0)
    max_n = send_lay->gp_geom_n - send_start;
  max_n = MIN(recv_lay->n_units - recv_start, max_n);
  max_n = MAX(1, max_n);        // lower limit of 1

  for(int i=0; i<max_n; i++) {  // loop over group index
    int runidx = i + recv_start;
    int sgpidx = i + send_start;
    if(!make_cons) {
      // pre-allocate connections
      UNIT_STATE* ru = recv_lay->GetUnitStateSafe(net, runidx);
      if(ru) {
        ru->RecvConsPreAlloc(net, prjn, su_nunits);
      }

      for(int sui=0; sui < su_nunits; sui++) {
        UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
        if(su) {
          su->SendConsPreAlloc(net, prjn, 1);
        }
      }
    }
    else {
      UNIT_STATE* ru = recv_lay->GetUnitStateSafe(net, runidx);
      if(!ru) continue;
      for(int sui=0; sui < su_nunits; sui++) {
        UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
        if(!su) continue;
        if(self_con || (ru != su))
          ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

void STATE_CLASS(GpOneToOnePrjnSpec)::Connect_SendUnitsRecvGps(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int ru_nunits = recv_lay->un_geom_n;

  int max_n = n_conns;
  if(max_n < 0)
    max_n = recv_lay->gp_geom_n - recv_start;
  max_n = MIN(send_lay->n_units - send_start, max_n);
  max_n = MAX(1, max_n);        // lower limit of 1

  for(int i=0; i<max_n; i++) {  // loop over group index
    int rgpidx = i + recv_start;
    int sunidx = i + send_start;

    if(!make_cons) {
      // pre-allocate connections
      UNIT_STATE* su = send_lay->GetUnitStateSafe(net, sunidx);
      if(!su) continue;
      su->SendConsPreAlloc(net, prjn, ru_nunits);

      for(int rui=0; rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        if(!ru) continue;
        ru->RecvConsPreAlloc(net, prjn, 1);
      }
    }
    else {
      UNIT_STATE* su = send_lay->GetUnitStateSafe(net, sunidx);
      if(!su) continue;
      for(int rui=0; rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        if(ru && (self_con || (ru != su)))
          ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}
