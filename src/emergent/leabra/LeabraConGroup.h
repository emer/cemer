// Copyright 2017, Regents of the University of Colorado,
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

#ifndef LeabraConGroup_h
#define LeabraConGroup_h 1

// parent includes:
#include "network_def.h"
#include <ConGroup>

// member includes:

// declare all other types mentioned but not required to include:
class LeabraConSpec; // 

eTypeDef_Of(LeabraConGroup);

class E_API LeabraConGroup : public ConGroup {
  // #STEM_BASE ##CAT_Leabra Leabra connection group
public:
  float	        scale_eff;      // #NO_SAVE #CAT_Activation effective scale parameter for netin -- copied from recv congroup where it is computed
  float	        net;            // #NO_SAVE #CAT_Activation netinput to this con_group: only computed for special statistics such as RelNetin
  float	        net_raw;        // #NO_SAVE #CAT_Activation raw summed netinput to this con_group -- only used for NETIN_PER_PRJN
  float	        wt_avg;	        // #NO_SAVE #CAT_Learning average effective weight values across this con group -- used for weight balance
  float	        wb_inc;	        // #NO_SAVE #CAT_Learning weight balance increment factor -- extra multiplier to add to weight increases to maintain overall weight balance
  float	        wb_dec;	        // #NO_SAVE #CAT_Learning weight balance decrement factor -- extra multiplier to add to weight decreases to maintain overall weight balance

  inline LeabraConSpec*    GetConSpec() const { return (LeabraConSpec*)con_spec; }

  void  LeabraInit()
  { scale_eff = 0.0f; net = 0.0f; net_raw = 0.0f; wt_avg = 0.5f; 
    wb_inc = 1.0f; wb_dec = 1.0f;
  }
  // leabra initialize
};

#endif // LeabraConGroup_h
