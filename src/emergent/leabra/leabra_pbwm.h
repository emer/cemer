// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_pvlv.h"

#ifndef leabra_pbwm_h
#define leabra_pbwm_h

// prefrontal-cortex basal ganglia working memory (PBWM) extensions to leabra

// based on the pvlv reinforcement learning mechanism
// this file defines BG + PFC gating/active maintenance mechanisms

//////////////////////////////////////////////////////////////////
// 	BG-based PFC Gating/RL learning Mechanism		//
//////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//	Patch/Striosomes and SNc

class LEABRA_API PatchLayerSpec : public LVeLayerSpec {
  // Patch version of the LVe layer -- functionally identical to LVe and just so-named so that other layers can use its functionality appropriately
INHERITED(LVeLayerSpec)
public:
  virtual void	Send_LVeToMatrix(LeabraLayer* lay, LeabraNetwork* net);
  // send the LVe value computed by the patch units to misc_1 field in any MarkerCons prjn to MatrixLayerSpec layers -- used for noise modulation

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  override TypeDef* 	UnGpDataType()  { return &TA_PBWMUnGpData; }

  TA_SIMPLE_BASEFUNS(PatchLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API SNcMiscSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc parameters for SNc layer spec
INHERITED(SpecMemberBase)
public:
  float		stripe_lv_pct;	// #MIN_0 #MAX_1 #DEF_0.5 proportion of total LV dopamine value determined by stripe-wise LV signals from the patch layer -- remainder is from global LV signal
  float		global_lv_pct;	// #READ_ONLY #SHOW 1 - stripe_lv_pct -- proportion of total LV dopamine value determined by global LV signals

  TA_SIMPLE_BASEFUNS(SNcMiscSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API SNcLayerSpec : public PVLVDaLayerSpec {
  // implements a substantia-nigra pars compacta (SNc) version of the PVLVDaLayerSpec, which receives stripe-wise LVe inputs from a PatchLayerSpec layer
INHERITED(PVLVDaLayerSpec)
public:
  SNcMiscSpec	snc;		// misc params for SNc layer

  override void	Compute_Da(LeabraLayer* lay, LeabraNetwork* net);
  override void	Send_Da(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  override TypeDef* 	UnGpDataType()  { return &TA_PBWMUnGpData; }

  TA_SIMPLE_BASEFUNS(SNcLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

//////////////////////////////////////////////////////////////////
//	  SNrThalLayer: Integrate Matrix and compute Gating 	//
//////////////////////////////////////////////////////////////////

class LEABRA_API SNrThalMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the snrthal layer
INHERITED(SpecMemberBase)
public:
  float		net_off;	// #DEF_0 #MIN_-1 #MAX_1 netinput offset -- how much to add to each unit's baseline netinput -- positive values make it more likely that some stripe will always fire, even if it has a net nogo activation state in the matrix -- very useful for preventing all nogo situations -- if net_off is .2 then act.gain should be 600, if net_off is 0 then act.gain should be 20 (dynamic range is diff)
  float		go_thr;		// #DEF_0.5 #MIN_0 #MAX_1 threshold in snrthal activation required to trigger a Go gating event
  float		rnd_go_inc;	// #DEF_0.1:0.2 #MIN_0 how much to add to the net input for a random-go signal triggered in corresponding matrix layer?
  float		leak;		// #DEF_1 #MIN_0 a leak-like term for the netinput computation -- just a constant added to the denominator in computing net input: (go - nogo) / (go + nogo + leak)
  bool		act_is_gate;	// #DEF_true activation state is always equal to the value that was active at the time of actual gating (act_m2) -- this makes things clearer and can be important for some models that use snrthal activations as inputs to other processes

  TA_SIMPLE_BASEFUNS(SNrThalMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API SNrThalLayerSpec : public LeabraLayerSpec {
  // Represents the substantial nigra pars reticulata (SNr) and Thalamus (MD) circuits that project from basal ganglia up to frontal cortex: computes activation based on netinput = (Go - NoGo) / (Go + NoGo + snrthal.leak) from MatrixLayerSpec -- if Go is sufficiently more active, a gating event is triggered in PFC
INHERITED(LeabraLayerSpec)
public:
  // note: following enum type must be sync'd with MatrixLayerSpec, and value is copied from input matrix layer
  enum 	BGType {       		// which type of basal ganglia circuit is this?
    OUTPUT,			// matrix that does output gating: controls access of frontal activations to other areas (e.g., motor output, or output of maintained PFC information)
    MAINT,			// matrix that does maintenance gating: controls toggling of maintenance of activity patterns (e.g., PFC) over time
  };

  BGType		bg_type;	// type of basal ganglia/frontal component system: both output and maintenance operate in most areas, but output gating is most relevant for motor output control, and maintenance is most relevant for working-memory updating (copied from input Matrix Layer)
  SNrThalMiscSpec	snrthal; // misc specs for snrthal layer

  virtual void	Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net);
  // compute netinput as GO - NOGO on matrix layer
  // hook for new netin goes here:
  override void Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net);

  virtual void	Compute_GatedActs(LeabraLayer* lay, LeabraNetwork* net);
  // compute act_eq reflecting the mutex on maint vs. output gating -- if the other has gated, then we turn our act_eq off to reflect that
  // hook for gated acts goes here:
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  virtual void 	Compute_MidMinusAct_ugp(LeabraLayer* lay,
					Layer::AccessMode acc_md, int gpidx,
					LeabraNetwork* net);
  // computes own mid minus (gating activation) and also calls same function on associated Matrix layer
  virtual void 	SendGateStates(LeabraLayer* lay, LeabraNetwork* net, LeabraLayer* pfc_lay);
  // send gating states from pfc layer to this and other associated layers (matrix, patch)

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  override TypeDef* 	UnGpDataType()  { return &TA_PBWMUnGpData; }

  TA_SIMPLE_BASEFUNS(SNrThalLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

////////////////////////////////////////////////////////////////////
//	  Base Specs for Algorithm-derived sub types

class LEABRA_API MatrixBaseLayerSpec : public LeabraLayerSpec {
  // #VIRT_BASE base matrix layer spec -- derive version-specific guys from this -- other code may check for this one to remain generic wrt versions
INHERITED(LeabraLayerSpec)
public:
  override TypeDef* 	UnGpDataType()  { return &TA_PBWMUnGpData; }

  TA_BASEFUNS_NOCOPY(MatrixBaseLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API PFCBaseLayerSpec : public LeabraLayerSpec {
  // #VIRT_BASE base pfc layer spec -- derive version-specific guys from this -- other code may check for this one to remain generic wrt versions
INHERITED(LeabraLayerSpec)
public:
  override TypeDef* 	UnGpDataType()  { return &TA_PBWMUnGpData; }

  TA_BASEFUNS_NOCOPY(PFCBaseLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

////////////////////////////////////////////////////////////////////
//	  Matrix Con/Units

class MatrixCon : public LeabraCon {
  // connection for recv cons into matrix units -- stores the sender activation prior to any gating update signals, used for learning
public:
  float		sact_lrn;	// #NO_SAVE sending activation value used for learning -- prior to any gating-based update in activation states

  MatrixCon() { sact_lrn = 0.0f; }
};

class LEABRA_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on dopamine modulation of activation
INHERITED(LeabraConSpec)
public:

  inline override void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
    // do NOT do this under any circumstances!!
  }

  inline void Compute_MidMinusAct(LeabraRecvCons* cg, LeabraUnit* ru) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      MatrixCon* cn = (MatrixCon*)cg->PtrCn(i);
      cn->sact_lrn = su->act_eq;
    }
  }
  // RECV-based save current sender activation states to sravg_m for subsequent learning -- call this at time of gating..

  inline void C_Compute_dWt_Matrix(LeabraCon* cn, float lin_wt, 
				   float mtx_act_m2, float mtx_da, float su_act_lrn, 
				   float ru_thr) {
    float sr_prod = mtx_act_m2 * su_act_lrn;
    float dwt = mtx_da * sr_prod;
    if(lmix.err_sb) {
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      MatrixCon* cn = (MatrixCon*)cg->OwnCn(i);
      C_Compute_dWt_Matrix(cn, LinFmSigWt(cn->wt), ru->act_m2, ru->dav, cn->sact_lrn,
			   ru->avg_l);
      // note: using cn->sact_lrn as having saved sending activation in Compute_MidMinusAct
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  inline override void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    // just run chl version through-and-through
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }
  inline override void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    // just run chl version through-and-through
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((LeabraCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  TA_SIMPLE_BASEFUNS(MatrixConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

class LEABRA_API MatrixBiasSpec : public LeabraBiasSpec {
  // Matrix bias spec: special learning parameters for matrix units
INHERITED(LeabraBiasSpec)
public:

  inline override void B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
    float err = ru->act_m2 * ru->dav;
    if(fabsf(err) >= dwt_thresh)
      cn->dwt += cur_lrate * err;
  }

  inline override void B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru,
						  LeabraLayer* rlay) {
    B_Compute_dWt_LeabraCHL(cn, ru);
  }

  inline override void B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru,
						 LeabraLayer* rlay) {
    B_Compute_dWt_LeabraCHL(cn, ru);
  }
  
  TA_BASEFUNS(MatrixBiasSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

class LEABRA_API MatrixNoiseSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra unit noise specs for matrix units
INHERITED(SpecMemberBase)
public:
  bool  patch_noise;		// get noise value from patch units (overrides netin_adapt setting if set to true) -- must have a patch layer spec prjn with marker con specs

  TA_SIMPLE_BASEFUNS(MatrixNoiseSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API MatrixUnitSpec : public LeabraUnitSpec {
  // basal ganglia matrix units: fire actions or WM updates. modulated by da signals
INHERITED(LeabraUnitSpec)
public:
  MatrixNoiseSpec matrix_noise;	// special noise parameters for matrix units

  override float Compute_Noise(LeabraUnit* u, LeabraNetwork* net);

  virtual void Compute_MidMinusAct(LeabraUnit* u, LeabraNetwork* net);
  // save the effective mid-minus (gating) activation state for subsequent learning

  void	InitLinks();
  SIMPLE_COPY(MatrixUnitSpec);
  TA_BASEFUNS(MatrixUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

//////////////////////////////////
//	  Matrix Layer Spec	//
//////////////////////////////////

class LEABRA_API MatrixGateBiasSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating biases depending on different conditions in the network -- helps to get the network performing appropriately for basic maintenance functions, to then be reinforced or overridden by learning
INHERITED(SpecMemberBase)
public:
  float		out_rew_go;	// #DEF_1 (Weak) #AKA_out_pvr (NOTE: can be superceded by out_rew_go_fun) for OUTPUT stripes with active maintenance on reward trials (e.g., recall/output trials -- signalled by PVr), amount Go bias (favors Go over NoGo) to encourage the output gating units to respond -- see out_empty_nogo for empty (non-maintaining) stripes
  float		out_norew_nogo;	// #DEF_1:2 (Strong) for all OUTPUT stripes (empty or maintaining) on non-reward trials (i.e., store, not recall trials -- signalled by PVr), amount of NoGo bias (favors NoGo over Go) to discourage output gating -- is not generally useful to output gate on store trials
  float		out_empty_nogo;	// #DEF_5 (Very Strong) for OUTPUT stripes that are not maintaining anything, on reward trials (e.g., recall/output trials -- signalled by PVr), amount of NoGo bias (favors NoGo over Go) to discourage output gating if there is nothing being maintained to output gate
  float		mnt_rew_nogo;	// #DEF_5 (Very Strong) for all MAINT stripes (empty or maintaining) on reward trials (e.g., recall/output trials -- signalled by PVr), amount of NoGo bias (favors NoGo over Go) -- adds to any mnt_mnt_nogo -- in general it is not useful to fire maint gating on recall/output trials (see pfc.gate spec -- can prevent entirely with flag there)
  float		mnt_mnt_nogo;	// #DEF_0 #AKA_mnt_nogo (NOTE: can be superceded by mnt_mnt_nogo_fun) for MAINT stripes that are maintaining on non-reward trials (i.e., store, not recall trials -- signalled by PVr), amount of NoGo bias (favors NoGo over Go) -- although this is useful for maintenance, it can get in the way of replacing outdated information, and so overall a null bias of 0 seems best
  float		mnt_empty_go;	// #DEF_0 (NOTE: can be superceded by mnt_empty_go_fun) for empty MAINT stripes on non-reward trials (i.e., store, not recall trials -- signalled by PVr), amount of Go bias (favors Go over NoGo) -- provides a bias for encoding and maintaining new information -- keeping this at 0 allows system to be "unbaised" in its selection of what to gate in, which appears to be useful in general

  TA_SIMPLE_BASEFUNS(MatrixGateBiasSpec);
protected:
  SPEC_DEFAULTS;
//   void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API MatrixGateBiasFunSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra for matrix gating biases that change over time as a function of time-based gating variable (e.g., duration of maintenance or emptiness -- value depends on bias in question)
INHERITED(SpecMemberBase)
public:
  enum FunType {
    LIN,			// linear function of time variable -- add incr for each unit of time
    EXP,			// exponential function of time variable -- multiply by incr for each unit of time
  };

  bool		on;		// use this function-based bias value, instead of the original static bias value shown in gate_bias
  FunType	fun;		// #CONDSHOW_ON_on what function of time to use -- linear vs. exponential
  int		off;		// #CONDSHOW_ON_on time offset (in trials) before function is computed -- value remains at its starting (min or max depending) during this initial offset, and then starts moving after that
  int		interval;	// #CONDSHOW_ON_on #MIN_1 time interval between start and end over which the bias value changes -- how long does it take to go from start to end?  must be at least 1 trial
  float		start;		// #CONDSHOW_ON_on starting value of the bias (for initial values of time)
  float		end;		// #CONDSHOW_ON_on ending value of the bias (puts a limit on how far incr can go)
  float		incr;		// #CONDSHOW_ON_on #READ_ONLY #SHOW computed from interval and start/end values -- how much bias increments per unit time in relevant variable (see fun for how this is computed)

  float		GetBiasLin(int time) {
    if(time <= off) return start;
    if(time-off > interval) return end;
    return (start + ((float)(time-off) * incr));
  }

  float		GetBiasExp(int time) {
    if(time <= off) return start;
    if(time-off > interval) return end;
    return (start + expf((float)(time-off) * incr));
  }

  float		GetBias(int time) {
    if(fun == LIN) return GetBiasLin(time);
    return GetBiasExp(time);
  }

  TA_SIMPLE_BASEFUNS(MatrixGateBiasFunSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API MatrixMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the matrix layer
INHERITED(SpecMemberBase)
public:
  float		da_gain;	// #DEF_0:2 #MIN_0 overall gain for da modulation of matrix units for the purposes of learning (ONLY) -- bias da is set directly by gate_bias params -- also, this value is in addition to other "upstream" gain parameters, such as vta.da.gain -- it is recommended that you leave those upstream parameters at 1.0 and adjust this parameter, as it also modulates rnd_go.nogo.da which is appropriate
  float		bias_gain;	// #DEF_0.1 overall gain factor for the gating biases as they are translated into multipliers on the net input values of Go vs. NoGo units -- allows the bias values to be expressed in standardized relative units and then overall impact can be dialed with this setting
  float		nv_gain;	// gain on the NV (novelty value) contribution to performance bias values -- increased Go bias with increased NV -- this is completely independent of the NV layer spec gain parameter, which enters into the learning da value -- can separately include nv as a bias and learning factor
  float		bias_pos_gain;	// #DEF_0 extra multiplicative gain for positive bias terms -- it is in general not great to increase netinput levels on units beyond their natural values, so setting this to zero (default) puts all the bias work on decreasing the relative netinputs for the non-favored population (biologically can be going into a down state)
  bool		mnt_only;	// set to true if there is only a MAINT matrix layer -- affects the way that rnd go is computed -- should also have PFCLayerSpec.gate.max_maint = 0

  TA_SIMPLE_BASEFUNS(MatrixMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };
};

class LEABRA_API MatrixTonicDaSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra how to adapt tonic dopamine in response to errors and overall nogo firing -- increases in tonic da facilitate Go firing globally
INHERITED(SpecMemberBase)
public:
  float		out_err_nogo_inc; // for OUTPUT stripes: how much to increase tonic da when an error occurs on a reward trial, and all of the stripes fired nogo
  float		mnt_nogo_inc;	// for MAINT stripes: how much to increase tonic da on norew trials and all of the stripes fired nogo
  float		out_err_go_inc;	// #DEF_0 for OUTPUT stripes: how much to increase tonic da when an error occurs on a reward trial, and at least one of the stripes has fired go 
  float		decay;		// rate of decay in tonic da per primary value feedback trial, in the absence of increases per above parameters
  float		max_da;		// maximum tonic da value
  int		nogo_thr;	// #DEF_20 per-stripe threshold of number of trials of nogo firing in a row, above which a stripe-specific tonic dopamine level will start to increase, promoting go firing in that stripe and keeping it in the game.  for maintenance gating, either maint or empty nogo counts.  for output gating, only maint nogo counts
  float		nogo_thr_inc; 	// how much to increase the stripe-specific tonic da per trial when nogo_thr has been exceeded in a given stripe

  bool		old_rnd_go;	// #DEF_false use old nogo mechanism -- deprecated
  bool		rng_eq_thr;	// #DEF_true #CONDSHOW_ON_old_rnd_go set the nogo_rng value to be the same as nogo_thr -- this generally makes sense and is characteristic of the Poisson distribution, and reduces the number of parameters to confront..
  int		nogo_rng;	// #CONDSHOW_ON_old_rnd_go #DEF_20 #MIN_1 range of trials with nogo firing beyond nogo_thr to allow before engaging random go firing -- sets a new effective threshold after each nogo random go as nogo_thr + Random::IntZeroN(nogo_rng)
  float		nogo_da;	// #DEF_10 #CONDSHOW_ON_old_rnd_go #MIN_0 strength of DA for driving learning of random Go units -- does not affect performance, only learning
  float		nogo_noise;	// #DEF_0;0.02 #CONDSHOW_ON_old_rnd_go #MIN_0 use .02 when using -- noise value to apply to a randomly selected subset of k Go units to get them activated during a random Go event

  TA_SIMPLE_BASEFUNS(MatrixTonicDaSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API MatrixGoNogoGainSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc
INHERITED(SpecMemberBase)
public:
  bool		on;		// #DEF_false enable the application of these go and nogo gain factors
  float		go_p;		// #CONDSHOW_ON_on #DEF_1 +DA gain for go neurons
  float		go_n;		// #CONDSHOW_ON_on #DEF_1 -DA gain for go neurons
  float		nogo_p;		// #CONDSHOW_ON_on #DEF_1 +DA gain for nogo neurons
  float		nogo_n;		// #CONDSHOW_ON_on #DEF_1 -DA gain for nogo neurons

  TA_SIMPLE_BASEFUNS(MatrixGoNogoGainSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };
};

// matrix unit/layer misc_ var docs
// * misc_1 = patch LVe value (send by PatchLayerSpec, for patch_noise in Matrix Units)

class LEABRA_API MatrixLayerSpec : public MatrixBaseLayerSpec {
  // basal ganglia matrix layer: initiate actions (OUTPUT gating) or working memory updating (MAINT gating) -- gating signal computed at mid minus point
INHERITED(MatrixBaseLayerSpec)
public:
  enum 	BGType {       		// which type of basal ganglia circuit is this?
    OUTPUT,			// matrix that does output gating: controls access of frontal activations to other areas (e.g., motor output, or output of maintained PFC information)
    MAINT,			// matrix that does maintenance gating: controls toggling of maintenance of activity patterns (e.g., PFC) over time
  };

  BGType		bg_type;	// type of basal ganglia/frontal component system: both output and maintenance operate in most areas, but output gating is most relevant for motor output control, and maintenance is most relevant for working-memory updating
  MatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  MatrixGateBiasSpec 	gate_bias;	// gating biases depending on different conditions in the network -- helps to get the network performing appropriately for basic maintenance functions, to then be reinforced or overridden by learning
  MatrixGateBiasFunSpec	out_rew_go_fun; // gating bias function for OUTPUT stripes with active maintenance on reward trials (e.g., recall/output trials -- signalled by PVr), amount Go bias (favors Go over NoGo) to encourage the output gating units to respond -- is (typically increasing) function of duration information has been maintained
  MatrixGateBiasFunSpec	mnt_mnt_nogo_fun; // gating bias function for MAINT stripes that are maintaining on non-reward trials (i.e., store, not recall trials -- signalled by PVr), amount of NoGo bias (favors NoGo over Go) -- is (typically decreasing) function of maintenance duration -- if starts high ends low, this causes stripe to try to maintain (NoGo) strongly initially, and then be more labile for updating after that
  MatrixGateBiasFunSpec	mnt_empty_go_fun; // gating bias function for empty MAINT stripes on non-reward trials (i.e., store, not recall trials -- signalled by PVr), amount of Go bias (favors Go over NoGo) -- provides a bias for encoding and maintaining new information -- is (typically increasing) function of time being empty, causing stripe to be more likely to maintain the longer it sits empty -- thus serves as a more graded, subtle version of rnd_go.
  MatrixTonicDaSpec	tonic_da;	// #AKA_rnd_go how to adapt tonic dopamine in response to errors and overall nogo firing -- increases in tonic da facilitate Go firing globally
  MatrixGoNogoGainSpec  go_nogo_gain;	// separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc

  override void Compute_NetinStats_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				       LeabraInhib* thr, LeabraNetwork* net);

  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  virtual void Compute_MidMinusAct_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       LeabraNetwork* net);
  // save the effective mid-minus (gating) activation state for subsequent learning -- for specific unit group (stripe)
  virtual float	Compute_BiasDaMod(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				  LeabraNetwork* net);
  // compute gate_bias da modulation to influence gating -- continuously throughout settling
  virtual void	Compute_MultBias(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				 LeabraNetwork* net, float bias_dav);
  // apply multiplicative bias to netinputs of units
  virtual void 	Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net);
  // compute u->dav learning dopamine value based on raw dav and gating state, etc -- this dav is then directly used in conspec learning rule
  virtual void 	Compute_NGo(LeabraLayer* lay, LeabraNetwork* net,
			    int& n_mnt_go, int& n_out_go);
  // compute number of maintenance and output go firing stripes
  virtual void 	Compute_TonicDa(LeabraLayer* lay, LeabraNetwork* net);
  // update tonic da value based on tonic_da parameters (errors, all nogo, etc)

  virtual void Compute_RndGoNoise_ugp(LeabraLayer* lay,
				      Layer::AccessMode acc_md, int gpidx,
				      LeabraNetwork* net);
  // compute random-go noise for k randomly selected Go units -- only does this if rnd_go.nogo_noise > 0
  virtual void 	Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // clear the rnd go signals at start of trial and compute new rnd go threshold if applicable

  virtual void	LabelUnits_impl(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx);
  // label units with Go/No (unit group) -- auto done in InitWeights
  virtual void	LabelUnits(LeabraLayer* lay);
  // label units with Go/No -- auto done in InitWeights

  override void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return true; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {return false; }

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

//////////////////////////////////////////
//	PFC Layer Spec (Maintenance)	//
//////////////////////////////////////////

// misc_state docs:
// * pfc ugp->misc_state = 

class LEABRA_API PFCGateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(SpecMemberBase)
public:
  enum	GateSignal {
    GATE_GO = 0,		// gate Go unit fired
    GATE_NOGO = 1,		// gate NoGo unit fired
    GATE_MNT_GO = 2,		// gate maintenance Go unit fired 
    GATE_OUT_GO = 3,		// gate output Go unit fired 
    GATE_OUT_MNT_GO = 4,	// gate maint and output Go units fired (not currently possible with mutex logic)
  };

  enum	GateState {		// what happened on last gating action, stored in gate_state on unit group -- for debugging etc
    EMPTY_MNT_GO = 0,		// stripe was empty, got MAINT Go
    EMPTY_OUT_GO = 1,		// stripe was empty, got OUTPUT Go
    EMPTY_OUT_MNT_GO = 2,	// stripe was empty, got OUTPUT then MAINT Go (not currently possible with mutex logic)
    EMPTY_NOGO = 3,		// stripe was empty, got NoGo
    MAINT_MNT_GO = 4,		// stripe was already maintaining, got MAINT Go: cleared, encoded
    MAINT_OUT_GO = 5,		// stripe was already maintaining, got OUTPUT Go
    MAINT_OUT_MNT_GO = 6,	// stripe was already maintaining, got OUTPUT then MAINT Go (not currently possible with mutex logic)
    MAINT_NOGO = 7,		// stripe was already maintaining, got NoGo
    NOGO_RND_GO = 20,		// random go for stripes constantly firing nogo
    INIT_STATE = 30,		// initialized state at start of trial
  };

  bool		graded_out_go;	// #DEF_true use actual activation level of output Go signal to drive output activation level
  float		clear_decay;	// #DEF_0.9 #MIN_0 #MAX_1 how much to decay the activation state for units in the stripe when the maintenance is cleared -- simulates a phasic inhibitory burst (GABA-B?) from the gating pulse
  float		go_learn_base;	// #DEF_0.05 #MIN_0 #MAX_1 how much PFC learning occurs in the absence of go gating modulation -- 1 minus this is how much happens with go gating -- determines how far plus phase activations used in learning can deviate from minus-phase activation state: plus phase act_nd = act_m + (go_learn_base + (1-go_learn_base) * gate_act) * (act - act_m)
  float		go_learn_mod;	// #READ_ONLY 1 - go_learn_base -- how much learning is actually modulated by go gating activation
  int		mid_minus_min;	// minimum number of cycles before computing any gating -- acts like an STN-like function -- must be < network mid_minus_cycle
  int		max_maint;	// a hard upper-limit on how long the PFC can maintain -- anything over this limit will be cleared.  set to 0 for motor areas that do not maintain but use maintenance gating to scope the set of possible responses
  bool		out_go_clear;	// #DEF_true an output Go clears the maintenance currents at the end of the trial -- only for reward trials (signalled by PVr) -- you use it, you lose it..
  bool		mnt_toggle;	// #DEF_true does maint Go do toggling or just re-updating?
  bool		mnt_wins;	// #DEF_false maint gating always wins out over output gating -- even if output gating has already taken place, maintenance gating will always 
  bool		updt_gch;	// #DEF_false always update gc.h for maintaining PFC units at the end of each trial, regardless of whether any gating signals ocurred -- reflects biology where maintenance currents are activity driven with a time constant, and thus always reflect current activity (with some delay -- trial-level updating achieves that)
  float		off_accom;	// #DEF_0 #EXPERT #MIN_0 #MAX_1 how much of the maintenance current to apply to accommodation after turning a unit off

  TA_SIMPLE_BASEFUNS(PFCGateSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};


class LEABRA_API PBWMUnGpData : public LeabraUnGpData {
  // PBWM version of data to maintain for independent unit groups of competing units within a single layer -- contains extra information for PBWM state
INHERITED(LeabraUnGpData)
public:
  int		mnt_count;	// #CAT_Activation counter of number of trials in maint or empty state -- 0 = just cleared (empty) -- 1+ = maint for a trial or a more -- -1- = empty for a trial or more (was misc_state)
  PFCGateSpec::GateState	gate_state;	// #CAT_Activation what happened on last gating action in terms of PFC state and gating signal action (was misc_state1)
  PFCGateSpec::GateSignal	gate_sig;	// #CAT_Activation what gating signal(s) happened on most recent trial (was misc_state2)
  float		cur_go_act;	// #CAT_Activation current Go activation value (mnt or out) -- used by units to boost netin -- already multiplied by gate.mnt/out_go_netin (was misc_float)
  float		out_go_act;	// #CAT_Activation current output gating Go activation value for graded Go -- just multiply directly by this number (was misc_float1)
  int		rnd_go_thr;	// #CAT_Activation current random go threshold for this stripe -- thresholds have a random interval element within bounds
  float		tonic_da;	// #CAT_Activation stripe-specific tonic dopamine level

  override void	Init_State();

  void	Copy_(const PBWMUnGpData& cp);
  TA_BASEFUNS(PBWMUnGpData);
private:
  void	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API PFCLayerSpec : public PFCBaseLayerSpec {
  // Prefrontal cortex layer: gets maintenance and output gating signals from SNrThal -- gate toggles off in minus phase, on at end of plus phase, output gating drives unit act relative to internal act_eq value
INHERITED(PFCBaseLayerSpec)
public:
  enum MaintUpdtAct {
    STORE,			// store current activity state in maintenance currents
    CLEAR,			// clear current activity state from maintenance currents
    UPDT,			// update existing gc.h values based on current activations
  };

  PFCGateSpec	gate;		// parameters controlling the gating of pfc units

  virtual void	GetSNrThalLayers(LeabraLayer* lay, LeabraLayer*& snrthal_mnt, LeabraLayer*& snrthal_out);
  // find maintenance (required) and output (optional) snrthal input layers

  virtual void Clear_Maint(LeabraLayer* lay, LeabraNetwork* net, int stripe_no=-1);
  // clear maintenance currents on given stripe or all stripes if stripe_no < 0 -- for program-based control over clearing
  virtual void Compute_MaintUpdt(LeabraLayer* lay, LeabraNetwork* net,
				 MaintUpdtAct updt_act, int stripe_no=-1);
  // perform given action (STORE, CLEAR, UPDT) on given stripe or all stripes if stripe_no < 0 -- for program-based control over pfc functionality
  virtual void 	Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net);
  // clear various gating signals at the start of the trial
    virtual void Compute_MaintUpdt_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       MaintUpdtAct updt_act, LeabraNetwork* net);
    // update maintenance state variables (gc.h, misc_1) based on given action: ugp impl
  virtual void 	Compute_MidMinusAct_ugp(LeabraLayer* lay,
					Layer::AccessMode acc_md, int gpidx,
					LeabraNetwork* net);
  // computes mid minus (gating activation) state prior to gating
  virtual void	SendGateStates(LeabraLayer* lay, LeabraNetwork* net);
  // send gating state variables to the snrthal and matrix layers
  virtual void 	Compute_Gating(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer activations -- each cycle during first minus phase
  virtual void 	Compute_Gating_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer activations -- at mid minus point
  virtual void 	Compute_Gating_Final(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer activations -- at end of plus phase
  virtual void	Compute_PfcMntAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute PFC maint layer activations -- add learn modulation


  override void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(PFCLayerSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Defaults_init()		{ };
};

//////////////////////////////////////////
//	PFC Layer Spec (Output)		//
//////////////////////////////////////////

class LEABRA_API PFCOutLayerSpec : public PFCBaseLayerSpec {
  // Prefrontal cortex output gated layer: gets gating signal and activations from PFC_mnt layer: gating modulates strength of activation
INHERITED(PFCBaseLayerSpec)
public:
  virtual void	Compute_PfcOutAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute PFC output layer activations -- replaces std act fun -- called in Compute_CycleStats -- just overwrites whatever the regular funs compute

  override void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(PFCOutLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Defaults_init() 	{ };
};


//////////////////////////////////////////
//	    Special PrjnSpecs	 	//
//////////////////////////////////////////

class LEABRA_API PFCLVPrjnSpec : public FullPrjnSpec {
  // A special projection spec for PFC to LVe/i layers. If n unit groups (stripes) in LV == PFC, then it makes Gp one-to-one projections; if LV stripes == 1, it makes a single full projection; if LV stripes == PFC + 1, the first projection is full and the subsequent are gp one-to-one; if recv fm multiple PFC layers, same logic applies to each
INHERITED(FullPrjnSpec)
public:

  virtual void Connect_Gp(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			  Layer::AccessMode sacc_md, int sgpidx);
  // make a projection from all senders in sugp into all receivers in rugp

  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(PFCLVPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};


class EMERGENT_API MatrixRndPrjnSpec : public ProjectionSpec {
  // For projections into Matrix layers (Maintenance and Output) -- uses a shared set of random sending projections *per recv group* (stripe), such that all units in the stripe (both maint and output) recv from the same random subset of sending units -- same prjn spec MUST be shared between maint and out gating matrix layers, but must NOT be shared between different Matrix layers (e.g., for other PFC areas within the same model)
INHERITED(ProjectionSpec)
public:
  float		p_con;		// overall probability of connection
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed
  taBase_List	send_idx_ars;	// #HIDDEN #NO_SAVE list of sending index arrays (int_Array), one per recv unit group

  void 	Connect_impl(Projection* prjn);
  // Connection function for full connectivity

  TA_SIMPLE_BASEFUNS(MatrixRndPrjnSpec);
protected:
  SPEC_DEFAULTS;
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API MatrixGradRFPrjnSpec : public GradientWtsPrjnSpec {
  // For projections into Matrix layers -- establishes a gradient of weight strengths into the matrix units to help different stripes specialize on encoding different things in the input -- MUST have init_wts on for it to do anything (also recommend add_rnd_wts) -- this is a much softer constraint than the MatrixRndPrjnSpec where connections either exist or not
INHERITED(GradientWtsPrjnSpec)
public:
  bool		invert_nogo;	// NoGo units get inverted weight pattern -- produces a net Go bias for sending units in corresponding topological location with associated stripes
 float		nogo_offset;	// how much to add to NoGo weights (can be + or - -- e.g., subtracting a fixed amount from all weights produces an overall Go Bias)

  override void SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist, int cg_idx);

  TA_SIMPLE_BASEFUNS(MatrixGradRFPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};


#endif // leabra_pbwm_h
