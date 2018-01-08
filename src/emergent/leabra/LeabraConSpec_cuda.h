// Co2018ght 2014-2017, Regents of the University of Colorado,
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

#ifndef LeabraConSpec_cuda_h
#define LeabraConSpec_cuda_h 1

// raw C++ (cuda) version of spec -- ideally no emergent / ta dependencies

// parent includes:
#include <ConSpec_cuda> // replace with actual parent

#include "ta_vector_ops.h"

#include <LeabraNetworkState_cuda>
#include <LeabraConState_cuda>
#include <LeabraUnitState_cuda>
#include <LeabraLayerState_cuda>

#include <State_cuda>

// member includes:

#include <LeabraConSpec_mbrs>

// declare all other types mentioned but not required to include:

class LeabraConSpec_cuda : public ConSpec_cuda {
  // #STEM_BASE ##CAT_Leabra raw C++ version of Leabra connection specs
INHERITED(ConSpec)
public:

#include <LeabraConSpec_core>

  LeabraConSpec_cuda() { Initialize_core(); }
};
  
#endif // LeabraConSpec_cuda_h
