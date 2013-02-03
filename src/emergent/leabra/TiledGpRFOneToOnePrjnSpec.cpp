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

#include "TiledGpRFOneToOnePrjnSpec.h"
#include <Network>
#include <taMath_float>

void TiledGpRFOneToOnePrjnSpec::Initialize() {
  gauss_sigma = 1.0f;
  su_idx_st = 0;
  ru_idx_st = 0;
  gp_n_cons = -1;
}

void TiledGpRFOneToOnePrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                Layer* send_lay, int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n - ru_idx_st;
  int su_nunits = send_lay->un_geom.n - su_idx_st;
  int maxn = MIN(ru_nunits, su_nunits);
  if(gp_n_cons > 0)
    maxn = MIN(gp_n_cons, maxn);

  if(reciprocal) {              // reciprocal is backwards!
    for(int ui=0; ui < maxn; ui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(su_idx_st + ui, sgpidx);
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(ru_idx_st + ui, rgpidx);
      if(!self_con && (su_u == ru_u)) continue;
      su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
    }
  }
  else {
    for(int ui=0; ui < maxn; ui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(ru_idx_st + ui, rgpidx);
      Unit* su_u = send_lay->UnitAtUnGpIdx(su_idx_st + ui, sgpidx);
      if(!self_con && (su_u == ru_u)) continue;
      ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
    }
  }
}

void TiledGpRFOneToOnePrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init

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

    cg->Cn(i)->wt = wt;
  }
}
