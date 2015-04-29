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

#include "GpCustomPrjnSpecBase.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(GpCustomPrjnSpecBase);


void GpCustomPrjnSpecBase::Connect_Gp(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			     Layer::AccessMode sacc_md, int sgpidx, bool make_cons) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
    if(!ru) continue;
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      if(su && (self_con || (ru != su))) {
        if(!make_cons) {
          su->SendConsAllocInc(1, prjn);
          ru->RecvConsAllocInc(1, prjn);
        }
        else {
          ru->ConnectFrom(su, prjn);
        }
      }
    }
  }
}
