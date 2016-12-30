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

#ifndef BLAmygUnitSpec_h
#define BLAmygUnitSpec_h 1

// parent includes:
#include <D1D2UnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(BLAmygDaMod);

class E_API BLAmygDaMod : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for da_mod gain factors and other learning parameters in BL Amygdala learning
INHERITED(SpecMemberBase)
public:
  float         burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign! should be small for acq, but roughly equal to burst_da_gain for ext 
  float         us_clamp_avg;   // averaging factor for clamping US (PV) values when sent using a SendDeepRaw connection -- better form of hard-clamping..
  
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


eTypeDef_Of(BLAmygUnitSpec);

class E_API BLAmygUnitSpec : public D1D2UnitSpec {
  // Basal Lateral Amygdala units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:
  DAReceptor    dar;            // type of predominant dopamine receptor: D1 vs. D2 -- determines whether primarily appetitive / relief (D1) or aversive / disappointment (D2)
  BLAmygDaMod   bla_da_mod;     // extra parameters for dopamine modulation of activation for BLA amyg units

  float Compute_DaModNetin(LeabraUnitVars* uv, LeabraNetwork* net,
                           int thr_no, float& net_syn) override;

  void  Compute_DeepMod(LeabraUnitVars* uv, LeabraNetwork* net,
                        int thr_no) override;
  float Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                            int thr_no, float& net_syn) override;

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
