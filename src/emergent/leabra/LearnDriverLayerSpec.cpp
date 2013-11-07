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

#include "LearnDriverLayerSpec.h"

#include <LeabraNetwork>

void LearnDriverLayerSpec::Initialize() {
  learn_thr = 0.1f;
}

void LearnDriverLayerSpec::Defaults_init() {
}

void LearnDriverLayerSpec::Send_LearnFlags(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    const bool lrn_on = (u->act_eq > learn_thr);
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())       continue;
      for(int j=0;j<send_gp->size; j++) {
        send_gp->Un(j,net)->SetUnitFlagState(Unit::LEARN, lrn_on);
      }
    }
  }
}

void LearnDriverLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  Send_LearnFlags(lay, net);
}

