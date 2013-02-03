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

#ifndef XCalCHLConSpec_h
#define XCalCHLConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(XCalCHLConSpec);

class LEABRA_API XCalCHLConSpec : public LeabraConSpec {
  // does CHL-based Leabra learning under XCAL and CAL learning framework -- sometimes CHL performs better, e.g., in the hippocampus..
INHERITED(LeabraConSpec)
public:
#ifdef __MAKETA__
  LearnMixSpec	lmix;		// #CAT_Learning mixture of hebbian & err-driven learning 
#endif
  bool		use_chl;	// use LeabraCHL learning instead of XCAL or CAL learning, even when rest of network is using those other algorithms

  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_dWt_LeabraCHL(cg, su);
    else
      inherited::Compute_dWt_CtLeabraXCAL(cg, su);
  }

  override void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_Weights_LeabraCHL(cg, su);
    else
      inherited::Compute_Weights_CtLeabraXCAL(cg, su);
  }

  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_dWt_LeabraCHL(cg, su);
    else
      inherited::Compute_dWt_CtLeabraCAL(cg, su);
  }

  override void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_Weights_LeabraCHL(cg, su);
    else
      inherited::Compute_Weights_CtLeabraCAL(cg, su);
  }

  override void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    if(use_chl)
      inherited::B_Compute_dWt_LeabraCHL(cn, ru);
    else
      inherited::B_Compute_dWt_CtLeabraXCAL(cn, ru, rlay);
  }
  override void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    if(use_chl)
      inherited::B_Compute_dWt_LeabraCHL(cn, ru);
    else
      inherited::B_Compute_dWt_CtLeabraCAL(cn, ru, rlay);
  }

  TA_SIMPLE_BASEFUNS(XCalCHLConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

#endif // XCalCHLConSpec_h
