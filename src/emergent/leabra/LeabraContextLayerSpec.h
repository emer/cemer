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

#ifndef LeabraContextLayerSpec_h
#define LeabraContextLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(CtxtUpdateSpec);

class LEABRA_API CtxtUpdateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra context updating specifications
INHERITED(SpecMemberBase)
public:
  float		fm_hid;		// from hidden (inputs to context layer)
  float		fm_prv;		// from previous context layer values (maintenance)
  float		to_out;		// outputs from context layer

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  SIMPLE_COPY(CtxtUpdateSpec);
  TA_BASEFUNS(CtxtUpdateSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

TypeDef_Of(CtxtNSpec);

class LEABRA_API CtxtNSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra context counting specifications
INHERITED(SpecMemberBase)
public:
  int		n_trials;	// #MIN_1 update every n trials
  int		n_offs;		// #MIN_0 offset of n during count, ex using 2 lays with 0 and N/2 gives half alternating offset

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(CtxtNSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize() 	{ n_trials = 2; n_offs = 0; }
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

TypeDef_Of(LeabraContextLayerSpec);

class LEABRA_API LeabraContextLayerSpec : public LeabraLayerSpec {
  // context layer that copies from its recv projection (like an input layer)
INHERITED(LeabraLayerSpec)
public:
  enum UpdateCriteria {
    UC_TRIAL = 0, // updates every trial (traditional "ContextLayer" behavior)
    UC_MANUAL, // manually updated via TriggerUpdate
    UC_N_TRIAL, // updates every n trials
  };
  
  UpdateCriteria update_criteria; // #DEF_UC_TRIAL #NO_SAVE_EMPTY how to determine when to copy the sending layer
  CtxtUpdateSpec updt;		// ctxt updating constants: from hidden, from previous values (hysteresis), outputs from context (n/a on simple gate layer)
  CtxtNSpec	n_spec; // #CONDSHOW_ON_update_criteria:UC_N_TRIAL trials per update and optional offset for multi
  
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // clamp from act_p values of sending layer
  override bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  // don't do any learning:
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }

  void TriggerUpdate(LeabraLayer* lay); // manually trigger an update of the context layer -- generally called at end of a Trial -- can always be called even if not on MANUAL
  
#ifndef __MAKETA__
  DumpQueryResult Dump_QuerySaveMember(MemberDef* md);
#endif

  TA_SIMPLE_BASEFUNS(LeabraContextLayerSpec);
  
protected:
  SPEC_DEFAULTS;
  STATIC_CONST String do_update_key;
  virtual void Compute_Context(LeabraLayer* lay, LeabraUnit* u, LeabraNetwork* net);
  // get context source value for given context unit

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // LeabraContextLayerSpec_h
