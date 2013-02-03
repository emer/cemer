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

#include "LeabraRecvCons.h"

void LeabraRecvCons::Initialize() {
  ClearBaseFlag(OWN_CONS);      // recv does NOT own!
  SetConType(&TA_LeabraCon);
  scale_eff = 0.0f;
  net = 0.0f;
  net_delta = 0.0f;
  net_raw = 0.0f;
}

void LeabraRecvCons::Copy_(const LeabraRecvCons& cp) {
  scale_eff = cp.scale_eff;
  net = cp.net;
  net_delta = cp.net_delta;
  net_raw = cp.net_raw;
}

