// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(FullPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);
  
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
      for(int sui = 0; sui < send_lay->n_units; sui++) {
        UNIT_STATE* su = send_lay->GetUnitState(net, sui);
        if(self_con || (ru != su))
          ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

int STATE_CLASS(FullPrjnSpec)::ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con,
                                                float init_wt) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);
  
  int rval = 0;

  int no = send_lay->n_units;
  if(!self_con && (send_lay == recv_lay))
    no--;

  int n_new_cons = (int)(p_add_con * (float)no);
  if(n_new_cons <= 0) return 0;
  int* new_idxs = new int[no];
  IntArrayFillSeq(new_idxs, no);
  
  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    IntArrayPermute(new_idxs, no, 0); // CRITICAL: do NOT use -1 for thr_no so dmem has same rnd!
    IntArraySort(new_idxs, n_new_cons, false); // keep selected subset sorted for optimizing con search etc
    for(int sui=0; sui < n_new_cons; sui++) {
      UNIT_STATE* su = send_lay->GetUnitState(net, new_idxs[sui]);
      int cn = ru->ConnectFromCk(net, su, prjn, false, true, init_wt); // set init_wt
      // check means that it won't add any new connections if already there!
      if(cn >= 0) {
        rval++;
      }
    }
  }

  delete [] new_idxs;
  
  return rval;
}
