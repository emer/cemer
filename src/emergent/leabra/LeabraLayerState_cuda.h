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

#ifndef LeabraLayerState_cuda_h
#define LeabraLayerState_cuda_h 1

// raw C++ (cuda) version of state -- no emergent / ta dependencies

// parent includes:
#include <LeabraNetworkState_cuda>
#include <LayerState_cuda> 
#include <LeabraUnGpState_cuda>

#include <LeabraAvgMax_cuda>
#include <LeabraLayer_mbrs>

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

class LeabraLayerState_cuda : public LayerState_cuda {
  // #STEM_BASE ##CAT_Leabra CUDA state version of leabra layer state values
INHERITED(LayerState)
public:

#include <LeabraLayer_core>
#include <LeabraLayerState_core>

  LeabraLayerState_cuda() { Initialize_lay_core(); Initialize_core(); }
};

#endif // LeabraLayerState_cuda_h
