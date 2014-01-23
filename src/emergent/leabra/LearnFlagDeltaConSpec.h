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

#ifndef LearnFlagDeltaConSpec_h
#define LearnFlagDeltaConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LearnFlagDeltaConSpec);

class E_API LearnFlagDeltaConSpec : public LeabraConSpec {
  // requires recv unit to have LEARN flag activated, and also includes optional dopamine modulation, on top of a basic delta rule -- used for gdPVLV learning in subcortical areas
INHERITED(LeabraConSpec)
public:
  enum DaModType {
    NO_DA_MOD,                  // do not modulate the learning by recv unit dopamine value (dav)
    DA_MOD,                     // modulate the learning by multiplying directly by the recv unit dopamine value (dav) -- this will change the sign of learning as a function of the sign of the dopamine value
    DA_MOD_ABS,                 // modulate the learning by multiplying by the absolute value of the recv unit dopamine (abs(dav)) -- this does not change the sign, only the magnitude of learning
  };

  DaModType     da_mod;         // how does receiving unit dopamine modulate learning (or not)?

  inline void C_Compute_dWt_Delta_NoDa(float& dwt, const float ru_act_p, 
                                       const float ru_act_m, const float su_act) {
    dwt += cur_lrate * (ru_act_p - ru_act_m) * su_act;
  }
  // #IGNORE
  inline void C_Compute_dWt_Delta_Da(float& dwt, const float ru_act_p, 
                                     const float ru_act_m, const float su_act,
                                     const float dav) {
    dwt += cur_lrate * dav * (ru_act_p - ru_act_m) * su_act;
  }
  // #IGNORE dopamine multiplication

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) CPP11_OVERRIDE {
    const float su_act = su->act_m; // note: using act_m
    float* dwts = cg->OwnCnVar(DWT);

    const int sz = cg->size;
    if(da_mod == NO_DA_MOD) {
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i, net);
        if(!ru->HasUnitFlag(Unit::LEARN)) continue; // must have this flag to learn
        C_Compute_dWt_Delta_NoDa(dwts[i], ru->act_p, ru->act_m, su_act);
      }
    }
    else if(da_mod == DA_MOD) {
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i, net);
        if(!ru->HasUnitFlag(Unit::LEARN)) continue; // must have this flag to learn
        C_Compute_dWt_Delta_Da(dwts[i], ru->act_p, ru->act_m, su_act, ru->dav);
      }
    }
    else {                      // DA_MOD_ABS
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i, net);
        if(!ru->HasUnitFlag(Unit::LEARN)) continue; // must have this flag to learn
        C_Compute_dWt_Delta_Da(dwts[i], ru->act_p, ru->act_m, su_act, fabsf(ru->dav));
      }
    }
  }

  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                             LeabraNetwork* net) CPP11_OVERRIDE {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                               LeabraNetwork* net) CPP11_OVERRIDE {
    Compute_dWt_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) CPP11_OVERRIDE {
    Compute_Weights_CtLeabraXCAL(cg, su, net); // do soft bound here
  }

  TA_SIMPLE_BASEFUNS(LearnFlagDeltaConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LearnFlagDeltaConSpec_h
