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

#include "MarkerGpOneToOnePrjnSpec.h"

#include <Network>

void MarkerGpOneToOnePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  taVector2i ru_geo = recv_lay->gp_geom;
  taVector2i su_geo = send_lay->gp_geom;

  // revert to main group if no sub groups
  Layer::AccessMode racc_md = Layer::ACC_GP;
  if(!recv_lay->unit_groups) racc_md = Layer::ACC_LAY;
  Layer::AccessMode sacc_md = Layer::ACC_GP;
  if(!send_lay->unit_groups) sacc_md = Layer::ACC_LAY;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  int s_alloc = (ru_nunits / su_nunits) + 1;

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = recv_lay->gp_geom.n - recv_start;
  max_n = MIN(recv_lay->gp_geom.n - recv_start, max_n);
  max_n = MIN(send_lay->gp_geom.n - send_start, max_n);
  max_n = MAX(1, max_n);        // lower limit of 1
  for(int i=0; i<max_n; i++) {

    int rgpidx = i + recv_start;
    int sgpidx = i + send_start;

    // pre-allocate connections
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      ru->RecvConsPreAlloc(1, prjn);
    }
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      su->SendConsPreAlloc(s_alloc, prjn);
    }

    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      int sui = rui % su_nunits;
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      if(self_con || (ru != su))
	ru->ConnectFrom(su, prjn);
    }
  }
}

