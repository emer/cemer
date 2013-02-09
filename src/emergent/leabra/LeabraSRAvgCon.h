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

#ifndef LeabraSRAvgCon_h
#define LeabraSRAvgCon_h 1

// parent includes:
#include <LeabraCon>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraSRAvgCon);

class LeabraSRAvgCon : public LeabraCon {
  // Leabra connection with send-recv average coproduct variables -- required for CTLEABRA_CAL learning rule and perhaps other situations
public:
  float		sravg_s;	// #NO_SAVE short time-scale, most recent (plus phase) average of sender and receiver activation product over time
  float		sravg_m;	// #NO_SAVE medium time-scale, trial-level average of sender and receiver activation product over time

  LeabraSRAvgCon() { sravg_s = sravg_m = 0.0f; }
};

#endif // LeabraSRAvgCon_h
