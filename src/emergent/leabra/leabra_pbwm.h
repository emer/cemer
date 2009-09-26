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
  bool		lv_mnt_pv_out;	// #DEF_true send LV-based da signals to maintenance matrix layers, and PV-based da to output matrix layers

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

//////////////////////////////////////////////////////////////////
//	  SNrThalLayer: Integrate Matrix and compute Gating 	//
//////////////////////////////////////////////////////////////////

class LEABRA_API SNrThalMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the snrthal layer
INHERITED(taOBase)
public:
  float		net_off;	// #DEF_-0.1:0.5 #MIN_-1 #MAX_1 netinput offset -- how much to add to each unit's baseline netinput -- positive values make it more likely that some stripe will always fire, even if it has a net nogo activation state in the matrix -- very useful for preventing all nogo situations -- if net_off is .2 then act.gain should be 600, if net_off is 0 then act.gain should be 20 (dynamic range is diff)
  float		go_thr;		// #DEF_0.1:0.5 #MIN_0 #MAX_1 threshold in snrthal activation required to trigger a Go gating event
  float		rnd_go_inc;	// #DEF_0.2 #MIN_0 how much to add to the net input for a random-go signal triggered in corresponding matrix layer?

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

////////////////////////////////////////////////////////////////////
//	  Base Specs for Algorithm-derived sub types

class LEABRA_API MatrixBaseLayerSpec : public LeabraLayerSpec {
  // #VIRT_BASE base matrix layer spec -- derive version-specific guys from this -- other code may check for this one to remain generic wrt versions
INHERITED(LeabraLayerSpec)
public:
  TA_BASEFUNS_NOCOPY(MatrixBaseLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API PFCBaseLayerSpec : public LeabraLayerSpec {
  // #VIRT_BASE base pfc layer spec -- derive version-specific guys from this -- other code may check for this one to remain generic wrt versions
INHERITED(LeabraLayerSpec)
public:
  TA_BASEFUNS_NOCOPY(PFCBaseLayerSpec);
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
      C_Compute_dWt_Matrix(cn, LinFmSigWt(cn->wt), ru->act_m2, ru->dav, su->act_m2,
			   ru->l_thr);
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

class LEABRA_API MatrixGateBiasSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating biases depending on different conditions in the network -- helps to get the network performing appropriately for basic maintenance functions, to then be reinforced or overridden by learning
INHERITED(taOBase)
public:
  bool		one_bias;	// #DEF_true use one value to specify the main gating biases (mnt_nogo, empty_go, out_pvr) instead of specifying each separately -- typically things work best with the same value for all biases, so this makes it simpler to set them
  float		bias; 		// #DEF_0:2 #CONDSHOW_ON_one_bias the one dopamine (da) bias value to use for mnt_nogo, empty_go, out_pvr, if one_bias flag is true 
  float		mnt_nogo;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for stripes that are maintaining, amount of NoGo bias da (negative dopamine) -- only if not on an output trial as determined by PVr -- this is critical for enabling robust maintenance 
  float		mnt_empty_go;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for empty MAINT stripes, amount of Go bias da (positive dopamine) -- only if not on an output trial as determined by PVr -- provides a bias for encoding and maintaining new information
  float		out_pvr;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for OUTPUT stripes, if PVr detects that this is trial where external rewards are typically provided, amount of OUTPUT Go bias da (positive dopamine) to encourage the output gating units to respond -- only applied for stripes that are maintaining information -- otherwise see out_empty_nogo
  float		out_empty_nogo;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for OUTPUT stripes that are not maintaining anything, amount of OUTPUT NoGo bias da (negative dopamine) to encourage the output gating units to NOT respond when the stripe is NOT maintaining anything
  float		mnt_pvr;	// #DEF_0 if PVr detects that this is trial where external rewards are typically provided, amount of MAINT Go bias da (positive dopamine) to encourage the maintenance gating units to respond

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
  float		neg_gain;	// #DEF_1;0.7;1.5 gain for negative DA signals relative to positive ones -- currently deprecated and should generally just be set to 1.0
  float		neg_da_bl;	// #MIN_0 #DEF_0;0.0002 negative da baseline in learning condition: this amount subtracted from all da values in learning phase (essentially reinforces nogo) -- currently deprecated and should generally be set to 0

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixMiscSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API MatrixRndGoSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc random go specifications -- when stripe has not fired for a long time, encourage some Go firing to get back into the game..
INHERITED(taOBase)
public:
  bool		sep_out_mnt;	// use separate logic for OUTPUT and MAINT Go -- trigger OUTPUT Go for over-maintenance (e.g., for out_go_clear case -- will use and clear info) and MAINT Go for over-empty (will update to new rep)
  int		nogo_thr;	// #DEF_50 threshold of number of nogo firing in a row that will trigger NoGo random go firing
  float		nogo_p;		// #DEF_0.1;0 probability of actually firing a nogo random Go once the threshold is exceeded
  float		nogo_da;	// #DEF_10 strength of DA for activating Go (gc.h) and inhibiting NoGo (gc.a) for a nogo-driven random go firing
  float		go_bias;	// how strong of a performance bias to apply to induce units to fire a Go during a random go firing episode -- this is like the gate_bias -- applied from start of trial

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixRndGoSpec);
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
  MatrixRndGoSpec	rnd_go;		// matrix random Go firing for nogo firing stripes case
  MatrixGoNogoGainSpec  go_nogo_gain;	// separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc

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
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

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

//////////////////////////////////////////
//	PFC Unit Spec	

class LEABRA_API PFCUnitSpec : public LeabraUnitSpec {
  // Prefrontal cortex units: activation output is modulated by basal ganglia gating signal -- can improve learning
INHERITED(LeabraUnitSpec)
public:

  override void Compute_Conduct(LeabraUnit* u, LeabraNetwork* net);

  TA_BASEFUNS_NOCOPY(PFCUnitSpec);
private:
  void	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
//	PFC Layer Spec (Maintenance)	//
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
// * pfc ugp->misc_float2 = current Go activation value (mnt or out) for modulating netinput of units in proportion to gating signals -- requires PFCUnitSpec to do that -- already multiplied by gate.go_netin_gain

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
    NOGO_RND_GO = 20,		// random go for stripes constantly firing nogo
    INIT_STATE = 30,		// initialized state at start of trial
  };

  enum MntOutGo {		// what to do when both MAINT and OUTPUT fire Go at the same time
    MOGO_MNT,			// treat like a MAINT Go -- store if empty, or toggle off then back on if maintaining -- this is very good for "promiscuous" updating dynamic, as in first version of PBWM
    MOGO_VETO,			// the MAINT veto's the OUT clear that would otherwise occur on out_go_clear -- this is good in combination with out_go_clear and the gating biases
    MOGO_OUT,			// treat like an OUTPUT Go -- depends on out_go_clear -- if that is set, then just ignores MNT update and clears if maintaining, otherwise stores -- not generally useful but here for logical completeness
  };

  float		base_gain;	// #DEF_0;0.5 #MIN_0 #MAX_1 how much activation gets through even without a Go gating signal
  float		go_gain;	// #READ_ONLY how much extra to add for a Go signal -- automatically computed to be 1.0 - base_gain
  bool		graded_out_go;	// #DEF_true use actual activation level of output Go signal to drive output activation level
  bool		no_empty_out; 	// #DEF_false prevent an output gating signal from being generated from an empty stripe (one that is not currently maintaining something) -- this can help focus output gating on maintained information -- logic is that even if Go firing happens, it still takes recurrent activity from PFC to drive it, so if not maintaining, nothing happens..
  float		clear_decay;	// #DEF_0 #MIN_0 #MAX_1 how much to decay the activation state for units in the stripe when the maintenance is cleared -- simulates a phasic inhibitory burst (GABA-B?) from the gating pulse
  bool		out_go_clear;	// #DEF_true an output Go clears the maintenance currents at the end of the trial -- you use it, you lose it..
  MntOutGo	mnt_out_go;	// #DEF_MOGO_VETO what to do when both MAINT and OUTPUT fire Go at the same time -- result also depends on out_go_clear status
  float		off_accom;	// #DEF_0 #EXPERT #MIN_0 #MAX_1 how much of the maintenance current to apply to accommodation after turning a unit off

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PFCGateSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API PFCLearnSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(taOBase)
public:
  float		go_learn_base;	// #DEF_0.06 #MIN_0 #MAX_1 how much PFC learning occurs in the absence of go gating modulation -- 1 minus this is how much happens with go gating -- determines how far plus phase activations used in learning can deviate from minus-phase activation state: plus phase act_nd = act_m + (go_learn_base + (1-go_learn_base) * gate_act) * (act - act_m)
  float		go_learn_mod;	// #READ_ONLY 1 - go_learn_base -- how much learning is actually modulated by go gating activation
  float		go_netin_gain;	// #DEF_0.01 #MIN_0 extra net input to add to active units as a function of gating signal -- applied in the plus phase (post gating) to help drive learning, as in the dopamine signal

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PFCLearnSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API PFCLayerSpec : public PFCBaseLayerSpec {
  // Prefrontal cortex layer: gets maintenance and output gating signals from SNrThal -- gate toggles off in minus phase, on at end of plus phase, output gating drives unit act relative to internal act_eq value
INHERITED(PFCBaseLayerSpec)
public:
  enum MaintUpdtAct {
    STORE,			// store current activity state in maintenance currents
    CLEAR,			// clear current activity state from maintenance currents
  };

  PFCGateSpec	gate;		// parameters controlling the gating of pfc units
  PFCLearnSpec	learn;		// parameters controlling the learning in pfc units (as modulated by BG gating signals)

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
//	PFC Layer Spec (Output)	//
//////////////////////////////////////////

class LEABRA_API PFCOutLayerSpec : public PFCBaseLayerSpec {
  // Prefrontal cortex output gated layer: gets gating signal and activations from PFC_mnt layer: gating modulates strength of activation
INHERITED(PFCBaseLayerSpec)
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


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// 	Xperimental version of PBWM -- not currently very functional (aka V2.1)


//////////////////////////////////
//	  Matrix Layer Spec	//
//////////////////////////////////

class LEABRA_API XMatrixGateBiasSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating biases depending on different conditions in the network -- helps to get the network performing appropriately for basic maintenance functions, to then be reinforced or overridden by learning
INHERITED(taOBase)
public:
  bool		one_bias;	// #DEF_true use one value to specify the main gating biases (mnt_nogo, empty_go, out_pvr) instead of specifying each separately -- typically things work best with the same value for all biases, so this makes it simpler to set them
  float		bias; 		// #DEF_0:2 #CONDSHOW_ON_one_bias the one dopamine (da) bias value to use for mnt_nogo, empty_go, out_pvr, if one_bias flag is true 
  float		mnt_nogo;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for stripes that are maintaining, amount of NoGo bias da (negative dopamine) -- only if not on an output trial as determined by PVr -- this is critical for enabling robust maintenance 
  float		empty_go;	// #DEF_0:2 #CONDEDIT_OFF_one_bias for empty stripes, amount of Go bias da (positive dopamine) -- only if not on an output trial as determined by PVr -- provides a bias for encoding and maintaining new information
  float		out_pvr;	// #DEF_0:2 #CONDEDIT_OFF_one_bias if PVr detects that this is trial where external rewards are typically provided, amount of OUTPUT Go bias da (positive dopamine) to encourage the output gating units to respond
  float		mnt_pvr;	// #DEF_0 if PVr detects that this is trial where external rewards are typically provided, amount of MAINT Go bias da (positive dopamine) to encourage the maintenance gating units to respond

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(XMatrixGateBiasSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API XMatrixMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the matrix layer
INHERITED(taOBase)
public:
  float		da_gain;	// #MIN_0 #DEF_1 overall gain for da modulation of matrix units for the purposes of learning (ONLY) -- bias da is set directly by gate_bias params -- also, this value is in addition to other "upstream" gain parameters, such as vta.da.gain
  float		out_noise_amp;	// #DEF_10 amplifier for output gating noise, which generally needs to be higher than maintenance gating -- multiplies noise value by this amount
  float		neg_da_bl;	// #DEF_0 #EXPERT negative da baseline in learning condition: this amount subtracted from all da values in learning phase (essentially reinforces nogo)
  float		neg_gain;	// #DEF_1 #EXPERT gain for negative DA signals relative to positive ones: neg DA may need to be stronger!

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(XMatrixMiscSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

// matrix unit misc_ var docs
// * misc_1 = patch LVe value for patch-modulated noise

class LEABRA_API XMatrixLayerSpec : public MatrixBaseLayerSpec {
  // eXperimental basal ganglia matrix layer, integrates maintenance and output gating
INHERITED(MatrixBaseLayerSpec)
public:
  XMatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  XMatrixGateBiasSpec 	gate_bias;	// gating biases depending on different conditions in the network -- helps to get the network performing appropriately for basic maintenance functions, to then be reinforced or overridden by learning
  MatrixGoNogoGainSpec  go_nogo_gain;	// separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc
  MatrixRndGoSpec	rnd_go;		// matrix random Go firing for nogo firing stripes case

  virtual void 	Compute_BiasDaMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork* net);
  // compute gate_bias da modulation to influence gating -- continuously throughout settling
    virtual void Compute_UnitBiasDaMod(LeabraUnit* u, float bias_dav, int go_no);
    // apply bias da modulation to individual unit
  virtual void 	Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net);
  // compute u->dav learning dopamine value based on raw dav and gating state, etc -- this dav is then directly used in conspec learning rule

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
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

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
// * pfc ugp->misc_float2 = current Go activation value (mnt or out) for modulating netinput of units in proportion to gating signals -- requires PFCUnitSpec to do that -- already multiplied by gate.go_netin_gain

class LEABRA_API XPFCGateSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(taOBase)
public:
  enum	GateSignal {
    GATE_MNT_GO = 0,		// gate maintenance Go unit fired 
    GATE_OUT_GO = 1,		// gate output Go unit fired 
    GATE_NOGO = 2,		// gate NoGo unit fired
    GATE_OUT_MNT_GO = 3,	// gate maint and output Go units fired 
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
    NOGO_RND_GO = 20,		// random go for stripes constantly firing nogo
    INIT_STATE = 30,		// initialized state at start of trial
  };

  enum MntOutGo {		// what to do when both MAINT and OUTPUT fire Go at the same time
    MOGO_MNT,			// treat like a MAINT Go -- store if empty, or toggle off then back on if maintaining
    MOGO_OUT,			// treat like an OUTPUT Go -- depends on out_go_clear -- if that is set, then just ignores MNT update and clears if maintaining, otherwise stores
    MOGO_VETO,			// the MAINT veto's the OUT clear that would otherwise occur on out_go_clear
  };

  float		base_gain;	// #DEF_0;0.5 #MIN_0 #MAX_1 how much activation gets through even without a Go gating signal
  float		go_gain;	// #READ_ONLY how much extra to add for a Go signal -- automatically computed to be 1.0 - base_gain
  bool		graded_out_go;	// #DEF_true use actual activation level of output Go signal to drive output activation level
  float		go_learn_base;	// #DEF_0.06 #MIN_0 #MAX_1 how much PFC learning occurs in the absence of go gating modulation -- 1 minus this is how much happens with go gating -- determines how far plus phase activations used in learning can deviate from minus-phase activation state: plus phase act_nd = act_m + (go_learn_base + (1-go_learn_base) * gate_act) * (act - act_m)
  float		go_learn_mod;	// #READ_ONLY 1 - go_learn_base -- how much learning is actually modulated by go gating activation
  float		go_netin_gain;	// #DEF_0.01 #MIN_0 extra net input to add to active units as a function of gating signal -- uses the mnt_go_learn_mod to determine if maintenance go contributes to the gating signal
  float		clear_decay;	// #DEF_0 #MIN_0 #MAX_1 how much to decay the activation state for units in the stripe when the maintenance is cleared -- simulates a phasic inhibitory burst (GABA-B?) from the gating pulse
  bool		out_go_clear;	// #DEF_true an output Go clears the maintenance currents at the end of the trial -- you use it, you lose it..
  MntOutGo	mnt_out_go;	// #DEF_MOGO_VETO what to do when both MAINT and OUTPUT fire Go at the same time -- result also depends on out_go_clear status
  float		off_accom;	// #DEF_0 #EXPERT #MIN_0 #MAX_1 how much of the maintenance current to apply to accommodation after turning a unit off

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(XPFCGateSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API XPFCLayerSpec : public PFCBaseLayerSpec {
  // Prefrontal cortex layer: gets maintenance and output gating signals from SNrThal -- gate toggles off in minus phase, on at end of plus phase, output gating drives unit act relative to internal act_eq value
INHERITED(PFCBaseLayerSpec)
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
  virtual void	Compute_PfcMntAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute PFC maint layer activations -- add learn modulation

  override void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(XPFCLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

//////////////////////////////////////////
//	XPFC Layer Spec (Output)	//
//////////////////////////////////////////

class LEABRA_API XPFCOutLayerSpec : public PFCBaseLayerSpec {
  // Prefrontal cortex output gated layer: gets gating signal and activations from PFC_mnt layer: gating modulates strength of activation
INHERITED(PFCBaseLayerSpec)
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

  TA_SIMPLE_BASEFUNS(XPFCOutLayerSpec);
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



#endif // leabra_pbwm_h
