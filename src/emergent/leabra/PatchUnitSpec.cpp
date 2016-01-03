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
  Defaults_init();
}

void PatchUnitSpec::Defaults_init() {
  shunt_factor = 0.05f;
  shunt_ach = true;
}

void PatchUnitSpec::Send_DAShunt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(u->act_eq < opt_thresh.send) return;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)send_gp->UnVars(j,net);
      uv->da_p *= shunt_factor; // shunt!
      if(shunt_ach) {
        uv->ach *= shunt_factor; // shunt!
      }
    }
  }
}

void PatchUnitSpec::Compute_Act_Post(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Send_DAShunt(u, net, thr_no);
  inherited::Compute_Act_Post(u, net, thr_no);
}
