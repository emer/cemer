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

#ifndef LeabraXCALSpikeConSpec_h
#define LeabraXCALSpikeConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraSpikeCon>
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(XCALSpikeSpec);

class E_API XCALSpikeSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra XCAL purely spiking learning rule based on Urakubo et al 2008
INHERITED(SpecMemberBase)
public:
  bool		ss_sr;	   // #DEF_false do super-short sender-recv multiplication instead of full Urakubo thing -- just for testing/debugging

  float		ca_norm;   // #DEF_5 normalization factor for ca -- divide all ca constants by this amount
  float		k_ca;	   // #READ_ONLY #SHOW (.3 in original units) effective Ca that gives 50% inhibition of maximal NMDA receptor activity
  float		ca_vgcc;   // #READ_ONLY #SHOW (1.3 in original units) Ca influx resulting from receiver spiking (due to voltage gated calcium channels)
  float		ca_v_nmda; // #READ_ONLY #SHOW (0.0223 in original units) Ca influx due to membrane-potential (voltage) driven NMDA receptor activation
  float		ca_nmda;   // #READ_ONLY #SHOW (0.5 in original units) Ca influx from NMDA that is NOT driven by membrane potential 
  float		ca_dt;     // #DEF_20 time constant (in msec) for decay of Ca 
  float		ca_rate;   // #READ_ONLY #NO_SAVE rate constant (1/dt) for decay of Ca 
  float		ca_off;	   // #DEF_0.55 offset for ca -- subtract this amount from ca (clipped to zero) for learning computations
  float		nmda_dt;   // #DEF_40 time constant (in msec) for decay of NMDA receptor conductance
  float		nmda_rate; // #READ_ONLY #NO_SAVE rate constant (1/dt) for decay of NMDA receptor conductance

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "ConSpec"; }
  
  TA_SIMPLE_BASEFUNS(XCALSpikeSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(LeabraXCALSpikeConSpec);

class E_API LeabraXCALSpikeConSpec : public LeabraConSpec {
  // XCAL purely spiking learning rule based on Urakubo et al 2008 -- computes a postsynaptic calcium value that drives learning using the XCAL_C fully continous-time learning parameters
INHERITED(LeabraConSpec)
public:
  enum SpikeConVars {
    SRAVG_SS = SRAVG_M+1,       // super-short term scale sr average
    NMDA,                       // NMDA open channels
    CA,                         // postsynaptic ca
  };

  XCALSpikeSpec	xcal_spike;	// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL_C XCAL (eXtended Contrastive Attractor Learning) spike-based fully continuous-time learning parameters

  inline void 	C_Init_Weights(RecvCons* cg, const int idx, Unit* ru, Unit* su,
                                       Network* net) CPP11_OVERRIDE
  { inherited::C_Init_Weights(cg, idx, ru, su, net); 
    cg->Cn(idx,SRAVG_SS,net) = 0.15f; cg->Cn(idx,SRAVG_S,net) = 0.15f; 
    cg->Cn(idx,SRAVG_M,net) = 0.15f; 
    cg->Cn(idx,NMDA,net) = 0.0f; cg->Cn(idx,CA,net) = 0.0f; 
#ifdef XCAL_DEBUG
    // not supported
    lcn->srprod_s = lcn->srprod_m = xcal.avg_init;
#endif
  }

  inline void Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net) { };
  // never init..

  inline void C_Compute_SRAvg_spike(float& sravg_ss, float& sravg_s, float& sravg_m,
                                    float& nmda, float& ca, const float ru_act, 
                                    const float ru_vm_dend,
                                    const float su_act, LeabraUnitSpec* us) {
    // this happens every cycle, and is the place to compute nmda and ca -- expensive!! :(
    float dnmda = -nmda * xcal_spike.nmda_rate;
    float dca = (nmda * (xcal_spike.ca_v_nmda * ru_vm_dend + xcal_spike.ca_nmda))
      - (ca * xcal_spike.ca_rate);
    if(su_act > 0.5f) { dnmda += xcal_spike.k_ca / (xcal_spike.k_ca + ca); }
    if(ru_act > 0.5f) { dca += xcal_spike.ca_vgcc; }
    nmda += dnmda;
    ca += dca;
    float sr = (ca - xcal_spike.ca_off);
    if(sr < 0.0f) sr = 0.0f;
    sravg_ss += us->act_avg.ss_dt * (sr - sravg_ss);
    sravg_s += us->act_avg.s_dt * (sravg_ss - sravg_s);
    sravg_m += us->act_avg.m_dt * (sravg_s - sravg_m);

#ifdef XCAL_DEBUG
    srprod_s = ru->avg_s * su->avg_s;
    srprod_m = ru->avg_m * su->avg_m;
#endif
  }
  // #IGNORE

  inline void C_Compute_SRAvg_sssr(float& sravg_ss, float& sravg_s, float& sravg_m,
                                   const float ru_avg_ss, const float su_avg_ss,
				   LeabraUnitSpec* us) {
    sravg_ss = ru_avg_ss * su_avg_ss; // use ss to capture local time window
    sravg_s += us->act_avg.s_dt * (sravg_ss - sravg_s);
    sravg_m += us->act_avg.m_dt * (sravg_s - sravg_m);
  }
  // #IGNORE

  inline void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su,
                            LeabraNetwork* net, const bool do_s) CPP11_OVERRIDE {
    LeabraUnitSpec* us = (LeabraUnitSpec*)su->GetUnitSpec();
    float* srss = cg->OwnCnVar(SRAVG_SS);
    float* srs = cg->OwnCnVar(SRAVG_S);
    float* srm = cg->OwnCnVar(SRAVG_M);
    float* nmdas = cg->OwnCnVar(NMDA);
    float* cas = cg->OwnCnVar(CA);
    const float su_act = su->act;
    const float su_avg_ss = su->avg_ss;

    if(learn_rule == CTLEABRA_XCAL_C) {
      if(xcal_spike.ss_sr) {
	CON_GROUP_LOOP(cg, C_Compute_SRAvg_sssr(srss[i], srs[i], srm[i], 
                                                ((LeabraUnit*)cg->Un(i,net))->avg_ss,
                                                su_avg_ss, us));
      }
      else {
	CON_GROUP_LOOP(cg, C_Compute_SRAvg_spike(srss[i], srs[i], srm[i], nmdas[i], cas[i],
                                                 cg->Un(i,net)->act, 
                                                 ((LeabraUnit*)cg->Un(i,net))->vm_dend,
                                                 su_act, us));
      }
    }
    else {
      inherited::Compute_SRAvg(cg, su, net, do_s);
    }
  }

  inline void C_Compute_dWt_CtLeabraXCAL_spike(float& dwt, 
                                               const float sravg_s, const float sravg_m,
                                               const float ru_avg_l, 
                                               const float su_act_mult) {
    float sm_mix = xcal.s_mix * sravg_s + xcal.m_mix * sravg_m;
    float effthr = xcal.thr_m_mix * sravg_m + su_act_mult * ru_avg_l;
    dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
  }
  // #IGNORE

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                LeabraNetwork* net) CPP11_OVERRIDE {
    const float su_avg_m = su->avg_m;
    const float su_act_mult = xcal.thr_l_mix * su_avg_m;
    float* dwts = cg->OwnCnVar(DWT);
    float* srs = cg->OwnCnVar(SRAVG_S);
    float* srm = cg->OwnCnVar(SRAVG_M);

    if(learn_rule == CTLEABRA_XCAL_C) {
      const int sz = cg->size;
      for(int i=0; i<sz; i++) {
	LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
	C_Compute_dWt_CtLeabraXCAL_spike(dwts[i], srs[i], srm[i], ru->avg_l,
                                         su_act_mult);
      }
    }
    else {
      inherited::Compute_dWt_CtLeabraXCAL(cg, su, net);
    }
  }

  virtual void	GraphXCALSpikeSim(DataTable* graph_data = NULL,
		  LeabraUnitSpec* unit_spec = NULL,
		  float rate_min=0.0f, float rate_max=150.0f, float rate_inc=5.0f,
		  float max_time=250.0f, int reps_per_point=5,
				  float lin_norm=0.01f);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph a simulation of the XCAL spike function by running a simulated synapse with poisson firing rates sampled over given range, with given samples per point, and other parameters as given

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraXCALSpikeConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // LeabraXCALSpikeConSpec_h
