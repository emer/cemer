// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef FastWtConSpec_h
#define FastWtConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <FastWtCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(FastWtSpec);

class E_API FastWtSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specificiations for fast weights
INHERITED(SpecMemberBase)
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

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(FastWtSpec);
  TA_BASEFUNS(FastWtSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

// TODO: the decay in this code cannot be parallelized over dwt's because the dynamics will
// be different!!!
// need to have a dmem small_batch over sequences type mode that does a SyncWts using sum_dwts = false
// and calls Compute_Weights after each trial..

// TODO: following code is not dealing with contrast enhancement on the swt vals!!!!

eTypeDef_Of(FastWtConSpec);

class E_API FastWtConSpec : public LeabraConSpec {
  // fast weight connection: standard wt learns fast, but decays toward slow weight value
INHERITED(LeabraConSpec)
public:
  FastWtSpec	fast_wt;	// fast weight specs: fast weights are added in separately to overall weight value as an increment (

  void 		C_Init_Weights_post(BaseCons* cg, Connection* cn, Unit* ru, Unit* su) {
    inherited::C_Init_Weights_post(cg, cn, ru, su);
    FastWtCon* lcn = (FastWtCon*)cn; lcn->swt = lcn->wt;
  }

  void 		C_Init_dWt(RecvCons* cg, Connection* cn, Unit* ru, Unit* su)
  { ConSpec::C_Init_dWt(cg, cn, ru, su); ((FastWtCon*)cn)->sdwt=0.0f; }

  override void SetCurLrate(LeabraNetwork* net, int epoch);

  inline float C_Compute_SlowHebb(FastWtCon* cn, LeabraSendCons* cg,
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

  // todo: do CtLeabra_XCal and cal

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_SAvgCor(cg, su);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      FastWtCon* cn = (FastWtCon*)cg->OwnCn(i);
      C_Compute_FastDecay(cn, ru, su);
      float lin_wt = LinFmSigWt(cn->lwt);
      C_Compute_dWt(cn, ru, 
		    C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
		    C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
      C_Compute_SlowdWt(cn, ru, 
			C_Compute_SlowHebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			C_Compute_SlowErr(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
    }
  }

  inline void C_Compute_Weights_LeabraCHL(FastWtCon* cn) {
    if(cn->sdwt != 0.0f) {
      cn->swt += cn->sdwt; // wt is not negative!
      if(cn->swt < wt_limits.min) cn->swt = wt_limits.min;
      if(cn->swt > wt_limits.max) cn->swt = wt_limits.max;
    }
    if(cn->dwt != 0.0f) {
      // always do this because of the decay term..
      cn->lwt = SigFmLinWt(LinFmSigWt(cn->lwt) + cn->dwt);
      Compute_EffWt(cn);
    }
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
    cn->sdwt = 0.0f;
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL((FastWtCon*)cg->OwnCn(i)));
    //  ApplyLimits(cg, ru); limits are automatically enforced anyway
  }

  TA_SIMPLE_BASEFUNS(FastWtConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // FastWtConSpec_h
