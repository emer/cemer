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

#ifndef ScalarValSelfPrjnSpec_h
#define ScalarValSelfPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <Layer>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ScalarValSelfPrjnSpec);

class E_API ScalarValSelfPrjnSpec : public ProjectionSpec {
  // special projection for making self-connection that establishes neighbor similarity in scalar val
INHERITED(ProjectionSpec)
public:
  int	width;			// width of neighborhood, in units (i.e., connect width units to the left, and width units to the right)
  float	wt_width;		// #CONDEDIT_ON_init_wts width of the sigmoid for providing initial weight values
  float	wt_max;			// #CONDEDIT_ON_init_wts maximum weight value (of 1st neighbor -- not of self unit!)

  virtual void	Connect_UnitGroup(Layer* lay, Layer::AccessMode acc_md, int gpidx,
				  Projection* prjn);
  void		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses weight values as specified in the tesselel's

  TA_SIMPLE_BASEFUNS(ScalarValSelfPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // ScalarValSelfPrjnSpec_h
