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

#ifndef TrialSynDepConSpec_h
#define TrialSynDepConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <TrialSynDepCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TrialSynDepSpec);

class E_API TrialSynDepSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for synaptic depression
INHERITED(SpecMemberBase)
public:
  float		rec;		// #DEF_1 rate of recovery from depression
  float		depl;		// #DEF_1.1 rate of depletion of synaptic efficacy as a function of sender-receiver activations

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(TrialSynDepSpec);
  TA_BASEFUNS(TrialSynDepSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(TrialSynDepConSpec);

class E_API TrialSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the trial level (as opposed to cycle level)
INHERITED(LeabraConSpec)
public:
  enum TrialSynDepConVars {
    EFFWT = SWT+1,                // effective weight value
  };

  TrialSynDepSpec	syn_dep;	// synaptic depression specifications
  
  inline void C_Depress_Wt(float& effwt, const float wt, const float su_act) {
    // NOTE: fctn of sender act and wt: could be just sender or sender*recv.. 
    float deff = syn_dep.rec * (wt - effwt) - syn_dep.depl * su_act * wt;
    effwt += deff;
    if(effwt > wt)              effwt = wt;
    if(effwt < wt_limits.min)   effwt = wt_limits.min;
  }
  // #IGNORE
  inline virtual void Depress_Wt(LeabraSendCons* cg, LeabraUnit* su,
                          LeabraNetwork* net) {
    const float su_act = su->act_eq;
    float* wts = cg->OwnCnVar(WT);
    float* effs = cg->OwnCnVar(EFFWT);

    CON_GROUP_LOOP(cg, C_Depress_Wt(effs[i], wts[i], su_act));
  }
  // #IGNORE

  inline override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                      LeabraNetwork* net) {
    inherited::Compute_dWt_LeabraCHL(cg, su, net);
    Depress_Wt(cg, su, net);
  }
  inline override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net) {
    inherited::Compute_dWt_CtLeabraXCAL(cg, su, net);
    Depress_Wt(cg, su, net);
  }
  inline override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                        LeabraNetwork* net) {
    inherited::Compute_dWt_CtLeabraCAL(cg, su, net);
    Depress_Wt(cg, su, net);
  }

  void C_Reset_EffWt(float& effwt, const float wt) {
    effwt = wt;
  }
  // #IGNORE
  virtual void Reset_EffWt(LeabraRecvCons* cg, LeabraNetwork* net) {
    // recv based -- slow
    CON_GROUP_LOOP(cg, C_Reset_EffWt(cg->PtrCn(i,EFFWT,net), cg->PtrCn(i,WT,net)));
  }
  // #IGNORE
  virtual void Reset_EffWt(LeabraSendCons* cg) {
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

  inline override void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                const int thread_no, const float su_act_delta)
  { Send_NetinDelta_impl(cg, net, thread_no, su_act_delta, cg->OwnCnVar(EFFWT)); }
  // use effwt instead of wt

  inline override float Compute_Netin(RecvCons* cg, Unit* ru, Network* net) {
    // this is slow b/c going through the PtrCn
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,EFFWT,net), // effwt
                                               cg->Un(i,net)->act));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }

  TA_SIMPLE_BASEFUNS(TrialSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // TrialSynDepConSpec_h
