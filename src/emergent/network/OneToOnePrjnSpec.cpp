// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(OneToOnePrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) { 
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  
  int n_recv = recv_lay->n_units - recv_start;
  int n_send = send_lay->n_units - send_start;

  if(n_recv == 0 || n_send == 0) return;

  if(use_gp) {
    if((recv_lay->n_ungps > 0) && recv_lay->un_geom_n >= n_send) {
      ConnectRecvGp_impl(prjn, net, make_cons);
      return;
    }
    else if((send_lay->n_ungps > 0) && send_lay->un_geom_n >= n_recv) {
      ConnectSendGp_impl(prjn, net, make_cons);
      return;
    }
  }

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = n_recv;
  max_n = MIN(n_recv, max_n);
  max_n = MIN(n_send, max_n);
  for(int i=0; i<max_n; i++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, recv_start + i);
    UNIT_STATE* su = send_lay->GetUnitState(net, send_start + i);
    if(self_con || (ru != su)) {
      if(!make_cons) {
        ru->RecvConsPreAlloc(net, prjn, 1);
        su->SendConsPreAlloc(net, prjn, 1);
      }
      else {
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

void STATE_CLASS(OneToOnePrjnSpec)::ConnectRecvGp_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int n_send = send_lay->n_units - send_start;
  int n_recv = recv_lay->un_geom_n - recv_start;

  int n_gps = recv_lay->gp_geom_n;

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = n_recv;
  max_n = MIN(n_recv, max_n);
  max_n = MIN(n_send, max_n);
  for(int gi=0; gi < n_gps; gi++) {
    for(int i=0; i<max_n; i++) {
      UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, gi, recv_start + i);
      UNIT_STATE* su = send_lay->GetUnitState(net, send_start + i);
      if(self_con || (ru != su)) {
        if(!make_cons) {
          ru->RecvConsPreAlloc(net, prjn, 1);
          if(gi == 0)
            su->SendConsPreAlloc(net, prjn, n_gps);
        }
        else {
          ru->ConnectFrom(net, su, prjn);
        }
      }
    }
  }
}

void STATE_CLASS(OneToOnePrjnSpec)::ConnectSendGp_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int n_recv = recv_lay->n_units - recv_start;
  int n_send = send_lay->un_geom_n - send_start;

  int n_gps = send_lay->gp_geom_n;

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = n_recv;
  max_n = MIN(n_recv, max_n);
  max_n = MIN(n_send, max_n);
  for(int gi=0; gi < n_gps; gi++) {
    for(int i=0; i<max_n; i++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, recv_start + i);
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, gi, send_start + i);
      if(self_con || (ru != su)) {
        if(!make_cons) {
          if(gi == 0)
            ru->RecvConsPreAlloc(net, prjn, n_gps);
          su->SendConsPreAlloc(net, prjn, 1);
        }
        else {
          ru->ConnectFrom(net, su, prjn);
        }
      }
    }
  }
}

