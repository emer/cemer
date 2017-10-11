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

#ifndef GPiInvUnitSpec_h
#define GPiInvUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GPiGateSpec);

class E_API GPiGateSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra timing of gating 
INHERITED(SpecMemberBase)
public:
  int           gate_cyc;       // #DEF_18 cycle within quarter to apply gating -- see gating_qtr for quarters when gating is computed -- we send thal_gate on this cycle in those quarters, regardless of whether our activation is over gating threshold
  bool          updt_net;        // we update the LeabraNetwork.times.thal_gate_cycle value whenver our gating timing is activated -- if there are multiple gating layers then might want to only listen to one of them -- importantly, this should not affect actual functioning of PBWM system, which depends on direct thal_gate signals -- only affects recording of act_g gating values for units outside of PBWM
  
  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(GPiGateSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(GPiMiscSpec);

class E_API GPiMiscSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra weighting of Go vs. NoGo inputs and other GPi params
INHERITED(SpecMemberBase)
public:
  float         net_gain;        // #DEF_3 extra netinput gain factor to compensate for reduction in netinput from subtracting away nogo -- this is IN ADDITION to adding the nogo factor as an extra gain: net = (net_gain + nogo) * (go_in - nogo * nogo_in)
  float         nogo;            // #MIN_0 #DEF_1;0.1 how much to weight NoGo inputs relative to Go inputs (which have an implied weight of 1.0)
  float         gate_thr;        // #DEF_0.2 threshold applied to activation to drive gating -- when any unit activation gets above this threshold, it sends the activation through sending projections to the thal field in Matrix units, otherwise it sends a 0
  bool          thr_act;         // #DEF_true apply threshold to unit act_eq activations (i.e., turn off units that are below threshold) -- this is what is sent to the InvertUnitSpec, so we effectively threshold the gating output
  float         min_thal;        // #DEF_0.2 minimum thal value to send to thal on layers we project to -- range between gate_thr and 1.0 is mapped into min_thal:1.0 range -- e.g., setting to 1.0 will effectively produce binary gating outputs -- set to 0 (or < gate_thr) to retain raw gate_thr to 1.0 range

  float         tot_gain;        // #EXPERT net_gain + nogo
  float         thal_rescale;    // #EXPERT (1.0 - min_thal) / (1.0 - gate_thr) -- multiplier for rescaling thal value from act_eq >= gate_thr onto min_thal range
  
  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(GPiMiscSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(GPiInvUnitSpec);

class E_API GPiInvUnitSpec : public LeabraUnitSpec {
  // #AKA_GPiUnitSpec Inverted GPi globus pallidus internal segment, analogous with SNr -- major output pathway of the basal ganglia.  This integrates Go and NoGo inputs, computing netin = Go - go_nogo.nogo * NoGo -- unlike real GPi units are typically off, and the most active wins through inhibitory competition -- is responsible for determining when gating happens, sends act to thal field on layers that it sends to -- can be used directly as a sole SNrThal gating layer, or indrectly with InvertUnitSpec to mimic actual non-inverted GPi in a projection to thalamus layer
INHERITED(LeabraUnitSpec)
public:
  Quarters       gate_qtr;    // #CAT_GPi quarter(s) during which GPi gating takes place -- typically Q1 and Q3
  GPiGateSpec    gate;        // #CAT_GPi timing for gating  within gate_qtr
  GPiMiscSpec    gpi;         // #CAT_GPi parameters controlling the gpi functionality: how to weight the Go vs. NoGo pathway inputs, and gating threshold

  inline  bool Quarter_GateNow(int qtr) { return gate_qtr & (1 << qtr); }
  // #CAT_Activation test whether gating happens in this quareter

  void	Compute_NetinRaw(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  virtual void  Send_Thal(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // send the act value as thal to sending projections: every cycle

  void	Compute_Act_Post(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  // no learning in this one..
  void 	Compute_dWt(UnitState* u, Network* net, int thr_no) override { };
  void	Compute_Weights(UnitState* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(GPiInvUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // GPiInvUnitSpec_h
