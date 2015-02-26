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

#ifndef MTRnd_h
#define MTRnd_h 1

// parent includes:

// member includes:
#include <MTRndState>

// declare all other types mentioned but not required to include:

class TA_API MTRnd {
  // #IGNORE A container for the Mersenne Twister (MT19937) random number generator by Makoto Matsumoto and Takuji Nishimura -- maintains multiple states that can be used for threaded calls (up to number of CPU's on machine)
public:
  static const  int N = 624;
  static const  int M = 397;
  static const  uint MATRIX_A = 0x9908b0dfU;    // constant vector a
  static const  uint UPPER_MASK = 0x80000000U;  // most significant w-r bits
  static const  uint LOWER_MASK = 0x7fffffffU;  // least significant r bits
  static int    n_states;                       // number of states allocated

private:
  static MTRndState* states;    // state -- this is allocated up to number of CPU's as listed in taMisc::thread_defaults.cpus to support separate threaded calls

public:
  static void   seed(uint s, int thr_no = 0);
  // seed the generator with given seed value -- when called on thr_no 0, it calls seed_to_threads to spread to other threads
  static uint   seed_time_pid(int thr_no = 0);
  // seed the generator with a random seed produced from the time and the process id
  static void   seed_array(uint init_key[], int key_length, int thr_no = 0);
  // seed with given initial key
  static void   seed_to_threads(uint s);  // seed threads (1..n_states-1) based on seed
  static void   alloc_states();     // allocate the states if needed

  static inline MTRndState& get_state(int thr_no = 0) {
    if(!states) alloc_states();
    if(thr_no >= n_states)
      thr_no = 0;
    return states[thr_no];
  }
  static void   set_state(const MTRndState &newState, int thr_no = 0);
  // set the state of the MT-RNG -- if called on thr_no == 0, does seed_to_threads based on 0 element in the seed

  static uint   genrand_int32(int thr_no = 0);
  // generates a random number on [0,0xffffffff]-interval -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static int    genrand_int31(int thr_no = 0);
  // generates a random number on [0,0x7fffffff]-interval -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double genrand_real1(int thr_no = 0);
  // generates a random number on [0,1]-real-interval -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double genrand_real2(int thr_no = 0);
  // generates a random number on [0,1)-real-interval -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double genrand_real3(int thr_no = 0);
  // generates a random number on (0,1)-real-interval -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double genrand_res53(int thr_no = 0);
  // generates a random number on [0,1) with 53-bit resolution -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)

  static double genrand_gauss_dev_double(int thr_no = 0);
  // generate a gaussian-distributed random deviate -- generates 2x at a time, so it caches the 2nd one for greater efficiency -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
};


#endif // MTRnd_h
