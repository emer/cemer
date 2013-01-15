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

#include "TiledNovlpPrjnSpec.h"
#include <Network>

void TiledNovlpPrjnSpec::Initialize() {
  reciprocal = false;
}

bool TiledNovlpPrjnSpec::InitRFSizes(Projection* prjn) {
  if(!(bool)prjn->from) return false;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return false;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }

  if(TestWarning(!recv_lay->unit_groups, "InitRFSizes",
                 "requires recv layer to have unit groups!")) {
    return false;
  }

  ru_geo = recv_lay->gp_geom;

  su_act_geom = send_lay->flat_geom;

  rf_width.x = (float)su_act_geom.x / (float)ru_geo.x;
  rf_width.y = (float)su_act_geom.y / (float)ru_geo.y;

  return true;
}

void TiledNovlpPrjnSpec::Connect_impl(Projection* prjn) {
  if(!InitRFSizes(prjn)) return;

  if(reciprocal) {
    Connect_Reciprocal(prjn);
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  taVector2i ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        su_st.x = (int)((float)ruc.x * rf_width.x);
        su_st.y = (int)((float)ruc.y * rf_width.y);

        for(int rui=0; rui < ru_nunits; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          int alloc_sz = ((int)(rf_width.x) + 1) * ((int)(rf_width.y) + 1);
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(alloc_sz, prjn);

          taVector2i suc;
          for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
            for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
              Unit* su_u = send_lay->UnitAtCoord(suc);
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

void TiledNovlpPrjnSpec::Connect_Reciprocal(Projection* prjn) {
  Layer* recv_lay = prjn->from;
  Layer* send_lay = prjn->layer;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int_Array alloc_sz;
  alloc_sz.SetSize(send_lay->flat_geom.Product());
  alloc_sz.InitVals(0);

  // find alloc sizes
  taVector2i ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      taVector2i su_st;
      su_st.x = (int)((float)ruc.x * rf_width.x);
      su_st.y = (int)((float)ruc.y * rf_width.y);

      taVector2i suc;
      for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
          Unit* su_u = send_lay->UnitAtCoord(suc);
          if(su_u == NULL) continue;
          int sugp_idx = suc.y * send_lay->flat_geom.x + suc.x;
          alloc_sz[sugp_idx] += ru_nunits;
        }
      }
    }
  }

  // do the alloc
  taVector2i suc;
  for(suc.y = 0; suc.y < send_lay->flat_geom.y; suc.y++) {
    for(suc.x = 0; suc.x < send_lay->flat_geom.x; suc.x++) {
      Unit* su_u = send_lay->UnitAtCoord(suc);
      if(su_u == NULL) continue;
      int sugp_idx = suc.y * send_lay->flat_geom.x + suc.x;
      su_u->RecvConsPreAlloc(alloc_sz[sugp_idx], prjn);
    }
  }

  // then make the connections!
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
        taVector2i su_st;
        su_st.x = (int)((float)ruc.x * rf_width.x);
        su_st.y = (int)((float)ruc.y * rf_width.y);

        taVector2i suc;
        for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
          for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
            Unit* su_u = send_lay->UnitAtCoord(suc);
            if(su_u == NULL) continue;

            for(int rui=0; rui < ru_nunits; rui++) {
              Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
              if(!self_con && (su_u == ru_u)) continue;
              su_u->ConnectFrom(ru_u, prjn, alloc_loop);
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


