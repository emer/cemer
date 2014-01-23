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

#ifndef NVLayerSpec_h
#define NVLayerSpec_h 1

// parent includes:
#include <PVLVLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(NVSpec);

class E_API NVSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for novelty value learning
INHERITED(SpecMemberBase)
public:
  float         da_gain;        // #DEF_0:1 #MIN_0 gain for novelty value dopamine signal
  float         val_thr;        // #DEF_0.1 #MIN_0 threshold for value (training value is 0) -- value is zero below this threshold
  float         prior_gain;     // #DEF_1 #MIN_0 #MAX_1 #EXPERT #AKA_prior_discount how much of the prior NV delta value (nvd = NV - val_thr) to subtract away in computing the net NV dopamine signal (NV DA = nvd_t - prior_gain * nvd_t-1)
  bool          er_reset_prior; // #EXPERT #DEF_true reset prior delta value (nvd_t-1) when external rewards are received (akin to absorbing rewards in TD)

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(NVSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};

eTypeDef_Of(NVLayerSpec);

class E_API NVLayerSpec : public PVLVLayerSpec {
  // novelty value (NV) layer: starts with a bias of 1.0, and learns to activate 0.0 value -- value signal is how novel the stimulus is: NV delta nvd = NV - val_thr; NV DA = nvd_t - nvd_t-1
INHERITED(PVLVLayerSpec)
public:
  NVSpec        nv;     // novelty value specs

  virtual float Compute_NVDa_raw(LeabraLayer* lay, LeabraNetwork* net);
  // compute raw novelty value da value -- no gain factor
  virtual float Compute_NVDa(LeabraLayer* lay, LeabraNetwork* net);
  // compute novelty value da value -- with gain factor applied
  virtual void  Compute_NVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as train target value and change weights
  virtual void  Update_NVPrior(LeabraLayer* lay, LeabraNetwork* net);
  // update the prior Nv value, stored in nv unit misc_1 values

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  void PostSettle(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  bool Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) CPP11_OVERRIDE { return false; }
  void Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  bool Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  bool Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(NVLayerSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init()         { };
};

#endif // NVLayerSpec_h
