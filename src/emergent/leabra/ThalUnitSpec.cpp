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

#include "ThalUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(ThalUnitSpec);

void ThalUnitSpec::Initialize() {
}

void ThalUnitSpec::Defaults_init() {
}

void ThalUnitSpec::Send_Thal(LeabraUnit* u, LeabraNetwork* net) {
  const float snd_val = u->act_eq;
  for(int g=0; g<u->send.size; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnit*)send_gp->Un(j,net))->thal = snd_val;
    }
  }
}

void ThalUnitSpec::Compute_Act(Unit* ru, Network* rnet, int thread_no) {
  inherited::Compute_Act(ru, rnet, thread_no);
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  Send_Thal(u, net);
}

