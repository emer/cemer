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

#ifndef LVeLayerSpec_h
#define LVeLayerSpec_h 1

// parent includes:
#include <PVLVLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LVMiscSpec);

class E_API LVMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for PV layer spec
INHERITED(SpecMemberBase)
public:
  bool          gd_pvlv;        // #DEF_true this is the new default: use goal-driven PVLV formulation, which ignores LVi and always computes y-dot directly on LVe values, and has 0 dopamine for trial after external reward -- this is when the new temporal derivative is being reestablished
  float         min_lvi;        // #CONDSHOW_OFF_gd_pvlv #DEF_0.1;0.4 minimum lvi value -- LVi is not allowed to go below this value for the purposes of computing the LV delta value: lvd = LVe - MAX(LVi,min_lvi)
  float         prior_gain;     // #DEF_1 #MIN_0 #MAX_1 #EXPERT #AKA_prior_discount how much of the the prior time step LV delta value (lvd = LVe - MAX(LVi,min_lvi)) to subtract away in computing the net LV dopamine signal (LV DA = lvd_t - prior_gain * lvd_t-1)
  bool          er_reset_prior; // #EXPERT #DEF_true reset prior delta value (lvd_t-1) when external rewards are received (akin to absorbing rewards in TD)

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LVMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};

eTypeDef_Of(LVeLayerSpec);

class E_API LVeLayerSpec : public PVLVLayerSpec {
  // learns value based on inputs that are associated with rewards, only learns at time of primary rewards (filtered by PV system). This is excitatory version LVe.  LV contribution to dopamine is based on LV delta lvd = LVe - LVi; LV DA = lvd_t - lvd_t-1
INHERITED(PVLVLayerSpec)
public:
  LVMiscSpec    lv;             // misc parameters controlling the LV computation (note: only the LVe instance of these parameters are used)

  virtual void  Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // if primary value detected (present/expected), compute plus phase activations for learning, and actually change weights

  virtual float Compute_LVDa(LeabraLayer* lve_lay, LeabraLayer* lvi_lay, LeabraNetwork* net);
  // compute da contribution from Lv, based on lve_layer and lvi_layer activations (multiple subgroups allowed)
  virtual float Compute_LVDa_ugp(LeabraLayer* lve_lay, LeabraLayer* lvi_lay,
                                 Layer::AccessMode lve_acc_md, int lve_gpidx,
                                 Layer::AccessMode lvi_acc_md, int lvi_gpidx,
                                 LeabraNetwork* net);
    // #IGNORE

  virtual void  Update_LVPrior(LeabraLayer* lay, LeabraNetwork* net);
  // update the prior Lv value, stored in lv unit misc_1 values
    virtual void Update_LVPrior_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                    bool er_avail);
    // #IGNORE

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;
  void PostSettle(LeabraLayer* lay, LeabraNetwork* net) override;
  bool Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) override { return false; }

  void Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) override;
  bool Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) override;
  bool Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) override;

  void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net) override;

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(LVeLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init()         { };
};

eTypeDef_Of(LViLayerSpec);

class E_API LViLayerSpec : public LVeLayerSpec {
  // inhibitory/slow version of LV layer spec: (just a marker for layer; same functionality as LVeLayerSpec)
INHERITED(LVeLayerSpec)
public:
  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;

  TA_BASEFUNS_NOCOPY(LViLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize()            { };
  void  Destroy()               { };
  void  Defaults_init()         { };
};

#endif // LVeLayerSpec_h
