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

#include "SendDeepModConSpec.h"

TA_BASEFUNS_CTORS_DEFN(SendDeepModConSpec);

void SendDeepModConSpec::Initialize() {
  SetUnique("learn", true);     // generally doesn't learn..
  learn = false;
  SetUnique("rnd", true);
  rnd.mean = 0.8f;
  rnd.var = 0.0f;
  SetUnique("wt_limits", true);
  wt_limits.sym = false;
}

void SendDeepModConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Deep_Mod_" + nm;
}

void SendDeepModConSpec::Trial_Init_Specs(LeabraNetwork* net) {
  inherited::Trial_Init_Specs(net);
  net->deep.mod_net = true;
}

