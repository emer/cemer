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

#ifndef LeabraLayerSpec_cpp_h
#define LeabraLayerSpec_cpp_h 1

// raw C++ (cpp) version of spec -- ideally no emergent / ta dependencies

// parent includes:
#include <LayerSpec_cpp> // replace with actual parent

#include <LeabraAvgMax_cpp>

#include <LeabraNetworkState_cpp>
#include <LeabraLayerState_cpp>
#include <LeabraUnGpState_cpp>
#include <LeabraPrjnState_cpp>
#include <LeabraUnitState_cpp>

#include <State_cpp>

// member includes:

#include <LeabraLayerSpec_mbrs>

// declare all other types mentioned but not required to include:

class LeabraLayerSpec_cpp : public LayerSpec_cpp {
  // #STEM_BASE ##CAT_Leabra Leabra layer specs, computes inhibitory input for all units in layer
INHERITED(LayerSpec)
public:

#include <LeabraLayerSpec_core>

  LeabraLayerSpec_cpp() { Initialize_core(); }
};

#endif // LeabraLayerSpec_cpp_h
