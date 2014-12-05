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

#include "Deep5bUnitSpec.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(Deep5bUnitSpec);

void Deep5bUnitSpec::Initialize() {
  Defaults_init();
}

void Deep5bUnitSpec::Defaults_init() {
}

bool Deep5bUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  LeabraUnit* u = (LeabraUnit*)un;
  bool rval = inherited::CheckConfig_Unit(un, quiet);

  LeabraNetwork* net = (LeabraNetwork*)u->own_net();

  LeabraConGroup* cg = (LeabraConGroup*)u->RecvConGroupSafe(0);
  if(u->CheckError(!cg, quiet, rval,
                     "Requires one recv projection!")) {
    return false;
  }
  LeabraUnit* su = (LeabraUnit*)cg->SafeUn(0);
  if(u->CheckError(!su, quiet, rval, 
                     "Requires one unit in recv projection!")) {
    return false;
  }

  return rval;
}

void Deep5bUnitSpec::Compute_ActFmSource(LeabraUnitVars* u, LeabraNetwork* net,
                                         int thr_no) {
  LeabraConGroup* cg = (LeabraConGroup*)u->RecvConGroupSafe(net, thr_no, 0);
  LeabraUnitVars* su = (LeabraUnitVars*)cg->UnVars(0, net);
  LeabraLayer* fmlay = (LeabraLayer*)cg->prjn->from.ptr();
  if(fmlay->lesioned()) {
    u->act = 0.0f;
    return;
  }
  u->act = su->deep5b;
  u->act_eq = u->act_nd = u->act;
  TestWrite(u->da, 0.0f);
  // u->AddToActBuf(syn_delay); // todo:
}

void Deep5bUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_ActFmSource(u, net, thr_no);
}

void Deep5bUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_ActFmSource(u, net, thr_no);
}

