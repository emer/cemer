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

#include "MSNConSpec.h"

TA_BASEFUNS_CTORS_DEFN(MSNTraceSpec);
TA_BASEFUNS_CTORS_DEFN(MSNConSpec);

void MSNTraceSpec::Initialize() {
  Defaults_init();
}

void MSNTraceSpec::Defaults_init() {
  da_reset_tr = 0.1f;
  tr_decay = 1.0f;
  otr_lrate = 0.5f;
  protect_pos = false;
  thal_mult = false;
  tr_max = 1.0f;
}

void MSNTraceSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void MSNConSpec::Initialize() {
  min_obj_type = &TA_MSNCon;
  su_act_var = PREV_TRIAL;
  ru_act_var = ACT_P;
  learn_rule = DA_HEBB;
  Defaults_init();
}

void MSNConSpec::Defaults_init() {
  burst_da_gain = 1.0f;
  dip_da_gain = 1.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  // SetUnique("lrate", true);
  lrate = 0.001f;

  learn_qtr = Q2_Q4;            // beta frequency default

  ignore_unlearnable = false;
}

void MSNConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

bool MSNConSpec::CheckConfig_RecvCons(ConGroup* cg, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(cg, quiet);
  LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetUnitSpec();
  
  if(rlay->CheckError(!rus->InheritsFrom(&TA_MSNUnitSpec), quiet, rval,
                      "requires receiving unit to use an MSNUnitSpec"))
    return false;
  return rval;
}

