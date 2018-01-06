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

#ifndef LeabraUnitState_cpp_h
#define LeabraUnitState_cpp_h 1

// raw C++ (cpp) version of state -- no emergent / ta dependencies

// parent includes:
#include <LeabraNetworkState_cpp>
#include <UnitState_cpp> // replace with actual parent

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

#define LEABRA_MAX_SPIKE_INTEG_WIN 10

class E_API LeabraUnitState_cpp : public UnitState_cpp {
  // #STEM_BASE ##CAT_Leabra Leabra unit variables, point-neuron approximation
INHERITED(UnitState)
public:

#include <LeabraUnitState_core>

  LeabraUnitState_cpp() { Initialize_core(); }
};

class E_API LeabraUnit : public LeabraUnitState_cpp {
  // LeabraUnit is an alias for LeabraUnitState_cpp for backward compatibility and simplicity in Programs
public:
};
  
#endif // LeabraUnitState_cpp_h
