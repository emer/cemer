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

#include "PPTgUnitSpec.h"

#include <LeabraNetwork>

void PPTgUnitSpec::Initialize() {
  d_net_gain = 1.0f;
}

void PPTgUnitSpec::Defaults_init() {
}

void PPTgUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  LeabraUnit* lu = (LeabraUnit*)u;
  float net_save = lu->net;
  lu->net = d_net_gain * (lu->net - lu->misc_1); // convert to delta
  if(lu->net < 0.0f) lu->net = 0.0f;
  // note: positive rectification means that trial after PV, which is often neg, will be nullified
  inherited::Compute_Act(u, net, thread_no);
  lu->net = net_save;           // restore
}

void PPTgUnitSpec::PostSettle(LeabraUnit* u, LeabraNetwork* net) {
  inherited::PostSettle(u, net);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    u->misc_1 = u->net;       // save for next time
  }
}

