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

#include "XCalHebbConSpec.h"

void XCalHebbConSpec::Initialize() {
  hebb_mix = 0.0f;
  su_act_min = 0.0f;
  hebb_mix_c = 1.0f - hebb_mix;
}

void XCalHebbConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  hebb_mix_c = 1.0f - hebb_mix;
}

