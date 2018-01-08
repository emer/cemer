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

#ifndef BpConSpec_cuda_h
#define BpConSpec_cuda_h 1

// cuda version of spec -- no emergent / ta dependencies

// parent includes:
#include <ConSpec_cuda> // replace with actual parent
#include <BpNetworkState_cuda>

#include <State_cuda>

// member includes:
#include <BpUnitState_cuda>

#include <BpConSpec_mbrs.h>

// declare all other types mentioned but not required to include:

class BpConSpec_cuda : public ConSpec_cuda {
  // NVIDIA CUDA GPU Backpropagation connection specfications -- learning rules
INHERITED(ConSpec)
public:

#include <BpConSpec_core>

  BpConSpec_cuda() { Initialize_core(); }
};

#endif // BpConSpec_cuda_h
