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

#ifndef BpNetworkState_cuda_h
#define BpNetworkState_cuda_h 1

// raw C++ (cuda) version of state -- no emergent / ta dependencies

// parent includes:
#include <NetworkState_cuda> // replace with actual parent

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

class BpNetworkState_cuda : public NetworkState_cuda {
  // NVIDIA CUDA GPU Backpropagation network state -- key computational code
INHERITED(NetworkState)
public:

#include <BpNetwork_core>
#include <BpNetworkState_core>

  INLINE BpNetworkState_cuda() { Initialize_net_core(); }
};

#endif // BpNetworkState_cuda_h