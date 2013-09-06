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

eTypeDef_Of(LeabraNegBiasSpec);

class E_API LeabraNegBiasSpec : public LeabraBiasSpec {
  // only learns negative bias changes, not positive ones (decay restores back to zero)
INHERITED(LeabraBiasSpec)
public:
  float		decay;		// rate of weight decay towards zero 
  bool		updt_immed;	// update weights immediately when weights are changed

  inline void	B_Compute_Weights(RecvCons* bias, LeabraUnit* ru) {
    if(!learn) return;
    float& wt = bias->OwnCn(0,WT);
    float& dwt = bias->OwnCn(0,DWT);
    float& pdw = bias->OwnCn(0,PDW);
    if(dwt > 0.0f)		// positive only
      dwt = 0.0f;
    dwt -= decay * wt;
    pdw = dwt;
    wt += dwt;
    dwt = 0.0f;
    C_ApplyLimits(wt, ru, NULL);
  }

  inline void	B_Compute_dWt_LeabraCHL(RecvCons* bias, LeabraUnit* ru) {
    LeabraBiasSpec::B_Compute_dWt_LeabraCHL(bias, ru);
    if(updt_immed) B_Compute_Weights(bias, ru);
  }
  inline void	B_Compute_dWt_CtLeabraXCAL(RecvCons* bias, LeabraUnit* ru, LeabraLayer* rlay) {
    LeabraBiasSpec::B_Compute_dWt_CtLeabraXCAL(bias, ru, rlay);
    if(updt_immed) B_Compute_Weights(bias, ru);
  }
  inline void	B_Compute_dWt_CtLeabraCAL(RecvCons* bias, LeabraUnit* ru, LeabraLayer* rlay) {
    LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(bias, ru, rlay);
    if(updt_immed) B_Compute_Weights(bias, ru);
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

#ifdef __TA_COMPILE__
// get the inlines
#include <LeabraNetwork>
#endif

#endif // LeabraNegBiasSpec_h
