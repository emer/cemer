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



// cl.h: competitive learning built out of so

#ifndef cl_h
#define cl_h

#include <so/so.h>

class ClConSpec;
class SoftClConSpec;
class ClLayerSpec;
class SoftClUnitSpec;
class SoftClConSpec;

class ClConSpec : public SoConSpec {
  // competitive learning connection spec (uses normalized input activation)
public:
  inline void	C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to Cl function (normalized input acts)

  void 	Initialize()		{ };
  void	Destroy()		{ };
  SIMPLE_COPY(ClConSpec);
  COPY_FUNS(ClConSpec, SoConSpec);
  TA_BASEFUNS(ClConSpec);
};

class SoftClConSpec : public SoConSpec {
  // soft competitive learning connection spec
public:
  inline void		C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline virtual void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to soft Cl function

  void 	Initialize()		{ };
  void	Destroy()		{ };
  SIMPLE_COPY(SoftClConSpec);
  COPY_FUNS(SoftClConSpec, SoConSpec);
  TA_BASEFUNS(SoftClConSpec);
};


class ClLayerSpec : public SoLayerSpec {
  // competitive learning layer spec
public:
  void		Compute_Act(SoLayer* lay);
  // set activation to be 1.0 for unit with most input, 0 else

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(ClLayerSpec);
};

class SoftClUnitSpec : public SoUnitSpec {
  // soft competitive learning unit spec
public:
  float		var;		// variance of the Gaussian activation function
  float         norm_const;     // #HIDDEN normalization const for Gaussian
  float         denom_const;    // #HIDDEN denominator const for Gaussian

  void 		Compute_Netin(Unit* u); // redefine to call compute_dist
  void 		Compute_Act(Unit* u);
  // activation is a gaussian function of the net input

  void  UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SoftClUnitSpec);
  COPY_FUNS(SoftClUnitSpec, SoUnitSpec);
  TA_BASEFUNS(SoftClUnitSpec);
};

class SoftClLayerSpec : public SoLayerSpec {
  // soft competitive learning layer spec: does a softmax on the units
public:

  void		Compute_Act(SoLayer* lay);
  // set activation to be softmax of unit activations

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(SoftClLayerSpec);
};

//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void ClConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group* cg, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * ((su->act / cg->sum_in_act) - cn->wt);
}

inline void ClConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  Compute_AvgInAct((SoCon_Group*)cg, ru);
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

inline void SoftClConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group*, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * (su->act - cn->wt);
}

inline void SoftClConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

#endif // cl_h
