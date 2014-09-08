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
  float         fwt;            // #NO_SAVE fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
  float         swt;            // #NO_SAVE slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
  
  LeabraCon() { fwt = swt = 0.0f; }
};

#endif // LeabraCon_h
