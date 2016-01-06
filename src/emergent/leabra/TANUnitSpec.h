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

#ifndef TANUnitSpec_h
#define TANUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TANActSpec);

class E_API TANActSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for TAN activation 
INHERITED(SpecMemberBase)
public:
  bool          plus_fm_pv_vs;  // #DEF_true drive plus phase activation from the MAX sending activation over any MarkerConSpec inputs -- allows TAN's to learn to fire for expected or actual outcomes conveyed from PV and VSPatch outcome-predicting neurons
  bool          send_plus;      // #CONDSHOW_ON_plus_fm_pv_vs send the plus phase training values to ach on receivers (otherwise only send the learned minus phase activation)
  
  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(TANActSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};


eTypeDef_Of(TANUnitSpec);

class E_API TANUnitSpec : public LeabraUnitSpec {
  // Models the Tonically Active Neurons of the Striatum, which are driven in part by the parafasicular pathway conveying projections from the CNA, PPTG, and OFC, and exhibit a burst-pause firing dynamic at the time of actual rewards and expected rewards, which has a net disinhibitory effect on MSN's -- we hypothesize that this drives consolidation of MSN synaptic traces and updates the sign with concurrent dopamine firing, and then clears the trace -- only sends ACh during deep_raw_qtr
INHERITED(LeabraUnitSpec)
public:
  TANActSpec      tan;             // parameters for computing TAN activation

  virtual void  Send_ACh(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the ach value to sending projections: every cycle
  virtual void  Compute_PlusPhase(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // compute plus phase activations from marker cons inputs

  void	Compute_ApplyInhib
    (LeabraUnitVars* uv, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
     LeabraInhib* thr, float ival) override { };
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(TANUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // TANUnitSpec_h
