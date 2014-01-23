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

#ifndef TiledGpRFOneToOnePrjnSpec_h
#define TiledGpRFOneToOnePrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <TiledGpRFPrjnSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TiledGpRFOneToOnePrjnSpec);

class E_API TiledGpRFOneToOnePrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with one-to-one connections for units with the same index within a unit group -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOneToOneWtsPrjnSpec for a softer version where only weights are set)
INHERITED(TiledGpRFPrjnSpec)
public:
  float		gauss_sigma;		// #CONDEDIT_ON_init_wts gaussian width parameter for initial weight values (only with init_wts on) to give a tuning curve in terms of distance from center of overall rf (normalized units)
  int		su_idx_st;		// starting sending unit index within each unit group to start connecting from -- allows for layers to have diff unit group structure
  int		ru_idx_st;		// starting receiving unit index within each unit group to start connecting from -- allows for layers to have diff unit group structure
  int		gp_n_cons;		// [-1 = all] number of units to connect within each group -- allows for layers to have diff unit group structure

  void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, int alloc_loop) override;
  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) override;

  TA_SIMPLE_BASEFUNS(TiledGpRFOneToOnePrjnSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // TiledGpRFOneToOnePrjnSpec_h
