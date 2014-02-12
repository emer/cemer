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

#include "CerebPfPcConSpec.h"

TA_BASEFUNS_CTORS_DEFN(CerebPfPcConSpec);

void CerebPfPcConSpec::Initialize() {
  wt_sig.dwt_norm = false;      // dwt norm VERY VERY bad for this!!
  stable_mix.stable_pct = 0.0f; // no point here either
  nerr_lrate = 0.1f;
}
