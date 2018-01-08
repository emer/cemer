// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef BpUnitSpec_cuda_h
#define BpUnitSpec_cuda_h 1

// raw C++ (cuda) version of spec -- no emergent / ta dependencies

// parent includes:
#include <UnitSpec_cuda>
#include <BpNetworkState_cuda>
#include <BpUnitState_cuda>
#include <BpConSpec_cuda>

#include <State_cuda>

// member includes:

#include <BpUnitSpec_mbrs.h>

// declare all other types mentioned but not required to include:

class BpUnitSpec_cuda : public UnitSpec_cuda {
  // Plain C++ Backpropagation unit computation and parameters
INHERITED(UnitSpec)
public:

#include <BpUnitSpec_core>

  BpUnitSpec_cuda() { Initialize_core(); }
};

#endif // BpUnitSpec_cuda_h
