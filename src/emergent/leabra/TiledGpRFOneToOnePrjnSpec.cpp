// Copyright 2016, Regents of the University of Colorado,
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

#include "TiledGpRFOneToOnePrjnSpec.h"
#include <Network>
#include <taMath_float>

TA_BASEFUNS_CTORS_DEFN(TiledGpRFOneToOnePrjnSpec);

void TiledGpRFOneToOnePrjnSpec::Initialize() {
  gauss_sigma = 1.0f;
  su_idx_st = 0;
  ru_idx_st = 0;
  gp_n_cons = -1;
}

void TiledGpRFOneToOnePrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                Layer* send_lay, int rgpidx, int sgpidx, bool make_cons) {
  int ru_nunits = recv_lay->un_geom.n - ru_idx_st;
  int su_nunits = send_lay->un_geom.n - su_idx_st;
  int maxn = MIN(ru_nunits, su_nunits);
  if(gp_n_cons > 0)
    maxn = MIN(gp_n_cons, maxn);

  Network* net = recv_lay->own_net;

  if(reciprocal) {              // reciprocal is backwards!
    for(int ui=0; ui < maxn; ui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(su_idx_st + ui, sgpidx);
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(ru_idx_st + ui, rgpidx);
      if(!self_con && (su_u == ru_u)) continue;
      if(!make_cons) {
        su_u->RecvConsAllocInc(1, prjn); // recip!
        ru_u->SendConsAllocInc(1, prjn); // recip!
      }
      else {
        su_u->ConnectFrom(ru_u, prjn); // recip!
      }
    }
  }
  else {
    for(int ui=0; ui < maxn; ui++) {
      Unit* ru_u;
      if(rgpidx >= 0) {
        ru_u = recv_lay->UnitAtUnGpIdx(ru_idx_st + ui, rgpidx);
        if(share_cons && net->RecvOwnsCons() && rgpidx > 0) {
          Unit* shru = recv_lay->UnitAtUnGpIdx(ru_idx_st + ui, 0); // group 0
          ru_u->ShareRecvConsFrom(shru, prjn);
        }
      }
      else {
        ru_u = recv_lay->units.SafeEl(ru_idx_st + ui);
      }
      Unit* su_u = send_lay->UnitAtUnGpIdx(su_idx_st + ui, sgpidx);
      if(!self_con && (su_u == ru_u)) continue;
      if(!make_cons) {
        ru_u->RecvConsAllocInc(1, prjn);
        su_u->SendConsAllocInc(1, prjn);
      }
      else {
        ru_u->ConnectFrom(su_u, prjn);
      }
    }
  }
}

void TiledGpRFOneToOnePrjnSpec::Init_Weights_Prjn
(Projection* prjn, ConGroup* cg, Network* net, int thr_no) {
  taVector2i rf_half_wd = send_gp_size / 2;
  taVector2f rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == send_gp_size) // even
    rf_ctr -= .5f;

  float sig_sq = (float)(rf_half_wd.x * rf_half_wd.x) * gauss_sigma * gauss_sigma;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % send_gp_size.x;
    int su_y = i / send_gp_size.x;

    float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / sig_sq);

    if(set_scale) {
      SetCnWtRnd(cg, i, net, thr_no);
      SetCnScale(wt, cg, i, net, thr_no);
    }
    else {
      SetCnWt(wt, cg, i, net, thr_no);
    }
  }
}
