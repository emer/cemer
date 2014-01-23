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
  float         min_lvi;        // #DEF_0.1;0.4 minimum lvi value -- LVi is not allowed to go below this value for the purposes of computing the LV delta value: lvd = LVe - MAX(LVi,min_lvi)
  bool          lvi_scale_min;  // if both the LVe and LVi values are below min_lvi, then scale the result by (LVi/min_lvi) -- as LVi gets lower, meaning that it expects to be doing poorly, then punish the system less (but still punish it)
  bool          lrn_pv_only;    // #DEF_true only compute weight changes on trials where primary rewards are expected or actually received -- the target PV value is only presented on such trials, but if this flag is off, it actually learns on other trials, but with whatever plus phase activation state happens to arise
  float		nopv_val;	// #CONDSHOW_ON_lrn_pv_only:false value to apply for learning on non-pv trials -- simulates a baseline effort cost for non-reward trials.  only works when lrn_pv_only is false.  see nopv_lrate for lrate multiplier for these trials, to independently manipulate how rapidly learning takes place
  float		nopv_lrate;	// #CONDSHOW_ON_lrn_pv_only:false learning rate for learning on non-pv trials -- see nopv_val for value that is clamped.  this can be used to simulate a baseline effort cost for non-reward trials.  only works when lrn_pv_only is false.
  float         prior_gain;     // #DEF_1 #MIN_0 #MAX_1 #EXPERT #AKA_prior_discount how much of the the prior time step LV delta value (lvd = LVe - MAX(LVi,min_lvi)) to subtract away in computing the net LV dopamine signal (LV DA = lvd_t - prior_gain * lvd_t-1)
  bool          er_reset_prior; // #EXPERT #DEF_true reset prior delta value (lvd_t-1) when external rewards are received (akin to absorbing rewards in TD)
  bool		no_y_dot; 	// #DEF_false don't use y-dot temporal derivative at all in computing LVe phasic DA 
  bool		pos_y_dot_only; // #DEF_false use only positive deviations for computing LVe phasic DA -- mutex with no_y_dot

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "LayerSpec"; }

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
    virtual void  Compute_LVCurLrate(LeabraLayer* lay, LeabraNetwork* net, LeabraUnit* u,
				     float lrate_mult);
    // set the cur_lrate for con specs coming into LV layer, using lrate_mult multiplier -- used for the nopv_lrate param

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

  void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  void PostSettle(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  bool Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) CPP11_OVERRIDE { return false; }

  void Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  bool Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;
  bool Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) CPP11_OVERRIDE;

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

#endif // LVeLayerSpec_h
