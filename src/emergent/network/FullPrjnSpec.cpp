// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(FullPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  
  int recv_no = send_lay->n_units; // recv from number of senders
  if(!self_con && (send_lay == recv_lay))
    recv_no--;

  int send_no = recv_lay->n_units; // number of recv's
  if(!self_con && (send_lay == recv_lay))
    send_no--;

  if(!make_cons) {
    // pre-allocate connections!
    recv_lay->RecvConsPreAlloc(net, prjn, recv_no);
    send_lay->SendConsPreAlloc(net, prjn, send_no);
  }
  else {
    for(int rui = 0; rui < recv_lay->n_units; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
      if(ru->lesioned()) continue;
      for(int sui = 0; sui < send_lay->n_units; sui++) {
        UNIT_STATE* su = send_lay->GetUnitState(net, sui);
        if(su->lesioned()) continue;
        if(self_con || (ru != su))
          ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

int STATE_CLASS(FullPrjnSpec)::ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con,
                                                float init_wt) {
  // todo: depends on int_Array!
  
  // if(!(bool)prjn->from) return 0;

  int rval = 0;

  // int no = prjn->from->units.leaves;
  // if(!self_con && (prjn->from.ptr() == prjn->layer))
  //   no--;

  // int n_new_cons = (int)(p_add_con * (float)no);
  // if(n_new_cons <= 0) return 0;
  // int_Array new_idxs;
  // new_idxs.SetSize(no);
  // new_idxs.FillSeq();
  // FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
  //   new_idxs.Permute();
  //   for(int i=0;i<n_new_cons;i++) {
  //     Unit* su = (Unit*)prjn->from->units.Leaf(new_idxs[i]);
  //     int cn = ru->ConnectFromCk(su, prjn, false, true, init_wt); // set init_wt
  //     // check means that it won't add any new connections if already there!
  //     if(cn >= 0) {
  //       rval++;
  //     }
  //   }
  // }
  return rval;
}
