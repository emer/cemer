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

#include "CaiSynDepConSpec.h"

void CaiSynDepSpec::Initialize() {
  ca_inc = .2f;                 // base per-cycle is .01
  ca_dec = .2f;                 // base per-cycle is .01
  sd_ca_thr = 0.2f;
  sd_ca_gain = 0.3f;
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CaiSynDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CaiSynDepConSpec::Initialize() {
  min_obj_type = &TA_CaiSynDepCon;
}

void CaiSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ca_dep.UpdateAfterEdit_NoGui();
}
