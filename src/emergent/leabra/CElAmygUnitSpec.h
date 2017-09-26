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

#ifndef CElAmygUnitSpec_h
#define CElAmygUnitSpec_h 1

// parent includes:
#include <D1D2UnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(CElAmygDaMod);

class E_API CElAmygDaMod : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for da_mod gain factors and other learning parameters in CEl central amygdala lateral learning
INHERITED(SpecMemberBase)
public:
  
  float         burst_da_gain;  // #MIN_0 #DEF_0;0.04 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 #DEF_0.1 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign! should be small for acq, but roughly equal to burst_da_gain for ext 
  bool          acq_deep_mod;   // #DEF_true use deep_mod_net for value from acquisition / go units, instead of inhibition current (otherwise use gi_syn) -- allows simpler parameter setting without titrating inhibition and this learning modulation signal
  
  bool          lrn_mod_act;  // #DEF_false if true, phasic dopamine values effect learning by modulating net_syn values (Compute_NetinExtras() - and thus unit activations; - CAUTION - very brittle and hard to use due to unintended consequences!
  float         us_clamp_avg;   // #CONDSHOW_LRN_MOD_ACT_true #DEF_0.2 averaging factor for quasi-clamping US (PV) values when sent using a SendDeepRaw connection to modulate net_syn values which in turn modulates actual activation values -- more graded form of clamping..
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(CElAmygDaMod);
protected:
  SPEC_DEFAULTS;
  // void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(CElAmygUnitSpec);

class E_API CElAmygUnitSpec : public D1D2UnitSpec {
  // Central Amygdala (lateral) units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:
  enum AcqExt { // acquisition (Go, On) vs. extinction (NoGo, Off) type
    ACQ,        // acquisition (Go, On) neurons -- get direct US projections and learn CS / context etc associations based on phasic dopamine modulation
    EXT,        // extinction (NoGo, Off) neurons -- get indirect US estimates from Acq inhibitory projections and learn CS / context etc associations based phasic dopamine modulatio
  };

  AcqExt        acq_ext;        // acquisition vs. extinction sub-type
  Valence       valence;        // US valence coding -- appetitive vs. aversive
  DAReceptor    dar;            // #READ_ONLY #SHOW type of dopamine receptor: D1 vs. D2 -- computed automatically from acq_ext and valence
  CElAmygDaMod  cel_da_mod;     // extra parameters for dopamine modulation of activation for CEl amyg units

  bool          deep_mod_zero;  // for unit group-based extinction-coding layers; modulation coming from the corresponding acquisition layer via deep_mod_net -- when this modulation signal is below deep.mod_thr, does it have the ability to zero out the extinction activations?  i.e., is the modulation required to enable extinction firing?
  
  float Compute_DaModNetin(LeabraUnitVars* uv, LeabraNetwork* net,
                           int thr_no, float& net_syn) override;

  void  Compute_DeepMod(LeabraUnitVars* uv, LeabraNetwork* net,
                        int thr_no) override;
  float Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                            int thr_no, float& net_syn) override;
  void  Compute_ActFun_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void  Quarter_Final_RecVals(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(CElAmygUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // CElAmygUnitSpec_h
