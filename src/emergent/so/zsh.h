// Copyright, 1995-2007, Regents of the University of Colorado,
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



// zsh.h: zero-sum-hebbian built out of so

#ifndef zsh_h
#define zsh_h

#include "so.h"

eTypeDef_Of(ZshConSpec);

class E_API ZshConSpec : public SoConSpec {
  // zero-sum-hebbian (subtractive normalization) learning
INHERITED(SoConSpec)
public:
  bool		soft_wt_bound;
  // soft weight bounding *(1-wt) for inc, *wt for dec

  inline void	C_Compute_dWt(float& dwt, const float ru_act, const float su_act,
                              const float avg_in_act, const float wt)
  { float tmp = ru_act * (su_act - avg_in_act);
    if(soft_wt_bound) {
      if(tmp > 0) tmp *= wt_limits.max - wt;
      else        tmp *= wt - wt_limits.min; 
    }
    dwt += tmp;
  }

  inline void 	Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
    Compute_AvgInAct((SoRecvCons*)cg, (SoUnit*)ru, (SoNetwork*)net);
    const float avg_in_act = ((SoRecvCons*)cg)->avg_in_act;
    const float ru_act = ru->act;
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->Un(i,net)->act, avg_in_act,
                                     wts[i]));
   
  }
  // compute weight change according to Cl function (normalized input acts)

  TA_SIMPLE_BASEFUNS(ZshConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

// MaxIn is an algorithm developed in O'Reilly, 1994, which is based
// on units that have a signal-to-noise ratio activation function
// and maximize this ratio over learning.  It basically amounts to 
// Zsh plus SoftCl, though the actual derivative for MaxIn dynamically
// weights the Zsh-like term, which is missing in this version

eTypeDef_Of(MaxInConSpec);

class E_API MaxInConSpec : public ZshConSpec {
  // approximation to MaxIn (Zsh + SoftCl)
INHERITED(ZshConSpec)
public:
  float		k_scl;
  // strength of the soft-competitive learning component

  inline void	C_Compute_dWt(float& dwt, const float ru_act, const float su_act,
                              const float avg_in_act, const float wt)
  { float tmp = ru_act * (su_act - avg_in_act) +
      k_scl * ru_act * (su_act - wt);
    if(soft_wt_bound) {
      if(tmp > 0) tmp *= wt_limits.max - wt;
      else        tmp *= wt - wt_limits.min; 
    }
    dwt += tmp;
  }

  inline void 	Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
    Compute_AvgInAct((SoRecvCons*)cg, (SoUnit*)ru, (SoNetwork*)net);
    const float avg_in_act = ((SoRecvCons*)cg)->avg_in_act;
    const float ru_act = ru->act;
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->Un(i,net)->act, avg_in_act,
                                     wts[i]));
   
  }
  // compute weight change according to approximate MaxIn function

  TA_SIMPLE_BASEFUNS(MaxInConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#endif // zsh_h
