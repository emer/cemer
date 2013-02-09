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

#ifndef LeabraCon_h
#define LeabraCon_h 1

// parent includes:
#include "network_def.h"
#include <Connection>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraCon);

class E_API LeabraCon : public Connection {
  // #STEM_BASE ##CAT_Leabra Leabra connection
public:
  float		pdw;		// #VIEW_HOT #NO_SAVE previous delta-weight change -- useful for viewing because current weight change (dwt) is typically reset to 0 when views are updated
  
  LeabraCon() { pdw = 0.0f; }
};

#endif // LeabraCon_h
