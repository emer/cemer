// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(TiledGpMapConvergePrjnSpec)::Initialize_core() {
  send_tile_size = 4;
  send_tile_skip = 2;
  wrap = false;
  reciprocal = false;
}

void STATE_CLASS(TiledGpMapConvergePrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  if(!recv_lay->HasUnitGroups()) {
    net->StateError("TiledSubGpRFPrjnSpec::Connect_impl requires recv layer to have unit groups!");
    return;
  }
  if(!send_lay->HasUnitGroups()) {
    net->StateError("TiledSubGpRFPrjnSpec::Connect_impl requires send layer to have unit groups!");
    return;
  }

  if(reciprocal) {
    Connect_Reciprocal(prjn, net, make_cons);
    return;
  }

  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->un_geom_x, send_lay->un_geom_y); // un_geom!
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  int su_ngps = send_lay->gp_geom_n;

  int sg_sz_tot = send_tile_size.Product();
  int alloc_no = sg_sz_tot * su_ngps;

  TAVECTOR2I ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      TAVECTOR2I su_st;
      if(wrap)        su_st = (ruc-1) * send_tile_skip;
      else            su_st = ruc * send_tile_skip;

      if(!make_cons) {
        for(int rui=0; rui < ru_nunits; rui++) {
          UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
          ru->RecvConsPreAlloc(net, prjn, alloc_no);
        }
      }

      TAVECTOR2I suc;
      TAVECTOR2I suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_tile_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_tile_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int suidx = send_lay->GetUnIdxFmXY(suc_wrp.x, suc_wrp.y);
          if(!send_lay->UnIdxInRange(suidx)) continue;

          Connect_UnitGroup(prjn, net, recv_lay, send_lay, rgpidx, suidx, make_cons);
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    send_lay->SendConsPostAlloc(net, prjn);
  }
}

void STATE_CLASS(TiledGpMapConvergePrjnSpec)::Connect_Reciprocal
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {

  LAYER_STATE* recv_lay = prjn->GetSendLayerState(net); // recip
  LAYER_STATE* send_lay = prjn->GetRecvLayerState(net); // recip

  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->un_geom_x, send_lay->un_geom_y);
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  int su_ngps = send_lay->gp_geom_n;

  TAVECTOR2I ruc;
  if(!make_cons) {
    // todo: couldn't we just use the basic allocinc for following??
    int n_send_tot = su_geo.Product();
    int* alloc_sz = new int[n_send_tot];
    IntArrayInitVals(alloc_sz, n_send_tot, 0);

    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        TAVECTOR2I su_st;
        if(wrap)  su_st = (ruc-1) * send_tile_skip;
        else      su_st = ruc * send_tile_skip;

        TAVECTOR2I suc;
        TAVECTOR2I suc_wrp;
        for(suc.y = su_st.y; suc.y < su_st.y + send_tile_size.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + send_tile_size.x; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            int suidx = send_lay->GetUnIdxFmXY(suc_wrp.x, suc_wrp.y);
            if(!send_lay->UnIdxInRange(suidx)) continue;
            alloc_sz[suidx] += ru_nunits;
          }
        }
      }
    }

    // now actually allocate
    for(int sug=0; sug < send_lay->gp_geom_n; sug++) {
      for(int sui=0; sui < su_nunits; sui++) {
        UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sug, sui);
        su->RecvConsPreAlloc(net, prjn, alloc_sz[sui]);
      }
    }

    delete [] alloc_sz;
  }

  // then connect
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      TAVECTOR2I su_st;
      if(wrap)        su_st = (ruc-1) * send_tile_skip;
      else            su_st = ruc * send_tile_skip;

      TAVECTOR2I suc;
      TAVECTOR2I suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_tile_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_tile_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int suidx = send_lay->GetUnIdxFmXY(suc_wrp.x, suc_wrp.y);
          if(!send_lay->UnIdxInRange(suidx)) continue;

          Connect_UnitGroup(prjn, net, recv_lay, send_lay, rgpidx, suidx, make_cons);
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->GetSendLayerState(net)->SendConsPostAlloc(net, prjn); // recip
  }
}

void STATE_CLASS(TiledGpMapConvergePrjnSpec)::Connect_UnitGroup
(PRJN_STATE* prjn, NETWORK_STATE* net, LAYER_STATE* recv_lay, LAYER_STATE* send_lay,
 int rgpidx, int suidx, int make_cons) {
  int ru_nunits = recv_lay->un_geom_n;
  int su_ngps = send_lay->gp_geom_n;

  if(reciprocal) {              // reciprocal is backwards!
    for(int sgpi=0; sgpi < su_ngps; sgpi++) {
      UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpi, suidx);
      for(int rui=0; rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        if(!self_con && (su == ru)) continue;
        su->ConnectFrom(net, ru, prjn, !make_cons); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
      for(int sgpi=0; sgpi < su_ngps; sgpi++) {
        UNIT_STATE* su = send_lay->GetUnitStateGpUnIdx(net, sgpi, suidx);
        if(!self_con && (su == ru)) continue;
        ru->ConnectFrom(net, su, prjn, !make_cons);
      }
    }
  }
}

// int TiledGpMapConvergePrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
//   // todo: needs impl
//   return -1;
// }

