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

#include "ActAvgHebbConSpec.h"

void ActAvgHebbMixSpec::Initialize() {
  act_avg = .5f;
  cur_act = .5f;
}

void ActAvgHebbMixSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cur_act = 1.0f - act_avg;
}

void ActAvgHebbConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  act_avg_hebb.UpdateAfterEdit_NoGui();
}

void ActAvgHebbConSpec::Initialize() {
}

