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

#include "MatrixUnitSpec.h"
#include <LeabraNetwork>
#include <MatrixConSpec>

TA_BASEFUNS_CTORS_DEFN(MatrixUnitSpec);


void MatrixUnitSpec::Initialize() {
  Defaults_init();
}

void MatrixUnitSpec::Defaults_init() {
  // SetUnique("noise_type", true);
  noise_type = NO_NOISE;        // NETIN_NOISE

  // SetUnique("noise", true);
  noise.var = 0.0005f;
  // SetUnique("noise_adapt", true);
  noise_adapt.mode = NoiseAdaptSpec::FIXED_NOISE;
  noise_adapt.trial_fixed = true;
}

void MatrixUnitSpec::InitLinks() {
  inherited::InitLinks();
}

