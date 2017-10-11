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

#ifndef UnitState_cpp_h
#define UnitState_cpp_h 1

// parent includes:
#include <NetworkState_cpp>

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class UnitState_cpp {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network Generic unit variables -- basic computational unit of a neural network (e.g., a neuron-like processing unit) -- Unit contains all the structural information, and this class just contains the computationally-relevant variables
public:

#include <UnitState_core>

  UnitState_cpp() { Initialize_core(); }
};

#endif // UnitState_cpp_h
