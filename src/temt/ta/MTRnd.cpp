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

#include "MTRnd.h"

#include <taMisc>
#include <cmath>

/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/

// Initialize the static MT-RNG state instance.

MTRndState* MTRnd::states = NULL;
int MTRnd::n_states = 0;

void MTRnd::alloc_states() {
  if(states)
    delete [] states;
  n_states = taMisc::thread_defaults.cpus;
  n_states = MAX(1, n_states);
  states = new MTRndState[n_states]; // note -- never deleted!
}

/* initializes mt[N] with a seed */
void MTRnd::seed(uint s, int thr_no) {
  if(states == NULL)
    alloc_states();
  MTRndState& state = get_state(thr_no);
  state.reset();
  state.mt[0] = s & 0xffffffffUL;
  for (state.mti = 1; state.mti < N; state.mti++) {
    state.mt[state.mti] =
      (1812433253UL * (state.mt[state.mti-1] ^ (state.mt[state.mti-1] >> 30)) + state.mti);
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    state.mt[state.mti] &= 0xffffffffUL;
    /* for >32 bit machines */
  }
  if(thr_no == 0)
    seed_to_threads(s);
}

uint MTRnd::seed_time_pid(int thr_no) {
  //NOTE: this is a bit simpler than the 3.2 version
  int pid = taMisc::ProcessId();
  int tc = taMisc::TickCount(); // ms since system started
  ulong sdval = (ulong)tc * (ulong)pid;
  sdval = sdval & 0xffffffffUL;
  seed(sdval, thr_no);          // use microseconds..
  return sdval;
}

void MTRnd::seed_to_threads(uint s) {
  for(int i=1; i<n_states; i++) {
    ulong sdval = (ulong)sdval + (ulong)(i * 7); // lame magic number 7.. :)
    sdval = sdval & 0xffffffffUL;
    seed(sdval, i); 
  }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void MTRnd::seed_array(uint init_key[], int key_length, int thr_no) {
  int i, j, k;
  seed(19650218UL);
  MTRndState& state = get_state(thr_no);
  i=1; j=0;
  k = (N>key_length ? N : key_length);
  for (; k; k--) {
    state.mt[i] = (state.mt[i] ^ ((state.mt[i-1] ^ (state.mt[i-1] >> 30)) * 1664525UL))
      + init_key[j] + j; /* non linear */
    state.mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++; j++;
    if (i>=N) { state.mt[0] = state.mt[N-1]; i=1; }
    if (j>=key_length) j=0;
  }
  for (k=N-1; k; k--) {
    state.mt[i] = (state.mt[i] ^ ((state.mt[i-1] ^ (state.mt[i-1] >> 30)) * 1566083941UL))
      - i; /* non linear */
    state.mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++;
    if (i>=N) { state.mt[0] = state.mt[N-1]; i=1; }
  }

  state.mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

void
MTRnd::set_state(const MTRndState &newState, int thr_no)
{
  MTRndState& state = get_state(thr_no);
  state.reset();
  state = newState;
  if(thr_no == 0) {
    seed_to_threads(state.mt[0]);
  }
}

/* generates a random number on [0,0xffffffff]-interval */

uint MTRnd::genrand_int32(int thr_no) {
  MTRndState& state = get_state(thr_no);
  ulong y;
  static ulong mag01[2]={0x0UL, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (state.mti >= N) { /* generate N words at one time */
    int kk;

    if (state.mti == N+1)   /* if init_genrand() has not been called, */
      seed_time_pid(thr_no);  // (5489UL); a default initial seed is used

    for (kk=0;kk<N-M;kk++) {
      y = (state.mt[kk]&UPPER_MASK)|(state.mt[kk+1]&LOWER_MASK);
      state.mt[kk] = state.mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (;kk<N-1;kk++) {
      y = (state.mt[kk]&UPPER_MASK)|(state.mt[kk+1]&LOWER_MASK);
      state.mt[kk] = state.mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (state.mt[N-1]&UPPER_MASK)|(state.mt[0]&LOWER_MASK);
    state.mt[N-1] = state.mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    state.mti = 0;
  }

  y = state.mt[state.mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
int MTRnd::genrand_int31(int thr_no) {
  return (int)(genrand_int32(thr_no)>>1);
}

/* generates a random number on [0,1]-real-interval */
double MTRnd::genrand_real1(int thr_no) {
  return genrand_int32(thr_no)*(1.0/4294967295.0);
  /* divided by 2^32-1 */
}

/* generates a random number on [0,1)-real-interval */
double MTRnd::genrand_real2(int thr_no) {
  return genrand_int32(thr_no)*(1.0/4294967296.0);
  /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double MTRnd::genrand_real3(int thr_no) {
  return (((double)genrand_int32(thr_no)) + 0.5)*(1.0/4294967296.0);
  /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double MTRnd::genrand_res53(int thr_no) {
  ulong a=genrand_int32(thr_no)>>5, b=genrand_int32(thr_no)>>6;
  return(a*67108864.0+b)*(1.0/9007199254740992.0);
}
/* These real versions are due to Isaku Wada, 2002/01/09 added */

double
MTRnd::genrand_gauss_dev_double(int thr_no)
{
  MTRndState& state = get_state(thr_no);
  
  if (state.is_gauss_double_cached) {
    state.is_gauss_double_cached = false;
    return state.cached_gauss_double;
  }

  double fac, r, v1, v2;
  do {
    v1 = 2.0 * MTRnd::genrand_res53(thr_no) - 1.0;
    v2 = 2.0 * MTRnd::genrand_res53(thr_no) - 1.0;
    r = v1 * v1 + v2 * v2;
  }
  while (r >= 1.0 || r == 0);

  fac = sqrt(-2.0 * log(r) / r);
  state.cached_gauss_double = v1 * fac;
  state.is_gauss_double_cached = true;
  return v2 * fac;
}
