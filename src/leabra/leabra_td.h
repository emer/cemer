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

#include "leabra_extra.h"

#ifndef leabra_td_h
#define leabra_td_h

//////////////////////////////////////////////////////////////////
// 	Reinforcement Learning Algorithms (TD/PVLV/BG/PFC)	//
//////////////////////////////////////////////////////////////////

class LEABRA_API DaModUnit : public LeabraUnit {
  // Leabra unit with dopamine-like modulation of minus phase activation for learning
public:
  float		act_m2;		// second minus phase activation
  float		act_p2;		// second plus phase activation
  float 	p_act_m;	// previous minus phase activation 
  float		p_act_p;	// previous plus phase activation
  float 	dav;		// modulatory dopamine value 

  void	Initialize();
  void	Destroy()	{ };
  void	Copy_(const DaModUnit& cp);
  COPY_FUNS(DaModUnit, LeabraUnit);
  TA_BASEFUNS(DaModUnit);
};

class LEABRA_API DaModSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for effects of da-based modulation: plus-phase = learning effects
public:
  enum ModType {
    PLUS_CONT,			// da modulates plus-phase activations (only) in a continuous manner
    PLUS_POST,			// da modulates plus-phase activations (only), at the end of the plus phase
    NEG_DIP			// da dips provide a (decaying) negative (accomodation) current on a trial-wise basis
  };

  bool		on;		// whether to actually modulate activations by da values
  ModType	mod;		// #CONDEDIT_ON_on:true how to apply DA modulation
  float		gain;		// #CONDEDIT_ON_on:true gain multiplier of da values
  float		neg_rec;	// #CONDEDIT_ON_mod:NEG_DIP recovery time constant for NEG_DIP case (how much to decay negative current per trial)
  bool		p_dwt;		// whether units learn based on prior activation states, as in TD and other algorithms (not really to do with DA modulation; just stuck here.. affects Compute_dWt and Compute_Weights calls)

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(DaModSpec);
  COPY_FUNS(DaModSpec, taBase);
  TA_BASEFUNS(DaModSpec);
};

class LEABRA_API DaModUnitSpec : public LeabraUnitSpec {
  // Leabra unit with temporal-differences error modulation of minus phase activation for learning
public:
  DaModSpec	da_mod;		// da modulation of activations (for da-based learning, and other effects)

  void 		Compute_Conduct(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);

  void		Init_Acts(LeabraUnit* u, LeabraLayer* lay);
  void		Init_Acts(Unit* u)	{ LeabraUnitSpec::Init_Acts(u); }
  void		Compute_dWt(Unit*) { };
  void 		Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  void 		Compute_Weights(Unit* u);
  void 		EncodeState(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  void		DecayEvent(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay);
  void		PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
			   LeabraNetwork* net, bool set_both=false);

  void	Defaults();
  void	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(DaModUnitSpec);
  COPY_FUNS(DaModUnitSpec, LeabraUnitSpec);
  TA_BASEFUNS(DaModUnitSpec);
};

//////////////////////////////////////////
//	External Reward Layer		//
//////////////////////////////////////////

class LEABRA_API AvgExtRewSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing average external rewards
public:
  bool		sub_avg;	// #DEF_false subtract average reward value in computing rewards
  float		avg_dt;		// #DEF_0.005 time constant for integrating average reward value

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(AvgExtRewSpec);
  COPY_FUNS(AvgExtRewSpec, taBase);
  TA_BASEFUNS(AvgExtRewSpec);
};

class LEABRA_API OutErrSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing external rewards based on output performance of network
public:
  float		err_tol;	// #DEF_0.5 error tolerance for counting an activation wrong
  bool		graded;		// #DEF_false compute a graded reward signal as a function of number of correct output values
  bool		no_off_err;	// #DEF_false do not count a unit wrong if it is off but target says on -- only count wrong units that are on but should be off
  bool		seq_all_cor;	// #DEF_false require that all RewTarg events in a sequence be correct before giving reward (on the last event in sequence);  if graded is checked, this reward is a graded function of % correct

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(OutErrSpec);
  COPY_FUNS(OutErrSpec, taBase);
  TA_BASEFUNS(OutErrSpec);
};

class LEABRA_API ExtRewSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing external rewards
public:
  float		err_val;	// #DEF_0 reward value for errors (when network does not respond correctly)
  float		norew_val;	// #DEF_0.5 reward value when no feedback information is present
  float		rew_val;	// #DEF_1 reward value for correct responses (positive rewards)

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(ExtRewSpec);
  COPY_FUNS(ExtRewSpec, taBase);
  TA_BASEFUNS(ExtRewSpec);
};

class LEABRA_API ExtRewLayerSpec : public ScalarValLayerSpec {
  // computes external reward feedback: minus phase is zero, plus phase is reward value derived from network performance or other inputs (computed at start of 1+)
INHERITED(ScalarValLayerSpec)
public:
  enum RewardType {		// how do we get the reward values?
    OUT_ERR_REW,		// get rewards as a function of errors on the output layer ONLY WHEN RewTarg layer act > .5 -- get from markerconspec from output layer(s)
    EXT_REW,			// get rewards as external inputs marked as ext_flag = TARG to the first unit in the layer
    DA_REW			// get rewards from da values on first unit in layer
  };

  RewardType	rew_type;	// how do we get the reward values?
  AvgExtRewSpec	avg_rew;	// average reward computation specifications
  OutErrSpec	out_err;	// #CONDEDIT_ON_rew_type:OUT_ERR_REW how to compute external rewards based on output performance
  ExtRewSpec	rew;		// misc reward computation specifications

  virtual bool	OutErrRewAvail(LeabraLayer* lay, LeabraNetwork* net);
  // figure out if reward is available on this trial (look if target signals are present)
  virtual float	GetOutErrRew(LeabraLayer* lay, LeabraNetwork* net);
  // get reward value based on error at layer with MarkerConSpec connection: 1 = rew (correct), 0 = err, -1 = no info
  virtual void 	Compute_OutErrRew(LeabraLayer* lay, LeabraNetwork* net);
  // get reward value based on external error (put in da val, clamp)
  virtual void 	Compute_ExtRew(LeabraLayer* lay, LeabraNetwork* net);
  // get external rewards from inputs (put in da val, clamp)
  virtual void 	Compute_DaRew(LeabraLayer* lay, LeabraNetwork* net);
  // clamp external rewards as da values (put in da val, clamp)
  virtual void 	Compute_UnitDa(float er, DaModUnit* u, Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
  // actually compute the unit da value based on external reward value er
  virtual void	Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork* net);
  // clamp zero activations, for minus phase
  virtual void	Compute_NoRewAct(LeabraLayer* lay, LeabraNetwork* net);
  // clamp norew_val values for when no reward information is present

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(ExtRewLayerSpec);
  COPY_FUNS(ExtRewLayerSpec, ScalarValLayerSpec);
  TA_BASEFUNS(ExtRewLayerSpec);
};

//////////////////////////////////////////////////////////
// 	Standard TD Reinforcement Learning 		//
//////////////////////////////////////////////////////////

class LEABRA_API TDRewPredConSpec : public LeabraConSpec {
  // Reward Prediction connections: for TD RewPred Layer, uses TD algorithm for predicting rewards
public:
  inline float C_Compute_Err(LeabraCon* cn, DaModUnit* ru, DaModUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->p_act_p;
    // wt is negative in linear form, so using opposite sign of usual here
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f + cn->wt);
      else		err *= -cn->wt;	
    }
    return err;
  }

  // this computes weight changes based on sender at time t-1
  inline void Compute_dWt(Con_Group* cg, Unit* ru) {
    DaModUnit* lru = (DaModUnit*)ru;
    LeabraCon_Group* lcg = (LeabraCon_Group*) cg;
    Compute_SAvgCor(lcg, lru);
    if(lru->p_act_p >= 0.0f) {
      for(int i=0; i<lcg->size; i++) {
	DaModUnit* su = (DaModUnit*)lcg->Un(i);
	LeabraCon* cn = (LeabraCon*)lcg->Cn(i);
	float orig_wt = cn->wt;
	C_Compute_LinFmWt(lcg, cn); // get into linear form
	C_Compute_dWt(cn, lru, 
		      C_Compute_Hebb(cn, lcg, lru->act_p, su->p_act_p),
		      C_Compute_Err(cn, lru, su));  
	cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
      }
    }
  }

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(TDRewPredConSpec);
  COPY_FUNS(TDRewPredConSpec, LeabraConSpec);
  TA_BASEFUNS(TDRewPredConSpec);
};

//////////////////////////////////////////////////
//	TD Reward Prediction Layer		//
//////////////////////////////////////////////////

class LEABRA_API TDRewPredLayerSpec : public ScalarValLayerSpec {
  // predicts rewards: minus phase = clamped prior expected reward V^(t), plus = settles on expectation of future reward V^(t+1)
INHERITED(ScalarValLayerSpec)
public:
  virtual void 	Compute_SavePred(Unit_Group* ugp, LeabraNetwork* net); // save current prediction to misc_1 for later clamping
  virtual void 	Compute_ClampPred(Unit_Group* ugp, LeabraNetwork* net); // clamp misc_1 to ext 
  virtual void 	Compute_ClampPrev(LeabraLayer* lay, LeabraNetwork* net);
  // clamp minus phase to previous act value
  virtual void 	Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork* net);
  // copy ext values to act_p
  virtual void 	Compute_TdPlusPhase_impl(Unit_Group* ugp, LeabraNetwork* net);
  virtual void 	Compute_TdPlusPhase(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations for learning including the td values

  void	Init_Acts(LeabraLayer* lay);
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void	PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both=false);
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(TDRewPredLayerSpec);
  COPY_FUNS(TDRewPredLayerSpec, ScalarValLayerSpec);
  TA_BASEFUNS(TDRewPredLayerSpec);
};

//////////////////////////////////////////
//	TD Reward Integration Layer	//
//////////////////////////////////////////

class LEABRA_API TDRewIntegSpec : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for TDRewIntegLayerSpec
public:
  float		discount;	// discount factor for V(t+1) from TDRewPredLayer

  void	Initialize();
  void 	Destroy()	{ };
  SIMPLE_COPY(TDRewIntegSpec);
  COPY_FUNS(TDRewIntegSpec, taBase);
  TA_BASEFUNS(TDRewIntegSpec);
};

class LEABRA_API TDRewIntegLayerSpec : public ScalarValLayerSpec {
  // integrates perceived and external rewards: delta over phases = DA td-like signal. minus phase = prev exp rew V^(t), plus phase = extrew (r) + tdrewpred computing V(t+1)
INHERITED(ScalarValLayerSpec)
public:
  TDRewIntegSpec	rew_integ;	// misc specs for TDRewIntegLayerSpec

  void 	Compute_Act(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(TDRewIntegLayerSpec);
  COPY_FUNS(TDRewIntegLayerSpec, ScalarValLayerSpec);
  TA_BASEFUNS(TDRewIntegLayerSpec);
};

//////////////////////////
//	  TdLayer 	//
//////////////////////////

class LEABRA_API TdLayerSpec : public LeabraLayerSpec {
  // computes activation = temporal derivative (act_eq - act_m) of sending units in plus phases: note, act will go negative!
INHERITED(LeabraLayerSpec)
public:
  virtual void	Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute a zero td value: in minus phase
  virtual void	Compute_Td(LeabraLayer* lay, LeabraNetwork* net);
  // compute the td value based on recv projections: every cycle in 1+ phases
  virtual void	Send_Td(LeabraLayer* lay, LeabraNetwork* net);
  // send the td value to sending projections: every cycle

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act(LeabraLayer* lay, LeabraNetwork* net);

  void	Compute_dWt(LeabraLayer*, LeabraNetwork*) { }; // nop

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  void 	Initialize();
  void	Destroy()		{ };
  void  InitLinks();
  SIMPLE_COPY(TdLayerSpec);
  COPY_FUNS(TdLayerSpec, LeabraLayerSpec);
  TA_BASEFUNS(TdLayerSpec);
};

#endif // leabra_td_h
