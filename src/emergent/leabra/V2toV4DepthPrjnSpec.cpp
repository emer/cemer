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

#include "V2toV4DepthPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(V2toV4DepthPrjnSpec);

void V2toV4DepthPrjnSpec::Initialize() {
  depth_idx = 0;
}

void V2toV4DepthPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                Layer* send_lay, int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;
  taVector2i sug;

  if(reciprocal) {              // reciprocal is backwards!
    for(int sui=0; sui < su_nunits; sui++) {
      sug.SetFmIndex(sui, send_lay->un_geom.x);
      int cur_depth = sug.y / 2; // v2bo has 2 per depth always
      if(cur_depth != depth_idx) continue;
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
        sug.SetFmIndex(sui, send_lay->un_geom.x);
        int cur_depth = sug.y / 2; // v2bo has 2 per depth always
        if(cur_depth != depth_idx) continue;
        Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
      }
    }
  }
}

