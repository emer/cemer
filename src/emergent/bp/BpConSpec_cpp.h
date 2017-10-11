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

#ifndef BpConSpec_cpp_h
#define BpConSpec_cpp_h 1

// raw C++ (cpp) version of spec -- ideally no emergent / ta dependencies

// parent includes:
#include <ConSpec_cpp>
#include <BpNetworkState_cpp>

// member includes:
#include <BpUnitState_cpp>

#include <State_cpp>

// #include <BpConSpec_mbrs.h>

// declare all other types mentioned but not required to include:

class BpConSpec_cpp : public ConSpec_cpp {
  // Plain C++ Backpropagation connection specfications -- learning rules
INHERITED(ConSpec)
public:

#include <BpConSpec_core>

  BpConSpec_cpp() { Initialize_core(); }
};

#endif // BpConSpec_cpp_h
