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
  int		in_mnt;		// #DEF_1 #MIN_0 how many trials INPUT layers maintain after initial gating trial
  int		out_mnt;	// #DEF_0 #MIN_0 how many trials OUTPUT layers maintain after initial gating trial
  float		out_nogate_gain; // #MIN_0 #MAX_1 how active are output gating units in absence of gating?

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(PFCGateSpec);
protected:
  SPEC_DEFAULTS;
  //  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(PFCLayerSpec);

class E_API PFCLayerSpec : public LeabraLayerSpec {
  // #AKA_PFCDeepLayerSpec Prefrontal cortex layer -- deep and superficial types
INHERITED(LeabraLayerSpec)
public:

  SNrThalLayerSpec::GatingTypes	pfc_type;	// type of pfc units present within this PFC layer -- must be just one of the options (INPUT, IN_MNT, OUTPUT, etc.)
  PFCGateSpec		gate;		// parameters controlling the gating of pfc units


  virtual LeabraLayer* 	SNrThalLayer(LeabraLayer* lay);
  // find the SNrThal layer that this pfc deep layer receives from

  virtual LeabraLayer*  SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
	int& n_in, int& n_in_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt);
  // get the starting index for this set of pfc stripes within the snrthal gating layer -- returns the snrthal layer and starting index

  virtual void CopySNrThalGpData(LeabraLayer* lay, LeabraNetwork* net);
  // copy PBWMUnGpData from SNrThal layer

  virtual void  Compute_OutGatedAct(LeabraLayer* lay, LeabraNetwork* net);
  // for output gating layer, only allow activation for trial immediately after gating -- called in CycleStats

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
