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

#ifndef MatrixLayerSpec_h
#define MatrixLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:
#include <SNrThalLayerSpec>

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixMiscSpec);

class E_API MatrixMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the matrix layer
INHERITED(SpecMemberBase)
public:
  bool          trace_learn;    // use trace-based learning of gating instead of LV DA-modulated learning snrthal activity multiplies da value -- this should be true for LV-modulated PBWM, but false for new trace_learn version (see MatrixConSpec parameter)
  float		da_gain;	// #DEF_0:2 #MIN_0 overall gain for da modulation of matrix units for the purposes of learning (ONLY) -- bias da is set directly by gate_bias params -- also, this value is in addition to other "upstream" gain parameters, such as vta.da.gain -- it is recommended that you leave those upstream parameters at 1.0 and adjust this parameter, as it also modulates rnd_go.nogo.da which is appropriate
  float		nogo_wtscale_inc; // multiplier on effective weight scale for projections into the winning stripe's nogo units in the second half of the minus phase, after gating -- the weight scale for all other stripes goes to 0 -- only the winning stripe is allowed to become active
  float		nogo_inhib;	// #DEF_0:0.1 #MIN_0 how strongly does the nogo stripe inhibit the go stripe -- net inputs are rescaled downward by (1 - (nogo_inhib*avg_nogo_act)) -- reshapes the competition so other stripes will win
  float		pvr_inhib;	// #DEF_0;0.8 #MIN_0 #MAX_1 amount of inhibition to apply to Go units based on pvr status -- inhibits output gating when no reward is expected, and otherwise inhibits input & maint when reward is expected -- net inputs are rescaled downward by (1 - pvr_inhib) -- reshapes the competition so other stripes will win
  float		refract_inhib;	// #DEF_0;0.5 #MIN_0 #MAX_1 amount of refractory inhibition to apply to Go units for stripes that are in maintenance mode for one trial -- net inputs are rescaled downward by (1 - refract_inhib) -- reshapes the competition so other stripes will win
  float		no_pfc_thr;	// #DEF_0:0.02 threshold for considering there to be no corresponding pfc activity -- average activity within the corresponding PFC_s stripe must be above this level -- if not, netin inhibition is applied to stripe by rescaling downward by (1 - no_pfc_inhib) -- enforces this as a hard constraint even if it is a soft constraint in connectivity
  float		no_pfc_inhib;	// #DEF_0:0.2 #MIN_0 #MAX_1 amount of inhibition to apply for stripes with pfc activity below go_pfc_thr -- net inputs are rescaled downward by (1 - no_pfc_inhib) -- reshapes the competition so other stripes will win

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(MatrixGoNogoGainSpec);

class E_API MatrixGoNogoGainSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc
INHERITED(SpecMemberBase)
public:
  bool		on;		// #DEF_false enable the application of these go and nogo gain factors
  float		go_p;		// #CONDSHOW_ON_on #DEF_1 +DA gain for go neurons
  float		go_n;		// #CONDSHOW_ON_on #DEF_1 -DA gain for go neurons
  float		nogo_p;		// #CONDSHOW_ON_on #DEF_1 +DA gain for nogo neurons
  float		nogo_n;		// #CONDSHOW_ON_on #DEF_1 -DA gain for nogo neurons

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixGoNogoGainSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(MatrixLayerSpec);

class E_API MatrixLayerSpec : public LeabraLayerSpec {
  // basal ganglia matrix layer -- one for Go pathway and another for NoGo pathway -- Go recv marker con from NoGo, both recv from SNrThal to get final go signal
INHERITED(LeabraLayerSpec)
public:
  enum GoNoGo {
    GO,				// Go or direct pathway
    NOGO,			// NoGo or indirect pathway
  };

  GoNoGo		go_nogo; 	// is this a Go pathway or a NoGo pathway layer
  SNrThalLayerSpec::GatingTypes		gating_type;	// type of gating units present within this Matrix layer -- must be just one of the options (INPUT, IN_MNT, OUTPUT, etc.)
  MatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  MatrixGoNogoGainSpec  go_nogo_gain;	// separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc

  virtual LeabraLayer* 	SNrThalLayer(LeabraLayer* lay);
  // find the SNrThal layer that this matrix layer interacts with
  virtual LeabraLayer* 	PVLVDaLayer(LeabraLayer* lay);
  // find the PVLVDaLayerSpec layer that this matrix layer interacts with
  virtual LeabraLayer*  SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
					int& n_in, int& n_in_mnt, int& n_mnt_out, int& n_out, int& n_out_mnt);
  // get the starting index for this set of matrix stripes within the snrthal gating layer -- returns the snrthal layer and starting index

  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  virtual void Compute_MidMinusAct_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       LeabraNetwork* net);
  // save the effective mid-minus (gating) activation state for subsequent learning -- for specific unit group (stripe)
  virtual void Compute_ZeroMidMinusAct_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       LeabraNetwork* net);
  // set the effective mid-minus (gating) activation state to zero for subsequent learning -- for specific unit group (stripe) -- for stripes that did not gate at all this time around (enforces strong credit assignment, for display purposes mostly as dopamine usually does the credit assignment)

  virtual float	Compute_NoGoInhibGo_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
					int gpidx, LeabraNetwork* net);
  // compute nogo layer stripes inhibition onto go layer units -- called in ApplyInhib
  virtual float	Compute_GoPfcThrInhib_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
					  int gpidx, LeabraNetwork* net);
  // compute inhib from go_pfc_thr
  virtual float	Compute_PVrInhib_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
					 int gpidx, LeabraNetwork* net);
  // compute pvr inhib
  virtual float	Compute_RefractInhib_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
					 int gpidx, LeabraNetwork* net);
  // compute refract inhib

  virtual void ModulateNoGoPrjns(LeabraLayer* lay, LeabraNetwork* net);
  // modulate effective weight scale for nogo projections, after gating

  // this is hook for modulating netinput according to above inhib factors
  override void	Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net);

  virtual void	Compute_GatingActs_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
				       int gpidx, LeabraNetwork* net);
  // save activations into act_mid at point of gating

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  virtual void 	Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net);
  // compute u->dav learning dopamine value based on raw dav and gating state, etc -- this dav is then directly used in conspec learning rule

  virtual  void NameMatrixUnits(LeabraLayer* lay, LeabraNetwork* net);
  // name the matrix units according to their functional role -- i = input, m = maint, o = output -- these names are used to support different learning rules for these different types

  override void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return true; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net)
  {return false; }

  override TypeDef* 	UnGpDataType()  { return &TA_PBWMUnGpData; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(MatrixLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MatrixLayerSpec_h
