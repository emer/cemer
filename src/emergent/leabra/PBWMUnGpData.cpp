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

#include "PBWMUnGpData.h"

void PBWMUnGpData::Initialize() {
  go_fired_now = false;
  go_fired_trial = false;
  go_cycle = -1;
  mnt_count = -1;		// empty
  prv_mnt_count = -1;
  nogo_inhib = 0.0f;
  refract_inhib = 0.0f;
  nogo_deep = 0.0f;
}

void PBWMUnGpData::Copy_(const PBWMUnGpData& cp) {
  go_fired_now = cp.go_fired_now;
  go_fired_trial = cp.go_fired_trial;
  go_cycle = cp.go_cycle;
  mnt_count = cp.mnt_count;
  prv_mnt_count = cp.prv_mnt_count;
  nogo_inhib = cp.nogo_inhib;
  refract_inhib = cp.refract_inhib;
  nogo_deep = cp.nogo_deep;
}

void PBWMUnGpData::Init_State() {
  inherited::Init_State();
  Initialize();
}

void PBWMUnGpData::CopyPBWMData(const PBWMUnGpData& cp) {
  PBWMUnGpData::Copy_(cp);
}

