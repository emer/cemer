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

#ifndef Connection_h
#define Connection_h 1

// parent includes:
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(Connection);

// the connection is managed fully by the ConSpec and the BaseCons
// and is no longer used as such -- all the memory is directly 
// allocated floats -- the Connection object is only used for type information
// regarding number of variables

// note: connections MUST have only float members -- type information 
// of this object is used by the BaseCons to manage the connection memory,
// which is allocated vector-wise so that each variable is contiguous in memory,
// to optimize the vectorization processes

class E_API Connection {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network base connection between two units
public:
  float        wt;             // #VIEW_HOT synaptic weight of connection
  float        dwt;            // #VIEW #NO_SAVE change in synaptic weight as computed by learning mechanism

  Connection() { wt = dwt = 0.0f; }
};

#endif // Connection_h
