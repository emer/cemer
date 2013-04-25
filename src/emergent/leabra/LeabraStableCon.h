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

#ifndef LeabraStableCon_h
#define LeabraStableCon_h 1

// parent includes:
#include <LeabraCon>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraStableCon);

class E_API LeabraStableCon : public LeabraCon {
  // stable synaptic connection -- wt = effective net weight = mix of lwt that learns and a stable weight swt that is only updated periodically -- reflects short vs. long term LTP dynamics and synaptic consolidation process -- both weights are set to the saved wt value when weights are reloaded from a file
INHERITED(LeabraCon)
public:
  float         lwt;            // #NO_SAVE learning weight value -- adapts according to learning rules
  float         swt;            // #NO_SAVE stable weight value -- updated from wt value periodically (e.g., at the end of an epoch)

  LeabraStableCon() { lwt = swt = 0.0f; }
};

#endif // LeabraStableCon_h
