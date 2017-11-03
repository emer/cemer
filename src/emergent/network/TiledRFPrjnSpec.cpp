// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(TiledRFPrjnSpec)::Initialize_core() {
  recv_gp_border = 0;
  recv_gp_ex_st = -1;
  recv_gp_ex_n = 0;
  send_border = 0;
  send_adj_rfsz = 0;
  send_adj_sndloc = 0;
  rf_width_mult = 1.0f;
}

bool STATE_CLASS(TiledRFPrjnSpec)::InitRFSizes(PRJN_STATE* prjn, NETWORK_STATE* net) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  if(!recv_lay->HasUnitGroups()) {
    net->StateError("TiledRFPrjnSpec::InitRFSizes requires recv layer to have unit groups!");
    return false;
  }

  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  recv_gp_ed = ru_geo - recv_gp_border;
  recv_gp_ex_ed = recv_gp_ex_st + recv_gp_ex_n;

  su_act_geom.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_y);

// 0 1 2 3 4 5 6 7 8 9 a b c d e = 14+1 = 15
// 0 0 0 0 0 0
//       1 1 1 1 1 1
//             2 2 2 2 2 2
//                   3 3 3 3 3 3
//
// 4 gps, ovlp = 3, width = 6
// ovlp = tot / (nr + 1) = 15 / 5 = 3

// send scale:
// ovlp / recv_gp size

// send off:
// - recv off * scale

  n_recv_gps = ru_geo - (2 * recv_gp_border);   // total number of recv groups covered
  n_send_units = TAVECTOR2I(su_act_geom) - (2 * send_border);

  rf_ovlp.x = (int)floor(((float)(n_send_units.x + send_adj_rfsz.x) / (float)(n_recv_gps.x + 1)) + .5f);
  rf_ovlp.y = (int)floor(((float)(n_send_units.y + send_adj_rfsz.y) / (float)(n_recv_gps.y + 1)) + .5f);

  // how to move the receptive fields over the sending layer (floating point)
  rf_move = TAVECTOR2F(n_send_units + send_adj_sndloc) / TAVECTOR2F(n_recv_gps + 1);

  TAVECTOR2F rfw = (TAVECTOR2F)rf_ovlp * 2.0f * rf_width_mult;
  rf_width = rfw;
  return true;
}

void STATE_CLASS(TiledRFPrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {

  if(!InitRFSizes(prjn, net)) return;

  int n_cons = rf_width.Product();

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  TAVECTOR2I ruc;
  for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
    for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {

      if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
         (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;

      int rgpidx = recv_lay->GetGpIdxFmXY(ruc.x, ruc.y);
      if(!recv_lay->GpIdxInRange(rgpidx)) continue;

      TAVECTOR2I su_st;
      su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
      su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

      TAVECTOR2I su_ed = su_st + rf_width;

      for(int rui=0;rui < ru_nunits; rui++) {
        UNIT_STATE* ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, rui);
        if(ru->lesioned()) continue;
        if(!make_cons) {
          ru->RecvConsPreAlloc(net, prjn, n_cons);
        }

        TAVECTOR2I suc;
        for(suc.y = su_st.y; suc.y < su_ed.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_ed.x; suc.x++) {
            UNIT_STATE* su = send_lay->GetUnitStateFlatXY(net, suc.x, suc.y);
            if(su == NULL) continue;
            if(su->lesioned()) continue;
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

// TODO: later..

// int STATE_CLASS(TiledRFPrjnSpec)::ProbAddCons_impl
// (PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt) {
//   if(!InitRFSizes(prjn)) return 0;
//   int rval = 0;

  // LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  // LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
//   int ru_nunits = recv_lay->un_geom_n;
//   int su_nunits = send_lay->un_geom_n;

//   int n_cons = rf_width.x * rf_width.y;
//   int n_new_cons = (int)(p_add_con * (float)n_cons);
//   if(n_new_cons <= 0) return 0;
//   int_Array new_idxs;
//   new_idxs.SetSize(n_cons);
//   new_idxs.FillSeq();

//   TAVECTOR2I ruc;
//   for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
//     for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {
//       if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
//          (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;

//       int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
//       if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;

//       TAVECTOR2I su_st;
//       su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
//       su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

//       for(int rui=0; rui < ru_nunits; rui++) {
//         Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);

//         new_idxs.Permute();

//         for(int i=0;i<n_new_cons;i++) {
//           int su_idx = new_idxs[i];
//           TAVECTOR2I suc;
//           suc.y = su_idx / rf_width.x;
//           suc.x = su_idx % rf_width.x;
//           suc += su_st;
//           Unit* su = send_lay->UnitAtCoord(suc);
//           if(su == NULL) continue;
//           if(!self_con && (su == ru)) continue;
//           int cn = ru->ConnectFromCk(su, prjn, false, true, init_wt); // gotta check!
//           if(cn >= 0) {
//             rval++;
//           }
//         }
//       }
//     }
//   }
//   return rval;
// }


