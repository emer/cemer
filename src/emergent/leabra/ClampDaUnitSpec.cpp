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

#include "ClampDaUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(ClampDaUnitSpec);

void ClampDaUnitSpec::Initialize() {
  send_da = CYCLE;
}

void ClampDaUnitSpec::Send_Da(LeabraUnit* u, LeabraNetwork* net) {
  const float snd_val = u->act;
  for(int g=0; g<u->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnit*)send_gp->Un(j,net))->dav = snd_val;
    }
  }
}

void ClampDaUnitSpec::Compute_Act(Unit* ru, Network* rnet, int thread_no) {
  inherited::Compute_Act(ru, rnet, thread_no);
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  if(send_da == CYCLE) {
    Send_Da(u, net);
  }
  else if(send_da == PLUS_START && net->phase == LeabraNetwork::PLUS_PHASE) {
    Send_Da(u, net);
  }
}

void ClampDaUnitSpec::PostSettle(LeabraUnit* u, LeabraNetwork* net) {
  inherited::PostSettle(u, net);
  if(send_da == PLUS_END && net->phase == LeabraNetwork::PLUS_PHASE) {
    Send_Da(u, net);
  }
}

