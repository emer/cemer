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

#ifndef LeabraContextUnitSpec_h
#define LeabraContextUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(CtxtUpdateSpec);

class E_API CtxtUpdateSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra context updating specifications
INHERITED(SpecMemberBase)
public:
  float		fm_hid;		// from hidden (inputs to context layer)
  float		fm_prv;		// from previous context layer values (maintenance)
  float		to_out;		// outputs from context layer

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  SIMPLE_COPY(CtxtUpdateSpec);
  TA_BASEFUNS(CtxtUpdateSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(CtxtNSpec);

class E_API CtxtNSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra context counting specifications
INHERITED(SpecMemberBase)
public:
  int		n_trials;	// #MIN_1 update every n trials
  int		n_offs;		// #MIN_0 offset of n during count, ex using 2 lays with 0 and N/2 gives half alternating offset

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(CtxtNSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize() 	{ n_trials = 2; n_offs = 0; }
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(LeabraContextUnitSpec);

class E_API LeabraContextUnitSpec : public LeabraUnitSpec {
  // context units that copy from their recv projection, which should have one-to-one connections with a source layer -- this is one way to achieve simple recurrent network (SRN) behavior, that has more flexibility and uses a separate context layer for visualization purposes -- also consider using DeepCtxtConSpec connections within a single layer, which implements this same form of computation more efficiently, but less transparently and with fewer options
INHERITED(LeabraUnitSpec)
public:
  enum UpdateCriteria {
    UC_TRIAL = 0, // updates every trial (traditional "ContextLayer" behavior)
    UC_MANUAL, // manually updated via TriggerUpdate
    UC_N_TRIAL, // updates every n trials
  };
  
  static const String do_update_key;
  // user data key for variable on layer that holds whether to do updating now or not -- used by TriggerUpdate code
  UpdateCriteria update_criteria; // #DEF_UC_TRIAL #NO_SAVE_EMPTY how to determine when to copy the sending layer
  CtxtUpdateSpec updt;		// ctxt updating constants: from hidden, from previous values (hysteresis), outputs from context (n/a on simple gate layer)
  CtxtNSpec	n_spec; // #CONDSHOW_ON_update_criteria:UC_N_TRIAL trials per update and optional offset for multi
  
  virtual void TriggerUpdate(LeabraLayer* lay, bool update);
  // #CAT_Context manually set update trigger status -- must be both set and un-set manually -- flag remains set until explicitly called with update=false -- tested on first cycle of processing within a trial -- can always be called even if not on MANUAL -- sets user data do_update_key as state that triggers update

  virtual void Compute_Context(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // #CAT_Context compute context activation
  virtual bool ShouldUpdateNow(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // #CAT_Context test whether it is time to update context rep now..

  void	Compute_NetinInteg(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;

  void 	Compute_dWt(UnitState* u, Network* net, int thr_no) override { };
  void	Compute_Weights(UnitState* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(LeabraContextUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LeabraContextUnitSpec_h
