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

#include "DecodeTwoDValLayerSpec.h"

void DecodeTwoDValLayerSpec::Initialize() {
}

void DecodeTwoDValLayerSpec::Compute_Inhib(LeabraLayer*, LeabraNetwork*) {
  return;                       // do nothing!
}

void DecodeTwoDValLayerSpec::ReadValue_ugp(TwoDValLeabraLayer* lay,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(u->recv.size == 0) continue;
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv[0];
    if(cg->size == 0) continue;
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    u->net = su->net;
    u->act = su->act;
    u->act_eq = su->act_eq;
    u->act_nd = su->act_nd;
    u->act_lrn = su->act_lrn;
  }
  inherited::ReadValue_ugp(lay, acc_md, gpidx, net);
}

