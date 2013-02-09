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

#ifndef V2toV4DepthPrjnSpec_h
#define V2toV4DepthPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <TiledGpRFPrjnSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(V2toV4DepthPrjnSpec);

class E_API V2toV4DepthPrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec that only connects to a specific depth in V2 border ownership input layer
INHERITED(TiledGpRFPrjnSpec)
public:
  int		depth_idx;	// which depth index (0..) to connect to

  override void	Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
				  int rgpidx, int sgpidx, int alloc_loop);

  TA_SIMPLE_BASEFUNS(V2toV4DepthPrjnSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // V2toV4DepthPrjnSpec_h
