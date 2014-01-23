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

#ifndef GradientWtsPrjnSpec_h
#define GradientWtsPrjnSpec_h 1

// parent includes:
#include <FullPrjnSpec>

// member includes:
#include <MinMaxRange>

// declare all other types mentioned but not required to include:

eTypeDef_Of(GradientWtsPrjnSpec);

class E_API GradientWtsPrjnSpec : public FullPrjnSpec {
  // full connectivity with a gradient of weight strengths (requires init_wts = true, otherwise is just like Full Prjn), where weights are strongest from sending units in same relative location as the receiving unit, and fall off from there (either linearly or as a Guassian) -- if recv layer has unit groups, then it is the unit group position that counts, and all units within the recv group have the same connectivity (can override with use_gps flag)
INHERITED(FullPrjnSpec)
public:
  enum GradType {		// type of gradient to establish
    LINEAR,			// linear fall-off as a function of distance
    GAUSSIAN,			// gaussian fall-off as a function of distance
  };

  MinMaxRange	wt_range;	// #CONDEDIT_ON_init_wts range of weakest (min) to strongest (max) weight values generated
  bool		invert;		// #CONDEDIT_ON_init_wts invert the gradient, such that the min is located "below" the recv units, and the max is furthest away
  bool		grad_x;		// #CONDEDIT_ON_init_wts compute a gradient over the x dimension of the sending layer, based on x axis location of the matrix stripe unit group
  bool		grad_y;		// #CONDEDIT_ON_init_wts compute a gradient over the y dimension of the sending layer, based on y axis location of the matrix stripe unit group
  bool		wrap;		// #CONDEDIT_ON_init_wts wrap weight values around relevant dimension(s) -- the closest location wins -- this ensures that all units have the same overall weight strengths
  bool		use_gps;	// #CONDEDIT_ON_init_wts if recv layer has unit groups, use them for determining relative position to compare with sending unit locations (unit group information is never used for the sending layer)
  GradType	grad_type;	// #CONDEDIT_ON_init_wts type of gradient to make -- applies to both axes
  float		gauss_sig;	// #CONDSHOW_ON_grad_type:GAUSSIAN gaussian sigma (width), in normalized units where entire distance across sending layer is 1.0 

  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) CPP11_OVERRIDE;

  virtual void	InitWeights_RecvGps(Projection* prjn, RecvCons* cg, Unit* ru);
  // for recv unit group case
  virtual void 	InitWeights_RecvFlat(Projection* prjn, RecvCons* cg, Unit* ru);
  // for flat recv layer case (just unit positions)
  virtual void 	SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist, int cg_idx);
  // actually set the weight value from distance value -- util used by both of above main routines -- can overload to implement different gradient functions -- cg_idx is index within con group, and dist is computed normalized distance value (0-1)

  TA_SIMPLE_BASEFUNS(GradientWtsPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

#endif // GradientWtsPrjnSpec_h
