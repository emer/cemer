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

#ifndef LeabraBiasSpec_cpp_h
#define LeabraBiasSpec_cpp_h 1

// raw C++ (cpp) version of spec -- ideally no emergent / ta dependencies

// parent includes:
#include <LeabraConSpec_cpp> // replace with actual parent

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class E_API LeabraBiasSpec_cpp : public LeabraConSpec_cpp {
  // Leabra bias-weight connection specs (bias wts are a little bit special)
INHERITED(LeabraConSpec)
public:

#include <LeabraBiasSpec_core>

  LeabraBiasSpec_cpp() { Initialize_core(); }
};

#endif // LeabraBiasSpec_cpp_h
