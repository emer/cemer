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

#ifndef LHbRMTgUnitSpec_h
#define LHbRMTgUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:
class LeabraLayer; //

eTypeDef_Of(LHbRMTgSpecs);

class E_API LHbRMTgSpecs : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs and params for LHbRMTg inputs
INHERITED(SpecMemberBase)
public:
  bool          patch_cur;      // #DEF_false use current trial patch activations -- otherwise use previous trial -- current trial is appropriate for simple discrete trial environments (e.g., with some PBWM models), whereas previous is more approprate for trials with more realistic temporal structure
  
  // TODO: matrix_td will need extensive rework if we still want to use it because of four different matrix inputs now entering LHb - LHbRMTgUnitSpec::see Quarter_Final()
  bool          matrix_td;      // #DEF_false compute temporal derivative over matrix pos inputs to produce a dip when LV values go down (misc_1 holds the prior trial net input) -- otherwise matrix is matrix_ind - matrix_dir difference between NoGo and Go (dips driven by greater NoGo than Go balance)
  
  float         min_pvneg;      // #DEF_0.1 #MIN_0 #MAX_1 proportion of PVNeg that cannot be predicted away - can never afford to take danger for granted!
  
  bool          rec_data;       // #DEF_false record values

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LHbRMTgSpecs);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(LHbRMTgGains);

class E_API LHbRMTgGains : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gains for LHbRMTg inputs
INHERITED(SpecMemberBase)
public:
  float         all;            // #MIN_0 #DEF_1 final overall gain on everything
  float         vspatch_pos_D1;      // #MIN_0 #DEF_1 VS patch D1 APPETITIVE pathway - versus pos PV outcomes
  float         vspatch_pos_D2;      // #MIN_0 #DEF_1 VS patch D2 APPETITIVE pathway versus vspatch_pos_D1
  float         vspatch_pos_net_neg_gain; // #DEF_0.2 proportion of positive reward prediction error (RPE) to use if RPE results from a predicted omission of positive reinforcement - e.g., conditioned inhibitor
  float         vsmatrix_pos_D1;  // #MIN_0 #DEF_1 gain on VS matrix D1 APPETITIVE guys
  float         vsmatrix_pos_D2;  // #MIN_0 #DEF_1 - VS matrix D2 APPETITIVE
  
  float         vspatch_neg_D1;      // #MIN_0 #DEF_1 VS patch D1 pathway versus neg PV outcomes
  float         vspatch_neg_D2;      // #MIN_0 #DEF_1 VS patch D2 pathway versus vspatch_neg_D1
  float         vspatch_neg_net_neg_gain; // #DEF_0.2 TODO: do we need any version of this guy?
  
  float         vsmatrix_neg_D1; // #MIN_0 #DEF_1 - VS matrix D1 AVERSIVE
  float         vsmatrix_neg_D2; // #MIN_0 #DEF_1 - VS matrix D2 AVERSIVE

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LHbRMTgGains);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};


eTypeDef_Of(LHbRMTgUnitSpec);

class E_API LHbRMTgUnitSpec : public LeabraUnitSpec {
  // combined lateral habenula and RMTg units -- receives from Patch and Matrix Direct and Indirect pathways, along with primary value (PV) positive and negative valence drivers, and computes dopamine dip signals, represented as positive activations in these units, which are then translated into dips through a projection to the VTAUnitSpec
INHERITED(LeabraUnitSpec)
public:
  LHbRMTgSpecs   lhb;         // lhb options and misc specs
  LHbRMTgGains   gains;       // gain parameters (multiplicative constants) for various sources of inputs
  virtual void	Compute_Lhb(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // compute the LHb value based on recv projections from VSMatrix_dir/ind, VSPatch_dir/ind, and PV_pos/neg
  
  virtual bool  GetRecvLayers(LeabraUnit* u,
                              LeabraLayer*& pv_pos_lay,
                              LeabraLayer*& vspatch_pos_D1_lay,
                              LeabraLayer*& vspatch_pos_D2_lay,
                              LeabraLayer*& vsmatrix_pos_D1_lay,
                              LeabraLayer*& vsmatrix_pos_D2_lay,
                              LeabraLayer*& pv_neg_lay,
                              LeabraLayer*& vspatch_neg_D1_lay,
                              LeabraLayer*& vspatch_neg_D2_lay,
                              LeabraLayer*& vsmatrix_neg_D1_lay,
                              LeabraLayer*& vsmatrix_neg_D2_lay);
  // get the recv layers..
  
  void	Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_ApplyInhib(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no,
                           LeabraLayerSpec* lspec, LeabraInhib* thr, float ival) override { };
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  bool  CheckConfig_Unit(Unit* un, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(LHbRMTgUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LHbRMTgUnitSpec_h
