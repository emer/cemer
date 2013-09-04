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

#ifndef LeabraDeltaConSpec_h
#define LeabraDeltaConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraDeltaConSpec);

class E_API LeabraDeltaConSpec : public LeabraConSpec {
  // basic delta-rule learning (plus - minus) * sender, with sender in the minus phase -- soft bounding as specified in spec -- no hebbian or anything else
INHERITED(LeabraConSpec)
public:
  inline override void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su,
                                     LeabraNetwork* net, bool do_s) {
    // do NOT do this under any circumstances!!
  }

  // everything can use one dwt with post-soft-bound because no hebbian term
  inline void C_Compute_dWt_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float dwt = (ru->act_p - ru->act_m) * su->act_m; // basic delta rule, sender in minus
    cn->dwt += cur_lrate * dwt;
    // soft bounding is managed in the weight update phase, not in dwt
  }

  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i, net);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta(cn, ru, su);  
    }
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                      LeabraNetwork* net) {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                        LeabraNetwork* net) {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                        LeabraNetwork* net) {
    Compute_Weights_CtLeabraXCAL(cg, su, net); // do soft bound here
  }

  TA_SIMPLE_BASEFUNS(LeabraDeltaConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ Initialize(); }
};

#endif // LeabraDeltaConSpec_h
