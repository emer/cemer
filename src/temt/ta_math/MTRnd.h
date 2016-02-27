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

/*
  Copyright (C) 2001-2009 Makoto Matsumoto and Takuji Nishimura.
  Copyright (C) 2009 Mutsuo Saito
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// NOTE:
// we're using the dcmt dynamic creation of different MT variants
// to allow parallel threads to each have their own independent
// random number generators.   we've converted the original c code
// into C++

// parent includes:
#include <taBase>
#include <taList>

// member includes:


taTypeDef_Of(MTRndPar);

class TA_API MTRndPar : public taBase {
  // #STEM_BASE #NO_UPDATE_AFTER ##INLINE ##INLINE_DUMP ##CAT_MATH one mersenne twister pseudo-random number generator, with dynamically generated parameters -- these can be dynamically created for parallel use by separate threads
INHERITED(taBase)
public:
  uint32_t aaa;
  int      mm;
  int      nn;                  // number of values in state vector
  int      rr;
  int      ww;
  uint32_t wmask;
  uint32_t umask;
  uint32_t lmask;
  int      shift0;
  int      shift1;
  int      shiftB;
  int      shiftC;
  uint32_t maskB;
  uint32_t maskC;
  int      mti;                 // current index within state vector
  bool     double_cached;       // if true, there is a cached double random var here
  double   cached_double;       // this is the cached var

  bool GenerateParams(int w, int p, uint32_t seed);
  // search for a set of parameters for PRNG -- w is number of bits in one word (31 or 32 only), p is exponent of the period -- the period should be 2^p-1 -- usable values are: 521 607 1279 2203 2281 3217 4253 4423 9689 9941 11213 19937 21701 23209 44497 -- seed initializes an internal PRNG used in creating the PRNG parameters -- this can take a very long time to run, O(p^3)
  
  bool GenerateParamsID(int w, int p, int id, uint32_t seed);
  // embeds ordinal ID (16 bit only) number into generator search for a set of parameters for PRNG -- w is number of bits in one word (31 or 32 only), p is exponent of the period -- the period should be 2^p-1 -- usable values are: 521 607 1279 2203 2281 3217 4253 4423 9689 9941 11213 19937 21701 23209 44497 -- seed initializes an internal PRNG used in creating the PRNG parameters -- this can take a very long time to run, O(p^3)

  void  AllocState();
  // if a valid set of parameters have been loaded here, then we can allocate state for actually using this PRNG

  void  InitSeed(uint32_t seed);
  // initialize state vector for this PRNG from given seed

  uint32_t GetCurSeed();
  // get current state seed value

  uint32_t GenRandInt32();
  // generates a random number on [0,0xffffffff]-interval
  inline double GenRandReal1()
  { return GenRandInt32() * (1.0/4294967295.0); }
  // generates a random number on [0,1]-real-interval
  inline double GenRandReal2()
  { return GenRandInt32() * (1.0/4294967296.0); }
  // generates a random number on [0,1)-real-interval
  inline double GenRandReal3()
  { return (((double)GenRandInt32()) + 0.5) * (1.0/4294967296.0); }
  // generates a random number on (0,1)-real-interval
  inline double GenRandRes53()
  { ulong a=GenRandInt32() >> 5; ulong b=GenRandInt32() >> 6;
    return(a*67108864.0+b)*(1.0/9007199254740992.0); }
  // generates a random number on [0,1) with 53-bit resolution

  double GenRandGaussDev();
  // generate a gaussian-distributed random deviate -- generates 2x at a time, so it caches the 2nd one for greater efficiency -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  TA_SIMPLE_BASEFUNS(MTRndPar);
protected:
  uint32_t* state;              // internal state for PRNG generator -- these are not params but rather internal state variables used dynamically during generation

private:
  void  Initialize();
  void  Destroy();
};

taTypeDef_Of(MTRndPar_List);

class TA_API MTRndPar_List: public taList<MTRndPar> {
  // ##CAT_Math a list of parameters for one mersenne twister pseudo-random number generator -- these can be dynamically created 
INHERITED(taList<MTRndPar>)
public:

  bool GenerateParamsID(int w, int p, int n_ids, uint32_t seed);
  // generate n_ids of DCMT parameters with given parameters, embedding the ID number into each: w is number of bits in one word (31 or 32 only), p is exponent of the period -- the period should be 2^p-1 -- usable values are: 521 607 1279 2203 2281 3217 4253 4423 9689 9941 11213 19937 21701 23209 44497 -- seed initializes an internal PRNG used in creating the PRNG parameters -- this can take a very long time to run, O(p^3)

  void  InitSeeds(uint32_t seed);
  // initialize state vector for all PRNG's in the list from given seed
 
  TA_BASEFUNS_NOCOPY(MTRndPar_List);
private:
  void          Initialize() { SetBaseType(&TA_MTRndPar); }
  void          Destroy() {}
};


taTypeDef_Of(MTRnd);

class TA_API MTRnd : public taNBase {
  // A container for the Mersenne Twister (MT19937) random number generator by Makoto Matsumoto and Takuji Nishimura -- maintains list of dynamically created MT generators, each designed to be as different from each other as possible -- because generation takes a very long time, we initialize from a saved list of generators
INHERITED(taNBase)
public:
  static const int      max_gens; // maximum number of generators -- defaults to 100 -- cannot use more than this number of threads in parallel unless new parameters are generated and saved in the code
  static MTRndPar_List  mtrnds;
  // list of MT generators -- of max_gens size

  static uint32_t  GetTimePidSeed();
  // get a new random seed produced from the time and the process id

  static void  InitSeeds(uint32_t seed);
  // initialize state vector for all PRNG's in the list from given seed

  static MTRndPar* GetRnd(int thr_no = -1);
  // get generator for specific thread -- return null and emit error if beyond range of list of generators

  static uint32_t GetCurSeed(int thr_no = -1)
  { MTRndPar* rng = GetRnd(thr_no); if(!rng) return 0;
    return rng->GetCurSeed(); }
  // returns current seed -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static uint32_t GenRandInt32(int thr_no = -1)
  { MTRndPar* rng = GetRnd(thr_no); if(!rng) return 0;
    return rng->GenRandInt32(); }
  // generates a random number on [0,0xffffffff]-interval -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double GenRandReal1(int thr_no = -1)
  { MTRndPar* rng = GetRnd(thr_no); if(!rng) return 0.0;
    return rng->GenRandReal1(); }
  // generates a random number on [0,1]-real-interval -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double GenRandReal2(int thr_no = -1)
  { MTRndPar* rng = GetRnd(thr_no); if(!rng) return 0.0;
    return rng->GenRandReal2(); }
  // generates a random number on [0,1)-real-interval -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double GenRandReal3(int thr_no = -1)
  { MTRndPar* rng = GetRnd(thr_no); if(!rng) return 0.0;
    return rng->GenRandReal3(); }
  // generates a random number on (0,1)-real-interval -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double GenRandRes53(int thr_no = -1)
  { MTRndPar* rng = GetRnd(thr_no); if(!rng) return 0.0;
    return rng->GenRandRes53(); }
  // generates a random number on [0,1) with 53-bit resolution -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double GenRandGaussDev(int thr_no = -1)
  { MTRndPar* rng = GetRnd(thr_no); if(!rng) return 0.0;
    return rng->GenRandGaussDev(); }
  // generate a gaussian-distributed random deviate -- generates 2x at a time, so it caches the 2nd one for greater efficiency -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static void  LoadInitParams();
  // #IGNORE load the initial parameters that were previously generated and saved within the source code -- this is called at an initialization step
  static void  GenInitParams(int n_gens, const String& save_file_name);
  // #IGNORE generate a new set of generator parameters, saving result to given file name
    
  TA_ABSTRACT_BASEFUNS_NOCOPY(MTRnd) //
private:
  void Initialize();
  void Destroy();
};


#endif // MTRnd_h
