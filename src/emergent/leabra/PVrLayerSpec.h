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

#ifndef PVrLayerSpec_h
#define PVrLayerSpec_h 1

// parent includes:
#include <PVLVLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(PVDetectSpec);

class LEABRA_API PVDetectSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for detecting if a primary value is present or expected -- just learns with value 1 for PV present, .5 for absent
INHERITED(SpecMemberBase)
public:
  float         thr;    // #DEF_0.7 threshold on PVr value, above which PV is considered present (i.e., reward) -- PVr learns a 1 for all reward-valence cases, regardless of value, and .5 for reward absent

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(PVDetectSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};

TypeDef_Of(PVrLayerSpec);

class LEABRA_API PVrLayerSpec : public PVLVLayerSpec {
  // primary value reward detection layer: learns when rewards are expected to occur -- gets a 1 for any primary value feedback (reward or punishment), and .5 otherwise
INHERITED(PVLVLayerSpec)
public:
  PVDetectSpec  pv_detect;      // primary reward value detection spec: detect if a primary reward is expected based on PVr value

  virtual void  Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as external rewards and change weights

  virtual bool  Compute_PVDetect(LeabraLayer* lay, LeabraNetwork* net);
  // detect PV expectation based on PVr value -- happens at end of minus phase, based on unit activations then

  // overrides:
  override bool Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  override void Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net);
  override bool Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_BASEFUNS_NOCOPY(PVrLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init()         { };
};

#endif // PVrLayerSpec_h
