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

#ifndef GPiUnitSpec_h
#define GPiUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GPiMiscSpec);

class E_API GPiMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra weighting of Go vs. NoGo inputs
INHERITED(SpecMemberBase)
public:
  float         net_gain;        // extra netinput gain factor to compensate for reduction in netinput from subtracting away nogo -- this is IN ADDITION to adding the nogo factor as an extra gain: net = (net_gain + nogo) * (go_in - nogo * nogo_in)
  float         nogo;            // #MIN_0 #DEF_0.01 how much to weight NoGo inputs relative to Go inputs (which have an implied weight of 1.0)
  float         gate_thr;        // threshold applied to activation to drive gating -- when any unit activation gets above this threshold, it sends the activation through sending projections to the thal field in Matrix units, otherwise it sends a 0
  bool          thr_act;         // #DEF_true apply threshold to unit act_eq activations -- this is what is sent to the InvertUnitSpec, so we effectively threshold the gating output

  float         tot_gain;        // #HIDDEN #EXPERT net_gain + nogo
  
  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(GPiMiscSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(GPiUnitSpec);

class E_API GPiUnitSpec : public LeabraUnitSpec {
  // GPi globus pallidus internal segment, analogous with SNr -- major output pathway of the basal ganglia.  This integrates Go and NoGo inputs, computing netin = Go - go_nogo.nogo * NoGo -- also sends act to thal field on Matrix layers that it sends to, to drive credit assignment learning in Matrix
INHERITED(LeabraUnitSpec)
public:
  GPiMiscSpec    gpi;      // parameters controlling the gpi functionality: how to weight the Go vs. NoGo pathway inputs, and gating threshold

  void	Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  virtual void  Send_Thal(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the act value as thal to sending projections: every cycle

  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  // no learning in this one..
  void 	Compute_dWt(UnitVars* u, Network* net, int thr_no) override { };
  void	Compute_dWt_Norm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Weights(UnitVars* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Unit* un, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(GPiUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // GPiUnitSpec_h
