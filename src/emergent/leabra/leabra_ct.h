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

#ifndef leabra_ct_h
#define leabra_ct_h

// pre-declarations
class CtLeabraCon;
class CtLeabraConSpec;
class CtLeabraRecvCons;
class CtLeabraUnit;
class CtLeabraUnitSpec;
class CtLeabraLayer;
class CtLeabraLayerSpec;
class CtLeabraNetwork; //

//////////////////////////////////////////////////////////////////
// 	Continuous Time Processing and Learning in Leabra	//
//////////////////////////////////////////////////////////////////

// The most abstract version features:
// * synaptic depression
// * targ_minus_sravg CHL learning that reinforces target activation state against entire range
//   of variation that occurs during settling as captured in average sender-receiver activation
//   coproducts

class LEABRA_API CtLeabraCon : public LeabraCon {
  // continuous time leabra con: most abstract version of continous time
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation
  float		intwt;		// #NO_SAVE slowly integrating weight value -- exponentially approaches current learned weight value -- mediates between wt and effwt
  float		cai;		// #NO_SAVE intracellular postsynaptic calcium current integrated over cycles, used for synaptic depression
  float		sravg;		// #NO_SAVE average of sender and receiver activation product over time, used for minus phase of targ_minus_avg learning

  CtLeabraCon() { effwt = 0.0f; intwt = 0.0f; cai = 0.0f; sravg = 0.0f; }
};

class LEABRA_API CtCaDepSpec : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(taBase)
public:
  float		intwt_dt;	// #DEF_0.02 time constant for integrating intwt value relative to current weight value -- default base value is .001 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)

  float		ca_inc;		// #DEF_0.2 time constant for increases in Ca_i (from NMDA etc currents) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)
  float		ca_dec;		// #DEF_0.2 time constant for decreases in Ca_i (from Ca pumps pushing Ca back out into the synapse) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)

  float		sd_ca_thr;	// #DEF_0.2 synaptic depression ca threshold: only when ca_i has increased by this amount (thus synaptic ca depleted) does it affect firing rates and thus synaptic depression
  float		sd_ca_gain;	// #DEF_0.3 multiplier on cai value for computing synaptic depression -- modulates overall level of depression independent of rate parameters
  float		sd_ca_thr_rescale; // #READ_ONLY rescaling factor taking into account sd_ca_gain and sd_ca_thr (= sd_ca_gain/(1 - sd_ca_thr))

  inline void	IntWtUpdt(float& intwt, float wt) {
    intwt += intwt_dt * (wt - intwt);
  }

  inline void	CaUpdt(float& cai, float ru_act, float su_act) {
    float drive = ru_act * su_act;
    cai += ca_inc * (1.0f - cai) * drive - ca_dec * cai;
  }

  inline float	SynDep(float cai) {
    float cao_thr = (cai > sd_ca_thr) ? (1.0 - sd_ca_thr_rescale * (cai - sd_ca_thr)) : 1.0f;
    return cao_thr * cao_thr;
  }

  SIMPLE_COPY(CtCaDepSpec);
  TA_BASEFUNS(CtCaDepSpec);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtDwtNorm : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra renormalize weight changes 
INHERITED(taBase)
public:
  bool		on;		// whether to do normalized dwt Compute_Weights function 
  float		norm_pct;	// #CONDEDIT_ON_on what proportion of full normalization to apply to the delta weights (0 = no norm, 1 = full norm)

  SIMPLE_COPY(CtDwtNorm);
  TA_BASEFUNS(CtDwtNorm);
  //protected:
  //  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};


class LEABRA_API CtLeabraConSpec : public LeabraConSpec {
  // continuous time leabra con spec: most abstract version of continous time
INHERITED(LeabraConSpec)
public:
  CtCaDepSpec	ca_dep;		// calcium-based depression of synaptic efficacy and learning rate
  CtDwtNorm	dwt_norm;	// renormalize weight changes to compensate for overal mean changes
  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  inline void C_Compute_Cai(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su);
  // connection-level Cai update
  inline void Compute_Cai(CtLeabraRecvCons* cg, CtLeabraUnit* ru);
  // connection-group level Cai update

  // connection-level synaptic depression: syn dep direct
  inline void C_Depress_Wt(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su);
  // connection-level synaptic depression: ca mediated
  inline void Depress_Wt(CtLeabraRecvCons* cg, CtLeabraUnit* ru);
  // connection-group level synaptic depression

  inline void C_Compute_CtCycle(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su,
				float& cai_avg, float& cai_max);
  // one cycle of updating at connection-level 
  inline void Compute_CtCycle(CtLeabraRecvCons* cg, CtLeabraUnit* ru,
			      float& cai_avg, float& cai_max);
  // one cycle of processing at a Ct synapse -- expensive!!  todo: need to find ways to optimize

  /////////////////////////////////////////////////////////////////////////////////////
  // 		Special learning functions
  
  inline void C_Compute_SRAvg(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su,
			      float sravg_wt);
  inline void Compute_SRAvg(CtLeabraRecvCons* cg, CtLeabraUnit* ru, CtLeabraNetwork* net,
			    float sravg_wt);
  // accumulate sender-receiver activation product average, using given weighting factor

  inline void Init_SRAvg(CtLeabraRecvCons* cg, CtLeabraUnit* ru, CtLeabraNetwork* net);
  // initialize sender-receiver activation product average

  inline void C_Compute_dWt(CtLeabraCon* cn, CtLeabraUnit* ru, float heb, float err);
  inline void C_Compute_dWt_NoHebb(CtLeabraCon* cn, CtLeabraUnit* ru, float err);
  // optimized version for lmix.hebb == 0 -- ct does not typically require hebb

  inline float C_Compute_Err(CtLeabraCon* cn, float lin_wt,
			     CtLeabraUnit* ru, CtLeabraUnit* su, float avg_nrm);

  inline void  Compute_dWtCt(CtLeabraRecvCons* cg, CtLeabraUnit* ru, CtLeabraNetwork* net);
  // Ct version of compute dwt function

  inline void C_Compute_Weights_Norm(CtLeabraCon* cn, CtLeabraRecvCons* cg,
		     CtLeabraUnit*, CtLeabraUnit*, CtLeabraUnitSpec*, float dwnorm);
  // compute new weights from changes, dwt_norm.on version with given norm factor
  inline void C_Compute_WeightsActReg_Norm(CtLeabraCon* cn, CtLeabraRecvCons* cg,
		   CtLeabraUnit* ru, CtLeabraUnit* su, CtLeabraUnitSpec* rus, float dwnorm);
  // compute new weights from changes, dwt_norm.on version with given norm factor

  inline void Compute_Weights(RecvCons* cg, Unit* ru);


  /////////////////////////////////////////////////////////////////////////////////////
  // 		Following are all standard code revised to use effwt instead of wt
  
  inline void C_Reset_EffWt(CtLeabraCon* cn) {
    cn->effwt = cn->wt; cn->intwt = cn->wt;
  }
  inline void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CtLeabraCon*)cg->Cn(i)));
  }
  inline void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CtLeabraCon*)cg->Cn(i)));
  }

  override void C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    CtLeabraCon* lcn = (CtLeabraCon*)cn; lcn->effwt = lcn->wt; lcn->intwt = lcn->wt;
    lcn->cai = 0.0f; lcn->sravg = 0.0f; 
  }

  float C_Compute_Netin(CtLeabraCon* cn, Unit*, Unit* su) {
    return cn->effwt * su->act;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CtLeabraCon*)cg->Cn(i), ru, cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }
  void C_Send_Inhib(LeabraSendCons* cg, CtLeabraCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->gc.i += su_act_eff * cn->effwt;
  }
  void Send_Inhib(LeabraSendCons* cg, LeabraUnit* su) {
    // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
    // LeabraUnitSpec::CheckConfig checks that this is ok.
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act;
    CON_GROUP_LOOP(cg, C_Send_Inhib(cg, (CtLeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
  }
  void C_Send_Netin(LeabraSendCons* cg, CtLeabraCon* cn, Unit* ru, float su_act_eff) {
    ru->net += su_act_eff * cn->effwt;
  }
  void Send_Netin(SendCons* cg, Unit* su) {
    // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
    // LeabraUnitSpec::CheckConfig checks that this is ok.
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_Inhib((LeabraSendCons*)cg, (CtLeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_Netin((LeabraSendCons*)cg, (CtLeabraCon*)cg->Cn(i), cg->Un(i), su_act_eff));
    }
  }

  void C_Send_InhibDelta(LeabraSendCons* cg, CtLeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += su_act_delta_eff * cn->effwt;
  }
  void Send_InhibDelta(LeabraSendCons* cg, LeabraUnit* su) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act_delta;
    CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (CtLeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
  }
  void C_Send_NetinDelta(LeabraSendCons* cg, CtLeabraCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += su_act_delta_eff * cn->effwt;
  }
  void Send_NetinDelta(LeabraSendCons* cg, LeabraUnit* su) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act_delta;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (CtLeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta(cg, (CtLeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
  }
  void C_Send_ClampNet(LeabraSendCons* cg, CtLeabraCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->clmp_net += su_act_eff * cn->effwt;
  }
  void Send_ClampNet(LeabraSendCons* cg, LeabraUnit* su) {
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su->act;
    CON_GROUP_LOOP(cg, C_Send_ClampNet(cg, (CtLeabraCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
  }

  TA_SIMPLE_BASEFUNS(CtLeabraConSpec);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraRecvCons : public LeabraRecvCons {
  // continuous time leabra recv cons: most abstract version of continous time
INHERITED(LeabraRecvCons)
public:
  float		dwt_mean;	// mean delta-weight changes (only computed for dwt_norm.on)

  void	Compute_CtCycle(CtLeabraUnit* ru, float& cai_avg, float& cai_max)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_CtCycle(this, ru, cai_avg, cai_max); }
  // #CAT_Learning compute one cycle of continuous time processing
  void	Compute_SRAvg(CtLeabraUnit* ru, CtLeabraNetwork* net, float sravg_wt)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_SRAvg(this, ru, net, sravg_wt); }
  // #CAT_Learning compute sending-receiving activation product averages
  void	Compute_dWtCt(CtLeabraUnit* ru, CtLeabraNetwork* net)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_dWtCt(this, ru, net); }
  // #CAT_Learning compute flipped version of dwt (plus-minus reversed)

  void	Copy_(const CtLeabraRecvCons& cp);
  TA_BASEFUNS(CtLeabraRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraBiasSpec : public LeabraBiasSpec {
  // continuous time leabra bias spec: most abstract version of continous time
INHERITED(LeabraBiasSpec)
public:
  inline void B_Compute_SRAvg(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraNetwork* net,
			      float sravg_wt);

 inline void B_Compute_dWtCt(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraLayer* rlay,
			     CtLeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(CtLeabraBiasSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	Ct Unit
//////////////////////////////////

class LEABRA_API CtLeabraUnitSpec : public DaModUnitSpec {
  // continuous time leabra unit spec: most abstract version of continous time
INHERITED(DaModUnitSpec)
public:
  virtual void 	Compute_CtCycle(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute one cycle of continuous time processing
  virtual void 	Compute_SRAvg(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute sending-receiving activation product averages
  override void	Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);

  virtual void 	Compute_ActP(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute target plus phase activations (snapshot prior to learning)

  override void	Init_Acts(LeabraUnit* u, LeabraLayer* lay);

  TA_BASEFUNS_NOCOPY(CtLeabraUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraUnit : public DaModUnit {
  // continuous time leabra unit: most abstract version of continous time
INHERITED(DaModUnit)
public:
  float		cai_avg;	// #NO_SAVE #CAT_Activation average level of cai in my incoming connections -- just for analysis and debugging in early development -- remove later
  float		cai_max;	// #NO_SAVE #CAT_Activation maximum level of cai in my incoming connections -- used for determining when to learn
  float		syndep_avg;	// #NO_SAVE #CAT_Activation average level of synaptic depression in my incoming connections -- just for analysis and debugging in early development -- remove later
  float		syndep_max;	// #NO_SAVE #CAT_Activation maximum level of synaptic depression in my incoming connections -- just for analysis and debugging in early development -- remove later

  void		Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_CtCycle(this, lay, net); }
  // #CAT_Learning compute one cycle of continuous time processing
  void		Compute_SRAvg(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_SRAvg(this, lay, net); }
  // #CAT_Learning compute sending-receiving activation product averages

  void		Compute_ActP(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_ActP(this, lay, net); }
  // #CAT_Learning compute target plus phase activations (snapshot prior to learning)

  TA_BASEFUNS_NOCOPY(CtLeabraUnit);
private:
  void	Initialize();
  void	Destroy()	{ };
};


//////////////////////////////////
// 	Ct Layer
//////////////////////////////////

class LEABRA_API CtLeabraLayerSpec : public LeabraLayerSpec {
  // continuous time leabra layer spec: most abstract version of continous time
INHERITED(LeabraLayerSpec)
public:
  virtual void 	Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute one cycle of continuous time processing
  virtual void 	Compute_SRAvg(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute sending-receiving activation product averages

  virtual void 	Compute_ActP(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute target plus phase activations (snapshot prior to learning)

  virtual void 	Compute_CtDynamicInhib(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Activation compute extra dynamic inhibition for ct leabra algorithm

  override void	Init_Weights(LeabraLayer* lay);
  override void Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  override void	Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  override void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net);

  override bool CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_BASEFUNS_NOCOPY(CtLeabraLayerSpec);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraLayer : public LeabraLayer {
  // continuous time leabra layer: most abstract version of continous time
INHERITED(LeabraLayer)
public:
  float		maxda_sum;	// #CAT_Activation sum of maxda since last SRAvg update
  float		sravg_sum;	// #CAT_Activation sum of sravg weightings (count of number of times sravg has been computed) -- used for normalizing the weighted average
  int		sravg_cyc;	// #CAT_Activation cycles since last sravg computation -- potentially useful for determining when unit is in attractor state
  float		mean_cai_max;	// mean across units of cai_max value for each unit, which is max cai across all incoming connections

  void 	Compute_CtCycle(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_CtCycle(this, net); };
  // #CAT_Learning compute one cycle of continuous time processing
  void 	Compute_SRAvg(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_SRAvg(this, net); };
  // #CAT_Learning compute sending-receiving activation product averages

  void 	Compute_ActP(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_ActP(this, net); };
  // #CAT_Learning compute target plus phase activations (snapshot prior to learning)

  void	Copy_(const CtLeabraLayer& cp);
  TA_BASEFUNS(CtLeabraLayer);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	Ct Network
//////////////////////////////////

class LEABRA_API CtTrialTiming : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra timing parameters for a single stimulus input trial of ct learning algorithm
INHERITED(taBase)
public:
  int		minus;		// number of cycles to run in the minus phase with only inputs and no targets (used by CtLeabraSettle program), sets cycle_max -- can be 0
  int		plus;		// number of cycles to run in the plus phase with input and target activations (used by CtLeabraSettle program), sets cycle_max -- must be > 0
  int		inhib;		// number of cycles to run in the final inhibitory phase -- network can do MINUS_PLUS_PLUS, MINUS_PLUS_MINUS, or MINUS_PLUS_NOTHING for inputs on this phase
  int		inhib_max;	// number of cycles into inhib phase for inhibition to reach its maximum level (on a linear ramp) -- 0 means do not apply this function

  int		burst;		// number of cycles at start of trial to reduce inhibition levels as a result of residual refractoriness from prior trial, allowing incoming activation to over-activate relative to stable values -- ramps down linearly from start of trial to 0 reduction after burst cycles -- <=1 means do not apply

  int		sravg_end;	// number of cycles from the start of the final inhibitory phase to continue recording sravg

  int		syndep_int;	// #DEF_20 interval for doing synaptic depression and associated Ca_i integration calcuations -- numbers > 1 result in faster processing and actually work better too -- need to adjust the conspec ca/syndep rate constants in step with this (multiply by this number)

  int		total_cycles;	// #READ_ONLY computed total number of cycles per trial
  int		inhib_start;	// #READ_ONLY computed start of inhib phase (=minus + plus)

  SIMPLE_COPY(CtTrialTiming);
  TA_BASEFUNS(CtTrialTiming);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtInhibMod : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra inhibitory modulation parameters simulating effects of slow time-constant inhibition dynamics
INHERITED(taBase)
public:
  float		inhib_i;	// maximum extra inhibition as proportion of computed kwta value to add during final inhib phase
  float		burst_i;	// maximum reduction in inhibition as a proportion of computed kwta value to subtract during initial burst phase (trial starts at this reduction, goes down to 0 after burst cycles) -- value should be a positive number!

  SIMPLE_COPY(CtInhibMod);
  TA_BASEFUNS(CtInhibMod);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtSRAvgSpec : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra how to compute the sravg value as a function of layer-level delta-activation (measure of how close network is to attractor)
INHERITED(taBase)
public:
  float		min_da_thr;	// #DEF_0.005 minimum threshold value of accumulated layer-level delta activation (da_sum) for computing sravg value
  float		max_da_thr;	// #DEF_0.5 maximum value of layer-level max da (max delta-activation), above which sravg is not computed (prevents learning when too far out of the attractor state)

  SIMPLE_COPY(CtSRAvgSpec);
  TA_BASEFUNS(CtSRAvgSpec);
  //protected:
  //  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtLeabraNetwork : public LeabraNetwork {
  // continuous time leabra network: most abstract version of continous time, using synaptic depression, trial-level inhibitory envelope, and target_minus_sravg attractor reinforcing learning mechanism
INHERITED(LeabraNetwork)
public:
  CtTrialTiming	 ct_time;	// #CAT_Learning timing parameters for ct leabra trial
  CtInhibMod 	 ct_inhib;	// #CAT_Learning inhibition parameters for inhibitory modulations during trial, simulating slow time-constant inhibitory dynamics
  CtSRAvgSpec	 ct_sravg;	// #CAT_Learning parameters controlling computation of sravg value as a function of layer-level delta-activation 

  int		ct_cycle;	// #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW continuous time cycle counter: counts up from start of trial 
  float		cai_max;	// #READ_ONLY #EXPERT #CAT_Statistic mean across entire network of maximum level of cai per unit in incoming connections -- could potentially be used for determining when to learn, though this proves difficult in practice..

  virtual void 	Compute_SRAvg();
  // #CAT_Learning compute sending-receiving activation coproduct averages
  virtual void 	Compute_ActP();
  // #CAT_Learning compute target plus phase activations (snapshot prior to learning)

  virtual void 	Compute_ActPAvgs();
  // #CAT_Learning compute averages of p_act_p values -- needed for preventing learning when layer or unit group is inactive in target phase
  virtual void 	Compute_CtdWt();
  // #CAT_Learning compute ct version of delta weight (actually does same as usual Compute_dWt, but that is overwritten to do nothing to prevent standard programs from breaking things)

  virtual void 	Compute_CtCycle();
  // #CAT_Cycle compute one cycle of continuous-time processing, after activations are updated -- calls above functions according to ct_time parameters

  override void	Init_Counters();
  override void	Init_Stats();
  override void	Cycle_Run();
  override void	Compute_dWt();
  override void	Compute_dWt_NStdLay();

  override void	SetProjectionDefaultTypes(Projection* prjn);

  TA_SIMPLE_BASEFUNS(CtLeabraNetwork);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{}
};


/////////////////////////////////////////////////
//		Inlines

/////////////////////////////////////////////
// 	Synaptic Depression

inline void CtLeabraConSpec::C_Compute_Cai(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su) {
  ca_dep.CaUpdt(cn->cai, ru->act_eq, su->act_eq);
  ca_dep.IntWtUpdt(cn->intwt, cn->wt);
}

inline void CtLeabraConSpec::Compute_Cai(CtLeabraRecvCons* cg, CtLeabraUnit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Cai((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i)));
}

inline void CtLeabraConSpec::C_Depress_Wt(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su) {
  cn->effwt = cn->intwt * ca_dep.SynDep(cn->cai);
}

inline void CtLeabraConSpec::Depress_Wt(CtLeabraRecvCons* cg, CtLeabraUnit* ru) {
  CON_GROUP_LOOP(cg, C_Depress_Wt((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i)));
}

inline void CtLeabraConSpec::C_Compute_CtCycle(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su, float& cai_avg, float& cai_max) {
  C_Compute_Cai(cn, ru, su);
  C_Depress_Wt(cn, ru, su);
  cai_avg += cn->cai;
  cai_max = MAX(cn->cai, cai_max);
}

inline void CtLeabraConSpec::Compute_CtCycle(CtLeabraRecvCons* cg, CtLeabraUnit* ru, float& cai_avg, float& cai_max) {
  CON_GROUP_LOOP(cg, C_Compute_CtCycle((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i), cai_avg, cai_max));
}

/////////////////////////////////////////////
//		Learning

inline void CtLeabraConSpec::C_Compute_SRAvg(CtLeabraCon* cn, CtLeabraUnit* ru,
					     CtLeabraUnit* su, float sravg_wt) {
  cn->sravg += sravg_wt * ru->act_eq * su->act_eq;
}

inline void CtLeabraConSpec::Compute_SRAvg(CtLeabraRecvCons* cg, CtLeabraUnit* ru,
					   CtLeabraNetwork* net, float sravg_wt) {
  CON_GROUP_LOOP(cg, C_Compute_SRAvg((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i),
				     sravg_wt));
}

inline void CtLeabraConSpec::Init_SRAvg(CtLeabraRecvCons* cg, CtLeabraUnit* ru,
					CtLeabraNetwork* net) {
  CON_GROUP_LOOP(cg, ((CtLeabraCon*)cg->Cn(i))->sravg = 0.0f);
}


inline float CtLeabraConSpec::C_Compute_Err(CtLeabraCon* cn, float lin_wt, CtLeabraUnit* ru, 
					    CtLeabraUnit* su, float avg_nrm) {
  float err = (ru->p_act_p * su->p_act_p) - avg_nrm * cn->sravg;
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f - lin_wt);
    else		err *= lin_wt;	
  }
  return err;
}

inline void CtLeabraConSpec::C_Compute_dWt(CtLeabraCon* cn, CtLeabraUnit* ru,
					   float heb, float err) {
  float dwt = lmix.err * err + lmix.hebb * heb;
  cn->dwt += cur_lrate * dwt;
  cn->sravg = 0.0f;
}

inline void CtLeabraConSpec::C_Compute_dWt_NoHebb(CtLeabraCon* cn, CtLeabraUnit* ru, float err) {
  cn->dwt += cur_lrate * err;
  cn->sravg = 0.0f;
}

inline void CtLeabraConSpec::Compute_dWtCt(CtLeabraRecvCons* cg, CtLeabraUnit* ru, 
					   CtLeabraNetwork* net) {
  Compute_SAvgCor(cg, ru);
  // need to do recv layer here because savg_cor.thresh is only here.. could optimize this later
  CtLeabraLayer* rlay = (CtLeabraLayer*)cg->prjn->layer;
  if(rlay->acts_p.avg < savg_cor.thresh) { // if layer not active in target phase, no learn
    Init_SRAvg(cg, ru, net);		  // critical: need to reset this!
    return;
  }
  CtLeabraLayer* lfm = (CtLeabraLayer*)cg->prjn->from.ptr();
  if(lfm->acts_p.avg < savg_cor.thresh) {
    Init_SRAvg(cg, ru, net);		  // critical: need to reset this!
    return;
  }
  if(ru->in_subgp) {
    LeabraUnit_Group* ogp = (LeabraUnit_Group*)ru->owner;
    if(ogp->acts_p.avg < savg_cor.thresh) {
      Init_SRAvg(cg, ru, net);		  // critical: need to reset this!
      return;
    }
  }

  float avg_nrm = 1.0f;
  if(rlay->sravg_sum > 0.0f)
    avg_nrm = 1.0f / rlay->sravg_sum; // normalize by sum of weighting factors, stored on layer

  if(lmix.hebb == 0.0f) {	// hebb is sufficiently infrequent to warrant optimizing
    for(int i=0; i<cg->cons.size; i++) {
      CtLeabraUnit* su = (CtLeabraUnit*)cg->Un(i);
      CtLeabraCon* cn = (CtLeabraCon*)cg->Cn(i);
      if(su->in_subgp) {
	LeabraUnit_Group* ogp = (LeabraUnit_Group*)su->owner;
	if(ogp->acts_p.avg < savg_cor.thresh) {
	  cn->sravg = 0.0f;	// critical: must reset!
	  continue;
	}
      }
      float lin_wt = GetLinFmWt(cn->wt);
      C_Compute_dWt_NoHebb(cn, ru, 
			   C_Compute_Err(cn, lin_wt, ru, su, avg_nrm));
    }
  }
  else {
    for(int i=0; i<cg->cons.size; i++) {
      CtLeabraUnit* su = (CtLeabraUnit*)cg->Un(i);
      CtLeabraCon* cn = (CtLeabraCon*)cg->Cn(i);
      if(su->in_subgp) {
	LeabraUnit_Group* ogp = (LeabraUnit_Group*)su->owner;
	if(ogp->acts_p.avg < savg_cor.thresh) {
	  cn->sravg = 0.0f;	// critical: must reset!
	  continue;
	}
      }
      float lin_wt = GetLinFmWt(cn->wt);
      C_Compute_dWt(cn, ru, 
		    C_Compute_Hebb(cn, cg, lin_wt, ru->p_act_p, su->p_act_p), // note: using p_act_p!
		    C_Compute_Err(cn, lin_wt, ru, su, avg_nrm));
    }
  }
}

inline void CtLeabraConSpec::C_Compute_Weights_Norm(CtLeabraCon* cn, CtLeabraRecvCons* cg,
				    CtLeabraUnit*, CtLeabraUnit*, 
				    CtLeabraUnitSpec*, float dwnorm)
{
  float eff_dw = cn->dwt + dwnorm;
  if(eff_dw != 0.0f) {
    // weight increment happens in linear weights
    cn->wt = GetWtFmLin(GetLinFmWt(cn->wt) + eff_dw);
  }
  cn->pdw = cn->dwt;
  cn->dwt = 0.0f;
}

inline void CtLeabraConSpec::C_Compute_WeightsActReg_Norm(CtLeabraCon* cn,
			  CtLeabraRecvCons* cg, CtLeabraUnit* ru, CtLeabraUnit* su,
			  CtLeabraUnitSpec* rus, float dwnorm)
{
  C_Compute_ActReg(cn, cg, ru, su, rus);
  C_Compute_Weights_Norm(cn, cg, ru, su, rus, dwnorm);
}


inline void CtLeabraConSpec::Compute_Weights(RecvCons* cg, Unit* ru) {
  CtLeabraUnitSpec* rus = (CtLeabraUnitSpec*)ru->GetUnitSpec();
  CtLeabraRecvCons* lcg = (CtLeabraRecvCons*)cg;
  if(dwt_norm.on && lcg->cons.size > 0) {
    lcg->dwt_mean = 0.0f;
    CON_GROUP_LOOP(cg, lcg->dwt_mean += ((LeabraCon*)cg->Cn(i))->dwt);
    lcg->dwt_mean /= (float)lcg->cons.size;
    float dwnorm = -dwt_norm.norm_pct * lcg->dwt_mean;
    if(rus->act_reg.on && !rus->act_reg.bias_only) {	// do this in update so inactive units can be reached (no opt_thresh.updt)
      CON_GROUP_LOOP(cg, C_Compute_WeightsActReg_Norm((CtLeabraCon*)cg->Cn(i), lcg,
			      (CtLeabraUnit*)ru, (CtLeabraUnit*)cg->Un(i), rus, dwnorm));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_Weights_Norm((CtLeabraCon*)cg->Cn(i), lcg,
			(CtLeabraUnit*)ru, (CtLeabraUnit*)cg->Un(i), rus, dwnorm));
    }
  }
  else {
    if(rus->act_reg.on && !rus->act_reg.bias_only) {	// do this in update so inactive units can be reached (no opt_thresh.updt)
      CON_GROUP_LOOP(cg, C_Compute_WeightsActReg((LeabraCon*)cg->Cn(i), lcg,
						 (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_Weights((LeabraCon*)cg->Cn(i), lcg,
					   (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
    }
  }
  //  ApplyLimits(cg, ru); limits are automatically enforced anyway
}


/////////////////////////////////////////////
//		Bias Weights

inline void CtLeabraBiasSpec::B_Compute_SRAvg(CtLeabraCon* cn, CtLeabraUnit* ru,
					      CtLeabraNetwork* net, float sravg_wt) {
  cn->sravg += sravg_wt * ru->act_eq;
}

inline void CtLeabraBiasSpec::B_Compute_dWtCt(CtLeabraCon* cn, CtLeabraUnit* ru,
					      CtLeabraLayer* rlay, CtLeabraNetwork* net) {
  if(rlay->acts_p.avg < savg_cor.thresh) {
    cn->sravg = 0.0f;
    return; // if not active in target phase, no learn
  }
  if(ru->in_subgp) {
    LeabraUnit_Group* ogp = (LeabraUnit_Group*)ru->owner;
    if(ogp->acts_p.avg < savg_cor.thresh) {
      cn->sravg = 0.0f;
      return;
    }
  }

  float avg_nrm = 1.0f;
  if(rlay->sravg_sum > 0.0f)
    avg_nrm = 1.0f / rlay->sravg_sum; // normalize by sum of weighting factors

  float err = ru->p_act_p - avg_nrm * cn->sravg;
  if(fabsf(err) >= dwt_thresh)
    cn->dwt += cur_lrate * err;
  cn->sravg = 0.0f;
}




#endif // leabra_ct_h
