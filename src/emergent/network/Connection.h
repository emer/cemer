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

// the connection is managed fully by the ConSpec and the RecvCons
// don't put any functions on the connection itself

// note: v4.0 connection stuff is not backwards compatible with v3.2

// note: connection must be just a bag of bits with *no* functions of its own
// especially no virtual functions!  it is managed with raw bit copy ops
// although the sending connections have pointers to connection objects
// it is up to the connection management code to ensure that when a
// connection is removed, its associated sending link is also removed

class E_API Connection {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network base connection between two units
public:
  float         wt;             // #VIEW_HOT weight of connection
  float         dwt;            // #VIEW #NO_SAVE resulting net weight change

  Connection() { wt = dwt = 0.0f; }
};

#endif // Connection_h
