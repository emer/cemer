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
  bool	learn_mnt_only;		// only learn for stripes that are currently maintaining!

  virtual void	Send_LVeToMatrix(LeabraLayer* lay, LeabraNetwork* net);
  // send the LVe value computed by the patch units to misc_1 field in any MarkerCons prjn to MatrixLayerSpec layers -- used for noise modulation

  override void Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(PatchLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API SNcMiscSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc parameters for SNc layer spec
INHERITED(taOBase)
public:
  float		stripe_lv_pct;	// #MIN_0 #MAX_1 #DEF_0.5 proportion of total LV dopamine value determined by stripe-wise LV signals from the patch layer -- remainder is from global LV signal
  float		global_lv_pct;	// #READ_ONLY #SHOW 1 - stripe_lv_pct -- proportion of total LV dopamine value determined by global LV signals
  bool		lv_mnt_pv_out;	// #DEF_false send LV-based da signals to maintenance matrix layers, and PV-based da to output matrix layers

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(SNcMiscSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
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

  TA_SIMPLE_BASEFUNS(SNcLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////////////////
//	  Matrix Con/Units

class LEABRA_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on dopamine modulation of activation
INHERITED(LeabraConSpec)
public:
#ifdef __MAKETA__
  XCalLearnSpec	xcal;		// #CAT_Learning XCAL learning parameters for matrix cons, used for keeping units from being either too active or not active enough -- note that mvl_mix default value should be multiplied by da_gain
#endif
  inline void C_Compute_dWt_Matrix(LeabraCon* cn, float lin_wt, 
				   float mtx_act_m2, float mtx_da, float su_act_m2, 
				   float ru_thr) {
    float sr_prod = mtx_act_m2 * su_act_m2;
    float err = mtx_da * sr_prod;
    float dwt = xcal.svm_mix * err + xcal.mvl_mix * xcal.dWtFun(sr_prod, ru_thr);
    // std leabra requires separate softbounding on all terms.. see XCAL for its version
    if(lmix.err_sb) {
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline void C_Compute_dWt_Matrix_NoSB(LeabraCon* cn, float mtx_act_m2, float mtx_da,
					float su_act_m2, float ru_thr) {
    float sr_prod = mtx_act_m2 * su_act_m2;
    float err = mtx_da * sr_prod;
    float dwt = xcal.svm_mix * err + xcal.mvl_mix * xcal.dWtFun(sr_prod, ru_thr);
    cn->dwt += cur_lrate * dwt;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Matrix(cn, LinFmSigWt(cn->wt), ru->act_m2, ru->dav, su->act_m2, ru->l_thr);
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Matrix_NoSB(cn, ru->act_m2, ru->dav, su->act_m2, ru->l_thr);
    }
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_CtLeabraXCAL(cg, su);
  }

  TA_SIMPLE_BASEFUNS(MatrixConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API MatrixBiasSpec : public MatrixConSpec {
  // Matrix bias spec: special learning parameters for matrix units
INHERITED(MatrixConSpec)
public:
  float		dwt_thresh;  // #DEF_0.1 #CAT_Learning don't change if dwt < thresh, prevents buildup of small changes

  inline override void B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
    float err;
    err = ru->act_m2 * ru->da;
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
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API MatrixNoiseSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra unit noise specs for matrix units
INHERITED(taOBase)
public:
  bool  patch_noise;		// get noise value from patch units (overrides netin_adapt setting if set to true) -- must have a patch layer spec prjn with marker con specs

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixNoiseSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};


class LEABRA_API MatrixUnitSpec : public LeabraUnitSpec {
  // basal ganglia matrix units: fire actions or WM updates. modulated by da signals
INHERITED(LeabraUnitSpec)
public:
  MatrixNoiseSpec matrix_noise;	// special noise parameters for matrix units

  override float Compute_Noise(LeabraUnit* u, LeabraNetwork* net);

  void	Defaults();

  void	InitLinks();
  SIMPLE_COPY(MatrixUnitSpec);
  TA_BASEFUNS(MatrixUnitSpec);
private:
  void	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////
//	  Matrix Layer Spec	//
//////////////////////////////////

class LEABRA_API MatrixRndGoSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc random go specifications (nogo)
INHERITED(taOBase)
public:
  int		nogo_thr;	// #DEF_50 threshold of number of nogo firing in a row that will trigger NoGo random go firing
  float		nogo_p;		// #DEF_0.1;0 probability of actually firing a nogo random Go once the threshold is exceeded
  float		nogo_da;	// #DEF_10 strength of DA for activating Go (gc.h) and inhibiting NoGo (gc.a) for a nogo-driven random go firing

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixRndGoSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API MatrixGateBiasSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating biases depending on different conditions in the network -- helps to get the network performing appropriately for basic maintenance functions, to then be reinforced or overridden by learning
INHERITED(taOBase)
public:
  bool		one_bias;	// #DEF_true use one value to specify the main gating biases (mnt_nogo, empty_go, out_pvr) instead of specifying each separately -- typically things work best with the same value for all biases, so this makes it simpler to set them
  float		bias; 		// #DEF_0:2 #CONDSHOW_ON_one_bias the one dopamine (da) bias value to use for mnt_nogo, empty_go, out_pvr, if one_bias flag is true 
  float		mnt_nogo;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for stripes that are maintaining, amount of NoGo bias da (negative dopamine) -- only if not on an output trial as determined by PVr -- this is critical for enabling robust maintenance 
  float		empty_go;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for empty stripes, amount of Go bias da (positive dopamine) -- only if not on an output trial as determined by PVr -- provides a bias for encoding and maintaining new information
  float		out_pvr;	// #DEF_0:2 #CONDEDIT_OFF_one_bias if PVr detects that this is trial where external rewards are typically provided, amount of OUTPUT Go bias da (positive dopamine) to encourage the output gating units to respond
  float		mnt_pvr;	// #DEF_0 if PVr detects that this is trial where external rewards are typically provided, amount of MAINT Go bias da (positive dopamine) to encourage the output gating units to respond

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixGateBiasSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API MatrixMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the matrix layer
INHERITED(taOBase)
public:
  float		da_gain;	// #MIN_0 #DEF_1 overall gain for da modulation of matrix units for the purposes of learning (ONLY) -- bias da is set directly by gate_bias params -- also, this value is in addition to other "upstream" gain parameters, such as vta.da.gain
  float		mnt_raw_empty;	// #MIN_0 #DEF_0.1 how much of the raw da signal to apply for learning in maintenance units, for case when an empty stripe is gated on, where the LV delta signal is otherwise zero, so it only can learn on the raw da value
  float		mnt_raw_updt; 	// #MIN_0 #DEF_0:0.1 how much of the raw da signal to apply for learning in maintenance units, for case when a maintaining stripe is updated to a new value -- a good LV delta value is present in this case so this is not necessary, and all indications are that it only impairs performance..
  float		neg_da_bl;	// #MIN_0 #DEF_0;0.0002 negative da baseline in learning condition: this amount subtracted from all da values in learning phase (essentially reinforces nogo)
  float		neg_gain;	// #DEF_1;0.7;1.5 gain for negative DA signals relative to positive ones
  bool		no_snr_mod;	// #DEF_false #EXPERT disable the Da learning modulation by SNrThal ativation (this is only to demonstrate how important it is)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixMiscSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API MatrixGoNogoGainSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc
INHERITED(taOBase)
public:
  bool		on;		// #DEF_false enable the application of these go and nogo gain factors
  float		go_p;		// #CONDSHOW_ON_on #DEF_1 +DA gain for go neurons
  float		go_n;		// #CONDSHOW_ON_on #DEF_1 -DA gain for go neurons
  float		nogo_p;		// #CONDSHOW_ON_on #DEF_1 +DA gain for nogo neurons
  float		nogo_n;		// #CONDSHOW_ON_on #DEF_1 -DA gain for nogo neurons

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixGoNogoGainSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

// matrix unit misc_ var docs
// * act_p2 = da value at mid minus
// * misc_2 = da value at end of minus
// * misc_1 = patch LVe value (send by PatchLayerSpec, for patch_noise in Matrix Units)

class LEABRA_API MatrixLayerSpec : public LeabraLayerSpec {
  // basal ganglia matrix layer: initiate actions (OUTPUT gating) or working memory updating (MAINT gating) -- gating signal computed at mid minus point
INHERITED(LeabraLayerSpec)
public:
  enum 	BGType {       		// which type of basal ganglia circuit is this?
    OUTPUT,			// matrix that does output gating: controls access of frontal activations to other areas (e.g., motor output, or output of maintained PFC information)
    MAINT,			// matrix that does maintenance gating: controls toggling of maintenance of activity patterns (e.g., PFC) over time
  };

  BGType		bg_type;	// type of basal ganglia/frontal component system: both output and maintenance operate in most areas, but output gating is most relevant for motor output control, and maintenance is most relevant for working-memory updating
  MatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  MatrixGateBiasSpec 	gate_bias;	// gating biases depending on different conditions in the network -- helps to get the network performing appropriately for basic maintenance functions, to then be reinforced or overridden by learning

  MatrixGoNogoGainSpec	go_nogo_gain;	// separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc
  MatrixRndGoSpec	rnd_go;		// matrix random Go firing for nogo firing stripes case

  virtual void 	Compute_BiasDaMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork* net);
  // compute gate_bias da modulation to influence gating -- continuously throughout settling
    virtual void Compute_UnitBiasDaMod(LeabraUnit* u, float bias_dav, int go_no);
    // apply bias da modulation to individual unit
  virtual void 	Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net);
  // compute u->dav learning dopamine value based on raw dav and gating state, etc -- this dav is then directly used in conspec leraning rule

  virtual void 	Compute_NoGoRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // compute random Go for nogo case
  virtual void 	Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // clear the rnd go signals

  virtual void	LabelUnits_impl(Unit_Group* ugp);
  // label units with Go/No (unit group) -- auto done in InitWeights
  virtual void	LabelUnits(LeabraLayer* lay);
  // label units with Go/No -- auto done in InitWeights

  override void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  //  override bool	Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return true; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(MatrixLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////////////////////////////
//	  SNrThalLayer: Integrate Matrix and compute Gating 	//
//////////////////////////////////////////////////////////////////

class LEABRA_API SNrThalMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the snrthal layer
INHERITED(taOBase)
public:
  float		net_off;	// [0.2 or 0] netinput offset -- how much to add to each unit's baseline netinput -- positive values make it more likely that some stripe will always fire, even if it has a net nogo activation state in the matrix -- very useful for preventing all nogo situations -- if net_off is .2 then act.gain should be 600, if net_off is 0 then act.gain should be 20 (dynamic range is diff)
  float		go_thr;		// #DEF_0.1 threshold in snrthal activation required to trigger a Go gating event
  float		rnd_go_inc;	// #DEF_0.2 how much to add to the net input for a random-go signal triggered in corresponding matrix layer?

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(SNrThalMiscSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API SNrThalLayerSpec : public LeabraLayerSpec {
  // Represents the substantial nigra pars reticulata (SNr) and Thalamus (MD) circuits that project from basal ganglia up to frontal cortex: computes activation based on netinput = Go - NoGo from MatrixLayerSpec -- if Go is sufficiently more active, a gating event is triggered in PFC
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

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(SNrThalLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
//	PFC Layer Spec	(Maintenance)	//
//////////////////////////////////////////

// misc_state docs:
// * pfc ugp->misc_state = counter of number of trials in maint or empty state
// ** 0 = just cleared (empty)
// ** 1+ = maint for a trial or a more
// ** -1- = empty for a trial or more
// * pfc ugp->misc_state1 = GateState -- what happened on last gating action in terms of PFC state and gating signal action
// * pfc ugp->misc_state2 = GateSignal -- what gating signal(s) happened on most recent trial
// * pfc ugp->misc_float = current Go activation value (mnt or out) -- used by units to boost netin -- already multiplied by gate.mnt/out_go_netin
// * pfc ugp->misc_float1 = current output gating Go activation value with base_gain and go_gain factored in -- used for graded Go -- just multiply directly by this number

class LEABRA_API PFCGateSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(taOBase)
public:
  enum	GateSignal {
    GATE_GO = 0,		// gate Go unit fired
    GATE_NOGO = 1,		// gate NoGo unit fired
    GATE_MNT_GO = 2,		// gate maintenance Go unit fired 
    GATE_OUT_GO = 3,		// gate output Go unit fired 
    GATE_OUT_MNT_GO = 4,	// gate maint and output Go units fired 
  };

  enum	GateState {		// what happened on last gating action, stored in misc_state1 on unit group -- for debugging etc
    EMPTY_MNT_GO = 0,		// stripe was empty, got MAINT Go
    EMPTY_OUT_GO = 1,		// stripe was empty, got OUTPUT Go
    EMPTY_OUT_MNT_GO = 2,	// stripe was empty, got OUTPUT then MAINT Go
    EMPTY_NOGO = 3,		// stripe was empty, got NoGo
    MAINT_MNT_GO = 4,		// stripe was already maintaining, got MAINT Go: cleared, encoded
    MAINT_OUT_GO = 5,		// stripe was already maintaining, got OUTPUT Go
    MAINT_OUT_MNT_GO = 6,	// stripe was already maintaining, got OUTPUT then MAINT Go
    MAINT_NOGO = 7,		// stripe was already maintaining, got NoGo
    NOGO_RND_GO = 8,		// random go for stripes constantly firing nogo
    INIT_STATE = 9,		// initialized state at start of trial
  };

  float		base_gain;	// #DEF_0;0.5 #MIN_0 #MAX_1 how much activation gets through even without a Go gating signal
  float		go_gain;	// #READ_ONLY how much extra to add for a Go signal -- automatically computed to be 1.0 - base_gain
  bool		graded_out_go;	// #DEF_true use actual activation level of output Go signal to drive output activation level
  float		go_learn_base;	// #DEF_0.06 #MIN_0 #MAX_1 how much PFC learning occurs in the absence of go gating modulation -- 1 minus this is how much happens with go gating -- determines how far plus phase activations used in learning can deviate from minus-phase activation state: plus phase act_nd = act_m + (go_learn_base + (1-go_learn_base) * gate_act) * (act - act_m)
  float		go_learn_mod;	// #READ_ONLY 1 - go_learn_base -- how much learning is actually modulated by go gating activation
  float		go_netin_gain;	  // #DEF_0.01 extra net input to add to active units as a function of gating signal -- uses the mnt_go_learn_mod to determine if maintenance go contributes to the gating signal
  float		clear_decay;	// #DEF_0 #MIN_0 #MAX_1 how much to decay the activation state for units in the stripe when the maintenance is cleared -- simulates a phasic inhibitory burst (GABA-B?) from the gating pulse
  bool		out_go_clear;	// #DEF_true an output Go clears the maintenance currents at the end of the trial -- you use it, you lose it..
  bool		clear_veto;	// #DEF_false #CONDEDIT_ON_out_go_clear if both MAINT and OUT Go signals occur at the same time, then this vetos the out_go_clear that would otherwise occur
  float		off_accom;	// #DEF_0 #EXPERT #MIN_0 #MAX_1 how much of the maintenance current to apply to accommodation after turning a unit off

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PFCGateSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API PFCLayerSpec : public LeabraLayerSpec {
  // Prefrontal cortex layer: gets maintenance and output gating signals from SNrThal -- gate toggles off in minus phase, on at end of plus phase, output gating drives unit act relative to internal act_eq value
INHERITED(LeabraLayerSpec)
public:
  enum MaintUpdtAct {
    STORE,			// store current activity state in maintenance currents
    CLEAR,			// clear current activity state from maintenance currents
  };

  PFCGateSpec	gate;		// parameters controlling the gating of pfc units

  virtual void	GetSNrThalLayers(LeabraLayer* lay, LeabraLayer*& snrthal_mnt, LeabraLayer*& snrthal_out);
  // find maintenance (required) and output (optional) snrthal input layers

  virtual void 	Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net);
  // clear various gating signals at the start of the trial

  virtual void 	Compute_MaintUpdt_ugp(LeabraUnit_Group* ugp, MaintUpdtAct updt_act,
				      LeabraLayer* lay, LeabraNetwork* net);
  // update maintenance state variables (gc.h, misc_1) based on given action: ugp impl
  virtual void	SendGateStates(LeabraLayer* lay, LeabraNetwork* net);
  // send misc_state gating state variables to the snrthal and matrix layers
  virtual void 	Compute_Gating(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer activations -- each cycle during normal processing
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
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PFCLayerSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

//////////////////////////////////////////
//	PFC Layer Spec	(Output)	//
//////////////////////////////////////////

class LEABRA_API PFCOutLayerSpec : public LeabraLayerSpec {
  // Prefrontal cortex output gated layer: gets gating signal and activations from PFC_mnt layer: gating modulates strength of activation
INHERITED(LeabraLayerSpec)
public:
  virtual void	Compute_PfcOutAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute PFC output layer activations -- replaces std act fun

  override void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PFCOutLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

//////////////////////////////////////////
//	    Special PFC LV PrjnSpec 	//
//////////////////////////////////////////

class LEABRA_API PFCLVPrjnSpec : public FullPrjnSpec {
  // A special projection spec for PFC to LVe/i layers. If n unit groups (stripes) in LV == PFC, then it makes Gp one-to-one projections; if LV stripes == 1, it makes a single full projection; if LV stripes == PFC + 1, the first projection is full and the subsequent are gp one-to-one; if recv fm multiple PFC layers, same logic applies to each
INHERITED(FullPrjnSpec)
public:

  virtual void Connect_Gp(Projection* prjn, Unit_Group* rugp, Unit_Group* sugp);
  // make a projection from all senders in sugp into all receivers in rugp

  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(PFCLVPrjnSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// 	New Xperimental version of PBWM

//////////////////////////////////
//	  Matrix Layer Spec	//
//////////////////////////////////

class LEABRA_API XMatrixDaSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra how dopamine is applied to the matrix units
INHERITED(taOBase)
public:
  float		gain;		// #DEF_1 overall gain for da modulation of matrix units
  float		perf_contrast;	// #DEF_0 for performance/bias dopamine values: proportion of da * act to apply for da modulation -- if this value is 1 (highest contrast) then inactive units get no additional boost, and if it is 0 then every unit gets the same amount
  float		mnt_lrn_contrast; // #DEF_0.5 for maintenance learning dopamine values: proportion of da * act to apply for da modulation -- if this value is 1 (highest contrast) then inactive units get no additional boost, and if it is 0 then every unit gets the same amount
  float		out_lrn_contrast; // #DEF_1 for output learning dopamine values: proportion of da * act to apply for da modulation -- if this value is 1 (highest contrast) then inactive units get no additional boost, and if it is 0 then every unit gets the same amount

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(XMatrixDaSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API XMatrixMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the matrix layer
INHERITED(taOBase)
public:
  bool		one_bias_da;	// #DEF_true use one value to specify the bias dopamine values (mnt_nogo_da, empty_go_da, out_pvr_da) instead of specifying each separately -- typically things work best with just one value
  float		bias_da;	// #DEF_5 #CONDSHOW_ON_one_bias_da the one dopamine (da) bias value to use for mnt_nogo_da, empty_go_da, out_pvr_da, if one_bias_da flag is true 
  float		mnt_nogo_da;	// #DEF_5 #CONDEDIT_OFF_one_bias_da for stripes that are maintaining, amount of NoGo (negative dopamine) performance da mod, only if not on an output trial as determined by PVr -- this is critical for enabling robust maintenance 
  float		empty_go_da;	// #DEF_5 #CONDEDIT_OFF_one_bias_da for empty stripes, amount of Go (positive dopamine) performance da mod, only if not on an output trial as determined by PVr -- provides a bias for maintaining new information
  float		out_pvr_da;	// #DEF_5 #CONDEDIT_OFF_one_bias_da if PVr detects a reward trial is coming, amount of Go (positive dopamine) peformance da mod to bias the output gating units to respond instead of maint or nogo
  float		out_noise_amp;	// #DEF_10 amplifier for output gating noise, which generally needs to be higher than maintenance gating -- multiplies noise value by this amount
  float		perf_gain;	// #DEF_0 performance effect da gain -- multiplies naturally-computed da values (mainly just LV) in minus phase -- does NOT multiply the following additional perf da factors
  float		neg_da_bl;	// #DEF_0 #EXPERT negative da baseline in learning condition: this amount subtracted from all da values in learning phase (essentially reinforces nogo)
  float		neg_gain;	// #DEF_1 #EXPERT gain for negative DA signals relative to positive ones: neg DA may need to be stronger!
  bool		no_snr_mod;	// #DEF_false #EXPERT disable the Da learning modulation by SNrThal ativation (this is only to demonstrate how important it is)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(XMatrixMiscSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

// matrix unit misc_ var docs
// * misc_1 = patch LVe value for patch-modulated noise
// * misc_2 = performance da value in minus phase, carried over to plus phase

class LEABRA_API XMatrixLayerSpec : public LeabraLayerSpec {
  // eXperimental basal ganglia matrix layer, integrates maintenance and output gating
INHERITED(LeabraLayerSpec)
public:
  XMatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  XMatrixDaSpec 	da;		// how dopamine is applied to the units
  MatrixGoNogoGainSpec	go_nogo_gain;	// separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc
  MatrixRndGoSpec	rnd_go;		// matrix random Go firing for nogo firing stripes case

  virtual void 	Compute_UnitPerfDaMod(LeabraUnit* u, float perf_dav, float act_val, int go_no);
  // apply given performance dopamine modulation value to the unit, based on whether it is a go or nogo; contrast enhancement based on activation (act_val)
  virtual void 	Compute_DaPerfMod(LeabraLayer* lay, LeabraUnit_Group* mugp,
				  int gpidx, LeabraNetwork* net);
  // compute dynamic da modulation; performance modulation, not learning (second minus phase)
  virtual void 	Compute_DaLearnMod(LeabraLayer* lay, LeabraUnit_Group* mugp,
				   int gpidx, LeabraNetwork* net);
  // compute dynamic da modulation: evaluation modulation, which is sensitive to GO/NOGO firing and activation in action phase

  virtual void 	Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // clear the rnd go signals
  virtual void 	Compute_NoGoRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // compute random Go for nogo case

  virtual void	LabelUnits_impl(Unit_Group* ugp);
  // label units with Go/No (unit group) -- auto done in InitWeights
  virtual void	LabelUnits(LeabraLayer* lay);
  // label units with Go/No -- auto done in InitWeights

  override void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);

  //  override bool	Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(XMatrixLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////////////////////////////
//	  SNrThalLayer: Integrate Matrix and compute Gating 	//
//////////////////////////////////////////////////////////////////

// snr unit misc_ var docs
// * misc_1 = patch LVe value for patch-modulated gating signal

class LEABRA_API XSNrThalLayer : public LeabraLayer {
  // layer for the SNr & Thalamus of the PBWM model -- has inhibitory competition both within group and between maint and output gating guys
INHERITED(LeabraLayer)
public:
  LeabraSort 	mnt_units;	// #HIDDEN #NO_SAVE #CAT_Activation list of maint units
  LeabraSort 	mnt_active_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of active units -- maint sort
  LeabraSort 	mnt_inact_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of inactive units -- maint sort
  LeabraSort 	out_units;	// #HIDDEN #NO_SAVE #CAT_Activation list of output units
  LeabraSort 	out_active_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of active units -- output sort
  LeabraSort 	out_inact_buf;	// #HIDDEN #NO_SAVE #CAT_Activation list of inactive units -- output sort
  KWTAVals	mnt_kwta;	// #READ_ONLY #EXPERT #CAT_Activation values for maint kwta -- activity levels, etc NOTE THIS IS A COMPUTED VALUE: k IS SET IN LayerSpec!
  InhibVals	mnt_i_val;	// #READ_ONLY #EXPERT #CAT_Activation inhibitory values computed by kwta
  KWTAVals	out_kwta;	// #READ_ONLY #EXPERT #CAT_Activation values for output kwta -- activity levels, etc NOTE THIS IS A COMPUTED VALUE: k IS SET IN LayerSpec!
  InhibVals	out_i_val;	// #READ_ONLY #EXPERT #CAT_Activation inhibitory values computed by kwta

  override void  BuildUnits();

  TA_SIMPLE_BASEFUNS(XSNrThalLayer);
private:
  void	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API XSNrThalMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the snrthal layer
INHERITED(taOBase)
public:
  float		net_off;	// #DEF_-0.1 netinput offset -- how much to add to each unit's baseline netinput -- positive values make it more likely that some stripe will always fire, even if it has a net nogo activation state in the matrix -- very useful for preventing all nogo situations -- if net_off is .2 then act.gain should be 600, if net_off is 0 then act.gain should be 20 (dynamic range is diff)
  float		go_thr;		// #DEF_0.5 threshold in snrthal activation required to trigger a Go gating event
  float		rnd_go_inc;	// #DEF_0.2 how much to add to the net input for a random-go signal triggered in corresponding matrix layer?

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(XSNrThalMiscSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API XSNrThalLayerSpec : public LeabraLayerSpec {
  // computes activation = GO - NOGO from MatrixLayerSpec
INHERITED(LeabraLayerSpec)
public:
  XSNrThalMiscSpec	snrthal; // misc specs for snrthal layer
  bool			mnt_inhib; // #DEF_true also perform inhibitory competition among the maintenance units 
  KWTASpec		mnt_kwta; // #CONDEDIT_ON_mnt_inhib #CAT_Activation desired activity level for maintenance units across stripes 
  bool			out_inhib; // #DEF_true also perform inhibitory competition among the output units 
  KWTASpec		out_kwta; // #CONDEDIT_ON_out_inhib #CAT_Activation desired activity level for output units across stripes 
  LeabraInhibSpec 	mnt_out_inhib;	// #CAT_Activation how to compute inhibition for maintenance and output unit competition

  virtual void	Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net);
  // compute netinput as GO - NOGO on matrix layer

  // hook for new netin goes here:
  override void Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net);

  // mnt/out inhibition

  override void Compute_Active_K(LeabraLayer* lay, LeabraNetwork* net);
    virtual void Compute_Active_K_mntout(XSNrThalLayer* lay, LeabraSort* ug,
				       KWTASpec& kwtspec, KWTAVals& kvals);
    // #IGNORE mnt out

  override void Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
    virtual void Compute_Inhib_kWTA_Sort_mntout(LeabraSort& ug, LeabraSort& act_buf,
						LeabraSort& inact_buf,
						int k_eff, float& k_net, int& k_idx);
    // #IGNORE mnt, out version of kwta sort
    virtual void Compute_Inhib_kWTA_mntout(KWTASpec& kwta, KWTAVals& kvals, InhibVals& ivals,
				 LeabraSort& uns, LeabraSort& act_buf,
				 LeabraSort& inact_buf, LeabraNetwork*);
    // #IGNORE mnt, out version of kwta

  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);


  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(XSNrThalLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
//	PFC Unit Spec	

class LEABRA_API PFCUnitSpec : public LeabraUnitSpec {
  // Prefrontal cortex units: activation output is modulated by output gating signal
INHERITED(LeabraUnitSpec)
public:

  override void	DecayState(LeabraUnit* u, LeabraNetwork* net, float decay);
  override void Send_NetinDelta(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  override void Compute_Conduct(LeabraUnit* u, LeabraNetwork* net);
  override void Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net);

  TA_BASEFUNS_NOCOPY(PFCUnitSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
//	XPFC Layer Spec	
//////////////////////////////////////////

// misc_state docs:
// * pfc ugp->misc_state = counter of number of trials in maint or empty state
// ** 0 = just cleared (empty)
// ** 1+ = maint for a trial or a more
// ** -1- = empty for a trial or more
// * pfc ugp->misc_state1 = GateState 
// * pfc ugp->misc_state2 = cur gating signal (cleared at start of trial, latches to first Go)
// * pfc ugp->misc_float = current Go activation value (mnt or out) -- used by units to boost netin -- already multiplied by gate.mnt/out_go_netin
// * pfc ugp->misc_float1 = current output gating Go activation value with base_gain and go_gain factored in -- used for graded Go -- just multiply directly by this number

class LEABRA_API XPFCGateSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(taOBase)
public:
  enum	GateSignal {
    GATE_MNT_GO = 0,		// gate maintenance Go unit fired 
    GATE_OUT_GO = 1,		// gate output Go unit fired 
    GATE_NOGO = 2		// gate NoGo unit fired
  };

  enum	GateState {		// what happened on last gating action, stored in misc_state1 on unit group -- for debugging etc
    EMPTY_MNT_GO,		// stripe was empty, got MAINT Go
    EMPTY_OUT_GO,		// stripe was empty, got OUTPUT Go
    EMPTY_OUT_MNT_GO,		// stripe was empty, got OUTPUT then MAINT Go
    EMPTY_NOGO,			// stripe was empty, got NoGo
    MAINT_MNT_GO,		// stripe was already maintaining, got MAINT Go: cleared, encoded
    MAINT_OUT_GO,		// stripe was already maintaining, got OUTPUT Go
    MAINT_OUT_MNT_GO,		// stripe was already maintaining, got OUTPUT then MAINT Go
    MAINT_NOGO,			// stripe was already maintaining, got NoGo
    NOGO_RND_GO,		// random go for stripes constantly firing nogo
    INIT_STATE,			// initialized state at start of trial
  };

  float		base_gain;	// #DEF_0.5 #MIN_0 #MAX_1 how much activation gets through even without a Go gating signal
  float		go_gain;	// #READ_ONLY how much extra to add for a Go signal -- automatically computed to be 1.0 - base_gain
  bool		graded_out_go;	// #DEF_true use actual activation level of output Go signal to drive output activation level
  float		go_learn_base;	// #DEF_0.06 #MIN_0 #MAX_1 how much PFC learning occurs in the absence of go gating modulation -- 1 minus this is how much happens with go gating -- determines how far plus phase activations used in learning can deviate from minus-phase activation state: plus phase act_nd = act_m + (go_learn_base + (1-go_learn_base) * gate_act) * (act - act_m)
  float		go_learn_mod;	// #READ_ONLY 1 - go_learn_base -- how much learning is actually modulated by go gating activation
  float		go_netin_gain;	  // #DEF_0.01 extra net input to add to active units as a function of gating signal -- uses the mnt_go_learn_mod to determine if maintenance go contributes to the gating signal
  bool	        mnt_to_bg;	// #DEF_true send maintenance activation values to the PVLV LVe and Matrix layers instead of the output gated activation (act) which is sent to other layers
  float		clear_decay;	// #DEF_0 #MIN_0 #MAX_1 how much to decay the activation state for units in the stripe when the maintenance is cleared -- simulates a phasic inhibitory burst (GABA-B?) from the gating pulse
  bool	        mnt_clear_veto;	// #DEF_true a maint Go gating signal, arriving after output gating, can veto the clearing of maintenance currents that would otherwise occur from the output gating
  bool		out_go_clear;	// #DEF_true an output Go clears the maintenance currents at the end of the trial -- you use it, you lose it..
  float		off_accom;	// #DEF_0 #EXPERT #MIN_0 #MAX_1 how much of the maintenance current to apply to accommodation after turning a unit off

  inline void	SetBaseGain(float bg)
  { base_gain = bg;
    if(base_gain > 1.0f) base_gain = 1.0f; if(base_gain < 0.0f) base_gain = 0.0f;
    go_gain = 1.0f - base_gain; }
  // set base gain value with limits enforced and go_gain updated


  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(XPFCGateSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API XPFCLayerSpec : public LeabraLayerSpec {
  // Prefrontal cortex layer: gets maintenance and output gating signals from SNrThal -- gate toggles off in minus phase, on at end of plus phase, output gating drives unit act relative to internal act_eq value
INHERITED(LeabraLayerSpec)
public:
  enum MaintUpdtAct {
    STORE,			// store current activity state in maintenance currents
    CLEAR,			// clear current activity state from maintenance currents
  };

  XPFCGateSpec	gate;		// parameters controlling the gating of pfc units

  virtual void 	Compute_TrialInitGates(LeabraLayer* lay, LeabraNetwork* net);
  // clear various gating signals at the start of the trial

  virtual void 	Compute_MaintUpdt_ugp(LeabraUnit_Group* ugp, MaintUpdtAct updt_act,
				      LeabraLayer* lay, LeabraNetwork* net);
  // update maintenance state variables (gc.h, misc_1) based on given action: ugp impl
  virtual void	SendGateStates(LeabraLayer* lay, LeabraNetwork* net);
  // send misc_state gating state variables to the snrthal and matrix layers
  virtual void 	Compute_Gating(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer activations -- each cycle during normal processing
  virtual void 	Compute_Gating_Final(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer activations -- at end of plus phase

  override void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(XPFCLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

#endif // leabra_pbwm_h
