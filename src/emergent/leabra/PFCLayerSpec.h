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

eTypeDef_Of(PFCGateSpec);

class E_API PFCGateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(SpecMemberBase)
public:
  float         gate_ctxt_mod;  // #MIN_0 #MAX_1 how much to modulate context input at time of gating -- decreases influence of prior context in pfc units so they can better reflect new inputs -- smaller values = more clearing of prior context
  float         ctxt_decay;     // #MIN_0 #MAX_1 decay rate for context, per trial when no updating occurs
  int           max_maint;      // #DEF_-1;1;2 maximum duration for maintenance: -1 = no limit; 1 = gated trial only; 2 = one additional trial; do NOT use 0) -- after this number trials, active maintenance will be terminated
  float         ctxt_drift;     // #MIN_0 #MAX_1 on non-gating trials, when something is maintained, how much the deep context information can drift toward the new context (reflecting current superficial activations) -- this drift happens prior to decay (and the two are somewhat in opposition to each other -- typically use one or the other but not both)
  float					pregate_gain;   // #MIN_0 #MAX_1 #DEF_0;1 how active are (superficial) PFC units prior to, or in the absence of, gating?

  float         ctxt_decay_c;   // #READ_ONLY #HIDDEN 1-ctxt_decay

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

eTypeDef_Of(PFCLayerSpec);

class E_API PFCLayerSpec : public LeabraLayerSpec {
  // #AKA_PFCDeepLayerSpec Prefrontal cortex layer -- uses built-in TI mechanisms to update act_ctxt deep layer activations, but unlike regular TI networks, this only occurs at the time of BG-driven gating, not every trial.  Much of the impl is in PFCUnitSpec
INHERITED(LeabraLayerSpec)
public:

  SNrThalLayerSpec::GatingTypes	pfc_type;	// type of pfc units present within this PFC layer -- must be just one of the options (INPUT, IN_MNT, OUTPUT, etc.)
  PFCGateSpec		gate;		// parameters controlling the gating of pfc units


  virtual LeabraLayer* 	SNrThalLayer(LeabraLayer* lay);
  // find the SNrThal layer that this pfc layer receives from

  virtual LeabraLayer*  SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
	int& n_in, int& n_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt);
  // get the starting index for this set of pfc stripes within the snrthal gating layer -- returns the snrthal layer and starting index

  virtual void CopySNrThalGpData(LeabraLayer* lay, LeabraNetwork* net);
  // copy PBWMUnGpData from SNrThal layer

  virtual void  Compute_GateCycle(LeabraLayer* lay, LeabraNetwork* net);
  // compute at time of gating -- called every cycle -- modulates influence of context at time of gating in early minus phase

  virtual void  GateOnDeepPrjns_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
                                    int gpidx, LeabraNetwork* net);
  // gate on the deep prjns using PFCDeepGatedConSpec 
  virtual void  Compute_PreGatedAct(LeabraLayer* lay, LeabraNetwork* net);
  // modulates how active a PFC (superficial) layer can get before being gated; special case, e.g., for output gating layer, can use to only allow activation for trial immediately after gating -- called in CycleStats

  override void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  override void TI_ClearContext(LeabraLayer* lay, LeabraNetwork* net);

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
