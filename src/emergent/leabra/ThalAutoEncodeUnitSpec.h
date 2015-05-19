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

#ifndef ThalAutoEncodeUnitSpec_h
#define ThalAutoEncodeUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(AutoEncodeSpecs);

class E_API AutoEncodeSpecs : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra thalamic auto-encoder specs
INHERITED(SpecMemberBase)
public:
  bool          binarize;       // apply a threshold and binarize the plus-phase deep-layer driven activation states
  float		thr;		// #DEF_0.5 threshold for binarizing the activations

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(AutoEncodeSpecs);
protected:
  SPEC_DEFAULTS;

 private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(ThalAutoEncodeUnitSpec);

class E_API ThalAutoEncodeUnitSpec : public LeabraUnitSpec {
  // #AKA_Deep5bClampUnitSpec When deep_qtr is active (which defines the effective plus phase), these units are exclusively driven by their deep_raw_net netinput, and otherwise they are just like regular neurons -- used for simulating thalamic relay cells as auto-encoder layers hard-clamped to deep_raw drivers in the plus phase
INHERITED(LeabraUnitSpec)
public:
  AutoEncodeSpecs       auto_enc; // auto-encoder specs

  float Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                           float& net_syn) override;
  void Compute_ActFun_Rate(LeabraUnitVars* u, LeabraNetwork* net,
                           int thr_no) override;
  void Trial_Init_SRAvg(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;

  bool DeepNormCopied() override { return true; }
  void Compute_DeepNorm(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
    

  TA_SIMPLE_BASEFUNS(ThalAutoEncodeUnitSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // ThalAutoEncodeUnitSpec_h
