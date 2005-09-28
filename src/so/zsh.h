// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// zsh.h: zero-sum-hebbian built out of so

#ifndef zsh_h
#define zsh_h

#include <so/so.h>

class ZshConSpec : public SoConSpec {
  // zero-sum-hebbian (subtractive normalization) learning
public:
  bool		soft_wt_bound;
  // soft weight bounding *(1-wt) for inc, *wt for dec

  inline void	C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to Zsh function

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(ZshConSpec);
  COPY_FUNS(ZshConSpec, SoConSpec);
  TA_BASEFUNS(ZshConSpec);
};

// MaxIn is an algorithm developed in O'Reilly, 1994, which is based
// on units that have a signal-to-noise ratio activation function
// and maximize this ratio over learning.  It basically amounts to 
// Zsh plus SoftCl, though the actual derivative for MaxIn dynamically
// weights the Zsh-like term, which is missing in this version

class MaxInConSpec : public ZshConSpec {
  // approximation to MaxIn (Zsh + SoftCl)
public:
  float		k_scl;
  // strength of the soft-competitive learning component

  inline void	C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to approximate MaxIn function

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(MaxInConSpec);
  COPY_FUNS(MaxInConSpec, ZshConSpec);
  TA_BASEFUNS(MaxInConSpec);
};



//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void ZshConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group* cg, Unit* ru, Unit* su) {
  float tmp = ru->act * (su->act - cg->avg_in_act);
  if(soft_wt_bound) {
    if(tmp > 0)			
      tmp *= wt_limits.max - cn->wt;
    else
      tmp *= cn->wt - wt_limits.min; 
  }
  cn->dwt += tmp;
}

inline void ZshConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  Compute_AvgInAct((SoCon_Group*)cg, ru);
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

inline void MaxInConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group* cg, Unit* ru, Unit* su) {
  float tmp = ru->act * (su->act - cg->avg_in_act) +
    k_scl * ru->act * (su->act - cn->wt);
  if(soft_wt_bound) {
    if(tmp > 0)			
      tmp *= wt_limits.max - cn->wt;
    else
      tmp *= cn->wt - wt_limits.min;
  }
  cn->dwt += tmp;
}

inline void MaxInConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  Compute_AvgInAct((SoCon_Group*)cg, ru);
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

#endif // zsh_h
