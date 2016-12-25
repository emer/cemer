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

#include "CElAmygConSpec.h"
#include <BasAmygUnitSpec>

TA_BASEFUNS_CTORS_DEFN(CElAmygLearnSpec);
TA_BASEFUNS_CTORS_DEFN(CElAmygConSpec);

void CElAmygLearnSpec::Initialize() {
  Defaults_init();
}

void CElAmygLearnSpec::Defaults_init() {
  dalr_gain = 1.0f;
  dalr_base = 0.0f;
}

void CElAmygConSpec::Initialize() {
  Defaults_init();
}

void CElAmygConSpec::Defaults_init() {
  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
}

bool CElAmygConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetUnitSpec();
  
  // if(rlay->CheckError(!rus->InheritsFrom(&TA_CElAmygUnitSpec), quiet, rval,
  //                     "requires receiving unit to use an CElAmygUnitSpec"))
  //   return false;
  return rval;
}

