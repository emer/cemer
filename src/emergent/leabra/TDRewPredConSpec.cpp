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

#include "TDRewPredConSpec.h"

TA_BASEFUNS_CTORS_DEFN(TDRewPredConSpec);

void TDRewPredConSpec::Initialize() {
  use_trace_act_avg = false;
  
  SetUnique("rnd", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;

  SetUnique("wt_limits", true);
  wt_limits.type = WeightLimits::NONE;
  wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;
  wt_sig.off = 1.0f;
  
  SetUnique("wt_norm_bal", true);
  wt_norm_bal.bal_on = false;
}
