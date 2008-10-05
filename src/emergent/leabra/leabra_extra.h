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

// leabra.h 

#ifndef leabra_extra_h
#define leabra_extra_h

#include "leabra.h"
#include "netstru_extra.h"
#include "ta_imgproc.h"

// extra specialized classes for variations on the Leabra algorithm

class LEABRA_API MarkerConSpec : public LeabraConSpec {
  // connection spec that marks special projections: doesn't send netin or adapt weights
INHERITED(LeabraConSpec)
public:
  // don't send regular net inputs or learn!
  inline float 	Compute_Netin(RecvCons*, Unit*) { return 0.0f; }
  inline void 	Send_Netin(RecvCons*, float su_act) { };
  inline void 	Send_NetinDelta(LeabraRecvCons*, float su_act_delta) { };
  inline void 	Send_ClampNet(LeabraRecvCons*, float su_act) { };
  inline void 	Compute_dWt(RecvCons*, Unit*) { };
  inline void 	Compute_dWt_LeabraCHL(LeabraRecvCons*, LeabraUnit*) { };
  inline void 	Compute_dWt_CtLeabraCAL(LeabraRecvCons*, LeabraUnit*) { };
  inline void 	Compute_SRAvg(LeabraRecvCons*, LeabraUnit*) { };
  inline void	Compute_Weights(RecvCons*, Unit*) { };
  inline void	Compute_Weights_LeabraCHL(LeabraRecvCons*, LeabraUnit*) { };
  inline void	Compute_Weights_CtLeabraCAL(LeabraRecvCons*, LeabraUnit*) { };

  bool	 DMem_AlwaysLocal() { return true; }
  // these connections always need to be there on all nodes..

  TA_BASEFUNS_NOCOPY(MarkerConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////////////
// 	Context Layer for Sequential	//
//////////////////////////////////////////

class LEABRA_API CtxtUpdateSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra context updating specifications
INHERITED(taOBase)
public:
  float		fm_hid;		// from hidden (inputs to context layer)
  float		fm_prv;		// from previous context layer values (maintenance)
  float		to_out;		// outputs from context layer

  SIMPLE_COPY(CtxtUpdateSpec);
  TA_BASEFUNS(CtxtUpdateSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraContextLayerSpec : public LeabraLayerSpec {
  // context layer that copies from its recv projection (like an input layer)
INHERITED(LeabraLayerSpec)
public:
  CtxtUpdateSpec updt;		// ctxt updating constants: from hidden, from previous values (hysteresis), outputs from context (n/a on simple gate layer)

  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // clamp from act_p values of sending layer

  virtual void Compute_Context(LeabraLayer* lay, LeabraUnit* u, LeabraNetwork* net);
  // get context source value for given context unit

  override bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  void	Compute_SRAvg(LeabraLayer* lay, LeabraNetwork* net) { };
  void	Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) { };
  // no learn

  void	Defaults();

  TA_SIMPLE_BASEFUNS(LeabraContextLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
// 	Misc Special Objects		//
//////////////////////////////////////////

class LEABRA_API LeabraLinUnitSpec : public LeabraUnitSpec {
  // a pure linear unit (suitable for an AC unit spec unit)
INHERITED(LeabraUnitSpec)
public:
  void 	Compute_ActFmVm(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net);
  
  void	Defaults();

  TA_BASEFUNS(LeabraLinUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API LeabraNegBiasSpec : public LeabraBiasSpec {
  // only learns negative bias changes, not positive ones (decay restores back to zero)
INHERITED(LeabraBiasSpec)
public:
  float		decay;		// rate of weight decay towards zero 
  bool		updt_immed;	// update weights immediately when weights are changed

  inline void	B_Compute_Weights(LeabraCon* cn, LeabraUnit* ru, LeabraUnitSpec* rus) {
    if(rus->act_reg.on) {		// do this in update so inactive units can be reached (no opt_thresh.updt)
      if(ru->act_avg <= rus->act_reg.min)
	cn->dwt += cur_lrate * rus->act_reg.inc_wt;
      else if(ru->act_avg >= rus->act_reg.max)
	cn->dwt -= cur_lrate * rus->act_reg.dec_wt;
    }
    if(cn->dwt > 0.0f)		// positive only
      cn->dwt = 0.0f;
    cn->dwt -= decay * cn->wt;
    cn->pdw = cn->dwt;
    cn->wt += cn->dwt;
    cn->dwt = 0.0f;
    C_ApplyLimits(cn, ru, NULL);
  }

  inline void	B_Compute_dWt_LeabraCHL(LeabraCon* cn, LeabraUnit* ru) {
    LeabraBiasSpec::B_Compute_dWt_LeabraCHL(cn, ru);
    if(updt_immed) B_Compute_Weights(cn, ru, (LeabraUnitSpec*)ru->GetUnitSpec());
  }
  inline void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    LeabraBiasSpec::B_Compute_dWt_CtLeabraCAL(cn, ru, rlay);
    if(updt_immed) B_Compute_Weights(cn, ru, (LeabraUnitSpec*)ru->GetUnitSpec());
  }

  SIMPLE_COPY(LeabraNegBiasSpec);
  TA_BASEFUNS(LeabraNegBiasSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
// 	Synaptic Depression: Trial Level

class LEABRA_API TrialSynDepCon : public LeabraCon {
  // synaptic depression connection at the trial level (as opposed to cycle level)
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (can be depressed) -- used for sending ativation

  TrialSynDepCon() { effwt = 0.0f; }
};

class LEABRA_API TrialSynDepSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(taOBase)
public:
  float		rec;		// #DEF_1 rate of recovery from depression
  float		depl;		// #DEF_1.1 rate of depletion of synaptic efficacy as a function of sender-receiver activations

  SIMPLE_COPY(TrialSynDepSpec);
  TA_BASEFUNS(TrialSynDepSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API TrialSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the trial level (as opposed to cycle level)
INHERITED(LeabraConSpec)
public:
  TrialSynDepSpec	syn_dep;	// synaptic depression specifications

  void C_Depress_Wt(TrialSynDepCon* cn, LeabraUnit*, LeabraUnit* su) {
    // NOTE: fctn of sender act and wt: could be just sender or sender*recv.. 
    float deff = syn_dep.rec * (cn->wt - cn->effwt) - syn_dep.depl * su->act_eq * cn->wt;
    cn->effwt += deff;
    if(cn->effwt > cn->wt) cn->effwt = cn->wt;
    if(cn->effwt < wt_limits.min) cn->effwt = wt_limits.min;
  }
  virtual void Depress_Wt(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Depress_Wt((TrialSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }

  void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_dWt_LeabraCHL(cg, ru);
    Depress_Wt(cg, ru);
  }
  void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_dWt_CtLeabraCAL(cg, ru);
    Depress_Wt(cg, ru);
  }

  void C_Reset_EffWt(TrialSynDepCon* cn) {
    cn->effwt = cn->wt;
  }
  virtual void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((TrialSynDepCon*)cg->Cn(i)));
  }
  virtual void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((TrialSynDepCon*)cg->Cn(i)));
  }

  void 	C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    TrialSynDepCon* lcn = (TrialSynDepCon*)cn; lcn->effwt = lcn->wt;
  }

  float C_Compute_Netin(TrialSynDepCon* cn, Unit*, Unit* su) {
    return cn->effwt * su->act;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((TrialSynDepCon*)cg->Cn(i), ru, cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }
  void C_Send_Inhib(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->gc.i += su_act_eff * cn->effwt;
  }
  void C_Send_Netin(LeabraSendCons* cg, TrialSynDepCon* cn, Unit* ru, float su_act_eff) {
    ru->net += su_act_eff * cn->effwt;
  }
  void Send_Netin(SendCons* cg, float su_act) {
    // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
    // LeabraUnitSpec::CheckConfig checks that this is ok.
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_Inhib((LeabraSendCons*)cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_Netin((LeabraSendCons*)cg, (TrialSynDepCon*)cg->Cn(i), cg->Un(i), su_act_eff));
    }
  }

  void C_Send_InhibDelta(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += su_act_delta_eff * cn->effwt;
  }
  void C_Send_NetinDelta(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += su_act_delta_eff * cn->effwt;
  }
  void Send_NetinDelta(LeabraSendCons* cg, float su_act_delta) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act_delta;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta(cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
  }
  void C_Send_ClampNet(LeabraSendCons* cg, TrialSynDepCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->clmp_net += su_act_eff * cn->effwt;
  }
  void Send_ClampNet(LeabraSendCons* cg, float su_act) {
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
    CON_GROUP_LOOP(cg, C_Send_ClampNet(cg, (TrialSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
  }

  TA_SIMPLE_BASEFUNS(TrialSynDepConSpec);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
};

// todo: the following functions need a hook to activate depression!

////////////////////////////////////////////////////////////////
//      Synaptic Depression: Cycle Level, simple

class LEABRA_API CycleSynDepCon : public LeabraCon {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation

  CycleSynDepCon() { effwt = 0.0f; }
};

class LEABRA_API CycleSynDepSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(taOBase)
public:
  float		rec;		// #DEF_0.002 rate of recovery from depression
  float		asymp_act;	// #DEF_0.4 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl rate value)
  float		depl;		// #READ_ONLY #SHOW rate of depletion of synaptic efficacy as a function of sender-receiver activations (computed from rec, asymp_act)

  inline void	Depress(float& effwt, float wt, float ru_act, float su_act) {
    float drive = ru_act * su_act * effwt;
    float deff = rec * (wt - effwt) - depl * drive;
    effwt += deff;
    if(effwt > wt) effwt = wt;
    if(effwt < 0.0f) effwt = 0.0f;
  }

  SIMPLE_COPY(CycleSynDepSpec);
  TA_BASEFUNS(CycleSynDepSpec);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CycleSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraConSpec)
public:
  CycleSynDepSpec	syn_dep;	// synaptic depression specifications

  inline void C_Compute_CycSynDep(CycleSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    syn_dep.Depress(cn->effwt, cn->wt, ru->act_eq, su->act_eq);
  }
  inline override void Compute_CycSynDep(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CycleSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }

  void C_Reset_EffWt(CycleSynDepCon* cn) {
    cn->effwt = cn->wt;
  }
  virtual void Reset_EffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CycleSynDepCon*)cg->Cn(i)));
  }
  virtual void Reset_EffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Reset_EffWt((CycleSynDepCon*)cg->Cn(i)));
  }

  void 	C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    CycleSynDepCon* lcn = (CycleSynDepCon*)cn; lcn->effwt = lcn->wt;
  }

  float C_Compute_Netin(CycleSynDepCon* cn, Unit*, Unit* su) {
    return cn->effwt * su->act;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CycleSynDepCon*)cg->Cn(i), ru, cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }
  void C_Send_Inhib(LeabraSendCons* cg, CycleSynDepCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->gc.i += su_act_eff * cn->effwt;
  }
  void C_Send_Netin(LeabraSendCons* cg, CycleSynDepCon* cn, Unit* ru, float su_act_eff) {
    ru->net += su_act_eff * cn->effwt;
  }
  void Send_Netin(SendCons* cg, float su_act) {
    // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
    // LeabraUnitSpec::CheckConfig checks that this is ok.
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_Inhib((LeabraSendCons*)cg, (CycleSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_Netin((LeabraSendCons*)cg, (CycleSynDepCon*)cg->Cn(i), cg->Un(i), su_act_eff));
    }
  }

  void C_Send_InhibDelta(LeabraSendCons* cg, CycleSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += su_act_delta_eff * cn->effwt;
  }
  void C_Send_NetinDelta(LeabraSendCons* cg, CycleSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += su_act_delta_eff * cn->effwt;
  }
  void Send_NetinDelta(LeabraSendCons* cg, float su_act_delta) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act_delta;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (CycleSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta(cg, (CycleSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
  }
  void C_Send_ClampNet(LeabraSendCons* cg, CycleSynDepCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->clmp_net += su_act_eff * cn->effwt;
  }
  void Send_ClampNet(LeabraSendCons* cg, float su_act) {
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
    CON_GROUP_LOOP(cg, C_Send_ClampNet(cg, (CycleSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
  }

  TA_SIMPLE_BASEFUNS(CycleSynDepConSpec);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////////////
//      Synaptic Depression: Cycle Level, Ca-Based

class LEABRA_API CaiSynDepCon : public LeabraCon {
  // synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraCon)
public:
  float		effwt;		// #NO_SAVE effective weight value (subject to synaptic depression) -- used for sending activation
  float		cai;		// #NO_SAVE intracellular postsynaptic calcium current integrated over cycles, used for synaptic depression

  CaiSynDepCon() { effwt = 0.0f; cai = 0.0f; }
};

class LEABRA_API CaiSynDepSpec : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(taOBase)
public:
  float		ca_inc;		// #DEF_0.2 time constant for increases in Ca_i (from NMDA etc currents) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)
  float		ca_dec;		// #DEF_0.2 time constant for decreases in Ca_i (from Ca pumps pushing Ca back out into the synapse) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)

  float		sd_ca_thr;	// #DEF_0.2 synaptic depression ca threshold: only when ca_i has increased by this amount (thus synaptic ca depleted) does it affect firing rates and thus synaptic depression
  float		sd_ca_gain;	// #DEF_0.3 multiplier on cai value for computing synaptic depression -- modulates overall level of depression independent of rate parameters
  float		sd_ca_thr_rescale; // #READ_ONLY rescaling factor taking into account sd_ca_gain and sd_ca_thr (= sd_ca_gain/(1 - sd_ca_thr))

  inline void	CaUpdt(float& cai, float ru_act, float su_act) {
    float drive = ru_act * su_act;
    cai += ca_inc * (1.0f - cai) * drive - ca_dec * cai;
  }

  inline float	SynDep(float cai) {
    float cao_thr = (cai > sd_ca_thr) ? (1.0 - sd_ca_thr_rescale * (cai - sd_ca_thr)) : 1.0f;
    return cao_thr * cao_thr;
  }

  SIMPLE_COPY(CaiSynDepSpec);
  TA_BASEFUNS(CaiSynDepSpec);
protected:
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API CaiSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraConSpec)
public:
  CaiSynDepSpec	ca_dep;		// calcium-based depression of synaptic efficacy
  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  inline void C_Compute_Cai(CaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    ca_dep.CaUpdt(cn->cai, ru->act_eq, su->act_eq);
  }
  // connection-level Cai update
  inline void Compute_Cai(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_Cai((CaiSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }

  // connection-level synaptic depression: syn dep direct
  inline void C_Compute_CycSynDep(CaiSynDepCon* cn, LeabraUnit* ru, LeabraUnit* su) {
    cn->effwt = cn->wt * ca_dep.SynDep(cn->cai);
  }
  // connection-level synaptic depression: ca mediated
  inline override void Compute_CycSynDep(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_Cai(cg, ru);
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep((CaiSynDepCon*)cg->Cn(i), ru, (LeabraUnit*)cg->Un(i)));
  }
  // connection-group level synaptic depression

  /////////////////////////////////////////////////////////////////////////////////////
  // 		Following are all standard code revised to use effwt instead of wt
  
  inline void C_Init_SdEffWt(CaiSynDepCon* cn) {
    cn->effwt = cn->wt; cn->cai = 0.0f; 
  }
  inline void Init_SdEffWt(LeabraRecvCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((CaiSynDepCon*)cg->Cn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)
  inline void Init_SdEffWt(LeabraSendCons* cg) {
    CON_GROUP_LOOP(cg, C_Init_SdEffWt((CaiSynDepCon*)cg->Cn(i)));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)

  override void C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    CaiSynDepCon* lcn = (CaiSynDepCon*)cn; lcn->effwt = lcn->wt;
    lcn->cai = 0.0f; 
  }

  float C_Compute_Netin(CaiSynDepCon* cn, Unit*, Unit* su) {
    return cn->effwt * su->act;
  }
  float Compute_Netin(RecvCons* cg, Unit* ru) {
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin((CaiSynDepCon*)cg->Cn(i), ru, cg->Un(i)));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }
  void C_Send_Inhib(LeabraSendCons* cg, CaiSynDepCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->gc.i += su_act_eff * cn->effwt;
  }
  void C_Send_Netin(LeabraSendCons* cg, CaiSynDepCon* cn, Unit* ru, float su_act_eff) {
    ru->net += su_act_eff * cn->effwt;
  }
  void Send_Netin(SendCons* cg, float su_act) {
    // apply scale based only on first unit in con group: saves lots of redundant mulitplies!
    // LeabraUnitSpec::CheckConfig checks that this is ok.
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_Inhib((LeabraSendCons*)cg, (CaiSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_Netin((LeabraSendCons*)cg, (CaiSynDepCon*)cg->Cn(i), cg->Un(i), su_act_eff));
    }
  }

  void C_Send_InhibDelta(LeabraSendCons* cg, CaiSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->g_i_delta += su_act_delta_eff * cn->effwt;
  }
  void C_Send_NetinDelta(LeabraSendCons* cg, CaiSynDepCon* cn, LeabraUnit* ru, float su_act_delta_eff) {
    ru->net_delta += su_act_delta_eff * cn->effwt;
  }
  void Send_NetinDelta(LeabraSendCons* cg, float su_act_delta) {
    Unit* ru = cg->Un(0);
    float su_act_delta_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act_delta;
    if(inhib) {
      CON_GROUP_LOOP(cg, C_Send_InhibDelta(cg, (CaiSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
    else {
      CON_GROUP_LOOP(cg, C_Send_NetinDelta(cg, (CaiSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_delta_eff));
    }
  }
  void C_Send_ClampNet(LeabraSendCons* cg, CaiSynDepCon* cn, LeabraUnit* ru, float su_act_eff) {
    ru->clmp_net += su_act_eff * cn->effwt;
  }
  void Send_ClampNet(LeabraSendCons* cg, float su_act) {
    Unit* ru = cg->Un(0);
    float su_act_eff = ((LeabraRecvCons*)ru->recv.FastEl(cg->recv_idx))->scale_eff * su_act;
    CON_GROUP_LOOP(cg, C_Send_ClampNet(cg, (CaiSynDepCon*)cg->Cn(i), (LeabraUnit*)cg->Un(i), su_act_eff));
  }

  TA_SIMPLE_BASEFUNS(CaiSynDepConSpec);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
};

/////////////////////////////////////////////////
//		Fast Weights

// todo: this code needs a lot of work!

class LEABRA_API FastWtCon : public LeabraCon {
  // fast weight connection: standard wt learns fast, but decays toward slow weight value
public:
  float		swt;		// slow weight value
  float		sdwt;		// #NO_SAVE slow weight delta-weight change

  FastWtCon() { swt = sdwt = 0.0f; }
};

class LEABRA_API FastWtSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specificiations for fast weights
INHERITED(taOBase)
public:
  enum DecayMode {
    ALWAYS,			// always decay all weights toward slow weight (swt)
    SU_PROP,			// decay in proportion to the sending unit activation: decay only happens after sending activity dissipates
    SU_THR			// decay only weights with sending unit activations below sending threshold (nom .1): decay only happens after sending activity dissipates
  };

  float		lrate;		// learning rate
  bool		use_lrs;	// #DEF_false use learning rate schedule to modify cur_lrate learning rate?
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current learning rate with lrate schedule factored in
  float		decay;		// rate of decay toward the slow weight values
  bool		slw_sat;	// #DEF_true does fast weight contribute to saturation of slow weights?
  DecayMode	dk_mode;	// how to apply the decay of fast weights back to the slow weight (swt) value

  SIMPLE_COPY(FastWtSpec);
  TA_BASEFUNS(FastWtSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

// TODO: the decay in this code cannot be parallelized over dwt's because the dynamics will
// be different!!!
// need to have a dmem small_batch over sequences type mode that does a SyncWts using sum_dwts = false
// and calls Compute_Weights after each trial..

// TODO: following code is not dealing with contrast enhancement on the swt vals!!!!

class LEABRA_API FastWtConSpec : public LeabraConSpec {
  // fast weight connection: standard wt learns fast, but decays toward slow weight value
INHERITED(LeabraConSpec)
public:
  FastWtSpec	fast_wt;	// fast weight specs: fast weights are added in separately to overall weight value as an increment (

  void 		C_Init_Weights_Post(RecvCons*, Connection* cn, Unit*, Unit*) {
    FastWtCon* lcn = (FastWtCon*)cn; lcn->swt = lcn->wt;
  }

  void 		C_Init_dWt(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((FastWtCon*)cn)->sdwt=0.0f; }

  override void SetCurLrate(LeabraNetwork* net, int epoch);

  inline float C_Compute_SlowHebb(FastWtCon* cn, LeabraRecvCons* cg,
				  float lin_wt, float ru_act, float su_act)
  {
    float swt_eff = (fast_wt.slw_sat) ? lin_wt : cn->swt;
    return ru_act * (su_act * (cg->savg_cor - swt_eff) - (1.0f - su_act) * swt_eff);
  }

  // todo: somewhat inefficient to do this computation twice..

  // generec error term with sigmoid activation function, and soft bounding
  inline float C_Compute_SlowErr(FastWtCon* cn, float lin_wt, float ru_act_p, float ru_act_m, float su_act_p, float su_act_m) {
    float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
    float swt_eff = (fast_wt.slw_sat) ? lin_wt : cn->swt;
    if(err > 0.0f)	err *= (1.0f - swt_eff);
    else		err *= swt_eff;	
    return err;
  }

  inline void C_Compute_SlowdWt(FastWtCon* cn, LeabraUnit*, float heb, float err) {
    float sdwt = lmix.err * err + lmix.hebb * heb;
    cn->sdwt += cur_lrate * sdwt; // standard lrate is slow one..
  }

  // combine hebbian and error-driven
  inline void C_Compute_dWt(LeabraCon* cn, LeabraUnit*, float heb, float err) {
    float dwt = lmix.err * err + lmix.hebb * heb;
    cn->dwt += fast_wt.cur_lrate * dwt;	// the regular weight is the fast one!
  }

  inline void C_Compute_FastDecay(FastWtCon* cn, LeabraUnit*, LeabraUnit* su) {
    if((fast_wt.dk_mode == FastWtSpec::SU_THR) && (su->act_p > .1f)) return;
    if(fast_wt.dk_mode == FastWtSpec::SU_PROP)
      cn->wt += (1.0f - su->act_p) * fast_wt.decay * (-cn->swt - cn->wt); // decay toward slow weights..
    else
      cn->wt += fast_wt.decay * (-cn->swt - cn->wt); // decay toward slow weights..
  }

  // todo: do ctleabra_cal

  inline void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_SAvgCor(cg, ru);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	FastWtCon* cn = (FastWtCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  C_Compute_FastDecay(cn, ru, su);
	  float lin_wt = GetLinFmWt(cn->wt);
	  C_Compute_dWt(cn, ru, 
			C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
	  C_Compute_SlowdWt(cn, ru, 
			    C_Compute_SlowHebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			    C_Compute_SlowErr(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
	}
      }
    }
  }

  inline void C_Compute_Weights_LeabraCHL(FastWtCon* cn, LeabraRecvCons* cg,
					  LeabraUnit*, LeabraUnit*, LeabraUnitSpec*)
  {
    if(cn->sdwt != 0.0f) {
      cn->swt += cn->sdwt; // wt is not negative!
      if(cn->swt < wt_limits.min) cn->swt = wt_limits.min;
      if(cn->swt > wt_limits.max) cn->swt = wt_limits.max;
    }
    if(cn->dwt != 0.0f) {
      // always do this because of the decay term..
      cn->wt = GetWtFmLin(GetLinFmWt(cn->wt) + cn->dwt);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
    cn->sdwt = 0.0f;
  }

  inline void C_Compute_WeightsActReg_LeabraCHL(FastWtCon* cn, LeabraRecvCons* cg,
				    LeabraUnit* ru, LeabraUnit* su, LeabraUnitSpec* rus)
  {
    C_Compute_ActReg_LeabraCHL(cn, cg, ru, su, rus);
    C_Compute_Weights_LeabraCHL(cn, cg, ru, su, rus);
  }

  inline void Compute_Weights_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();
    if(rus->act_reg.on) {		// do this in update so inactive units can be reached (no opt_thresh.updt)
      CON_GROUP_LOOP(cg, C_Compute_WeightsActReg_LeabraCHL((FastWtCon*)cg->Cn(i), cg,
					       (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((FastWtCon*)cg->Cn(i), cg,
					 (LeabraUnit*)ru, (LeabraUnit*)cg->Un(i), rus));
    }
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  TA_SIMPLE_BASEFUNS(FastWtConSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API ActAvgHebbMixSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra mixture of average activation hebbian learning and regular hebbian learning (on current act value)
INHERITED(taOBase)
public:
  float		act_avg;	// what proportion of average activation to include in hebbian receiving unit activation learning term
  float		cur_act;	// #READ_ONLY #SHOW 1.0 - act_avg -- proportion of current activation for hebbian learning

  SIMPLE_COPY(ActAvgHebbMixSpec);
  TA_BASEFUNS(ActAvgHebbMixSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API ActAvgHebbConSpec : public LeabraConSpec {
  // hebbian learning that includes a proportion of average activation over time, in addition to standard current unit activation;  produces a trace-based learning effect for learning over trajectories -- only for Leabra_CHL
INHERITED(LeabraConSpec)
public:
  ActAvgHebbMixSpec	act_avg_hebb; // mixture of current and average activations to use in hebbian learning

 inline float C_Compute_Hebb(LeabraCon* cn, LeabraRecvCons* cg, float lin_wt,
			     float ru_act, float su_act, float ru_avg_act)
  {
    // wt is negative in linear form, so using opposite sign of usual here
    float eff_ru_act = act_avg_hebb.act_avg * ru_avg_act + act_avg_hebb.cur_act * ru_act;
    return eff_ru_act * (su_act * (cg->savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
  }

  // this computes weight changes based on sender at time t-1
  inline void Compute_dWt_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    Compute_SAvgCor(cg, ru);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
      for(int i=0; i<cg->cons.size; i++) {
	LeabraUnit* su = (LeabraUnit*)cg->Un(i);
	LeabraCon* cn = (LeabraCon*)cg->Cn(i);
	if(!(su->in_subgp &&
	     (((LeabraUnit_Group*)su->owner)->acts_p.avg < savg_cor.thresh))) {
	  float lin_wt = GetLinFmWt(cn->wt);
	  C_Compute_dWt(cn, ru, 
			C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p, ru->act_avg),
			C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
	}
      }
    }
  }

  TA_SIMPLE_BASEFUNS(ActAvgHebbConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API LeabraLimPrecConSpec : public LeabraConSpec {
  // ##CAT_Leabra Leabra limited precision connection specs: limits weight values to specified level of precision between 0-1
INHERITED(LeabraConSpec)
public:
  float		prec_levels;	// number of levels of precision available in the weight values

  inline float	PrecLimitVal(float val) {
    int tmp = (int)((prec_levels * val) + .5f); // integerize with rounding -- val 0-1
    float rval = (float)tmp / prec_levels;
    if(rval > 1.0f) rval = 1.0f;
    if(rval < 0.0f) rval = 0.0f;
    return rval;
  }

  inline void C_Compute_LimPrecWts(LeabraCon* cn) {
    cn->wt = PrecLimitVal(cn->wt);
  }
  inline void Compute_LimPrecWts(LeabraRecvCons* cg, LeabraUnit* ru) {
    CON_GROUP_LOOP(cg, C_Compute_LimPrecWts((LeabraCon*)cg->Cn(i)));
  }

  inline override void	Compute_Weights_LeabraCHL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_Weights_LeabraCHL(cg, ru);
    Compute_LimPrecWts(cg, ru);
  }

  inline override void	Compute_Weights_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    inherited::Compute_Weights_CtLeabraCAL(cg, ru);
    Compute_LimPrecWts(cg, ru);
  }

  // NOTE: bias weights typically not subject to limited precision!

  SIMPLE_COPY(LeabraLimPrecConSpec);
  TA_BASEFUNS(LeabraLimPrecConSpec);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};


class LEABRA_API LeabraCtExptConSpec : public LeabraConSpec {
  // experimental variations of Ct learning rules
INHERITED(LeabraConSpec)
public:
  enum LearnVar {
    STD_CT,			// standard ct
    CHL,
    CHL_SUBPROD,		// CHL with subprod: (x+ - x-) * (y+ - y-)
    MINUS_INDEP_AVG, // use the independently computed average activations instead of sravg coproduct: x+ y+ - <x-> <y-> instead of - <x- y->
    MINUS_INDEP_AVG_SUBPROD, // compute product of sub terms: (x+ - <x->)(y+ - <y->)
    CT_DELTA_BCM,	     // delta-rule version of ct with BCM floating longer-term avg sub
    CT_DELTA_SAVG_BCM,	     // delta-rule version of ct with BCM floating longer-term avg sub - use avg of plus and minus for sending acts
  };

  LearnVar	learn_var;	// learning variant to implement
  float		bcm_pct;	// amount of bcm floating avg learnign to inject

  inline float C_Compute_Err_CtLeabraCAL(LeabraCon* cn, 
					 float ru_act_p, float su_act_p, float avg_nrm, 
					 LeabraCon* rbwt, LeabraCon* sbwt,
					 float ru_act_m, float su_act_m,
					 float ru_act_avg) {
    float err;
    if(learn_var == STD_CT) {
      err = (ru_act_p * su_act_p) - (avg_nrm * cn->sravg);
    }
    else if(learn_var == CHL) {
      err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
    }
    else if(learn_var == CHL_SUBPROD) {
      err = (ru_act_p - ru_act_m) * (su_act_p - su_act_m);
    }
    else if(learn_var == MINUS_INDEP_AVG) {
      // using bias weight's sravg here!
      err = (ru_act_p * su_act_p) - ((avg_nrm * rbwt->sravg) * (avg_nrm * sbwt->sravg));
    }
    else if(learn_var == MINUS_INDEP_AVG_SUBPROD) {
      // using bias weight's sravg here!
      err = (ru_act_p - (avg_nrm * rbwt->sravg)) * (su_act_p - (avg_nrm * sbwt->sravg));
    }
    else if(learn_var == CT_DELTA_BCM) {
      err = (ru_act_p - (((1.0f - bcm_pct) * (avg_nrm * rbwt->sravg)) + (bcm_pct * ru_act_avg)))
	* su_act_p;
    }
    else if(learn_var == CT_DELTA_SAVG_BCM) {
      err = (ru_act_p - (((1.0f - bcm_pct) * (avg_nrm * rbwt->sravg)) + (bcm_pct * ru_act_avg)))
	* .5 * (su_act_p + su_act_m);
    }
    // note: sb now done in compute weights
    return err;
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    // need to do recv layer here because savg_cor.thresh is only here.. could optimize this later
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    if(rlay->acts_p.avg < savg_cor.thresh) { // if layer not active in attractor phase, no learn
      Init_SRAvg(cg, ru);	return;	  // critical: need to reset this!
    }
    LeabraLayer* lfm = (LeabraLayer*)cg->prjn->from.ptr();
    if(lfm->acts_p.avg < savg_cor.thresh) {
      Init_SRAvg(cg, ru);	return;	  // critical: need to reset this!
    }
    if(ru->in_subgp) {
      LeabraUnit_Group* ogp = (LeabraUnit_Group*)ru->owner;
      if(ogp->acts_p.avg < savg_cor.thresh) {
	Init_SRAvg(cg, ru);	return;	  // critical: need to reset this!
      }
    }

    // only no hebb condition supported!!

    LeabraCon* rbwt = (LeabraCon*)ru->bias.Cn(0);

    for(int i=0; i<cg->cons.size; i++) {
      LeabraUnit* su = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->Cn(i);
      if(su->in_subgp) {
	LeabraUnit_Group* ogp = (LeabraUnit_Group*)su->owner;
	if(ogp->acts_p.avg < savg_cor.thresh) {
	  cn->sravg = 0.0f;	continue; // critical: must reset!
	}
      }
      LeabraCon* sbwt = (LeabraCon*)su->bias.Cn(0);
      C_Compute_dWt_NoHebb(cn, ru, 
			   C_Compute_Err_CtLeabraCAL(cn, ru->act_p, su->act_p,
						     rlay->sravg_nrm, rbwt, sbwt,
						     ru->act_m, su->act_m, ru->act_avg));
      cn->sravg = 0.0f;
    }
  }

  SIMPLE_COPY(LeabraCtExptConSpec);
  TA_BASEFUNS(LeabraCtExptConSpec);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

////////////////////////////////////////////////////////////////////////
// 	XCal -- Extremization Contrastive Attractor Learning
//

class LEABRA_API XCalLeabraUnit : public LeabraUnit {
  // extremization contrastive attractor learning: biologically derived learning rule: stronger get stronger, weaker get weaker
INHERITED(LeabraUnit)
public:
  float 	trl_avg;	// trial-wise average activation value
  float 	trl_sum;	// trial-wise sum activation, for computing average
  float		avg_trl_avg;	// long time-average of trial-wise average activation

  void	Copy_(const XCalLeabraUnit& cp);
  TA_BASEFUNS(XCalLeabraUnit);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class LEABRA_API XCalActSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra XCal activation specs
INHERITED(taOBase)
public:
  float		avg_init;	// initial value for avg_trl_avg 
  float		avg_dt;		// time constant for integrating avg_trl_avg
  int		n_avg_only_epcs; // #DEF_2 number of epochs during which time only an average activation value is accumulated -- no learning

  SIMPLE_COPY(XCalActSpec);
  TA_BASEFUNS(XCalActSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API XCalLeabraUnitSpec : public LeabraUnitSpec {
  // extremization contrastive attractor learning: biologically derived learning rule: stronger get stronger, weaker get weaker
INHERITED(LeabraUnitSpec)
public:
  XCalActSpec	xcal;		// xcal parameters

  override void 	Init_Weights(Unit* u);
  override void 	Init_ActAvg(LeabraUnit* u);
  override void 	Compute_SRAvg(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  override void		Compute_ActTimeAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
					   LeabraNetwork* net);
  override void		Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  override void		Compute_Weights(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net);
  
  TA_BASEFUNS(XCalLeabraUnitSpec);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API XCalLearnSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra XCal learning specs
INHERITED(taOBase)
public:
  float		p_thr_gain;	// multiplier on recv avg_trl_avg to produce p_thr value: higher values = more sparse strong weights; lower values = more distributed 
  float		d_rev;		// #DEF_0.15 proportional point within LTD range where magnitude reverses to go back down to zero at zero sravg

  float		d_rev_ratio;	// #HIDDEN #READ_ONLY (1-d_rev)/d_rev -- muliplication factor in learning rule

  SIMPLE_COPY(XCalLearnSpec);
  TA_BASEFUNS(XCalLearnSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API XCalLeabraConSpec : public LeabraConSpec {
  // extremization contrastive attractor learning: biologically derived learning rule: stronger get stronger, weaker get weaker
INHERITED(LeabraConSpec)
public:
  XCalLearnSpec	xcal;		// XCal learning parameters

  inline float C_Compute_Err_CtLeabraCAL(LeabraCon* cn, 
					 float ru_trl_avg, float su_trl_avg,
					 float thr_p, float thr_p_d_rev) {
    float srval = ru_trl_avg * su_trl_avg;
    float err;
    if(srval >= thr_p_d_rev)
      err = srval - thr_p;
    else
      err = -srval * xcal.d_rev_ratio;
    return err;
  }

  inline void Compute_dWt_CtLeabraCAL(LeabraRecvCons* cg, LeabraUnit* ru) {
    // need to do recv layer here because savg_cor.thresh is only here.. could optimize this later
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    if(rlay->acts_p.avg < savg_cor.thresh) { // if layer not active in attractor phase, no learn
      Init_SRAvg(cg, ru);	return;	  // critical: need to reset this!
    }
    LeabraLayer* lfm = (LeabraLayer*)cg->prjn->from.ptr();
    if(lfm->acts_p.avg < savg_cor.thresh) {
      Init_SRAvg(cg, ru);	return;	  // critical: need to reset this!
    }
    if(ru->in_subgp) {
      LeabraUnit_Group* ogp = (LeabraUnit_Group*)ru->owner;
      if(ogp->acts_p.avg < savg_cor.thresh) {
	Init_SRAvg(cg, ru);	return;	  // critical: need to reset this!
      }
    }

    // only no hebb condition supported!!

    XCalLeabraUnit* xru = (XCalLeabraUnit*)ru;
    float thr_p = xcal.p_thr_gain * xru->avg_trl_avg;
    float thr_p_d_rev = thr_p * xcal.d_rev;

    for(int i=0; i<cg->cons.size; i++) {
      XCalLeabraUnit* su = (XCalLeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->Cn(i);
      if(su->in_subgp) {
	LeabraUnit_Group* ogp = (LeabraUnit_Group*)su->owner;
	if(ogp->acts_p.avg < savg_cor.thresh) {
	  cn->sravg = 0.0f;	continue; // critical: must reset!
	}
      }
      C_Compute_dWt_NoHebb(cn, ru, 
			   C_Compute_Err_CtLeabraCAL(cn, xru->trl_avg, su->trl_avg,
						     thr_p, thr_p_d_rev));
      cn->sravg = 0.0f;
    }
  }

  SIMPLE_COPY(XCalLeabraConSpec);
  TA_BASEFUNS(XCalLeabraConSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////
// 	Scalar Value Layer	//
//////////////////////////////////

// NOTE: got rid of sum_bar -- never worked and requires lots of different params
// to support in the future it should probably be a subclass of the main guy
// misc todo: items for SUM_BAR:
// to do equivalent of "clamp_value" (e.g., LV units at end of settle), add a special
// mode where asymptotic Vm is computed based on current params, and act from that, etc.

// also possible: unit subgroups that all have the same gc.i value, but different random
// connectivity from inputs: sending units all send to a fixed # (permute) of these 
// group units, producing a random sensory representation.  not really necc. for S2 
// spikes, because of syndep..

class LEABRA_API ScalarValSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
INHERITED(taOBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST,			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// #APPLY_IMMED type of representation of scalar value to use
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in unit_range min-max units, unless norm_width is true, meaning use normalized 0-1 proportion of unit range)
  bool		norm_width;	// un_width is specified in normalized 0-1 proportion of unit range
  bool		clamp_pat;	// #DEF_false if true, environment provides full set of values to clamp over entire layer (instead of providing single scalar value to clamp on 1st unit, which then generates a corresponding distributed pattern)
  float		min_sum_act;	// #DEF_0.2 minimum total activity of all the units representing a value: when computing weighted average value, this is used as a minimum for the sum that you divide by
  bool		val_mult_lrn;	// #DEF_false for learning, effectively multiply the learning rate by the minus-plus phase difference in overall represented value (i.e., if overall value is the same, no learning takes place)
  bool		clip_val;	// ensure that value remains within specified range
  bool		send_thr;	// use unitspec.opt_thresh.send threshold to cut off small activation contributions to overall average value (i.e., if unit's activation is below this threshold, it doesn't contribute to weighted average computation)
  bool		init_nms;	// initialize unit names when weights are initialized

  float		min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		range;		// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  float		un_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective unit range

  virtual void	InitRange(float umin, float urng);
  // initialize range values (also sets un_width_eff)
  virtual void	InitVal(float sval, int ugp_size, float umin, float urng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual float	GetUnitVal(int unit_idx);
  // get target value associated with unit at given index: MUST CALL InitVal first!

  TA_SIMPLE_BASEFUNS(ScalarValSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API ScalarValBias : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(taOBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    GC,				// bias value enters as a conductance in gc.h or gc.a
    BWT				// bias value enters as a bias.wt
  };

  enum BiasShape {		// shape of the bias pattern
    VAL,			// bias as a particular value representation
    NEG_SLP,			// bias as an increasingly negative slope (gc.a or -bwt) as unit values increase
    POS_SLP			// bias as an increasingly positive slope (gc.h or +bwt) as unit values increase
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT				// input weights
  };

  UnitBias	un;		// #APPLY_IMMED bias on individual units
  BiasShape	un_shp;		// shape of unit bias
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// #APPLY_IMMED bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		val;		// value location (center of gaussian bump)

  SIMPLE_COPY(ScalarValBias);
  TA_BASEFUNS(ScalarValBias);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API ScalarValLayerSpec : public LeabraLayerSpec {
  // represents a scalar value using a coarse-coded distributed code over units.  first unit represents scalar value.
INHERITED(LeabraLayerSpec)
public:
  ScalarValSpec	 scalar;	// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 unit_range;	// range of values represented across the units; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  ScalarValBias	 bias_val;	// specifies bias for given value (as gaussian bump) 
  MinMaxRange	 val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	ClampValue(Unit_Group* ugp, LeabraNetwork* net, float rescale=1.0f);
  // #CAT_ScalarVal clamp value in the first unit's ext field to the units in the group
  virtual float	ClampAvgAct(int ugp_size);
  // #CAT_ScalarVal computes the average activation for a clamped unit pattern (for computing rescaling)
  virtual float	ReadValue(Unit_Group* ugp, LeabraNetwork* net);
  // #CAT_ScalarVal read out current value represented by activations in layer
  virtual void	ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal reset activation of first unit(s) after hard clamping
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal hard clamp current ext values (on all units, after ClampValue called) to all the units

  virtual void	LabelUnits_impl(Unit_Group* ugp);
  // #CAT_ScalarVal label units with their underlying values
  virtual void	LabelUnits(LeabraLayer* lay);
  // #BUTTON #CAT_ScalarVal label units in given layer with their underlying values
  virtual void	LabelUnitsNet(Network* net);
  // #BUTTON #CAT_ScalarVal label all layers in given network using this spec

  virtual void	Compute_WtBias_Val(Unit_Group* ugp, float val);
  virtual void	Compute_UnBias_Val(Unit_Group* ugp, float val);
  virtual void	Compute_UnBias_NegSlp(Unit_Group* ugp);
  virtual void	Compute_UnBias_PosSlp(Unit_Group* ugp);
  virtual void	Compute_BiasVal(LeabraLayer* lay);
  // #CAT_ScalarVal initialize the bias value 

  void 	Init_Weights(LeabraLayer* lay);
  void	Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);

  // don't include first unit in any of these computations!
  void 	Compute_ActAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);

  virtual void 	Compute_dWt_Ugp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
  // compute weight changes just for one unit group
  void	Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net);

  virtual float Compute_SSE_Ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals);
  override float Compute_SSE(LeabraLayer* lay, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON #CAT_ScalarVal reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(ScalarValLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API ScalarValSelfPrjnSpec : public ProjectionSpec {
  // special projection for making self-connection that establishes neighbor similarity in scalar val
INHERITED(ProjectionSpec)
public:
  int	width;			// width of neighborhood, in units (i.e., connect width units to the left, and width units to the right)
  float	wt_width;		// width of the sigmoid for providing initial weight values
  float	wt_max;			// maximum weight value (of 1st neighbor -- not of self unit!)

  virtual void	Connect_UnitGroup(Unit_Group* gp, Projection* prjn);
  void		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses weight values as specified in the tesselel's

  TA_SIMPLE_BASEFUNS(ScalarValSelfPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API MotorForceSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
INHERITED(taOBase)
public:
  float		pos_width;	// sigma parameter of a gaussian specifying the tuning width of the coarse-coded integration over position (in pos_range or normalized units, depending on norm_width)
  float		vel_width;	// sigma parameter of a gaussian specifying the tuning width of the coarse-coded integration over velocity (in vel_range or normalized units, depending on norm_width)
  bool		norm_width;	// #DEF_true if true, use normalized 0-1 width parameters instead of raw parameters
  bool		clip_vals;	// #DEF_true clip the pos and velocity values within their ranges, for purposes of computing values

  float		cur_pos;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos val
  float		cur_vel;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel val
  float		pos_min;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos min
  float		vel_min;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel min
  float		pos_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos range
  float		vel_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel range
  float		pos_incr;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos increment
  float		vel_incr;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel increment
  float		pos_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective position width
  float		vel_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective velocity width

  virtual void	InitRanges(float pos_min, float pos_range, float vel_min, float vel_range);
  // #CAT_MotorForce initialize range parameters
  virtual void	InitVals(float pos, int pos_size, float pos_min, float pos_range, 
			float vel, int vel_size, float vel_min, float vel_range);
  // #CAT_MotorForce intiailize value and range parameters for subsequent calls
  virtual float	GetWt(int pos_gp_idx, int vel_gp_idx);
  // #CAT_MotorForce get weighting factor for position & velocity group at given indexes: MUST CALL InitVal first!

  TA_SIMPLE_BASEFUNS(MotorForceSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API MotorForceLayerSpec : public ScalarValLayerSpec {
  // represents motor force as a function of joint position and velocity using scalar val layer spec: layer uses unit groups -- each group represents a force (typically localist), and groups are organized in X axis by position, Y axis by velocity.  Overall value is weighted average from neighboring unit groups
INHERITED(ScalarValLayerSpec)
public:
  MotorForceSpec motor_force;   // misc specs for motor force representation
  MinMaxRange	 pos_range;	// range of position values encoded over the X axis of unit groups in the layer
  MinMaxRange	 vel_range;	// range of velocity values encoded over the Y axis of unit groups in the layer
  bool		 add_noise;	// add some noise after computing value from layer
  Random	 force_noise;	// #CONDEDIT_ON_add_noise parameters for random added noise to forces

  virtual float	ReadForce(LeabraLayer* lay, LeabraNetwork* net, float pos, float vel);
  // #CAT_MotorForce read the force value from the layer, as a gaussian weighted average over units near the current position and velocity values
  virtual void	ClampForce(LeabraLayer* lay, LeabraNetwork* net, float force, float pos, float vel);
  // #CAT_MotorForce clamp the force value to the layer, as a gaussian weighted average over units near the current position and velocity values

  override void	Compute_BiasVal(LeabraLayer* lay);
  
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(MotorForceLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};


//////////////////////////////////
// 	TwoD Value Layer	//
//////////////////////////////////

class LEABRA_API TwoDValSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for two-dimensional values
INHERITED(taOBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// #APPLY_IMMED type of representation of scalar value to use
  int		n_vals;		// number of values to represent in layer: layer geom.x must be >= 2 * n_vals because vals are represented in first row of layer!
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in unit_range min-max units, unless norm_width is true, meaning use normalized 0-1 proportion of unit range)
  bool		norm_width;	// un_width is specified in normalized 0-1 proportion of unit range
  bool		clamp_pat;	// #DEF_false if true, environment provides full set of values to clamp over entire layer (instead of providing single scalar value to clamp on 1st unit, which then generates a corresponding distributed pattern)
  float		min_sum_act;	// #DEF_0.2 minimum total activity of all the units representing a value: when computing weighted average value, this is used as a minimum for the sum that you divide by
  float		mn_dst;		// #DEF_0.5 minimum distance factor for reading out multiple bumps: must be at least this times un_width far away from other bumps
  bool		clip_val;	// #DEF_true ensure that value remains within specified range

  float		x_min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		x_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		y_min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		y_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		x_val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		y_val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		x_incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  float		y_incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  int		x_size;		// #READ_ONLY #NO_SAVE #NO_INHERIT size of axis
  int		y_size;		// #READ_ONLY #NO_SAVE #NO_INHERIT size of axis
  float		un_width_x;	// #READ_ONLY #NO_SAVE #NO_INHERIT unit width, x axis (use for all computations -- can be normalized)
  float		un_width_y;	// #READ_ONLY #NO_SAVE #NO_INHERIT unit width, y axis (use for all computations -- can be normalized)

  virtual void	InitRange(float xmin, float xrng, float ymin, float yrng);
  // initialize range values, including normalized unit width values per axis
  virtual void	InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual void	GetUnitVal(int unit_idx, float& x_cur, float& y_cur);
  // get target values associated with unit at given index: MUST CALL InitVal first!

  SIMPLE_COPY(TwoDValSpec);
  TA_BASEFUNS(TwoDValSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API TwoDValBias : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(taOBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    GC,				// bias value enters as a conductance in gc.h or gc.a
    BWT				// bias value enters as a bias.wt
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT				// input weights
  };

  UnitBias	un;		// #APPLY_IMMED bias on individual units
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// #APPLY_IMMED bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		x_val;		// X axis value location (center of gaussian bump)
  float		y_val;		// Y axis value location (center of gaussian bump)

  SIMPLE_COPY(TwoDValBias);
  TA_BASEFUNS(TwoDValBias);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API TwoDValLayerSpec : public LeabraLayerSpec {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  first row represents scalar value(s).  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid
INHERITED(LeabraLayerSpec)
public:
  TwoDValSpec	 twod;		// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 x_range;	// range of values represented across the X (horizontal) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  MinMaxRange	 y_range;	// range of values represented across the Y (vertical) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  TwoDValBias	 bias_val;	// specifies bias values
  MinMaxRange	 x_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)
  MinMaxRange	 y_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	ClampValue(Unit_Group* ugp, LeabraNetwork* net, float rescale=1.0f);
  // clamp value in the first unit's ext field to the units in the group
  virtual void	ReadValue(Unit_Group* ugp, LeabraNetwork* net);
  // read out current value represented by activations in layer
  virtual void	ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net);
  // reset activation of first unit(s) after hard clamping
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // hard clamp current ext values (on all units, after ClampValue called) to all the units

  virtual void	LabelUnits_impl(Unit_Group* ugp);
  // label units with their underlying values
  virtual void	LabelUnits(LeabraLayer* lay);
  // #BUTTON label units in given layer with their underlying values
  virtual void	LabelUnitsNet(Network* net);
  // #BUTTON label all layers in given network using this spec

  virtual void	Compute_WtBias_Val(Unit_Group* ugp, float x_val, float y_val);
  virtual void	Compute_UnBias_Val(Unit_Group* ugp, float x_val, float y_val);
  virtual void	Compute_BiasVal(LeabraLayer* lay);
  // initialize the bias value 

  void 	Init_Weights(LeabraLayer* lay);
  void	Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net);
  void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);

  // don't include first unit in any of these computations!
  void 	Compute_ActAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);
  void 	Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*);

  virtual void 	Compute_dWtUgp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net);
  // compute weight changes just for one unit group
  void	Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net);

  virtual float Compute_SSE_Ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals);
  override float Compute_SSE(LeabraLayer* lay, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TwoDValLayerSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

class LEABRA_API DecodeTwoDValLayerSpec : public TwoDValLayerSpec {
  // a two-d-value layer spec that copies its activations from one-to-one input prjns, to act as a decoder of another layer
INHERITED(TwoDValLayerSpec)
public:
  void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net);
  void 	Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net);
  void	Compute_SRAvg(LeabraLayer* lay, LeabraNetwork* net) { };
  void	Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) { };

  TA_BASEFUNS_NOCOPY(DecodeTwoDValLayerSpec);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};


//////////////////////////////////////
// 	V1RF Prjn Spec
//////////////////////////////////////

class LEABRA_API V1RFPrjnSpec : public ProjectionSpec {
  // V1 receptive field projection spec: does overlapping tiled receptive fields with Gabor and Blob filter weights - rf_spec.rf_width specifies the width of the receptive field, and rf_move specifies how much to move in input coordinates per each recv group
INHERITED(ProjectionSpec)
public:
  GaborV1Spec	rf_spec;	// #SHOW_TREE receptive field specs
  FloatTwoDCoord rf_move;	// how much to move in input coordinates per each receiving layer group
  float		dog_surr_mult;	// multiplier on surround weight values for DoG -- can be used to turn off surround entirely or to amplify it
  
  void 		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual void	GraphFilter(DataTable* disp_data, int recv_unit_no);
  // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a graph of a given unit number's gabor / blob filter
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the filter gaussian into data table and generate a grid view of all the gabor or blob filters

  TA_SIMPLE_BASEFUNS(V1RFPrjnSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API SaliencyPrjnSpec : public ProjectionSpec {
  // Saliency projection spec from V1 layer: receiving layer must have a unit group for each feature, with each unit group having the V1 unit group geometry -- gets excitatory connection from feature corresponding to group index, and from all-but that feature in surrounding sending areas, producing a contrast enhancement effect.  Competition within group and across whole layer produces pop-out dynamics
INHERITED(ProjectionSpec)
public:
  int		convergence;	// how many sending groups to count as a single entity from the perspective of the saliency computation -- values larger than 1 make the saliency layer smaller than the sending layer
  bool		reciprocal;	// opposite direction connection: feedback to the features 
  bool		feat_only;	// only get excitatory input from feature only -- no DoG or anything else -- just pure feature aggregation (for use with kwta-based popout dynamics)
  int		feat_gps;	// number of feature groups contained within V1 unit group -- surround connections are only for within-group connections
  DoGFilterSpec	dog_wts;	// #CONDEDIT_OFF_feat_only Difference of Gaussians filter for setting the weights -- determines the width of the projection, etc -- where the net filter values are positive, receives excitation from the target feature, else from all-but the target feature
  float		wt_mult;	// #CONDEDIT_OFF_feat_only multiplier on weight values coming from the dog_wts 
  float		surr_mult;	// #CONDEDIT_OFF_feat_only multiplier on surround weight values -- DoG has very weak surround in general so it can be useful to increase that

  int		units_per_feat_gp; // #READ_ONLY #NO_SAVE #SHOW number of units per feature group (computed from sending layer)
  
  void 		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);

  virtual void 	Connect_feat_only(Projection* prjn);
  virtual void 	Connect_full_dog(Projection* prjn);

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the DoG center-surround weights into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK plot the DoG center-surround weights into data table and generate a grid view

  TA_SIMPLE_BASEFUNS(SaliencyPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API GpAggregatePrjnSpec : public ProjectionSpec {
  // aggregates across a layer with unit groups into a receiving layer that has the same geometry as each of the unit groups -- each unit receives from the corresponding unit in each of the sending unit groups
INHERITED(ProjectionSpec)
public:
  
  void 		Connect_impl(Projection* prjn);

  TA_SIMPLE_BASEFUNS(GpAggregatePrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API LeabraV1Layer : public LeabraLayer {
  // Specialized layer that implements competition both within unit groups and among features across the entire layer, where a feature is defined as a specific unit position within the unit groups (layer must have unit groups)
INHERITED(LeabraLayer)
public:
  LeabraUnit_Group		feat_gps;    // #LINK_GROUP #NO_SAVE #READ_ONLY #HIDDEN feature-level unit groups: the .gp subgroups are populated with sub unit groups, one per feature group, each containing links to the units
  LeabraInhib			feat_lay_thr; // #NO_SAVE #READ_ONLY #HIDDEN feature layer-level inhbition threshold stuff (use instead of layer's own thr values)

  override void	BuildUnits();
  override void	ResetSortBuf();

  TA_SIMPLE_BASEFUNS(LeabraV1Layer);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class LEABRA_API V1FeatInhibSpec : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies inhibition parameters for V1 layer units based on feature-level inhibition
INHERITED(taOBase)
public:
  float		feat_gain;	// multiplier for feature-level inhibition
  float		dist_sigma;	// Gaussian std deviation (sigma) for weighting competitors based on distance, in normalized units relative to max x,y size dimension of the layer group geometry
  float		i_rat_thr;	// optimization threshold of unit i_thr relative to group's inhib threshold -- if less than this threshold, no distance costs are assessed

  void 	Defaults()	{ Initialize(); }
  TA_SIMPLE_BASEFUNS(V1FeatInhibSpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

class LEABRA_API LeabraV1LayerSpec : public LeabraLayerSpec {
  // LayerSpec that implements competition both within unit groups and among features across the entire layer, where a feature is defined as a specific unit position within the unit groups (layer must have unit groups, and must be of LeabraV1Layer type!) -- feature inhibition is based on distance -- adds to layer-level gp_g based inhib for each unit in proportion to distance from active units
INHERITED(LeabraLayerSpec)
public:
  V1FeatInhibSpec	feat_inhib; // feature-level inhibition parameters

  virtual void	Compute_FeatGpActive(LeabraLayer* lay, LeabraUnit_Group* fug, LeabraNetwork* net);
  // compute active units in active_buf for given feature unit group

  override void	Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  override bool CheckConfig_Layer(LeabraLayer* lay, bool quiet=false);
  
  TA_SIMPLE_BASEFUNS(LeabraV1LayerSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // leabra_extra_h

