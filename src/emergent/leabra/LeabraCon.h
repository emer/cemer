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
  // #STEM_BASE ##CAT_Leabra #AKA_MatrixCon Leabra connection
public:
  float		pdw;		// #VIEW_HOT #NO_SAVE previous delta-weight change -- useful for viewing because current weight change (dwt) is typically reset to 0 when views are updated
  float         lwt;            // #NO_SAVE learning weight value -- adapts according to learning rules every trial in a dynamic online manner
  float         swt;            // #NO_SAVE stable (protein-synthesis and potentially sleep dependent) weight value -- updated from lwt value periodically (e.g., at the end of an epoch) by Compute_StableWeight function
  
  LeabraCon() { pdw = 0.0f; lwt = swt = 0.0f; }
};

#endif // LeabraCon_h
