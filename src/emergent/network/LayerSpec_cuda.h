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

#ifndef LayerSpec_cuda_h
#define LayerSpec_cuda_h 1

// parent includes:
#include <NetworkState_cuda>
#include <LayerState_cuda>

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LayerSpec_cuda);

class LayerSpec_cuda : public BaseSpec_cuda {
  // ##NO_TOKENS ##CAT_Network plain C++ version of LayerSpec
INHERITED(BaseSpec)
public:

#include <LayerSpec_core>

  LayerSpec_cuda() { Initialize_core(); }
};

#endif // LayerSpec_cuda_h
