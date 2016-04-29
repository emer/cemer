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

#include "DeepCtxtConSpec.h"

TA_BASEFUNS_CTORS_DEFN(DeepCtxtConSpec);

void DeepCtxtConSpec::Initialize() {
  SetUnique("wt_scale", true);
  wt_scale.rel = 1.0;
  delta_dwt = false;
}

void DeepCtxtConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Ctxt_" + nm;
}

void DeepCtxtConSpec::Trial_Init_Specs(LeabraNetwork* net) {
  inherited::Trial_Init_Specs(net);
  net->deep.ctxt = true;
}

bool DeepCtxtConSpec::CheckConfig_RecvCons(Unit* ru, Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(ru, prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraLayerSpec* rls = (LeabraLayerSpec*)rlay->GetLayerSpec();
  
  if(rlay->CheckError(rls->decay.trial == 1.0f, quiet, rval,
                      "cannot have layer decay.trial = 1 when using temporal integration deep context learning -- set to 0 -- you can use .99f if you really want to"))
    return false;

  LeabraLayer* slay = (LeabraLayer*)prjn->from.ptr();
  LeabraLayerSpec* sls = (LeabraLayerSpec*)slay->GetLayerSpec();
  if(slay->CheckError(sls->decay.trial == 1.0f, quiet, rval,
                      "cannot have layer decay.trial = 1 when using temporal integration deep context learning -- set to 0 -- you can use .99f if you really want to"))
    return false;

  return rval;
}

