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

#include "GpMapConvergePrjnSpec.h"
#include <Network>

void GpMapConvergePrjnSpec::Initialize() {
}

void GpMapConvergePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires sending layer to have unit groups!")) {
    return;
  }
  // below assumes the "==" operator has been overloaded for object un_geom in the intuitive way
  if(TestWarning(!(prjn->from->un_geom==prjn->layer->un_geom), "GpMapConvergePrjnSpec::Connect_impl",
                 "requires sending layer unit geometry to match receiving layer unit geometry, i.e., within each unit group!")) {
    return;
  }

Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i su_geo = send_lay->gp_geom;
  int n_su_gps = send_lay->gp_geom.n;

  int alloc_no = n_su_gps;      // number of cons per recv unit

  // pre-alloc senders -- only 1
  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units)
    su->SendConsPreAlloc(1, prjn);

  for(int ri = 0; ri<recv_lay->units.leaves; ri++) {
    Unit* ru_u = (Unit*)recv_lay->units.Leaf(ri);
    if(!ru_u) break;
    ru_u->RecvConsPreAlloc(alloc_no, prjn);

    taVector2i suc;
    for(suc.y = 0; suc.y < su_geo.y; suc.y++) {
      for(suc.x = 0; suc.x < su_geo.x; suc.x++) {
        int sgpidx = send_lay->UnitGpIdxFmPos(suc);
        Unit* su_u = send_lay->UnitAtUnGpIdx(ri, sgpidx);
        if(su_u) {
          ru_u->ConnectFrom(su_u, prjn);
        }
      }
    }
  }
}

