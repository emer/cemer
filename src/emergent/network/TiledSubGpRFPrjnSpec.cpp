// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(TiledSubGpRFPrjnSpec)::Initialize_core() {
  send_gp_size = 4;
  send_gp_skip = 2;
  send_gp_start = 0;
  send_subgp_size = 2;
  recv_subgp_size = 2;
  wrap = false;
  reciprocal = false;
  share_cons = false;
  wts_type = GAUSSIAN;
  gauss_sig = 1.0f;
  gauss_ctr_mv = 0.5f;
  wt_range.min = 0.1f;
  wt_range.max = 0.9f;
  p_high = 0.25f;
}

void STATE_CLASS(TiledSubGpRFPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(reciprocal) {
    recv_lay = send_lay;
    send_lay = prjn->GetRecvLayerState(net);
  }

  if(!recv_lay->HasUnitGroups()) {
    net->StateError("TiledSubGpRFPrjnSpec::Connect_impl requires recv layer to have unit groups!");
    return;
  }
  if(!send_lay->HasUnitGroups()) {
    net->StateError("TiledSubGpRFPrjnSpec::Connect_impl requires send layer to have unit groups!");
    return;
  }

  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->gp_geom_x, send_lay->gp_geom_y);
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  if((ru_geo.x % recv_subgp_size.x != 0) || (ru_geo.y % recv_subgp_size.y != 0)) {
    net->StateError("TiledSubGpRFPrjnSpec::Connect_impl recv layer group geometry must be an even multiple of recv_subgp_size");
    return;
  }
  if((su_geo.x % send_subgp_size.x != 0) || (su_geo.y % send_subgp_size.y != 0)) {
    net->StateError("TiledSubGpRFPrjnSpec::Connect_impl send layer group geometry must be an even multiple of send_subgp_size");
    return;
  }

  // topographic receptive field geometry -- rf suffix means in receptive field coords
  TAVECTOR2I ru_geo_rf = ru_geo / recv_subgp_size;
  TAVECTOR2I su_geo_rf = su_geo / send_subgp_size;

  int sg_sz_tot = send_gp_size.Product();
  int alloc_no = sg_sz_tot * su_nunits; // todo what happens with != subgp sizes??

  TAVECTOR2I ruc_rf;            // first loop over outer receptive field coords
  for(ruc_rf.y = 0; ruc_rf.y < ru_geo_rf.y; ruc_rf.y++) {
    for(ruc_rf.x = 0; ruc_rf.x < ru_geo_rf.x; ruc_rf.x++) {
      TAVECTOR2I su_st_rf = send_gp_start + ruc_rf * send_gp_skip;

      TAVECTOR2I ruc_sg;        // then sub-group coords within
      for(ruc_sg.y = 0; ruc_sg.y < recv_subgp_size.y; ruc_sg.y++) {
        for(ruc_sg.x = 0; ruc_sg.x < recv_subgp_size.x; ruc_sg.x++) {
          TAVECTOR2I ruc = ruc_rf * recv_subgp_size + ruc_sg;
          int rgpidx = recv_lay->GetGpIdxFmXY(ruc.x, ruc.y);
          
          TAVECTOR2I suc_rf;
          TAVECTOR2I suc_wrp;
          for(suc_rf.y = su_st_rf.y; suc_rf.y < su_st_rf.y + send_gp_size.y; suc_rf.y++) {
            for(suc_rf.x = su_st_rf.x; suc_rf.x < su_st_rf.x + send_gp_size.x; suc_rf.x++) {
              TAVECTOR2I suc = (suc_rf * send_subgp_size);
              if(send_subgp_size == recv_subgp_size)
                suc += ruc_sg;
              // + ((send_gp_size * ruc_sg) / recv_subgp_size);

              suc_wrp = suc;
              if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
                continue;

              int sgpidx = send_lay->GetGpIdxFmXY(suc_wrp.x, suc_wrp.y);
              if(!send_lay->GpIdxInRange(sgpidx)) continue;

              Connect_UnitGroupRF(prjn, net, recv_lay, send_lay, rgpidx, sgpidx,
                                  make_cons, share_cons, reciprocal);
            }
          }
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->GetRecvLayerState(net)->RecvConsPostAlloc(net, prjn);
    prjn->GetSendLayerState(net)->SendConsPostAlloc(net, prjn);
  }
}

void STATE_CLASS(TiledSubGpRFPrjnSpec)::Init_Weights_Prjn
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  if(wts_type == GAUSSIAN) {
    Init_Weights_Gaussian(prjn, net, thr_no, cg);
  }
}

void STATE_CLASS(TiledSubGpRFPrjnSpec)::Init_Weights_Gaussian
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  // todo: need to fix for reciprocal?
  
  TAVECTOR2I send_un_geo;
  send_un_geo.SetXY(send_lay->un_geom_x, send_lay->un_geom_y);
  TAVECTOR2I recv_un_geo;
  recv_un_geo.SetXY(recv_lay->un_geom_x, recv_lay->un_geom_y);

  TAVECTOR2I full_size = send_un_geo * send_gp_size;
  TAVECTOR2F half_size = full_size;
  half_size *= 0.5f;

  float eff_sig = gauss_sig * (float)half_size.x;
  
  UNIT_STATE* ru = cg->OwnUnState(net);
  TAVECTOR2F ru_pos;
  ru_pos.SetXY(ru->pos_x, ru->pos_y);

  TAVECTOR2F rugpctr = recv_un_geo;
  rugpctr *= 0.5f;
  TAVECTOR2F ru_nrm_pos = gauss_ctr_mv * (((TAVECTOR2F)ru_pos - rugpctr) / rugpctr);

  TAVECTOR2F s_ctr = (ru_nrm_pos * half_size) + half_size;
  
  for(int i=0; i<cg->size; i++) {
    // note: these are organized within unit group first, then by groups
    int ug_idx = i / send_lay->un_geom_n; // which unit group, ordinally
    int un_idx = i % send_lay->un_geom_n; // index in unit group

    int un_x = un_idx % send_lay->un_geom_x;
    int un_y = un_idx / send_lay->un_geom_x;

    int ug_x = ug_idx % send_gp_size.x;
    int ug_y = ug_idx / send_gp_size.x;

    int su_x = ug_x * send_lay->un_geom_x + un_x;
    int su_y = ug_y * send_lay->un_geom_y + un_y;

    float dst = STATE_CLASS(taMath_float)::euc_dist(su_x, su_y, s_ctr.x, s_ctr.y);
    float wt = STATE_CLASS(taMath_float)::gauss_den_nonorm(dst, eff_sig);
    wt = wt_range.min + wt_range.range * wt;

    if(set_scale) {
      SetCnWtRnd(prjn, net, thr_no, cg, i);
      SetCnScale(prjn, net, thr_no, cg, i, wt);
    }
    else {
      SetCnWt(prjn, net, thr_no, cg, i, wt);
    }
  }
}

