// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taProjVersion.h"
#include <taVersion>

TA_BASEFUNS_CTORS_DEFN(taProjVersion);


void taProjVersion::SetFromString(String ver) {
  Clear();
  // parse, mj.mn.st -- just blindly go through, harmless if missings
  major = taVersion::BeforeOrOf('.', ver);
  minor = taVersion::BeforeOrOf('.', ver);
  step = taVersion::BeforeOrOf(' ', ver); // dummy
}

bool taProjVersion::GtEq(int mj, int mn, int st) {
  return (major > mj) ||
    ((major == mj) && (minor > mn)) ||
     ((major == mj) && (minor == mn) && (step >= st));
}

