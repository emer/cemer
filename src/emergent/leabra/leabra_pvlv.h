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

//////////////////////////////////////////////////////////////////////////////////////////
//	Pavlovian (PVLV): Primary Value and Learned Value Reward Learning System	//
//////////////////////////////////////////////////////////////////////////////////////////

// PV primary value: learns continuously about primary rewards (present or absent)
//   PVe = excitatory: primary reward (just uses ExtRewLayerSpec -- nothing specific for pvlv)
//   PVi = inhibitory: cancelling expected primary rewards
//   PVr = detector of time when rewards are present (fast to learn, slow to extinguish,
// 	   learns based on 1/0 values instead of actual reward magnitudes)
// LV learned value: learns only at the time of primary (expected) rewards,
//         free to fire at time CS's come on
//   LVe = excitatory: rapidly learns excitatory CS assocs
//   LVi = inhibitory: slowly learns to cancel CS assocs (adaptive baseline for LVe)
// PVLVDa (VTA/SNc) computes DA signal as: IF PV present, PVe - PVi, else LVe - LVi
// delta version (5/07) uses temporal derivative of LV & PV signals, not synaptic depression

//////////////////////////////////////////
//	PV: Primary Value Layer		//
//////////////////////////////////////////

class LEABRA_API PVConSpec : public LeabraConSpec {
  // primary value connection spec: learns using delta rule from PVe - PVi values -- does not use hebb or err_sb parameters
INHERITED(LeabraConSpec)
public:
  inline void C_Compute_dWt_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->act_p; // basic delta rule
    cn->dwt += cur_lrate * err;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    for(int i=0; i<cg->cons.size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->Cn(i);
      C_Compute_dWt_Delta(cn, ru, su);  
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    // no softbound so same as above
    Compute_dWt_LeabraCHL(cg, ru);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    // no softbound so same as above
    Compute_dWt_LeabraCHL(cg, ru);
  }

  TA_SIMPLE_BASEFUNS(PVConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};


class LEABRA_API PVMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for PV layer spec
INHERITED(taOBase)
public:
  float		min_pvi;	// #DEF_0.4 minimum pvi value -- PVi is not allowed to go below this value for the purposes of computing the PV delta value: pvd = PVe - MAX(PVi,min_pvi)
  float		prior_discount;	// #MIN_0 #MAX_1 how much to discount the prior PV delta value (pvd = PVe - MAX(PVi,min_pvi)) in computing the net PV dopamine signal (PV DA = pvd_t - prior_discount * pvd_t-1)
  bool		er_reset_prior;	// #DEF_true reset prior delta value (pvd_t-1) when external rewards are received (akin to absorbing rewards in TD)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PVMiscSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API PViLayerSpec : public ScalarValLayerSpec {
  // primary value inhibitory (PVi) layer: continously learns to expect primary reward values, contribute to overall dopamine with PV delta pvd = PVe - PVi; PV DA = pvd_t - pvd_t-1
INHERITED(ScalarValLayerSpec)
public:
  PVMiscSpec	pv;		// misc parameters for the PV computation

  virtual void 	Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as external rewards and change weights

  virtual float	Compute_PVDa(LeabraLayer* lay, LeabraNetwork* net);
  // compute da contribution from PV
    virtual float Compute_PVDa_ugp(Unit_Group* ugp, float pve_val);
    // #IGNORE
  virtual void	Update_PVPrior(LeabraLayer* lay, LeabraNetwork* net);
  // update the prior PV value, stored in pv unit misc_1 values -- at very end of trial
    virtual void Update_PVPrior_ugp(Unit_Group* ugp, bool er_avail);
    // #IGNORE

  // overrides:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }

  override void	Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PViLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////////////////////////////////
//	PVr = PV reward detection system (habenula?)

class LEABRA_API PVrConSpec : public PVConSpec {
  // primary value connection spec with asymmetrical learning rates -- used for reward detection connections -- have asymmetric weight decrease to lock in expectations for longer
INHERITED(PVConSpec)
public:
  float 	wt_dec_mult;   // multiplier for weight decrease rate relative to basic lrate used for weight increases

  inline void C_Compute_dWt_Delta(LeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    float err = (ru->act_p - ru->act_m) * su->act_p; // basic delta rule
    if(err < 0.0f)	err *= wt_dec_mult;
    // note: no err_sb
    cn->dwt += cur_lrate * err;
  }

  inline override void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    for(int i=0; i<cg->cons.size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->Cn(i);
      C_Compute_dWt_Delta(cn, ru, su);
    }
  }

  inline override void Compute_dWt_CtLeabraXCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    // no softbound so same as above
    Compute_dWt_LeabraCHL(cg, ru);
  }

  inline override void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    // no softbound so same as above
    Compute_dWt_LeabraCHL(cg, ru);
  }


  TA_SIMPLE_BASEFUNS(PVrConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API PVDetectSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for detecting if a primary value is present or expected
INHERITED(taOBase)
public:
  float		thr_min;	// #DEF_0.2 minimum threshold on PVr value, below which PV is considered present (i.e., punishment) -- PVr learns a 0 for all punishment-valence cases, regardless of value
  float		thr_max;	// #DEF_0.8 maximum threshold on PVr value, above which PV is considered present (i.e., reward) -- PVr learns a 1 for all reward-valence cases, regardless of value

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PVDetectSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API PVrLayerSpec : public ScalarValLayerSpec {
  // primary value reward detection layer: learns when rewards are expected to occur -- gets a 1 for positive rewards (> PVe.norew_val) and a 0 for punishments (< PVe.norew_val)
INHERITED(ScalarValLayerSpec)
public:
  PVDetectSpec	pv_detect;	// primary reward value detection spec: detect if a primary reward is expected based on PVr value

  virtual void 	Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as external rewards and change weights

  virtual bool 	Compute_PVDetect(LeabraLayer* lay, LeabraNetwork* net);
  // detect PV expectation based on PVr value -- happens at end of minus phase, based on unit activations then

  // overrides:
  override bool	Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override void	Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_BASEFUNS_NOCOPY(PVrLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


////////////////////////////////////////////////////////////////////////
//		LV System: Learned Value

class LEABRA_API LVMiscSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for PV layer spec
INHERITED(taOBase)
public:
  float		min_lvi;	// #DEF_0.1 minimum lvi value -- LVi is not allowed to go below this value for the purposes of computing the LV delta value: lvd = LVe - MAX(LVi,min_lvi)
  float		prior_discount;	// #MIN_0 #MAX_1 how much to discount the prior time step LV delta value (lvd = LVe - MAX(LVi,min_lvi)) in computing the net LV dopamine signal (LV DA = lvd_t - prior_discount * lvd_t-1)
  bool		er_reset_prior;	// #DEF_true reset prior delta value (lvd_t-1) when external rewards are received (akin to absorbing rewards in TD)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(LVMiscSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LVeLayerSpec : public ScalarValLayerSpec {
  // learns value based on inputs that are associated with rewards, only learns at time of primary rewards (filtered by PV system). This is excitatory version LVe.  LV contribution to dopamine is based on LV delta lvd = LVe - LVi; LV DA = lvd_t - lvd_t-1
INHERITED(ScalarValLayerSpec)
public:
  LVMiscSpec	lv;		// misc parameters controlling the LV computation (note: only the LVe instance of these parameters are used)

  virtual void 	Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // if primary value detected (present/expected), compute plus phase activations for learning, and actually change weights

  virtual float	Compute_LVDa(LeabraLayer* lve_lay, LeabraLayer* lvi_lay, LeabraNetwork* net);
  // compute da contribution from Lv, based on lve_layer and lvi_layer activations (multiple subgroups allowed)
    virtual float Compute_LVDa_ugp(Unit_Group* lve_ugp, Unit_Group* lvi_ugp);
    // #IGNORE

  virtual void	Update_LVPrior(LeabraLayer* lay, LeabraNetwork* net);
  // update the prior Lv value, stored in lv unit misc_1 values
    virtual void Update_LVPrior_ugp(Unit_Group* lve_ugp, bool er_avail);
    // #IGNORE

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }

  override void	Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(LVeLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API LViLayerSpec : public LVeLayerSpec {
  // inhibitory/slow version of LV layer spec: (just a marker for layer; same functionality as LVeLayerSpec)
INHERITED(LVeLayerSpec)
public:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);

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
  float		prior_discount;	// #MIN_0 #MAX_1 how much to discount the prior NV delta value (nvd = NV - val_thr) in computing the net NV dopamine signal (NV DA = nvd_t - prior_discount * nvd_t-1)
  bool		er_reset_prior;	// #DEF_true reset prior delta value (nvd_t-1) when external rewards are received (akin to absorbing rewards in TD)

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(NVSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API NVLayerSpec : public ScalarValLayerSpec {
  // novelty value (NV) layer: starts with a bias of 1.0, and learns to activate 0.0 value -- value signal is how novel the stimulus is: NV delta nvd = NV - val_thr; NV DA = nvd_t - nvd_t-1
INHERITED(ScalarValLayerSpec)
public:
  NVSpec	nv;	// novelty value specs

  virtual float	Compute_NVDa(LeabraLayer* lay, LeabraNetwork* net);
  // compute novelty value da value
  virtual void 	Compute_NVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations as train target value and change weights
  virtual void	Update_NVPrior(LeabraLayer* lay, LeabraNetwork* net);
  // update the prior Nv value, stored in nv unit misc_1 values

  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_SRAvg_Test(LeabraLayer*, LeabraNetwork*) { return false; }
  override void	Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net);

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

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(PVLVDaSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API PVLVDaLayerSpec : public LeabraLayerSpec {
  // computes PVLV dopamine (Da) signal: typically if(ER), da = PVe-PVi, else LVe - LVi
INHERITED(LeabraLayerSpec)
public:
  PVLVDaSpec	da;		// parameters for the pvlv da computation

  virtual void	Send_Da(LeabraLayer* lay, LeabraNetwork* net);
  // send the da value to sending projections: every cycle
  virtual void	Compute_Da(LeabraLayer* lay, LeabraNetwork* net);
  // compute the da value based on recv projections: every cycle in 1+ phases (delta version)

  override void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);

  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) { };
  override void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) { };
  override void	Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  // never learn
  override bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_SecondPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  void	Defaults();

  TA_SIMPLE_BASEFUNS(PVLVDaLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

#endif // leabra_pvlv_h
