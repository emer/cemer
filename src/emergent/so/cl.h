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

eTypeDef_Of(ClConSpec);

class E_API ClConSpec : public SoConSpec {
  // competitive learning connection spec (uses normalized input activation)
INHERITED(SoConSpec)
public:
  inline void	C_Compute_dWt(float& dwt, const float ru_act, const float su_act,
                              const float sum_in_act, const float wt)
  { dwt += ru_act * ((su_act / sum_in_act) - wt); }

  inline void 	Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
    Compute_AvgInAct((SoRecvCons*)cg, (SoUnit*)ru, (SoNetwork*)net);
    const float sum_in_act = ((SoRecvCons*)cg)->sum_in_act;
    const float ru_act = ru->act;
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->Un(i,net)->act, sum_in_act,
                                     wts[i]));
  }
  // compute weight change according to Cl function (normalized input acts)

  TA_BASEFUNS_NOCOPY(ClConSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};

eTypeDef_Of(SoftClConSpec);

class E_API SoftClConSpec : public SoConSpec {
  // soft competitive learning connection spec
INHERITED(SoConSpec)
public:
  inline void	C_Compute_dWt(float& dwt, const float ru_act, const float su_act,
                              const float wt)
  { dwt += ru_act * (su_act - wt); }

  inline void 	Compute_dWt(RecvCons* cg, Unit* ru, Network* net) {
    const float ru_act = ru->act;
    float* dwts = cg->OwnCnVar(DWT);
    float* wts = cg->OwnCnVar(WT);
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->Un(i,net)->act, wts[i]));
  }

  TA_BASEFUNS_NOCOPY(SoftClConSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};


eTypeDef_Of(ClLayerSpec);

class E_API ClLayerSpec : public SoLayerSpec {
  // competitive learning layer spec
INHERITED(SoLayerSpec)
public:
  void	Compute_Act_post(SoLayer* lay, SoNetwork* net) CPP11_OVERRIDE;
  // set activation to be 1.0 for unit with most input, 0 else

  TA_BASEFUNS_NOCOPY(ClLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(SoftClUnitSpec);

class E_API SoftClUnitSpec : public SoUnitSpec {
  // soft competitive learning unit spec
INHERITED(SoUnitSpec)
public:
  float		var;		// variance of the Gaussian activation function
  float         norm_const;     // #HIDDEN normalization const for Gaussian
  float         denom_const;    // #HIDDEN denominator const for Gaussian

  void Compute_Netin(Unit* u, Network* net, int thread_no=-1) CPP11_OVERRIDE;
  // redefine to call compute_dist
  void Compute_Act(Unit* u, Network* net, int thread_no=-1) CPP11_OVERRIDE;
  // activation is a gaussian function of the net input

  TA_SIMPLE_BASEFUNS(SoftClUnitSpec);
protected:
  void	UpdateAfterEdit_impl() CPP11_OVERRIDE;
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(SoftClLayerSpec);

class E_API SoftClLayerSpec : public SoLayerSpec {
  // soft competitive learning layer spec: does a softmax on the units
INHERITED(SoLayerSpec)
public:

  void	Compute_Act_post(SoLayer* lay, SoNetwork* net) CPP11_OVERRIDE;
  // set activation to be softmax of unit activations

  TA_BASEFUNS_NOCOPY(SoftClLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};


#endif // cl_h
