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

  LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
  if(u->CheckError(!cg, quiet, rval,
                     "Requires one recv projection!")) {
    return false;
  }
  LeabraUnit* su = (LeabraUnit*)cg->Un(0, net);
  if(u->CheckError(!su, quiet, rval, 
                     "Requires one unit in recv projection!")) {
    return false;
  }

  return rval;
}

void Deep5bUnitSpec::Compute_ActFmSource(LeabraUnit* u, LeabraNetwork* net) {
  LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
  if(!cg) return;
  LeabraUnit* su = (LeabraUnit*)cg->Un(0, net);
  if(!su) return;
  LeabraLayer* fmlay = (LeabraLayer*)cg->prjn->from.ptr();
  if(fmlay->lesioned()) {
    u->act = 0.0f;
    return;
  }
  u->act = su->deep5b;
  u->act_eq = u->act_nd = u->act;
  u->da = 0.0f;            // I'm fully settled!
  u->AddToActBuf(syn_delay);
}

void Deep5bUnitSpec::Compute_Act(Unit* un, Network* rnet, int thread_no) {
  Compute_ActFmSource((LeabraUnit*)un, (LeabraNetwork*)rnet);
}

