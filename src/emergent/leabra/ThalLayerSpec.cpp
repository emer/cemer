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

#include "ThalLayerSpec.h"

#include <LeabraNetwork>
#include <MarkerConSpec>

TA_BASEFUNS_CTORS_DEFN(ThalLayerSpec);

void ThalLayerSpec::Initialize() {
}

void ThalLayerSpec::Send_Thal(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    const float snd_val = u->act;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      for(int j=0;j<send_gp->size; j++) {
        ((LeabraUnit*)send_gp->Un(j,net))->thal = snd_val;
      }
    }
  }
}

void ThalLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  Send_Thal(lay, net);
  inherited::Compute_CycleStats(lay, net, thread_no);
}

