// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(TiledGpRFOneToOnePrjnSpec)::Initialize_core() {
  gauss_sigma = 1.0f;
  su_idx_st = 0;
  ru_idx_st = 0;
  gp_n_cons = -1;
}

void STATE_CLASS(TiledGpRFOneToOnePrjnSpec)::Connect_Gps
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con, bool sym_same_lay,
   int make_cons, bool share_con, bool recip) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(recip) {
    recv_lay = send_lay;
    send_lay = prjn->GetRecvLayerState(net);
  }
  int ru_nunits = recv_lay->un_geom_n - ru_idx_st;
  int su_nunits = send_lay->un_geom_n - su_idx_st;
  int maxn = MIN(ru_nunits, su_nunits);
  if(gp_n_cons > 0)
    maxn = MIN(gp_n_cons, maxn);

  if(recip) {              // reciprocal is backwards!
    for(int ui=0; ui < maxn; ui++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, su_idx_st + ui);
      UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ru_idx_st + ui);
      if(!self_con && (su == ru)) continue;
      if(!make_cons) {
        su->RecvConsAllocInc(net, prjn, 1); // recip!
        ru->SendConsAllocInc(net, prjn, 1); // recip!
      }
      else {
        su->ConnectFrom(net, ru, prjn); // recip!
      }
    }
  }
  else {
    for(int ui=0; ui < maxn; ui++) {
      UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ru_idx_st + ui);
      if(share_con && net->RecvOwnsCons() && rgpidx > 0) {
        UNIT_STATE* shru = recv_lay->GetUnitStateGpUnIdx(net, 0, ru_idx_st + ui);
        ru->ShareRecvConsFrom(net, shru, prjn);
      }
      else {
        ru = recv_lay->GetUnitState(net, ru_idx_st + ui);
      }
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, su_idx_st + ui);
      if(!self_con && (su == ru)) continue;
      if(!make_cons) {
        ru->RecvConsAllocInc(net, prjn, 1);
        su->SendConsAllocInc(net, prjn, 1);
      }
      else {
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

void STATE_CLASS(TiledGpRFOneToOnePrjnSpec)::Init_Weights_Prjn
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {

  TAVECTOR2I rf_half_wd = send_gp_size / 2;
  TAVECTOR2F rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == send_gp_size) // even
    rf_ctr -= .5f;

  float sig_sq = (float)(rf_half_wd.x * rf_half_wd.x) * gauss_sigma * gauss_sigma;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % send_gp_size.x;
    int su_y = i / send_gp_size.x;

    float dst = STATE_CLASS(taMath_float)::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / sig_sq);

    SetCnWtScale(prjn, net, thr_no, cg, i, wt);
  }
}
