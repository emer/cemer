// Copyright 2017, Regents of the University of Colorado,
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

#ifndef NetworkState_cpp_h
#define NetworkState_cpp_h 1

// State classes define full implementation of code -- this is included in all other
// State classes so it cannot depend on any of them -- every other class uses
// NetworkState to access their State variables which are allocated here

#include "network_def.h"

#include <State_main>
// including random seems to get all the relevant emergent background for _cpp files
#include <Random>

// so far not entirely possible to eliminate dependency on emergent entirely -- need
// access to the network class for threads.SyncSpin
class Network; //

#include <State_cpp>

// this defines classes that are members of NetworkState -- as _cpp
#include <Network_mbrs>

class NetworkState_cpp {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network plain C++ implementation of NetworkState 
public:
  
// this directly defines a bunch of shared vars with Network
#include <Network_core>
// and common state between C++ and _cuda
#include <NetworkState_core>

  Network*      own_net; // #IGNORE the network that owns this network state
  
  inline bool  NetStateMalloc(void** ptr, size_t sz) const {
    // alignment -- 64 = 64 byte (not bit) -- this is needed for Phi MIC but not clear
    // that it is useful for AVX2??  anyway, better safe than sorry?
    // 8/23/16 -- unnec to do the 64 byte align -- even any align may be not needed
    // and windows requires a different free based on the align alloc type, so
    // we are just standardizing on 16 byte align for all..
    // if(sz > 1024) {
#ifdef TA_OS_WIN
    *ptr = _aligned_malloc(sz, 16);
#else
    posix_memalign(ptr, 16, sz);
#endif
    // }
    // else {                        // don't bother with align for small guys..
    //   *ptr = malloc(sz);
    // }
    if(!*ptr) {
      StateError("Network::net_aligned_alloc memory allocation error! usually fatal -- please quit!  maybe your network is too big to fit into RAM?");
      return false;
    }
    return true;
  }
  // #IGNORE properly (maximally) aligned memory allocation routine to given pointer of given number of bytes -- alignment is (currently) 64 bytes
  
  inline bool  NetStateFree(void** ptr) const {
    if(ptr && *ptr) {
#ifdef TA_OS_WIN
      _aligned_free(*ptr);
#else
      free(*ptr);
#endif
      *ptr = NULL;
      return true;
    }
    return false;
  }
  // #IGNORE free previously malloc'd memory, and set *ptr = NULL


  NetworkState_cpp() { Initialize_net_core(); Initialize_core(); }
};

#endif // NetworkState_cpp_h
