// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(TiledNovlpPrjnSpec)::Initialize_core() {
  reciprocal = false;
}

bool STATE_CLASS(TiledNovlpPrjnSpec)::InitRFSizes(PRJN_STATE* prjn, NETWORK_STATE* net) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);

  if(reciprocal) {
    recv_lay = send_lay;
    send_lay = prjn->GetRecvLayer(net);
  }
  
  if(!recv_lay->HasUnitGroups()) {
    net->StateError("TiledNovlpPrjnSpec::InitRFSizes requires effective recv layer to have unit groups!");
    return false;
  }

  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);

  su_act_geom.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_y);

  rf_width.x = (float)su_act_geom.x / (float)ru_geo.x;
  rf_width.y = (float)su_act_geom.y / (float)ru_geo.y;

  return true;
}

void STATE_CLASS(TiledNovlpPrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {

  if(!InitRFSizes(prjn, net)) return;

  if(reciprocal) {
    Connect_Reciprocal(prjn, net, make_cons);
    return;
  }

  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);

  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  TAVECTOR2I ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      TAVECTOR2I su_st;
      su_st.x = (int)((float)ruc.x * rf_width.x);
      su_st.y = (int)((float)ruc.y * rf_width.y);

      for(int rui=0; rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        int alloc_sz = ((int)(rf_width.x) + 1) * ((int)(rf_width.y) + 1);
        if(!make_cons) {
          ru->RecvConsPreAlloc(net, prjn, alloc_sz);
        }

        TAVECTOR2I suc;
        for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
            UNIT_STATE* su = send_lay->GetUnitStateFlatXY(net, suc.x, suc.y);
            if(su == NULL) continue;

            if(!self_con && (su == ru)) continue;
            ru->ConnectFrom(net, su, prjn, !make_cons);
          }
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    send_lay->SendConsPostAlloc(net, prjn);
  }
}

void STATE_CLASS(TiledNovlpPrjnSpec)::Connect_Reciprocal
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {

  LAYER_STATE* recv_lay = prjn->GetSendLayer(net); // recip
  LAYER_STATE* send_lay = prjn->GetRecvLayer(net); // recip

  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_y); // not su_act_geom due to recip
  
  TAVECTOR2I ruc;
  if(!make_cons) {
    // todo: couldn't we just use the basic allocinc for following??
    int n_send_tot = su_geo.Product();
    int* alloc_sz = new int[n_send_tot];
    IntArrayInitVals(alloc_sz, n_send_tot, 0);

    // find alloc sizes
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        TAVECTOR2I su_st;
        su_st.x = (int)((float)ruc.x * rf_width.x);
        su_st.y = (int)((float)ruc.y * rf_width.y);

        TAVECTOR2I suc;
        for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
            UNIT_STATE* su = send_lay->GetUnitStateFlatXY(net, suc.x, suc.y);
            if(su == NULL) continue;

            int sugp_idx = suc.y * su_geo.x + suc.x;
            alloc_sz[sugp_idx] += ru_nunits;
          }
        }
      }
    }

    // do the alloc
    TAVECTOR2I suc;
    for(suc.y = 0; suc.y < su_geo.y; suc.y++) {
      for(suc.x = 0; suc.x < su_geo.x; suc.x++) {
        UNIT_STATE* su = send_lay->GetUnitStateFlatXY(net, suc.x, suc.y);
        if(su == NULL) continue;
        int sugp_idx = suc.y * su_geo.x + suc.x;
        su->RecvConsPreAlloc(net, prjn, alloc_sz[sugp_idx]);
      }
    }

    delete [] alloc_sz;
  }

  // then make the connections!
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      TAVECTOR2I su_st;
      su_st.x = (int)((float)ruc.x * rf_width.x);
      su_st.y = (int)((float)ruc.y * rf_width.y);

      TAVECTOR2I suc;
      for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
          UNIT_STATE* su = send_lay->GetUnitStateFlatXY(net, suc.x, suc.y);
          if(su == NULL) continue;

          for(int rui=0; rui < ru_nunits; rui++) {
            UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
            if(!self_con && (su == ru)) continue;
            su->ConnectFrom(net, ru, prjn, !make_cons); // recip
          }
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    recv_lay->SendConsPostAlloc(net, prjn); // reversed..
  }
}


