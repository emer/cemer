// Copyright 2016-2018, Regents of the University of Colorado,
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

#ifndef bp_cuda_h
#define bp_cuda_h 1

// parent includes:
#include <Network_cuda.h>

class BpUnitState_cuda : public UnitState_cuda {
  // NVIDIA CUDA backprop unit variables: MUST be exact memory-aligned copy of BpUnitState
public:
  float         bias_pdw;       // #VIEW_HOT previous bias weight change
  float 	err; 		// #VIEW_HOT error value -- this is E for target units, not dEdA
  float 	dEdA;		// #VIEW_HOT derivative of error wrt activation
  float 	dEdNet;		// #VIEW_HOT derivative of error wrt net input
  float         misc1;          // miscellaneous computational value -- used for  exp(netin) in SOFTMAX case, and to hold the index of the most active unit among input connections for MAX_POOL (cast to int)
};



/*
note on specs: could make a template or define or a multiple-inheritance mix-in class for all the shared things between UnitSpec and cuda, but overall easier to just copy and update as needed based on changes in main codebase -- which should be relatively infrequent..
*/

class BpUnitSpec_cuda : public UnitSpec_cuda {
  // NVIDIA CUDA backprop unit spec params -- copied from BpUnitSpec
public:
  
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
  
  // this stuff not supported in CUDA (yet)
  
  // bool          save_err;       // whether to save the actual error value -- this requires extra computation and is not necessary for learning -- just for instructional / informational purposes

  // NLXX1ActSpec  nlxx1;          // #CONDSHOW_ON_act_fun:NLXX1 specs for nlxx1 function when that is being used
  // GaussActSpec  gauss;          // #CONDSHOW_ON_act_fun:GAUSS||act_fun:RBF specs for Gaussian bump or RBF activation function when that is being used
  // BpNoiseType   noise_type;     // type of noise to use
  // RandomSpec    noise;          // #CONDSHOW_OFF_noise_type:NO_NOISE distribution 
  // parameters for random added noise
  
  // these methods keep sigmoidal-type values within the correct range to prevent numerical errors
  CUDAFUN static inline float  ClipSigAct(float y)
  { y = fmaxf(y,0.000001f); y = fminf(y,0.999999f); return y; }
  CUDAFUN static inline float  ClipTanhAct(float y)
  { y = fmaxf(y,-0.999999f); y = fminf(y,0.999999f); return y; }
  CUDAFUN static inline float  ClipSigNet(float x)
  { x = fmaxf(x,-13.81551f); x = fminf(x,13.81551f); return x; }

  // different activation and error derivative functions
  CUDAFUN static inline float  SigmoidFun(float netin)
  { return ClipSigAct(1.0f / (1.0f + expf(-ClipSigNet(netin)))); }
  CUDAFUN static inline float  SigmoidDeriv(float act)
  { act = ClipSigAct(act); return act * (1.0f - act); }
  CUDAFUN static inline float  TanhFun(float netin)
  { return ClipTanhAct( (2.0f / (1.0f + expf(-ClipSigNet(netin)))) - 1.0f); }
  CUDAFUN static inline float  TanhDeriv(float act)
  { act = ClipSigAct(0.5f * (act + 1.0f)); return act * (1.0f - act); }
  CUDAFUN static inline float  ReLuFun(float netin)
  { return fmaxf(netin, 0.0f); }
  CUDAFUN static inline float  ReLuDeriv(float act) { return (act > 0.0f) ? 1.0f : 0.0f; }

  CUDAFUN inline float  ActFromNetin(float netin) {
    switch(act_fun) {
    case SIGMOID:
      return SigmoidFun(netin);
    case TANH:
      return TanhFun(netin);
    case RELU:
      return ReLuFun(netin);
    case LINEAR:
      return netin;
    default:
      return 0.0f;
    }
  }
  // compute activation from netinput

  CUDAFUN inline float ActDeriv(float act) {
    switch(act_fun) {
    case SIGMOID:
      return SigmoidDeriv(act);
    case TANH:
      return TanhDeriv(act);
    case RELU:
      return ReLuDeriv(act);
    case LINEAR:
      return 1.0f;
    case SOFTMAX:
      return 1.0f;
    default:
      return 0.0f;
    }
  }
  // compute derivative of activation from either netin or act


};

class BpConSpec_cuda : public ConSpec_cuda {
  // NVIDIA CUDA backprop con spec params -- copied from BpConSpec
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
  
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  float 	momentum;	// momentum factor -- as of 8.0 this is standardized to NOT include an additional learning rate factor (previous AFTER_LRATE option -- momentum is effectively after the learning rate)
  DecayType     decay_type;     // type of weight decay to apply (before 8.0 this was set by selecting a function, but this prevents optimization)
  float 	decay;		// #CONDSHOW_OFF_decay_type:NO_DECAY decay rate -- the learning rate is also applied to the decay -- i.e., decay comes before the learning rate factor
  WtUpdtType    wt_updt;        // #READ_ONLY type of weight update to perform -- computed from other parameters set -- used to optimize computation

};


class Bp_cuda : public Network_cuda {
  // NVIDIA CUDA support for backprop computation
public:

  void  Compute_NetinAct();
  void  Compute_dEdA_dEdNet();
  void  Compute_dWt(bool sync = true); // could get away without sync..
  void  Compute_Weights(bool sync = true); // could get away without sync..

  Bp_cuda();
};


#endif // bp_cuda_h
