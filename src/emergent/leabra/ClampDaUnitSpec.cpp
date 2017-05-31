// Copyright 2017, Regents of the University of Colorado,
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
  da_val = DA_P;
}

void ClampDaUnitSpec::Send_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  const float snd_val = u->act;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      if(da_val == DA_P)
        ((LeabraUnitVars*)send_gp->UnVars(j,net))->da_p = snd_val;
      else
        ((LeabraUnitVars*)send_gp->UnVars(j,net))->da_n = snd_val;
    }
  }
}

void ClampDaUnitSpec::Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Quarter_Final(u, net, thr_no);
  if(send_da == PLUS_END && net->phase == LeabraNetwork::PLUS_PHASE) {
    Send_Da(u, net, thr_no);
  }
}

void ClampDaUnitSpec::Compute_Act_Post(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Post(u, net, thr_no);
  if(send_da == CYCLE) {
    Send_Da(u, net, thr_no);
  }
  else if(send_da == PLUS_START && net->phase == LeabraNetwork::PLUS_PHASE) {
    Send_Da(u, net, thr_no);
  }
}

