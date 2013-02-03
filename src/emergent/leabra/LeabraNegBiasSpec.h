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

#ifndef LeabraNegBiasSpec_h
#define LeabraNegBiasSpec_h 1

// parent includes:
#include <LeabraBiasSpec>

// member includes:
#include <LeabraUnit>

// declare all other types mentioned but not required to include:

TypeDef_Of(LeabraNegBiasSpec);

class LEABRA_API LeabraNegBiasSpec : public LeabraBiasSpec {
  // only learns negative bias changes, not positive ones (decay restores back to zero)
INHERITED(LeabraBiasSpec)
public:
  float		decay;		// rate of weight decay towards zero 
  bool		updt_immed;	// update weights immediately when weights are changed

  inline void	B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru) {
    if(cn->dwt > 0.0f)		// positive only
      cn->dwt = 0.0f;
    cn->dwt -= decay * cn->wt;
    cn->pdw = cn->dwt;
    cn->wt += cn->dwt;
    cn->dwt = 0.0f;
    C_ApplyLimits(cn, ru, NULL);
  }

  inline void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
    LeabraBiasSpec::B_Compute_dWt_LeabraCHL(cn, ru);
    if(updt_immed) B_Compute_Weights(cn, ru);
  }
  inline void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    LeabraBiasSpec::B_Compute_dWt_CtLeabraXCAL(cn, ru, rlay);
    if(updt_immed) B_Compute_Weights(cn, ru);
  }
  inline void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(cn, ru, rlay);
    if(updt_immed) B_Compute_Weights(cn, ru);
  }

  SIMPLE_COPY(LeabraNegBiasSpec);
  TA_BASEFUNS(LeabraNegBiasSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { };	
};

#endif // LeabraNegBiasSpec_h
