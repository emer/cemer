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
#include "leabra_def.h"
#include <SendCons>

// member includes:
#include <LeabraConSpec>

// declare all other types mentioned but not required to include:
class LeabraNetwork; // 
class LeabraUnit; // 

TypeDef_Of(LeabraSendCons);

class LEABRA_API LeabraSendCons : public SendCons {
  // #STEM_BASE ##CAT_Leabra Leabra sending connection group
INHERITED(SendCons)
public:
  float		scale_eff;	// #NO_SAVE #CAT_Activation effective scale parameter for netin -- copied from recv cons group where it is computed
  float		savg_cor;	// #NO_SAVE #CAT_Learning savg correction factor for hebbian learning

  void 	Send_NetinDelta(LeabraNetwork* net, int thread_no, float su_act_delta)
  { ((LeabraConSpec*)GetConSpec())->Send_NetinDelta(this, net, thread_no, su_act_delta); }
  // #CAT_Activation send delta-netin

  void	Compute_dWt_LeabraCHL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_LeabraCHL(this, su); }
  // #CAT_Learning compute weight changes: Leabra CHL version
  void	Compute_Weights_LeabraCHL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_LeabraCHL(this, su); }
  // #CAT_Learning compute weights: Leabra CHL version

  void	Compute_dWt_CtLeabraXCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraXCAL(this, su); }
  // #CAT_Learning compute weight changes: CtLeabra XCAL version
  void	Compute_Weights_CtLeabraXCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_CtLeabraXCAL(this, su); }
  // #CAT_Learning compute weights: CtLeabra XCAL version

  void	Trial_Init_SRAvg(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Trial_Init_SRAvg(this, su); }
  // #CAT_Learning initialize send-recv average coproduct
  void	Compute_SRAvg(LeabraUnit* su, bool do_s)
  { ((LeabraConSpec*)GetConSpec())->Compute_SRAvg(this, su, do_s); }
  // #CAT_Learning compute send-recv average coproduct
  void	Compute_dWt_CtLeabraCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_dWt_CtLeabraCAL(this, su); }
  // #CAT_Learning compute weight changes: CtLeabra CAL version
  void	Compute_Weights_CtLeabraCAL(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Weights_CtLeabraCAL(this, su); }
  // #CAT_Learning compute weights: CtLeabra CAL version

  void	Compute_Leabra_dWt(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Leabra_dWt(this, su); }
  // #CAT_Learning #IGNORE overall compute delta-weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant
  void	Compute_Leabra_Weights(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_Leabra_Weights(this, su); }
  // #CAT_Learning #IGNORE overall compute weights for Leabra -- just a switch on learn rule to select above algorithm-specific variant

  void	Compute_CycSynDep(LeabraUnit* su)
  { ((LeabraConSpec*)GetConSpec())->Compute_CycSynDep(this, su); }
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  void	Copy_(const LeabraSendCons& cp);
  TA_BASEFUNS(LeabraSendCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#endif // LeabraSendCons_h
