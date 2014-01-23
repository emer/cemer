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

#ifndef CaiSynDepConSpec_h
#define CaiSynDepConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <CaiSynDepCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(CaiSynDepSpec);

class E_API CaiSynDepSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for synaptic depression based in synaptic integration of calcium
INHERITED(SpecMemberBase)
public:
  float		ca_inc;		// #DEF_0.2 time constant for increases in Ca_i (from NMDA etc currents) -- default base 
  // value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value (default = 20)
  float		ca_dec;		// #DEF_0.2 time constant for decreases in Ca_i (from Ca pumps pushing Ca back out into 
  // the synapse) -- default base value is .01 per cycle -- multiply by network->ct_learn.syndep_int to get this value 
  // (default = 20)

  float		sd_ca_thr;	// #DEF_0.2 synaptic depression ca threshold: only when ca_i has increased by this 
  // amount (thus synaptic ca depleted) does it affect firing rates and thus synaptic depression
  float		sd_ca_gain;	// #DEF_0.3 multiplier on cai value for computing synaptic depression -- modulates 
  // overall level of depression independent of rate parameters
  float		sd_ca_thr_rescale; // #READ_ONLY rescaling factor taking into account sd_ca_gain and sd_ca_thr 
  // (= sd_ca_gain/(1 - sd_ca_thr))

  inline void	CaUpdt(float& cai, const float ru_act, const float su_act) {
    float drive = ru_act * su_act;
    cai += ca_inc * (1.0f - cai) * drive - ca_dec * cai;
  }

  inline float	SynDep(const float cai) {
    float cao_thr = (cai > sd_ca_thr) ? (1.0 - sd_ca_thr_rescale * (cai - sd_ca_thr)) : 1.0f;
    return cao_thr * cao_thr;
  }

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "ConSpec"; }

  SIMPLE_COPY(CaiSynDepSpec);
  TA_BASEFUNS(CaiSynDepSpec);
protected:
  SPEC_DEFAULTS;
  void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(CaiSynDepConSpec);

class E_API CaiSynDepConSpec : public LeabraConSpec {
  // synaptic depression connection at the cycle level, based on synaptic integration of calcium
INHERITED(LeabraConSpec)
public:
  enum CaiSynDepConVars {
    EFFWT = SWT+1,                // effective weight value
    CAI,                          // cai intacelluarl calcium
  };

  CaiSynDepSpec	ca_dep;		// calcium-based depression of synaptic efficacy

  
  /////////////////////////////////////////////////////////////////////////////////////
  // 		Ca updating and synaptic depression

  inline void C_Compute_Cai(float& cai, const float ru_act, const float su_act)
  { ca_dep.CaUpdt(cai, ru_act, su_act); }
  // #IGNORE connection-level Cai update
  inline void Compute_Cai(LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net)
  { float* cais = cg->OwnCnVar(CAI);
    const float su_act = su->act_eq;
    CON_GROUP_LOOP(cg, C_Compute_Cai(cais[i],
                                     ((LeabraUnit*)cg->Un(i,net))->act_eq, su_act));
  }
  // #IGNORE connection-level synaptic depression: syn dep direct

  inline void C_Compute_CycSynDep(float& effwt, const float wt, const float cai)
  { effwt = wt * ca_dep.SynDep(cai); }
  // #IGNORE connection-level synaptic depression: ca mediated
  inline void Compute_CycSynDep(LeabraSendCons* cg, LeabraUnit* su,
                                         LeabraNetwork* net)  CPP11_OVERRIDE
  { Compute_Cai(cg, su, net);
    float* wts = cg->OwnCnVar(WT);
    float* cais = cg->OwnCnVar(CAI);
    float* effs = cg->OwnCnVar(EFFWT);
    CON_GROUP_LOOP(cg, C_Compute_CycSynDep(effs[i], wts[i], cais[i]));
  }
  // #IGNORE connection-group level synaptic depression

  inline void C_Init_SdEffWt(float& effwt, const float wt, float& cai)
  { effwt = wt; cai = 0.0f;  }
  // #IGNORE 

  inline void Init_SdEffWt(LeabraRecvCons* cg, LeabraNetwork* net) {
    // receiver based
    CON_GROUP_LOOP(cg, C_Init_SdEffWt(cg->PtrCn(i,EFFWT,net), cg->PtrCn(i,WT,net),
                                      cg->PtrCn(i,CAI,net)));
  }
  // #IGNORE #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables) -- receiver based -- slow -- use sender if possible
  inline void Init_SdEffWt(LeabraSendCons* cg) {
    float* wts = cg->OwnCnVar(WT);
    float* cais = cg->OwnCnVar(CAI);
    float* effs = cg->OwnCnVar(EFFWT);
    CON_GROUP_LOOP(cg, C_Init_SdEffWt(effs[i], wts[i], cais[i]));
  }
  // #CAT_Activation reset synaptic depression effective weight (remove any existing synaptic depression and associated variables)

  inline void C_Init_Weights_post(BaseCons* cg, const int idx,
                                           Unit* ru, Unit* su, Network* net) CPP11_OVERRIDE {
    inherited::C_Init_Weights_post(cg, idx, ru, su, net);
    cg->Cn(idx,EFFWT,net) = cg->Cn(idx,WT,net); cg->Cn(idx,CAI,net) = 0.0f;
  }
  // #IGNORE 

  inline void Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                const int thread_no, const float su_act_delta) CPP11_OVERRIDE
  { Send_NetinDelta_impl(cg, net, thread_no, su_act_delta, cg->OwnCnVar(EFFWT)); }
  // #IGNORE use effwt instead of wt

  inline float Compute_Netin(RecvCons* cg, Unit* ru, Network* net)  CPP11_OVERRIDE {
    // this is slow b/c going through the PtrCn
    float rval=0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_Netin(cg->PtrCn(i,EFFWT,net), // effwt
                                               cg->Un(i,net)->act));
    return ((LeabraRecvCons*)cg)->scale_eff * rval;
  }
  // #IGNORE 

  TA_SIMPLE_BASEFUNS(CaiSynDepConSpec);
protected:
  SPEC_DEFAULTS;
  void 	UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // CaiSynDepConSpec_h
