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

#ifndef LeabraUnitVars_h
#define LeabraUnitVars_h 1

// parent includes:
#include <UnitVars>

// member includes:

// declare all other types mentioned but not required to include:

// NOTE: this defines how big these spike integration buffers can be
#define LEABRA_MAX_SPIKE_INTEG_WIN 10

eTypeDef_Of(LeabraUnitVars);

class E_API LeabraUnitVars : public UnitVars {
  // #STEM_BASE ##CAT_Leabra Leabra unit variables, point-neuron approximation
public:
  // directly include variables here!
  #include "LeabraUnitVars_core.h"
};

#endif // LeabraUnitVars_h
