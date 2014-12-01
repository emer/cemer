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

#include "LeabraUnGpData.h"

TA_BASEFUNS_CTORS_DEFN(LeabraUnGpData);

void LeabraUnGpData::Initialize() {
  Inhib_Initialize();
}

void LeabraUnGpData::InitLinks() {
  inherited::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(acts, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);

  taBase::Own(i_val, this);
}

void LeabraUnGpData::Copy_(const LeabraUnGpData& cp) {
  Inhib_Copy_(cp);
}

void LeabraUnGpData::Init_State() {
  // nop
}
