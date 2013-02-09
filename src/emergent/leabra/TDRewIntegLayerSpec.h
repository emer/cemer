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

#ifndef TDRewIntegLayerSpec_h
#define TDRewIntegLayerSpec_h 1

// parent includes:
#include <ScalarValLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TDRewIntegSpec);

class E_API TDRewIntegSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for TDRewIntegLayerSpec
INHERITED(SpecMemberBase)
public:
  float		discount;	// discount factor for V(t+1) from TDRewPredLayer
  bool		max_r_v;	// represent the maximum of extrew (r) and tdrewpred estimate of V(t+1) instead of the sum of these two factors -- produces a kind of "absorbing" reward function instead of a cumulative reward function

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(TDRewIntegSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(TDRewIntegLayerSpec);

class E_API TDRewIntegLayerSpec : public ScalarValLayerSpec {
  // integrates perceived and external rewards: delta over phases = DA td-like signal. minus phase = prev exp rew V^(t), plus phase = extrew (r) + tdrewpred computing V(t+1)
INHERITED(ScalarValLayerSpec)
public:
  TDRewIntegSpec	rew_integ;	// misc specs for TDRewIntegLayerSpec

  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  // never learn
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TDRewIntegLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // TDRewIntegLayerSpec_h
