// Copyright 2015-2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "T3CameraParams.h"

TA_BASEFUNS_CTORS_DEFN(T3CameraParams);

void T3CameraParams::Initialize() {
  field_of_view = 45;
  near_distance = 0.1f;
  focal = 5.0f;
  far_distance = 1000.0f;
}
