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

#ifndef RndSeed_h
#define RndSeed_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <int_Array>

// declare all other types mentioned but not required to include:


taTypeDef_Of(RndSeed);

class TA_API RndSeed : public taNBase {
  // #STEM_BASE ##CAT_Math random seeds: can control the random number generator to restart with the same pseudo-random sequence or get a new one
INHERITED(taNBase)
public:
  int_Array             seed;   // #READ_ONLY the seed, 624 elements long
  int                   mti;    // #READ_ONLY the index into the seed, also needs to be saved
  bool                  is_gauss_double_cached; // #READ_ONLY The state of the gaussian generator.
  double                cached_gauss_double;    // #READ_ONLY The state of the gaussian generator.

  virtual void   NewSeed();
  // #BUTTON #CONFIRM set the seed to a new random value (based on time and process id)
  virtual void   OldSeed();
  // #BUTTON #CONFIRM restore current seed to random num generator
  virtual void   GetCurrent();
  // #BUTTON #CONFIRM get the current seed in use by the generator
  virtual void   Init(ulong i);
  // initialize the seed based on given initializer

  virtual void  DMem_Sync(MPI_Comm comm);
   // #IGNORE synchronize seeds across all procs -- uses the first proc's seed

  void  Initialize();
  void  Destroy()                { CutLinks(); }
  TA_SIMPLE_BASEFUNS(RndSeed);
};

#endif // RndSeed_h
