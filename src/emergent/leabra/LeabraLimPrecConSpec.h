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

#ifndef LeabraLimPrecConSpec_h
#define LeabraLimPrecConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraLimPrecConSpec);

class E_API LeabraLimPrecConSpec : public LeabraConSpec {
  // ##CAT_Leabra Leabra limited precision connection specs: limits weight values to specified level of precision between 0-1
INHERITED(LeabraConSpec)
public:
  float		prec_levels;	// number of levels of precision available in the weight values

  inline float	PrecLimitVal(const float val) {
    int tmp = (int)((prec_levels * val) + .5f); // integerize with rounding -- val 0-1
    float rval = (float)tmp / prec_levels;
    if(rval > 1.0f) rval = 1.0f;
    if(rval < 0.0f) rval = 0.0f;
    return rval;
  }

  inline void C_Compute_LimPrecWts(float& wt)
  { wt = PrecLimitVal(wt); }
  // #IGNORE

  inline void Compute_LimPrecWts(LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net) {
    float* wts = cg->OwnCnVar(WT);
    CON_GROUP_LOOP(cg, C_Compute_LimPrecWts(wts[i]));
  }
  // #IGNORE

  inline void	Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                  LeabraNetwork* net) CPP11_OVERRIDE {
    inherited::Compute_Weights_LeabraCHL(cg, su, net);
    Compute_LimPrecWts(cg, su, net);
  }

  inline void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                     LeabraNetwork* net) CPP11_OVERRIDE {
    inherited::Compute_Weights_CtLeabraXCAL(cg, su, net);
    Compute_LimPrecWts(cg, su, net);
  }

  inline void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) CPP11_OVERRIDE {
    inherited::Compute_Weights_CtLeabraCAL(cg, su, net);
    Compute_LimPrecWts(cg, su, net);
  }

  // NOTE: bias weights typically not subject to limited precision!

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "ConSpec"; }

  SIMPLE_COPY(LeabraLimPrecConSpec);
  TA_BASEFUNS(LeabraLimPrecConSpec);
protected:
  SPEC_DEFAULTS;
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

#endif // LeabraLimPrecConSpec_h
