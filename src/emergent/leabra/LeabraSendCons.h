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

#ifndef LeabraSendCons_h
#define LeabraSendCons_h 1

// parent includes:
#include "network_def.h"
#include <SendCons>

// member includes:
#include <LeabraConSpec>

// declare all other types mentioned but not required to include:
class LeabraNetwork; // 
class LeabraUnit; // 

eTypeDef_Of(LeabraSendCons);

class E_API LeabraSendCons : public SendCons {
  // #STEM_BASE ##CAT_Leabra Leabra sending connection group
INHERITED(SendCons)
public:
  float		scale_eff;	// #NO_SAVE #CAT_Activation effective scale parameter for netin -- copied from recv cons group where it is computed
  float		savg_cor;	// #NO_SAVE #CAT_Learning savg correction factor for hebbian learning

  void 	Send_NetinDelta(LeabraNetwork* net, int thread_no, float su_act_delta)
  { ((LeabraConSpec*)GetConSpec())->Send_NetinDelta(this, net, thread_no, su_act_delta); }
  // #CAT_Activation send delta-netin

  void	Compute_dWt_CtLeabraXCAL(LeabraUnit* su, LeabraNetwork* net)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraXCAL(this, su, net); }
  // #CAT_Learning compute weight changes: CtLeabra XCAL version
  void	Compute_Weights_CtLeabraXCAL(LeabraUnit* su, LeabraNetwork* net)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_CtLeabraXCAL(this, su, net); }
  // #CAT_Learning compute weights: CtLeabra XCAL version

  TypeDef*      DefaultConType() override
  { return &TA_LeabraCon; }

  void	Copy_(const LeabraSendCons& cp);
  TA_BASEFUNS(LeabraSendCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#endif // LeabraSendCons_h
