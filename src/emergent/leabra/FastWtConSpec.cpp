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

#include "FastWtConSpec.h"

void FastWtSpec::Initialize() {
  lrate = .05f;
  use_lrs = false;
  cur_lrate = .05f;
  decay = 1.0f;
  slw_sat = true;
  dk_mode = SU_THR;
}

void FastWtConSpec::Initialize() {
  min_obj_type = &TA_FastWtCon;
}

void FastWtConSpec::SetCurLrate(LeabraNetwork* net, int epoch) {
  LeabraConSpec::SetCurLrate(net, epoch);
  if(fast_wt.use_lrs)
    fast_wt.cur_lrate = fast_wt.lrate * lrate_sched.GetVal(epoch);
  else
    fast_wt.cur_lrate = fast_wt.lrate;
}

