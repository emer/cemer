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
#include "netstru_extra.h" // NOTE

#ifndef leabra_pbwm_h
#define leabra_pbwm_h

// prefrontal-cortex basal ganglia working memory (PBWM) extensions to leabra

// based on the pvlv reinforcement learning mechanism
// this file defines BG + PFC gating/active maintenance mechanisms

class LEABRA_API PBWMUnGpData : public LeabraUnGpData {
  // PBWM version of data to maintain for independent unit groups of competing units within a single layer -- contains extra information for PBWM state
INHERITED(LeabraUnGpData)
public:
  bool		go_fired_now;	// #CAT_Activation has Go fired on this cycle?  only true for one cycle
  bool		go_fired_trial;	// #CAT_Activation has Go fired on this trial -- false until it fires
  bool		go_forced;	// #CAT_Activation no other stripe had fired Go within the appropriate gating interval, so Go firing for this stripe was forced
  int		go_cycle;	// #CAT_Activation cycle on which Go fired, if it did on this trial (-1 if no Go)
  int		mnt_count;	// #CAT_Activation current counter of number of trials in maint or empty state -- 0 = gating just happened -- will only be true on trial when go_fired_trial is true -- 1+ = maint for a trial or a more (increments at start of each trial, 1 = trial right after gating, etc) -- -1- = empty for a trial or more (also increments at start of each trial)
  int		prv_mnt_count;	// #CAT_Activation previous counter of number of trials in maint or empty state, just prior to last gating event (only updated at time of gating) -- enables determination of whether a current Go signal was to a maintaining stripe (go_fired_trial && prv_mnt_count > 0) or an empty stripe (otherwise)

  override void	Init_State();

  virtual void	CopyPBWMData(const PBWMUnGpData& cp);
  // copy just the pbwm data

  void	Copy_(const PBWMUnGpData& cp);
  TA_BASEFUNS(PBWMUnGpData);
private:
  void	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////////////////////////////
//	  SNrThalLayer: Integrate Matrix and compute Gating 	//
//////////////////////////////////////////////////////////////////

class LEABRA_API SNrThalMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the snrthal layer
INHERITED(SpecMemberBase)
public:
  float		go_thr;			// #DEF_0.5 threshold on activity to fire go -- only stripes that get this active before the mid minus cycle will actually fire
  int		gate_cycle; 		// cycle to compute gating signal on -- this is a fixed cycle count, which should be delayed enough to allow striatum to receive all the information it needs and neurons to get sufficiently activated, but also enough before the end of the minus phase to allow deep layer activations to produce whatever effects they might have
  bool		force;			// if nobody has fired by min_go_cycle, pick top unit by netin value (or at random if all netin == 0) to fire go
  
  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(SNrThalMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API SNrThalLayerSpec : public LeabraLayerSpec {
  // Represents the substantia nigra, pars reticulata (SNr) and Thalamus (MD) circuits that project from basal ganglia up to frontal cortex -- activation is directly computed from matrix -- all nogo enters into matrix activations, not snrthal -- gating val reflected in act_m2, gating status in unit group data per stripe
INHERITED(LeabraLayerSpec)
public:
  enum GatingTypes {		// #BITS types of gating stripes present, for INPUT, MAINT, OUTPUT gating -- used for coordinating structure of network (projections mostly) -- all gating is functionally identical
    NO_GATE_TYPE = 0x00,	// #NO_BIT no type set
    INPUT = 0x01,		// Gating of input to PFC_in layers -- if active, these are first units in Matrix and SNrThal layers (TODO: not well supported yet)
    MAINT = 0x02,		// Gating of maintenance in PFC_mnt layers -- if active, these are next units in Matrix and SNrThal layers
    OUTPUT = 0x04,		// Gating of output in PFC_out layers -- if active, these are last units in Matrix and SNrThal layers
#ifndef __MAKETA__
    IN_MNT_OUT = INPUT | MAINT | OUTPUT,// #NO_BIT input maint and output -- typical default
#endif
  };

  GatingTypes		gating_types;	// types of gating units present within this SNrThal layer -- used for coordinating structure of network (projections mostly) -- snrthal is the official "source" of this setting, which is copied to associated matrix and pfc layers during config check
  int			n_in_out_stripes; // number of input/output stripes -- maint stripes MUST be some multiple of this number
  SNrThalMiscSpec	snrthal;    	// misc specs for snrthal layer

  virtual void	Init_GateStats(LeabraLayer* lay, LeabraNetwork* net);
  // initialize the gating stats in the group data -- called by Trial_Init_Layer
  virtual void	Compute_GateActs(LeabraLayer* lay, LeabraNetwork* net);
  // compute gating activations -- called at gate_cycle
  virtual void	Compute_GateStats(LeabraLayer* lay, LeabraNetwork* net);
  // update layer user data gating statistics which are useful to monitor for overall performance -- called at gate_cycle
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  override void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }

  virtual void	GatingTypesNStripes(LeabraLayer* lay, int& n_in, int& n_mnt, int& n_out);
  // get the number of stripes associated with each gating type, based on matrix projections into the snrthal layer -- also updates the gating_types to reflect actual connectivity
  virtual int  SNrThalStartIdx(LeabraLayer* lay, GatingTypes gating_type, 
			       int& n_in, int& n_mnt, int& n_out);
  // returns the starting index for a given gating type within the SNrThal, and also returns the number of each type of stripe.  returns -1 if snrthal does not have that kind of stripe

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
//	  Matrix Con/Units

class MatrixCon : public LeabraCon {
  // connection for recv cons into matrix units -- stores the sender activation prior to any gating update signals, used for learning
public:
  float		sact_lrn;	// #NO_SAVE sending activation value used for learning -- prior to any gating-based update in activation states

  MatrixCon() { sact_lrn = 0.0f; }
};

class LEABRA_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on dopamine modulation of activation -- for Matrix_Go connections only -- use MatrixNoGoConSpec for NoGo pathway
INHERITED(LeabraConSpec)
public:
  float		err_gain;	// how much error-driven learning should be added to non-maintenance gating unit learning?

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

  inline void C_Compute_dWt_Matrix_Mnt(LeabraCon* cn, float lin_wt, 
				       float mtx_act_m2, float mtx_da, float su_act_lrn) {
    float sr_prod = mtx_act_m2 * su_act_lrn;
    float dwt = mtx_da * sr_prod;
    if(lmix.err_sb) {
      if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
      else		dwt *= lin_wt;
    }
    cn->dwt += cur_lrate * dwt;
  }

  inline void C_Compute_dWt_Matrix_Err(LeabraCon* cn, float lin_wt, 
		       float mtx_act_m2, float mtx_da, float su_act_lrn,
		       float mtx_act_m, float mtx_act_p, float su_act_m, float su_act_p) {
    float sr_prod = mtx_act_m2 * su_act_lrn;
    float err = err_gain * (mtx_act_p * su_act_p - mtx_act_m * su_act_m);
    float dwt = mtx_da * sr_prod + err;
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
      if(ru->dav == 0.0f) continue; // if dav == 0 then was not gated!  in any case, dwt = 0
      if(err_gain > 0.0f && ru->name[0] != 'm') { 
	C_Compute_dWt_Matrix_Err(cn, LinFmSigWt(cn->wt), ru->act_m2, ru->dav, cn->sact_lrn,
				 ru->act_m, ru->act_p, su->act_m, su->act_p);
      }
      else {
	C_Compute_dWt_Matrix_Mnt(cn, LinFmSigWt(cn->wt), ru->act_m2, ru->dav, cn->sact_lrn);
	// note: using cn->sact_lrn as having saved sending activation in Compute_MidMinusAct
      }
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

class LEABRA_API MatrixNoGoConSpec : public LeabraConSpec {
  // Learning of Matrix_NoGo pathway input connections based on dopamine modulation of activation -- learns all the time based on minus-phase activations
INHERITED(LeabraConSpec)
public:

  inline override void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
    // do NOT do this under any circumstances!!
  }

  inline void C_Compute_dWt_Matrix(LeabraCon* cn, float lin_wt, 
				   float mtx_act_m, float mtx_da, float su_act_m) {
    float sr_prod = mtx_act_m * su_act_m;
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
      C_Compute_dWt_Matrix(cn, LinFmSigWt(cn->wt), ru->act_m, ru->dav, su->act_m);
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

  TA_SIMPLE_BASEFUNS(MatrixNoGoConSpec);
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

class LEABRA_API MatrixUnitSpec : public LeabraUnitSpec {
  // basal ganglia matrix units: fire actions or WM updates. modulated by da signals
INHERITED(LeabraUnitSpec)
public:
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

class LEABRA_API MatrixMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for the matrix layer
INHERITED(SpecMemberBase)
public:
  float		nogo_inhib;	// #DEF_0.2 #MIN_0 how strongly does the nogo stripe inhibit the go stripe -- nogo stripe average activation times this factor adds directly to gc.i inhibition in go units
  float		refract_inhib;	// #MIN_0 amount of refractory inhibition to apply to Go units for stripes that are in maintenance mode for one trial
  float		da_gain;	// #DEF_0:2 #MIN_0 overall gain for da modulation of matrix units for the purposes of learning (ONLY) -- bias da is set directly by gate_bias params -- also, this value is in addition to other "upstream" gain parameters, such as vta.da.gain -- it is recommended that you leave those upstream parameters at 1.0 and adjust this parameter, as it also modulates rnd_go.nogo.da which is appropriate
  float		go_pfc_thr;	// #DEF_0:0.02 threshold on average activity within the corresponding PFC_s stripe before a Go pathway stripe can fire -- requires that matrix recv from PFC_s in first place -- avoids having to implement this constraint in connectivity

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixMiscSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };
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

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixGoNogoGainSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };
};


class LEABRA_API MatrixLayerSpec : public LeabraLayerSpec {
  // basal ganglia matrix layer -- one for Go pathway and another for NoGo pathway -- Go recv marker con from NoGo, both recv from SNrThal to get final go signal
INHERITED(LeabraLayerSpec)
public:
  enum GoNoGo {
    GO,				// Go or direct pathway
    NOGO,			// NoGo or indirect pathway
  };

  GoNoGo		go_nogo; 	// is this a Go pathway or a NoGo pathway layer
  SNrThalLayerSpec::GatingTypes		gating_type;	// type of gating units present within this Matrix layer -- must be just one of the options (INPUT, MAINT, OUTPUT)
  MatrixMiscSpec 	matrix;		// misc parameters for the matrix layer
  MatrixGoNogoGainSpec  go_nogo_gain;	// separate Go and NoGo DA gain parameters for matrix units -- mainly for simulating various drug effects, etc

  virtual LeabraLayer* 	SNrThalLayer(LeabraLayer* lay);
  // find the SNrThal layer that this matrix layer interacts with
  virtual LeabraLayer* 	PVLVDaLayer(LeabraLayer* lay);
  // find the PVLVDaLayerSpec layer that this matrix layer interacts with

  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  virtual void Compute_MidMinusAct_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       LeabraNetwork* net);
  // save the effective mid-minus (gating) activation state for subsequent learning -- for specific unit group (stripe)
  virtual void Compute_ZeroMidMinusAct_ugp(LeabraLayer* lay,
				       Layer::AccessMode acc_md, int gpidx,
				       LeabraNetwork* net);
  // set the effective mid-minus (gating) activation state to zero for subsequent learning -- for specific unit group (stripe) -- for stripes that did not gate at all this time around (enforces strong credit assignment, for display purposes mostly as dopamine usually does the credit assignment)

  virtual float	Compute_NoGoInhibGo(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				    LeabraNetwork* net);
  // compute nogo layer stripes inhibition onto go layer units -- called in ApplyInhib

  override void Compute_ApplyInhib_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
					LeabraInhib* thr, LeabraNetwork* net);

  virtual void	Compute_GatingActs_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
				       int gpidx, LeabraNetwork* net);
  // save activations into act_m2 at point of gating
  virtual void Compute_ClearActAfterGo(LeabraLayer* lay, Layer::AccessMode acc_md,
				  int gpidx, LeabraNetwork* net);
  // generally used only if doublegate.on --- to clear activity of stripes that fired Go in early gating window so others can fire in the late  
  virtual void Compute_ClearActAfterGo_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
				      int gpidx, LeabraNetwork* net);

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

  virtual void 	Compute_LearnDaVal(LeabraLayer* lay, LeabraNetwork* net);
  // compute u->dav learning dopamine value based on raw dav and gating state, etc -- this dav is then directly used in conspec learning rule

  virtual  void NameMatrixUnits(LeabraLayer* lay, LeabraNetwork* net);
  // name the matrix units according to their functional role -- i = input, m = maint, o = output -- these names are used to support different learning rules for these different types

  override void	Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return true; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
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

//////////////////////////////////////////
//	PFC Layer Spec (Maintenance)	//
//////////////////////////////////////////

class LEABRA_API PFCsUnitSpec : public LeabraUnitSpec {
  // superficial layer PFC unit spec -- supports modulation of learning as function of gating (resets dwts during dwt_norm for those that shouldn't learn)
INHERITED(LeabraUnitSpec)
public:

  virtual void	Compute_LearnMod(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // main routine that zeros out dwt for any units that should not learn on this trial -- finds the pfc deep layer and drives everything from there
  override void  Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net);
  override void	Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);

  TA_SIMPLE_BASEFUNS(PFCsUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

class LEABRA_API PFCGateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gating specifications for basal ganglia gating of PFC maintenance layer
INHERITED(SpecMemberBase)
public:
  bool		learn_deep_act;	// #DEF_true superficial layer PFC units only learn when corresponding deep pfc layers are active (i.e., have been gated) -- they must use a PFCsUnitSpec to support this learning modulation
  int		in_mnt;		// #DEF_1 #MIN_0 how many trials INPUT layers maintain after initial gating trial
  int		out_mnt;	// #DEF_1 #MIN_0 how many trials OUTPUT layers maintain after initial gating trial
  float		maint_decay;	// #MIN_0 #MAX_1 #DEF_0:0.05 how much maintenance activation decays every trial
  float		maint_thr;	// #DEF_0.2 #MIN_0 when max activity in layer falls below this threshold, activations are no longer maintained and stripe is cleared

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

class LEABRA_API PFCLayerSpec : public LeabraLayerSpec {
  // #AKA_PFCDeepLayerSpec Prefrontal cortex layer -- deep and superficial types
INHERITED(LeabraLayerSpec)
public:
  enum MaintUpdtAct {
    STORE,			// store current activity state in maintenance currents
    CLEAR,			// clear current activity state from maintenance currents
    DECAY,			// decay current maintenance currents
  };

  enum PFCLayer {
    SUPER,			// superficial layer -- activations are labile until trial after gating event, when they get locked into maintenance for at least one trial
    DEEP,			// deep layer -- not active at all until the trial when gating occurs -- after gating during that trial they track superficial, and then after that are locked into maintenance 
  };

  SNrThalLayerSpec::GatingTypes	pfc_type;	// type of pfc units present within this PFC layer -- must be just one of the options (INPUT, MAINT, OUTPUT)
  PFCLayer	pfc_layer;	// which layer type is this -- superficial (SUPER) or deep (DEEP)?
  PFCGateSpec	gate;		// parameters controlling the gating of pfc units

  virtual LeabraLayer* 	DeepLayer(LeabraLayer* lay);
  // find the DEEP layer for this SUPER layer
  virtual LeabraLayer* 	SNrThalLayer(LeabraLayer* lay);
  // find the SNrThal layer that this pfc deep layer receives from
  virtual LeabraLayer* 	LVeLayer(LeabraLayer* lay);
  // find the LVe layer that this pfc deep layer projects to
  virtual LeabraLayer* 	LViLayer(LeabraLayer* lay);
  // find the LVi layer that this pfc deep layer projects to

  virtual LeabraLayer*  SNrThalStartIdx(LeabraLayer* lay, int& snr_st_idx,
					int& n_in, int& n_mnt, int& n_out);
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

  // don't do any learning:
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net)
  { return false; }

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


//////////////////////////////////////////
//	    Special PrjnSpecs	 	//
//////////////////////////////////////////

class LEABRA_API GpCustomPrjnSpecBase : public ProjectionSpec {
  // #VIRT_BASE basic custom group-level projection spec -- core methods for connecting groups
INHERITED(ProjectionSpec)
public:
  virtual void AllocGp_Recv(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			    Layer::AccessMode sacc_md, int n_send);
  // allocate recv cons for given recv gp, for given number of sending groups
  virtual void AllocGp_Send(Projection* prjn, Layer::AccessMode sacc_md, int sgpidx,
			    Layer::AccessMode racc_md, int n_recv);
  // allocate send cons for given send gp, for given number of recv groups
  virtual void Connect_Gp(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			  Layer::AccessMode sacc_md, int sgpidx,
			  int extra_rgp_alloc = 0, int extra_sgp_alloc = 0);
  // make a projection from all senders in sugp into all receivers in rugp -- if extra_*gp_alloc > 0 then alloc this many extra groups worth of [recv/send] for the [recv/send] -- use -1 for already allocated to prevent re-allocation

  TA_BASEFUNS_NOCOPY(GpCustomPrjnSpecBase);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};


class LEABRA_API PFCLVPrjnSpec : public GpCustomPrjnSpecBase {
  // A special projection spec for PFC to LVe/i layers. If n unit groups (stripes) in LV == PFC, then it makes Gp one-to-one projections; if LV stripes == 1, it makes a single full projection; if LV stripes == PFC + 1, the first projection is full and the subsequent are gp one-to-one; if recv fm multiple PFC layers, same logic applies to each
INHERITED(GpCustomPrjnSpecBase)
public:

  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(PFCLVPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API SNrPrjnSpec : public GpCustomPrjnSpecBase {
  // SNrThal projection -- automatically deals with the convergence and divergence of connectivity between gating-specific layers in either Matrix or PFC (INPUT, MAINT, OUTPUT) and the SNrThal which represents all gating types in one layer 
INHERITED(GpCustomPrjnSpecBase)
public:
  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(SNrPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

class LEABRA_API PVrToMatrixGoPrjnSpec : public ProjectionSpec {
  // Projection from PVLV PVr layer to Matrix_Go layer -- PVr=1 value unit (right-most) connects to Output Go units, while PVr=.5 value unit (middle) connects to Input and Maint Go units, providing a bias for output gating when rewards are expected
INHERITED(ProjectionSpec)
public:
  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(PVrToMatrixGoPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};


class EMERGENT_API TopoWtsPrjnSpec : public FullPrjnSpec {
  // #AKA_FullTopolGradWtsPrjnSpec full connectivity, with user-definable topologically-defined gradient of weight strengths (requires init_wts = true, otherwise is just like Full Prjn), where weights are strongest from sending units (unit groups) in the same topologically relative location as the receiving unit (unit groups), and fall off from there (either linearly or as a Gaussian; other functions can be added) -- if send and/or recv layer(s) have unit groups, then use_send_gps/use_recv_gps must be checked and it is the unit group position that counts -- all params then refer to un_gp position indices and all units within the send and/or recv group have the same connectivity (TODO: can this be overridden with use_recv_gps, use_send_grps flags? i.e., are these flags optional? depends on how the indexing works!!); subsets of units (un_gps) can also be designated for either sending layer, receiving layer, or both
INHERITED(FullPrjnSpec)
public:
  enum GradType {	// type of gradient to establish
    LINEAR,		// linear fall-off as a function of distance
    GAUSSIAN,		// gaussian fall-off as a function of distance
  };

  MinMaxRange	wt_range;	// #CONDSHOW_ON_init_wts range of weakest (min) to strongest (max) weight values generated

  bool		invert;			// #CONDSHOW_ON_init_wts invert the gradient, such that the min is located "below" the recv units, and the max is furthest away
  bool		grad_x;			// #CONDSHOW_ON_init_wts compute a gradient over the x dimension of the sending layer, based on x axis location of the send layer to x dimension of recv layer, unless grad_x_grad_y also true
  bool		grad_x_grad_y;		// #CONDSHOW_ON_grad_x map gradient from x dimension of send layer to y dimension of recv layer

  bool		grad_y;			// #CONDSHOW_ON_init_wts compute a gradient over the y dimension of the sending layer, based on y axis location of the send layer to y dimension of recv layer, unless grad_y_grad_x also
  bool		grad_y_grad_x;		// #CONDSHOW_ON_grad_y map gradient from y dimension of send layer to x dimension of recv layer

  bool		wrap;			// #CONDSHOW_ON_init_wts wrap weight values around relevant dimension(s) -- the closest location wins -- this ensures that all units have the same overall weight strengths
  bool		use_recv_gps;		// #CONDSHOW_ON_init_wts if recv layer has unit groups, use them for determining relative position to compare with sending unit locations (unit group information is not used for the sending layer unless use_send_gps also true)
  bool		use_send_gps;		// #CONDSHOW_ON_init_wts if send layer has unit groups, use them for determining relative position to compare with receiving unit locations (unit group information is not used for the receiving layer unless use_recv_gps also true)

  bool		custom_send_range;	// #CONDSHOW_ON_init_wts use custom (sub)range of send layer

  TwoDCoord 	send_range_start;	// #CONDSHOW_ON_custom_send_range start coords used in send layer (either unit groups or units depending on use_send_gps setting)
  TwoDCoord 	send_range_end;		// #CONDSHOW_ON_custom_send_range end coords used in send layer (either unit groups or units depending on use_send_gps setting) -- use -1 for last unit/group
  
  bool		custom_recv_range;	// #CONDSHOW_ON_init_wts use custom (sub)range of recv layer?

  TwoDCoord 	recv_range_start;	// #CONDSHOW_ON_custom_recv_range start coords used in recv layer (either unit groups or units depending on use_recv_gps setting)
  TwoDCoord 	recv_range_end;		// #CONDSHOW_ON_custom_recv_range end coords used in recv layer (either unit groups or units depending on use_recv_gps setting) -- use -1 for last unit/group
  
  GradType	grad_type;		// #CONDSHOW_ON_init_wts type of gradient to make -- applies to both axes
  float		gauss_sig;		// #CONDSHOW_ON_grad_type:GAUSSIAN gaussian sigma (width), in normalized units where entire distance across sending layer is 1.0

  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // calls one of the four InitWeights fns below according to use of unit groups
  override void Connect_impl(Projection* prjn);
  // enables flexibility whether to use unit group indexing (default) or not
  virtual void 	InitWeights_SendFlatRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru);
  // for flat recv layer case (just unit positions)
  virtual void	InitWeights_SendGpsRecvGps(Projection* prjn, RecvCons* cg, Unit* ru);
  // for both send and recv unit group case
  virtual void	InitWeights_SendGpsRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru);
  // for send unit group, flat recv case
  virtual void	InitWeights_SendFlatRecvGps(Projection* prjn, RecvCons* cg, Unit* ru);
  // for send flat, recv unit group case
  virtual void 	SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist, int cg_idx);
  // actually set the weight value from distance value -- util used by both of above main routines -- can overload to implement different gradient functions -- cg_idx is index within con group, and dist is computed normalized distance value (0-1)

  TA_SIMPLE_BASEFUNS(TopoWtsPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

#endif // leabra_pbwm_h
