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

#include "BasAmygConSpec.h"

TA_BASEFUNS_CTORS_DEFN(BasAmygConSpec);

void BasAmygConSpec::Initialize() {
  burst_da_gain = 1.0f;
  dip_da_gain = 1.0f;
}

void BasAmygConSpec::Defaults_init() {
  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
}

bool BasAmygConSpec::CheckConfig_RecvCons(ConGroup* cg, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(cg, quiet);
  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetUnitSpec();
  
  if(rlay->CheckError(!rus->InheritsFrom(&TA_BasAmygUnitSpec), quiet, rval,
                      "requires receiving unit to use an BasAmygUnitSpec"))
    return false;
  return rval;
}

