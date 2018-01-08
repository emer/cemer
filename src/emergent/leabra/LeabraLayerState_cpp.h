// Copyright 2017-2018, Regents of the University of Colorado,
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

#ifndef LeabraLayerState_cpp_h
#define LeabraLayerState_cpp_h 1

// raw C++ (cpp) version of state -- no emergent / ta dependencies

// parent includes:
#include <LeabraNetworkState_cpp>
#include <LayerState_cpp>
#include <LeabraUnGpState_cpp>

#include <LeabraAvgMax_cpp>
#include <LeabraLayer_mbrs>

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class E_API LeabraLayerState_cpp : public LayerState_cpp {
  // #STEM_BASE ##CAT_Leabra pure C++ state version of leabra layer state values
INHERITED(LayerState)
public:

#include <LeabraLayer_core>
#include <LeabraLayerState_core>

  LeabraLayerState_cpp() { Initialize_lay_core(); Initialize_core(); }
};

#endif // LeabraLayerState_cpp_h
