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

#ifndef LeabraUnitSpec_cuda_h
#define LeabraUnitSpec_cuda_h 1

// raw C++ (cuda) version of spec -- no emergent / ta dependencies

// parent includes:
#include <UnitSpec_cuda> // replace with actual parent

#include <State_cuda>

// member includes:

#include <LeabraUnitSpec_mbrs.h>

// declare all other types mentioned but not required to include:

class LeabraUnitSpec_cuda : public UnitSpec_cuda {
  // <describe here in full detail in one extended line comment>
INHERITED(UnitSpec)
public:

#include <LeabraUnitSpec_core>

  LeabraUnitSpec_cuda() { Initialize_core(); }
};

#endif // LeabraUnitSpec_cuda_h
