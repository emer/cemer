// Copyright 2017, Regents of the University of Colorado,
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

#include "CerebConj2PrjnSpec.h"
#include <Network>
#include <taMath_float>

TA_BASEFUNS_CTORS_DEFN(CerebConj2PrjnSpec);

void CerebConj2PrjnSpec::Initialize() {
  init_wts = true;
  rf_width = 6;
  rf_move = 3.0f;
  wrap = false;
  gauss_sigma = 1.0f;
}

void CerebConj2PrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(prjn->recv_idx == 0)
    Connect_Outer(prjn, make_cons);
  else
    Connect_Inner(prjn, make_cons);
}

void CerebConj2PrjnSpec::Connect_Outer(Projection* prjn, bool make_cons) {
  int n_cons = rf_width.Product();
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  taVector2i rf_half_wd = rf_width / 2;
  taVector2i rug_geo = recv_lay->gp_geom;
  taVector2i run_geo = recv_lay->un_geom;
  taVector2i su_geo = send_lay->flat_geom;

  taVector2i ruc;
  for(ruc.y = 0; ruc.y < rug_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < rug_geo.x; ruc.x++) {
      int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
      if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;

      taVector2i su_st;
      if(wrap) {
        su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
        su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
      }
      else {
        su_st.x = (int)floor((float)ruc.x * rf_move.x);
        su_st.y = (int)floor((float)ruc.y * rf_move.y);
      }

      su_st.WrapClip(wrap, su_geo);
      taVector2i su_ed = su_st + rf_width;
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

      for(int rui=0; rui<recv_lay->un_geom.n; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(!make_cons)
          ru_u->RecvConsPreAlloc(n_cons, prjn);

        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
          for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
            suc_wrp = su_st + suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            Unit* su_u = send_lay->UnitAtCoord(suc_wrp);
            if(!su_u) continue;
            if(!self_con && (su_u == ru_u)) continue;

            ru_u->ConnectFrom(su_u, prjn, !make_cons); // don't check: saves lots of time!
          }
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    send_lay->SendConsPostAlloc(prjn);
  }
}

void CerebConj2PrjnSpec::Connect_Inner(Projection* prjn, bool make_cons) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(!(bool)send_lay)   return;
  if(recv_lay->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!recv_lay->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }

  int n_cons = rf_width.Product();
  taVector2i rf_half_wd = rf_width / 2;
  taVector2i rug_geo = recv_lay->gp_geom;
  taVector2i run_geo = recv_lay->un_geom;
  taVector2i su_geo = send_lay->flat_geom;

  for(int rug=0;rug<recv_lay->gp_geom.n;rug++) {
    taVector2i ruc;
    for(ruc.y = 0; ruc.y < run_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < run_geo.x; ruc.x++) {
        int rui = ruc.y * run_geo.x + ruc.x;
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rug);
        if(!ru_u) continue;
        if(!make_cons)
          ru_u->RecvConsPreAlloc(n_cons, prjn);

        taVector2i su_st;
        if(wrap) {
          su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
          su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
        }
        else {
          su_st.x = (int)floor((float)ruc.x * rf_move.x);
          su_st.y = (int)floor((float)ruc.y * rf_move.y);
        }

        su_st.WrapClip(wrap, su_geo);
        taVector2i su_ed = su_st + rf_width;
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

        taVector2i suc;
        taVector2i suc_wrp;
        for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
          for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
            suc_wrp = su_st + suc;
            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
              continue;
            Unit* su_u = send_lay->UnitAtCoord(suc_wrp);
            if(su_u == NULL) continue;
            if(!self_con && (su_u == ru_u)) continue;
            ru_u->ConnectFrom(su_u, prjn, !make_cons); // don't check: saves lots of time!
          }
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    send_lay->SendConsPostAlloc(prjn);
  }
}

void CerebConj2PrjnSpec::Init_Weights_Prjn(Projection* prjn, ConState_cpp* cg,
                                           Network* net, int thr_no) {
  taVector2i rf_half_wd = rf_width / 2;
  taVector2f rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == rf_width) // even
    rf_ctr -= .5f;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % rf_width.x;
    int su_y = i / rf_width.x;

    float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / (gauss_sigma * gauss_sigma));

    SetCnWtScale(prjn, net, thr_no, cg, i, wt);
  }
}

bool CerebConj2PrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom +1) * rf_move);

  SigEmitUpdated();
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool CerebConj2PrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom+1) * rf_move);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  SigEmitUpdated();
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}


