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

#include "XCalSRAvgConSpec.h"

void XCalSRAvgConSpec::Initialize() {
}

bool XCalSRAvgConSpec::CheckConfig_RecvCons(RecvCons* cg, bool quiet) {
  bool rval = true;
  if((learn_rule == CTLEABRA_CAL) || (learn_rule == CTLEABRA_XCAL)) {
    if(cg->prjn) {
      if(cg->prjn->CheckError(!cg->prjn->con_type->InheritsFrom(&TA_LeabraSRAvgCon), quiet, rval,
                      "does not have con_type = LeabraSRAvgCon -- required for CTLEABRA_CAL or _XCAL learning to hold the sravg connection-level values -- I just fixed this for you in this projection, but must do Build to get it to take effect")) {
        cg->prjn->con_type = &TA_LeabraSRAvgCon;
      }
    }
  }
  return rval;
}

