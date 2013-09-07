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

#ifndef CycleSynDepConSpec_h
#define CycleSynDepConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <CycleSynDepCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CycleSynDepSpec);

class E_API CycleSynDepSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(SpecMemberBase)
public:
  float		rec;		// #DEF_0.002 rate of recovery from depression
  float		asymp_act;	// #DEF_0.4 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl rate value)
  float		depl;		// #READ_ONLY #SHOW rate of depletion of synaptic efficacy as a function of sender-receiver activations (computed from rec, asymp_act)

  inline void	Depress(float& effwt, const float wt, const float ru_act,
                        const float su_act) {
    const float drive = ru_act * su_act * effwt;
    const float deff = rec * (wt - effwt) - depl * drive;
    effwt += deff;
    if(effwt > wt) effwt = wt;
    if(effwt < 0.0f) effwt = 0.0f;
  }

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(CycleSynDepSpec);
  TA_BASEFUNS(CycleSynDepSpec);
protected:
  SPEC_DEFAULTS;
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(CycleSynDepConSpec);

class E_API CycleSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level (as opposed to the trial level) -- this is the simpler version -- Ca_i based version below
INHERITED(LeabraConSpec)
public:
  enum CycleSynDepConVars {
    EFFWT = SWT+1,                // effective weight value
  };

  CycleSynDepSpec	syn_dep;	// synaptic depression specifications

  inline void C_Compute_CycSynDep(float& effwt, const float wt,
                                  const float ru_act, const float su_act)
  { syn_dep.Depress(effwt, wt, ru_act, su_act); }
  // #IGNORE 

  inline override void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net) {
    float* wts = cg->OwnCnVar(WT);
    float* effs = cg->OwnCnVar(EFFWT);
    const float su_act = su->act_eq;
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep(effs[i], wts[i],
					   ((LeabraUnit*)cg->Un(i,net))->act_eq, su_act));
  }
  // #IGNORE 

  inline void C_Reset_EffWt(float& effwt, const float wt)
  { effwt = wt; }

  inline virtual void Reset_EffWt(LeabraRecvCons* cg, LeabraNetwork* net) {
    // receiver based -- avoid
    CON_GROUP_LOOP(cg, C_Reset_EffWt(cg->PtrCn(i,EFFWT,net), cg->PtrCn(i,WT,net)));
  }
  // #IGNORE 
  inline virtual void Reset_EffWt(LeabraSendCons* cg) {
    float* wts = cg->OwnCnVar(WT);
    float* effs = cg->OwnCnVar(EFFWT);
    CON_GROUP_LOOP(cg, C_Reset_EffWt(effs[i], wts[i]));
  }
  // #IGNORE 
  inline override void 	C_Init_Weights_post(BaseCons* cg, const int idx,
                                            Unit* ru, Unit* su, Network* net) {
    inherited::C_Init_Weights_post(cg, idx, ru, su, net);
    cg->Cn(idx,EFFWT,net) = cg->Cn(idx,WT,net);
  }
  // #IGNORE 
  inline override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                const int thread_no, const float su_act_delta)
  { Send_NetinDelta_impl(cg, net, thread_no, su_act_delta, cg->OwnCnVar(EFFWT)); }
  // #IGNORE use effwt instead of wt

  inline override float Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
    // this is slow b/c going through the PtrCn
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,EFFWT,net), // effwt
                                               cg->Un(i,net)->act));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }
  // #IGNORE 

  TA_SIMPLE_BASEFUNS(CycleSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // CycleSynDepConSpec_h
