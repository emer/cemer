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

#ifndef LeabraRecvCons_h
#define LeabraRecvCons_h 1

// parent includes:
#include "network_def.h"
#include <RecvCons>

// member includes:
#include <LeabraConSpec>

// declare all other types mentioned but not required to include:
class LeabraUnit; // 

eTypeDef_Of(LeabraRecvCons);

class E_API LeabraRecvCons : public RecvCons {
  // #STEM_BASE ##CAT_Leabra Leabra receiving connection group
INHERITED(RecvCons)
public:
  float		scale_eff;	// #NO_SAVE #CAT_Activation effective scale parameter for netin -- copied to send cons group where it is actually used, but it is computed here
  float		net;		// #NO_SAVE #CAT_Activation netinput to this con_group: only computed for special statistics such as RelNetin
  float		net_delta;	// #NO_SAVE #CAT_Activation delta netinput to this con_group -- only used for NETIN_PER_PRJN
  float		net_raw;	// #NO_SAVE #CAT_Activation raw summed netinput to this con_group -- only used for NETIN_PER_PRJN

  void	Compute_dWt_CtLeabraXCalC(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraXCalC(this, ru); }
  // #CAT_Learning compute weight changes: CtLeabra XCalC version

  void	Compute_dWt_Norm(LeabraUnit* ru)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_Norm(this, ru); }
  // #CAT_Learning compute dwt normalization

  void	Copy_(const LeabraRecvCons& cp);
  TA_BASEFUNS(LeabraRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#endif // LeabraRecvCons_h
