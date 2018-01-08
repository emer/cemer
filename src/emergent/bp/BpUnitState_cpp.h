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

#ifndef BpUnitState_cpp_h
#define BpUnitState_cpp_h 1

// raw C++ (cpp) version of state -- no emergent / ta dependencies

// parent includes:
#include <BpNetworkState_cpp>
#include <UnitState_cpp> // replace with actual parent

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class E_API BpUnitState_cpp : public UnitState_cpp {
  // Backprop unit state values
INHERITED(UnitState)
public:

#include <BpUnitState_core>

  BpUnitState_cpp() { Initialize_core(); }
};

class E_API BpUnit : public BpUnitState_cpp {
  // BpUnit is an alias for BpUnitState_cpp for backward compatibility and simplicity in Programs
public:
};
  

#endif // BpUnitState_cpp_h
