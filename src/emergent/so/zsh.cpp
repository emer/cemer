// Copyright 2007-2018, Regents of the University of Colorado,
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



#include "zsh.h"

TA_BASEFUNS_CTORS_DEFN(ZshConSpec);

TA_BASEFUNS_CTORS_DEFN(MaxInConSpec);

void ZshConSpec::Initialize() {
  soft_wt_bound = true;
}

void MaxInConSpec::Initialize() {
  k_scl = 1.0f;
}
