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

#ifndef ECoutLayerSpec_h
#define ECoutLayerSpec_h 1

// parent includes:
#include <ThetaPhaseLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ECoutLayerSpec);

class E_API ECoutLayerSpec : public ThetaPhaseLayerSpec {
  // layer spec for EC out layers that implements ThetaPhase learning -- automatically clamps to EC in activations in plus phase and records act_mid mid minus -- must use HippoEncoderConSpec for connections to learn from first half of minus phase (act_mid)
INHERITED(ThetaPhaseLayerSpec)
public:
  // following is main hook into code:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override bool CheckConfig_Layer(Layer* lay, bool quiet=false);
  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) { };
  // no mid minus -- could overwrite!

  virtual void 	ClampFromECin(LeabraLayer* lay, LeabraNetwork* net);
  // clamp ECout values from ECin values

  TA_SIMPLE_BASEFUNS(ECoutLayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // ECoutLayerSpec_h
