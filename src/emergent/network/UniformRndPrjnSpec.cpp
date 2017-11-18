// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(UniformRndPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) { 
  if(same_seed) {
    rndm_seed.OldSeed();
  }

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  const int rlay_no = recv_lay->n_units;
  const int slay_no = send_lay->n_units;
  int recv_no;
  if(!self_con && (send_lay == recv_lay))
    recv_no = (int) ((p_con * (float)(slay_no-1)) + .5f);
  else
    recv_no = (int) ((p_con * ((float)slay_no)) + .5f);
  if(recv_no <= 0) recv_no = 1;

  // sending number is even distribution across senders plus some imbalance factor
  float send_no_flt = (float)(rlay_no * recv_no) / (float)slay_no;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(send_no_flt);
  int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
  if(send_no > rlay_no) send_no = rlay_no;

  // pre-allocate connections!
  if(!make_cons) {
    recv_lay->RecvConsPreAlloc(net, prjn, recv_no);
    send_lay->SendConsPreAlloc(net, prjn, send_no);
    return;
  }

  if((send_lay == recv_lay) && sym_self) {
    // trick is to divide cons in half, choose recv, send at random
    // for 1/2 cons, then go through all units and make the symmetric cons..
    // TestWarning(p_con > .95f, "Connect_impl", "there is usually less than complete connectivity for high values of p_con (>.95) in symmetric, self-connected layers using permute!");
    // pre-allocate connections!
    int first;
    if(!self_con)
      first = (int) (.5f * p_con * (float)(slay_no-1));
    else
      first = (int) (.5f * p_con * (float)slay_no);
    if(first <= 0) first = 1;

    int* ru_list = new int[rlay_no]; // receiver permution list
    int* perm_list = new int[rlay_no]; // sender permution list

    IntArraySeqPermute(ru_list, rlay_no, 0); // CRITICAL: do NOT use -1 for thr_no so dmem has same rnd!

    for(int rui=0; rui < rlay_no; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, ru_list[rui]);

      int n_send = 0;
      for(int sui=0; sui < rlay_no; sui++) {
        UNIT_STATE* su = recv_lay->GetUnitState(net, sui);
        if(!self_con && (ru == su)) continue;
        // don't connect to anyone who already recvs from me cuz that will make
        // a symmetric connection which isn't good: symmetry will be enforced later
        CON_STATE* scg = su->RecvConStatePrjn(net, prjn);
        if(scg->FindConFromIdx(ru->flat_idx) >= 0) continue;
        perm_list[n_send++] = sui;
      }
      IntArrayPermute(perm_list, n_send, 0); // CRITICAL: do NOT use -1 for thr_no so dmem has same rnd!
      int mxno = MIN(first, n_send);
      IntArraySort(perm_list, mxno, false); // keep selected subset sorted for optimizing con search etc
      for(int j=0; j < mxno; j++) {       // only connect 1/2 of the units
        UNIT_STATE* su = recv_lay->GetUnitState(net, perm_list[j]);
        ru->ConnectFrom(net, su, prjn, false, true); // true = ignore errs -- to be expected
      }
    }
    // now go thru and make the symmetric connections
    for(int rui=0; rui < rlay_no; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, ru_list[rui]);
      CON_STATE* scg = ru->SendConStatePrjn(net, prjn);
      if(scg == NULL) continue;
      for(int i=0; i < scg->size; i++) {
        UNIT_STATE* su = scg->UnState(i,net);
        ru->ConnectFromCk(net, su, prjn, true); // true = ignore errs -- to be expected
      }
    }

    delete [] ru_list;
    delete [] perm_list;
  }
  else {                        // not a symmetric self projection
    int* perm_list = new int[slay_no]; // sender permution list
    
    for(int rui=0; rui < rlay_no; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
      int n_send = 0;
      for(int sui=0; sui < slay_no; sui++) {
        UNIT_STATE* su = send_lay->GetUnitState(net, sui);
        if(!self_con && (ru == su)) continue;
        perm_list[n_send++] = sui;
      }
      IntArrayPermute(perm_list, n_send, 0); // CRITICAL: do NOT use -1 for thr_no so dmem has same rnd!
      int mxno = MIN(n_send, recv_no);
      IntArraySort(perm_list, mxno, false); // keep selected subset sorted for optimizing con search etc
      for(int j=0; j < mxno; j++) {
        UNIT_STATE* su = send_lay->GetUnitState(net, perm_list[j]);
        ru->ConnectFrom(net, su, prjn, false, true); // true = ignore errs -- to be expected
      }
    }

    delete [] perm_list;
  }
}

