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

#include "GpMapDivergePrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(GpMapDivergePrjnSpec);

void GpMapDivergePrjnSpec::Initialize() {
}

void GpMapDivergePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "GpMapDivergePrjnSpec::Connect_impl",
                 "requires receiving layer to have unit groups!")) {
    return;
  }
  // below assumes the "==" operator has been overloaded for object un_geom in the intuitive way
  if(TestWarning(!(prjn->from->un_geom==prjn->layer->un_geom), "GpMapDivergePrjnSpec::Connect_impl",
                 "requires sending layer unit geometry to match receiving layer unit geometry, i.e., within each unit group!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  int n_ru_gps = recv_lay->gp_geom.n;

  int su_alloc_no = n_ru_gps;   // number of cons from each send unit

  // pre-alloc senders
  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units)
    su->SendConsPreAlloc(su_alloc_no, prjn);

  // pre-alloc receivers -- only 1
  for(int ri = 0; ri<recv_lay->units.leaves; ri++) {
    Unit* ru_u = (Unit*)recv_lay->units.Leaf(ri);
    if(!ru_u) break;
    ru_u->RecvConsPreAlloc(1, prjn);
  }

  // now actually build connections
  for(int si = 0; si<send_lay->units.leaves; si++) {
    Unit* su_u = (Unit*)send_lay->units.Leaf(si);
    taVector2i ruc;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++) {
        int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(si, rgpidx);
        if(ru_u) {
          ru_u->ConnectFrom(su_u, prjn);
        }
      }
    }
  }
}

