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

#ifndef LeabraBiasSpec_cuda_h
#define LeabraBiasSpec_cuda_h 1

// raw C++ (cuda) version of spec -- no emergent / ta dependencies

// parent includes:
#include <LeabraConSpec_cuda> // replace with actual parent

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

class LeabraBiasSpec_cuda : public LeabraConSpec_cuda {
  // Leabra bias-weight connection specs (bias wts are a little bit special)
INHERITED(LeabraConSpec)
public:

#include <LeabraBiasSpec_core>

  LeabraBiasSpec_cuda() { Initialize_core(); }
};

#endif // LeabraBiasSpec_cuda_h
