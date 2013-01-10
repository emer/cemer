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

#include "taPluginDep.h"

void taPluginDep::Initialize() {
  dep_check = DC_OK;
}

void taPluginDep::Copy_(const taPluginDep& cp) {
  dep_check = cp.dep_check; // not really used for copies
}

void taPluginDep::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (dep_check != DC_OK) {
    if (!quiet) {
      String msg;
      switch (dep_check) {
      case DC_MISSING: msg = " is missing"; break;
      case DC_NOT_LOADED: msg = " is not loaded"; break;
      default: break; // compiler food
      }
      taMisc::CheckError("Required plugin: ",
        GetDisplayName(), msg);
    }
    rval = false;
  }
}

