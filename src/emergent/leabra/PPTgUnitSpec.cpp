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

TA_BASEFUNS_CTORS_DEFN(PPTgUnitSpec);

void PPTgUnitSpec::Initialize() {
  d_net_gain = 1.0f;
  clamp_act = true;
  act_thr = 0.0f;
}

void PPTgUnitSpec::Defaults_init() {
}

void PPTgUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float net_save = u->net;
  u->net = d_net_gain * (u->net - u->misc_1); // convert to delta
  if(u->net < act_thr) u->net = 0.0f;
  // note: positive rectification means that trial after PV, which is often neg, will be nullified
  inherited::Compute_Act_Rate(u, net, thr_no);
  if(clamp_act) {
    u->act_eq = u->act_nd = u->act = u->net;
    u->da = 0.0f;
  }
  u->net = net_save;           // restore
}

void PPTgUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float net_save = u->net;
  u->net = d_net_gain * (u->net - u->misc_1); // convert to delta
  if(u->net < 0.0f) u->net = 0.0f;
  // note: positive rectification means that trial after PV, which is often neg, will be nullified
  inherited::Compute_Act_Spike(u, net, thr_no);
  if(clamp_act) {
    u->act_eq = u->act_nd = u->act = u->net;
    u->da = 0.0f;
  }
  u->net = net_save;           // restore
}

void PPTgUnitSpec::Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Quarter_Final(u, net, thr_no);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    u->misc_1 = u->net;       // save for next time -- this is the raw net..
  }
}

