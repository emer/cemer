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
  float		cai;		// #NO_SAVE intracellular postsynaptic calcium current integrated over cycles, used for synaptic depression and learning 

  CtLeabraCon() { effwt = 0.0f; cai = 0.0f; }
};

class LEABRA_API CtCaDepSpec : public taBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(taBase)
public:
  float		ca_inc;		// time constant for increases in Ca_i (from NMDA etc currents)
  float		ca_dec;		// time constant for decreases in Ca_i (from Ca pumps pushing Ca back out into the synapse)
  bool		ca_effdrive;	// include effwt in the calcium driving equations?

  bool		old_sd;		// use old syndep eqs!
  float		rec;		// #DEF_0.002 rate of recovery from depression
  float		asymp_act;	// #DEF_0.4 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl rate value)
  float		depl;		// #READ_ONLY #SHOW rate of depletion of synaptic efficacy as a function of sender-receiver activations (computed from rec, asymp_act)
  

  bool		sd_sq;		// square the cai value for syndep
  float		sd_ca_thr;	// synaptic depression ca threshold: only when ca_i has increased by this amount (thus synaptic ca depleted) does it affect firing rates and thus synaptic depression
  float		sd_ca_gain;	// multiplier on cai value for computing synaptic depression -- modulates overall level of depression independent of rate parameters
  float		sd_ca_thr_rescale; // #READ_ONLY rescaling factor taking into account sd_ca_gain and sd_ca_thr (= sd_ca_gain/(1 - sd_ca_thr))

  float		lrd_ca_thr;	// learning rate depression ca threshold: only when ca_i has increased by this amount (thus synaptic ca depleted) does it affect subsequent learning ability
  float		lrd_ca_gain;	// multiplier on cai value for computing learning rate depression -- modulates overall level of depression independent of rate parameters
  float		lrd_ca_thr_rescale; // #READ_ONLY rescaling factor taking into account lrd_ca_gain and lrd_ca_thr (= lrd_ca_gain/(1 - lrd_ca_thr))

  inline void	CaUpdt(float& cai, float effwt, float ru_act, float su_act) {
    float drive = ru_act * su_act;
    if(ca_effdrive)
      drive *= effwt;
    cai += ca_inc * (1.0f - cai) * drive - ca_dec * cai;
  }

  inline float	SynDep(float cai, float effwt, float wt, float ru_act, float su_act) {
    if(old_sd) {
      float drive = ru_act * su_act * effwt;
      float deff = rec * (wt - effwt) - depl * drive;
      float rval = effwt + deff;
      if(rval > wt) rval = wt;
      if(rval < 0.0f) rval = 0.0f;
      return rval;
    }
    else {
      float cao_thr = (cai > sd_ca_thr) ? (1.0 - sd_ca_thr_rescale * (cai - sd_ca_thr)) : 1.0f;
      if(sd_sq) return wt * cao_thr * cao_thr;
      else      return wt * cao_thr;	
    }
  }

  inline float	LrateDep(float cai) {
    float cao_thr = (cai > lrd_ca_thr) ? (1.0 - lrd_ca_thr_rescale * (cai - lrd_ca_thr)) : 1.0f;
    return cao_thr * cao_thr; // squared
  }

  SIMPLE_COPY(CtCaDepSpec);
  TA_BASEFUNS(CtCaDepSpec);
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
  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  void C_Compute_Cai(CtLeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ca_dep.CaUpdt(cn->cai, cn->effwt, ru->act_eq, su->act_eq);
  }
  // connection-level Cai update
  virtual void Compute_Cai(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_Cai((CtLeabraCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }
  // connection-group level Cai update

  void C_Depress_Wt(CtLeabraCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    cn->effwt = ca_dep.SynDep(cn->cai, cn->effwt, cn->wt, ru->act_eq, su->act_eq);
  }
  // connection-level synaptic depression
  virtual void Depress_Wt(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Depress_Wt((CtLeabraCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }
  // connection-group level synaptic depression

  inline void C_Compute_CtCycle(CtLeabraCon* cn, LeabraUnit* ru, LeabraUnit* su, float& cai_avg, float& cai_max) {
    C_Compute_Cai(cn, ru, su);
    C_Depress_Wt(cn, ru, su);
    cai_avg += cn->cai;
    cai_max = MAX(cn->cai, cai_max);
  }
  // one cycle of updating at connection-level 
  virtual void Compute_CtCycle(LeabraRecvCons* cg, LeabraUnit* ru, float& cai_avg, float& cai_max) {
    CON_GROUP_LOOP(cg, C_Compute_CtCycle((CtLeabraCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i), cai_avg, cai_max));
  }
  // one cycle of processing at a Ct synapse -- expensive!!  todo: need to find ways to optimize
  
  void C_Reset_EffWt(CtLeabraCon* cn) {
    cn->effwt = cn->wt;
  }
  virtual void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CtLeabraCon*)cg->Cn(i)));
  }
  virtual void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CtLeabraCon*)cg->Cn(i)));
  }

  void 	C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    CtLeabraCon* lcn = (CtLeabraCon*)cn; lcn->effwt = lcn->wt;
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

  void	Compute_CtCycle(LeabraUnit* ru, float& cai_avg, float& cai_max)
  { ((CtLeabraConSpec*)GetConSpec())->Compute_CtCycle(this, ru, cai_avg, cai_max); }
  // #CAT_Learning compute one cycle of continuous time processing

  TA_BASEFUNS_NOCOPY(CtLeabraRecvCons);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

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
  float		lrdep_avg;	// #NO_SAVE average level of lrate depression in my incoming connections -- just for analysis and debugging in early development -- remove later
  float		lrdep_max;	// #NO_SAVE maximum level of lrate depression in my incoming connections -- just for analysis and debugging in early development -- remove later

  void		Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net)
  { ((CtLeabraUnitSpec*)GetUnitSpec())->Compute_CtCycle(this, lay, net); }
  // #CAT_Learning compute one cycle of continuous time processing
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

inline void CtLeabraBiasSpec::B_Compute_dWt(LeabraCon* cn, LeabraUnit* ru) {
  CtLeabraUnit* lru = (CtLeabraUnit*)ru;
  float err = lru->p_act_p - lru->p_act_p;
  if(fabsf(err) >= dwt_thresh)
    cn->dwt += cur_lrate * err;
}

inline float CtLeabraConSpec::C_Compute_Err(CtLeabraCon* cn, CtLeabraUnit* ru,
					    CtLeabraUnit* su) {
  float err = (ru->p_act_p * su->p_act_p) - (ru->p_act_m * su->p_act_m);
  // wt is negative in linear form, so using opposite sign of usual here
  if(lmix.err_sb) {
    if(err > 0.0f)	err *= (1.0f + cn->wt);
    else		err *= -cn->wt;	
  }
  return err;
}

inline void CtLeabraConSpec::C_Compute_dWt(CtLeabraCon* cn, LeabraUnit*, float heb, float err) {
  float dwt = lmix.err * err + lmix.hebb * heb;
  cn->dwt += cur_lrate * dwt; // ca_dep.LrateDep(cn->cai) * dwt;
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

#endif // leabra_ct_h
