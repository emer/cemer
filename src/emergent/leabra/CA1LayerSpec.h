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

#ifndef CA1LayerSpec_h
#define CA1LayerSpec_h 1

// parent includes:
#include <ThetaPhaseLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(CA1LayerSpec);

class E_API CA1LayerSpec : public ThetaPhaseLayerSpec {
  // layer spec for CA1 layers that implements ThetaPhase learning -- modulates EC_in and CA1 weight scale strengths, and records act_mid mid minus for auto encoder
INHERITED(ThetaPhaseLayerSpec)
public:
  float		recall_decay; 		// #DEF_1 proportion to decay layer activations at start of recall phase
  bool		use_test_mode;		// #DEF_true if network train_mode == TEST, then keep EC_in -> CA1 on, and don't do recall_decay -- makes it more likely to at least get input parts right

  // following are main hook into code:
  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE { };
  // no mid minus -- could overwrite!

  bool CheckConfig_Layer(Layer* lay, bool quiet=false) CPP11_OVERRIDE;
  virtual void 	ModulateCA3Prjn(LeabraLayer* lay, LeabraNetwork* net, bool ca3_on);
  // control the strength of the CA3 -> CA1 projection according to ca3_on arg
  virtual void 	ModulateECinPrjn(LeabraLayer* lay, LeabraNetwork* net, bool ecin_on);
  // control the strength of the EC_in -> CA1 projection according to ecin_on arg

  TA_SIMPLE_BASEFUNS(CA1LayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

#endif // CA1LayerSpec_h
