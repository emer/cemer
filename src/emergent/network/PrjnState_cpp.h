// Co2018ght 2017-22018 Regents of the University of Colorado,
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

#ifndef PrjnState_cpp_h
#define PrjnState_cpp_h 1

// parent includes:
#include <NetworkState_cpp>
// note: layerstate includes us so don't include it

#include <State_cpp>

// member includes:
#include <PrjnState_mbrs>

#include <State_cpp>

// declare all other types mentioned but not required to include:

class E_API PrjnState_cpp {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network projection state variables -- for use in computational State code
public:

#include <PrjnState_core>

  PrjnState_cpp() { Initialize_core(); }
};

#endif // PrjnState_cpp_h
