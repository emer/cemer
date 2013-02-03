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

#include "ECinLayerSpec.h"
#include <LeabraNetwork>

void ECinLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle == auto_m_cycles)
    RecordActM2(lay,net);
  inherited::Compute_CycleStats(lay, net);
}
