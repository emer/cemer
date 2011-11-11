// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef TA_MTRND_H
#define TA_MTRND_H

#include "ta_def.h"

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

class TA_API MTRnd {
  // #IGNORE A container for the Mersenne Twister (MT19937) random number generator by Makoto Matsumoto and Takuji Nishimura
public:
  static const  int N = 624;
  static const  int M = 397;
  static const  uint MATRIX_A = 0x9908b0dfU;    // constant vector a
  static const  uint UPPER_MASK = 0x80000000U;  // most significant w-r bits
  static const  uint LOWER_MASK = 0x7fffffffU;  // least significant r bits

private:
  static MTRndState state;

public:
  static void   seed(uint s);    // seed the generator with given seed value
  static uint   seed_time_pid(); // seed the generator with a random seed produced from the time and the process id
  static void   seed_array(uint init_key[], int key_length); // seed with given initial key

  static const  MTRndState & get_state(); // retrieve the current state of the MT-RNG
  static void   set_state(const MTRndState &newState); // set the state of the MT-RNG

  static uint   genrand_int32(); // generates a random number on [0,0xffffffff]-interval
  static int    genrand_int31(); // generates a random number on [0,0x7fffffff]-interval
  static double genrand_real1(); // generates a random number on [0,1]-real-interval
  static double genrand_real2(); // generates a random number on [0,1)-real-interval
  static double genrand_real3(); // generates a random number on (0,1)-real-interval
  static double genrand_res53(); // generates a random number on [0,1) with 53-bit resolution

  static double genrand_gauss_dev_double();
};

#endif // MTRND_H
