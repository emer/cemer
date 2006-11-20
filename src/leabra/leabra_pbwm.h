// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
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

//////////////////////////////////
//	  Patch/SNc	 	//
//////////////////////////////////

class LEABRA_API PatchLayerSpec : public LVeLayerSpec {
  // simulates Patch as a LV layer: currently no doing anything different than regular LV
public:

  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(PatchLayerSpec);
};

class LEABRA_API SNcMiscSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER misc parameters for SNc layer
public:
  enum PatchMode {
    NO_PATCH,			// no patch at all
    PATCH			// use patch: Note currently not supported.
  };

  PatchMode	patch_mode;	// #DEF_NO_PATCH how to run the patch computation
  float		patch_gain;	// #CONDEDIT_ON_patch_mode:PATCH #DEF_0.5 proportion of patch (stripe-specific) da relative to global abl da

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SNcMiscSpec);
  COPY_FUNS(SNcMiscSpec, taBase);
  TA_BASEFUNS(SNcMiscSpec);
};

class LEABRA_API SNcLayerSpec : public PVLVDaLayerSpec {
  // computes PVLV 'Da' signal, uses inputs from the patch to provide stripe-specific modulation (NOTE: not currently supported); Does not do SNrThal modulation, which is computed directly in Matrix units
public:
  SNcMiscSpec	snc;		// misc SNc specific parameters controlling influence of patch modulation

  void	Compute_Da(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  void 	Initialize();
  void	Destroy()		{ };
  void 	InitLinks();
  SIMPLE_COPY(SNcLayerSpec);
  COPY_FUNS(SNcLayerSpec, PVLVDaLayerSpec);
  TA_BASEFUNS(SNcLayerSpec);
};

//////////////////////////////////
//	  Matrix Con/Units	//
//////////////////////////////////

class LEABRA_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on dopamine modulation of activation
public:
  enum LearnRule {
    OUTPUT_DELTA,		// #AKA_MOTOR_DELTA delta rule for BG_motor: (bg+ - bg-) * s-
    OUTPUT_CHL,			// #AKA_MOTOR_CHL CHL rule for BG_motor: (bg+ * s+) - (bg- * s-)
    MAINT   			// #AKA_PFC MAINT learning rule: (bg_p2 - bg_p) * s_p
  };

  LearnRule	learn_rule;	// learning rule to use

  inline float C_Compute_Hebb(LeabraCon* cn, LeabraCon_Group* cg, DaModUnit* ru, DaModUnit* su) {
    // wt is negative in linear form, so using opposite sign of usual here
    float rval;
    if((learn_rule == OUTPUT_DELTA) || (learn_rule == OUTPUT_CHL))
      rval = ru->act_p * (su->act_p * (cg->savg_cor + cn->wt) + (1.0f - su->act_p) * cn->wt);
    else
      rval = ru->act_p2 * (su->act_p * (cg->savg_cor + cn->wt) + (1.0f - su->act_p) * cn->wt);
    return rval;
  }

  inline float C_Compute_Err(LeabraCon* cn, DaModUnit* ru, DaModUnit* su) {
    float err = 0.0f;
    switch(learn_rule) {
    case OUTPUT_DELTA:
      err = (ru->act_p - ru->act_m) * su->act_m;
      break;
    case OUTPUT_CHL:
      err = (ru->act_p * su->act_p) - (ru->act_m * su->act_m);
      break;
    case MAINT:
      err = (ru->act_p2  - ru->act_p) * su->act_p;
      break;
    }
    // wt is negative in linear form, so using opposite sign of usual here
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f + cn->wt);
      else		err *= -cn->wt;	
    }
    return err;
  }

  inline void Compute_dWt(Con_Group* cg, Unit* ru) {
    DaModUnit* lru = (DaModUnit*)ru;
    LeabraCon_Group* lcg = (LeabraCon_Group*) cg;
    Compute_SAvgCor(lcg, lru);
    if(((LeabraLayer*)cg->prjn->from)->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<lcg->size; i++) {
	DaModUnit* su = (DaModUnit*)lcg->Un(i);
	LeabraCon* cn = (LeabraCon*)lcg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts.avg < savg_cor.thresh))) {
	  float orig_wt = cn->wt;
	  C_Compute_LinFmWt(lcg, cn); // get into linear form
	  C_Compute_dWt(cn, lru, 
			C_Compute_Hebb(cn, lcg, lru, su), 
			C_Compute_Err(cn, lru, su));
	  cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
	}
      }
    }
  }

  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  TA_BASEFUNS(MatrixConSpec);
};

class LEABRA_API MatrixBiasSpec : public LeabraBiasSpec {
  // Matrix bias spec: special learning paramters for matrix units
public:
  enum LearnRule {
    OUTPUT_DELTA,		// delta rule for BG_motor: (bg+ - bg-) * s-
    OUTPUT_CHL,			// CHL rule for BG_motor: (bg+ * s+) - (bg- * s-)
    MAINT   			// MAINT: learn on 2p - p
  };

  LearnRule	learn_rule;	// learning rule to use

  void B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru) {
    DaModUnit* dau = (DaModUnit*)ru;
    float err;
    if(learn_rule == MAINT)
      err = dau->act_p2 - dau->act_p;
    else
      err = ru->act_p - ru->act_m;
    if(fabsf(err) >= dwt_thresh)
      cn->dwt += cur_lrate * err;
  }
  
  void 	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(MatrixBiasSpec);
};

class LEABRA_API MatrixUnitSpec : public DaModUnitSpec {
  // basal ganglia matrix units: fire actions or WM updates. modulated by da signals
public:
  bool	freeze_net;		// #DEF_true freeze netinput (MAINT in 2+ phase, OUTPUT in 1+ phase) during learning modulation so that learning only reflects DA modulation and not other changes in netin

  void 	Compute_NetAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  void	PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
		   LeabraNetwork* net, bool set_both=false);

  void	Defaults();

  void	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(MatrixUnitSpec);
  COPY_FUNS(MatrixUnitSpec, DaModUnitSpec);
  TA_BASEFUNS(MatrixUnitSpec);
};

//////////////////////////////////
//	  Matrix Layer Spec	//
//////////////////////////////////

class LEABRA_API MatrixMiscSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER misc specs for the matrix layer
public:
  float		neg_da_bl;	// #DEF_0.0002 negative da baseline in learning condition: this amount subtracted from all da values in learning phase (essentially reinforces nogo)
  float		neg_gain;	// #DEF_1.5 gain for negative DA signals relative to positive ones: neg DA may need to be stronger!
  float		perf_gain;	// #DEF_0 performance effect da gain (in 2- phase for trans, 1+ for gogo)
  bool		no_snr_mod;	// #DEF_false disable the Da learning modulation by SNrThal ativation (this is only to demonstrate how important it is)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(MatrixMiscSpec);
  COPY_FUNS(MatrixMiscSpec, taBase);
  TA_BASEFUNS(MatrixMiscSpec);
};

class LEABRA_API ContrastSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER contrast enhancement of the GO units
public:
  float		gain;		// #DEF_1 overall gain for da modulation
  float		go_p;		// #DEF_0.5 proportion of da * gate_act for DA+ on GO units: contrast enhancement
  float		go_n;		// #DEF_0.5 proportion of da * gate_act for DA- on GO units: contrast reduction
  float		nogo_p;		// #DEF_0.5 proportion of da * gate_act for DA+ on NOGO units: contrast enhancement
  float		nogo_n;		// #DEF_0.5 proportion of da * gate_act for DA- on NOGO units: contrast reduction

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ContrastSpec);
  COPY_FUNS(ContrastSpec, taBase);
  TA_BASEFUNS(ContrastSpec);
};

class LEABRA_API MatrixRndGoSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER misc random go specifications (unconditional, nogo)
public:
  float		avgrew;		// #DEF_0.9 threshold on global avg reward value (0..1) below which random GO can fire (uses ExtRew_Stat if avail, else avg value from ExtRewLayer) -- once network is doing well overall, shutdown the exploration.  This is true for all cases EXCEPT err rnd go

  float		ucond_p;	// #DEF_1e-04 unconditional random go probability (on every trial, each stripe has this probability of firing a Go randomly, without conditions)
  float		ucond_da;	// #DEF_1 strength of DA for activating Go (gc.h) and inhibiting NoGo (gc.a) for the unconditional random go

  int		nogo_thr;	// #DEF_50 threshold of number of nogo firing in a row that will trigger NoGo random go firing
  float		nogo_p;		// #DEF_0.1 probability of actually firing a nogo random Go once the threshold is exceeded
  float		nogo_da;	// #DEF_10 strength of DA for activating Go (gc.h) and inhibiting NoGo (gc.a) for a nogo-driven random go firing

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(MatrixRndGoSpec);
  COPY_FUNS(MatrixRndGoSpec, taBase);
  TA_BASEFUNS(MatrixRndGoSpec);
};

class LEABRA_API MatrixErrRndGoSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER matrix random Go firing to encourage exploration when (a series of) errors occur: a stripe is chosen from a softmax over the snrthal netinputs (closer to firing chosen with higher probability)
public:
  bool		on;		// #DEF_true whether to use error-driven random go
  int		min_cor;	// #CONDEDIT_ON_on:true [Default is 5 for MAINT, 1 for OUTPUT] minimum number of sequential correct to start counting errors and doing random go: need some amount of correct before errors count!
  int		min_errs;	// #CONDEDIT_ON_on:true #DEF_1 minimum number of sequential errors to start this random go exploration
  float		err_p;		// #CONDEDIT_ON_on:true #DEF_1 baseline probability of firing Go, once above min_cor and min_errs
  float		gain;		// #CONDEDIT_ON_on:true [Default is .2 for MAINT, .5 for OUTPUT] gain on softmax over netinputs on snrthal units for choosing the stripe to activate Go for: softmax = normalized exp(gain * snrthal->net)
  float		if_go_p;	// #CONDEDIT_ON_on:true #DEF_0 probability of firing a random Go if some stripes are actually currently firing a Go (i.e., the not-all-nogo case); by default, only fire Go if all stripes are firing nogo
  float		err_da;		// #CONDEDIT_ON_on:true #DEF_10 strength of DA for activating Go (gc.h) and inhibiting NoGo (gc.a) when error Go is fired (for learning effect) -- this multiplies the actual DA value coming from the SNc, and is also weighted by the netinput of the snrthal stripe; da = -dav * err_da * (snrthal->net + 1)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(MatrixErrRndGoSpec);
  COPY_FUNS(MatrixErrRndGoSpec, taBase);
  TA_BASEFUNS(MatrixErrRndGoSpec);
};

class LEABRA_API MatrixAvgDaRndGoSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER matrix random Go firing to encourage exploration for non-productive stripes based on softmax of avg_go_da for that stripe (matrix_u->misc_1)
public:
  bool		on;		// [Default true for MAINT, false for OUTPUT] whether to use random go based on average dopamine values
  float		avgda_p;	// #CONDEDIT_ON_on:true #DEF_0.1 baseline probability of firing random Go for any stripe (first pass before doing softmax)
  float		gain;		// #CONDEDIT_ON_on:true #DEF_0.5 gain on softmax over avgda values on snrthal units for choosing the stripe to activate Go for (softmax = normalized exp(gain * (avgda_thr - avg_go_da))
  float		avgda_thr;	// #CONDEDIT_ON_on:true #DEF_0.1 threshold on per stripe avg_go_da value (-1..1) below which the random Go starts happening (and is subtracted from avgda as the reference point for the softmax computation)
  int		nogo_thr;	// #CONDEDIT_ON_on:true #DEF_10 minimum number of sequential nogos in a row for a stripe before a avg-da random Go will fire (not to be confused with nogo_thr, which is regardless of avgda -- this is specifically for avg-da random go)
  float		avgda_da;	// #CONDEDIT_ON_on:true #DEF_10 strength of DA for activating Go (gc.h) and inhibiting NoGo (gc.a) when go is fired (for learning effect)
  float		avgda_dt;	// #CONDEDIT_ON_on:true #DEF_0.005 time constant for integrating the average DA value associated with Go firing for each stripe (stored in matrix_u->misc_1)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(MatrixAvgDaRndGoSpec);
  COPY_FUNS(MatrixAvgDaRndGoSpec, taBase);
  TA_BASEFUNS(MatrixAvgDaRndGoSpec);
};

class LEABRA_API MatrixLayerSpec : public LeabraLayerSpec {
  // basal ganglia matrix layer: fire actions/WM updates, or nogo; MAINT = gate in 1+ and 2+, OUTPUT = gate in -
public:
  enum 	BGType {       		// which type of basal ganglia circuit is this?
    OUTPUT,			// #AKA_MOTOR matrix that does output gating: controls access of frontal activations to other areas (e.g., motor output, or output of maintained PFC information)
    MAINT			// #AKA_PFC matrix that does maintenance gating: controls toggling of maintenance of activity patterns (e.g., PFC) over time
  };

  BGType		bg_type;	// type of basal ganglia/frontal system: output gating (e.g., motor) or maintenance gating (e.g., pfc)
  MatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  ContrastSpec 	 	contrast;	// contrast enhancement effects of da/dopamine neuromodulation
  MatrixRndGoSpec	rnd_go;		// matrix random Go firing for unconditional and nogo firing stripes cases
  MatrixErrRndGoSpec	err_rnd_go;	// matrix random Go firing to encourage exploration when (a series of) errors are made: chooses stripe to Go at random using probabilities from a softmax over snrthal netinput values: stripes that are closer to firing fire more often
  MatrixAvgDaRndGoSpec	avgda_rnd_go;	// matrix random Go firing based on average da to encourage exploration for non-productive stripes based on a softmax over the avg_go_da for that stripe (matrix_u->misc_1) 

  virtual bool 	Check_RndGoAvgRew(LeabraLayer* lay, LeabraNetwork* net);
  // check avg_rew levels to see whether we should compute random go (don't do when avg_rew is good!); false = don't do rnd go, true = do it
  virtual void 	Compute_UCondNoGoRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // compute random Go for unconditional and nogo cases
  virtual void 	Compute_ErrRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // compute random Go signal when errors have been made recently
  virtual void 	Compute_AvgDaRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // compute random Go signal based on average da values for stripes 
  virtual void 	Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork* net);
  // clear the rnd go signal

  virtual void 	Compute_DaModUnit_NoContrast(DaModUnit* u, float dav, int go_no);
  // apply given dopamine modulation value to the unit, based on whether it is a go (0) or nogo (1); no contrast enancement based on activation
  virtual void 	Compute_DaModUnit_Contrast(DaModUnit* u, float dav, float gating_act, int go_no);
  // apply given dopamine modulation value to the unit, based on whether it is a go (0) or nogo (1); contrast enhancement based on activation (gating_act)
  virtual void 	Compute_DaTonicMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraInhib* thr, LeabraNetwork* net);
  // compute tonic da modulation (for pfc gating units in first two phases)
  virtual void 	Compute_DaPerfMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraInhib* thr, LeabraNetwork* net);
  // compute dynamic da modulation; performance modulation, not learning (second minus phase)
  virtual void 	Compute_DaLearnMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraInhib* thr, LeabraNetwork* net);
  // compute dynamic da modulation: evaluation modulation, which is sensitive to GO/NOGO firing and activation in action phase
  virtual void 	Compute_AvgGoDa(LeabraLayer* lay, LeabraNetwork* net);
  // compute average da present when stripes fire a Go (stored in u->misc_1); used to modulate rnd_go firing
  virtual void 	Compute_MotorGate(LeabraLayer* lay, LeabraNetwork* net);
  // compute gating signal for OUTPUT Matrix_out

  void	Init_Weights(LeabraLayer* lay);
  void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void	PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both=false);
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(MatrixLayerSpec);
  COPY_FUNS(MatrixLayerSpec, LeabraLayerSpec);
  TA_BASEFUNS(MatrixLayerSpec);
};

//////////////////////////////////////////////////////////////////
//	  SNrThalLayer: Integrate Matrix and compute Gating 	//
//////////////////////////////////////////////////////////////////

class SNrThalMiscSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER misc specs for the snrthal layer
public:
  float		avg_net_dt;	// #DEF_0.005 time-averaged netinput computation integration rate
  float		go_thr;		// #DEF_0.1 threshold in snrthal activation required to trigger a Go gating event
  float		rnd_go_inc;	// #DEF_0.2 how much to add to the net input for a random-go signal triggered in corresponding matrix layer?

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SNrThalMiscSpec);
  COPY_FUNS(SNrThalMiscSpec, taBase);
  TA_BASEFUNS(SNrThalMiscSpec);
};

class LEABRA_API SNrThalLayerSpec : public LeabraLayerSpec {
  // computes activation = GO - NOGO from MatrixLayerSpec
public:
  SNrThalMiscSpec	snrthal; // misc specs for snrthal layer

  virtual void	Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net);
  // compute netinput as GO - NOGO on matrix layer

  void 	Compute_Clamp_NetAvg(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt(LeabraLayer*, LeabraNetwork*);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(SNrThalLayerSpec);
  COPY_FUNS(SNrThalLayerSpec, LeabraLayerSpec);
  TA_BASEFUNS(SNrThalLayerSpec);
};

//////////////////////////////////////////
//	PFC Layer Spec	(Maintenance)	//
//////////////////////////////////////////

class LEABRA_API PFCGateSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER gating specifications for basal ganglia gating of PFC maintenance layer
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
    UCOND_RND_GO,		// unconditional random go: just fire random go with a given probability, 
    NOGO_RND_GO,		// random go for stripes constantly firing nogo
    ERR_RND_GO,			// random go when an error has just been made: explore on error (ACC/LC?)
    AVGDA_RND_GO		// random go for stripes with consistently low average dopamine levels (under performers)
  };

  float		off_accom;	// #DEF_0 how much of the maintenance current to apply to accommodation after turning a unit off
  bool		updt_reset_sd;	// #DEF_true reset synaptic depression when units are updated
  bool		allow_clamp;	// #DEF_false allow external hard clamp of layer (e.g., for testing)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(PFCGateSpec);
  COPY_FUNS(PFCGateSpec, taBase);
  TA_BASEFUNS(PFCGateSpec);
};

class LEABRA_API PFCLayerSpec : public LeabraLayerSpec {
  // Prefrontal cortex layer: gets gating signal from SNrThal, gate updates before plus and 2nd plus (update) phase (toggle off, toggle on)
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

  virtual void 	ResetSynDep(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  // reset synaptic depression for sending cons from unit that was just toggled off in plus phase 1
  virtual void 	Compute_MaintUpdt(LeabraUnit_Group* ugp, MaintUpdtAct updt_act, LeabraLayer* lay, LeabraNetwork* net);
  // update maintenance state variables (gc.h, misc_1) based on given action
  virtual void	SendGateStates(LeabraLayer* lay, LeabraNetwork* net);
  // send misc_state gating state variables to the snrthal and matrix layers
  virtual void 	Compute_TmpClear(LeabraLayer* lay, LeabraNetwork* net);
  // temporarily clear the maintenance of pfc units to prepare way for transient acts
  virtual void 	Compute_GatingGOGO(LeabraLayer* lay, LeabraNetwork* net);
  // compute the gating signal based on SNrThal layer: GOGO model

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void	PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both=false);
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  TA_BASEFUNS(PFCLayerSpec);
};

//////////////////////////////////////////
//	PFC Layer Spec	(Output)	//
//////////////////////////////////////////

class LEABRA_API PFCOutGateSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER specifications for pfc output gating
public:
  float		base_gain;	// #DEF_0.5 how much activation gets through even without a Go gating signal
  float		go_gain;	// #DEF_0.5 how much extra to add for a Go signal
  bool		graded_go;	// #DEF_false use a graded Go signal as a function of strength of corresponding SNrThal unit?

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(PFCOutGateSpec);
  COPY_FUNS(PFCOutGateSpec, taBase);
  TA_BASEFUNS(PFCOutGateSpec);
};

class LEABRA_API PFCOutLayerSpec : public LeabraLayerSpec {
  // Prefrontal cortex output gated layer: gets gating signal from SNrThal and activations from PFC_mnt layer: gating modulates strength of activation
public:
  PFCOutGateSpec out_gate;		// parameters controlling the output gating of pfc units

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);

  void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	InitLinks();
  TA_BASEFUNS(PFCOutLayerSpec);
};

#endif // leabra_pbwm_h
