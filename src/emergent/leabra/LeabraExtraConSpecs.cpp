// Copyright 2017, Regents of the University of Colorado,
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

#include "LeabraExtraConSpecs.h"

#include <LeabraNetwork>

#include <State_main>

TA_BASEFUNS_CTORS_DEFN(DeepCtxtConSpec);

void DeepCtxtConSpec::Initialize() {
  SetUnique("wt_scale", true);
  SetUnique("momentum", true);
  Initialize_core();
}

void DeepCtxtConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Ctxt_" + nm;
}

bool DeepCtxtConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraLayerSpec* rls = (LeabraLayerSpec*)rlay->GetMainLayerSpec();
  // LeabraNetwork* net = (LeabraNetwork*)rlay->own_net;
  
  if(rlay->CheckError(rls->decay.trial == 1.0f, quiet, rval,
                      "cannot have layer decay.trial = 1 when using temporal integration deep context learning -- this value will be set to 0 -- you can use .99f if you really want to")) {
    rls->decay.trial = 0.0f;
    rval = false;
  }

  LeabraLayer* slay = (LeabraLayer*)prjn->from.ptr();
  LeabraLayerSpec* sls = (LeabraLayerSpec*)slay->GetMainLayerSpec();
  if(slay->CheckError(sls->decay.trial == 1.0f, quiet, rval,
                      "cannot have layer decay.trial = 1 when using temporal integration deep context learning -- this value will be set to 0 -- you can use .99f if you really want to")) {
    sls->decay.trial = 0.0f;
    rval = false;
  }

  return rval;
}



