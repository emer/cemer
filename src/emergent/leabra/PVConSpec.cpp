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

#include "PVConSpec.h"

void PVConSpec::Initialize() {
  SetUnique("lrate", true);
  lrate = .01f;
  cur_lrate = .01f;

  // SetUnique("lrate_sched", true); // not to have any lrate schedule!!
  // SetUnique("lrs_value", true); // not to have any lrate schedule!!
  lrs_value = NO_LRS;

  send_act = ACT_P;

  Defaults_init();
}

void PVConSpec::Defaults_init() {
  // SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  lmix.err_sb = false;

  // SetUnique("rnd", true);
  rnd.mean = 0.1f;
  rnd.var = 0.0f;

  // SetUnique("wt_limits", true);
  wt_limits.sym = false;

  // SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;  wt_sig.off = 1.0f;
  wt_sig.dwt_norm = false;      // bad news for pvlv
}

void PVConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
  // actually now allowing sb!!
//   lmix.err_sb = false;
  wt_sig.dwt_norm = false;      // bad news for pvlv
}

