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
  float         da_reset_tr;    // #DEF_0.1 amount of dopamine needed to completely reset the trace
  float         tr_decay;       // #DEF_1 #MIN_0 #MAX_1 how much to decay the existing trace when adding a new trace -- actual decay rate is multiplied by the new trace value: decay in proportion to how much new trace is coming in -- so effective decay rate is typically less than value entered here
  float         otr_lrate;      // #CONDSHOW_ON_use_thal #MIN_0 #DEF_0.5 learning rate associated with other non-gated activations (only avail when using thalamic gating) -- should generally be less than 1 -- the non-gated trace has the opposite sign (negative) from the gated trace -- encourages exploration of other alternatives if a negative outcome occurs, so that otr = opposite trace or opponent trace as well as other trace
  bool          protect_pos;    // #CONDSHOW_ON_use_thal #DEF_false do not add in an opposite trace value if the synapse already has a positive trace value from prior gating -- in theory this makes sense to protect the trace over intervening gating events, but only if the gated trace is actually the correct one -- by continuing to add in the opponent traces, the system remains more open to exploration..
  bool          thal_mult;      // #CONDSHOW_ON_use_thal #DEF_false multiply gated trace by the actual thal gating signal value -- otherwise it is just used as a gating signal but does not multiply
  float         tr_max;         // maximum trace value -- cap trace at this value (either positive or negative) -- if tr_decay is 1, this is not needed, but otherwise the trace can build up over time

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
    NTR = SCALE+1,      // new trace -- drives updates to trace value -- thal * ru * su
    TR,                 // current ongoing trace of activations, which drive learning -- adds ntr and clears after learning on current values -- includes both thal gated (+ and other nongated, - inputs)
  };

  enum LearnActVal {            // activation value to use for learning
    PREV_TRIAL,                 // previous trial
    ACT_P,                      // act_p from current trial
    ACT_M,                      // act_m from current trial
  };
  
  enum LearningRule {           // type of learning rule to use
    DA_HEBB,                    // immediate use of dopamine * send * recv activation triplet to drive learning
    TRACE_THAL,                 // send * recv activation establishes a trace (long-lasting synaptic tag), with thalamic activation determining sign of the trace (if thal active (gated) then sign is positive, else sign is negative) -- when dopamine later arrives, the trace is applied * dopamine and the trace is then reset in proportion to size of dopamine
    TRACE_NO_THAL,              // send * recv activation establishes a trace (long-lasting synaptic tag), with no influence of thalamic gating signal -- when dopamine later arrives, the trace is applied * dopamine and the trace is then reset in proportion to size of dopamine
    WM_DEPENDENT,               // learning depends on a working memory trace.. 
  };
    
  LearnActVal        su_act_var;     // what variable to use for sending unit activation
  LearnActVal        ru_act_var;     // what variable to use for recv unit activation
  LearningRule       learn_rule;     // what kind of learning rule to use
  MSNTraceSpec       trace;          // #AKA_matrix #CONDSHOW_ON_learn_rule:TRACE_THAL,TRACE_NO_THAL parameters for trace-based learning 

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

  // IMPORTANT: need to always build in a temporal asymmetry so LV-level gating does not
  // disrupt the trace right as it is established..
  
  inline void C_Compute_dWt_Trace_Thal
    (float& dwt, float& ntr, float& tr, const float otr_lr, const float mtx_da,
     const float ru_thal, const float ru_act, const float su_act) {

    dwt += cur_lrate * mtx_da * tr;
    float reset_factor = (trace.da_reset_tr - fabs(mtx_da)) / trace.da_reset_tr;
    if(reset_factor < 0.0f) reset_factor = 0.0f;
    tr *= reset_factor;

    if(ru_thal > 0.0f) {              // gated
      if(trace.thal_mult)
        ntr = ru_thal * ru_act * su_act;
      else
        ntr = ru_act * su_act;
    }
    else {                      // non-gated, do otr: opposite / other / opponent trace
      if(!trace.protect_pos || tr <= 0.0f) {
        ntr = otr_lr * ru_act * su_act;
      }
      else {
        ntr = 0.0f;             // no otr if already gated!!
      }
    }

    float decay_factor = trace.tr_decay * fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
    if(tr > trace.tr_max) tr = trace.tr_max;
    else if(tr < -trace.tr_max) tr = -trace.tr_max;
  }
  // #IGNORE

  inline void C_Compute_dWt_Trace_NoThal
    (float& dwt, float& ntr, float& tr, const float mtx_da,
     const float ru_act, const float su_act) {

    dwt += cur_lrate * mtx_da * tr;
    float reset_factor = (trace.da_reset_tr - fabs(mtx_da)) / trace.da_reset_tr;
    if(reset_factor < 0.0f) reset_factor = 0.0f;
    tr *= reset_factor;

    ntr = ru_act * su_act;
    
    float decay_factor = trace.tr_decay * fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;

    tr += ntr - decay_factor * tr;
    if(tr > trace.tr_max) tr = trace.tr_max;
    else if(tr < -trace.tr_max) tr = -trace.tr_max;
  }
  // #IGNORE

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (ignore_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    LeabraLayer* rlay = (LeabraLayer*)cg->prjn->layer;
    MSNUnitSpec* rus = (MSNUnitSpec*)rlay->GetUnitSpec(); // todo: checkconfig!!!

    // todo: get su_act and ru_act per enums!!
    
    bool d2r = (rus->dar == MSNUnitSpec::D2R);
    
    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);
    
    const float otr_lr = -trace.otr_lrate;
    
    const int sz = cg->size;
    switch(learn_rule) {
    case DA_HEBB: {
      // todo!
      break;
    }
    case TRACE_THAL: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float da_p = ((d2r) ? -ru->da_p : ru->da_p);
        C_Compute_dWt_Trace_Thal(dwts[i], ntrs[i], trs[i], otr_lr,
                                  da_p, ru->thal, ru->act_eq, su->act_eq);
      }
      break;
    }
    case TRACE_NO_THAL: {
      for(int i=0; i<sz; i++) {
        LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
        float da_p = ((d2r) ? -ru->da_p : ru->da_p);
        C_Compute_dWt_Trace_NoThal(dwts[i], ntrs[i], trs[i], 
                                    da_p, ru->act_eq, su->act_eq);
      }
      break;
    }
    case WM_DEPENDENT: {
      // todo!
      break;
    }
    }
  }

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
