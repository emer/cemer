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

#include "leabra_td.h"

#ifndef leabra_pvlv_h
#define leabra_pvlv_h

// main issues fixed in new version (5/07):
// * the syndep code is definitely not the way the brain works, and has dmem issues
//   as noted below (is this still relevant??)
// * the integration of pv&lv produces artifacts from lv firing during pv..
//   need to revisit.
// * separate detector of times when reward occurs PVr

//////////////////////////////////////////////////////////////////////////////////////////
//	Pavlovian (PVLV): Primary Value and Learned Value Reward Learning System	//
//////////////////////////////////////////////////////////////////////////////////////////

// PV primary value: learns continuously about primary rewards (present or absent)
//   PVe = excitatory: primary reward (ExtRew)
//   [PVr] = detector of time when rewards are present (fast to learn, slow to extinguish)
//   PVi = inhibitory: cancelling expected primary rewards
// LV learned value: learns only at the time of primary (expected) rewards, free to fire at time CS's come on
//   LVe = excitatory: rapidly learns excitatory CS assocs
//   LVi = inhibitory: slowly learns to cancel CS assocs (adaptive baseline for LVe)
// PVLVDa (VTA/SNc) computes DA signal as: IF PV present, PVe - PVi, else LVe - LVi
// delta version  (5/07) uses temporal derivative of LV signals instead of synaptic depression

//////////////////////////////////////////
//	PV: Primary Value Layer		//
//////////////////////////////////////////

// NOTE: the syndep in this code cannot be parallelized over dwt's because the dynamics will
// be different!!!
// need to have a dmem small_batch over sequences type mode that does a SyncWts using sum_dwts = false
// and calls Compute_Weights after each trial..
// problem is that this does averaging of dwts;  one soln is to keep the
// orig wt from last wt change, apply dwt every trial, and then subtract wt - origwt ->dwt
// aggretate these dwts as a SUM, apply to wts!

class LEABRA_API PVConSpec : public LeabraConSpec {
  // primary value connection spec: learns using delta rule from PVe - PVi values -- does not use hebb or err_sb parameters
INHERITED(LeabraConSpec)
public:
  inline float C_Compute_Err_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    return (ru->act_p - ru->act_m) * su->act_p; // basic delta rule
    // note: no err_sb
  }

  inline override void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_dWt_NoHebb(cn, ru, C_Compute_Err_Delta(cn, ru, su));  
	}
      }
    }
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_dWt_NoHebb(cn, ru, C_Compute_Err_Delta(cn, ru, su));  
	}
      }
    }
  }

  TA_SIMPLE_BASEFUNS(PVConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API PVDetectSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for detecting if a primary value is present or expected
INHERITED(taOBase)
public:
  float		thr_min;	// #DEF_0.2 minimum threshold on PVe (ExtRew) or PVi, below which PV is considered present (i.e., punishment) (set to 0 if PVe.rew.norew_val = 0)
  float		thr_max;	// #DEF_0.8 maximum threshold on PVe (ExtRew) or PVi, above which PV is considered present (i.e., reward) (set to .4 if PVe.rew.norew_val = 0)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PVDetectSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API PViLayerSpec : public ScalarValLayerSpec {
  // primary value inhibitory (PVi) layer: continously learns to expect primary reward values
INHERITED(ScalarValLayerSpec)
public:
  PVDetectSpec	pv_detect;	// primary reward value detection spec: detect if a primary reward is present or expected

  virtual float	Compute_PVe(LeabraLayer* lay, LeabraNetwork* net, bool& actual_er_avail, bool& pv_detected);
  // get excitatory primary reward value from PVe/ExtRew layer if it is available: actual_er_avail = internal flag from ExtRew layer (cheating), pv_detected = PVi layer's detection of primary reward presence or expectation based on PVi, PVe values (not cheating)
  virtual void 	Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork* net);
  // copy ext values to act_p
  virtual void 	Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as external rewards and change weights
//   virtual void 	Update_PVPrior_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp, bool er_avail);
//   // update the prior PV value, stored in pv unit misc_1 values
  virtual void	Update_PVPrior(LeabraLayer* lay, bool er_avail, float pve_val);
  // update the prior PV value, stored in pv unit misc_1 values

  // overrides:
  void	Compute_SRAvg(LeabraLayer*, LeabraNetwork*) { };
  void	Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PViLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////
//	PVr = PV reward detection system (habenula?)
////////////////////////////////////////////////////////

class LEABRA_API PVrConSpec : public PVConSpec {
  // primary value connection spec with asymmetrical learning rates -- used for reward detection connections
INHERITED(PVConSpec)
public:
  float 	wt_dec_mult;   // multiplier for weight decrease rate relative to basic lrate used for weight increases

  inline float C_Compute_Err_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->act_p; // basic delta rule
    if(err < 0.0f)	err *= wt_dec_mult;
    // note: no err_sb
    return err;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_dWt_NoHebb(cn, ru, C_Compute_Err_Delta(cn, ru, su));  
	}
      }
    }
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_dWt_NoHebb(cn, ru, C_Compute_Err_Delta(cn, ru, su));  
	}
      }
    }
  }

  TA_SIMPLE_BASEFUNS(PVrConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API PVrLayerSpec : public PViLayerSpec {
  // primary value reward detection layer: learns when rewards are expected to occur (just for marking purposes -- same functionality as PVi)
INHERITED(PViLayerSpec)
public:
  TA_BASEFUNS_NOCOPY(PVrLayerSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};


//////////////////////////////////////////
//	LV: Learned Value Layer		//
//////////////////////////////////////////

class LEABRA_API LVConSpec : public TrialSynDepConSpec {
  // learned value connection spec: learns using delta rule from PVe - LV values; also does synaptic depression to do novelty filtering
INHERITED(TrialSynDepConSpec)
public:

  inline float C_Compute_Err_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    return (ru->act_p - ru->act_m) * su->act_p; // basic delta rule
    // note: no err_sb
  }

  inline override void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_dWt_NoHebb(cn, ru, C_Compute_Err_Delta(cn, ru, su));  
	  C_Depress_Wt((TrialSynDepCon*)cn, ru, su);
	}
      }
    }
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_dWt_NoHebb(cn, ru, C_Compute_Err_Delta(cn, ru, su));  
	  C_Depress_Wt((TrialSynDepCon*)cn, ru, su);
	}
      }
    }
  }

  TA_SIMPLE_BASEFUNS(LVConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API LVSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for learned value layers
INHERITED(taOBase)
public:
  bool		delta_on_sum;	// #DEF_false if there are multiple lv subgroups, compute the temporal delta on the summed lv values (else deltas are per each sub-group, then summed)
  bool		use_actual_er;	// #DEF_false use actual external reward presence to determine when to learn (cheating), otherwise use PVi's estimate of when primary value is avail (more realistic)
  float		min_lvi;	// minimum effective lvi value, for computing lv da
  bool		syn_dep;	// #DEF_false #APPLY_IMMED use the old synaptic depression version of LV

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(LVSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LVeLayerSpec : public ScalarValLayerSpec {
  // learns value based on inputs that are associated with rewards, only learns at time of primary rewards (filtered by PV system). This is excitatory version
INHERITED(ScalarValLayerSpec)
public:
  LVSpec	lv;	// learned value specs

  virtual void 	Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork* net);
  // copy ext values to act_p
  virtual void 	Compute_DepressWt(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
  // depress weights for units in unit group (only if not doing dwts!)
  virtual void 	Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // if primary value detected (present/expected), compute plus phase activations for learning, and actually change weights; otherwise just depress weights

  virtual float	Compute_ActEqAvg(LeabraLayer* lay);
  // compute average over value representation subgroups of act_eq values
  virtual float Compute_LVDa_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp);
  // compute da contribution from Lv, based on lve_layer and lvi_layer activations (multiple subgroups allowed)
  virtual float	Compute_LVDa(LeabraLayer* lve_lay, LeabraLayer* lvi_lay);
  // compute da contribution from Lv, based on lve_layer and lvi_layer activations (multiple subgroups allowed)
  virtual void 	Update_LVPrior_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp, bool er_avail);
  // update the prior Lv value, stored in lv unit misc_1 values
  virtual void	Update_LVPrior(LeabraLayer* lve_lay, LeabraLayer* lvi_lay, bool er_avail);
  // update the prior Lv value, stored in lv unit misc_1 values

  void	Compute_SRAvg(LeabraLayer*, LeabraNetwork*) { };
  void	Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(LVeLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API LViLayerSpec : public LVeLayerSpec {
  // inhibitory/slow version of LV layer spec: (just a marker for layer; same functionality as LVeLayerSpec)
INHERITED(LVeLayerSpec)
public:

  TA_BASEFUNS_NOCOPY(LViLayerSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};

//////////////////////////////////////////
//	  Novelty Value Layer (NV)	//
//////////////////////////////////////////

class LEABRA_API NVSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for novelty value learning
INHERITED(taOBase)
public:
  float		da_gain;	// #DEF_1 gain for novelty value dopamine signal
  float		val_thr;	// #DEF_0.1 threshold for value (training value is 0) -- value is zero below this threshold

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(NVSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API NVLayerSpec : public ScalarValLayerSpec {
  // novelty value (NV) layer: starts with a bias of 1.0, and learns to activate 0.0 value -- value signal is how novel the stimulus is
INHERITED(ScalarValLayerSpec)
public:
  NVSpec	nv;	// novelty value specs

  virtual float	Compute_NVDa(LeabraLayer* lay);
  // compute novelty value da value
  virtual void 	Compute_NVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as train target value and change weights
  virtual void	Update_NVPrior(LeabraLayer* lay, bool er_avail);
  // update the prior Nv value, stored in nv unit misc_1 values

  virtual void 	Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork* net);
  // copy ext values to act_p

  void	Compute_SRAvg(LeabraLayer*, LeabraNetwork*) { };
  void	Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_FirstPlus(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_SecondPlus(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt_Nothing(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(NVLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////
//	  DaLayer 	//
//////////////////////////

class LEABRA_API PVLVDaSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for PVLV da parameters
INHERITED(taOBase)
public:
  float		da_gain;	// #DEF_1 multiplier for dopamine values
  float		tonic_da;	// #DEF_0 set a tonic 'dopamine' (DA) level (offset to add to da values)
  float		min_pvi;	// minimum PVi value, so that a low PVe value (~0) makes for negative DA regardless of pvi learning: DA_pv = PVe - MAX(PVi, min_pvi) -- not that beneficial for PBWM gating, but can be useful for motor learning (e.g., .4)
  bool		use_actual_er;	// #DEF_false use actual external reward presence to determine when PV is detected (cheating), otherwise use PVi's estimate of when primary value is avail (more realistic)
  bool		syn_dep;	// #DEF_false old synaptic depression-based mechanism: note that this uses LV_PLUS_IF_PV mode automatically (and otherwise lv_delta mode uses IV_PV_ELSE_LV)
  float		min_lvi;	// #CONDEDIT_ON_syn_dep #DEF_0.1 minimum LVi value, so that a low LVe value (~0) makes for negative DA: DA_lv = LVe - MAX(LVi, min_lvi)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PVLVDaSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API PVLVDaLayerSpec : public LeabraLayerSpec {
  // computes PVLV 'Da' signal: typically if(ER), da = ER-PV, else LVe - LVs
INHERITED(LeabraLayerSpec)
public:
  PVLVDaSpec	da;		// parameters for the lvpv da computation

  virtual void	Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute a zero da value: in minus phase -- not used!
  virtual void	Send_Da(LeabraLayer* lay, LeabraNetwork* net);
  // send the da value to sending projections: every cycle

  virtual void	Compute_Da_LvDelta(LeabraLayer* lay, LeabraNetwork* net);
  // compute the da value based on recv projections: every cycle in 1+ phases (delta version)
  virtual void	Update_LvDelta(LeabraLayer* lay, LeabraNetwork* net);
  // update the LV

  virtual void	Compute_Da_SynDep(LeabraLayer* lay, LeabraNetwork* net);
  // compute the da value based on recv projections: every cycle in 1+ phases (synaptic depression version)

  // overrides:
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_SRAvg(LeabraLayer*, LeabraNetwork*) { };
  void	Compute_dWt_impl(LeabraLayer*, LeabraNetwork*) { };
  void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PVLVDaLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#endif // leabra_pvlv_h
