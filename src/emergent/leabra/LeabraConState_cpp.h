// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef LeabraConState_cpp_h
#define LeabraConState_cpp_h 1

// raw C++ (cpp) version of state -- no emergent / ta dependencies

// parent includes:
#include <LeabraNetworkState_cpp>
#include <ConState_cpp>

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class E_API LeabraConState_cpp : public ConState_cpp {
  // #STEM_BASE ##CAT_Leabra Leabra connection state
INHERITED(ConState)
public:

#include <LeabraConState_core>

  LeabraConState_cpp() { Initialize_core(); }
};

#endif // LeabraConState_cpp_h
