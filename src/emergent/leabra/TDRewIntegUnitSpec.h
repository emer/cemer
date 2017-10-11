// Copyright 2017, Regents of the University of Colorado,
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

#ifndef TDRewIntegUnitSpec_h
#define TDRewIntegUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TDRewIntegSpec);

class E_API TDRewIntegSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra misc specs for TDRewIntegUnitSpec
INHERITED(SpecMemberBase)
public:
  float		discount;	// discount factor for V(t+1) from TDRewPredLayer
  bool		max_r_v;	// represent the maximum of extrew (r) and tdrewpred estimate of V(t+1) instead of the sum of these two factors -- produces a kind of "absorbing" reward function instead of a cumulative reward function

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(TDRewIntegSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(TDRewIntegUnitSpec);

class E_API TDRewIntegUnitSpec : public LeabraUnitSpec {
  // integrates perceived and external rewards: delta over phases = DA td-like signal. minus phase = prev exp rew V^(t), plus phase = extrew (r) + tdrewpred computing V(t+1)
INHERITED(LeabraUnitSpec)
public:
  TDRewIntegSpec	rew_integ;	// misc specs for TDRewIntegUnitSpec

  virtual void  Compute_TDRewInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);

  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  // nullify other major routines:
  void	Compute_NetinInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override { };
  void 	Compute_dWt(UnitState* u, Network* net, int thr_no) override { };
  void	Compute_Weights(UnitState* u, Network* net, int thr_no) override { };

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false)  override;

  TA_SIMPLE_BASEFUNS(TDRewIntegUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // TDRewIntegUnitSpec_h
