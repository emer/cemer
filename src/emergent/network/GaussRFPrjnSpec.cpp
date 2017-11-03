// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(GaussRFPrjnSpec)::Initialize_core() {
  init_wts = true;
  rf_width = 6;
  rf_move = 3.0f;
  wrap = false;
  gauss_sigma = 1.0f;
}

void STATE_CLASS(GaussRFPrjnSpec)::Connect_impl
(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int n_cons = rf_width.Product();
  TAVECTOR2I rf_half_wd = rf_width / 2;

  TAVECTOR2I ru_geo; // uses flat geom -- not ug based
  ru_geo.SetXY(recv_lay->flat_geom_x, recv_lay->flat_geom_y);
  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_y);

  TAVECTOR2I ruc;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++) {
      UNIT_STATE* ru = recv_lay->GetUnitStateFlatXY(net, ruc.x, ruc.y);
      if(!ru || ru->lesioned()) continue;

      if(!make_cons) {
        ru->RecvConsPreAlloc(net, prjn, n_cons);
      }

      TAVECTOR2I su_st;
      if(wrap) {
        su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
        su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
      }
      else {
        su_st.x = (int)floor((float)ruc.x * rf_move.x);
        su_st.y = (int)floor((float)ruc.y * rf_move.y);
      }

      su_st.WrapClip(wrap, su_geo);
      TAVECTOR2I su_ed = su_st + rf_width;
      if(wrap) {
        su_ed.WrapClip(wrap, su_geo); // just wrap ends too
      }
      else {
        if(su_ed.x > su_geo.x) {
          su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
        }
        if(su_ed.y > su_geo.y) {
          su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
        }
      }

      TAVECTOR2I suc;
      TAVECTOR2I suc_wrp;
      for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
        for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
          suc_wrp = su_st + suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          UNIT_STATE* su = send_lay->GetUnitStateFlatXY(net, suc_wrp.x, suc_wrp.y);
          if(su == NULL) continue;
          if(!self_con && (su == ru)) continue;

          ru->ConnectFrom(net, su, prjn, !make_cons); // don't check: saves lots of time!
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    send_lay->SendConsPostAlloc(net, prjn);
  }
}

void STATE_CLASS(GaussRFPrjnSpec)::Init_Weights_Prjn
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no,  CON_STATE* cg) {
  TAVECTOR2I rf_half_wd = rf_width / 2;
  TAVECTOR2F rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == rf_width) // even
    rf_ctr -= .5f;

  float sig_sq = gauss_sigma * gauss_sigma;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % rf_width.x;
    int su_y = i / rf_width.x;

    float dst = STATE_CLASS(taMath_float)::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / sig_sq);

    if(set_scale) {
      SetCnWtRnd(prjn, net, thr_no, cg, i);
      SetCnScale(prjn, net, thr_no, cg, i, wt);
    }
    else {
      SetCnWt(prjn, net, thr_no, cg, i, wt);
    }
  }
}

