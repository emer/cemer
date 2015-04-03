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

#include "ThalAutoEncodeUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(ThalAutoEncodeUnitSpec);

void ThalAutoEncodeUnitSpec::Initialize() {
  Defaults_init();
}

void ThalAutoEncodeUnitSpec::Defaults_init() {
  deep.on = true;
  deep_qtr = Q4;
}

void ThalAutoEncodeUnitSpec::Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  TestWrite(u->deep_norm, u->deep_norm_net); // always get from net
  inherited::Compute_NetinRaw(u, net, thr_no);
  if(net->phase == LeabraNetwork::PLUS_PHASE) { // note: using plus phase here..
    u->net_raw = u->deep_raw_net;          // only gets from deep!
  }
}

void ThalAutoEncodeUnitSpec::Trial_Init_SRAvg(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  inherited::Trial_Init_SRAvg(u, net, thr_no);
  u->avg_l_lrn = 0.0f;        // no self organizing in clamped layers!
}

