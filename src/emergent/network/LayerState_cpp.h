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

#ifndef LayerState_h
#define LayerState_h 1

// parent includes:
#include <NetworkState_cpp>
#include <PrjnState_cpp>

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class E_API LayerState_cpp {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network layer state variables -- for use in computational State code
public:

#include <Layer_core>
#include <LayerState_core>


  LayerState_cpp() { Initialize_core(); }
};

#endif // LayerState_h
