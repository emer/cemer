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

#include "VSPatchUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(VSPatchUnitSpec);

void VSPatchUnitSpec::Initialize() {
}

void VSPatchUnitSpec::Defaults_init() {
}

void VSPatchUnitSpec::Compute_Act(Unit* ru, Network* rnet, int thread_no) {
  LeabraUnit* u = (LeabraUnit*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    u->act = u->act_eq = u->act_nd = u->act_lrn = u->lrnmod;
  }
  else {
    inherited::Compute_Act(ru, rnet, thread_no);
  }
}

