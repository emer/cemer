// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef LeabraUnitState_cuda_h
#define LeabraUnitState_cuda_h 1

// raw C++ (cuda) version of state -- no emergent / ta dependencies

// parent includes:
#include <LeabraNetworkState_cuda>
#include <UnitState_cuda> // replace with actual parent

#include <State_cuda>

// member includes:

#define LEABRA_MAX_SPIKE_INTEG_WIN 10

class LeabraUnitState_cuda : public UnitState_cuda {
  // #STEM_BASE ##CAT_Leabra Leabra unit variables, point-neuron approximation
INHERITED(UnitState)
public:

#include <LeabraUnitState_core>

  LeabraUnitState_cuda() { Initialize_core(); }
};

#endif // LeabraUnitState_cuda_h
