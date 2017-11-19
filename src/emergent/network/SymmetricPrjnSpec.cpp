// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(SymmetricPrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);

  int ru_nunits = recv_lay->n_units;
  int su_nunits = send_lay->n_units;

  int recv_no = (int)((recv_p_con * (float)su_nunits) + 0.5f);
  int send_no = (int)((send_p_con * (float)ru_nunits) + 0.5f);

  PRJN_STATE* send_fm_recv = send_lay->FindRecvPrjnFromLay(net, recv_lay);
  if(!send_fm_recv) return;
  
  if(!make_cons) {
    // pre-allocate connections!
    recv_lay->RecvConsPreAlloc(net, prjn, recv_no);
    send_lay->SendConsPreAlloc(net, prjn, send_no);
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
      for(int sui=0; sui < su_nunits; sui++) {
        UNIT_STATE* su = send_lay->GetUnitState(net,  sui);
        CON_STATE* scg = su->RecvConStatePrjn(net, send_fm_recv);
        if(scg->FindConFromIdx(ru->flat_idx) < 0) continue; // they don't connect from us..
        if(make_cons) {
          ru->ConnectFrom(net, su, prjn);                     // go ahead
        }
        else { // just inc allocs
          ru->RecvConsAllocInc(net, prjn, 1);
          su->SendConsAllocInc(net, prjn, 1);
        }
      }
    }
  }
}

