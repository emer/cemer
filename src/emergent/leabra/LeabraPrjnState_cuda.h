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

#ifndef LeabraPrjnState_cuda_h
#define LeabraPrjnState_cuda_h 1

// raw C++ (cpp) version of state -- no emergent / ta dependencies

// parent includes:
#include <PrjnState_cuda>

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

class LeabraPrjnState_cuda : public PrjnState_cuda {
  // #STEM_BASE ##CAT_Leabra leabra specific projection -- has special variables at the projection-level
INHERITED(PrjnState)
public:

#include <LeabraPrjnState_core>

  LeabraPrjnState_cuda() { Initialize_core(); }
};

#endif // LeabraPrjnState_cuda_h
