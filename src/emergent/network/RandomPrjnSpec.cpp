// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(RandomPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) { 
  rndm_seed.OldSeed();

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  
  int n_recv_units = recv_lay->n_units;
  int n_send_units = send_lay->n_units;

  int* send_alloc = new int[n_send_units];
  int* recv_alloc = new int[n_recv_units];
  memset(recv_alloc, 0, n_recv_units * sizeof(int));
  memset(send_alloc, 0, n_send_units * sizeof(int));
  
  bool* cons = NULL;
  
  if(make_cons) {
    cons = new bool[n_recv_units * n_send_units];
    memset(cons, 0, n_recv_units * n_send_units * sizeof(bool));
  }

  for (int i = 0; i < n_recv_units; i++) {
    for (int j= 0; j < n_send_units; j++) {
      if (Random::BoolProb(p_con)) {
        if(make_cons) {
          cons[ j * n_recv_units + i] = true;
        }
        send_alloc[j]++;
        recv_alloc[i]++;
      }
    }
  }

  if(!make_cons) {
    for (int i = 0; i < n_recv_units; i++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, i);
      ru->RecvConsPreAlloc(net, prjn, recv_alloc[i]);
    }
    for (int j = 0; j < n_send_units; j++) {
      UNIT_STATE* su = send_lay->GetUnitState(net, j);
      su->SendConsPreAlloc(net, prjn, send_alloc[j]);
    }
  }
  else {
    for (int i = 0; i < n_recv_units; i++) {
      for (int j = 0; j < n_send_units; j++) {
        if (cons[j * n_recv_units + i]) {
          UNIT_STATE* ru = recv_lay->GetUnitState(net, i);
          UNIT_STATE* su = send_lay->GetUnitState(net, j);
          ru->ConnectFrom(net, su, prjn);
        }
      }
    }
  }

  delete [] send_alloc;
  delete [] recv_alloc;
  if(cons) {
    delete [] cons;
  }
}


