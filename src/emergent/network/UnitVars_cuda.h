// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef UnitVars_cuda_h
#define UnitVars_cuda_h 1

// parent includes:
#include <UnitVars_core>

// member includes:

// declare all other types mentioned but not required to include:

class UnitVars_cuda : public UnitVars_core {
  // cuda version of UnitVars
public:
  void*   unit_spec; // keep our unit vars aligned with C++ one..
};

#endif // UnitVars_cuda_h
