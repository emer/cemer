// Copyright 2017-2018, Regents of the University of Colorado,
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

#ifndef BpCon_h
#define BpCon_h 1

// parent includes:
#include <Connection>

// member includes:

// declare all other types mentioned but not required to include:

// note: dwt is equivalent to the old dEdW and pdw = the old dwt

eTypeDef_Of(BpCon);

class E_API BpCon : public Connection {
  // #STEM_BASE ##CAT_Bp Bp connection
public:
  float         pdw;            // #NO_SAVE previous delta-weight change -- needed for momentum term in the learning mechanism

  BpCon() { pdw = 0.0f; }
};

#endif // BpCon_h
