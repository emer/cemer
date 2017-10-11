// Copyright 2017, Regents of the University of Colorado,
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
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specifications for trace-based learning in the MSN's
INHERITED(SpecMemberBase)
public:
  float         ach_reset_thr;  // #MIN_0 #DEF_0.5 threshold on receiving unit ach value, sent by TAN units, for reseting the trace -- only applicable for trace-based learning
  bool          msn_deriv;      // #DEF_true use the sigmoid derivative factor msn * (1-msn) in modulating learning -- otherwise just multiply by msn activation directly -- this is generally beneficial for learning to prevent weights from continuing to increase when activations are already strong (and vice-versa for decreases)
  float         max_vs_deep_mod; // for VS matrix TRACE_NO_THAL_VS and DA_HEBB_VS learning rules, this is the maximum value that the deep_mod_net modulatory inputs from the basal amygdala (up state enabling signal) can contribute to learning

  inline float  MsnActLrnFactor(const float msn_act) {
    if(!msn_deriv) return msn_act;
    return 2.0f * msn_act * (1.0f - msn_act);
  }
  // learning factor for level of msn activation, of the general form of msn * (1-msn) -- the factor of 2 compensates for otherwise reduction in learning from these factors
  
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
  // ##INLINE ##NO_TOKENS ##CAT_Leabra gains for trace-based thalamic gated learning in the MSN's -- learning is also modulated typically by msn*(1-msn) activation of msn receiving units, to drive learning most in the sensitive middle range of activations (see msn_deriv option)
INHERITED(SpecMemberBase)
public:
  bool          full_params;    // #DEF_false use the full set of 2x2x2=8 parameters for all combinations of gating, go/nogo, and positive/negative dopamine -- otherwise just use the two key parameters that typically matter (other values are all 1.0f)
  float         not_gated;      // #DEF_0.7 #MIN_0 #CONDSHOW_OFF_full_params learning rate for all not-gated stripes, which learn in the opposite direction to the gated stripes, and typically with a slightly lower learning rate -- although there are different learning logics associated with each of these different not-gated cases (click full_params on to see each of them), it turns out in practice that the same learning rate for all works best, and is simplest
 
  float         gate_go_pos;    // #DEF_1 #MIN_0 #CONDSHOW_ON_full_params learning rate for gated, Go (D1), positive dopamine (weights increase) -- this is main direct pathway learning for positive reinforcement (outcomes better than expected), and defaults to 1 as the 'reference' learning rate -- per Thorndike's Law of Effect, actions that result in positive outcomes should be reinforced -- even though the action is already successful, it should be strengthened to better compete with other possible actions in the future, and make the action more vigorous
  float         gate_go_neg;    // #DEF_1 #MIN_0 #CONDSHOW_ON_full_params learning rate for gated, Go (D1), negative dopamine (weights decrease) -- this is the complementary main direct pathway learning for negative reinforcement (outcomes worse than expected), and defaults to 1 to balance the positive case, and allow learning here to track rate of success essentially linearly in an unbiased manner
  float         gate_nogo_pos;  // #DEF_0.1 #MIN_0 learning rate for gated, NoGo (D2), positive dopamine (weights decrease) -- this is the single most important learning parameter here -- by making this relatively small (but non-zero), an asymmetry in the role of Go vs. NoGo is established, whereby the NoGo pathway focuses largely on punishing and preventing actions associated with negative outcomes, while those assoicated with positive outcomes only very slowly get relief from this NoGo pressure -- this is critical for causing the model to explore other possible actions even when a given action SOMETIMES produces good results -- NoGo demands a very high, consistent level of good outcomes in order to have a net decrease in these avoidance weights.  Note that the gating signal applies to both Go and NoGo MSN's for gated stripes, ensuring learning is about the action that was actually selected (see not_ cases for logic for actions that were close but not taken)
  float         gate_nogo_neg;  // #DEF_1 #MIN_0 #CONDSHOW_ON_full_params learning rate for gated, NoGo (D2), negative dopamine (weights increase) -- strong (1.0) learning here to drive more NoGo for actions associated with negative outcomes -- the asymmetry with gate_nogo_pos is key as described there -- this remains at the default 1 maximal learning rate
  float         not_go_pos;     // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, Go (D1), positive dopamine (weights decrease) -- serves to 'preventatively' tune the timing of Go firing, by decreasing weights to the extent that the Go unit fires but does not win the competition, and yet performance is still good (i.e., positive dopamine)
  float         not_go_neg;     // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, Go (D1), negative dopamine (weights increase) -- increases weights to alternative Go firing pathways during errors, to help explore alternatives that work better, given that there are still errors -- because learning is proportional to level of MSN activation, those neurons that are most active, while still not winning the overall gating competition at the GPi stripe level, learn the most
  float         not_nogo_pos;   // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, NoGo (D2), positive dopamine (weights increase) -- these are NoGo units that were active and, because this stripe was not gated, effectively blocked the gating of this action, and performance was overall successful (positive dopamine) -- thus, this learning reinforces that successful blocking to maintain and reinforce it further to the extent that it continues to be successful -- overall this learning must be well balanced with the not_nogo_neg learning -- having the same assymmetry that is present in the gated nogo pos vs. neg is NOT beneficial here -- because these are non-gated stripes, there are presumably a larger population of them and the non-gated nature means that we don't have that good of a credit assignment signal about how critical these are, so the strong punishment-oriented asymmetry doesn't work here -- instead a basic balanced accounting of pos vs. neg for these stripes (using the same values for pos vs. neg) works best
  float         not_nogo_neg;   // #DEF_0.7 #MIN_0 #CONDSHOW_ON_full_params learning rate for not-gated, NoGo (D2), negative dopamine (weights decrease) -- these are NoGo units that were active and therefore caused the corresponding stripe to NOT win the gating competition, and yet the outcome was NOT successful, so weights decrease here to STOP blocking these actions and explore more whether this action might be useful -- see not_nogo_pos for logic about these being balanced values, not strongly asymmetric as in the gated case


  inline float FullLrates(const bool gated, const bool d2r, const bool pos_da) {
    if(gated) {
      if(d2r) {               // nogo
        if(pos_da)
          return gate_nogo_pos;
        else
          return gate_nogo_neg;
      }
      else {                  // go
        if(pos_da)
          return gate_go_pos;
        else
          return gate_go_neg;
      }
    }
    else {                    // not-gated trace
      if(d2r) {               // nogo
        if(pos_da)
          return not_nogo_pos;
        else
          return not_nogo_neg;
      }
      else {                  // go
        if(pos_da)
          return not_go_pos;
        else
          return not_go_neg;
      }
    }
  }
  // get learning rate factor using full set of 2x2x2 parameters

  inline float Lrate(const bool gated, const bool d2r, const bool pos_da) {
    if(full_params) return FullLrates(gated, d2r, pos_da);
    if(!gated) return not_gated;
    if(d2r && pos_da) return gate_nogo_pos;
    return 1.0f;
  }
  // get learning rate using current parameter settings -- call this method
  
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
    ACT_G,                      // act_g from current trial -- activation on cycle of gating -- use this for PBWM Dorsal matrix
  };
  
  enum LearningRule {           // type of learning rule to use
    DA_HEBB,                    // immediate use of dopamine * send * recv activation triplet to drive learning
    DA_HEBB_VS,                 // ventral striatum version of DA_HEBB, which uses MAX(deep_lrn, ru_act) for recv term in dopamine * send * recv activation triplet to drive learning
    TRACE_THAL,                 // send * recv activation establishes a trace (long-lasting synaptic tag), with thalamic activation determining sign of the trace (if thal active (gated) then sign is positive, else sign is negative) -- when dopamine later arrives, the trace is applied * dopamine, and the amount of dopamine and/or any above-threshold ach from TAN units resets the trace
    TRACE_NO_THAL,              // send * recv activation establishes a trace (long-lasting synaptic tag), with no influence of thalamic gating signal -- when dopamine later arrives, the trace is applied * dopamine, and any above-threshold ach from TAN units resets the trace
    TRACE_NO_THAL_VS,           // ventral striatum version of TRACE_NO_THAL, which uses MAX(MIN(deep_mod_net, max_vs_deep_mod), ru_act) for recv term to set trace
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
  

  inline void Init_Weights(ConState* cg, Network* net, int thr_no) override {
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

  inline float GetActVal(LeabraUnitState_cpp* u, const LearnActVal& val) {
    switch(val) {
    case PREV_TRIAL:
      return u->act_q0;
    case ACT_P:
      return u->act_p;
    case ACT_M:
      return u->act_m;
    case ACT_EQ:
      return u->act_eq;
    case ACT_G:
      return u->act_g;
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
     const float deep_mod_net, const float su_act, const float lrate_eff) {
    float eff_ru_act = fmaxf(ru_act, fminf(deep_mod_net, trace.max_vs_deep_mod));
    dwt += lrate_eff * GetDa(da_p, d2r) * eff_ru_act * su_act;
  }
  // #IGNORE

  inline void C_Compute_dWt_Trace_Thal
    (float& dwt, float& ntr, float& tr, const float da_p,
     const float ach, const bool d2r, const float ru_thal, const float ru_act,
     const float su_act, const float lrate_eff, const float ru_deep_raw_net) {

    const float da = GetDa(da_p, d2r);
    const bool pos_da = (da_p > 0.0f); // raw da
    if(da != 0.0f) {
      dwt += tr_thal.Lrate((tr > 0.0f), d2r, pos_da) * lrate_eff * da * tr;
    }

    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }

    float new_ntr = trace.MsnActLrnFactor(ru_act) * su_act;
    if(ru_thal > 0.0f) {        // gated
      ntr = new_ntr;
    }
    else {                      // not-gated
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
    
    ntr = trace.MsnActLrnFactor(ru_act) * su_act;
    
    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE
  
  inline void C_Compute_dWt_Trace_NoThalVS
  (float& dwt, float& ntr, float& tr, const float da_p, const float ach, const bool d2r,
   const float ru_act, const float deep_mod_net, const float su_act, const float lrate_eff) {
    
    const float da = GetDa(da_p, d2r);
    dwt += lrate_eff * da * tr;
    
    if(ach >= trace.ach_reset_thr) {
      tr = 0.0f;
    }
    
    float eff_ru_act = fmaxf(ru_act, fminf(deep_mod_net, trace.max_vs_deep_mod));
    
    ntr = trace.MsnActLrnFactor(eff_ru_act) * su_act;
    
    float decay_factor = fabs(ntr); // decay is function of new trace
    if(decay_factor > 1.0f) decay_factor = 1.0f;
    tr += ntr - decay_factor * tr;
  }
  // #IGNORE

  inline void ClearMSNTrace(LeabraConState_cpp* scg, LeabraNetwork* net, int thr_no) {
    float* trs = scg->OwnCnVar(TR);
    const int sz = scg->size;
    for(int i=0; i<sz; i++) {
      trs[i] = 0.0f;
    }
  }
  // #IGNORE clear the trace value

  inline void Compute_dWt(ConState* scg, Network* rnet, int thr_no) override {
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConState_cpp* cg = (LeabraConState_cpp*)scg;
    LeabraUnitState_cpp* su = (LeabraUnitState_cpp*)cg->ThrOwnUnState(net, thr_no);
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
        LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i,net);
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
        LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        C_Compute_dWt_DaHebbVS(dwts[i], ru->da_p, d2r, ru_act, ru->deep_mod_net,
                               su_act, lrate_eff);
      }
      break;
    }
    case TRACE_THAL: {
      for(int i=0; i<sz; i++) {
        LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_Thal
          (dwts[i], ntrs[i], trs[i], ru->da_p, ach, d2r, ru->thal_cnt,
           ru_act, su_act, lrate_eff, ru->deep_raw_net);
      }
      break;
    }
    case TRACE_NO_THAL: {
      for(int i=0; i<sz; i++) {
        LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i,net);
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
        LeabraUnitState_cpp* ru = (LeabraUnitState_cpp*)cg->UnState(i,net);
        float lrate_eff = clrate;
        if(deep_on) {
          //            lrate_eff *= (bg_lrate + fg_lrate * ru->deep_lrn);
          lrate_eff *= (bg_lrate + fg_lrate); // TODO: deep_lrn was turning off before phaDA hits
        }
        const float ru_act = GetActVal(ru, ru_act_var);
        const float ach = q4 ? ru->ach : 0.0f;
        C_Compute_dWt_Trace_NoThalVS(dwts[i], ntrs[i], trs[i], ru->da_p, ach, d2r,
                                     ru_act, ru->deep_mod_net, su_act, lrate_eff);
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
