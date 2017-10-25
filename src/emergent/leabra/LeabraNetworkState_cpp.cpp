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

// contains cpp non-inline (INIMPL) functions from _cpp.h

#include "LeabraNetworkState_cpp.h"

#include <LeabraConSpec_cpp>
#include <LeabraUnitSpec_cpp>
#include <LeabraLayerSpec_cpp>
#include <LeabraBiasSpec_cpp>

#include <State_cpp>

#include "LeabraNetworkState_core.cpp"

using namespace std;

bool LeabraNetworkState_cpp::NetworkLoadWeights_strm(istream& strm, bool quiet) {
  bool rval = inherited::NetworkLoadWeights_strm(strm, quiet);
  if(!rval) return rval;
  for(int li=0; li < n_layers_built; li++) {
    LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)GetLayerState(li);
    if(lay->lesioned()) continue;
    LEABRA_UNGP_STATE* lgpd = (LEABRA_UNGP_STATE*)lay->GetLayUnGpState(this);
    // these are important for saving in weights files -- keep them updated
    // todo: fixme!
    // lst->acts_m_avg = lay->acts_m_avg;
    // lst->acts_p_avg = lay->acts_p_avg;
    // lst->acts_p_avg_eff = lay->acts_p_avg_eff;
    // lgpd->acts_m_avg = lay->acts_m_avg;
    // lgpd->acts_p_avg = lay->acts_p_avg;
    // lgpd->acts_p_avg_eff = lay->acts_p_avg_eff;
  }
  return rval;
}
