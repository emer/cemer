// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef MTRndState_h
#define MTRndState_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(MTRndState);

class TA_API MTRndState
{
public:
  MTRndState();
  void reset(); // Call when seeding the RNG to reset all state.

  static const int N = 624;

  uint   mt[N]; // the array for the state vector
  int    mti;   // mti==N+1 means mt[N] is not initialized
  bool   is_gauss_double_cached;
  double cached_gauss_double;
};

#endif // MTRndState_h
