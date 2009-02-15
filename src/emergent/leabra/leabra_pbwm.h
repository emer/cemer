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
  virtual void	Send_PVeToMatrix(LeabraLayer* lay, LeabraNetwork* net);
  // send the PVe value computed by the patch units to misc_1 field in any MarkerCons prjn to MatrixLayerSpec layers -- used for noise modulation

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(PatchLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API SNcLayerSpec : public PVLVDaLayerSpec {
  // implements a substantia-nigra pars compacta (SNc) version of the PVLVDaLayerSpec, which receives stripe-wise LVe inputs from a PatchLayerSpec layer
INHERITED(PVLVDaLayerSpec)
public:
  float		stripe_da_gain;	// extra multiplier on the stripe-wise dopamine value relative to the global computed value (enters into weighted average with global value -- remains normalized)

  override void	Compute_Da(LeabraLayer* lay, LeabraNetwork* net);
  // compute the da value based on recv projections: every cycle in 1+ phases (delta version)

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(SNcLayerSpec);
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
  enum MatrixLearnRule {
    OUTPUT,		// output/motor delta rule for: (bg_p - bg_m) * s_m
    MAINT   		// maintenance learning rule: (bg_p2 - bg_p) * s_p
  };

  MatrixLearnRule	matrix_rule;	// learning rule to use

  inline void C_Compute_dWt_Matrix(LeabraCon* cn, float lin_wt, 
				   float ru_act_p, float ru_act_m, float su_act) {
    float err = (ru_act_p - ru_act_m) * su_act;
    // std leabra requires separate softbounding on all terms.. see XCAL for its version
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f - lin_wt);
      else		err *= lin_wt;
    }
    cn->dwt += cur_lrate * err;
  }

  inline void C_Compute_dWt_Matrix_NoSB(LeabraCon* cn, float ru_act_p, float ru_act_m,
					float su_act) {
    float err = (ru_act_p - ru_act_m) * su_act;
    cn->dwt += cur_lrate * err;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(matrix_rule == OUTPUT) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	C_Compute_dWt_Matrix(cn, LinFmSigWt(cn->wt), ru->act_p, ru->act_m, su->act_m);
      }
    }
    else { // MAINT
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	C_Compute_dWt_Matrix(cn, LinFmSigWt(cn->wt), ru->act_p2, ru->act_p, su->act_p);
      }
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(matrix_rule == OUTPUT) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	C_Compute_dWt_Matrix_NoSB(cn, ru->act_p, ru->act_m, su->act_m);
      }
    }
    else { // MAINT
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	C_Compute_dWt_Matrix_NoSB(cn, ru->act_p2, ru->act_p, su->act_p);
      }
    }
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_dWt_CtLeabraXCAL(cg, ru);
  }

  TA_SIMPLE_BASEFUNS(MatrixConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API MatrixBiasSpec : public LeabraBiasSpec {
  // Matrix bias spec: special learning parameters for matrix units
INHERITED(LeabraBiasSpec)
public:
  enum MatrixLearnRule {
    OUTPUT,		// output/motor delta rule for: (bg+ - bg-) * s-
    MAINT   		// maintenance learning rule: (bg_p2 - bg_p) * s_p
  };

  MatrixLearnRule	matrix_rule;	// learning rule to use

  inline override void B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
    float err;
    if(matrix_rule == MAINT)
      err = ru->act_p2 - ru->act_p;
    else
      err = ru->act_p - ru->act_m;
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
  int	nogo_thr;		// threshold number of sequential NOGO's per stripe before onset of nogo noise increment
  float	nogo_gain;		// how much to increase noise amplitude per every trial beyond nogo_thr: noise_amp += nogo_gain * (nogos - nogo_thr)

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
  bool	freeze_net;		// #DEF_true freeze netinput (MAINT in 2+ phase, OUTPUT in 1+ phase) during learning modulation so that learning only reflects DA modulation and not other changes in netin
  MatrixNoiseSpec matrix_noise;	// special noise parameters for matrix units

  override void Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no);
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

class LEABRA_API MatrixMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the matrix layer
INHERITED(taOBase)
public:
  float		neg_da_bl;	// #DEF_0.0002 negative da baseline in learning condition: this amount subtracted from all da values in learning phase (essentially reinforces nogo)
  float		neg_gain;	// #DEF_1.5 gain for negative DA signals relative to positive ones: neg DA may need to be stronger!
  float		perf_gain;	// #DEF_0 performance effect da gain (in 2- phase for trans, 1+ for gogo)
  bool		no_snr_mod;	// #DEF_false #EXPERT disable the Da learning modulation by SNrThal ativation (this is only to demonstrate how important it is)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(MatrixMiscSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API ContrastSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra contrast enhancement of the Matrix units, where dopamine is applied as a function of gating activation levels
INHERITED(taOBase)
public:
  float		gain;		// #DEF_1 overall gain for da modulation
  bool		one_val;	// #DEF_true use only one contrast spec value instead of 4 separate ones
  float		contrast;	// #CONDSHOW_ON_one_val [0.5 for maint, 1 for out] proportion of da * gate_act to apply for da modulation -- if this value is 1 (highest contrast) then inactive units get no additional boost, and if it is 0 then every unit gets the same amount
  float		go_p;		// #CONDSHOW_OFF_one_val [0.5 for maint, 1 for out] proportion of da * gate_act for DA+ on GO units: contrast enhancement
  float		go_n;		// #CONDSHOW_OFF_one_val [0.5 for maint, 1 for out] proportion of da * gate_act for DA- on GO units: contrast reduction
  float		nogo_p;		// #CONDSHOW_OFF_one_val [0.5 for maint, 1 for out] proportion of da * gate_act for DA+ on NOGO units: contrast enhancement
  float		nogo_n;		// #CONDSHOW_OFF_one_val [0.5 for maint, 1 for out] proportion of da * gate_act for DA- on NOGO units: contrast reduction

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(ContrastSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API MatrixLayerSpec : public LeabraLayerSpec {
  // basal ganglia matrix layer: fire actions/WM updates, or nogo; MAINT = gate in 1+ and 2+, OUTPUT = gate in -
INHERITED(LeabraLayerSpec)
public:
  enum 	BGType {       		// which type of basal ganglia circuit is this?
    OUTPUT,			// #AKA_MOTOR matrix that does output gating: controls access of frontal activations to other areas (e.g., motor output, or output of maintained PFC information)
    MAINT			// #AKA_PFC matrix that does maintenance gating: controls toggling of maintenance of activity patterns (e.g., PFC) over time
  };

  BGType		bg_type;	// type of basal ganglia/frontal system: output gating (e.g., motor) or maintenance gating (e.g., pfc)
  MatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  ContrastSpec 	 	contrast;	// contrast enhancement effects of da/dopamine neuromodulation
  MatrixRndGoSpec	rnd_go;		// matrix random Go firing for nogo firing stripes case

  virtual void 	Compute_DaMod_NoContrast(LeabraUnit* u, float dav, int go_no);
  // apply given dopamine modulation value to the unit, based on whether it is a go (0) or nogo (1); no contrast enancement based on activation
  virtual void 	Compute_DaMod_Contrast(LeabraUnit* u, float dav, float gating_act, int go_no);
  // apply given dopamine modulation value to the unit, based on whether it is a go (0) or nogo (1); contrast enhancement based on activation (gating_act)
  virtual void 	Compute_DaTonicMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork* net);
  // compute tonic da modulation (for pfc gating units in first two phases)
  virtual void 	Compute_DaPerfMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork* net);
  // compute dynamic da modulation; performance modulation, not learning (second minus phase)
  virtual void 	Compute_DaLearnMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraNetwork* net);
  // compute dynamic da modulation: evaluation modulation, which is sensitive to GO/NOGO firing and activation in action phase
  virtual void 	Compute_MotorGate(LeabraLayer* lay, LeabraNetwork* net);
  // compute gating signal for OUTPUT Matrix_out

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

  override bool	Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
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

class SNrThalMiscSpec : public taOBase {
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
  // computes activation = GO - NOGO from MatrixLayerSpec
INHERITED(LeabraLayerSpec)
public:
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
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(SNrThalLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
//	PFC Layer Spec	(Maintenance)	//
//////////////////////////////////////////

class LEABRA_API PFCGateSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(taOBase)
public:
  enum	GateSignal {
    GATE_GO = 0,		// gate GO unit fired 
    GATE_NOGO = 1		// gate NOGO unit fired
  };
  
  enum	GateState {		// what happened on last gating action, stored in misc_state1 on unit group
    EMPTY_GO,			// stripe was empty, got a GO
    EMPTY_NOGO,			// stripe was empty, got a NOGO
    LATCH_GO,			// stripe was already latched, got a GO
    LATCH_NOGO,			// stripe was already latched, got a NOGO
    LATCH_GOGO,			// stripe was already latched, got a GO then another GO
    NO_GATE,			// no gating took place
    NOGO_RND_GO,		// random go for stripes constantly firing nogo
  };

  float		off_accom;	// #DEF_0 how much of the maintenance current to apply to accommodation after turning a unit off
  bool		out_gate_learn_mod; // modulate the learning as a function of whether the corresponding output gating layer fired Go, to enforce appropriate credit assignment to only learn when given stripe participated in output -- this is a discrete modulation (all or nothing)
  bool		allow_clamp;	// #DEF_false allow external hard clamp of layer (e.g., for testing)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PFCGateSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API PFCLayerSpec : public LeabraLayerSpec {
  // Prefrontal cortex layer: gets gating signal from SNrThal, gate updates before plus and 2nd plus (update) phase (toggle off, toggle on)
INHERITED(LeabraLayerSpec)
public:
  enum MaintUpdtAct {
    NO_UPDT,			// no update action
    STORE,			// store current activity state in maintenance currents
    CLEAR,			// clear current activity state from maintenance currents
    RESTORE,			// restore prior maintenance currents (after transient input activation)
    TMP_STORE,			// temporary store of current activity state (for default maintenance of last state)
    TMP_CLEAR			// temporary clear of current maintenance state (for transient representation in second plus)
  };

  PFCGateSpec	gate;		// parameters controlling the gating of pfc units

  virtual void 	Compute_MaintUpdt_ugp(LeabraUnit_Group* ugp, MaintUpdtAct updt_act, LeabraLayer* lay, LeabraNetwork* net);
  // update maintenance state variables (gc.h, misc_1) based on given action: ugp impl
  virtual void 	Compute_MaintUpdt(MaintUpdtAct updt_act, LeabraLayer* lay, LeabraNetwork* net);
  // update maintenance state variables (gc.h, misc_1) based on given action
  virtual void	SendGateStates(LeabraLayer* lay, LeabraNetwork* net);
  // send misc_state gating state variables to the snrthal and matrix layers
  virtual void 	Compute_TmpClear(LeabraLayer* lay, LeabraNetwork* net);
  // temporarily clear the maintenance of pfc units to prepare way for transient acts
  virtual void 	Compute_GatingGOGO(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer: GOGO model

  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PFCLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

//////////////////////////////////////////
//	PFC Layer Spec	(Output)	//
//////////////////////////////////////////

class LEABRA_API PFCOutGateSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for pfc output gating
INHERITED(taOBase)
public:
  float		base_gain;	// #DEF_0.5 how much activation gets through even without a Go gating signal
  float		go_gain;	// #READ_ONLY #SHOW how much extra to add for a Go signal -- automatically computed to be 1.0 - base_gain
  bool		graded_go;	// #DEF_false use a graded Go signal as a function of strength of corresponding SNrThal unit?

  inline void	SetBaseGain(float bg)
  { base_gain = bg;
    if(base_gain > 1.0f) base_gain = 1.0f; if(base_gain < 0.0f) base_gain = 0.0f;
    go_gain = 1.0f - base_gain; }
  // set base gain value with limits enforced and go_gain updated

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PFCOutGateSpec);
protected:
  void  UpdateAfterEdit_impl();

private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API PFCOutLayerSpec : public LeabraLayerSpec {
  // Prefrontal cortex output gated layer: gets gating signal from SNrThal and activations from PFC_mnt layer: gating modulates strength of activation
INHERITED(LeabraLayerSpec)
public:
  enum	BGSValue {		// what value to drive the base gain schedule with
    NO_BGS,			// don't use a base gain schedule
    EPOCH,			// current epoch counter
    EXT_REW_STAT,		// avg_ext_rew value on network (computed over an "epoch" of training): value is * 100 (0..100) 
    EXT_REW_AVG	= 0x0F,		// uses average reward computed by ExtRew layer (if present): value is units[0].act_avg (avg_rew) * 100 (0..100) 
  };

  PFCOutGateSpec out_gate;	// #CAT_PFC parameters controlling the output gating of pfc units
  BGSValue	gain_sched_value; // #CAT_PFC what value drives the base_gain schedule (Important: affects values entered in start_ctr fields of schedule!)
  Schedule	gain_sched;	// #CAT_PFC schedule of out_gate.base_gain values as a function of training epochs or other -- note that these are the literal values and not multipliers on the value entered in out_gate.base_gain -- they replace that value

  virtual void	SetCurBaseGain(LeabraNetwork* net);
  // set current base gain based on gain_sched if in use
  virtual void	Compute_PfcOutAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute PFC output layer activations -- replaces std act fun

  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);

  override void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PFCOutLayerSpec);
protected:
  void  UpdateAfterEdit_impl();

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
