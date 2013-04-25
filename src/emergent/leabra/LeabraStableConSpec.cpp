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

#include "LeabraStableConSpec.h"

void StableMixSpec::Initialize() {
  stable_pct = 0.5f;
  learn_pct = 1.0f - stable_pct;
  updt_to_lwt = true;
}

void StableMixSpec::Defaults_init() {
}

void StableMixSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  learn_pct = 1.0f - stable_pct;
}

void LeabraStableConSpec::Initialize() {
  min_obj_type = &TA_LeabraStableCon;
}

void LeabraStableConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  stable_mix.UpdateAfterEdit_NoGui();
}
