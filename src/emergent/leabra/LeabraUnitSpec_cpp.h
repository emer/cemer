// Copyright 2017-22018 Regents of the University of Colorado,
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

#ifndef LeabraUnitSpec_cpp_h
#define LeabraUnitSpec_cpp_h 1

// raw C++ (cpp) version of spec -- ideally no emergent / ta dependencies

// parent includes:
#include <UnitSpec_cpp> // replace with actual parent

#include <LeabraNetworkState_cpp>
#include <LeabraUnitState_cpp>
#include <LeabraUnGpState_cpp>
#include <LeabraLayerState_cpp>
#include <LeabraConState_cpp>

#include <State_cpp>

// member includes:

#include <LeabraUnitSpec_mbrs>

// declare all other types mentioned but not required to include:

class E_API LeabraUnitSpec_cpp : public UnitSpec_cpp {
  // #STEM_BASE ##CAT_Leabra Leabra unit specifications, point-neuron approximation
INHERITED(UnitSpec)
public:

#include <LeabraUnitSpec_core>

  LeabraUnitSpec_cpp() { Initialize_core(); }
};

#endif // LeabraUnitSpec_cpp_h
