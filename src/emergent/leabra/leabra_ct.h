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
// * present-trains-past version of CHL learning that accumulates send*recv products
//   during settling and uses that for periodic learning
// * NOTE: any overlap in srp_p and srp_m just serves to dilute training signal!
//   because it is a difference.. anything in common just subtracts out.  so,
//   no need to explore exponential integrations, etc -- just keep it simple blocks

class LEABRA_API CtLeabraCon : public LeabraCon {
  // continuous time leabra con: most abstract version of continous time
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (can be depressed) -- used for sending ativation
  float		intwt;		// #NO_SAVE slowly integrating weight value -- exponentially approaches current learned weight value -- mediates between wt and effwt!
  float		cai;		// #NO_SAVE intracellular postsynaptic calcium current integrated over cycles, used for synaptic depression and learning 
  float		sravg;		// #NO_SAVE sender-receiver average coproduct

  CtLeabraCon() { effwt = 0.0f; intwt = 0.0f; cai = 0.0f; sravg = 0.0f; }
};

class LEABRA_API CtCaDepSpec : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(taBase)
public:
  float		intwt_dt;	// #DEF_0.001 time constant for integrating intwt value relative to current weight value

  float		ca_inc;		// #DEF_0.01 time constant for increases in Ca_i (from NMDA etc currents)
  float		ca_dec;		// #DEF_0.01 time constant for decreases in Ca_i (from Ca pumps pushing Ca back out into the synapse)

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

class LEABRA_API CtLearnSpec : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for learning rule
INHERITED(taBase)
public:
  float		sravg_dt;	// time constant for integrating average of sender and receiver activations
  bool		use_sravg_m;	// use sravg as minus phase value

  inline void	SrAvgUpdt(float& sravg, float ru_act, float su_act) {
    sravg += sravg_dt * ((ru_act * su_act) - sravg);
  }

  SIMPLE_COPY(CtLearnSpec);
  TA_BASEFUNS(CtLearnSpec);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CtLeabraConSpec : public LeabraConSpec {
  // continuous time leabra con spec: most abstract version of continous time
INHERITED(LeabraConSpec)
public:
  CtCaDepSpec	ca_dep;		// calcium-based depression of synaptic efficacy and learning rate
  CtLearnSpec	ct_lrn;		// special ct-based learning parameters
  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  inline void C_Compute_Cai(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su);
  // connection-level Cai update
  inline void Compute_Cai(LeabraRecvCons* cg, CtLeabraUnit* ru);
  // connection-group level Cai update

  // connection-level synaptic depression: syn dep direct
  inline void C_Depress_Wt(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su);
  // connection-level synaptic depression: ca mediated
  inline void Depress_Wt(LeabraRecvCons* cg, CtLeabraUnit* ru);
  // connection-group level synaptic depression

  inline void C_Compute_CtCycle(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su,
				float& cai_avg, float& cai_max);
  // one cycle of updating at connection-level 
  inline void Compute_CtCycle(LeabraRecvCons* cg, CtLeabraUnit* ru,
			      float& cai_avg, float& cai_max);
  // one cycle of processing at a Ct synapse -- expensive!!  todo: need to find ways to optimize

  inline void C_Compute_SrAvg(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su);
  // update sending-receiving average activation
  inline void Compute_SrAvg(LeabraRecvCons* cg, CtLeabraUnit* ru);
  // update sending-receiving average activation
  
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
    lcn->sravg = 0.0f;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  // 		Following are all standard code revised to use effwt instead of wt
  
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

  /////////////////////////////////////////////////////////////////////////////////////
  // 		Special learning functions
  
  inline void  C_Compute_dWt(CtLeabraCon* cn, LeabraUnit* ru, float heb, float err);

  inline float C_Compute_Err(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su);

  inline void Compute_dWt(RecvCons* cg, Unit* ru);

  inline void  C_Compute_dWtFlip(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su);
  // compute flipped version of dwt (plus-minus reversed)
  inline void Compute_dWtFlip(LeabraRecvCons* cg, CtLeabraUnit* ru);
  // compute flipped version of dwt (plus-minus reversed)

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

  void	Compute_CtCycle(CtLeabraUnit* ru, float& cai_avg, float& cai_max)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_CtCycle(this, ru, cai_avg, cai_max); }
  // #CAT_Learning compute one cycle of continuous time processing
  void	Compute_SrAvg(CtLeabraUnit* ru)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_SrAvg(this, ru); }
  // #CAT_Learning update sending-receiving average activation
  void	Compute_dWtFlip(CtLeabraUnit* ru)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_dWtFlip(this, ru); }
  // #CAT_Learning compute flipped version of dwt (plus-minus reversed)

  TA_BASEFUNS_NOCOPY(CtLeabraRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

// TODO: rederive based on std conspec and support sravg if it works..

class LEABRA_API CtLeabraBiasSpec : public LeabraBiasSpec {
  // continuous time leabra bias spec: most abstract version of continous time
INHERITED(LeabraBiasSpec)
public:
  override inline void B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru);

  TA_BASEFUNS_NOCOPY(CtLeabraBiasSpec);
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
  virtual void 	Compute_SrAvg(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning update sending-receiving average activation
  virtual void 	Compute_dWtFlip(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute flipped version of dwt (plus-minus reversed)

  virtual void 	Compute_ActMP(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)
  virtual void 	Compute_ActM(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute minus phase activations (snapshot prior to learning)
  virtual void 	Compute_ActP(CtLeabraUnit* u, CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute plus phase activations (snapshot prior to learning)

  TA_BASEFUNS(CtLeabraUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API CtLeabraUnit : public DaModUnit {
  // continuous time leabra unit: most abstract version of continous time
INHERITED(DaModUnit)
public:
  float		cai_avg;	// #NO_SAVE average level of cai in my incoming connections -- just for analysis and debugging in early development -- remove later
  float		cai_max;	// #NO_SAVE maximum level of cai in my incoming connections -- just for analysis and debugging in early development -- remove later
  float		syndep_avg;	// #NO_SAVE average level of synaptic depression in my incoming connections -- just for analysis and debugging in early development -- remove later
  float		syndep_max;	// #NO_SAVE maximum level of synaptic depression in my incoming connections -- just for analysis and debugging in early development -- remove later

  void		Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_CtCycle(this, lay, net); }
  // #CAT_Learning compute one cycle of continuous time processing
  void		Compute_SrAvg(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_SrAvg(this, lay, net); }
  // #CAT_Learning update sending-receiving average activation
  void		Compute_dWtFlip(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_dWtFlip(this, lay, net); }
  // #CAT_Learning compute flipped version of dwt (plus-minus reversed)

  void		Compute_ActMP(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_ActMP(this, lay, net); }
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)
  void		Compute_ActM(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_ActM(this, lay, net); }
  // #CAT_Learning compute minus phase activations (snapshot prior to learning)
  void		Compute_ActP(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_ActP(this, lay, net); }
  // #CAT_Learning compute plus phase activations (snapshot prior to learning)

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
  virtual void 	Compute_SrAvg(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning update sending-receiving average activation
  virtual void 	Compute_dWtFlip(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute flipped version of dwt (plus-minus reversed)

  virtual void 	Compute_ActMP(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)
  virtual void 	Compute_ActM(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute minus phase activations (snapshot prior to learning)
  virtual void 	Compute_ActP(CtLeabraLayer* lay, CtLeabraNetwork* net);
  // #CAT_Learning compute plus phase activations (snapshot prior to learning)

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
  void 	Compute_CtCycle(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_CtCycle(this, net); };
  // #CAT_Learning compute one cycle of continuous time processing
  void 	Compute_SrAvg(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_SrAvg(this, net); };
  // #CAT_Learning update sending-receiving average activation
  void 	Compute_dWtFlip(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_dWtFlip(this, net); };
  // #CAT_Learning compute flipped version of dwt (plus-minus reversed)

  void 	Compute_ActMP(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_ActMP(this, net); };
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)
  void 	Compute_ActM(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_ActM(this, net); };
  // #CAT_Learning compute minus phase activations (snapshot prior to learning)
  void 	Compute_ActP(CtLeabraNetwork* net) 
  { ((CtLeabraLayerSpec*)spec.SPtr())->Compute_ActP(this, net); };
  // #CAT_Learning compute plus phase activations (snapshot prior to learning)

  TA_BASEFUNS_NOCOPY(CtLeabraLayer);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	Ct Network
//////////////////////////////////

class LEABRA_API CtLeabraNetwork : public LeabraNetwork {
  // continuous time leabra network: most abstract version of continous time -- uses tick counter to mark when weight updates are performed
INHERITED(LeabraNetwork)
public:
  int		cycles_per_tick;  // #DEF_10 #CAT_Counter number of cycles to perform per each tick of processing, where a tick consists of one weight update based on continuously sampled activation states

  virtual void 	Compute_CtCycle() ;
  // #CAT_Cycle compute one cycle of continuous-time processing, after activations are updated
  virtual void 	Compute_SrAvg() ;
  // #CAT_Cycle update sending-receiving average activation
  virtual void 	Compute_dWtFlip() ;
  // #CAT_Learning compute flipped version of dwt (plus-minus reversed)
  override void	Cycle_Run();

  virtual void 	Compute_ActMP() ;
  // #CAT_Learning compute minus and plus phase activations (snapshot prior to learning)
  virtual void 	Compute_ActM() ;
  // #CAT_Learning compute minus phase activations (snapshot prior to learning)
  virtual void 	Compute_ActP() ;
  // #CAT_Learning compute plus phase activations (snapshot prior to learning)

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

inline void CtLeabraConSpec::C_Compute_Cai(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su) {
  ca_dep.CaUpdt(cn->cai, ru->act_eq, su->act_eq);
  ca_dep.IntWtUpdt(cn->intwt, cn->wt);
}

inline void CtLeabraConSpec::Compute_Cai(LeabraRecvCons* cg, CtLeabraUnit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_Cai((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i)));
}

inline void CtLeabraConSpec::C_Depress_Wt(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su) {
  cn->effwt = cn->intwt * ca_dep.SynDep(cn->cai);
}

inline void CtLeabraConSpec::Depress_Wt(LeabraRecvCons* cg, CtLeabraUnit* ru) {
  CON_GROUP_LOOP(cg, C_Depress_Wt((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i)));
}

inline void CtLeabraConSpec::C_Compute_CtCycle(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su, float& cai_avg, float& cai_max) {
  C_Compute_Cai(cn, ru, su);
  C_Depress_Wt(cn, ru, su);
  cai_avg += cn->cai;
  cai_max = MAX(cn->cai, cai_max);
}

inline void CtLeabraConSpec::Compute_CtCycle(LeabraRecvCons* cg, CtLeabraUnit* ru, float& cai_avg, float& cai_max) {
  CON_GROUP_LOOP(cg, C_Compute_CtCycle((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i), cai_avg, cai_max));
}

inline void CtLeabraConSpec::C_Compute_SrAvg(CtLeabraCon* cn, CtLeabraUnit* ru, CtLeabraUnit* su) {
  ct_lrn.SrAvgUpdt(cn->sravg, ru->act_eq, su->act_eq);
}

inline void CtLeabraConSpec::Compute_SrAvg(LeabraRecvCons* cg, CtLeabraUnit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_SrAvg((CtLeabraCon*)cg->Cn(i), ru, (CtLeabraUnit*)cg->Un(i)));
}

inline void CtLeabraBiasSpec::B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru) {
  CtLeabraUnit* lru = (CtLeabraUnit*)ru;
  float err = lru->p_act_p - lru->p_act_m;
  if(fabsf(err) >= dwt_thresh)
    cn->dwt += cur_lrate * err;
}

inline float CtLeabraConSpec::C_Compute_Err(CtLeabraCon* cn, CtLeabraUnit* ru,
					    CtLeabraUnit* su) {
  float err;
  if(ct_lrn.use_sravg_m) {
    err = (ru->act_eq * su->act_eq) - cn->sravg; // note: does not require any saved state!!
  }
  else {
    err = (ru->p_act_p * su->p_act_p) - (ru->p_act_m * su->p_act_m);
  }
  // wt is negative in linear form, so using opposite sign of usual here
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f + cn->wt);
    else		err *= -cn->wt;	
  }
  return err;
}

inline void CtLeabraConSpec::C_Compute_dWt(CtLeabraCon* cn, LeabraUnit*, float heb, float err) {
  float dwt = lmix.err * err + lmix.hebb * heb;
  cn->dwt += cur_lrate * dwt;
}

inline void CtLeabraConSpec::Compute_dWt(RecvCons* cg, Unit* ru) {
  CtLeabraUnit* lru = (CtLeabraUnit*)ru;
  LeabraRecvCons* lcg = (LeabraRecvCons*) cg;
  Compute_SAvgCor(lcg, lru);
  for(int i=0; i<cg->cons.size; i++) {
    CtLeabraUnit* su = (CtLeabraUnit*)cg->Un(i);
    CtLeabraCon* cn = (CtLeabraCon*)cg->Cn(i);
    float orig_wt = cn->wt;
    C_Compute_LinFmWt(lcg, cn); // get weight into linear form
    C_Compute_dWt(cn, lru, 
		  C_Compute_Hebb(cn, lcg, lru->p_act_p, su->p_act_p), // note: using p_act_p!
		  C_Compute_Err(cn, lru, su));  
    cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
  }
}

inline void CtLeabraConSpec::C_Compute_dWtFlip(CtLeabraCon* cn, CtLeabraUnit* ru,
						CtLeabraUnit* su) {
  // note: no hebbian or anything here -- just pure flipped dwt!
  float err;
  if(ct_lrn.use_sravg_m) {
    err = cn->sravg - (ru->act_eq * su->act_eq); // note: does not require any saved state!!
  }
  else {
    err = (ru->p_act_m * su->p_act_m) - (ru->p_act_p * su->p_act_p);
  }
  // wt is negative in linear form, so using opposite sign of usual here
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f + cn->wt);
    else		err *= -cn->wt;	
  }
  cn->dwt += cur_lrate * err;
}

inline void CtLeabraConSpec::Compute_dWtFlip(LeabraRecvCons* cg, CtLeabraUnit* ru) {
  for(int i=0; i<cg->cons.size; i++) {
    CtLeabraUnit* su = (CtLeabraUnit*)cg->Un(i);
    CtLeabraCon* cn = (CtLeabraCon*)cg->Cn(i);
    float orig_wt = cn->wt;
    C_Compute_LinFmWt(cg, cn); // get weight into linear form
    C_Compute_dWtFlip(cn, ru, su);  
    cn->wt = orig_wt; // restore original value; note: no need to convert there-and-back for dwt, saves numerical lossage!
  }
}

#endif // leabra_ct_h
