// Copyright, 1995-2007, Regents of the University of Colorado,
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



#ifndef bp_h
#define bp_h

#include <Network>
#include <Schedule>
#include <SigmoidSpec>
#include <float_CircBuffer>
#include <ProjectBase>
#include <ta_vector_ops.h>
#include <SpecMemberBase>

#include "network_def.h"

// forwards this file
class BpConSpec;
class BpCon;
class BpUnitVars;
class BpUnitSpec;
class BpNetwork;
//

// note: dwt is equivalent to the old dEdW and pdw = the old dwt

eTypeDef_Of(BpCon);

class E_API BpCon : public Connection {
  // #STEM_BASE ##CAT_Bp Bp connection
public:
  float		pdw;		// #NO_SAVE previous delta-weight change -- needed for momentum term in the learning mechanism

  BpCon() { pdw = 0.0f; }
};

// ConSpec now has 3 versions of some functions, the two regular ones
// (one for the group and one for the connection), and now a third which is a
// virtual version of the C_ for use with the bias weights.
// every time a C_ version is overloaded, it is necessary to overload the B_
// version, where the new B_ just calls the new C_

eTypeDef_Of(BpConSpec);

class E_API BpConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Bp Bp connection specifications
INHERITED(ConSpec)
public:
  enum BpConVars {
    PDW = N_CON_VARS,           // previous delta weight
    N_BP_CON_VARS,              // #IGNORE number of bp con vars
  };

  enum DecayType {              // type of weight decay -- previously set by a function, but this prevents optimization and use in CUDA, etc
    NO_DECAY,                   // do not perform any form of weight decay
    SIMPLE_DECAY,               // apply simple weight decay -- subtract a constant factor of the weight value with every update to the weights
    ELIMINATION,                // weight elimination weight decay -- subtract: (decay * w) / (1 + wt^2)^2 -- this eliminates small weights but weights that are larger (around 1 or higher) are subject to a much smaller amount of decay -- based on a bimodal underlying distribution -- developed by Weigend et al (1990) -- note that prior to 8.0, this included a 2.0 factor and incorrectly used wt in the numerator which was kind of a deadly mistake!
  };

  enum  WtUpdtType {             // overall type of weight update to perform, based on parameters that are set -- for optimizing the code to use the most appropriate function
    WU_DWT_ONLY,                 // just straight dwt, no decay, no momentum
    WU_SIMPLE_DECAY,             // simple decay
    WU_ELIMINATION,              // weight elimination
    WU_MOMENT,                   // momentum only
    WU_MOMENT_SIMPLE,            // momentum and simple decay
    WU_MOMENT_ELIM,              // momentum and weight elimination
  };
  
  float 	lrate;		// learning rate
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  Schedule	lrate_sched;	// schedule of learning rate over training epochs (multiplies lrate to produce cur_lrate factor)
  float 	momentum;	// momentum factor -- as of 8.0 this is standardized to NOT include an additional learning rate factor (previous AFTER_LRATE option -- momentum is effectively after the learning rate)
  DecayType     decay_type;     // type of weight decay to apply (before 8.0 this was set by selecting a function, but this prevents optimization)
  float 	decay;		// #CONDSHOW_OFF_decay_type:NO_DECAY decay rate -- the learning rate is also applied to the decay -- i.e., decay comes before the learning rate factor
  WtUpdtType    wt_updt;        // #READ_ONLY type of weight update to perform -- computed from other parameters set -- used to optimize computation

  void 		(*decay_fun)(BpConSpec* spec, float& wt, float& dwt);
  // #OBSOLETE #HIDDEN #READ_ONLY #NO_SAVE #LIST_BpConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  inline void   Init_dWt(ConGroup* cg, Network* net, int thr_no) override {
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    for(int i=0; i<cg->size; i++) {
      C_Init_dWt(dwts[i]);
      pdws[i] = 0.0f;
    }
  }

  inline void   Init_Weights_post(ConGroup* cg, Network* net, int thr_no) override {
    float* pdws = cg->OwnCnVar(PDW);
    for(int i=0; i<cg->size; i++) {
      pdws[i] = 0.0f;
    }
  }
  // all non-wt, pdw vars MUST be initialized in _post!
  
  // vectorization notes: Compute_Weights is fully vectorized and easy, as everything
  // is in the connections
  // other functions require unit access -- the sending units are vector chunked,
  // but due to the thread organization, their ram is discontinuous and cannot
  // be sequentially accessed.  The gather<> load function is potentially usable, but
  // it takes a common offset and has different indexes -- not workable with the current
  // setup -- so the only way to make it work would be to copy all the acts into a
  // single common flat array -- probably that takes more time than we save.. but..
  // could give it a try.. would only work for compute_dwt -- overall unlikely to be
  // worth it though, so postponing in favor of CUDA etc at this point
  
  inline float		C_Compute_dEdA(const float wt, const float ru_dEdNet)
  { return wt * ru_dEdNet; }
  // #IGNORE 
  inline virtual float 	Compute_dEdA(ConGroup* cg, Network* net, int thr_no);
  // get error from units I send to

  inline void 		C_Compute_dWt(float& dwt, const float ru_dEdNet,
                                     const float su_act)
  { dwt += su_act * ru_dEdNet; }
  // #IGNORE
  inline void 		Compute_dWt(ConGroup* cg, Network* net, int thr_no) override;
  // Compute dE with respect to the weights

  // the different forms of weight update -- also implemented in vectorized
  inline void C_Compute_Weights_dWtOnly(float& wt, float& dwt) {
    wt += cur_lrate * dwt;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  inline void Compute_Weights_dWtOnly_vec(ConGroup* cg, float* wts, float* dwts) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      wt += cur_lrate * dwt;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_dWtOnly(wts[i], dwts[i]);
    }
  }
#endif
  // no momentum, no decay
  inline void C_Compute_Weights_SimpleDecay(float& wt, float& dwt) {
    wt += cur_lrate * (dwt - decay * wt);
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  inline void Compute_Weights_SimpleDecay_vec(ConGroup* cg, float* wts, float* dwts) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      wt += cur_lrate * (dwt - decay * wt);
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_SimpleDecay(wts[i], dwts[i]);
    }
  }
#endif
  inline void C_Compute_Weights_Elimination(float& wt, float& dwt) {
    float denom = (1.0f + wt * wt);
    wt += cur_lrate * (dwt - ((decay * wt) / (denom * denom)));
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  inline void Compute_Weights_Elimination_vec(ConGroup* cg, float* wts, float* dwts) {
    VECF zeros(0.0f);
    VECF ones(1.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF denom = ones + wt * wt;
      wt += cur_lrate * (dwt - ((decay * wt) / (denom * denom)));
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Elimination(wts[i], dwts[i]);
    }
  }
#endif
  inline void C_Compute_Weights_Moment(float& wt, float& dwt, float& pdw) {
    pdw = cur_lrate * dwt + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  inline void Compute_Weights_Moment_vec(ConGroup* cg, float* wts, float* dwts, float* pdws) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF pdw; pdw.load(pdws+i);
      pdw = cur_lrate * dwt + momentum * pdw;
      wt += pdw;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
      pdw.store(pdws+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Moment(wts[i], dwts[i], pdws[i]);
    }
  }
#endif
  inline void C_Compute_Weights_Moment_Simple(float& wt, float& dwt, float& pdw) {
    pdw = cur_lrate * (dwt - decay * wt) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  inline void Compute_Weights_Moment_Simple_vec(ConGroup* cg, float* wts, float* dwts, float* pdws) {
    VECF zeros(0.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF pdw; pdw.load(pdws+i);
      pdw = cur_lrate * (dwt - decay * wt) + momentum * pdw;
      wt += pdw;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
      pdw.store(pdws+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Moment_Simple(wts[i], dwts[i], pdws[i]);
    }
  }
#endif
  inline void C_Compute_Weights_Moment_Elim(float& wt, float& dwt, float& pdw) {
    float denom = (1.0f + wt * wt);
    pdw = cur_lrate * (dwt - ((decay * wt) / (denom * denom))) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
  }
#ifdef TA_VEC_USE  
  inline void Compute_Weights_Moment_Elim_vec(ConGroup* cg, float* wts, float* dwts, float* pdws) {
    VECF zeros(0.0f);
    VECF ones(1.0f);
    const int sz = cg->size; const int parsz = cg->vec_chunked_size;
    int i;
    for(i=0; i<parsz; i += TA_VEC_SIZE) {
      VECF wt;  wt.load(wts + i);
      VECF dwt; dwt.load(dwts+i);
      VECF pdw; pdw.load(pdws+i);
      VECF denom = ones + wt * wt;
      pdw = cur_lrate * (dwt - ((decay * wt) / (denom * denom))) + momentum * pdw;
      wt += pdw;
      dwt = zeros;
      wt.store(wts+i);
      dwt.store(dwts+i);
      pdw.store(pdws+i);
    }
    for(;i<sz;i++) {              // get the remainder
      C_Compute_Weights_Moment_Elim(wts[i], dwts[i], pdws[i]);
    }
  }
#endif

  inline void	Compute_Weights(ConGroup* cg, Network* net, int thr_no) override {
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);

#ifdef TA_VEC_USE  
    switch(wt_updt) {
    case WU_DWT_ONLY:
      Compute_Weights_dWtOnly_vec(cg, wts, dwts);
      break;
    case WU_SIMPLE_DECAY:
      Compute_Weights_SimpleDecay_vec(cg, wts, dwts);
      break;
    case WU_ELIMINATION:
      Compute_Weights_Elimination_vec(cg, wts, dwts);
      break;
    case WU_MOMENT:
      Compute_Weights_Moment_vec(cg, wts, dwts, pdws);
      break;
    case WU_MOMENT_SIMPLE:
      Compute_Weights_Moment_Simple_vec(cg, wts, dwts, pdws);
      break;
    case WU_MOMENT_ELIM:
      Compute_Weights_Moment_Elim_vec(cg, wts, dwts, pdws);
      break;
    }
#else
    switch(wt_updt) {
    case WU_DWT_ONLY:
      CON_GROUP_LOOP(cg, C_Compute_Weights_dWtOnly(wts[i], dwts[i]));
      break;
    case WU_SIMPLE_DECAY:
      CON_GROUP_LOOP(cg, C_Compute_Weights_SimpleDecay(wts[i], dwts[i]));
      break;
    case WU_ELIMINATION:
      CON_GROUP_LOOP(cg, C_Compute_Weights_Elimination(wts[i], dwts[i]));
      break;
    case WU_MOMENT:
      CON_GROUP_LOOP(cg, C_Compute_Weights_Moment(wts[i], dwts[i], pdws[i]));
      break;
    case WU_MOMENT_SIMPLE:
      CON_GROUP_LOOP(cg, C_Compute_Weights_Moment_Simple(wts[i], dwts[i], pdws[i]));
      break;
    case WU_MOMENT_ELIM:
      CON_GROUP_LOOP(cg, C_Compute_Weights_Moment_Elim(wts[i], dwts[i], pdws[i]));
      break;
    }
#endif

    ApplyLimits(cg, net, thr_no);
  }

  inline void B_Init_dWt(UnitVars* uv, Network* net, int thr_no) override;
  inline void B_Compute_dWt(UnitVars* uv, Network* net, int thr_no) override;
  inline void B_Compute_Weights(UnitVars* uv, Network* net, int thr_no) override;

  virtual void	LogLrateSched(int epcs_per_step = 50, int n_steps=7);
  // #BUTTON #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks

  virtual void	SetCurLrate(BpNetwork* net);
  // set current learning rate based on schedule given epoch

  void InitLinks() override;
  SIMPLE_COPY(BpConSpec);
  TA_BASEFUNS(BpConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};



///////////////////////////////////////////////////////////
//              Units: Spec, Vars, etc

eTypeDef_Of(NLXX1ActSpec);

class E_API NLXX1ActSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra noisy linear XX1 activation function specifications
INHERITED(SpecMemberBase)
public:
  float         ramp_start;     // value of negative net input where the linear ramping function starts -- this should be a negative number, typically between -2 and -5
  float         ramp_max;       // maximum value, achieved at 0, of the linear ramp that started at ramp_start -- typically .05 to .1
  float         ramp_gain;      // #READ_ONLY = ramp_max / -ramp_start
  
  inline float  NLXX1Fun(float netin)
  { if(netin < ramp_start) return 0.0f;
    if(netin > 0.0f) { return ramp_max + (netin / (netin + 1.0f)); }
    return ramp_gain * (ramp_start - netin); }
  // compute the noisy-linear XX1 function

  inline float  NLXX1Deriv(float netin) {
    if(netin > 0.0f) {
      float denom = (1.0f + netin); return 1.0f / (denom * denom); }
    if(netin < ramp_start) return 0.0f;
    return -ramp_gain;
  }
  
  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(NLXX1ActSpec);
protected:
  SPEC_DEFAULTS;
  void        UpdateAfterEdit_impl() override;

private:
  void        Initialize();
  void        Destroy()        { };
  void        Defaults_init();
};

eTypeDef_Of(GaussActSpec);

class E_API GaussActSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra gaussian function
INHERITED(SpecMemberBase)
public:
  float         mean;           // mean of Gaussian
  float         std_dev;        // std deviation of Gaussian
  float         std_dev_r;      // #HIDDEN reciprocal of std_dev
  
  inline float  GaussActFun(float netin)
  { float val = std_dev_r * (netin - mean); return expf(- (val * val)); }
  // compute the gaussian function

  inline float  GaussActDeriv(float act, float net)
  { return -act * 2.0f * (net - mean) * std_dev_r;  }
  
  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(GaussActSpec);
protected:
  SPEC_DEFAULTS;
  void        UpdateAfterEdit_impl() override;

private:
  void        Initialize();
  void        Destroy()        { };
  void        Defaults_init();
};


eTypeDef_Of(BpUnitSpec);

class E_API BpUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_Bp specifications for Bp units, with major different activation functions available 
INHERITED(UnitSpec)
public:
#ifndef __MAKETA__
  static const float SIGMOID_MAX_VAL; // #READ_ONLY #HIDDEN max eval value
  static const float SIGMOID_MIN_VAL; // #READ_ONLY #HIDDEN min eval value
  static const float SIGMOID_MAX_NET; // #READ_ONLY #HIDDEN maximium net input value
#else
  static const float SIGMOID_MAX_VAL = 0.999999f; // #READ_ONLY #HIDDEN max eval value
  static const float SIGMOID_MIN_VAL = 0.000001f; // #READ_ONLY #HIDDEN min eval value
  static const float SIGMOID_MAX_NET = 13.81551f; // #READ_ONLY #HIDDEN maximium net input value
#endif
  
  enum BpActFun {               // backprop activation function to use
    SIGMOID,                    // standard sigmoidal activation in 0-1 range: 1 / (1 + e^-net) -- note that we dropped gain and offset params in 8.0 -- just uses standard gain = 1, 0 offset
    TANH,                       // hyperbolic tangent function -- basically a sigmoid centered around 0 with range -1..1 (which is how we implement it)
    RELU,                       // rectified linear unit, i.e., a simple threshold nonlinearity --- if netinput < 0, activation = 0, else act = net
    LINEAR,                     // purely linear output -- not suitable for hidden units, as some form of nonlinearity is essential, but can be useful for output units
    NLXX1,                      // noisy-linear version of XX1 function = x/(x+1) (= 1/(1+x^1) -- sigmoidal asymptote but hard 0 threshold) where x = netin -- NL version adds a small linear "ramp" for negative inputs starting at ramp_start negative input and reaching a magnitude of ramp_max at 0, after which point it transitions to regular XX1 (starting at ramp_max) -- ramp approximates effects of convolving XX1 with gaussian noise, but with a simpler function -- provides a hybrid of sigmoid and ReLu with good overall properties for sparse representations with bias set to -2
    BINARY,                     // stochastic binary activation -- produces a 1 or a 0 based on sigmoidal underlying probability
    GAUSS,                      // gaussian applied to the standard dot-product netinput -- also known as a 'bump' function
    RBF,                        // radial basis function activation -- uses distance-based net input (net input is distance between activations and weights, instead of usual dot product) and runs that through a gaussian function to produce a radial basis function kernel activation
    MAX_POOL,                   // compute the activation = max over input activations, and send backprop error only back to max input, rest = 0
    SOFTMAX,                    // soft-max over the units within the layer -- does a few passes to compute activation -- useful for single-winner output layers (e.g., localist classification outputs) -- unlike in 8.0, this does NOT require an exponential layer input -- everything is done internally -- implies CROSS_ENTROPY error so that derivative is simple, and is ONLY applicable to output (TARGET) layers
  };

  enum BpErrFun {                // type of error function to use -- replaces err_fun setting from earlier versions 
    SQUARED_ERR,                 // use the squared error function in computing derivatives of Target layer units -- appropriate for unbounded linear targets
    CROSS_ENTROPY,               // use the cross-entropy (sigmoid-based) error function in computing derivatives of Target layer units -- appropriate for binary targets
  };

  enum BpNoiseType {
    NO_NOISE,                   // no noise
    ACT_NOISE,                  // add noise to activations
    NETIN_NOISE,                // add noise to netinputs
    DROPOUT,                    // drop out (zero activations) of units according to probability of noise.mean (like multiplying by a bernoulli distribution)
  };

  BpActFun      act_fun;        // activation function to use -- note that act_range is ignored for most functions except linear-based ones, and for output units using the cross-entropy error function
  BpErrFun      error_fun;      // error function to use: only applicable to layers of type TARGET -- squared error is appropriate for unbounded linear targets, while cross-entropy is more appropriate for binary targets
  float		err_tol;	// #DEF_0.05;0 error tolerance: no error signal for a unit if |targ-act| < err_tol) (i.e., as if act == targ exactly) -- often useful to set to .05 or so to prevent over-learning with binary training signals -- big weights often needed to get very high or low activations
  bool          save_err;       // whether to save the actual error value -- this requires extra computation and is not necessary for learning -- just for instructional / informational purposes
  NLXX1ActSpec  nlxx1;          // #CONDSHOW_ON_act_fun:NLXX1 specs for nlxx1 function when that is being used
  GaussActSpec  gauss;          // #CONDSHOW_ON_act_fun:GAUSS||act_fun:RBF specs for Gaussian bump or RBF activation function when that is being used
  BpNoiseType   noise_type;     // type of noise to use
  RandomSpec    noise;          // #CONDSHOW_OFF_noise_type:NO_NOISE distribution parameters for random added noise
  

  void 	(*err_fun)(BpUnitSpec* spec, BpUnitVars* u);
  // #LIST_BpUnit_Error #OBSOLETE #HIDDEN #READ_ONLY #NO_SAVE replaced by enum -- this points to the error fun, set appropriately

  // these methods keep sigmoidal-type values within the correct range to prevent numerical errors
  static inline float  ClipSigAct(float y)
  { y = fmaxf(y,SIGMOID_MIN_VAL); y = fminf(y,SIGMOID_MAX_VAL); return y; }
  static inline float  ClipTanhAct(float y)
  { y = fmaxf(y,-SIGMOID_MAX_VAL); y = fminf(y,SIGMOID_MAX_VAL); return y; }
  static inline float  ClipSigNet(float x)
  { x = fmaxf(x,-SIGMOID_MAX_NET); x = fminf(x,SIGMOID_MAX_NET); return x; }

  // different activation and error derivative functions
  static inline float  SigmoidFun(float netin)
  { return ClipSigAct(1.0f / (1.0f + expf(-ClipSigNet(netin)))); }
  static inline float  SigmoidDeriv(float act)
  { act = ClipSigAct(act); return act * (1.0f - act); }
  static inline float  TanhFun(float netin)
  { return ClipTanhAct( (2.0f / (1.0f + expf(-ClipSigNet(netin)))) - 1.0f); }
  static inline float  TanhDeriv(float act)
  { act = ClipSigAct(0.5f * (act + 1.0f)); return act * (1.0f - act); }
  static inline float  ReLuFun(float netin)
  { return fmaxf(netin, 0.0f); }
  static inline float  ReLuDeriv(float act) { return (act > 0.0f) ? 1.0f : 0.0f; }
  inline float  NLXX1Fun(float netin) { return nlxx1.NLXX1Fun(netin); }
  inline float  NLXX1Deriv(float netin) { return nlxx1.NLXX1Deriv(netin); }

  inline float  ActFromNetin(float netin, int thr_no) {
    switch(act_fun) {
    case SIGMOID:
      return SigmoidFun(netin);
    case TANH:
      return TanhFun(netin);
    case RELU:
      return act_range.Clip(ReLuFun(netin));
    case LINEAR:
      return act_range.Clip(netin);
    case NLXX1:
      return NLXX1Fun(netin);
    case BINARY:
      return Random::BoolProb(SigmoidFun(netin), thr_no) ? 1.0f : 0.0f;
    case GAUSS:
      return gauss.GaussActFun(netin);
    case RBF:
      return gauss.GaussActFun(netin);
    case MAX_POOL:
      return netin;
    default:
      return 0.0f;
    }
  }
  // compute activation from netinput

  inline float ActDeriv(float netin, float act, int thr_no) {
    switch(act_fun) {
    case SIGMOID:
      return SigmoidDeriv(act);
    case TANH:
      return TanhDeriv(act);
    case RELU:
      return ReLuDeriv(act);
    case LINEAR:
      return 1.0f;
    case NLXX1:
      return NLXX1Deriv(netin);
    case BINARY:
      return act == 0.0f ? 0.0f : 1.0f;
    case GAUSS:
      return gauss.GaussActDeriv(act, netin);
    case RBF:
      return -act;
    case SOFTMAX:
      return 1.0f;
    case MAX_POOL:
      return 1.0f;
    default:
      return 0.0f;
    }
  }
  // compute derivative of activation from either netin or act
  
  // generic unit functions
  void Init_Acts(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Netin(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Act(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_dWt(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Weights(UnitVars* uv, Network* net, int thr_no) override;

  // bp special functions
  virtual void 	Compute_Error(BpUnitVars* u, BpNetwork* net, int thr_no);
  // call the unit error function (only on target units)
  virtual void 	Compute_dEdA(BpUnitVars* u, BpNetwork* net, int thr_no);
  // compute derivative of error with respect to unit activation
  virtual void 	Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no);
  // compute derivative of error with respect to unit net input

  inline void   Compute_dEdA_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no)
  { Compute_dEdA(u, net, thr_no); Compute_Error(u, net, thr_no);
    Compute_dEdNet(u, net, thr_no); }

  virtual void	SetCurLrate(BpUnitVars* u, BpNetwork* net, int thr_no);
  // set current learning rate based on epoch

  virtual void Compute_MaxPoolNetin(BpUnitVars* u, Network* net, int thr_no);
  // compute netin = max over sending acts for max_pool case

  virtual void	GraphActFun(DataTable* graph_data, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function along with derivative (NULL = new graph log)

  void	InitLinks() override;
  SIMPLE_COPY(BpUnitSpec);
  TA_BASEFUNS(BpUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
  
private:
  void 	Initialize();
  void 	Destroy()		{ CutLinks(); }
  void	Defaults_init();
};


eTypeDef_Of(BpUnitVars);

class E_API BpUnitVars : public UnitVars {
  // #STEM_BASE ##CAT_Bp standard feed-forward Bp unit variables
public:
  float         bias_pdw;       // #VIEW_HOT previous bias weight change
  float 	err; 		// #VIEW_HOT error value -- this is E for target units, not dEdA
  float 	dEdA;		// #VIEW_HOT derivative of error wrt activation
  float 	dEdNet;		// #VIEW_HOT derivative of error wrt net input
  float         misc1;          // miscellaneous computational value -- used for exp(netin) in SOFTMAX case, and to hold the index of the most active unit among input connections for MAX_POOL (cast to int)
};

eTypeDef_Of(BpUnit);

class E_API BpUnit : public Unit {
  // #STEM_BASE ##CAT_Bp standard feed-forward Bp unit
INHERITED(Unit)
public:
  inline UnitVars::ExtFlags ext_flag() { return GetUnitVars()->ext_flag; }
  // #CAT_UnitVar external input flags -- determines whether the unit is receiving an external input (EXT), target (TARG), or comparison value (COMP)
  inline float& targ()  { return GetUnitVars()->targ; }
  // #VIEW_HOT #CAT_UnitVar target value: drives learning to produce this activation value
  inline float& ext()   { return GetUnitVars()->ext; }
  // #VIEW_HOT #CAT_UnitVar external input: drives activation of unit from outside influences (e.g., sensory input)
  inline float& act()   { return GetUnitVars()->act; }
  // #VIEW_HOT #CAT_UnitVar activation value -- what the unit communicates to others
  inline float& net()   { return GetUnitVars()->net; }
  // #VIEW_HOT #CAT_UnitVar net input value -- what the unit receives from others  (typically sum of sending activations times the weights)
  inline float& bias_wt() { return GetUnitVars()->bias_wt; }
  // #VIEW_HOT #CAT_UnitVar bias weight value -- the bias weight acts like a connection from a unit that is always active with a constant value of 1 -- reflects intrinsic excitability from a biological perspective
  inline float& bias_dwt() { return GetUnitVars()->bias_dwt; }
  // #VIEW_HOT #CAT_UnitVar change in bias weight value as computed by a learning mechanism

  inline float& bias_pdw()
  { return ((BpUnitVars*)GetUnitVars())->bias_pdw; }
  // #VIEW_HOT #CAT_UnitVar previous bias weight change
  inline float&	err()
  { return ((BpUnitVars*)GetUnitVars())->err; }
  // #VIEW_HOT #CAT_UnitVar error value -- this is E for target units, not dEdA
  inline float& dEdA()
  { return ((BpUnitVars*)GetUnitVars())->dEdA; }
  // #VIEW_HOT #CAT_UnitVar derivative of error wrt activation
  inline float& dEdNet()
  { return ((BpUnitVars*)GetUnitVars())->dEdNet; }
  // #VIEW_HOT #CAT_UnitVar derivative of error wrt net input
  inline float& misc1()
  { return ((BpUnitVars*)GetUnitVars())->misc1; }
  // #VIEW_HOT #CAT_UnitVar miscellaneous computational value -- used for exp(netin) in SOFTMAX case, and to hold the index of the most active unit among input connections for MAX_POOL (cast to int)

  TA_BASEFUNS_NOCOPY(BpUnit);
private:
  void 	Initialize()            { };
  void 	Destroy()		{ };
};

///////////////////////////////////////////////////////////
//      inline functions (for speed)

inline float BpConSpec::Compute_dEdA(ConGroup* cg, Network* net, int thr_no) {
  // this is ptr-con based and thus very slow..
  float rval = 0.0f;
  CON_GROUP_LOOP(cg, rval += C_Compute_dEdA(cg->PtrCn(i,WT,net),
                                            ((BpUnitVars*)cg->UnVars(i,net))->dEdNet));
  return rval;
}


inline void BpConSpec::Compute_dWt(ConGroup* cg, Network* net, int thr_no) {
  BpUnitVars* ru = (BpUnitVars*)cg->ThrOwnUnVars(net, thr_no);
  const float ru_dEdNet = ru->dEdNet;
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_dEdNet, cg->UnVars(i,net)->act));
  // todo: if unit act is all in a contiguous vector, and with vec chunking, this 
  // could be a very fast vector op
}

inline void BpConSpec::B_Init_dWt(UnitVars* uv, Network* net, int thr_no) {
  C_Init_dWt(uv->bias_dwt);
  ((BpUnitVars*)uv)->bias_pdw = 0.0f;
}

inline void BpConSpec::B_Compute_dWt(UnitVars* uv, Network* net, int thr_no) {
  uv->bias_dwt += ((BpUnitVars*)uv)->dEdNet;
}

inline void BpConSpec::B_Compute_Weights(UnitVars* u, Network* net, int thr_no) {
  BpUnitVars* uv = (BpUnitVars*)u;
  switch(wt_updt) {
  case WU_DWT_ONLY:
    C_Compute_Weights_dWtOnly(uv->bias_wt, uv->bias_dwt);
    break;
  case WU_SIMPLE_DECAY:
    C_Compute_Weights_SimpleDecay(uv->bias_wt, uv->bias_dwt);
    break;
  case WU_ELIMINATION:
    C_Compute_Weights_Elimination(uv->bias_wt, uv->bias_dwt);
    break;
  case WU_MOMENT:
    C_Compute_Weights_Moment(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
    break;
  case WU_MOMENT_SIMPLE:
    C_Compute_Weights_Moment_Simple(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
    break;
  case WU_MOMENT_ELIM:
    C_Compute_Weights_Moment_Elim(uv->bias_wt, uv->bias_dwt, uv->bias_pdw);
    break;
  }
  C_ApplyLimits(uv->bias_wt);
}


//////////////////////////////////////////////////////////////
//	                Specialized types


eTypeDef_Of(HebbBpConSpec);

class E_API HebbBpConSpec : public BpConSpec {
  // Simple Hebbian wt update (send act * recv act)
INHERITED(BpConSpec)
public:
  inline void C_Compute_dWt(float& dwt, const float ru_act, const float su_act)
  { dwt += su_act * ru_act; }

  inline void Compute_dWt(ConGroup* cg, Network* net, int thr_no) override {
    BpUnitVars* ru = (BpUnitVars*)cg->ThrOwnUnVars(net, thr_no);
    const float ru_act = ru->HasExtFlag(UnitVars::TARG) ? ru->targ : ru->act;
    float* dwts = cg->OwnCnVar(DWT);
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_act, cg->UnVars(i,net)->act));
  }

  inline void B_Compute_dWt(UnitVars* uv, Network* net, int thr_no) override {
    const float ru_act = uv->HasExtFlag(UnitVars::TARG) ? uv->targ : uv->act;
    uv->bias_dwt += ru_act;            // is this really what we want?
  }

  TA_BASEFUNS_NOCOPY(HebbBpConSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};


eTypeDef_Of(ErrScaleBpConSpec);

class E_API ErrScaleBpConSpec : public BpConSpec {
  // con spec that scales the error by given parameter -- can be used to differentially weight the learning impact of one projection relative to another
INHERITED(BpConSpec)
public:
  float		err_scale;	// the scaling parameter

  inline float 		C_Compute_dEdA(const float wt, const float ru_dEdNet)
  { return err_scale * wt * ru_dEdNet; }

  inline float 	Compute_dEdA(ConGroup* cg, Network* net, int thr_no) override {
    // this is ptr-con based and thus very slow..
    float rval = 0.0f;
    CON_GROUP_LOOP(cg, rval += C_Compute_dEdA(cg->PtrCn(i,WT,net),
                                              ((BpUnitVars*)cg->UnVars(i,net))->dEdNet));
    return rval;
  }

  TA_SIMPLE_BASEFUNS(ErrScaleBpConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize()	{ err_scale = 1.0f; }
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(DeltaBarDeltaBpCon);

class E_API DeltaBarDeltaBpCon : public BpCon {
  // delta-bar-delta connection object with local learning rate
public:
  float 		lr; 		// #NO_SAVE local synapse-specific learning rate

  DeltaBarDeltaBpCon() { lr = 0.0f; }
};

eTypeDef_Of(DeltaBarDeltaBpConSpec);

class E_API DeltaBarDeltaBpConSpec : public BpConSpec {
  // delta-bar-delta has local learning rates for each connection
INHERITED(BpConSpec)
public:
  enum DBDBpConVars {
    LR = N_BP_CON_VARS,         // local learning rate
  };

  float		lrate_incr;	// rate of learning rate increase (additive)
  float		lrate_decr;	// rate of learning rate decrease (multiplicative)
  float		act_lrate_incr;	// #HIDDEN actual lrate increase (times lrate)

  inline void   Init_Weights(ConGroup* cg, Network* net, int thr_no) override {
    Init_Weights_symflag(net, thr_no);

    int eff_thr_no = net->HasNetFlag(Network::INIT_WTS_1_THREAD) ? 0 : thr_no;
    
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    float* lrs = cg->OwnCnVar(LR);

    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i], eff_thr_no);
        C_Init_dWt(dwts[i]);
        pdws[i] = 0.0f;
        lrs[i] = lrate;
      }
    }
  }

  inline void	C_UpdateLrate(float& lr, const float dwt, const float pdw) {
    const float prod = pdw * dwt;
    if(prod > 0.0f)
      lr += act_lrate_incr;
    else if(prod < 0.0f)
      lr *= lrate_decr;
    // prod = 0 means first epoch, don't change lrate..
  }

  inline void C_Compute_Weights_DBD(float& wt, float& dwt, float& pdw, float& lr) {
    pdw = lr * dwt + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
    C_UpdateLrate(lr, dwt, pdw);
  }
  inline void C_Compute_Weights_DBD_Simple(float& wt, float& dwt, float& pdw, float& lr) {
    pdw = lr * (dwt - decay * wt) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
    C_UpdateLrate(lr, dwt, pdw);
  }
  inline void C_Compute_Weights_DBD_Elim(float& wt, float& dwt, float& pdw, float &lr) {
    float denom = (1.0f + wt * wt);
    pdw = lr * (dwt - ((decay * wt) / (denom * denom))) + momentum * pdw;
    wt += pdw;
    dwt = 0.0f;
    C_UpdateLrate(lr, dwt, pdw);
  }
  
  inline void	Compute_Weights(ConGroup* cg, Network* net, int thr_no) override {
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    float* lrs = cg->OwnCnVar(LR);

    switch(decay_type) {
    case NO_DECAY: {
      CON_GROUP_LOOP(cg, C_Compute_Weights_DBD(wts[i], dwts[i], pdws[i], lrs[i]));
      break;
    }
    case SIMPLE_DECAY: {
      CON_GROUP_LOOP(cg, C_Compute_Weights_DBD_Simple(wts[i], dwts[i], pdws[i], lrs[i]));
      break;
    }
    case ELIMINATION: {
      CON_GROUP_LOOP(cg, C_Compute_Weights_DBD_Elim(wts[i], dwts[i], pdws[i], lrs[i]));
      break;
    }
    }
    ApplyLimits(cg, net, thr_no);
  }

  // bias weight is NOT suported for delta-bar-delta -- would require a new unit var -- not worth it..

  TA_SIMPLE_BASEFUNS(DeltaBarDeltaBpConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init();
};

//////////////////////////////////////////
//	Additional Unit Types		//
//////////////////////////////////////////

eTypeDef_Of(BpContextSpec);

class E_API BpContextSpec : public BpUnitSpec {
  // for context units in simple recurrent nets (SRN), expects one-to-one prjn from layer it copies, must be AFTER that layer in .layers
INHERITED(BpUnitSpec)
public:
  float		hysteresis;	 // hysteresis factor: (1-hyst)*new + hyst*old
  float		hysteresis_c;	 // #READ_ONLY complement of hysteresis
  RandomSpec	initial_act;	 // initial activation value
  String	variable;	 // name of unit variable to copy into
  UnitVars::ExtFlags unit_flags;	 // flags to set on the unit after copying value
  MemberDef*	var_md;		 // #IGNORE memberdef of variable

  void Init_Acts(UnitVars* uv, Network* net, int thr_no) override;
  void Compute_Act(UnitVars* uv, Network* net, int thr_no) override;
  // copy activation from corresponding unit in projection from layer

  // nullify all other functions..
  void Compute_Netin(UnitVars*, Network* net, int thr_no) 	override { };
  void Init_dWt(UnitVars*, Network* net, int thr_no) 	override { };
  void Compute_dWt(UnitVars*, Network* net, int thr_no) 	override { };
  void Compute_Weights(UnitVars*, Network* net, int thr_no) 	override { };

  // bp special functions
  void Compute_Error(BpUnitVars*, BpNetwork* net, int thr_no)  override { };
  void Compute_dEdA(BpUnitVars*, BpNetwork* net, int thr_no) override { };
  void Compute_dEdNet(BpUnitVars*, BpNetwork* net, int thr_no) override { }; //

  TA_SIMPLE_BASEFUNS(BpContextSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};



eTypeDef_Of(BpLayer);

class E_API BpLayer : public Layer {
  // #STEM_BASE ##CAT_Bp A feedforward backpropagation layer
INHERITED(Layer)
public:

  TA_BASEFUNS_NOCOPY(BpLayer);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

eTypeDef_Of(BpNetwork);

class E_API BpNetwork : public Network {
  // #STEM_BASE ##CAT_Bp project for feedforward backpropagation networks (recurrent backprop is in RBpNetwork)
INHERITED(Network)
public:
  bool	bp_to_inputs;	// #DEF_false backpropagate errors to input layers (faster if not done, which is the default)
  int   prev_epoch;     // #NO_SAVE #HIDDEN #READ_ONLY previous epoch counter -- for detecting changes
  bool  lrate_updtd;    // #NO_SAVE #HIDDEN #READ_ONLY if learning rate was updated..
    
  virtual void	SetCurLrate_Thr(int thr_no);
  // #IGNORE set current learning rate, based on network epoch counter

  virtual void	Compute_dEdA_dEdNet_Thr(int thr_no);
  // #IGNORE compute derivatives of error with respect to activations & net inputs (backpropagate)
  virtual void	Compute_Error();
  // #IGNORE compute local error values, for display purposes only (only call when testing, not training)

  void  Init_Weights() override;
  void  Compute_NetinAct_Thr(int thr_no) override;
  void	Compute_dWt_Thr(int thr_no) override;
  void	Compute_Weights_Thr(int thr_no) override;
  
  virtual void  Trial_Run(); // #CAT_Bp run one trial of Bp: calls SetCurLrate, Compute_NetinAct, Compute_dEdA_dEdNet, and, if train_mode == TRAIN, Compute_dWt.  If you want to save some speed just for testing, you can just call Compute_NetinAct and skip the other two (esp Compute_dEdA_dEdNet, which does a full backprop and is expensive, but often useful for visualization & testing)
  virtual void  Trial_Run_Thr(int thr_no); // #IGNORE
  
  void	SetProjectionDefaultTypes(Projection* prjn) override;
  void  BuildNullUnit() override;

#ifdef CUDA_COMPILE
  bool  Cuda_MakeCudaNet() override;

  void  Cuda_CopyUnitSpec(void* cuda_us, const UnitSpec* source) override;
  void  Cuda_CopyConSpec(void* cuda_cs, const ConSpec* source) override;

  virtual void  Cuda_Trial_Run();
  // #IGNORE
  virtual void  Cuda_Compute_NetinAct();
  // #IGNORE
  virtual void  Cuda_Compute_dEdA_dEdNet();
  // #IGNORE
  virtual void  Cuda_Compute_dWt();
  // #IGNORE
  virtual void  Cuda_Compute_Weights();
  // #IGNORE
#endif
  
  TA_SIMPLE_BASEFUNS(BpNetwork);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{}
};


eTypeDef_Of(BpProject);

class E_API BpProject : public ProjectBase {
  // #STEM_BASE ##CAT_Bp project for backpropagation networks
INHERITED(ProjectBase)
public:

  TA_BASEFUNS_NOCOPY(BpProject);
private:
  void	Initialize();
  void 	Destroy()		{}
};



// These are now OBSOLETE -- remove sometime (as of 8/2016, version 8.0)

// #REG_FUN
E_API void Bp_Simple_WtDecay(BpConSpec* spec, float& wt, float& dwt)
// #LIST_BpConSpec_WtDecay #OBSOLETE -- replaced with enum -- Simple weight decay (subtract decay*wt)
     ;				// term here so maketa picks up comment
// #REG_FUN
E_API void Bp_WtElim_WtDecay(BpConSpec* spec, float& wt, float& dwt)
// #LIST_BpConSpec_WtDecay #OBSOLETE -- replaced with enum -- Weight Elimination (Rumelhart) weight decay
     ;				// term here so maketa picks up comment

// #REG_FUN
E_API void Bp_Squared_Error(BpUnitSpec* spec, BpUnitVars* u)
// #LIST_BpUnit_Error #OBSOLETE -- replaced with enum -- Squared error function for bp
     ;				// term here so scanner picks up comment
// #REG_FUN
E_API void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnitVars* u)
// #LIST_BpUnit_Error #OBSOLETE -- replaced with enum -- Cross entropy error function for bp
     ;				// term here so scanner picks up comment



#endif // bp.h
