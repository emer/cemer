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

#include "Deep5bClampUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(Deep5bClampUnitSpec);

void Deep5bClampUnitSpec::Initialize() {
  Defaults_init();
}

void Deep5bClampUnitSpec::Defaults_init() {
}

void Deep5bClampUnitSpec::Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  inherited::Compute_NetinRaw(u, net, thr_no);
  if(Quarter_Deep5bNow(net->quarter)) {
    u->net_raw = 0.0f;          // zero the regular netins
  }
}
