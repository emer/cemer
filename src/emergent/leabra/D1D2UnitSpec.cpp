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

#include "D1D2UnitSpec.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(D1D2UnitSpec);

void D1D2UnitSpec::Initialize() {
  Defaults_init();
}

void D1D2UnitSpec::Defaults_init() {
}

void D1D2UnitSpec::Compute_DeepMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(deep.SendDeepMod()) {
    u->deep_lrn = u->deep_mod = u->act;      // record what we send!
  }
  else if(deep.TRCUnits()) {
    u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
  }
  // must be SUPER units at this point
  else if(lay->am_deep_mod_net.max < 0.01f) { // not enough yet // was 0.1f
    u->deep_lrn = u->deep_mod = 0.0f;    // default is 0!
  }
  else {
    u->deep_lrn = u->deep_mod_net / lay->am_deep_mod_net.max; // todo: could not normalize this..
    u->deep_mod = deep.mod_min + deep.mod_range * u->deep_lrn;
  }
}

