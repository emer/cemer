// Copyright 2016-2017, Regents of the University of Colorado,
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

#ifndef ConState_cuda_h
#define ConState_cuda_h 1

// parent includes:
#include <NetworkState_cuda>
#include <PrjnState_cuda>

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

class ConState_cuda  {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network NVIDIA CUDA version of ConState -- has same memory layout and cuda-usable accessors
public:

  #include <ConState_core>
  
};

#endif // ConState_cuda_h
