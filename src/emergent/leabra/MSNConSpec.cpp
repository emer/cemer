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
TA_BASEFUNS_CTORS_DEFN(MSNTraceThalGains);
TA_BASEFUNS_CTORS_DEFN(MSNConSpec);

void MSNTraceSpec::Initialize() {
  Defaults_init();
}

void MSNTraceSpec::Defaults_init() {
  otr_nogo_veto = true;
  nogo_max = 0.3f;
  ach_reset_thr = 0.5f;
}

// void MSNTraceSpec::UpdateAfterEdit_impl() {
//   inherited::UpdateAfterEdit_impl();
// }

void MSNTraceThalGains::Initialize() {
  Defaults_init();
}

void MSNTraceThalGains::Defaults_init() {
  gate_go_pos = 1.0f;
  gate_go_neg = 1.0f;
  gate_nogo_pos = 0.1f;
  gate_nogo_neg = 1.0f;
  not_go_pos = 0.8f;
  not_go_neg = 0.2f;
  not_nogo_pos = 0.01f;
  not_nogo_neg = 0.02f;
}

void MSNConSpec::Initialize() {
  min_obj_type = &TA_MSNCon;
  su_act_var = PREV_TRIAL;
  ru_act_var = ACT_P;
  learn_rule = DA_HEBB_VS;
  Defaults_init();
}

void MSNConSpec::Defaults_init() {
  burst_da_gain = 1.0f;
  dip_da_gain = 1.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  // SetUnique("lrate", true);
  lrate = 0.005f;

  learn_qtr = Q2_Q4;            // beta frequency default

  // use_unlearnable = false;
}

void MSNConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

bool MSNConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
  bool rval = inherited::CheckConfig_RecvCons(prjn, quiet);
  LeabraLayer* rlay = (LeabraLayer*)prjn->layer;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)rlay->GetUnitSpec();
  
  if(rlay->CheckError(!rus->InheritsFrom(&TA_MSNUnitSpec), quiet, rval,
                      "requires receiving unit to use an MSNUnitSpec"))
    return false;
  return rval;
}

