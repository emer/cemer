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
  float         otr_lrate;      // #MIN_0 #DEF_0.5 learning rate associated with other non-gated activations (only avail when using thalamic gating) -- should generally be less than 1 -- the non-gated trace has the opposite sign (negative) from the gated trace -- encourages exploration of other alternatives if a negative outcome occurs, so that otr = opposite trace or opponent trace as well as other trace
  float         da_reset_tr;    // #DEF_0.2;0 amount of dopamine needed to completely reset the trace -- if > 0, then either da or ach can reset the trace
  float         ach_reset_thr;  // #MIN_0 #DEF_0.5 threshold on receiving unit ach value, sent by TAN units, for reseting the trace -- only applicable for trace-based learning

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(MSNTraceSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
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
  enum MtxConVars {
    NTR = N_LEABRA_CON_VARS,    // new trace -- drives updates to trace value -- thal * ru * su
    TR,                         // current ongoing trace of activations, which drive learning -- adds ntr and clears after learning on current values -- includes both thal gated (+ and other nongated, - inputs)
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
    TRACE_NO_THAL_VS,            // ventral striatum version of TRACE_NO_THAL, which uses MAX(deep_lrn, ru_act) for recv term to set trace
    WM_DEPENDENT,               // learning depends on a working memory trace.. 
  };
    
  LearnActVal        su_act_var;     // what variable to use for sending unit activation
  LearnActVal        ru_act_var;     // what variable to use for recv unit activation
  LearningRule       learn_rule;     // what kind of learning rule to use
  MSNTraceSpec       trace;          // #AKA_matrix #CONDSHOW_ON_learn_rule:TRACE_THAL,TRACE_NO_THAL parameters for trace-based learning 
  float         burst_da_gain;  // #MIN_0 multiplicative gain factor applied to positive dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!
  float         dip_da_gain;    // #MIN_0 multiplicative gain factor applied to negative dopamine signals -- this operates on the raw dopamine signal prior to any effect of D2 receptors in reversing its sign!

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

    for(int i=0; i<cg->size; i++) {
      scales[i] = 1.0f;         // default -- must be set in prjn spec if different
    }
    
    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], thr_no);
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
    dwt += lrate_eff * GetDa(da_p, d2r) * MAX(ru_act, deep_lrn) * su_act;
  }
  // #IGNORE

  inline void C_Compute_dWt_Trace_Thal
    (float& dwt, float& ntr, float& tr, const float otr_lr, const float da_p,
     const float ach, const bool d2r,
     const float ru_thal, const float ru_act, const float su_act, const float lrate_eff) {

    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * da * tr;

    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }
    else if(trace.da_reset_tr > 0.0f) {
      float reset_factor = (trace.da_reset_tr - fabs(da)) / trace.da_reset_tr;
      if(reset_factor < 0.0f) reset_factor = 0.0f;
      tr *= reset_factor;
    }

    if(ru_thal > 0.0f) {              // gated
      ntr = ru_act * su_act;
    }
    else {                      // non-gated, do otr: opposite / other / opponent trace
      ntr = otr_lr * ru_act * su_act;
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
    else if(trace.da_reset_tr > 0.0f) {
      float reset_factor = (trace.da_reset_tr - fabs(da)) / trace.da_reset_tr;
      if(reset_factor < 0.0f) reset_factor = 0.0f;
      tr *= reset_factor;
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
    else if(trace.da_reset_tr > 0.0f) {
      float reset_factor = (trace.da_reset_tr - fabs(da)) / trace.da_reset_tr;
      if(reset_factor < 0.0f) reset_factor = 0.0f;
      tr *= reset_factor;
    }
    
    ntr = MAX(ru_act, deep_lrn) * su_act;
    
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
    bool d2r = (rus->dar == MSNUnitSpec::D2R);

    float su_act = GetActVal(su, su_act_var);

    float clrate, bg_lrate, fg_lrate;
    bool deep_on;
    GetLrates(cg, clrate, deep_on, bg_lrate, fg_lrate);
    
    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);
    
    const float otr_lr = -trace.otr_lrate;
    
    const int sz = cg->size;
    switch(learn_rule) {
    case DA_HEBB: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        float ru_act = GetActVal(ru, ru_act_var);
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
        float ru_act = GetActVal(ru, ru_act_var);
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
        float ru_act = GetActVal(ru, ru_act_var);
        C_Compute_dWt_Trace_Thal(dwts[i], ntrs[i], trs[i], otr_lr,
                         ru->da_p, ru->ach, d2r, ru->thal, ru_act, su_act, lrate_eff);
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
        float ru_act = GetActVal(ru, ru_act_var);
        C_Compute_dWt_Trace_NoThal(dwts[i], ntrs[i], trs[i],
                            ru->da_p, ru->ach, d2r, ru_act, su_act, lrate_eff);
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
          float ru_act = GetActVal(ru, ru_act_var);
          C_Compute_dWt_Trace_NoThalVS(dwts[i], ntrs[i], trs[i], ru->da_p, ru->ach, d2r,
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

  bool  CheckConfig_RecvCons(ConGroup* cg, bool quiet=false) override;
  
  TA_SIMPLE_BASEFUNS(MSNConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MSNConSpec_h
