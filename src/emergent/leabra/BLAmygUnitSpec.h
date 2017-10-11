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

#ifndef BLAmygUnitSpec_h
#define BLAmygUnitSpec_h 1

// parent includes:
#include <D1D2UnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(BLAmygDaMod);

class E_API BLAmygDaMod : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for da_mod gain factors and other learning parameters in BL Amygdala learning
INHERITED(SpecMemberBase)
public:
  
  
  float         burst_da_gain;  // #MIN_0 #DEF_0.1 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 #DEF_0.1 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign! should be small for acq, but roughly equal to burst_da_gain for ext 
  
  bool          lrn_mod_act;    // #DEF_false if true, phasic dopamine values effect learning by modulating net_syn values (Compute_NetinExtras() - and thus unit activations; - CAUTION - very brittle and hard to use due to unintended consequences!
  float         pct_act;        // #CONDSHOW_LRN_MOD_ACT_true #DEF_1;0.8 proportion of activation used for computing dopamine modulation value -- 1-pct_act comes from net-input -- activation is more differentiated and leads to more differentiated representations, but if there is no activation then dopamine modulation has no effect, so it depends on having that activation signal
  float         us_clamp_avg;   // #CONDSHOW_LRN_MOD_ACT_true #DEF_0.2 averaging factor for quasi-clamping US (PV) values when sent using a SendDeepRaw connection to modulate net_syn values which in turn modulates actual activation values -- more gradual form of clamping; requires SendDeepRawConSpec fm PosPV/NegPV layer
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(BLAmygDaMod);
protected:
  SPEC_DEFAULTS;
  // void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(BLAmygAChMod);

class E_API BLAmygAChMod : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for ach_mod gain factors
INHERITED(SpecMemberBase)
public:
  bool          on;             // whether to modulate activations as a function of ach levels
  float         mod_min;        // #DEF_0.8 minimum ach modulation factor -- net modulation is mod_mid + ach * (1 - mod_min)
  float         mod_min_c;      // #READ_ONLY #EXPERT 1 - mod_min
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(BLAmygAChMod);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(BLAmygUnitSpec);

class E_API BLAmygUnitSpec : public D1D2UnitSpec {
  // Basal Lateral Amygdala units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:
  DAReceptor    dar;            // type of predominant dopamine receptor: D1 vs. D2 -- determines whether primarily appetitive / relief (D1) or aversive / disappointment (D2)
  BLAmygDaMod   bla_da_mod;     // extra parameters for dopamine modulation of activation for BLA amyg units; or, just modulate learning (wt changes) directly w/o affecting actual unit activations
  BLAmygAChMod  bla_ach_mod;   // ach modulation of activation for BLA amyg units
  
  bool          deep_mod_zero;  // for unit group-based extinction-coding layers; modulation coming from the corresponding BLA acquisition layer via deep_mod_net -- when this modulation signal is below deep.mod_thr, does it have the ability to zero out the extinction activations?  i.e., is the modulation required to enable extinction firing?
  

  float Compute_DaModNetin(LeabraUnitState_cpp* uv, LeabraNetwork* net,
                           int thr_no, float& net_syn) override;

  void  Compute_DeepMod(LeabraUnitState_cpp* uv, LeabraNetwork* net,
                        int thr_no) override;
  float Compute_NetinExtras(LeabraUnitState_cpp* u, LeabraNetwork* net,
                            int thr_no, float& net_syn) override;
  
  void Compute_ActFun_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void  Quarter_Final_RecVals(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  
  TA_SIMPLE_BASEFUNS(BLAmygUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // BLAmygUnitSpec_h
