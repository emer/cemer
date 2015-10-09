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

#include "BasAmygUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(BasAmygUnitSpec);

void BasAmygUnitSpec::Initialize() {
  acq_ext = ACQ;
  valence = APPETITIVE;
  dar = D1R;
  deep_vg_netin = false;
  Defaults_init();
}

void BasAmygUnitSpec::Defaults_init() {
}

void BasAmygUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(acq_ext == ACQ) {
    if(valence == APPETITIVE) {
      dar = D1R;
    }
    else {
      dar = D2R;
    }
  }
  else {
    if(valence == APPETITIVE) {
      dar = D2R;                // reversed!
    }
    else {
      dar = D1R;
    }
  }
}

float BasAmygUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no, float& net_syn) {
  float net_ex = inherited::Compute_NetinExtras(u, net, thr_no, net_syn);
  if(deep.ApplyDeepMod() && deep_vg_netin) {
    net_ex += u->deep_lrn * u->act_eq;
  }
  return net_ex;
}

void BasAmygUnitSpec::Compute_DeepMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(deep.SendDeepMod()) {
    u->deep_lrn = u->deep_mod = u->act;      // record what we send!
  }
  else if(deep.TRCUnits()) {
    u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
  }
  // must be SUPER units at this point
  else if(lay->am_deep_net.max < 0.1f) { // not enough yet 
    u->deep_lrn = 0.0f;    // default is 0!
    u->deep_mod = 1.0f;
  }
  else {
    u->deep_lrn = u->deep_net / lay->am_deep_net.max; // todo: could not normalize this..
    u->deep_mod = 1.0f;                               // do not modulate with deep_mod!
  }
}

