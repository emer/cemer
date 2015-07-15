// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "float_CircBuffer.h"

TA_BASEFUNS_CTORS_DEFN(float_CircBuffer);

void float_CircBuffer::Initialize() {
  st_idx = 0;
  length = 0;
}

void float_CircBuffer::Copy_(const float_CircBuffer& cp) {
  st_idx = cp.st_idx;
  length = cp.length;
}

void float_CircBuffer::Reset() {
  float_Array::Reset();
  st_idx = 0;
  length = 0;
}
