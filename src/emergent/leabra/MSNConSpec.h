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

#ifndef MSNConSpec_h
#define MSNConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include <MSNCon>
#include <MSNUnitSpec>

// declare all other types mentioned but not required to include:

eTypeDef_Of(MSNTraceSpec);

class E_API MSNTraceSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for trace-based learning in the MSN's
INHERITED(SpecMemberBase)
public:
  float         ach_reset_thr;  // #MIN_0 #DEF_0.5 threshold on receiving unit ach value, sent by TAN units, for reseting the trace -- only applicable for trace-based learning
  float         max_msn_act;    // for purposes of learning, what is the maximum msn activation -- above this level, learning is effectively zero


  inline float  MsnActLrnFactor(const float msn_act) {
    if(msn_act > max_msn_act) return 0.0f;
    return 2.0f * msn_act * (max_msn_act - msn_act);
  }
  // learning factor for level of msn activation, of the general form of msn * (1-msn), except using max_msn_act instead of 1 -- the factor of 2 compensates for otherwise reduction in learning from these factors
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(MSNTraceSpec);
protected:
  SPEC_DEFAULTS;
  // void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(MSNTraceThalLrates);

class E_API MSNTraceThalLrates : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra gains for trace-based thalamic gated learning in the MSN's
INHERITED(SpecMemberBase)
public:
  float         go_nogo_inhib;  // #DEF_0.2 how much does NoGo (D2) activation in corresponding stripe serve to inhibit learning in Go (D1) pathway
  float         gate_go_pos;    // #DEF_1 learning rate for gated, Go (D1), positive dopamine -- strong positive reinforcement
  float         gate_go_neg;    // #DEF_1 learning rate for gated, Go (D1), negative dopamine -- strong negative punishment
  float         gate_nogo_pos;  // #DEF_0.1 learning rate for gated, NoGo (D2), positive dopamine -- generally weaker -- nogo focuses on punishing bad, not reinforcing good
  float         gate_nogo_neg;  // #DEF_1 learning rate for gated, NoGo (D2), negative dopamine -- strong learning here to learn more NoGo for bad actions
  float         not_go_pos;     // #DEF_0.4 learning rate for not-gated, Go (D1), positive dopamine -- this serves to tune the timing of Go firing, by decreasing weights to the extent that the Go unit fires but does not win the competition, and performance is good (i.e., positive dopamine)
  float         not_go_neg;     // #DEF_0.4 learning rate for not-gated, Go (D1), negative dopamine -- this increases weights to alternative Go firing pathways during errors, to help explore alternatives that work better, given that there are still errors
  float         not_nogo_pos;   // #DEF_0.4 learning rate for not-gated, NoGo (D2), positive dopamine -- weight increases here serve to reinforce nogo firing to block competing responses
  float         not_nogo_neg;   // #DEF_0.4 learning rate for not-gated, NoGo (D2), negative dopamine -- weight decreases here serve to reduce firing of NoGo to explore other alternatives 
  
  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(MSNTraceThalLrates);
protected:
  SPEC_DEFAULTS;
  // void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(MSNConSpec);

class E_API MSNConSpec : public LeabraConSpec {
  // Learning of striatal medium spiny neurons input (afferent) connections -- must have an MSNUnitSpec on recv neuron -- based on dopamine, sender * receiver activation product, and (optionally) thal gating activation signal -- supports a trace mechanism which accumulates an ongoing synaptic trace over time, which is then multiplied by a later dopamine da_p value that is typically driven by primary value (US) outcome at end of a sequence of actions -- dwt = da_p * tr; tr = [thal] * su * ru - otr_lrate * su * ru, representing a contrast between gated activations that go one way, and other non-gated activations that go the opposite way (which supports engagement of alternative gating strategies, and avoids overall reductions in weights) -- the trace is reset when this weight change is computed, as a result of an over-threshold level of dopamine.  Patch units shunt dopamine from actively maintaining stripes / information processing channels, to prevent this clearing.
INHERITED(LeabraConSpec)
public:
  enum MSNConVars {
    NTR = N_LEABRA_CON_VARS,    // new trace -- drives updates to trace value -- thal * ru * su
    TR,                         // current ongoing trace of activations using ru * su, which drive learning -- adds ntr and clears after learning on current values -- includes both thal gated (+ and other nongated, - inputs)
    N_MSN_CON_VARS,
  };

  enum LearnActVal {            // activation value to use for learning
    PREV_TRIAL,                 // previous trial
    ACT_P,                      // act_p from current trial
    ACT_M,                      // act_m from current trial
    ACT_EQ,                     // act_eq from current trial -- use this for PBWM Dorsal matrix
  };
  
  enum LearningRule {           // type of learning rule to use
    DA_HEBB,                    // immediate use of dopamine * send * recv activation triplet to drive learning
    DA_HEBB_VS,                 // ventral striatum version of DA_HEBB, which uses MAX(deep_lrn, ru_act) for recv term in dopamine * send * recv activation triplet to drive learning
    TRACE_THAL,                 // send * recv activation establishes a trace (long-lasting synaptic tag), with thalamic activation determining sign of the trace (if thal active (gated) then sign is positive, else sign is negative) -- when dopamine later arrives, the trace is applied * dopamine, and the amount of dopamine and/or any above-threshold ach from TAN units resets the trace
    TRACE_NO_THAL,              // send * recv activation establishes a trace (long-lasting synaptic tag), with no influence of thalamic gating signal -- when dopamine later arrives, the trace is applied * dopamine, and any above-threshold ach from TAN units resets the trace
    TRACE_NO_THAL_VS,           // ventral striatum version of TRACE_NO_THAL, which uses MAX(deep_lrn, ru_act) for recv term to set trace
    WM_DEPENDENT,               // learning depends on a working memory trace.. 
  };
    
  LearnActVal        su_act_var;     // what variable to use for sending unit activation
  LearnActVal        ru_act_var;     // what variable to use for recv unit activation
  LearningRule       learn_rule;     // what kind of learning rule to use
  MSNTraceSpec       trace;          // #AKA_matrix #CONDSHOW_ON_learn_rule:TRACE_THAL,TRACE_NO_THAL,TRACE_NO_THAL_VS parameters for trace-based learning 
  MSNTraceThalLrates tr_thal;        // #CONDSHOW_ON_learn_rule:TRACE_THAL gain parameters for trace-based thalamic-gated learning 
  float              burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float              dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!

  inline float  GetDa(float da, bool d2r) {
    if(da < 0.0f) da *= dip_da_gain; else da *= burst_da_gain;
    if(d2r) da = -da;
    return da;
  }
  // get effective dopamine signal taking into account gains and reversal by D2R
  

  inline void Init_Weights(ConGroup* cg, Network* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* scales = cg->OwnCnVar(SCALE);
    
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);

    int eff_thr_no = net->HasNetFlag(Network::INIT_WTS_1_THREAD) ? 0 : thr_no;
  
    for(int i=0; i<cg->size; i++) {
      scales[i] = 1.0f;         // default -- must be set in prjn spec if different
    }
    
    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], eff_thr_no);
        C_Init_dWt(dwts[i]);
        ntrs[i] = 0.0f;
        trs[i] = 0.0f;
      }
    }
  }

  inline float GetActVal(LeabraUnitVars* u, const LearnActVal& val) {
    switch(val) {
    case PREV_TRIAL:
      return u->act_q0;
      break;
    case ACT_P:
      return u->act_p;
      break;
    case ACT_M:
      return u->act_m;
      break;
    case ACT_EQ:
      return u->act_eq;
      break;
    }
    return 0.0f;
  }    
  
  // IMPORTANT: need to always build in a temporal asymmetry so LV-level gating does not
  // disrupt the trace right as it is established..
  
  inline void C_Compute_dWt_DaHebb
    (float& dwt, const float da_p, const bool d2r, const float ru_act, const float su_act,
     const float lrate_eff) {
    dwt += lrate_eff * GetDa(da_p, d2r) * ru_act * su_act;
  }
  // #IGNORE
  inline void C_Compute_dWt_DaHebbVS
    (float& dwt, const float da_p, const bool d2r, const float ru_act,
     const float deep_lrn, const float su_act, const float lrate_eff) {
    dwt += lrate_eff * GetDa(da_p, d2r) * fmaxf(ru_act, deep_lrn) * su_act;
  }
  // #IGNORE

  inline void C_Compute_dWt_Trace_Thal
    (float& dwt, float& ntr, float& tr, const float da_p,
     const float ach, const bool d2r, const float ru_thal, const float ru_act,
     const float su_act, const float lrate_eff, const float ru_deep_raw_net) {

    const float da = GetDa(da_p, d2r);
    const bool pos_da = (da_p > 0.0f); // raw da
    if(da != 0.0f) {
      if(tr >= 0.0f) {          // gated trace
        if(d2r) {               // nogo
          if(pos_da) {
            dwt += tr_thal.gate_nogo_pos * lrate_eff * da * tr;
          }
          else {
            dwt += tr_thal.gate_nogo_neg * lrate_eff * da * tr;
          }
        }
        else {                  // go
          if(pos_da) {
            dwt += tr_thal.gate_go_pos * lrate_eff * da * tr;
          }
          else {
            dwt += tr_thal.gate_go_neg * lrate_eff * da * tr;
          }
        }
      }
      else {                    // not-gated trace
        if(d2r) {               // nogo
          if(pos_da) {
            dwt += tr_thal.not_nogo_pos * lrate_eff * da * tr;
          }
          else {
            dwt += tr_thal.not_nogo_neg * lrate_eff * da * tr;
          }
        }
        else {                  // go
          if(pos_da) {
            dwt += tr_thal.not_go_pos * lrate_eff * da * tr;
          }
          else {
            dwt += tr_thal.not_go_neg * lrate_eff * da * tr;
          }
        }
      }
    }

    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }

    float new_ntr = trace.MsnActLrnFactor(ru_act) * su_act;
    if(ru_thal > 0.0f) {        // gated
      ntr = new_ntr;
    }
    else {                      // not-gated
      if(!d2r) {
        new_ntr -= new_ntr * tr_thal.go_nogo_inhib;
      }
      ntr = -new_ntr;           // opposite sign for non-gated
    }

    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE

  inline void C_Compute_dWt_Trace_NoThal
    (float& dwt, float& ntr, float& tr, const float da_p, const float ach, const bool d2r,
     const float ru_act, const float su_act, const float lrate_eff) {

    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * da * tr;

    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }
    
    ntr = ru_act * su_act;
    
    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE
  
  inline void C_Compute_dWt_Trace_NoThalVS
  (float& dwt, float& ntr, float& tr, const float da_p, const float ach, const bool d2r,
   const float ru_act, const float deep_lrn, const float su_act, const float lrate_eff) {
    
    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * da * tr;
    
    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }
    
    ntr = fmaxf(ru_act, deep_lrn) * su_act;
    
    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE

  inline void ClearMSNTrace(LeabraConGroup* scg, LeabraNetwork* net, int thr_no) {
    float* trs = scg->OwnCnVar(TR);
    const int sz = scg->size;
    for(int i=0; i<sz; i++) {
      trs[i] = 0.0f;
    }
  }
  // #IGNORE clear the trace value

  inline void Compute_dWt(ConGroup* scg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)scg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    MSNUnitSpec* rus = (MSNUnitSpec*)rlay->GetUnitSpec();
    const bool d2r = (rus->dar == MSNUnitSpec::D2R);

    const float su_act = GetActVal(su, su_act_var);
    const bool q4 = (net->quarter == 3);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, clrate, deep_on, bg_lrate, fg_lrate);
    
    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);
    
    const int sz = cg->size;
    switch(learn_rule) {
    case DA_HEBB: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        C_Compute_dWt_DaHebb(dwts[i], ru->da_p, d2r, ru_act, su_act, lrate_eff);
      }
      break;
    }
    case DA_HEBB_VS: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        C_Compute_dWt_DaHebbVS(dwts[i], ru->da_p, d2r, ru_act, ru->deep_lrn,
                               su_act, lrate_eff);
      }
      break;
    }
    case TRACE_THAL: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_Thal(dwts[i], ntrs[i], trs[i], 
                        ru->da_p, ach, d2r, ru->thal_cnt, ru_act, su_act, lrate_eff,
                                 ru->deep_raw_net);
      }
      break;
    }
    case TRACE_NO_THAL: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_NoThal(dwts[i], ntrs[i], trs[i],
                            ru->da_p, ach, d2r, ru_act, su_act, lrate_eff);
      }
      break;
    }
    case TRACE_NO_THAL_VS: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          //            lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
          lrate_eff *= (bg_lrate + fg_lrate); // TODO: deep_lrn was turning off before phaDA hits
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_NoThalVS(dwts[i], ntrs[i], trs[i], ru->da_p, ach, d2r,
                                     ru_act, ru->deep_lrn, su_act, lrate_eff);
      }
      break;
    }
    case WM_DEPENDENT: {
      // todo!
      break;
    }
    }
  }
  
  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;
  
  TA_SIMPLE_BASEFUNS(MSNConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MSNConSpec_h
