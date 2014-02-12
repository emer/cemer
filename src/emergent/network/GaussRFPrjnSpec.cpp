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

#include "GaussRFPrjnSpec.h"
#include <Network>
#include <taMath_float>

TA_BASEFUNS_CTORS_DEFN(GaussRFPrjnSpec);

void GaussRFPrjnSpec::Initialize() {
  init_wts = true;
  rf_width = 6;
  rf_move = 3.0f;
  wrap = false;
  gauss_sigma = 1.0f;
}

void GaussRFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

  int n_cons = rf_width.Product();
  taVector2i rf_half_wd = rf_width / 2;
  taVector2i rug_geo = prjn->layer->flat_geom; // uses flat geom -- not ug based
  taVector2i su_geo = prjn->from->flat_geom;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(ruc.y = 0; ruc.y < rug_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < rug_geo.x; ruc.x++) {
        Unit* ru_u = prjn->layer->UnitAtCoord(ruc);
        if(!ru_u) continue;

        if(!alloc_loop)
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
            Unit* su_u = prjn->from->UnitAtCoord(suc_wrp);
            if(su_u == NULL) continue;
            if(!self_con && (su_u == ru_u)) continue;

            ru_u->ConnectFrom(su_u, prjn, alloc_loop); // don't check: saves lots of time!
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void GaussRFPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init

  Network* net = prjn->layer->own_net;

  taVector2i rf_half_wd = rf_width / 2;
  taVector2f rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == rf_width) // even
    rf_ctr -= .5f;

  float sig_sq = gauss_sigma * gauss_sigma;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % rf_width.x;
    int su_y = i / rf_width.x;

    float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / sig_sq);

    cg->Cn(i,BaseCons::WT,net) = wt;
  }
}

bool GaussRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
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

bool GaussRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
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



