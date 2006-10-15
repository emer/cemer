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

#include "leabra_td.h"

#ifndef leabra_pvlv_h
#define leabra_pvlv_h

// todo:
// * the syndep code is definitely not the way the brain works, and has dmem issues
//   as noted below (is this still relevant??)
// * the integration of pv&lv produces artifacts from lv firing during pv..
//   need to revisit.


//////////////////////////////////////////////////////////////////////////////////////////
//	Pavlovian (PVLV): Primary Value and Learned Value Reward Learning System	//
//////////////////////////////////////////////////////////////////////////////////////////

// PV primary value: learns continuously about primary rewards (present or absent)
//   PVe = excitatory: primary reward (ExtRew)
//   PVi = inhibitory: cancelling expected primary rewards
// LV learned value: learns only at the time of primary (expected) rewards, free to fire at time CS's come on
//   LVe = excitatory: rapidly learns excitatory CS assocs
//   LVi = inhibitory: slowly learns to cancel CS assocs (adaptive baseline for LVe)
// PVLVDa (VTA/SNc) computes DA signal as: IF PV present, PVe - PVi, else LVe - LVi

//////////////////////////////////////////
//	PV: Primary Value Layer		//
//////////////////////////////////////////

// TODO: the syndep in this code cannot be parallelized over dwt's because the dynamics will
// be different!!!
// need to have a dmem small_batch over sequences type mode that does a SyncWts using sum_dwts = false
// and calls UpdateWeights after each trial..
// problem is that this does averaging of dwts;  one soln is to keep the
// orig wt from last wt change, apply dwt every trial, and then subtract wt - origwt ->dwt
// aggretate these dwts as a SUM, apply to wts!

class LEABRA_API PVConSpec : public LeabraConSpec {
  // primary value connection spec: learns using delta rule from PVe - PVi values
public:
  inline float C_Compute_Err(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->act_p;
    // wt is negative in linear form, so using opposite sign of usual here
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f + cn->wt);
      else		err *= -cn->wt;	
    }
    return err;
  }

  // this computes weight changes based on sender at time t-1
  inline void Compute_dWt(Con_Group* cg, Unit* ru) {
    LeabraUnit* lru = (LeabraUnit*)ru;
    LeabraCon_Group* lcg = (LeabraCon_Group*) cg;
    Compute_SAvgCor(lcg, lru);
    if(((LeabraLayer*)cg->prjn->from)->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  float orig_wt = cn->wt;
	  C_Compute_LinFmWt(lcg, cn); // get weight into linear form
	  C_Compute_dWt(cn, lru, 
			C_Compute_Hebb(cn, lcg, lru->act_p, su->act_p),
			C_Compute_Err(cn, lru, su));  
	  cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
	}
      }
    }
  }

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(PVConSpec);
  COPY_FUNS(PVConSpec, LeabraConSpec);
  TA_BASEFUNS(PVConSpec);
};

class LEABRA_API PVDetectSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER specs for detecting if a primary value is present or expected
public:
  float		thr_min;	// #DEF_0.2 minimum threshold on PVe (ExtRew) or PVi, below which PV is considered present (i.e., punishment) (set to 0 if PVe.rew.norew_val = 0)
  float		thr_max;	// #DEF_0.8 maximum threshold on PVe (ExtRew) or PVi, above which PV is considered present (i.e., reward) (set to .4 if PVe.rew.norew_val = 0)

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(PVDetectSpec);
  COPY_FUNS(PVDetectSpec, taBase);
  TA_BASEFUNS(PVDetectSpec);
};

class LEABRA_API PViLayerSpec : public ScalarValLayerSpec {
  // primary value inhibitory (PVi) layer: continously learns to expect primary reward values
public:
  PVDetectSpec	pv_detect;	// primary reward value detection spec: detect if a primary reward is present or expected

  virtual float	Compute_PVe(LeabraLayer* lay, LeabraNetwork* net, bool& actual_er_avail, bool& pv_detected);
  // get excitatory primary reward value from PVe/ExtRew layer if it is available: actual_er_avail = internal flag from ExtRew layer (cheating), pv_detected = PVi layer's detection of primary reward presence or expectation based on PVi, PVe values (not cheating)
  virtual void 	Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork* net);
  // copy ext values to act_p
  virtual void 	Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as external rewards and change weights

  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet=false);
  void	Defaults();

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(PViLayerSpec);
  COPY_FUNS(PViLayerSpec, ScalarValLayerSpec);
  TA_BASEFUNS(PViLayerSpec);
};

//////////////////////////////////////////
//	LV: Learned Value Layer		//
//////////////////////////////////////////

class LEABRA_API LVConSpec : public TrialSynDepConSpec {
  // learned value connection spec: learns using delta rule from PVe - LV values; also does synaptic depression to do novelty filtering
public:
  inline float C_Compute_Err(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->act_p;
    // wt is negative in linear form, so using opposite sign of usual here
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f + cn->wt);
      else		err *= -cn->wt;	
    }
    return err;
  }

  inline void Compute_dWt(Con_Group* cg, Unit* ru) {
    LeabraUnit* lru = (LeabraUnit*)ru;
    LeabraCon_Group* lcg = (LeabraCon_Group*) cg;
    Compute_SAvgCor(lcg, lru);
    if(((LeabraLayer*)cg->prjn->from)->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  float orig_wt = cn->wt;
	  C_Compute_LinFmWt(lcg, cn); // get weight into linear form
	  C_Compute_dWt(cn, lru, 
			C_Compute_Hebb(cn, lcg, lru->act_p, su->act_p),
			C_Compute_Err(cn, lru, su));  
	  cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
	  // depression operates on linear weight!
	  C_Depress_Wt((TrialSynDepCon*)cn, lru, su);
	}
      }
    }
  }

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(LVConSpec);
  COPY_FUNS(LVConSpec, TrialSynDepConSpec);
  TA_BASEFUNS(LVConSpec);
};

class LEABRA_API LVSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER specs for learned value layers
public:
  float		discount;	// #DEF_0 multiplicative discount factor for PVe/ExtRew/US training signal: plus phase clamp = (1-discount)*PVe
  bool		use_actual_er;	// #DEF_false use actual external reward presence to determine when to learn (cheating), otherwise use PVi's estimate of when primary value is avail (more realistic)

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(LVSpec);
  COPY_FUNS(LVSpec, taBase);
  TA_BASEFUNS(LVSpec);
};

class LEABRA_API LVeLayerSpec : public ScalarValLayerSpec {
  // learns value based on inputs that are associated with rewards, only learns at time of primary rewards (filtered by PV system). This is excitatory version
public:
  LVSpec	lv;	// learned value specs

  virtual void 	Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork* net);
  // copy ext values to act_p
  virtual void 	Compute_DepressWt(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
  // depress weights for units in unit group (only if not doing dwts!)
  virtual void 	Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // if primary value detected (present/expected), compute plus phase activations for learning, and actually change weights; otherwise just depress weights

  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet=false);
  void	Defaults();

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(LVeLayerSpec);
  COPY_FUNS(LVeLayerSpec, ScalarValLayerSpec);
  TA_BASEFUNS(LVeLayerSpec);
};

class LEABRA_API LViLayerSpec : public LVeLayerSpec {
  // inhibitory/slow version of LV layer spec: (just a marker for layer; same functionality as LVeLayerSpec)
public:

  void 	Initialize()		{ };
  void	Destroy()		{ };
  TA_BASEFUNS(LViLayerSpec);
};

//////////////////////////
//	  DaLayer 	//
//////////////////////////

class LEABRA_API PVLVDaSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER specs for PVLV da parameters
public:
  enum	DaMode {
    LV_PLUS_IF_PV,		// da = (LVe - LVi) + [if (PV detected (present/expected), PVe - PVi]
    IF_PV_ELSE_LV,		// if (PV detected (present/expected), da = PVe - PVi; else da = LVe - LVi
    PV_PLUS_LV			// da = (PVe - PVi) + (LVe - LVi)
  };

  DaMode	mode;		// #DEF_LV_PLUS_IF_PV how to compute DA as a function of PV and LV systems
  float		tonic_da;	// #DEF_0 set a tonic 'dopamine' (DA) level (offset to add to da values)
  float		min_lvi;	// #DEF_0.1 minimum LVi value, so that a low LVe value (~0) makes for negative DA: DA_lv = LVe - MAX(LVi, min_lvi)
  bool		use_actual_er;	// #DEF_false use actual external reward presence to determine when PV is detected (cheating), otherwise use PVi's estimate of when primary value is avail (more realistic)

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(PVLVDaSpec);
  COPY_FUNS(PVLVDaSpec, taBase);
  TA_BASEFUNS(PVLVDaSpec);
};

class LEABRA_API PVLVDaLayerSpec : public LeabraLayerSpec {
  // computes PVLV 'Da' signal: typically if(ER), da = ER-PV, else LVe - LVs
public:
  PVLVDaSpec	da;		// parameters for the lvpv da computation

  virtual void	Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute a zero da value: in minus phase
  virtual void	Compute_Da(LeabraLayer* lay, LeabraNetwork* net);
  // compute the da value based on recv projections: every cycle in 1+ phases
  virtual void	Send_Da(LeabraLayer* lay, LeabraNetwork* net);
  // send the da value to sending projections: every cycle

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt(LeabraLayer*, LeabraNetwork*);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet=false);
  void	Defaults();

  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(PVLVDaLayerSpec);
  COPY_FUNS(PVLVDaLayerSpec, LeabraLayerSpec);
  TA_BASEFUNS(PVLVDaLayerSpec);
};

#endif // leabra_pvlv_h
