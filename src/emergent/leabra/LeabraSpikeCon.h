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

#ifndef LeabraSpikeCon_h
#define LeabraSpikeCon_h 1

// parent includes:
#include <LeabraSRAvgCon>

// member includes:

// declare all other types mentioned but not required to include:

// turn this on to enable various debugging things..
// #define XCAL_DEBUG 1

eTypeDef_Of(LeabraSpikeCon);

class LeabraSpikeCon : public LeabraSRAvgCon {
  // #STEM_BASE ##CAT_Leabra Leabra connection for spike-based learning
public:
  float		sravg_ss;	// #NO_SAVE super-short time-scale average of sender and receiver activation product over time (just for smoothing over transients) -- cascaded into sravg_s
  float		nmda;		// #NO_SAVE proportion of open NMDA receptor channels
  float		ca;		// #NO_SAVE postsynaptic Ca value, drives learning

#ifdef XCAL_DEBUG
  float		srprod_s;	// #NO_SAVE TODO: temp!!! s-r product comparison short term value (from sep bias guys)
  float		srprod_m;	// #NO_SAVE TODO: temp!!! s-r product comparison medium term value (from sep bias guys)
#endif
  
  LeabraSpikeCon() { sravg_ss = nmda = ca = 0.0f;
#ifdef XCAL_DEBUG
    srprod_s = srprod_m = 0.0f;
#endif
  }
};

#endif // LeabraSpikeCon_h
