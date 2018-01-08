// Copyright 2017-22018 Regents of the University of Colorado,
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

#ifndef LeabraExtraLayerSpecs_cpp_h
#define LeabraExtraLayerSpecs_cpp_h 1

// parent includes:
#include <LeabraLayerSpec_cpp>

#include <State_cpp>

// member includes:

#include <LeabraExtraLayerSpecs_mbrs>

// declare all other types mentioned but not required to include:

class E_API ScalarValLayerSpec_cpp : public LeabraLayerSpec_cpp {
  // represents a scalar value using a coarse-coded distributed code over units.  the external input to the first unit is used to generate distributed inputs to the rest of the units, but unlike in earlier versions, all the units represent the distributed representation - the first unit is not just for display anymore, though it does contain the scalar readout val in misc_1 unit variable, and misc_2 has the minus phase read out
INHERITED(LeabraLayerSpec)
public:

#include <ScalarValLayerSpec>
  
  ScalarValLayerSpec_cpp() { Initialize_core(); }
};


class E_API ExtRewLayerSpec_cpp : public ScalarValLayerSpec_cpp {
  // computes external reward feedback: minus phase is zero, plus phase is reward value derived from network performance or other inputs (computed at start of 1+)
INHERITED(ScalarValLayerSpec)
public:

#include <ExtRewLayerSpec>

  ExtRewLayerSpec_cpp() { Initialize_core(); }
};


class E_API TwoDValLayerSpec_cpp : public LeabraLayerSpec_cpp {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid
INHERITED(LeabraLayerSpec)
public:

#include <TwoDValLayerSpec>

  TwoDValLayerSpec_cpp() { Initialize_core(); }
};


class E_API DecodeTwoDValLayerSpec_cpp : public TwoDValLayerSpec_cpp {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid
INHERITED(TwoDValLayerSpec)
public:

#include <DecodeTwoDValLayerSpec>

  DecodeTwoDValLayerSpec_cpp() { Initialize_core(); }
};

// not converting below:
// #include <MotorForceLayerSpec>
// #include <LeabraV1LayerSpec>
// #include <GPiSoftMaxLayerSpec>
// #include <SubiculumLayerSpec>

#endif // LeabraExtraLayerSpecs_cpp_h
