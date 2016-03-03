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
  // #STEM_BASE ##CAT_Leabra #AKA_MSNCon Leabra connection
public:
  float         scale;          // #NO_SAVE scaling paramter for this connection -- effective weight value is scaled by this factor -- useful for topographic connectivity patterns e.g., to enforce more distant connections to always be lower in magnitude than closer connections -- set by custom weight init code for certain projection specs -- can also adapt this value using adapt_scale params
  float         fwt;            // #NO_SAVE fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, wt, via sigmoidal contrast enhancement (wt_sig)
  float         swt;            // #NO_SAVE slow learning linear (underlying) weight value -- slowly tracks changes in fwt value, and contributes in some proportion to net effective weight value, wt
#ifdef SUGP_NETIN
  int32_t       sugp;           // #NO_SAVE sending unit group index -- for integrating net input over unit groups
#endif
  
  LeabraCon() { fwt = swt = 0.0f; scale = 1.0f;
#ifdef SUGP_NETIN
    sugp = 0;
#endif
  }

};

#endif // LeabraCon_h
