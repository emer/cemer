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

#ifndef LeabraConState_cuda_h
#define LeabraConState_cuda_h 1

// raw C++ (cuda) version of state -- no emergent / ta dependencies

// parent includes:
#include <LeabraNetworkState_cuda>
#include <ConState_cuda>

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

class LeabraConState_cuda : public ConState_cuda {
  // #STEM_BASE ##CAT_Leabra GPU Leabra connection state
INHERITED(ConState)
public:

#include <LeabraConState_core>

  LeabraConState_cuda() { Initialize_core(); }
};

#endif // LeabraConState_cuda_h
