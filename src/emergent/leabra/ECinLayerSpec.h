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

#ifndef ECinLayerSpec_h
#define ECinLayerSpec_h 1

// parent includes:
#include <ThetaPhaseLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ECinLayerSpec);

class E_API ECinLayerSpec : public ThetaPhaseLayerSpec {
  // layer spec for EC input layers that implements ThetaPhase learning -- this serves only as a marker for ECout layers to search for -- no new functionality over LeabraLayerSpec
INHERITED(ThetaPhaseLayerSpec)
public:
  // following is main hook into code:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  TA_SIMPLE_BASEFUNS(ECinLayerSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};

#endif // ECinLayerSpec_h
