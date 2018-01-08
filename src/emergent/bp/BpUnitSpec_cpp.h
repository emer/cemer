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

#ifndef BpUnitSpec_cpp_h
#define BpUnitSpec_cpp_h 1

// raw C++ (cpp) version of spec -- ideally no emergent / ta dependencies

// parent includes:
#include <UnitSpec_cpp> // replace with actual parent
#include <BpNetworkState_cpp>
#include <BpUnitState_cpp>
#include <BpConSpec_cpp>

#include <State_cpp>

// member includes:

#include <BpUnitSpec_mbrs.h>

// declare all other types mentioned but not required to include:

class E_API BpUnitSpec_cpp : public UnitSpec_cpp {
  // Plain C++ Backpropagation unit computation and parameters
INHERITED(UnitSpec)
public:

#include <BpUnitSpec_core>

  BpUnitSpec_cpp() { Initialize_core(); }
};

#endif // BpUnitSpec_cpp_h
