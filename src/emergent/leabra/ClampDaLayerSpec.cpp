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

#include "ClampDaLayerSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(ClampDaLayerSpec);

void ClampDaLayerSpec::Initialize() {
  send_da = CYCLE;
}


void ClampDaLayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    const float snd_val = u->act;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())       continue;
      for(int j=0;j<send_gp->size; j++) {
        ((LeabraUnit*)send_gp->Un(j,net))->dav = snd_val;
      }
    }
  }
}

void ClampDaLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(send_da == CYCLE) {
    Send_Da(lay, net);
  }
  inherited::Compute_CycleStats(lay, net);
}

void ClampDaLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(send_da == PLUS_END && net->phase == LeabraNetwork::PLUS_PHASE) {
    Send_Da(lay, net);
  }
}

void ClampDaLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_HardClamp(lay, net);
  if(send_da == PLUS_START && net->phase == LeabraNetwork::PLUS_PHASE) {
    Send_Da(lay, net);
  }
}
