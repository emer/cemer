// contains non-inline (INIMPL) functions 
// if used, must be included directly in ProjectionSpec*.cpp

void STATE_CLASS(ProjectionSpec)::Connect_Sizes(PRJN_STATE* prjn, NETWORK_STATE* net) {
  if(!prjn->IsActive(net)) return;
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(!recv_lay || !send_lay) return; // unnec but..
  if(recv_lay->n_units == 0 || send_lay->n_units == 0) return;
  Connect_impl(prjn, net, 0); // 0 = allocate = make_cons false
}

void STATE_CLASS(ProjectionSpec)::Connect_Cons(PRJN_STATE* prjn, NETWORK_STATE* net, int pass) {
  if(!prjn->IsActive(net)) return;
  if(!ConnectPassCheck(prjn, net, pass)) {
    if(pass == 1) {             // if we're skipping on pass1, then need pass2!
      net->needs_prjn_pass2 = true;
    }
    return; // nope
  }
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(!recv_lay || !send_lay) return; // unnec but..
  if(recv_lay->n_units == 0 || send_lay->n_units == 0) return;
  Connect_impl(prjn, net, pass); // 1 = make connections, first/main pass, 2 = optional
  // Init_Weights(prjn); // connection is NOT init weights -- now definitivitely 2 separate steps -- this is super slow for large nets
}

int STATE_CLASS(ProjectionSpec)::ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt) {
  return 0;
}

int STATE_CLASS(ProjectionSpec)::ProbAddCons(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt) {
  int rval = ProbAddCons_impl(prjn, net, p_add_con);
  return rval;
}

void STATE_CLASS(ProjectionSpec)::SetCnWtScale(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float wt_val) {
  if(set_scale) {
    SetCnWtRnd(prjn, net, thr_no, cg, cn_idx);
    SetCnScale(prjn, net, thr_no, cg, cn_idx, wt_val);
  }
  else {
    SetCnWt(prjn, net, thr_no, cg, cn_idx, wt_val);
  }
}

void STATE_CLASS(ProjectionSpec)::SetCnWt(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float wt_val) {
  CON_SPEC_CPP* cs = cg->GetConSpec(net);
  if(add_rnd_var) {
    int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
    cs->C_Init_Weight_AddRndVar(wt_val, eff_thr_no);
  }
  cs->C_ApplyLimits(wt_val);
  cg->Cn(cn_idx,CON_STATE::WT,net) = wt_val;
  cs->SetConScale(1.0f, cg, cn_idx, net, thr_no); // reset scale..
  cs->C_Init_dWt(cg->Cn(cn_idx,CON_STATE::DWT,net));
}

void STATE_CLASS(ProjectionSpec)::SetCnWtRnd(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx) {
  CON_SPEC_CPP* cs = cg->GetConSpec(net);
  float& wt_val = cg->Cn(cn_idx,CON_STATE::WT,net);
  int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
  cs->C_Init_Weight_Rnd(wt_val, eff_thr_no); // std rnd wts
  cs->C_Init_dWt(cg->Cn(cn_idx,CON_STATE::DWT,net));
}

void STATE_CLASS(ProjectionSpec)::SetCnScale(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float sc_val) {
  CON_SPEC_CPP* cs = cg->GetConSpec(net);
  cs->SetConScale(sc_val, cg, cn_idx, net, thr_no);
}

void STATE_CLASS(ProjectionSpec)::Init_Weights_Prjn(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  if(!init_wts) return;         // shouldn't happen
  if(cg->size == 0) return;
  CON_SPEC_CPP* cs = prjn->GetConSpec(net);
  if(set_scale) {
    cs->Init_Weights_scale(cg, net, thr_no, init_wt_val);
  }
  else {
    cs->Init_Weights(cg, net, thr_no);
  }
}

void STATE_CLASS(ProjectionSpec)::Init_Weights_renorm(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  if(!renorm_wts.on) return;
  if(cg->size == 0) return;
  CON_SPEC_CPP* cs = prjn->GetConSpec(net);
  if(set_scale) {
    cs->RenormScales(cg, net, thr_no, renorm_wts.mult_norm, renorm_wts.avg_wt);
  }
  else {
    cs->RenormWeights(cg, net, thr_no, renorm_wts.mult_norm, renorm_wts.avg_wt);
  }
}


///////////////////////////////////////////////////////
//      Group-based connectivity routines for subtypes

void STATE_CLASS(ProjectionSpec)::Connect_Gps
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con,
   bool sym_same_lay, int make_cons, bool share_con, bool recip) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(recip) {
    recv_lay = send_lay;
    send_lay = prjn->GetRecvLayerState(net);
  }
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  bool same_gp = (recv_lay == send_lay && rgpidx == sgpidx);

  if(!make_cons) {
    int send_no = 1;
    int recv_no = 1;
    if(p_con < 1.0f) {
      if(p_con < 0) p_con = 1.0f;
      if(!self_con && same_gp)
        recv_no = (int) ((p_con * (float)(su_nunits-1)) + .5f);
      else
        recv_no = (int) ((p_con * (float)su_nunits) + .5f);
      if(recv_no <= 0)  recv_no = 1;
      recv_no = MIN(su_nunits, recv_no);

      // sending number is even distribution across senders plus some imbalance factor
      float send_no_flt = (float)(ru_nunits * recv_no) / (float)su_nunits;
      // add SEM as corrective factor
      float send_sem = send_no_flt / sqrtf(send_no_flt);
      send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
      send_no = MIN(ru_nunits, send_no);

      if(same_gp)
        recv_no += 2;           // bit of extra room here too
    }
    else {
      send_no = ru_nunits;
      recv_no = su_nunits;
    }
    for(int rui=0;rui<ru_nunits;rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
      if(recip) {
        ru->SendConsAllocInc(net, prjn, recv_no);
      }
      else {
        ru->RecvConsAllocInc(net, prjn, recv_no);
      }
    }
    for(int sui=0;sui<su_nunits;sui++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
      if(recip) {
        su->RecvConsAllocInc(net, prjn, send_no);
      }
      else {
        su->SendConsAllocInc(net, prjn, send_no);
      }
    }
    return;
  }

  if(p_con < 0 || (p_con < 1.0f && recip)) {               // this means: make symmetric connections!
    Connect_Gps_Sym(prjn, net, rgpidx, sgpidx, recip);
  }
  else if(p_con == 1.0f) {
    Connect_Gps_Full(prjn, net, rgpidx, sgpidx, share_con, recip);
  }
  else if(same_gp && sym_same_lay) {
    Connect_Gps_ProbSymSameGp(prjn, net, rgpidx, sgpidx, p_con);
  }
  else {
    if(recv_lay == send_lay && sym_same_lay) {
      Connect_Gps_ProbSymSameLay(prjn, net, rgpidx, sgpidx, p_con);
    }
    else {
      Connect_Gps_Prob(prjn, net, rgpidx, sgpidx, p_con, share_con, recip);
    }
  }
}

void STATE_CLASS(ProjectionSpec)::Connect_Gps_Full
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, bool share_con, bool recip) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(recip) {
    recv_lay = send_lay;
    send_lay = prjn->GetRecvLayerState(net);
  }
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  for(int rui=0; rui < ru_nunits; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
    if(share_con && rgpidx > 0 && !recip && net->RecvOwnsCons()) {
      UNIT_STATE* shru = recv_lay->GetUnitStateGpUnIdx(net, 0, rui); // group 0
      ru->ShareRecvConsFrom(net, shru, prjn);
    }
    for(int sui=0; sui < su_nunits; sui++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
      if(!self_con && (su == ru)) continue;
      if(recip) {             // backwards
        su->ConnectFrom(net, ru, prjn);
      }
      else {
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

void STATE_CLASS(ProjectionSpec)::Connect_Gps_Prob
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con, bool share_con, bool recip) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(recip) {
    recv_lay = send_lay;
    send_lay = prjn->GetRecvLayerState(net);
  }
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  bool same_gp = (recv_lay == send_lay && rgpidx == sgpidx);

  int recv_no;
  if(!self_con && same_gp)
    recv_no = (int) ((p_con * (float)(su_nunits-1)) + .5f);
  else
    recv_no = (int) ((p_con * (float)su_nunits) + .5f);
  if(recv_no <= 0)  recv_no = 1;
  recv_no = MIN(su_nunits, recv_no);

  int* perm_list = new int[su_nunits]; // sender permution list
  for(int rui=0; rui < ru_nunits; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
    if(share_con && rgpidx > 0 && !recip && net->RecvOwnsCons()) {
      UNIT_STATE* shru = recv_lay->GetUnitStateGpUnIdx(net, 0, rui); // group 0
      ru->ShareRecvConsFrom(net, shru, prjn);
    }
    
    int n_send = 0;
    for(int sui=0; sui < su_nunits; sui++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
      if(!self_con && (ru == su)) continue;
      perm_list[n_send++] = sui;
    }
    IntArrayPermute(perm_list, n_send);
    for(int j=0; j<recv_no; j++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, perm_list[j]);
      if(recip) {
        su->ConnectFrom(net, ru, prjn);
      }
      else {
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
  delete [] perm_list;
}


void STATE_CLASS(ProjectionSpec)::Connect_Gps_ProbSymSameGp
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  // trick is to divide cons in half, choose recv, send at random
  // for 1/2 cons, then go through all units and make the symmetric cons..
  // pre-allocate connections!
  // TestWarning(p_con > .95f, "Connect_Gps_SymSameGp",
  //             "usually produces less than complete connectivity for high values of p_con in symmetric, self-connected layers using permute!");
  int n_cons;
  if(!self_con)
    n_cons = (int) (p_con * (float)(su_nunits-1) + .5f);
  else
    n_cons = (int) (p_con * (float)su_nunits + .5f);
  int first = (int)(.5f * (float)n_cons);
  if(first <= 0) first = 1;

  int* ru_list = new int[ru_nunits]; // receiver permution list
  IntArraySeqPermute(ru_list, ru_nunits);

  int* perm_list = new int[su_nunits]; // sender permution list
  for(int rui=0; rui < ru_nunits; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ru_list[rui]);
    
    int n_send = 0;
    for(int sui=0; sui < su_nunits; sui++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
      if(!self_con && (ru == su)) continue;
      // don't connect to anyone who already recvs from me cuz that will make
      // a symmetric connection which isn't good: symmetry will be enforced later
      CON_STATE* scg = su->RecvConStatePrjn(net, prjn);
      if(scg->FindConFromIdx(ru->flat_idx) >= 0) continue;
      perm_list[n_send++] = sui;
    }
    IntArrayPermute(perm_list, n_send);
    int mxno = MIN(first, n_send);
    for(int j=0; j < mxno; j++) {    // only connect 1/2 of the units
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, perm_list[j]);
      ru->ConnectFromCk(net, su, prjn);
    }
  }
  // now go thru and make the symmetric connections
  for(int rui=0; rui < ru_nunits; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ru_list[rui]);
    CON_STATE* scg = ru->SendConStatePrjn(net, prjn);
    if(scg == NULL) continue;
    for(int i=0;i<scg->size;i++) {
      UNIT_STATE* su = scg->UnState(i,net);
      ru->ConnectFromCk(net, su, prjn, true); // ignore errs
    }
  }
}

void STATE_CLASS(ProjectionSpec)::Connect_Gps_ProbSymSameLay
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  // within the same layer, i want to make connections symmetric: either i'm the
  // first to connect to other group, or other group has already connected to me
  // so I should just make symmetric versions of its connections
  // take first send unit and find if it recvs from anyone in this prjn yet
  UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, 0);
  CON_STATE* scg = su->RecvConStatePrjn(net, prjn);
  if((scg != NULL) && (scg->size > 0)) {        // sender has been connected already: try to connect me!
    int n_con = 0;              // number of actual connections made

    for(int rui=0; rui < ru_nunits; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
      CON_STATE* rcg = ru->SendConStatePrjn(net, prjn);
      if(rcg == NULL) continue;
      for(int i=0;i<rcg->size;i++) {
        UNIT_STATE* ssu = rcg->UnState(i,net);
        // only connect if this sender is in actual group I'm trying to connect
        int osgpidx = ssu->gp_idx;
        if(osgpidx == sgpidx) {
          if(ru->ConnectFromCk(net, ssu, prjn) >= 0)
            n_con++;
        }
      }
    }
    if(n_con > 0)               // made some connections, bail
      return;
    // otherwise, go ahead and make new connections!
  }
  Connect_Gps_Prob(prjn, net, rgpidx, sgpidx, p_con, false, false);
}

void STATE_CLASS(ProjectionSpec)::Connect_Gps_Sym
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, bool recip) {
  
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  if(recip) {                   // swap idxs
    int rgt = rgpidx;
    rgpidx = sgpidx;
    sgpidx = rgt;
  }

  PRJN_STATE* send_fm_recv = send_lay->FindRecvPrjnFromLay(net, recv_lay);
  if(!send_fm_recv) return;
  
  for(int rui=0; rui < ru_nunits; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
    for(int sui=0; sui < su_nunits; sui++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, sui);
      CON_STATE* scg = su->RecvConStatePrjn(net, send_fm_recv);
      if(scg->FindConFromIdx(ru->flat_idx) < 0) continue; // they don't connect from us..
      ru->ConnectFrom(net, su, prjn);                     // go ahead
    }
  }
}

