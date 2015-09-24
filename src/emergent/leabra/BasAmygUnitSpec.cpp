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

TA_BASEFUNS_CTORS_DEFN(BasAmygUnitSpec);

void BasAmygUnitSpec::Initialize() {
  acq_ext = ACQ;
  valence = APPETITIVE;
  dar = D1R;
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
