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

#include "PatchUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(PatchUnitSpec);

void PatchUnitSpec::Initialize() {
}

void PatchUnitSpec::Defaults_init() {
}

void PatchUnitSpec::Send_DAShunt(LeabraUnit* u, LeabraNetwork* net) {
  if(u->act_eq < opt_thresh.send) return;
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnit*)send_gp->Un(j,net))->dav = 0.0f; // shunt!
    }
  }
}

void PatchUnitSpec::Compute_Act_Post(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  Send_DAShunt(u, net);
  inherited::Compute_Act_Post(u, net, thread_no);
}
