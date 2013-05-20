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

#include "taFBase.h"
#include <QFileInfo>

bool taFBase::SetFileName(const String& val) {
  QFileInfo fi(val);
  //note: canonical paths only available if file actually exists...
  if (fi.exists()) {
    // we get the canonical path, so we insure we can do exact filename compares later
    file_name = fi.canonicalFilePath();
  }
  else {
    file_name = val;
  }
  return true;
}

