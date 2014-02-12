// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "TiledRFPrjnSpec.h"
#include <Network>
#include <int_Array>

TA_BASEFUNS_CTORS_DEFN(TiledRFPrjnSpec);


void TiledRFPrjnSpec::Initialize() {
  recv_gp_border = 0;
  recv_gp_ex_st = -1;
  recv_gp_ex_n = 0;
  send_border = 0;
  send_adj_rfsz = 0;
  send_adj_sndloc = 0;
  rf_width_mult = 1.0f;
}

bool TiledRFPrjnSpec::InitRFSizes(Projection* prjn) {
  if(!(bool)prjn->from) return false;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return false;
  if(TestWarning(!prjn->layer->unit_groups, "InitRFSizes",
                 "requires recv layer to have unit groups!")) {
    return false;
  }

  ru_geo = prjn->layer->gp_geom;
  recv_gp_ed = ru_geo - recv_gp_border;
  recv_gp_ex_ed = recv_gp_ex_st + recv_gp_ex_n;

  su_act_geom = prjn->from->flat_geom;

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
  n_send_units = taVector2i(su_act_geom) - (2 * send_border);

  rf_ovlp.x = (int)floor(((float)(n_send_units.x + send_adj_rfsz.x) / (float)(n_recv_gps.x + 1)) + .5f);
  rf_ovlp.y = (int)floor(((float)(n_send_units.y + send_adj_rfsz.y) / (float)(n_recv_gps.y + 1)) + .5f);

  // how to move the receptive fields over the sending layer (floating point)
  rf_move = taVector2f(n_send_units + send_adj_sndloc) / taVector2f(n_recv_gps + 1);

  taVector2f rfw = (taVector2f)rf_ovlp * 2.0f * rf_width_mult;
  rf_width = rfw;
  return true;
}

void TiledRFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!InitRFSizes(prjn)) return;
  int n_cons = rf_width.Product();
  taVector2i ruc;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
      for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {

        if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
           (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;

        int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
        if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;

        taVector2i su_st;
        su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
        su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

        taVector2i su_ed = su_st + rf_width;

        for(int rui=0;rui < ru_nunits; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(n_cons, prjn);

          taVector2i suc;
          for(suc.y = su_st.y; suc.y < su_ed.y; suc.y++) {
            for(suc.x = su_st.x; suc.x < su_ed.x; suc.x++) {
              Unit* su_u = prjn->from->UnitAtCoord(suc);
              if(su_u == NULL) continue;
              if(!self_con && (su_u == ru_u)) continue;
              ru_u->ConnectFrom(su_u, prjn, alloc_loop);
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

int TiledRFPrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  if(!InitRFSizes(prjn)) return 0;
  int rval = 0;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int n_cons = rf_width.x * rf_width.y;
  int n_new_cons = (int)(p_add_con * (float)n_cons);
  if(n_new_cons <= 0) return 0;
  int_Array new_idxs;
  new_idxs.SetSize(n_cons);
  new_idxs.FillSeq();

  taVector2i ruc;
  for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
    for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {
      if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
         (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;

      int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
      if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;

      taVector2i su_st;
      su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
      su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);

        new_idxs.Permute();

        for(int i=0;i<n_new_cons;i++) {
          int su_idx = new_idxs[i];
          taVector2i suc;
          suc.y = su_idx / rf_width.x;
          suc.x = su_idx % rf_width.x;
          suc += su_st;
          Unit* su_u = prjn->from->UnitAtCoord(suc);
          if(su_u == NULL) continue;
          if(!self_con && (su_u == ru_u)) continue;
          int cn = ru_u->ConnectFromCk(su_u, prjn, false, true, init_wt); // gotta check!
          if(cn >= 0) {
            rval++;
          }
        }
      }
    }
  }
  return rval;
}

void TiledRFPrjnSpec::SelectRF(Projection* prjn) {
  if(!InitRFSizes(prjn)) return;
  // todo: fixme

  /*  Network* net = prjn->layer->own_net;
  if(net == NULL) return;
  NetView* nv = (NetView*)net->views.DefaultEl();
  if(nv == NULL) return;

  taBase_List* selgp = nv->GetSelectGroup();
  selgp->Reset();

  taVector2i ruc;
  for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
    for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {
      if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
         (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;
      Unit_Group* ru_gp = prjn->layer->UnitGpAtCoord(ruc);
      if(ru_gp == NULL) continue;

      selgp->LinkUnique(ru_gp);

      taVector2i su_st;
      su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
      su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

      taVector2i suc;
      for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
          Unit* su_u = prjn->from->UnitAtCoord(suc);
          if(su_u == NULL) continue;

          selgp->LinkUnique(su_u);
        }
      }
    }
  }
  nv->UpdateSelect();
  */
}

