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



// cl.h: competitive learning built out of so

#ifndef cl_h
#define cl_h

#include "so.h"

class ClConSpec;
class SoftClConSpec;
class ClLayerSpec;
class SoftClUnitSpec;
class SoftClConSpec;

class ClConSpec : public SoConSpec {
  // competitive learning connection spec (uses normalized input activation)
INHERITED(SoConSpec)
public:
  inline void	C_Compute_dWt(SoCon* cn, SoRecvCons* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(RecvCons* cg, Unit* ru);
  // compute weight change according to Cl function (normalized input acts)

  TA_BASEFUNS_NOCOPY(ClConSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};

class SoftClConSpec : public SoConSpec {
  // soft competitive learning connection spec
INHERITED(SoConSpec)
public:
  inline void		C_Compute_dWt(SoCon* cn, SoRecvCons* cg, 
				      Unit* ru, Unit* su);
  inline virtual void 	Compute_dWt(RecvCons* cg, Unit* ru);
  // compute weight change according to soft Cl function

  TA_BASEFUNS_NOCOPY(SoftClConSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};


class ClLayerSpec : public SoLayerSpec {
  // competitive learning layer spec
INHERITED(SoLayerSpec)
public:
  void		Compute_Act(SoLayer* lay);
  // set activation to be 1.0 for unit with most input, 0 else

  TA_BASEFUNS_NOCOPY(ClLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class SoftClUnitSpec : public SoUnitSpec {
  // soft competitive learning unit spec
INHERITED(SoUnitSpec)
public:
  float		var;		// variance of the Gaussian activation function
  float         norm_const;     // #HIDDEN normalization const for Gaussian
  float         denom_const;    // #HIDDEN denominator const for Gaussian

  void 		Compute_Netin(Unit* u); // redefine to call compute_dist
  void 		Compute_Act(Unit* u);
  // activation is a gaussian function of the net input

  TA_SIMPLE_BASEFUNS(SoftClUnitSpec);
protected:
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class SoftClLayerSpec : public SoLayerSpec {
  // soft competitive learning layer spec: does a softmax on the units
INHERITED(SoLayerSpec)
public:

  void		Compute_Act(SoLayer* lay);
  // set activation to be softmax of unit activations

  TA_BASEFUNS_NOCOPY(SoftClLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void ClConSpec::
C_Compute_dWt(SoCon* cn, SoRecvCons* cg, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * ((su->act / cg->sum_in_act) - cn->wt);
}

inline void ClConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  Compute_AvgInAct((SoRecvCons*)cg, ru);
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoRecvCons*)cg, ru, cg->Un(i)));
}

inline void SoftClConSpec::
C_Compute_dWt(SoCon* cn, SoRecvCons*, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * (su->act - cn->wt);
}

inline void SoftClConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoRecvCons*)cg, ru, cg->Un(i)));
}

#endif // cl_h
