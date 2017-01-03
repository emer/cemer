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

#include "BLAmygConSpec.h"
#include <BLAmygUnitSpec>

TA_BASEFUNS_CTORS_DEFN(BLAmygLearnSpec);
TA_BASEFUNS_CTORS_DEFN(BLAmygConSpec);

void BLAmygLearnSpec::Initialize() {
  Defaults_init();
}

void BLAmygLearnSpec::Defaults_init() {
  dalr_gain = 1.0f;
  dalr_base = 0.0f;
  wt_decay = 0.1f;
  wt_floor = 0.5f;
}

void BLAmygConSpec::Initialize() {
  Defaults_init();
}

void BLAmygConSpec::Defaults_init() {
  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
}

bool BLAmygConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetUnitSpec();
  
  if(rlay->CheckError(!rus->InheritsFrom(&TA_BLAmygUnitSpec), quiet, rval,
                    "requires receiving unit to use an BLAmygUnitSpec"))
    return false;
  return rval;
}

