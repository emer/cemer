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

#include "leabra_extra.h"

#ifndef leabra_td_h
#define leabra_td_h

//////////////////////////////////////////////////////////////////
// 	Reinforcement Learning Algorithms (TD/PVLV/BG/PFC)	//
//////////////////////////////////////////////////////////////////

class LEABRA_API DaModUnit : public LeabraUnit {
  // obsolete -- now incorporated into base LeabraUnit
INHERITED(LeabraUnit)
public:
  TA_BASEFUNS_NOCOPY(DaModUnit);
private:
 void	Initialize()    { };
  void	Destroy()	{ };
};

class LEABRA_API DaModUnitSpec : public LeabraUnitSpec {
  // obsolete -- now incoroporated into base LeabraUnitSpec
INHERITED(LeabraUnitSpec)
public:
  TA_BASEFUNS_NOCOPY(DaModUnitSpec);
private:
  void	Initialize() { };
  void	Destroy()    { };
};

////////////////////////////////////////////////////////////
//		Td Unit still needs separate vars

class LEABRA_API LeabraTdUnit : public LeabraUnit {
  // Leabra unit with dopamine-like modulation of minus phase activation for learning
INHERITED(LeabraUnit)
public:
  float 	p_act_m;	// previous minus phase activation 
  float		p_act_p;	// previous plus phase activation
  float		trace;		// the trace of activation states that is used for learning

  void	Copy_(const LeabraTdUnit& cp);
  TA_BASEFUNS(LeabraTdUnit);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API LeabraTdUnitSpec : public LeabraUnitSpec {
  // Leabra unit with temporal-differences variables for prior activation states
INHERITED(LeabraUnitSpec)
public:
  float		lambda;		// exponential decay parameter for updating activation trace values over time: these trace values are used in learning.  in principle this should also include the effects of the discount (gamma) parameter from the TdRewInteg layer

  override void	Init_Acts(Unit* u, Network* net);
  override void Init_Weights(Unit* u, Network* net);
  override void EncodeState(LeabraUnit* u, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(LeabraTdUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

//////////////////////////////////////////
//	External Reward Layer		//
//////////////////////////////////////////

class LEABRA_API AvgExtRewSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing average external rewards
INHERITED(SpecMemberBase)
public:
  bool		sub_avg;	// #DEF_false subtract average reward value in computing rewards
  float		avg_dt;		// #DEF_0.005 time constant for integrating average reward value

  TA_SIMPLE_BASEFUNS(AvgExtRewSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API OutErrSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing external rewards based on output performance of network
INHERITED(SpecMemberBase)
public:
  float		err_tol;	// #DEF_0.5 error tolerance for counting an activation wrong
  bool		graded;		// #DEF_false compute a graded reward signal as a function of number of correct output values
  bool		no_off_err;	// #DEF_false do not count a unit wrong if it is off but target says on -- only count wrong units that are on but should be off
  float		scalar_val_max;	// #CONDEDIT_ON_graded maximum value for scalar value output layers when using a graded value -- reward is error normalized by this value, and clipped at min/max

  TA_SIMPLE_BASEFUNS(OutErrSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API ExtRewSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for computing external rewards
INHERITED(SpecMemberBase)
public:
  float		err_val;	// #DEF_0 reward value for errors (when network does not respond correctly)
  float		norew_val;	// #DEF_0.5 reward value when no feedback information is present
  float		rew_val;	// #DEF_1 reward value for correct responses (positive rewards)

  TA_SIMPLE_BASEFUNS(ExtRewSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API ExtRewLayerSpec : public ScalarValLayerSpec {
  // computes external reward feedback: minus phase is zero, plus phase is reward value derived from network performance or other inputs (computed at start of 1+)
INHERITED(ScalarValLayerSpec)
public:
  enum RewardType {		// how do we get the reward values?
    OUT_ERR_REW,		// get rewards as a function of errors on the output layer ONLY WHEN RewTarg layer act > .5 -- get from markerconspec from output layer(s)
    EXT_REW,			// get rewards as external inputs marked as ext_flag = TARG to the first unit in the layer (if ext val == norew_val, then no ext rew signaled)
    DA_REW			// get rewards from da values on first unit in layer (if ext val == norew_val, then no ext rew signaled)
  };

  RewardType	rew_type;	// how do we get the reward values?
  AvgExtRewSpec	avg_rew;	// average reward computation specifications
  OutErrSpec	out_err;	// #CONDEDIT_ON_rew_type:OUT_ERR_REW how to compute external rewards based on output performance
  ExtRewSpec	rew;		// misc reward computation specifications


  virtual void Compute_Rew(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ExtRew overall compute reward function: calls appropriate sub-function based on rew_type
    virtual void Compute_OutErrRew(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew get reward value based on external error (put in da val, clamp)
      virtual bool OutErrRewAvail(LeabraLayer* lay, LeabraNetwork* net);
      // #CAT_ExtRew figure out if reward is available on this trial (look if target signals are present)
      virtual float GetOutErrRew(LeabraLayer* lay, LeabraNetwork* net);
      // #CAT_ExtRew get reward value based on error at layer with MarkerConSpec connection: 1 = rew (correct), 0 = err, -1 = no info
    virtual void Compute_ExtRew(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew get external rewards from inputs (put in da val, clamp)
    virtual void Compute_DaRew(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew clamp external rewards as da values (put in da val, clamp)

    virtual void Compute_UnitDa(float er, LeabraUnit* u, Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew used in above routines: actually compute the unit da value based on external reward value er
    virtual void Compute_NoRewAct(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_ExtRew used in above routines: clamp norew_val values for when no reward information is present

  // overrides:
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);

  // don't do any learning:
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(ExtRewLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

//////////////////////////////////////////////////////////
// 	Standard TD Reinforcement Learning 		//
//////////////////////////////////////////////////////////

class LEABRA_API TDRewPredConSpec : public LeabraConSpec {
  // Reward Prediction connections: for TD RewPred Layer, uses TD algorithm for predicting rewards
INHERITED(LeabraConSpec)
public:
  inline void C_Compute_dWt_Delta(LeabraCon* cn, float lin_wt, LeabraTdUnit* ru, LeabraTdUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->trace;
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f - lin_wt);
      else		err *= lin_wt;	
    }
    cn->dwt += cur_lrate * err;
  }

  inline void C_Compute_dWt_Delta_NoSB(LeabraCon* cn, LeabraTdUnit* ru, LeabraTdUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->trace;
    cn->dwt += cur_lrate * err;
  }


  // this computes weight changes based on sender at time t-1
  inline void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraTdUnit* lsu = (LeabraTdUnit*)su;
    for(int i=0; i<cg->size; i++) {
      LeabraTdUnit* ru = (LeabraTdUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta(cn, LinFmSigWt(cn->wt), ru, lsu);
    }
  }

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    LeabraTdUnit* lsu = (LeabraTdUnit*)su;
    for(int i=0; i<cg->size; i++) {
      LeabraTdUnit* ru = (LeabraTdUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      C_Compute_dWt_Delta_NoSB(cn, ru, lsu);
    }
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_CtLeabraXCAL(cg, su);
  }

  TA_BASEFUNS_NOCOPY(TDRewPredConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { Initialize(); }
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
  virtual void 	Compute_TdPlusPhase(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations for learning including the td values
    virtual void Compute_TdPlusPhase_ugp(Unit_Group* ugp, LeabraNetwork* net);
    // #IGNORE 

  override void	Init_Acts(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_BASEFUNS_NOCOPY(TDRewPredLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

//////////////////////////////////////////
//	TD Reward Integration Layer	//
//////////////////////////////////////////

class LEABRA_API TDRewIntegSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra misc specs for TDRewIntegLayerSpec
INHERITED(SpecMemberBase)
public:
  float		discount;	// discount factor for V(t+1) from TDRewPredLayer
  bool		max_r_v;	// represent the maximum of extrew (r) and tdrewpred estimate of V(t+1) instead of the sum of these two factors -- produces a kind of "absorbing" reward function instead of a cumulative reward function

  TA_SIMPLE_BASEFUNS(TDRewIntegSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

class LEABRA_API TDRewIntegLayerSpec : public ScalarValLayerSpec {
  // integrates perceived and external rewards: delta over phases = DA td-like signal. minus phase = prev exp rew V^(t), plus phase = extrew (r) + tdrewpred computing V(t+1)
INHERITED(ScalarValLayerSpec)
public:
  TDRewIntegSpec	rew_integ;	// misc specs for TDRewIntegLayerSpec

  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  // never learn
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TDRewIntegLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
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

  override void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  // never learn
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TdLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() { Initialize(); }
};

#endif // leabra_td_h
