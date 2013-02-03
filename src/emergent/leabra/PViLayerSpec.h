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

#ifndef PViLayerSpec_h
#define PViLayerSpec_h 1

// parent includes:
#include <PVLVLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(PVMiscSpec);

class LEABRA_API PVMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for PV layer spec
INHERITED(SpecMemberBase)
public:
  float         min_pvi;        // #DEF_0.4 minimum pvi value -- PVi is not allowed to go below this value for the purposes of computing the PV delta value: pvd = PVe - MAX(PVi,min_pvi)
  bool          pvi_scale_min;  // if both the PVe and PVi values are below min_pvi, then scale the result by (PVi/min_pvi) -- as PVi gets lower, meaning that it expects to be doing poorly, then punish the system less (but still punish it)
  float         prior_gain;     // #DEF_1 #MIN_0 #MAX_1 #EXPERT #AKA_prior_discount how much of the prior PV delta value (pvd = PVe - MAX(PVi,min_pvi)) to subtract away in computing the net PV dopamine signal (PV DA = pvd_t - prior_gain * pvd_t-1)
  bool          er_reset_prior; // #EXPERT #DEF_true reset prior delta value (pvd_t-1) when external rewards are received (akin to absorbing rewards in TD)
  bool		no_y_dot; // #DEF_false if true do not use y-dot for phasic DA calculation (PVi)

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(PVMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};

TypeDef_Of(PViLayerSpec);

class LEABRA_API PViLayerSpec : public PVLVLayerSpec {
  // primary value inhibitory (PVi) layer: continously learns to expect primary reward values, contribute to overall dopamine with PV delta pvd = PVe - PVi; PV DA = pvd_t - pvd_t-1
INHERITED(PVLVLayerSpec)
public:
  PVMiscSpec    pv;             // misc parameters for the PV computation

  virtual void  Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as external rewards and change weights

  virtual float Compute_PVDa(LeabraLayer* lay, LeabraNetwork* net);
  // compute da contribution from PV
    virtual float Compute_PVDa_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                   float pve_val, LeabraNetwork* net);
    // #IGNORE
  virtual void  Update_PVPrior(LeabraLayer* lay, LeabraNetwork* net);
  // update the prior PV value, stored in pv unit misc_1 values -- at very end of trial
    virtual void Update_PVPrior_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                    bool er_avail);
    // #IGNORE

  // overrides:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  override bool Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }

  override void Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net);
  override bool Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(PViLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init()         { };
};

#endif // PViLayerSpec_h
