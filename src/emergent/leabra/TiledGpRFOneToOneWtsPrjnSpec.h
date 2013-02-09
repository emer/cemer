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

#ifndef TiledGpRFOneToOneWtsPrjnSpec_h
#define TiledGpRFOneToOneWtsPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <TiledGpRFPrjnSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TiledGpRFOneToOneWtsPrjnSpec);

class E_API TiledGpRFOneToOneWtsPrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with initial weights (when init_wts is set) that have differential weights for units with the same index within a unit group vs. differential weights -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOnetToOnePrjnSpec for harder version where connections are only made among units with same index within group)
INHERITED(TiledGpRFPrjnSpec)
public:
  float		one_to_one_wt;	// #CONDEDIT_ON_init_wts weight between units with the same index in the unit group
  float		other_wt;	// #CONDEDIT_ON_init_wts weight between other non-one-to-one units

  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  TA_SIMPLE_BASEFUNS(TiledGpRFOneToOneWtsPrjnSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // TiledGpRFOneToOneWtsPrjnSpec_h
