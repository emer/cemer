// Copyright 2017-20188 Regents of the University of Colorado,
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

#ifndef LayerSpec_cpp_h
#define LayerSpec_cpp_h 1

// parent includes:
#include <NetworkState_cpp>
#include <LayerState_cpp>

#include <State_cpp>

// member includes:

// declare all other types mentioned but not required to include:

class E_API LayerSpec_cpp : public BaseSpec_cpp {
  // ##NO_TOKENS ##CAT_Network plain C++ version of LayerSpec
INHERITED(BaseSpec)
public:

#include <LayerSpec_core>

  LayerSpec_cpp() { Initialize_core(); }
};

#endif // LayerSpec_cpp_h
