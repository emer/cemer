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

#include "LearnModUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(LearnModUnitSpec);

void LearnModUnitSpec::Initialize() {
  learn_thr = 0.1f;
}

void LearnModUnitSpec::Defaults_init() {
}

void LearnModUnitSpec::Send_LearnMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float lrnmod = (u->act_eq > learn_thr) ? u->act_eq : 0.0f;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitVars*)send_gp->UnVars(j,net))->lrnmod = lrnmod;
    }
  }
}

void LearnModUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no)
{
  inherited::Compute_Act_Rate(u, net, thr_no);
  Send_LearnMod(u, net, thr_no);
}

void LearnModUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no)
{
  inherited::Compute_Act_Spike(u, net, thr_no);
  Send_LearnMod(u, net, thr_no);
}

