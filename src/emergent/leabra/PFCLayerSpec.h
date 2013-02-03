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

#ifndef PFCLayerSpec_h
#define PFCLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:
#include <SNrThalLayerSpec>

// declare all other types mentioned but not required to include:

TypeDef_Of(PFCGateSpec);

class LEABRA_API PFCGateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(SpecMemberBase)
public:
  int		in_mnt;		// #DEF_1 #MIN_0 how many trials INPUT layers maintain after initial gating trial
  int		out_mnt;	// #DEF_0 #MIN_0 how many trials OUTPUT layers maintain after initial gating trial
   float	maint_pct;	// #DEF_0.9 #MIN_0 #MAX_1 what proportion (0-1) of activation value of maintaining units that comes from the gated maint activation value -- the rest comes from activation that would otherwise be computed live directly from current inputs
  float		maint_decay;	// #MIN_0 #MAX_1 #DEF_0:0.05 how much maintenance activation decays every trial
  float		maint_thr;	// #MIN_0 #DEF_0.2 when max activity in layer falls below this threshold, activations are no longer maintained and stripe is cleared
  float		clear_decay;	// #MIN_0 how much to decay existing activations when a gating signal comes into an already-maintaining stripe

  float		maint_pct_c;	// #READ_ONLY #NO_SAVE 1-maint_pct

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(PFCGateSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

TypeDef_Of(PFCLayerSpec);

class LEABRA_API PFCLayerSpec : public LeabraLayerSpec {
  // #AKA_PFCDeepLayerSpec Prefrontal cortex layer -- deep and superficial types
INHERITED(LeabraLayerSpec)
public:
  enum MaintUpdtAct {
    STORE,			// store current activity state in maintenance currents
    CLEAR,			// clear current activity state from maintenance currents
    CLEAR_DECAY,		// apply clear decay to existing maintained activations
    DECAY,			// decay maintenance currents by maint_decay factor
  };

  enum PFCLayer {
    SUPER,			// superficial layer -- activations are labile until trial after gating event, when they get locked into maintenance for at least one trial
    DEEP,			// deep layer -- not active at all until the trial when gating occurs -- after gating during that trial they track superficial, and then after that are locked into maintenance 
  };

  SNrThalLayerSpec::GatingTypes	pfc_type;	// type of pfc units present within this PFC layer -- must be just one of the options (INPUT, IN_MNT, OUTPUT, etc.)
  PFCLayer		pfc_layer;	// which layer type is this -- superficial (SUPER) or deep (DEEP)?
  PFCGateSpec		gate;		// parameters controlling the gating of pfc units


  virtual LeabraLayer* 	DeepLayer(LeabraLayer* lay);
  // find the DEEP layer for this SUPER layer
  virtual LeabraLayer* 	SNrThalLayer(LeabraLayer* lay);
  // find the SNrThal layer that this pfc deep layer receives from
  virtual LeabraLayer* 	LVeLayer(LeabraLayer* lay);
  // find the LVe layer that this pfc deep layer projects to
  virtual LeabraLayer* 	LViLayer(LeabraLayer* lay);
  // find the LVi layer that this pfc deep layer projects to

  virtual LeabraLayer*  SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
					int& n_in, int& n_in_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt);
  // get the starting index for this set of pfc stripes within the snrthal gating layer -- returns the snrthal layer and starting index

  virtual void Clear_Maint(LeabraLayer* lay, LeabraNetwork* net, int stripe_no=-1);
  // clear maintenance currents on given stripe or all stripes if stripe_no < 0 -- for program-based control over clearing
  virtual void Compute_MaintUpdt(LeabraLayer* lay, LeabraNetwork* net,
				 MaintUpdtAct updt_act, int stripe_no=-1);
  // perform given action (STORE, CLEAR) on given stripe or all stripes if stripe_no < 0 -- for program-based control over pfc functionality
  virtual void 	Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net);
  // clear various gating signals at the start of the trial
    virtual void Compute_MaintUpdt_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       MaintUpdtAct updt_act, LeabraNetwork* net);
    // update maintenance state variables (gc.h, misc_1) based on given action: ugp impl
    virtual void Compute_MaintAct_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
				      int gpidx,LeabraNetwork* net);
    // compute activation = maint_h for all units always
    virtual void GateOnDeepPrjns_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
				      int gpidx,LeabraNetwork* net);
    // compute activation = maint_h for all units always
  virtual void 	Compute_MidMinusAct_ugp(LeabraLayer* lay,
					Layer::AccessMode acc_md, int gpidx,
					LeabraNetwork* net);
  // computes mid minus (gating activation) state prior to gating
  virtual void 	Compute_Gating(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer activations -- each cycle during first minus phase

  virtual void 	Compute_FinalGating(LeabraLayer* lay, LeabraNetwork* net);
  // final gating at end of trial (phase_no == 1, PostSettle)
    virtual void Compute_ClearNonMnt(LeabraLayer* lay, LeabraNetwork* net);
    // clear the non-maintaining stripes at end of trial

  override void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return (pfc_layer == SUPER); }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return (pfc_layer == SUPER); }

  override TypeDef* 	UnGpDataType()  { return &TA_PBWMUnGpData; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(PFCLayerSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Defaults_init();
};

#endif // PFCLayerSpec_h
