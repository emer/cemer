// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(TiledGpRFPrjnSpec)::Initialize_core() {
  send_gp_size = 4;
  send_gp_skip = 2;
  send_gp_start = -1;
  wrap = true;
  reciprocal = false;
  p_con = 1.0f;
  symmetric = true;
  wts_type = GAUSSIAN;
  share_cons = false;
  full_send = BY_UNIT;
  full_recv = BY_UNIT;
  wt_range.min = 0.4f;
  wt_range.max = 0.6f;
}

bool STATE_CLASS(TiledGpRFPrjnSpec)::ConnectPassCheck(PRJN_STATE* prjn, NETWORK_STATE* net, int pass) const {
  if(p_con < 1.0f && reciprocal && symmetric) return (pass == 2);
  return (pass == 1);
}

void STATE_CLASS(TiledGpRFPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);
  if(reciprocal) {
    recv_lay = send_lay;
    send_lay = prjn->GetRecvLayer(net);
  }

  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  if(!recv_lay->HasUnitGroups()) {
    ru_geo = 1;
  }
  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->gp_geom_x, send_lay->gp_geom_y);
  if(!send_lay->HasUnitGroups()) {
    su_geo = 1;
  }
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  int sg_sz_tot = send_gp_size.Product();
  int alloc_no = sg_sz_tot * su_nunits;

  if(share_cons) {
    if(ru_nunits % net->n_thrs_built != 0) {
      net->StateErrorVals
        ("Connect_impl: for share_cons = true, number of recv units per unit group must be an even multiple of number of threads:",
         "ru_nunits:", ru_nunits,
         "n_thrs_built:", net->n_thrs_built);
      return;
    }
  }
  
  TAVECTOR2I ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      if(!recv_lay->HasUnitGroups()) {
        rgpidx = -1;
      }
      TAVECTOR2I su_st;
      su_st = send_gp_start + ruc * send_gp_skip;
      TAVECTOR2I suc;
      TAVECTOR2I suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int sgpidx = -1;
          if(send_lay->HasUnitGroups()) {
            sgpidx = send_lay->GetGpIdxFmXY(suc_wrp.x, suc_wrp.y);
            if(!send_lay->GpIdxInRange(sgpidx)) continue;
          }
          // in base prjnspec
          Connect_Gps(prjn, net, rgpidx, sgpidx, p_con, symmetric, make_cons, share_cons,
                      reciprocal);
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, alloc
    prjn->GetRecvLayer(net)->RecvConsPostAlloc(net, prjn); // using these b/c reciprocal..
    prjn->GetSendLayer(net)->SendConsPostAlloc(net, prjn);
  }
}


void STATE_CLASS(TiledGpRFPrjnSpec)::Init_Weights_Prjn
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  if(cg->size == 0) return;
  if(wts_type == GAUSSIAN) {
    Init_Weights_Gaussian(prjn, net, thr_no, cg);
  }
  else {
    Init_Weights_Sigmoid(prjn, net, thr_no, cg);
  }
}

void STATE_CLASS(TiledGpRFPrjnSpec)::Init_Weights_Gaussian
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);

  // if(reciprocal) {
  //   recv_lay = prjn->from; // from perspective of non-recip!
  //   send_lay = prjn->layer;
  // }

  bool no_send_gp = (send_gp_size.Product() == 1);


  TAVECTOR2I send_un_geo;
  send_un_geo.SetXY(send_lay->un_geom_x, send_lay->un_geom_y);
  TAVECTOR2I recv_un_geo;
  recv_un_geo.SetXY(recv_lay->un_geom_x, recv_lay->un_geom_y);

  
  TAVECTOR2F un_full_size = send_un_geo * send_gp_size -1.0f;
  TAVECTOR2F un_half_size = un_full_size;
  un_half_size *= 0.5f;

  float full_eff_sig = full_gauss.sigma * un_half_size.x;
  if(full_eff_sig <= 0.0f)
    full_eff_sig = full_gauss.sigma;

  TAVECTOR2F gp_full_size = send_un_geo;
  gp_full_size -= 1.0f;
  TAVECTOR2F gp_half_size = gp_full_size;
  gp_half_size *= 0.5f;
  float gp_eff_sig = gp_gauss.sigma * gp_half_size.x;
  if(gp_eff_sig <= 0.0f)
    gp_eff_sig = gp_gauss.sigma;
  
  UNIT_STATE* ru = cg->OwnUnState(net);
  TAVECTOR2F ru_un_pos;
  ru_un_pos.SetXY(ru->pos_x, ru->pos_y);

  TAVECTOR2F ru_ctr = recv_un_geo;
  ru_ctr -= 1.0f;  ru_ctr *= 0.5f;
  TAVECTOR2F ru_nrm_pos = (ru_un_pos - ru_ctr) / ru_ctr;
  TAVECTOR2F ru_nrm_pos_full = ru_nrm_pos * full_gauss.ctr_mv;
  TAVECTOR2F ru_nrm_pos_gp = ru_nrm_pos * gp_gauss.ctr_mv;
  TAVECTOR2F s_un_ctr = (ru_nrm_pos_full * un_half_size) + un_half_size;
  TAVECTOR2F s_gp_ctr = (ru_nrm_pos_gp * gp_half_size) + gp_half_size;

  if(full_recv == BY_GROUP && recv_lay->HasUnitGroups()) {
    TAVECTOR2F ru_gp_geo;
    ru_gp_geo.SetXY(recv_lay->gp_geom_x-1, recv_lay->gp_geom_y-1);
    TAVECTOR2I ru_gp_pos;
    recv_lay->GetGpXYFmIdx(ru->gp_idx, ru_gp_pos.x, ru_gp_pos.y);
    TAVECTOR2F ru_gp_ctr = ru_gp_geo;
    ru_gp_ctr *= 0.5f;
    TAVECTOR2F ru_gp_nrm_pos = ((TAVECTOR2F)ru_gp_pos - ru_gp_ctr) / ru_gp_ctr;
    ru_nrm_pos_full = ru_gp_nrm_pos * full_gauss.ctr_mv;
    s_un_ctr = (ru_nrm_pos_full * un_half_size) + un_half_size;
  }
  if(full_recv == ALL_SAME) {
    s_un_ctr = un_half_size;
  }
  
  TAVECTOR2I unc, ugc;
  TAVECTOR2F suc;
  TAVECTOR2F sucw;
  TAVECTOR2F suncw;
  
  UNIT_STATE* su0 = cg->UnState(0, net);
  int st_gp_idx = su0->gp_idx;
  for(int i=0; i<cg->size; i++) {
    UNIT_STATE* su = cg->UnState(i, net); // using the unit coord allows it to work with random cons
    int ug_idx = su->gp_idx - st_gp_idx;
    int un_idx = su->ungp_un_idx;

    unc.x = un_idx % send_lay->un_geom_x;
    unc.y = un_idx / send_lay->un_geom_x;

    ugc.x = ug_idx % send_gp_size.x;
    ugc.y = ug_idx / send_gp_size.x;

    float fwt = 1.0f;
    if(full_gauss.on) {
      suc = ugc * send_un_geo;
      if(full_send == BY_UNIT) {
        suc += unc;
      }
      else {                    // by group
        suc += gp_half_size;
      }
      sucw = suc;
      // wrap coords around to get min dist from ctr either way
      if(full_gauss.wrap_wts) {
        if(fabs((sucw.x + un_full_size.x) - s_un_ctr.x) < fabs(sucw.x - s_un_ctr.x))
          sucw.x = sucw.x + un_full_size.x;
        else if(fabs((sucw.x - un_full_size.x) - s_un_ctr.x) < fabs(sucw.x - s_un_ctr.x))
          sucw.x = sucw.x - un_full_size.x;
        if(fabs((sucw.y + un_full_size.y) - s_un_ctr.y) < fabs(sucw.y - s_un_ctr.y))
          sucw.y = sucw.y + un_full_size.y;
        else if(fabs((sucw.y - un_full_size.y) - s_un_ctr.y) < fabs(sucw.y - s_un_ctr.y))
          sucw.y = sucw.y - un_full_size.y;
      }

      float dst = STATE_CLASS(taMath_float)::euc_dist(sucw.x, sucw.y, s_un_ctr.x, s_un_ctr.y);
      fwt = STATE_CLASS(taMath_float)::gauss_den_nonorm(dst, full_eff_sig);
    }

    float gwt = 1.0f;
    if(gp_gauss.on) {
      suncw = unc;
      if(gp_gauss.wrap_wts) {
        if(fabs((suncw.x + gp_full_size.x) - s_gp_ctr.x) < fabs(suncw.x - s_gp_ctr.x))
          suncw.x = suncw.x + gp_full_size.x;
        else if(fabs((suncw.x - gp_full_size.x) - s_gp_ctr.x) < fabs(suncw.x - s_gp_ctr.x))
          suncw.x = suncw.x - gp_full_size.x;
        if(fabs((suncw.y + gp_full_size.y) - s_gp_ctr.y) < fabs(suncw.y - s_gp_ctr.y))
          suncw.y = suncw.y + gp_full_size.y;
        else if(fabs((suncw.y - gp_full_size.y) - s_gp_ctr.y) < fabs(suncw.y - s_gp_ctr.y))
          suncw.y = suncw.y - gp_full_size.y;
      }
    
      float dst = STATE_CLASS(taMath_float)::euc_dist(suncw.x, suncw.y, s_gp_ctr.x, s_gp_ctr.y);
      gwt = STATE_CLASS(taMath_float)::gauss_den_nonorm(dst, gp_eff_sig);
    }

    float wt = fwt * gwt;
    wt = wt_range.min + wt_range.range * wt;

    SetCnWtScale(prjn, net, thr_no, cg, i, wt);
  }
}

void STATE_CLASS(TiledGpRFPrjnSpec)::Init_Weights_Sigmoid
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);

  TAVECTOR2I send_un_geo;
  send_un_geo.SetXY(send_lay->un_geom_x, send_lay->un_geom_y);
  TAVECTOR2I recv_un_geo;
  recv_un_geo.SetXY(recv_lay->un_geom_x, recv_lay->un_geom_y);

  TAVECTOR2F un_full_size = send_un_geo;
  un_full_size *= send_gp_size;
  TAVECTOR2F un_half_size = un_full_size;
  un_half_size *= 0.5f;

  float full_eff_gain = full_sig.gain * un_half_size.x;

  TAVECTOR2F gp_full_size = send_un_geo;
  TAVECTOR2F gp_half_size = gp_full_size;
  gp_half_size *= 0.5f;
  float gp_eff_gain = gp_sig.gain * gp_half_size.x;
  
  TAVECTOR2I ru_pos;
  UNIT_STATE* ru = cg->OwnUnState(net);
  ru_pos.SetXY(ru->pos_x, ru->pos_y);

  TAVECTOR2F ru_ctr = recv_un_geo;
  ru_ctr *= 0.5f;
  TAVECTOR2F ru_nrm_pos = ((TAVECTOR2F)ru_pos / ru_ctr);
  TAVECTOR2F sgn = 1.0f;
  TAVECTOR2F ru_nrm_pos_full;
  TAVECTOR2F ru_nrm_pos_gp;
  // su coords
  //  0  1  2   3    4  5  6  7 or - for flipped
  // .2 .4 .6. .8 | -.2 -.4 -.6 -.8
  if(ru_nrm_pos.x >= 1.0f) {
    ru_nrm_pos.x = -ru_nrm_pos.x + 1.0f;
    sgn.x = -1.0f;
    ru_nrm_pos_full.x = (ru_nrm_pos.x + 0.5f) * full_sig.ctr_mv - 0.5f;
    ru_nrm_pos_gp.x = (ru_nrm_pos.x + 0.5f) * gp_sig.ctr_mv - 0.5f;
  }
  else {
    ru_nrm_pos_full.x = (ru_nrm_pos.x - 0.5f) * full_sig.ctr_mv + 0.5f;
    ru_nrm_pos_gp.x = (ru_nrm_pos.x - 0.5f) * gp_sig.ctr_mv + 0.5f;
  }
  if(ru_nrm_pos.y >= 1.0f) {
    ru_nrm_pos.y = -ru_nrm_pos.y + 1.0f;
    sgn.y = -1.0f;
    ru_nrm_pos_full.y = (ru_nrm_pos.y + 0.5f) * full_sig.ctr_mv - 0.5f;
    ru_nrm_pos_gp.y = (ru_nrm_pos.y + 0.5f) * gp_sig.ctr_mv - 0.5f;
  }
  else {
    ru_nrm_pos_full.y = (ru_nrm_pos.y - 0.5f) * full_sig.ctr_mv + 0.5f;
    ru_nrm_pos_gp.y = (ru_nrm_pos.y - 0.5f) * gp_sig.ctr_mv + 0.5f;
  }

  TAVECTOR2F s_un_ctr = ru_nrm_pos_full * un_full_size;
  TAVECTOR2F s_gp_ctr = ru_nrm_pos_gp * gp_full_size;

  TAVECTOR2I unc, ugc;
  TAVECTOR2F suc;
  TAVECTOR2F sucw;
  TAVECTOR2F suncw;
  
  UNIT_STATE* su0 = cg->UnState(0, net);
  int st_gp_idx = su0->gp_idx;
  for(int i=0; i<cg->size; i++) {
    UNIT_STATE* su = cg->UnState(i, net); // using the unit coord allows it to work with random cons
    int ug_idx = su->gp_idx - st_gp_idx;
    int un_idx = su->ungp_un_idx;

    unc.x = un_idx % send_lay->un_geom_x;
    unc.y = un_idx / send_lay->un_geom_x;

    ugc.x = ug_idx % send_gp_size.x;
    ugc.y = ug_idx / send_gp_size.x;

    float fwt = 1.0f;
    if(full_sig.on) {
      suc = ugc * send_un_geo;
      suc += unc;
      float sig_x = STATE_CLASS(taMath_float)::logistic(sgn.x * suc.x, full_eff_gain, s_un_ctr.x);
      float sig_y = STATE_CLASS(taMath_float)::logistic(sgn.y * suc.y, full_eff_gain, s_un_ctr.y);
      fwt = sig_x * sig_y;
    }

    float gwt = 1.0f;
    if(gp_sig.on) {
      suncw = unc;
      
      float sig_x = STATE_CLASS(taMath_float)::logistic(sgn.x * unc.x, gp_eff_gain, s_gp_ctr.x);
      float sig_y = STATE_CLASS(taMath_float)::logistic(sgn.y * unc.y, gp_eff_gain, s_gp_ctr.y);
      gwt = sig_x * sig_y;
    }

    float wt = fwt * gwt;
    wt = wt_range.min + wt_range.range * wt;

    SetCnWtScale(prjn, net, thr_no, cg, i, wt);
  }
}
