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

TypeDef_Of(XCALSpikeSpec);

class LEABRA_API XCALSpikeSpec : public SpecMemberBase {
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

  override String       GetTypeDecoKey() const { return "ConSpec"; }
  
  TA_SIMPLE_BASEFUNS(XCALSpikeSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

TypeDef_Of(LeabraXCALSpikeConSpec);

class LEABRA_API LeabraXCALSpikeConSpec : public LeabraConSpec {
  // XCAL purely spiking learning rule based on Urakubo et al 2008 -- computes a postsynaptic calcium value that drives learning using the XCAL_C fully continous-time learning parameters
INHERITED(LeabraConSpec)
public:
  XCALSpikeSpec	xcal_spike;	// #CAT_Learning #CONDSHOW_ON_learn_rule:CTLEABRA_XCAL_C XCAL (eXtended Contrastive Attractor Learning) spike-based fully continuous-time learning parameters

  inline void 	C_Init_Weights(RecvCons* cg, Connection* cn, Unit* ru, Unit* su) {
    LeabraConSpec::C_Init_Weights(cg, cn, ru, su); LeabraSpikeCon* lcn = (LeabraSpikeCon*)cn;
    lcn->sravg_ss = 0.15f; lcn->sravg_s = 0.15f; lcn->sravg_m = 0.15f;
    lcn->nmda = 0.0f; lcn->ca = 0.0f; 
#ifdef XCAL_DEBUG
    lcn->srprod_s = lcn->srprod_m = xcal.avg_init;
#endif
  }

  inline void Trial_Init_SRAvg(LeabraSendCons* cg, LeabraUnit* su) { };
  // never init..

  inline void C_Compute_SRAvg_spike(LeabraSpikeCon* cn, LeabraUnit* ru, LeabraUnit* su,
				    LeabraUnitSpec* us) {
    // this happens every cycle, and is the place to compute nmda and ca -- expensive!! :(
    float dnmda = -cn->nmda * xcal_spike.nmda_rate;
    float dca = (cn->nmda * (xcal_spike.ca_v_nmda * ru->vm_dend + xcal_spike.ca_nmda))
      - (cn->ca * xcal_spike.ca_rate);
    if(su->act > 0.5f) { dnmda += xcal_spike.k_ca / (xcal_spike.k_ca + cn->ca); }
    if(ru->act > 0.5f) { dca += xcal_spike.ca_vgcc; }
    cn->nmda += dnmda;
    cn->ca += dca;
    float sr = (cn->ca - xcal_spike.ca_off);
    if(sr < 0.0f) sr = 0.0f;
    cn->sravg_ss += us->act_avg.ss_dt * (sr - cn->sravg_ss);
    cn->sravg_s += us->act_avg.s_dt * (cn->sravg_ss - cn->sravg_s);
    cn->sravg_m += us->act_avg.m_dt * (cn->sravg_s - cn->sravg_m);

#ifdef XCAL_DEBUG
    cn->srprod_s = ru->avg_s * su->avg_s;
    cn->srprod_m = ru->avg_m * su->avg_m;
#endif
  }

  inline void C_Compute_SRAvg_sssr(LeabraSpikeCon* cn, LeabraUnit* ru, LeabraUnit* su,
				   LeabraUnitSpec* us) {
    cn->sravg_ss = ru->avg_ss * su->avg_ss; // use ss to capture local time window
    cn->sravg_s += us->act_avg.s_dt * (cn->sravg_ss - cn->sravg_s);
    cn->sravg_m += us->act_avg.m_dt * (cn->sravg_s - cn->sravg_m);
  }

  inline void Compute_SRAvg(LeabraSendCons* cg, LeabraUnit* su, bool do_s) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)su->GetUnitSpec();
    if(learn_rule == CTLEABRA_XCAL_C) {
      if(xcal_spike.ss_sr) {
	CON_GROUP_LOOP(cg, C_Compute_SRAvg_sssr((LeabraSpikeCon*)cg->OwnCn(i), 
						(LeabraUnit*)cg->Un(i), su, us));
      }
      else {
	CON_GROUP_LOOP(cg, C_Compute_SRAvg_spike((LeabraSpikeCon*)cg->OwnCn(i), 
						 (LeabraUnit*)cg->Un(i), su, us));
      }
    }
    else {
      inherited::Compute_SRAvg(cg, su, do_s);
    }
  }

  inline void C_Compute_dWt_CtLeabraXCAL_spike(LeabraSpikeCon* cn, LeabraUnit* ru,
					       LeabraUnit* su, float su_act_mult) {
    float srs = cn->sravg_s;
    float srm = cn->sravg_m;
    float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
    float effthr = xcal.thr_m_mix * srm + su_act_mult * ru->avg_l;
    cn->dwt += cur_lrate * xcal.dWtFun(sm_mix, effthr);
  }

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    float su_avg_m = su->avg_m;
    float su_act_mult = xcal.thr_l_mix * su_avg_m;

    if(learn_rule == CTLEABRA_XCAL_C) {
      for(int i=0; i<cg->size; i++) {
	LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	C_Compute_dWt_CtLeabraXCAL_spike((LeabraSpikeCon*)cg->OwnCn(i), ru, su, su_act_mult);
      }
    }
    else {
      inherited::Compute_dWt_CtLeabraXCAL(cg, su);
    }
  }

  virtual void	GraphXCALSpikeSim(DataTable* graph_data = NULL,
		  LeabraUnitSpec* unit_spec = NULL,
		  float rate_min=0.0f, float rate_max=150.0f, float rate_inc=5.0f,
		  float max_time=250.0f, int reps_per_point=5,
				  float lin_norm=0.01f);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph a simulation of the XCAL spike function by running a simulated synapse with poisson firing rates sampled over given range, with given samples per point, and other parameters as given

  override String       GetTypeDecoKey() const { return "ConSpec"; }

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
