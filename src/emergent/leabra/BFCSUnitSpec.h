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

#ifndef BFCSUnitSpec_h
#define BFCSUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:
class LeabraLayer; //

eTypeDef_Of(BFCSAChSpec);

class E_API BFCSAChSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra basal forebrain cholinergic system (BFCS) specs for ach computation 
INHERITED(SpecMemberBase)
public:
  float         tonic_ach;      // #DEF_0.5 set a tonic basline level of ach
  float         tau;            // time constant of integration over trials for delta input signals to drive changes in ach levels -- updates are computed at the trial time scale so that is the operative scale for this time scale 
  float         cea_gain;       // central nucleus of the amygdala input gain -- how strongly delta factor here contributes to overall BFCS activation
  float         vs_gain;        // ventral striatum input gain -- how strongly delta factor here contributes to overall BFCS activation

  float         dt;             // #READ_ONLY #EXPERT rate = 1 / tau
  
  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(BFCSAChSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
  
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};


eTypeDef_Of(BFCSUnitSpec);

class E_API BFCSUnitSpec : public LeabraUnitSpec {
  // Models basal forebrain cholinergic system (BFCS) according to a Pearce-Hall style saliency dynamic, where lack of predictability in reward predictions drives increased ACh attentional modulation output -- updates and sends ACh at start of trial, based on trial-level delta values (act - act_q0) over its inputs
INHERITED(LeabraUnitSpec)
public:
  BFCSAChSpec     ach;          // parameters for the ach computation

  virtual void  Send_ACh(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // send the ach value to sending projections: start of quarters
  virtual void  Compute_ACh(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // compute ach value from delta change values: at end of trial

  void  Quarter_Init_Unit(LeabraUnitState_cpp* uv, LeabraNetwork* net, int thr_no) override;
  void  Compute_ActTimeAvg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  void	Compute_NetinInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  
  void 	Compute_dWt(UnitState* u, Network* net, int thr_no) override { };
  void	Compute_Weights(UnitState* u, Network* net, int thr_no) override { };

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(BFCSUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init() { Initialize(); }
};

#endif // BFCSUnitSpec_h
