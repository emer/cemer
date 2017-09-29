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

#ifndef LeabraUnitSpec_core_h
#define LeabraUnitSpec_core_h 1

// This file contains the core inline Leabra algorithm code for Unit-level computations
// it can be shared between basic C++ and other systems such as CUDA

// outer context including this MUST have the following defines:

// for member class elements that group together relevant parameters
// #define MEMBER_CLASS_SUFFIX _cuda -- or empty for base case
// #define SPEC_MEMBER_BASE SpecMemberBase or SpecMemberBase_cuda or ..

// for main spec class
// #define SPEC_CLASS_SUFFIX _cuda or _core
// #define SPEC_BASE UnitSpec_cuda or UnitSpec

// make sure we have a cudafun defined -- actual cuda needs to include ta_cuda.h
#ifndef CUDAFUN
#define CUDAFUN
#endif

#define INLINE CUDAFUN inline

#include <UnitVars_core>

class LeabraUnitVars_gen : public UnitVars_gen {
  // generic version of LeabraUnitVars -- e.g., same as cuda
public:
  // directly include variables here!
  #include "LeabraUnitVars_core.h"
};

#define PASTETWOITEMSTOGETHER(c,s) c ## s

#define CLASS_SUFFIXED(c,s) PASTETWOITEMSTOGETHER(c,s)

#define SPEC_MEMBER(c) CLASS_SUFFIXED(c,MEMBER_CLASS_SUFFIX)

#define SPEC_MEMBER_CLASS(c) class E_API CLASS_SUFFIXED(c,MEMBER_CLASS_SUFFIX) : public SPEC_MEMBER_BASE

#define SPEC_CLASS(c) class E_API CLASS_SUFFIXED(c,SPEC_CLASS_SUFFIX) : public SPEC_BASE

#ifdef LeabraUnitSpec_h
// for the standard C++ TA case

#include <LeabraUnitVars>

#define UNITVARS LeabraUnitVars

#define TA_STD_CODE(c) \
  String        GetTypeDecoKey() const override { return "UnitSpec"; } \
  \
  TA_SIMPLE_BASEFUNS(CLASS_SUFFIXED(c, MEMBER_CLASS_SUFFIX));   \
protected: \
  SPEC_DEFAULTS; \
private: \
  void Destroy() { }

#define TA_STD_CODE_SPEC(c) \
  TA_SIMPLE_BASEFUNS(CLASS_SUFFIXED(c, SPEC_CLASS_SUFFIX));     \
protected: \
  SPEC_DEFAULTS; \
private: \
  void Destroy() { }

#define UPDATE_AFTER_EDIT(c) \
  protected: \
  void UpdateAfterEdit_impl() override { inherited::UpdateAfterEdit_impl(); \
    c }

#else
// for cuda / other

#define UNITVARS LeabraUnitVars_gen

#define TA_STD_CODE(c) \
  CLASS_SUFFIXED(c, MEMBER_CLASS_SUFFIX) () { Initialize(); }

#define TA_STD_CODE_SPEC(c) \
  CLASS_SUFFIXED(c, SPEC_CLASS_SUFFIX) () { Initialize(); }

#define UPDATE_AFTER_EDIT(c) void UpdateAfterEdit_impl() { c }

#endif


////////////////////////////////////////////////////////////////////////////////////

// this takes maketa to treat this file as a target when processing LeabraUnitSpec.h
// note: it doesn't process the .h extension
#pragma maketa_file_is_target LeabraUnitSpec


SPEC_MEMBER_CLASS(LeabraActFunSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra activation function specifications, using the gelin (g_e linear) activation function by default
INHERITED(SPEC_MEMBER_BASE)
public:
  float         thr;                // #DEF_0.5 threshold value Theta (Q) for firing output activation (.5 is more accurate value based on AdEx biological parameters and normalization -- see BioParams button)
  float         gain;                // #DEF_80;100;40 #MIN_0 gain (gamma) of the rate-coded activation functions -- 100 is default, 80 works better for larger models, and 40 is closer to the actual spiking behavior of the AdEx model -- use lower values for more graded signals, generally in lower input/sensory layers of the network
  float         nvar;                // #DEF_0.005;0.01 #MIN_0 variance of the Gaussian noise kernel for convolving with XX1 in NOISY_XX1 and NOISY_LINEAR -- determines the level of curvature of the activation function near the threshold -- increase for more graded responding there -- note that this is not actual stochastic noise, just constant convolved gaussian smoothness to the activation function
  float         sig_mult;            // #DEF_0.33 #EXPERT multiplier on sigmoid used for computing values for net < thr
  float         sig_mult_pow;        // #DEF_0.8 #EXPERT power for computing sig_mult_eff as function of gain * nvar
  float         sig_gain;            // #DEF_3 #EXPERT gain multipler on (net - thr) for sigmoid used for computing values for net < thr
  float         interp_range;        // #DEF_0.01 #EXPERT interpolation range above zero to use interpolation
  float         gain_cor_range;      // #DEF_10 #EXPERT range in units of nvar over which to apply gain correction to compensate for convolution
  float         gain_cor;            // #DEF_0.1 #EXPERT gain correction multiplier -- how much to correct gains

  float         sig_gain_nvar;      // #READ_ONLY sig_gain / nvar
  float         sig_mult_eff;       // #READ_ONLY overall multiplier on sigmoidal component for values below threshold = sig_mult * pow(gain * nvar, sig_mult_pow)
  float         sig_val_at_0;       // #READ_ONLY 0.5 * sig_mult_eff -- used for interpolation portion
  float         interp_val;         // #READ_ONLY function value at interp_range - sig_val_at_0 -- for interpolation


  INLINE float  XX1(float x) { return x / (x + 1.0f); }
  // x/(x+1) function

  INLINE float  XX1GainCor(float x) {
    float gain_cor_fact = (gain_cor_range - (x / nvar)) / gain_cor_range;
    if(gain_cor_fact < 0.0f) {
      return XX1(gain * x);
    }
    float new_gain = gain * (1.0f - gain_cor * gain_cor_fact);
    return XX1(new_gain * x);
  }
  // x/(x+1) with gain correction within gain_cor_range to compensate for convolution effects
  
  INLINE float  NoisyXX1(float x) {
    if(x < 0.0f) {        // sigmoidal for < 0
      return sig_mult_eff / (1.0f + expf(-(x * sig_gain_nvar)));
    }
    else if(x < interp_range) {
      float interp = 1.0f - ((interp_range - x) / interp_range);
      return sig_val_at_0 + interp * interp_val;
    }
    else {
      return XX1GainCor(x);
    }
  }
  // noisy x/(x+1) function -- directly computes close approximation to x/(x+1) convolved with a gaussian noise function with variance nvar -- no need for a lookup table -- very reasonable approximation for standard range of parameters (nvar = .01 or less -- higher values of nvar are less accurate with large gains, but ok for lower gains)

  TA_STD_CODE(LeabraActFunSpec);
  
  UPDATE_AFTER_EDIT( UpdateParams(); );
  
private:
  INLINE void UpdateParams() {  // too many to duplicate..
    sig_gain_nvar = sig_gain / nvar;
    sig_mult_eff = sig_mult * powf(gain * nvar, sig_mult_pow);
    sig_val_at_0 = 0.5f * sig_mult_eff;
    interp_val = XX1GainCor(interp_range) - sig_val_at_0;
  }

  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    thr = 0.5f;  gain = 100.0f;  nvar = 0.005f;
    sig_mult = 0.33f; sig_mult_pow = 0.8f; sig_gain = 3.0f;
    interp_range = 0.01f; gain_cor_range = 10.0f; gain_cor = 0.1f;
    UpdateParams();
  }
};


SPEC_MEMBER_CLASS(LeabraActMiscSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra miscellaneous activation computation parameters and specs
INHERITED(SPEC_MEMBER_BASE)
public:
  bool          rec_nd;         // record the act_nd non-depressed activation variable (instead of act_eq) for the act_q* quarter-trial and phase (act_m, act_p) activation state variables -- these are used primarily for statistics, or possibly for specialized learning mechanisms
  bool          avg_nd;         // use the act_nd non-depressed activation variable (instead of act_eq) for the time-average activation values (avg_ss, avg_s, avg_m, avg_l) used in the XCAL learning mechanism -- this is appropriate for action-potential driven learning dynamics, as compared to synaptic efficacy, when short term plasticity is present
  bool          dif_avg;        // compute act_dif as avg_s_eff - avg_m (difference of average values that actually drive learning) -- otherwise it is act_p - act_m (difference of final activation states in plus phase minus minus phase -- the typical error signal)
  float         net_gain;       // #DEF_1 #MIN_0 multiplier on total synaptic net input -- this multiplies the net_raw, but AFTER the net_raw variable is saved (upon which the netin_raw statistics are computed)

  float         act_max_hz;     // #DEF_100 #MIN_1 for translating rate-code activations into discrete spiking (only used for clamped layers), what is the maximum firing rate associated with a maximum activation value (max act is typically 1.0 -- depends on act_range)
  bool          avg_trace;      // #DEF_false set act_avg unit variable to the exponentially decaying trace of activation -- used for TD (temporal differences) reinforcement learning for example -- lambda parameter determines how much of the prior trace carries over into the new trace 
  float         lambda;         // #CONDSHOW_ON_avg_trace determines how much of the prior trace carries over into the new trace (act_avg = lambda * act_avg + new_act)
  float         avg_tau;        // #CONDSHOW_OFF_avg_trace #DEF_200 #MIN_1 for integrating activation average (act_avg), time constant in trials (roughly, how long it takes for value to change significantly) -- used mostly for visualization and tracking "hog" units
  float         avg_init;        // #DEF_0.15 #MIN_0 initial activation average value -- used for act_avg, avg_s, avg_m, avg_l
  float         avg_dt;          // #READ_ONLY #EXPERT rate = 1 / tau

  INLINE int    ActToInterval(const float time_inc, const float integ, const float act)
  { return (int) (1.0f / (time_inc * integ * act * act_max_hz)); }
  // #CAT_ActMisc compute spiking interval based on network time_inc, dt.integ, and unit act -- note that network time_inc is usually .001 = 1 msec per cycle -- this depends on that being accurately set

  TA_STD_CODE(LeabraActMiscSpec);

  UPDATE_AFTER_EDIT(avg_dt = 1.0f / avg_tau;);

private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init()   {
    rec_nd = true; avg_nd = true; dif_avg = false; net_gain = 1.0f;
    act_max_hz = 100.0f; avg_trace = false; lambda = 0.0f; avg_tau = 200.0f;
    avg_init = 0.15f;

    avg_dt = 1.0f / avg_tau;
  }
};


SPEC_MEMBER_CLASS(SpikeFunSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra spiking activation function specs -- conductance is computed postsynaptically using an alpha function based on spike pulses sent presynaptically -- for clamped layers, spiking probability is proportional to external input controlled by the clamp_type and clamp_max_p values -- soft clamping may still be a better option though
INHERITED(SPEC_MEMBER_BASE)
public:
  float         rise;                // #DEF_0 #MIN_0 exponential rise time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to only include decay time (1/decay e^(-t/decay)), which is highly optimized (doesn't use window -- just uses recursive exp decay) and thus the default!
  float         decay;                // #DEF_5 #MIN_0 exponential decay time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to implement a delta function (not very useful)
  float         g_gain;                // #DEF_9 #MIN_0 multiplier for the spike-generated conductances when using alpha function which is normalized by area under the curve -- needed to recalibrate the alpha-function currents relative to rate code net input which is overall larger -- in general making this the same as the decay constant works well, effectively neutralizing the area normalization (results in consistent peak current, but differential integrated current over time as a function of rise and decay)
  int           window;                // #DEF_3 #MIN_0 #MAX_10 spike integration window -- when rise==0, this window is used to smooth out the spike impulses similar to a rise time -- each net contributes over the window in proportion to 1/window -- for rise > 0, this is used for computing the alpha function -- should be long enough to incorporate the bulk of the alpha function, but the longer the window, the greater the computational cost (max of 10 imposed by fixed buffer required in LeabraUnitVars structure)
  float         eq_gain;        // #DEF_8 #MIN_0 gain for computing act_eq relative to actual average: act_eq = eq_gain * (spikes/cycles)
  float         eq_tau;                // #DEF_50 #MIN_0 if non-zero, compute act_eq as a continuous running average instead of explicit spikes / cycles -- this is the time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life)

  float         gg_decay;        // #READ_ONLY #NO_SAVE g_gain/decay
  float         gg_decay_sq;     // #READ_ONLY #NO_SAVE g_gain/decay^2
  float         gg_decay_rise;   // #READ_ONLY #NO_SAVE g_gain/(decay-rise)
  float         oneo_decay;      // #READ_ONLY #NO_SAVE 1.0/decay
  float         oneo_rise;       // #READ_ONLY #NO_SAVE 1.0/rise
  float         eq_dt;           // #READ_ONLY #EXPERT rate = 1 / tau

  INLINE float  ComputeAlpha(float t) {
    if(decay == 0.0f) return (t == 0.0f) ? g_gain : 0.0f; // delta function
    if(rise == 0.0f) return gg_decay * taMath_float::exp_fast(-t * oneo_decay);         // exponential
    if(rise == decay) return t * gg_decay_sq * taMath_float::exp_fast(-t * oneo_decay); // symmetric alpha
    return gg_decay_rise * (taMath_float::exp_fast(-t * oneo_decay) - taMath_float::exp_fast(-t * oneo_rise)); // full alpha
  }

  TA_STD_CODE(SpikeFunSpec);

  UPDATE_AFTER_EDIT
    (if(window <= 0) window = 1;
     if(decay > 0.0f) {
       gg_decay = g_gain / decay;
       gg_decay_sq = g_gain / (decay * decay);
       if(decay != rise)
         gg_decay_rise = g_gain / (decay - rise);

       oneo_decay = 1.0f / decay;
       if(rise > 0.0f)
         oneo_rise = 1.0f / rise;
       else
         oneo_rise = 1.0f;
     }
     if(eq_tau > 0.0f)
       eq_dt = 1.0f / eq_tau;
     else
       eq_dt = 0.0f;
     );
  
private:
  void        Initialize()    { Defaults_init(); }
  void        Defaults_init() {
    g_gain = 9.0f; rise = 0.0f; decay = 5.0f; window = 3; eq_gain = 8.0f; eq_tau = 50.0f;

    gg_decay = g_gain / decay;
    gg_decay_sq = g_gain / (decay * decay);
    gg_decay_rise = g_gain / (decay - rise);

    oneo_decay = 1.0f / decay;
    if(rise > 0.0f)
      oneo_rise = 1.0f / rise;
    else
      oneo_rise = 1.0f;

    if(eq_tau > 0.0f)
      eq_dt = 1.0f / eq_tau;
    else
      eq_dt = 0.0f;
  }
};


SPEC_MEMBER_CLASS(SpikeMiscSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra extra misc spiking parameters 
INHERITED(SPEC_MEMBER_BASE)
public:
  enum ClampType {                // how to generate spikes during hard clamp conditions
    POISSON,                        // generate spikes according to Poisson distribution with probability = clamp_max_p * u->ext
    UNIFORM,                        // generate spikes according to Uniform distribution with probability = clamp_max_p * u->ext
    REGULAR,                        // generate spikes every 1 / (clamp_max_p * u->ext) cycles -- this works the best, at least in smaller networks, due to the lack of additional noise, and the synchrony of the inputs for driving synchrony elsewhere
    CLAMPED,                        // just use the straight clamped activation value -- do not do any further modifications
  };

  bool          ex;             // #DEF_false turn on exponential excitatory current that drives v_m rapidly upward for spiking as it gets past its nominal firing threshold (act.thr) -- nicely captures the Hodgkin Huxley dynamics of Na and K channels -- uses Brette & Gurstner 2005 AdEx formulation -- this mechanism has an unfortunate interaction with the continuous inhibitory currents generated by the standard FF_FB inhibitory function, which cause this mechanism to desensitize and fail to spike
  float         exp_slope;        // #CONDSHOW_ON_ex #DEF_0.02 slope in v_m (2 mV = .02 in normalized units) for extra exponential excitatory current that drives v_m rapidly upward for spiking as it gets past its nominal firing threshold (act.thr) -- nicely captures the Hodgkin Huxley dynamics of Na and K channels -- uses Brette & Gurstner 2005 AdEx formulation -- a value of 0 disables this mechanism
  float         spk_thr;        // #CONDSHOW_ON_ex #DEF_1.2 membrane potential threshold for actually triggering a spike when using the exponential mechanism -- the nominal threshold in act.thr enters into the exponential mechanism, but this value is actually used for spike thresholding 
  float         vm_r;                // #DEF_0;0.15;0.3 #AKA_v_m_r post-spiking membrane potential to reset to, produces refractory effect if lower than vm_init -- 0.30 is apropriate biologically-based value for AdEx (Brette & Gurstner, 2005) parameters
  int           t_r;                // #DEF_3 post-spiking explicit refractory period, in cycles -- prevents v_m updating for this number of cycles post firing
  float         clamp_max_p;        // #DEF_0.12 #MIN_0 #MAX_1 maximum probability of spike rate firing for hard-clamped external inputs -- multiply ext value times this to get overall probability of firing a spike -- distribution is determined by clamp_type
  ClampType     clamp_type;        // how to generate spikes when layer is hard clamped -- in many cases soft clamping may work better

  float         eff_spk_thr;    // #HIDDEN #READ_ONLY effective spiking threshold -- depends on whether exponential mechanism is being used (= act.thr if not ex, else spk_thr)

  TA_STD_CODE(SpikeMiscSpec);

private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    ex = false;
    exp_slope = 0.02f;
    spk_thr = 1.2f;
    clamp_max_p = 0.12f;
    clamp_type = REGULAR;
    vm_r = 0.30f;
    t_r = 3;

    eff_spk_thr = 0.5f;           // ex = off
  }
};


SPEC_MEMBER_CLASS(OptThreshSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra optimization thresholds for faster processing
INHERITED(SPEC_MEMBER_BASE)
public:
  float         send;                   // #DEF_0.1 don't send activation when act <= send -- greatly speeds processing
  float         delta;                  // #DEF_0.005 don't send activation changes until they exceed this threshold: only for when LeabraNetwork::send_delta is on!
  float         xcal_lrn;               // #DEF_0.01 xcal learning threshold -- don't learn when sending unit activation is below this value in both phases -- due to the nature of the learning function being 0 when the sr coproduct is 0, it should not affect learning in any substantial way -- this is applied in the LeabraConSpec, so other learning algorithms that have different properties should ignore it

  TA_STD_CODE(OptThreshSpec);
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init()   { send = .1f; delta = 0.005f; xcal_lrn = 0.01f; }
};


SPEC_MEMBER_CLASS(LeabraInitSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra initial values for key network state variables -- initialized at start of trial with Init_Acts or DecayState
INHERITED(SPEC_MEMBER_BASE)
public:
  float       v_m;        // #DEF_0.4 initial membrane potential -- see e_rev.l for the resting potential (typically .3) -- often works better to have a somewhat elevated initial membrane potential relative to that
  float       act;        // #DEF_0 initial activation value -- typically 0
  float       netin;      // #DEF_0 baseline level of excitatory net input -- netin is initialized to this value, and it is added in as a constant background level of excitatory input -- captures all the other inputs not represented in the model, and intrinsic excitability, etc

  TA_STD_CODE(LeabraInitSpec);
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init()   { act = 0.0f;  v_m = 0.4f;  netin = 0.0f; }
};


SPEC_MEMBER_CLASS(LeabraDtSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra time and rate constants for temporal derivatives in Leabra (Vm, net input)
INHERITED(SPEC_MEMBER_BASE)
public:
  float         integ;           // #DEF_1;0.5 #MIN_0 overall rate constant for numerical integration, for all equations at the unit level -- all time constants are specified in millisecond units, with one cycle = 1 msec -- if you instead want to make one cycle = 2 msec, you can do this globaly by setting this integ value to 2 (etc).  However, stability issues will likely arise if you go too high.  For improved numerical stability, you may even need to reduce this value to 0.5 or possibly even lower (typically however this is not necessary).  MUST also coordinate this with network.time_inc variable to ensure that global network.time reflects simulated time accurately
  float         vm_tau;          // #AKA_vm_time #DEF_2.81:10 [3.3 std for rate code, 2.81 for spiking] #MIN_1 membrane potential and rate-code activation time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) -- reflects the capacitance of the neuron in principle -- biological default for AeEx spiking model C = 281 pF = 2.81 normalized -- for rate-code activation, this also determines how fast to integrate computed activation values over time
  float         net_tau;         // #AKA_net_time #DEF_1.4 #MIN_1 net input time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) -- this is important for damping oscillations -- generally reflects time constants associated with synaptic channels which are not modeled in the most abstract rate code models (set to 1 for detailed spiking models with more realistic synaptic currents)
  int           fast_cyc;        // #AKA_vm_eq_cyc #DEF_0 number of cycles at start of a trial to run units in a fast integration mode -- the rate-code activations have no effective time constant and change immediately to the new computed value (vm_time is ignored) and vm is computed as an equilibirium potential given current inputs: set to 1 to quickly activate soft-clamped input layers (primary use); set to 100 to always use this computation

  float         vm_dt;           // #READ_ONLY #EXPERT rate = 1 / tau
  float         net_dt;          // #READ_ONLY #EXPERT rate = 1 / tau

  TA_STD_CODE(LeabraDtSpec);
  
  UPDATE_AFTER_EDIT(vm_dt = 1.0f / vm_tau;  net_dt = 1.0f / net_tau; );
  
private:
  void        Initialize()      { fast_cyc = 0; Defaults_init(); }
  void        Defaults_init() {
    integ = 1.0f;  vm_tau = 3.3f;  net_tau = 1.4f;

    vm_dt = 1.0f / vm_tau;
    net_dt = 1.0f / net_tau;
  }
};


SPEC_MEMBER_CLASS(LeabraActAvgSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra rate constants for averaging over activations -- only used in XCAL learning rules
INHERITED(SPEC_MEMBER_BASE)
public:
  float         ss_tau;                // #DEF_2;20 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life), for continuously updating the super-short time-scale avg_ss value -- this is provides a pre-integration step before integrating into the avg_s short time scale
  float         s_tau;                // #DEF_2;20 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life), for continuously updating the short time-scale avg_s value from the super-short avg_ss value (cascade mode) -- avg_s represents the plus phase learning signal that reflects the most recent past information
  float         m_tau;                // #DEF_10;100 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life), for continuously updating the medium time-scale avg_m value from the short avg_s value (cascade mode) -- avg_m represents the minus phase learning signal that reflects the expectation representation prior to experiencing the outcome (in addition to the outcome)
  float         m_in_s;                // #DEF_0.1 #MIN_0 #MAX_1 how much of the medium term average activation to include at the short (plus phase) avg_s_eff variable that is actually used in learning -- important to ensure that when unit turns off in plus phase (short time scale), enough medium-phase trace remains so that learning signal doesn't just go all the way to 0, at which point no learning would take place -- typically need faster time constant for updating s such that this trace of the m signal is lost

  float         ss_dt;               // #READ_ONLY #EXPERT rate = 1 / tau
  float         s_dt;                // #READ_ONLY #EXPERT rate = 1 / tau
  float         m_dt;                // #READ_ONLY #EXPERT rate = 1 / tau
  float         s_in_s;              // #READ_ONLY #EXPERT 1-m_in_s

  TA_STD_CODE(LeabraActAvgSpec);
  
  UPDATE_AFTER_EDIT(ss_dt = 1.0f / ss_tau;  s_dt = 1.0f / s_tau;
                    m_dt = 1.0f / m_tau;    s_in_s = 1.0f - m_in_s; );
  
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    ss_tau = 2.0f;  s_tau = 2.0f;  m_tau = 10.0f;  m_in_s = 0.1f;

    ss_dt = 1.0f / ss_tau;
    s_dt = 1.0f / s_tau;
    m_dt = 1.0f / m_tau;
    s_in_s = 1.0f - m_in_s;
  }
};


SPEC_MEMBER_CLASS(LeabraAvgLSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra parameters for computing the long-term floating average value, avg_l, which is used for driving BCM-style hebbian learning in XCAL -- this form of learning increases contrast of weights and generally decreases overall activity of neuron, to prevent "hog" units -- it is computed as a running average of the (gain multiplied) medium-time-scale average activation at the end of the trial
INHERITED(SPEC_MEMBER_BASE)
public:
  float         init;           // #DEF_0.4 #MIN_0 #MAX_1 initial avg_l value at start of training
  float         gain;           // #DEF_1.5;2;2.5 #AKA_max #MIN_0 gain multiplier on activation used in computing the running average avg_l value -- lower values are sometimes better for some models but higher ones are generally not -- it is a good idea to experiment with this parameter a bit
  float         min;            // #DEF_0.2 #MIN_0 miniumum avg_l value -- running average cannot go lower than this value even when it otherwise would due to inactivity -- this value is generally good and typically does not need to be changed
  float         tau;            // #DEF_10 #MIN_1 time constant for updating the running average avg_l -- avg_l moves toward gain*act with this time constant on every trial - longer time constants can also work fine, but the default of 10 allows for quicker reaction to beneficial weight changes
  float         lrn_max;        // #DEF_0.5 #MIN_0 maximum avg_l_lrn value, which is amount of learning driven by avg_l factor -- when avg_l is at its maximum value (i.e., gain, as act does not exceed 1), then avg_l_lrn will be at this maximum value -- by default, strong amounts of this homeostatic Hebbian form of learning can be used when the receiving unit is highly active -- this will then tend to bring down the average activity of units -- the default of 0.5, in combination with the err_mod flag, works well for most models -- use around 0.0004 for a single fixed value (with err_mod flag off)
  float         lrn_min;        // #DEF_0.0001;0.0004 #MIN_0 miniumum avg_l_lrn value (amount of learning driven by avg_l factor) -- if avg_l is at its minimum value, then avg_l_lrn will be at this minimum value -- neurons that are not overly active may not need to increase the contrast of their weights as much -- use around 0.0004 for a single fixed value (with err_mod flag off)
  
  float         dt;             // #READ_ONLY #EXPERT rate = 1 / tau
  float         lrn_fact;       // #READ_ONLY #EXPERT (lrn_max - lrn_min) / (gain - min)
  
  INLINE float  GetLrn(const float avg_l) {
    return lrn_min + lrn_fact * (avg_l - min);
  }
  // get the avg_l_lrn value for given avg_l value

  INLINE void   UpdtAvgL(float& avg_l, const float act) {
    avg_l += dt * (gain * act - avg_l);
    if(avg_l < min) avg_l = min;
  }
  // update long-term average value from given activation, using average-based update

  
  TA_STD_CODE(LeabraAvgLSpec);
  
  UPDATE_AFTER_EDIT(dt = 1.0f / tau;  lrn_fact = (lrn_max - lrn_min) / (gain - min); );
  
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    init = 0.4f;  gain = 2.5f;  min = 0.2f;  tau = 10.0f;  lrn_max = 0.5f;
    lrn_min = 0.0001f;
  
    dt = 1.0f / tau;
    lrn_fact = (lrn_max - lrn_min) / (gain - min);
  }
};


SPEC_MEMBER_CLASS(LeabraAvgL2Spec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra additional parameters for computing the long-term floating average value, avg_l, which is used for driving BCM-style hebbian learning in XCAL -- this form of learning increases contrast of weights and generally decreases overall activity of neuron, to prevent "hog" units
INHERITED(SPEC_MEMBER_BASE)
public:
  bool          err_mod;        // #DEF_true if true, then we multiply avg_l_lrn factor by layer.cos_diff_avg_lrn to make hebbian term roughly proportional to amount of error driven learning signal across layers -- cos_diff_avg computes the running average of the cos diff value between act_m and act_p (no diff is 1, max diff is 0), and cos_diff_avg_lrn = 1 - cos_diff_avg (and 0 for non-HIDDEN layers), so the effective lrn value is high when there are large error signals (differences) in a layer, and low when error signals are low, producing a more consistent mix overall -- typically this error level tends to be stable for a given layer, so this is really just a quick shortcut for setting layer-specific mixes by hand (which the brain can do) -- see LeabraLayerSpec cos_diff.avg_tau rate constant for integrating cos_diff_avg value
  float         err_min;        // #DEF_0.01:0.1 #CONDSHOW_ON_err_mod minimum layer.cos_diff_avg_lrn value (for non-zero cases, i.e., not for target or input layers) -- ensures a minimum amount of self-organizing learning even for layers that have a very small level of error signal
  float         lay_act_thr;    // #DEF_0.01 threshold of layer average activation on this trial, in order to update avg_l values -- setting to 0 disables this check
  
  TA_STD_CODE(LeabraAvgL2Spec);

private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    err_mod = true;  err_min = 0.01f;  lay_act_thr = 0.01f;
  }

};


SPEC_MEMBER_CLASS(LeabraChannels) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra channels used in Leabra
INHERITED(SPEC_MEMBER_BASE)
public:
  float         e;                // excitatory (sodium (Na) channel), synaptic glutamate AMPA activated
  float         l;                // constant leak (potassium, K+) channel 
  float         i;                // inhibitory (chloride, Cl-) channel, synaptic GABA activated

  TA_STD_CODE(LeabraChannels);
  
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init()   {  e = l = i = 0.0f;  }
};


SPEC_MEMBER_CLASS(ActAdaptSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra activation-driven adaptation dynamics -- negative feedback on v_m based on sub- and super-threshold activation -- relatively rapid time-scale and especially relevant for spike-based models -- drives the adapt variable on the unit
INHERITED(SPEC_MEMBER_BASE)
public:
  bool          on;                // apply adaptation?
  float         tau;                // #CONDSHOW_ON_on #DEF_144 adaptation dynamics time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life)
  float         vm_gain;        // #CONDSHOW_ON_on #MIN_0 #DEF_0.04 gain on the membrane potential v_m driving the adapt adaptation variable -- default of 0.04 reflects 4nS biological value converted into normalized units
  float         spike_gain;        // #CONDSHOW_ON_on #DEF_0.00805;0.004;0.002 value to add to the adapt adaptation variable after spiking -- default of 0.00805 is normalized version of .0805 nA in biological values -- weaker levels often work better (e.g., 0.004)
  bool          Ei_dyn;         // #CONDSHOW_ON_on does the inhibitory reversal potential (E_i) update dynamically over time in response to activation of the receiving neuron (backpropagating action potentials), or is it static -- dynamics are important when using adaptation, as this compensates for adaptation and allows active neurons to remain active -- only enabeled when activation is on, because it is bad when adaptation is not on
  float         Ei_gain;        // #CONDSHOW_ON_Ei_dyn&&on #MIN_0 #DEF_0.001 multiplier on postsynaptic cell activation (act_eq), driving increases in E_i reversal potential for Cl- -- this factor determines how strong the e_rev change effect is
  float         Ei_tau;         // #CONDSHOW_ON_Ei_dyn&&on #MIN_1 #DEF_50 decay time constant for decay of inhibitory reversal potential -- active neurons raise their inhibitory reversal potential, giving them an advantage over inactive neurons
  float         dt;                // #READ_ONLY #EXPERT rate = 1 / tau
  float         Ei_dt;          // #READ_ONLY #EXPERT rate = 1 / tau

  INLINE float  Compute_dAdapt(float vm, float e_rev_l, float adapt)
  { return dt * (vm_gain * (vm - e_rev_l) - adapt); }
  // compute the change in adapt given vm, resting reversal potential (leak reversal), and adapt inputs

  TA_STD_CODE(ActAdaptSpec);
  
  UPDATE_AFTER_EDIT(dt = 1.0f / tau;  Ei_dt = 1.0f / Ei_tau; );
  
private:
  void        Initialize()      { on = false; Defaults_init(); }
  void        Defaults_init() {
    tau = 144;  vm_gain = 0.04f;  spike_gain = 0.00805f;  Ei_dyn = true;
    Ei_gain = 0.001f;  Ei_tau = 50.0f;

    dt = 1.0f / tau;
    Ei_dt = 1.0f / Ei_tau;
  }
};


SPEC_MEMBER_CLASS(ShortPlastSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra short-term plasticity specifications -- different algorithms are available to update the syn_tr amount of neurotransmitter available to release, which multiplies computed firing rate or spiking (but not act_nd) to produce a net sending activation efficacy in the act and act_eq variables
INHERITED(SPEC_MEMBER_BASE)
public:
  enum STPAlgorithm {           // which algorithm to use for STP
     CYCLES,                    // uses standard equations summarized in Hennig, 2013 (eq 6) to capture both facilitation and depression dynamics as a function of presynaptic firing -- models interactions between number of vesicles available to release, and probability of release, and a time-varying recovery rate -- rate code uses generated spike var to drive this
     TRIAL_BINARY,              // units continously above thresh for n_trials will depress to 0 synaptic transmitter and individually recover at rec_prob to full strength on trial-by-trial basis (update at start of trial)
  };
 
  bool          on;             // synaptic depression is in effect: multiplies normal activation computed by current activation function in effect
  STPAlgorithm  algorithm;      // #CONDSHOW_ON_on which algorithm to use for computing short term synaptic plasticity, syn_tr (and other related syn_ vars depending on algo)
  float         f_r_ratio;      // #CONDSHOW_ON_on&&algorithm:CYCLES #DEF_0.01:3 ratio of facilitating (t_fac) to depression recovery (t_rec) time constants -- influences overall nature of response balance (ratio = 1 is balanced, > 1 is facilitating, < 1 is depressing).  Wang et al 2006 found: ~2.5 for strongly facilitating PFC neurons (E1), ~0.02 for strongly depressing PFC and visual cortex (E2), and ~1.0 for balanced PFC (E3)
  float         rec_tau;        // #CONDSHOW_ON_on&&algorithm:CYCLES #DEF_100:1000 #MIN_1 [200 std] time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for the constant form of the recovery of number of available vesicles to release at each action potential -- one factor influencing how strong and long-lasting depression is: nr += (1-nr)/rec_tau.  Wang et al 2006 found: ~200ms for strongly depressing in visual cortex and facilitating PFC (E1), 600ms for depressing PFC (E2), and between 200-600 for balanced (E3)
  float         p0;             // #CONDSHOW_ON_on&&algorithm:CYCLES #DEF_0.1:0.4 [0.2 std] baseline probability of release -- lower values around .1 produce more strongly facilitating dynamics, while .4 makes depression dominant -- interacts with f_r_ratio time constants as well.  Tuning advice: keeping all other params at their default values, and focusing on depressing dynamics, this value relative to p0_norm = 0.2 can give different degrees of depression: 0.2 = strong depression, 0.15 = weaker, and 0.1 = very weak depression dynamics
  float         p0_norm;        // #CONDSHOW_ON_on&&algorithm:CYCLES #DEF_0.1:1 [0.2 std] baseline probability of release to use for normalizing the overall net synaptic transmitter release (syn_tr) -- for depressing synapses, this should be = p0, but for facilitating, it make sense to normalize at a somewhat higher level, so that the syn_tr starts out lower and rises to a max -- it maxes out at 1.0 so you don't want to lose dynamic range
  float         kre_tau;        // #CONDSHOW_ON_on&&algorithm:CYCLES #DEF_100 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) on dynamic enhancement of time constant of recovery due to activation -- recovery time constant increases as a function of activity, helping to linearize response (reduce level of depression) at higher frequencies -- supported by multiple sources of biological data (Hennig, 2013)
  float         kre;            // #CONDSHOW_ON_on&&algorithm:CYCLES #DEF_0.002;0 how much the dynamic enhancement of recovery time constant increases for each action potential -- determines how strong this dynamic component is -- set to 0 to turn off this extra adaptation
  float         fac_tau;        // #CONDSHOW_ON_on&&algorithm:CYCLES #READ_ONLY #SHOW auto computed from f_r_ratio and rec_tau: time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for the dynamics of facilitation of release probability: pr += (p0 - pr) / fac_tau. Wang et al 2006 found: 6ms for visual cortex, 10-20ms strongly depressing PFC (E2), ~500ms for strongly facilitating (E1), and between 200-600 for balanced (E3)
  float         fac;            // #CONDSHOW_ON_on&&algorithm:CYCLES #DEF_0.2:0.5 #MIN_0 strength of facilitation effect -- how much each action potential facilitates the probability of release toward a maximum of one: pr += fac (1-pr) -- typically right around 0.3 in Wang et al, 2006

  float         thresh;         // #CONDSHOW_ON_on&&algorithm:TRIAL_BINARY #DEF_0.5 the levels of activation in q3 over which a unit is subject to synaptic NT depletion
  int           n_trials;       // #CONDSHOW_ON_on&&algorithm:TRIAL_BINARY number of continious trials above threshold after which syn_tr transmitter available goes to 0
  float         rec_prob;       // #CONDSHOW_ON_on&&algorithm:TRIAL_BINARY #MIN_0 #MAX_1  depleted units recover independently with recovery probability on a trial by trial basis 

  float         rec_dt;         // #CONDSHOW_ON_on&&algorithm:CYCLES #READ_ONLY #EXPERT rate constant for recovery = 1 / rec_tau
  float         fac_dt;         // #CONDSHOW_ON_on&&algorithm:CYCLES #READ_ONLY #EXPERT rate constant for facilitation =  1 / fac_tau
  float         kre_dt;         // #CONDSHOW_ON_on&&algorithm:CYCLES #READ_ONLY #EXPERT rate constant for recovery enhancement = 1 / kre_tau
  float         oneo_p0_norm;   // #CONDSHOW_ON_on&&algorithm:CYCLES #READ_ONLY #EXPERT 1 / p0_norm
  

  INLINE float dNR(float dt_integ, float syn_kre, float syn_nr, float syn_pr, float spike) {
    return (dt_integ * rec_dt + syn_kre) * (1.0f - syn_nr) - syn_pr * syn_nr * spike;
  }
  
  INLINE float dPR(float dt_integ, float syn_pr, float spike) {
    return dt_integ * fac_dt * (p0 - syn_pr) + fac * (1.0f - syn_pr) * spike;
  }

  INLINE float dKRE(float dt_integ, float syn_kre, float spike) {
    return -dt_integ * kre_dt * syn_kre + kre * (1.0f - syn_kre) * spike;
  }

  INLINE float TR(float syn_nr, float syn_pr) {
    float syn_tr = oneo_p0_norm * (syn_nr * syn_pr); // normalize pr by p0_norm
    if(syn_tr > 1.0f) syn_tr = 1.0f;                  // max out at 1.0
    return syn_tr;
  }
  
  TA_STD_CODE(ShortPlastSpec);
  
  UPDATE_AFTER_EDIT
    ( fac_tau = f_r_ratio * rec_tau;  rec_dt = 1.0f / rec_tau;
      fac_dt = 1.0f / fac_tau;  kre_dt = 1.0f / kre_tau;
      oneo_p0_norm = 1.0f / p0_norm; );
  
private:
  void        Initialize() {
    on = false;  algorithm = CYCLES;  f_r_ratio = 0.02f;  kre = 0.002f;
    Defaults_init();
  }
  
  void        Defaults_init() {
    p0 = 0.2f;
    p0_norm = 0.2f;
    rec_tau = 200.0f;
    fac = 0.3f;
    kre_tau = 100.0f;

    fac_tau = f_r_ratio * rec_tau;
    rec_dt = 1.0f / rec_tau;
    fac_dt = 1.0f / fac_tau;
    kre_dt = 1.0f / kre_tau;
    oneo_p0_norm = 1.0f / p0_norm;
  
    thresh = 0.5;
    n_trials = 1;
    rec_prob = 0.1;
  }    
};

SPEC_MEMBER_CLASS(SynDelaySpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra synaptic delay -- activation sent to other units is delayed by a given number of cycles
INHERITED(SPEC_MEMBER_BASE)
public:
  bool          on;                // is synaptic delay active?
  int           delay;             // #CONDSHOW_ON_on #MIN_0 number of cycles to delay for

  TA_STD_CODE(SynDelaySpec);
private:
  void        Initialize()      { on = false; delay = 4; Defaults_init(); }
  void        Defaults_init()   { }; // note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};


SPEC_MEMBER_CLASS(DeepSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for DeepLeabra deep neocortical layer dynamics, which capture attentional, thalamic auto-encoder, and temporal integration mechanisms 
INHERITED(SPEC_MEMBER_BASE)
public:
  enum DeepRole {          // what role do these neurons play in the deep layer network dynamics -- determines what function the deep_net plays
    SUPER,                 // superficial layer cortical neurons -- generate a deep_raw activation based on thresholds, and receive a deep_net signal from ongoing deep layer neuron activations via SendDeepModConSpec connections, which they then turn into a deep_mod activation value that multiplies activations in this layer
    DEEP,                  // deep layer cortical neurons -- receive deep_net inputs via DeepCtxtConSpec from superficial layer neurons to drive deep_ctxt values that are added into net input along with other inputs to support predictive learning of thalamic relay cell layer activations, which these should project to and receive from -- they should also receive direct top-down projections from other deep layer neurons for top-down attentional signals
    TRC,                   // thalamic relay cell neurons -- receive a SendDeepRawConSpec topographic projection from lower-layer superficial neurons (sending their deep_raw values into deep_net) which is all that is used for net input in the plus phase -- minus phase activation is driven from projections from deep layer predictive learning neurons
  };

  bool       on;         // enable the DeepLeabra mechanisms, including temporal integration via deep_ctxt context connections, thalamic-based auto-encoder driven by deep_raw projections, and attentional modulation by deep_mod
  DeepRole   role;       // #CONDSHOW_ON_on what role do these neurons play in overall deep layer network dynamics -- determines what function the deep_net plays, among other things
  float      raw_thr_rel;    // #CONDSHOW_ON_on #MAX_1 #DEF_0.1;0.2;0.5 #AKA_thr_rel relative threshold on act_raw value (distance between average and maximum act_raw values within layer, e.g., 0 = average, 1 = max) for deep_raw neurons to fire -- neurons below this level have deep_raw = 0 -- above this level, deep_raw = act_raw
  float      raw_thr_abs;    // #CONDSHOW_ON_on #MIN_0 #MAX_1 #DEF_0.1;0.2;0.5 #AKA_thr_abs absolute threshold on act_raw value for deep_raw neurons to fire -- see thr_rel for relative threshold and activation value -- effective threshold is MAX of relative and absolute thresholds
  float      mod_min;     // #CONDSHOW_ON_on&&role:SUPER #MIN_0 #MAX_1 minimum deep_mod value -- provides a non-zero baseline for deep-layer modulation
  float      mod_thr;     // #CONDSHOW_ON_on&&role:SUPER #MIN_0 threshold on deep_mod_net before deep mod is applied -- if not receiving even this amount of overall input from deep_mod sender, then do not use the deep_mod_net to drive deep_mod and deep_lrn values -- only for SUPER units -- based on LAYER level maximum for base LeabraLayerSpec, PVLV classes are based on actual deep_mod_net for each unit
  float      ctxt_prv;    // #CONDSHOW_ON_on&&role:DEEP #MIN_0 #MAX_1 amount of prior deep context to retain when updating deep context net input -- (1-ctxt_prv) will be used for the amount of new context to add -- provides a built-in level of hysteresis / longer-term memory of prior informaiton -- can also achieve this kind of functionality, with more learning dynamics, using a deep ti context projection from the deep layer itself!
  int        tick_updt;   // #CONDSHOW_ON_on&&role:DEEP if this value is >= 0, then only perform normal deep context updating when network.tick is this value -- otherwise use the else_prv value instead of ctxt_prv to determine how much of the previous context to retain (typically set this to a high value near 1 to retain information from the tick_updt time period) -- this simulates a simple form of gating-like behavior in the updating of deep context information
  float      else_prv;    // #CONDSHOW_OFF_tick_updt:-1||!on when tick_updt is being used, this is the amount of prior deep context to retain on all other ticks aside from tick_updt when updating deep context net input -- (1-else_prv) will be used for the amount of new context to add -- ctxt_prv is still used on the time of tick_updt in case that is non-zero

  float      mod_range;  // #READ_ONLY #EXPERT 1 - mod_min -- range for the netinput to modulate value of deep_mod, between min and 1 value
  float      ctxt_new;   // #READ_ONLY #EXPERT 1 - ctxt_prv -- new context amount
  float      else_new;   // #READ_ONLY #EXPERT 1 - else_prv -- new context amount
  
  INLINE bool   IsSuper()
  { return on && role == SUPER; }
  // are we SUPER?
  INLINE bool   IsDeep()
  { return on && role == DEEP; }
  // are we DEEP?
  INLINE bool   IsTRC()
  { return on && role == TRC; }
  // are we thalamic relay cell units?

  INLINE bool   ApplyDeepMod()
  { return on && role == SUPER; }
  // should deep modulation be applied to these units?

  INLINE bool   SendDeepMod()
  { return on && role == DEEP; }
  // should we send our activation into deep_net of other (superficial) units via SendDeepModConSpec connections?

  INLINE bool   ApplyDeepCtxt()
  { return on && role == DEEP; }
  // should we apply deep context netinput?  only for deep guys

  TA_STD_CODE(DeepSpec);
  
  UPDATE_AFTER_EDIT(mod_range = 1.0f - mod_min;  ctxt_new = 1.0f - ctxt_prv;
                    else_new = 1.0f - else_prv; );

private:
  void        Initialize()      {  on = false; role = SUPER; Defaults_init(); }
  void        Defaults_init() {
    raw_thr_rel = 0.1f;  raw_thr_abs = 0.1f;  mod_min = 0.8f;  mod_thr = 0.1f;
    ctxt_prv = 0.0f;  tick_updt = -1;  else_prv = 0.9f;
  
    mod_range = 1.0f - mod_min;
    ctxt_new = 1.0f - ctxt_prv;
    else_new = 1.0f - else_prv;
  }
    
};


SPEC_MEMBER_CLASS(TRCSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for DeepLeabra thalamic relay cells -- engaged only for deep.on and deep.role == TRC
INHERITED(SPEC_MEMBER_BASE)
public:
  bool       p_only_m;          // TRC plus-phase (clamping) for TRC units only occurs if the minus phase max activation for given unit group is above .1
  bool       thal_gate;         // apply thalamic gating to TRC activations -- multiply netin by current thal parameter
  bool       avg_clamp;         // TRC plus-phase netinput is weighted average (see deep_gain) of current plus-phase deep netin and standard netin -- produces a better clamping dynamic
  float      deep_gain;         // #CONDSHOW_ON_avg_clamp how much to weight the deep netin relative to standard netin  (1.0-deep_gain) for avg_clamp
  bool       clip;              // clip the deep netin to clip_max value -- produces more of an OR-like behavior for TRC reps
  float      clip_max;          // #CONDSHOW_ON_clip maximum netin value to clip deep raw netin in trc plus-phase clamping -- prevents strong from dominating weak too much..

  float      std_gain;          // #READ_ONLY #HIDDEN 1-deep_gain

  INLINE float  TRCClampNet(float deep_raw_net, const float net_syn)
  { if(clip)      deep_raw_net = fminf(deep_raw_net, clip_max);
    if(avg_clamp) return deep_gain * deep_raw_net + std_gain * net_syn;
    else          return deep_raw_net; }
  // compute TRC plus-phase clamp netinput
  
  
  TA_STD_CODE(TRCSpec);
  
  UPDATE_AFTER_EDIT(std_gain = 1.0f - deep_gain;);

private:
  void        Initialize()
  { avg_clamp = false; clip = false; clip_max = 0.4f;  Defaults_init(); }

  void        Defaults_init() {
    p_only_m = false;
    thal_gate = false;
    deep_gain = 0.2f;
    std_gain = 1.0f - deep_gain;
  }
};


SPEC_MEMBER_CLASS(DaModSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for effects of da-based modulation: plus-phase = learning effects
INHERITED(SPEC_MEMBER_BASE)
public:
  bool          on;               // whether to add dopamine factor to net input
  float         minus;            // #CONDSHOW_ON_on how much to multiply da_p in the minus phase to add to netinput -- use negative values for NoGo/indirect pathway/D2 type neurons
  float         plus;             // #CONDSHOW_ON_on #AKA_gain how much to multiply da_p in the plus phase to add to netinput -- use negative values for NoGo/indirect pathway/D2 type neurons

  TA_STD_CODE(DaModSpec);
private:
  void        Initialize()
  { on = false;  minus = 0.0f;  plus = 0.01f; Defaults_init(); }
  void        Defaults_init() { };
};


SPEC_MEMBER_CLASS(LeabraNoiseSpec) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for noise type etc
INHERITED(SPEC_MEMBER_BASE)
public:
  enum NoiseType {               // where to add processing noise
    NO_NOISE,                    // no noise added to processing
    VM_NOISE,                    // noise in the value of v_m (membrane potential) -- IMPORTANT: this should NOT be used for rate-code (NXX1) activations, because they do not depend directly on the vm -- this then has no effect
    NETIN_NOISE,                // noise in the net input (g_e) -- this should be used for rate coded activations (NXX1)
    ACT_NOISE,                  // noise in the activations
    NET_MULT_NOISE,             // multiplicative net-input noise: multiply net input by the noise term
  };

  NoiseType     type;            // where to add processing noise
  bool          trial_fixed;     // keep the same noise value over the entire trial -- prevents noise from being washed out and produces a stable effect that can be better used for learning -- this is strongly recommended for most learning situations

  TA_STD_CODE(LeabraNoiseSpec);

private:
  void        Initialize()      { type = NO_NOISE; trial_fixed = true; }
  void        Defaults_init()   { };
};


  
SPEC_CLASS(LeabraUnitSpec) {
  // #STEM_BASE ##CAT_Leabra Leabra unit specifications, point-neuron approximation
INHERITED(SPEC_BASE)
public:
  enum ActFun {
    NOISY_XX1,                        // x over x plus 1 convolved with Gaussian noise (noise is nvar)
    SPIKE,                        // discrete spiking activations (spike when > thr) -- default params produce adaptive exponential (AdEx) model
  };

  enum Quarters {               // #BITS specifies gamma frequency quarters within an alpha-frequency trial on which to do things
    QNULL = 0x00,              // #NO_BIT no quarter (yeah..)
    Q1 = 0x01,                 // first quarter
    Q2 = 0x02,                 // second quarter
    Q3 = 0x04,                 // third quarter -- posterior cortical minus phase
    Q4 = 0x08,                 // fourth quarter -- posterior cortical plus phase
    Q2_Q4 = Q2 | Q4,           // #NO_BIT standard beta frequency option, for pfc
    Q1_Q3 = Q1 | Q3,           // #NO_BIT standard beta frequency option, for bg
    QALL = Q1 | Q2 | Q3 | Q4,  // #NO_BIT all quarters
  };

  ActFun            act_fun;        // #CAT_Activation activation function to use -- typically NOISY_XX1 or SPIKE -- others are for special purposes or testing
  SPEC_MEMBER(LeabraActFunSpec)  act;            // #CAT_Activation activation function parameters -- very important for determining the shape of the selected act_fun
  SPEC_MEMBER(LeabraActMiscSpec) act_misc;       // #CAT_Activation miscellaneous activation parameters
  SPEC_MEMBER(SpikeFunSpec)      spike;          // #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation spiking function specs (only for act_fun = SPIKE)
  SPEC_MEMBER(SpikeMiscSpec)    spike_misc;      // #CAT_Activation misc extra spiking function specs (only for act_fun = SPIKE)
  SPEC_MEMBER(OptThreshSpec)    opt_thresh;      // #CAT_Learning optimization thresholds for speeding up processing when units are basically inactive
  SPEC_MEMBER(MinMaxRange)      clamp_range;     // #CAT_Activation range of clamped activation values (min, max, 0, .95 std), don't clamp to 1 because acts can't reach, so .95 instead
  SPEC_MEMBER(MinMaxRange)      vm_range;        // #CAT_Activation membrane potential range (min, max, 0-2 for normalized)
  SPEC_MEMBER(LeabraInitSpec)   init;            // #CAT_Activation initial starting values for various key neural parameters
  SPEC_MEMBER(LeabraDtSpec)     dt;              // #CAT_Activation time constants (rate of updating): membrane potential (vm) and net input (net)
  SPEC_MEMBER(LeabraActAvgSpec) act_avg;         // #CAT_Learning time constants (rate of updating) for computing activation averages -- used in XCAL learning rules
  SPEC_MEMBER(LeabraAvgLSpec)   avg_l;           // #CAT_Learning parameters for computing the avg_l long-term floating average that drives BCM-style hebbian learning
  SPEC_MEMBER(LeabraAvgL2Spec)  avg_l_2;         // #CAT_Learning additional parameters for computing the avg_l long-term floating average that drives BCM-style hebbian learning
  SPEC_MEMBER(LeabraChannels)   g_bar;           // #CAT_Activation [Defaults: 1, .1, 1] maximal conductances for channels
  SPEC_MEMBER(LeabraChannels)   e_rev;           // #CAT_Activation [Defaults: 1, .3, .25] reversal potentials for each channel
  SPEC_MEMBER(ActAdaptSpec)     adapt;           // #CAT_Activation activation-driven adaptation factor that drives spike rate adaptation dynamics based on both sub- and supra-threshold membrane potentials
  SPEC_MEMBER(ShortPlastSpec)   stp;             // #CAT_Activation short term presynaptic plasticity specs -- can implement full range between facilitating vs. depresssion
  SPEC_MEMBER(SynDelaySpec)     syn_delay;       // #CAT_Activation synaptic delay -- if active, activation sent to other units is delayed by a given amount
  Quarters         deep_raw_qtr;    // #CAT_Learning #AKA_deep_qtr quarter(s) during which deep_raw layer 5 intrinsic bursting activations should be updated -- deep_raw is updated and sent to deep_raw_net during this quarter, and deep_ctxt is updated right after this quarter (wrapping around to the first quarter for the 4th quarter)
  SPEC_MEMBER(DeepSpec)         deep;            // #CAT_Learning specs for DeepLeabra deep neocortical layer dynamics, which capture attentional, thalamic auto-encoder, and temporal integration mechanisms 
  SPEC_MEMBER(TRCSpec)          trc;             // #CAT_Learning #CONDSHOW_ON_deep.on&&deep.role:TRC specs for DeepLeabra TRC thalamic relay cells
  SPEC_MEMBER(DaModSpec)        da_mod;          // #CAT_Learning da modulation of activations (for da-based learning, and other effects)
  SPEC_MEMBER(LeabraNoiseSpec)  noise_type;      // #CAT_Activation random noise in the processing parameters
  RandomSpec       noise;           // #CONDSHOW_OFF_noise_type.type:NO_NOISE #CAT_Activation distribution parameters for random added noise
  
  SPEC_MEMBER(LeabraChannels) e_rev_sub_thr;     // #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN e_rev - act.thr for each item -- used for compute_ithresh
  float          thr_sub_e_rev_i;   // #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.i * (act.thr - e_rev.i) used for compute_ithresh
  float          thr_sub_e_rev_e;   // #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.e * (act.thr - e_rev.e) used for compute_ethresh

  
  ///////////////////////////////////////////////////////////////////////
  //        General Init functions

  INLINE void Init_Netins_impl(UNITVARS* u) {
    u->act_sent = 0.0f;
    u->net_raw = 0.0f;
    u->gi_raw = 0.0f;
    // u->gi_syn = 0.0f;
    // u->net = 0.0f;

    u->deep_raw_net = 0.0f;
    u->deep_mod_net = 0.0f;
    u->deep_raw_sent = 0.0f;
  }
  // #IGNORE just unit -- congroups also have netin vals that need init
    
  INLINE void Init_Acts_impl(UNITVARS* u) {
    Init_Netins_impl(u);

    u->ClearExtFlag(UnitVars_core::COMP_TARG_EXT);
    u->ext = 0.0f;
    u->targ = 0.0f;

    u->act = init.act;
    u->net = init.netin;

    u->act_eq = u->act;
    u->act_nd = u->act_eq;
    u->spike = 0.0f;
    u->act_q0 = 0.0f;
    u->act_q1 = 0.0f;
    u->act_q2 = 0.0f;
    u->act_q3 = 0.0f;
    u->act_q4 = 0.0f;
    u->act_g = 0.0f;
    u->act_m = 0.0f;
    u->act_p = 0.0f;
    u->act_dif = 0.0f;
    u->net_prv_q = 0.0f;        // note: init acts clears this kind of history..
    u->net_prv_trl = 0.0f;

    u->da = 0.0f;
    u->avg_ss = act_misc.avg_init;
    u->avg_s = act_misc.avg_init;
    u->avg_s_eff = u->avg_s;
    u->avg_m = act_misc.avg_init;
    u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
    // not avg_l
    // not act_avg
    u->margin = 0.0f;
    u->act_raw = 0.0f;
    u->deep_raw = 0.0f;
    u->deep_raw_prv = 0.0f;
    u->deep_mod = 1.0f;
    u->deep_lrn = 1.0f;
    u->deep_ctxt = 0.0f;

    u->thal = 0.0f;
    u->thal_gate = 0.0f;
    u->thal_cnt = -1.0f;
    u->gc_i = 0.0f;
    u->I_net = 0.0f;
    u->v_m = init.v_m;
    u->v_m_eq = u->v_m;
    u->adapt = 0.0f;
    u->gi_syn = 0.0f;
    u->gi_self = 0.0f;
    u->gi_ex = 0.0f;
    u->E_i = e_rev.i;
    u->syn_tr = 1.0f;
    u->syn_nr = 1.0f;
    u->syn_pr = stp.p0;
    u->syn_kre = 0.0f;
    u->noise = 0.0f;
    u->da_p = 0.0f;
    u->da_n = 0.0f;
    u->ach = 0.0f;
    u->shunt = 0.0f;
    // u->sev = 0.0f; // longer time-course

    // not the scales
    // init netin gets act_sent, net_raw, etc
    u->spk_t = -1;
  }
  
  INLINE void  Init_Vars_impl(UNITVARS* u) {
    u->ext_flag = UnitVars_core::NO_EXTERNAL;
    u->bias_wt = 0.0f;
    u->bias_dwt = 0.0f;
    u->bias_fwt = 0.0f;
    u->bias_swt = 0.0f;
    u->ext_orig = 0.0f;
    u->avg_l = avg_l.init;
    u->act_avg = 0.15f;
    u->deep_mod_net = 0.0f;
    u->deep_raw_net = 0.0f;
    u->sev = 0.0f;
    u->ach = 0.0f;
    u->misc_1 = 0.0f;
    u->misc_2 = 0.0f;
    u->bias_scale = 0.0f;
    u->act_sent = 0.0f;
    u->net_raw = 0.0f;
    u->gi_raw = 0.0f;
    u->deep_raw_sent = 0.0f;

    // note: also spike_e_st, spike_e_len, etc
    
    Init_Acts_impl(u);
  }
  // #IGNORE

  INLINE void Init_ActAvg_impl(UNITVARS* u) {
    if(act_misc.avg_trace) {
      u->act_avg = 0.0f;
    }
    else {
      u->act_avg = act_misc.avg_init;
    }
    u->avg_l = avg_l.init;
    u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
  }
  // #IGNORE
  
  INLINE void  Init_Weights_impl(UNITVARS* u) {
    u->net_prv_q = 0.0f;
    u->net_prv_trl = 0.0f;
    u->misc_1 = 0.0f;
    u->misc_2 = 0.0f;

    Init_ActAvg_impl(u);
  }
  // #IGNORE only the unit aspect of weights -- actual weights need to be done separately
  
  INLINE void DecayState_impl(UNITVARS* u, float decay) {
    if(decay > 0.0f) {            // no need to reset netin if not decaying at all
      u->act -= decay * (u->act - init.act);
      u->net -= decay * (u->net - init.netin);
      u->act_eq -= decay * (u->act_eq - init.act);
      u->act_nd -= decay * (u->act_nd - init.act);
      u->act_raw -= decay * (u->act_raw - init.act);
      u->gc_i -= decay * u->gc_i;
      u->v_m -= decay * (u->v_m - init.v_m);
      u->v_m_eq -= decay * (u->v_m_eq - init.v_m);
      if(adapt.on) {
        u->adapt -= decay * u->adapt;
      }

      u->gi_syn -= decay * u->gi_syn;
      u->gi_self -= decay * u->gi_self;
      u->gi_ex -= decay * u->gi_ex;
      u->E_i -= decay * (u->E_i - e_rev.i);

      if(stp.on && (stp.algorithm==ShortPlastSpec::CYCLES)) {
        u->syn_tr -= decay * (u->syn_tr - 1.0f);
        u->syn_nr -= decay * (u->syn_nr - 1.0f);
        u->syn_pr -= decay * (u->syn_tr - stp.p0);
        u->syn_kre -= decay * u->syn_kre;
      }
    }
    u->da = 0.0f;
    u->I_net = 0.0f;
    // note: for decay = 1, spike buffers are reset
  }
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
  
  INLINE void ResetSynTR_impl(UNITVARS* u) {
    u->syn_tr = 1.0;
    u->syn_kre = 0.0;
  }
  // #IGNORE


  ///////////////////////////////////////////////////////////////////////
  //        TrialInit -- at start of trial

  // INLINE void Trial_Init_Unit_impl(UNITVARS* uv);
  // #CAT_Activation trial unit-level initialization functions: Trial_STP_TrialBinary_Updt, Trial_Init_PrvVals, Trial_Init_SRAvg, DecayState, NoiseInit

    INLINE void Trial_Init_PrvVals_impl(UNITVARS* u) {
      u->net_prv_trl = u->net; 
      u->act_q0 = u->act_q4;
    }
    // #IGNORE save previous trial values at start of new trial -- allow values at end of trial to be valid for visualization..
    INLINE void Trial_Init_SRAvg_impl(UNITVARS* u, float lay_acts_p_avg,
                                      float lay_cos_diff_avg_lrn, bool zero_avg_l_lrn) {
      if(lay_acts_p_avg >= avg_l_2.lay_act_thr) {
        avg_l.UpdtAvgL(u->avg_l, u->avg_m);
      }
      u->avg_l_lrn = avg_l.GetLrn(u->avg_l);
      if(avg_l_2.err_mod) {
        float eff_err = fmaxf(lay_cos_diff_avg_lrn, avg_l_2.err_min);
        u->avg_l_lrn *= eff_err;
      }
      if(zero_avg_l_lrn) { // (lay->layer_type != Layer::HIDDEN) || deep.IsTRC()
        u->avg_l_lrn = 0.0f;        // no self organizing in non-hidden layers!
      }
    }
    // #IGNORE reset the sender-receiver coproduct average -- call at start of trial
    
    INLINE void Trial_DecayState_impl(UNITVARS* u, float decay_trial) {
      DecayState_impl(u, decay_trial);
      // note: theoretically you could avoid doing Init_Netins if there is no decay between
      // trials, and save some compute time, but the delta-based netin has enough
      // error accumulation that this does NOT work well in practice, so we always do it here
      Init_Netins_impl(u); 
      u->da_p = 0.0f;
      u->da_n = 0.0f;
      u->ach = 0.0f;
      u->thal = 0.0f;
      //  u->sev = 0.0f; // longer time-course
      // reset all the time vars so it isn't ambiguous as these update
      u->act_q1 = u->act_q2 = u->act_q3 = u->act_q4 = 0.0f;
      u->act_g = 0;
      u->act_m = u->act_p = u->act_dif = 0.0f;
    }
    // #IGNORE decay activation states towards initial values: at trial-level boundary
    
    // INLINE void Trial_NoiseInit_impl(UNITVARS* u);
    // #CAT_Activation init trial-level noise -- ONLY called if noise_adapt.trial_fixed is set
    // INLINE void Trial_STP_TrialBinary_Updt_impl(UNITVARS* u);
    // #CAT_Activation calculates short term depression of units by a trial by trial basis


  ///////////////////////////////////////////////////////////////////////
  //        QuarterInit -- at start of new gamma-quarter

  INLINE  bool Quarter_DeepRawNow(int qtr)
  { return deep_raw_qtr & (1 << qtr); }
  // #CAT_Activation test whether to compute deep_raw activations and send deep_raw_net netintput at given quarter (pass net->quarter as arg)
  INLINE  bool Quarter_DeepRawPrevQtr(int qtr)
  { if(qtr == 0) qtr = 3; else qtr--; return deep_raw_qtr & (1 << qtr); }
  // #CAT_Activation test whether the previous quarter was when deep_raw was updated
  INLINE  bool Quarter_DeepRawNextQtr(int qtr)
  { return deep_raw_qtr & (1 << (qtr + 1)); }
  // #CAT_Activation test whether the next quarter will be when deep_raw is updated

  // INLINE void Quarter_Init_Unit_impl(UNITVARS* u);
  // #CAT_Activation quarter unit-level initialization functions: Init_TargFlags, Init_PrvVals, NetinScale, HardClamp
    INLINE void Quarter_Init_TargFlags_impl(UNITVARS* u, bool minus_phase) {
      if(!u->HasExtFlag(UnitVars_core::TARG))
        return;

      if(minus_phase) { // net->phase == LeabraNetwork::MINUS_PHASE) {
        u->ext = 0.0f;
        u->ext_orig = u->ext;
        u->ClearExtFlag(UnitVars_core::EXT);
      }
      else {
        u->ext = u->targ;
        u->ext_orig = u->ext;
        u->SetExtFlag(UnitVars_core::EXT);
      }
    }
    // #IGNORE initialize external input flags based on phase
    
    INLINE void Quarter_Init_PrvVals_impl(UNITVARS* u, int quarter) {
      if(deep.on && (deep_raw_qtr & Q2)) {
        // if using beta rhythm, this happens at that interval
        if(Quarter_DeepRawPrevQtr(quarter)) {
          if(quarter == 0) {
            u->net_prv_q = u->net_prv_trl; // net was cleared
          }
          else {
            u->net_prv_q = u->net;
          }
        }
      }
      else {
        if(quarter == 0) {
          u->net_prv_q = u->net_prv_trl; // net was cleared
        }
        else {
          u->net_prv_q = u->net;
        }
      }
    }
    // #IGNORE update the previous values: e.g., netinput variables (prv_net_q) based on current counters

  INLINE void Compute_DeepStateUpdt_impl(UNITVARS* u, int quarter) {
    if(!deep.on || !Quarter_DeepRawPrevQtr(quarter)) return;
    
    u->deep_raw_prv = u->deep_raw; // keep track of what we sent here, for context learning
  }
  // #IGNORE state update for deep leabra -- typically at start of new alpha trial --

  INLINE void  Init_InputData_impl(UNITVARS* u) {
    u->ClearExtFlag(UnitVars_core::COMP_TARG_EXT);
    u->ext = 0.0f;
    u->targ = 0.0f;
    u->ext_orig = 0.0f;
  }
  // #IGNORE note: must keep sync'd with UnitSpec version
  
  INLINE void ApplyInputData_post_impl(UNITVARS* u) {
    if(!u->HasExtFlag(UnitVars_core::EXT))
      return;
    u->ext_orig = u->ext;
  }
  // #IGNORE post-apply input data -- cache the ext value b/c it might get overwritten in transforms of the input data, as in ScalarValLayerSpec
  
  INLINE void Compute_HardClamp_impl(UNITVARS* u, int cycle, bool clip) {
    float ext_in = u->ext;
    u->act_raw = ext_in;
    if(cycle > 0 && deep.ApplyDeepMod()) {
      ext_in *= u->deep_mod;
    }
    u->net = u->thal = ext_in;
    if(clip) {
      ext_in = clamp_range.Clip(ext_in);
    }
    u->act_eq = u->act_nd = u->act = ext_in;
    if(u->act_eq == 0.0f) {
      u->v_m = e_rev.l;
    }
    else {
      u->v_m = act.thr + ext_in / act.gain;
    }
    u->v_m_eq = u->v_m;
    u->da = u->I_net = 0.0f;
  }
  // #IGNORE force units to external values provided by environment
  
  INLINE void ExtToComp_impl(UNITVARS* u) {
    if(!u->HasExtFlag(UnitVars::EXT))
      return;
    u->ClearExtFlag(UnitVars::EXT);
    u->SetExtFlag(UnitVars::COMP);
    u->targ = u->ext_orig;        // orig is safer
    u->ext = 0.0f;
  }
  // #IGNORE change external inputs to comparisons (remove input)
  
  INLINE void TargExtToComp_impl(UNITVARS* u) {
    if(!u->HasExtFlag(UnitVars::TARG_EXT))
      return;
    if(u->HasExtFlag(UnitVars::EXT))
      u->targ = u->ext_orig;      // orig is safer
    u->ext = 0.0f;
    u->ClearExtFlag(UnitVars::TARG_EXT);
    u->SetExtFlag(UnitVars::COMP);
  }
  // #IGNORE change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 1: Netinput 

  INLINE float Compute_DaModNetin_impl(UNITVARS* u, float& net_syn, bool plus_phase) {
    if(plus_phase) { // net->phase == LeabraNetwork::PLUS_PHASE
      return da_mod.plus * u->da_p * net_syn;
    }
    else {                      // MINUS_PHASE
      return da_mod.minus * u->da_p * net_syn;
    }
  }
  // #IGNORE compute the da_mod netinput extra contribution -- only called if da_mod.on is true so this doesn't need to check that flag -- subtypes can do things to change the function (e.g., D1 vs D2 effects)
  
  INLINE float Compute_EThresh(UNITVARS* u) {
    float gc_l = g_bar.l;
    return ((g_bar.i * u->gc_i * (u->E_i - act.thr)
             + gc_l * e_rev_sub_thr.l - u->adapt) /
            thr_sub_e_rev_e);
  }
  // #IGNORE compute excitatory value that would place unit directly at threshold

  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 2: Inhibition: these are actually called by Compute_Act to integrate
  //            inhibition computed at the layer level

  INLINE void  Compute_SelfInhib_impl(UNITVARS* u, float self_fb, float self_dt) {
    float self = self_fb * u->act;
    u->gi_self += self_dt * (self - u->gi_self);
  }
  // #IGNORE compute self inhibition value

  INLINE void  Compute_ApplyInhib_impl(UNITVARS* u, float ival, float gi_ex,
                                       float lay_adapt_gi) {
    u->gc_i = ival + lay_adapt_gi * (u->gi_syn + u->gi_self) + gi_ex;
    u->gi_ex = gi_ex;
  }
  // #IGNORE apply computed inhibition value to unit inhibitory conductance -- called by Compute_Act functions -- this is not a separate step in computation


  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 3: Activation

  // main function is Compute_Act_Rate or _Spike which calls all the various sub-functions
  // below derived types that send activation directly to special unit variables (e.g.,
  // VTAUnitSpec -> da_p) should do this here, so they can be processed in Compute_Act_Post

  INLINE void Compute_DeepMod_impl(UNITVARS* u, float lay_am_deep_mod_net_max) {
    if(deep.SendDeepMod()) {
      u->deep_lrn = u->deep_mod = u->act;      // record what we send!
      return;
    }
    else if(deep.IsTRC()) {
      u->deep_lrn = u->deep_mod = 1.0f;         // don't do anything interesting
      if(trc.thal_gate) {
        u->net *= u->thal;
      }
      return;
    }
    // must be SUPER units at this point
    else if(lay_am_deep_mod_net_max <= deep.mod_thr) { // not enough yet 
      u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100%
    }
    else {
      u->deep_lrn = u->deep_mod_net / lay_am_deep_mod_net_max;
      u->deep_mod = deep.mod_min + deep.mod_range * u->deep_lrn;
    }
  }
  // #IGNORE compute deep_lrn and deep_mod values
    
  INLINE void Compute_DeepModClampAct_impl(UNITVARS* u) {
    float ext_in = u->ext * u->deep_mod;
    u->net = u->thal = ext_in;
    ext_in = clamp_range.Clip(ext_in);
    u->act_eq = u->act_nd = u->act = ext_in;
  }
  // #IGNORE called for hard_clamped ApplyDeepMod() layers

  INLINE float Compute_ActFun_Rate_fun(float val_sub_thr) {
    return act.NoisyXX1(val_sub_thr);
  }
  // #IGNORE raw activation function: computes an activation value from given value subtracted from its relevant threshold value

  INLINE void Compute_ActFun_Rate_impl(UNITVARS* u, int cycle, int tot_cycle) {
    float new_act;
    if(u->v_m_eq <= act.thr) {
      // note: this is quite important -- if you directly use the gelin
      // the whole time, then units are active right away -- need v_m_eq dynamics to
      // drive subthreshold activation behavior
      new_act = Compute_ActFun_Rate_fun(u->v_m_eq - act.thr);
    }
    else {
      float g_e_thr = Compute_EThresh(u);
      new_act = Compute_ActFun_Rate_fun((u->net * g_bar.e) - g_e_thr);
    }
    if(cycle >= dt.fast_cyc) {
      new_act = u->act_nd + dt.integ * dt.vm_dt * (new_act - u->act_nd); // time integral with dt.vm_dt  -- use nd to avoid synd problems
    }

    u->da = new_act - u->act_nd;
    if((noise_type.type == LeabraNoiseSpec::ACT_NOISE) && (noise.type != Random::NONE) && (cycle >= 0)) {
      new_act += u->noise;
    }

    u->act_raw = new_act;
    if(deep.ApplyDeepMod()) { // apply attention directly to act
      new_act *= u->deep_mod;
    }
    u->act_nd = act_range.Clip(new_act);

    if(stp.on) {                   // short term plasticity, depression
      u->act = u->act_nd * u->syn_tr; // overall probability of transmission
    }
    else {
      u->act = u->act_nd;
    }
    u->act_eq = u->act;           // for rate code, eq == act

    // we now use the exact same vm-based dynamics as in SPIKE model, for full consistency!
    // note that v_m_eq is NOT reset here:
    if(u->v_m > spike_misc.eff_spk_thr) {
      u->spike = 1.0f;
      u->v_m = spike_misc.vm_r;
      u->spk_t = tot_cycle;
      u->I_net = 0.0f;
    }
    else {
      u->spike = 0.0f;
    }
  }
  // #IGNORE Rate coded activation

  
  INLINE void Compute_RateCodeSpike_impl(UNITVARS* u, int tot_cycle, const float time_inc) {
    // use act_nd here so it isn't a self-fulfilling function!
    // note: this is only used for clamped layers -- dynamic layers use SPIKE-based mechanisms
    u->spike = 0.0f;
    if(u->act_nd <= opt_thresh.send) { // no spiking below threshold..
      u->spk_t = -1;
      return;
    }
    if(u->spk_t < 0) {            // start counting from first time above threshold
      u->spk_t = tot_cycle;
      return;
    }
    int interval = act_misc.ActToInterval(time_inc, dt.integ, u->act_nd);
    if((tot_cycle - u->spk_t) >= interval) {
      u->spike = 1.0f;
      u->v_m = spike_misc.vm_r;   // reset vm when we spike -- now we can use it just like spiking!
      u->spk_t = tot_cycle;
      u->I_net = 0.0f;
    }
  }    
  // #IGNORE compute spiking activation (u->spike) based off of rate-code activation value

  INLINE void Compute_ActFun_Spike_impl(UNITVARS* u, int tot_cycle, int cycle) {
    if(u->v_m > spike_misc.eff_spk_thr) {
      u->act = 1.0f;
      u->spike = 1.0f;
      u->v_m = spike_misc.vm_r;
      u->spk_t = tot_cycle;
      u->I_net = 0.0f;
    }
    else {
      u->act = 0.0f;
      u->spike = 0.0f;
    }

    float act_nd = u->act_nd / spike.eq_gain;
    if(spike.eq_dt > 0.0f) {
      act_nd += dt.integ * spike.eq_dt * (u->act - act_nd);
    }
    else {                        // increment by phase
      if(cycle > 0)
        act_nd *= (float)cycle;
      act_nd = (act_nd + u->act) / (float)(cycle+1);
    }
    act_nd = act_range.Clip(spike.eq_gain * act_nd);
    u->da = act_nd - u->act_nd;   // da is on equilibrium activation
    u->act_nd = act_nd;

    if(stp.on) {
      u->act *= u->syn_tr;
      u->act_eq = u->syn_tr * u->act_nd; // act_eq is depressed rate code
    }
    else {
      u->act_eq = u->act_nd;      // eq = nd
    }
  }
  // #IGNORE compute the activation from membrane potential -- discrete spiking
  
  INLINE void Compute_Vm_impl(UNITVARS* u, int tot_cycle, int cycle) {
    bool updt_spk_vm = true;
    if(spike_misc.t_r > 0 && u->spk_t > 0) {
      int spkdel = tot_cycle - u->spk_t;
      if(spkdel >= 0 && spkdel <= spike_misc.t_r)
        updt_spk_vm = false;    // don't update the spiking vm during refract
    }

    if(cycle < dt.fast_cyc) {
      // directly go to equilibrium value
      float new_v_m = Compute_EqVm(u);
      float I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
      u->v_m += I_net;
      u->v_m_eq = u->v_m;
      u->I_net = I_net;
    }
    else {
      float net_eff = u->net * g_bar.e;
      float E_i;
      if(adapt.on && adapt.Ei_dyn) {
        // update the E_i reversal potential as function of inhibitory current
        // key to assume that this is driven by backpropagating AP's
        E_i = u->E_i;
        u->E_i += adapt.Ei_gain * u->act_eq + adapt.Ei_dt * (e_rev.i - u->E_i);
      }
      else {
        E_i = e_rev.i;
      }

      float gc_l = g_bar.l;
      float gc_i = u->gc_i * g_bar.i;

      if(updt_spk_vm) {
        // first compute v_m, using midpoint method:
        float v_m_eff = u->v_m;
        // midpoint method: take a half-step:
        float I_net_1 =
          (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff)) +
          (gc_i * (E_i - v_m_eff)) - u->adapt;
        v_m_eff += .5f * dt.integ * dt.vm_dt * I_net_1; // go half way
        float I_net = (net_eff * (e_rev.e - v_m_eff)) + (gc_l * (e_rev.l - v_m_eff))
          + (gc_i * (E_i - v_m_eff)) - u->adapt;
        // add spike current if relevant
        if(spike_misc.ex) {
          I_net += gc_l * spike_misc.exp_slope *
            expf((v_m_eff - act.thr) / spike_misc.exp_slope); // todo: exp_fast
        }
        u->v_m += dt.integ * dt.vm_dt * I_net;
        u->I_net = I_net;
      }

      // always compute v_m_eq with simple integration -- used for rate code subthreshold
      float I_net_r = (net_eff * (e_rev.e - u->v_m_eq)) 
        + (gc_l * (e_rev.l - u->v_m_eq)) +  (gc_i * (E_i - u->v_m_eq));
      u->v_m_eq += dt.integ * dt.vm_dt * (I_net_r - u->adapt);
    }

    if((noise_type.type == LeabraNoiseSpec::VM_NOISE) && (noise.type != Random::NONE) && (cycle >= 0)) {
      float ns = u->noise;
      u->v_m += ns;
      u->v_m_eq += ns;
    }

    if(u->v_m < vm_range.min) u->v_m = vm_range.min;
    if(u->v_m > vm_range.max) u->v_m = vm_range.max;
    if(u->v_m_eq < vm_range.min) u->v_m_eq = vm_range.min;
    if(u->v_m_eq > vm_range.max) u->v_m_eq = vm_range.max;
  }
  // #IGNORE Act Step 2: compute the membrane potential from input conductances
  
  INLINE float Compute_EqVm(UNITVARS* u) {
    float gc_l = g_bar.l;
    float new_v_m = (((u->net * e_rev.e) + (gc_l * e_rev.l)
                      + (g_bar.i * u->gc_i * u->E_i) - u->adapt) /
                     (u->net + gc_l + g_bar.i * u->gc_i));
    return new_v_m;
  }
  // #IGNORE compute the equilibrium (asymptotic) membrante potential from input conductances (assuming they remain fixed as they are)

  INLINE void Compute_SelfReg_Cycle_impl(UNITVARS* u) {
    Compute_ActAdapt_Cycle_impl(u);
    Compute_ShortPlast_Cycle_impl(u);
  }
  // #IGNORE Act Step 3: compute self-regulatory dynamics at the cycle time scale -- adapt, etc
  
  INLINE void Compute_ActAdapt_Cycle_impl(UNITVARS* u) {
    if(!adapt.on) {
      u->adapt = 0.0f;
    }
    else {
      float dad = dt.integ * (adapt.Compute_dAdapt(u->v_m, e_rev.l, u->adapt) +
                              u->spike * adapt.spike_gain);
      u->adapt += dad;
    }
  }
  // #IGNORE compute the activation-based adaptation value based on spiking and membrane potential
    
  INLINE void Compute_ShortPlast_Cycle_impl(UNITVARS* u) {
    if(!stp.on) {
      if(u->syn_tr != 1.0f) {
        u->syn_tr = 1.0f;
        u->syn_nr = 1.0f;
        u->syn_pr = stp.p0;
        u->syn_kre = 0.0f;
      }
    }
    else {
      if (stp.on&&(stp.algorithm==ShortPlastSpec::CYCLES)) {
        float dnr = stp.dNR(dt.integ, u->syn_kre, u->syn_nr, u->syn_pr, u->spike);
        float dpr = stp.dPR(dt.integ, u->syn_pr, u->spike);
        float dkre = stp.dKRE(dt.integ, u->syn_kre, u->spike);
        u->syn_nr += dnr;
        u->syn_pr += dpr;
        u->syn_kre += dkre;
        if(u->spike > 0.0f) {                                     // only update at spike
          u->syn_tr = stp.TR(u->syn_nr, u->syn_pr);
        }
      }
    }
  }
  // #IGNORE compute whole-neuron (presynaptic) short-term plasticity at the cycle level, using the stp parameters -- updates the syn_* unit variables


  ///////////////////////////////////////////////////////////////////////
  //        Post Activation Step

  // INLINE void Compute_Act_Post_impl(UNITVARS* u);
  // #CAT_Activation post-processing step after activations are computed -- calls  Compute_SRAvg and Compute_Margin by default -- this is also when any modulatory signals should be sent -- NEVER send any such signals during Compute_Act as they might be consumed by other layers during that time
  INLINE void Compute_SRAvg_impl(UNITVARS* u) {
    float ru_act;
    if(act_misc.avg_nd) {
      ru_act = u->act_nd;
    }
    else {
      ru_act = u->act_eq;
    }

    u->avg_ss += dt.integ * act_avg.ss_dt * (ru_act - u->avg_ss);
    u->avg_s += dt.integ * act_avg.s_dt * (u->avg_ss - u->avg_s);
    u->avg_m += dt.integ * act_avg.m_dt * (u->avg_s - u->avg_m);

    u->avg_s_eff = act_avg.s_in_s * u->avg_s + act_avg.m_in_s * u->avg_m;
  }
  // #IGNORE compute sending-receiving running activation averages (avg_ss, avg_s, avg_m) -- only for this unit (SR name is a hold-over from connection-level averaging that is no longer used) -- unit level only, used for XCAL -- called by Compute_Act_Post
  
  INLINE void Compute_Margin_impl(UNITVARS* u, float margin_low_thr, float margin_med_thr,
                                  float margin_hi_thr) {
    const float v_m_eq = u->v_m_eq;
    if(v_m_eq >= margin_low_thr) {
      if(v_m_eq > margin_hi_thr) {
        u->margin = 2.0f;
      }
      else if(v_m_eq > margin_med_thr) {
        u->margin = 1.0;
      }
      else {
        u->margin = -1.0f;
      }
    }
    else {
      u->margin = -2.0f;
    }
  }
  // #IGNORE compute margin status of acivation relative to layer thresholds

  ///////////////////////////////////////////////////////////////////////
  //        Cycle Stats


  ///////////////////////////////////////////////////////////////////////
  //        Deep Leabra Computations -- after superifical acts updated

  INLINE void ClearDeepActs_impl(UNITVARS* u) {
    u->deep_raw = 0.0f;
    u->deep_raw_prv = 0.0f;
    u->deep_ctxt = 0.0f;
    u->deep_mod = 1.0f;
    u->deep_lrn = 1.0f;
    u->deep_raw_net = 0.0f;
    u->deep_mod_net = 0.0f;
    u->deep_raw_sent = 0.0f;
  }
  // #CAT_Deep clear all the deep lamina variables -- can be useful to do at discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //        Quarter Final

  INLINE float GetRecAct(UNITVARS* u) {
    if(act_misc.rec_nd) return u->act_nd;
    return u->act_eq;
  }
  // #IGNORE get activation to record
  
  // INLINE void Quarter_Final_impl(UNITVARS* u);
  // // #CAT_Activation record state variables after each gamma-frequency quarter-trial of processing
  INLINE void Quarter_Final_RecVals_impl(UNITVARS* u, int quarter) {
    float use_act = GetRecAct(u);
    switch(quarter) {        // this has not advanced yet -- still 0-3
    case 0: {
      u->act_q1 = use_act;
      break;
    }
    case 1: {
      u->act_q2 = use_act;
      break;
    }
    case 2: {
      u->act_q3 = use_act;
      u->act_m = use_act;
      break;
    }
    case 3: {
      u->act_q4 = use_act;
      u->act_p = use_act;
      if(act_misc.dif_avg) {
        u->act_dif = u->avg_s_eff - u->avg_m;
      }
      else {
        u->act_dif = u->act_p - u->act_m;
      }
      Compute_ActTimeAvg_impl(u);
      break;
    }
    }
  }
  // #IGNORE record state variables after each gamma-frequency quarter-trial of processing

  INLINE void Compute_ActTimeAvg_impl(UNITVARS* u) {
    if(act_misc.avg_trace) {
      u->act_avg = act_misc.lambda * u->act_avg + u->act_q0; // using prior act to be compatible with std td learning mechanism
    }
    else {
      if(act_misc.avg_dt <= 0.0f) return;
      u->act_avg += act_misc.avg_dt * (u->act_nd - u->act_avg);
    }
  }
  // #IGNORE compute time-averaged activation of unit (using act.avg_dt time constant), typically done at end of settling in Quarter_Final function

  ///////////////////////////////////////////////////////////////////////
  //        Stats

  INLINE float Compute_SSE_impl(UNITVARS* u, bool& has_targ) {
    float sse = 0.0f;
    has_targ = false;
    if(u->HasExtFlag(UnitVars::COMP_TARG)) {
      has_targ = true;
      float uerr = u->targ - u->act_m;
      if(fabsf(uerr) >= sse_tol)
        sse = uerr * uerr;
    }
    return sse;
  }
  // #IGNORE
  
  INLINE bool  Compute_PRerr_impl(UNITVARS* u, float& true_pos, float& false_pos,
                           float& false_neg, float& true_neg) {
    true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f; true_neg = 0.0f;
    bool has_targ = false;
    if(u->HasExtFlag(UnitVars::COMP_TARG)) {
      has_targ = true;
      if(u->targ > u->act_m) {
        true_pos = u->act_m;
        true_neg = 1.0 - u->targ;
        false_neg = u->targ - u->act_m;
      }
      else {
        true_pos = u->targ;
        false_pos = u->act_m - u->targ;
        true_neg = 1.0 - u->act_m;
      }
    }
    return has_targ;
  }
  // #IGNORE
  
  INLINE float  Compute_NormErr_impl(UNITVARS* u, bool& targ_active, bool on_errs, bool off_errs) {
    targ_active = false;
    if(!u->HasExtFlag(UnitVars::COMP_TARG)) return 0.0f;

    targ_active = (u->targ > 0.5f);   // use this for counting expected activity level
    
    if(on_errs) {
      if(u->act_m > 0.5f && u->targ < 0.5f) return 1.0f;
    }
    if(off_errs) {
      if(u->act_m < 0.5f && u->targ > 0.5f) return 1.0f;
    }
    return 0.0f;
  }
  // IGNORE compute normalized binary error (0-1 as function of bits off of act_m vs target) according to settings on the network (returns a 1 or 0) -- if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0

  TA_STD_CODE_SPEC(LeabraUnitSpec);

  UPDATE_AFTER_EDIT
    ( e_rev_sub_thr.e = e_rev.e - act.thr;
      e_rev_sub_thr.l = e_rev.l - act.thr;
      e_rev_sub_thr.i = e_rev.i - act.thr;
      //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
      thr_sub_e_rev_i = (act.thr - e_rev.i);
      thr_sub_e_rev_e = (act.thr - e_rev.e); );

private:
  void        Initialize() {
    act_fun = NOISY_XX1;
    deep_raw_qtr = Q4;
    Defaults_init();
  }

  void        Defaults_init() {
    sse_tol = .5f;
    clamp_range.min = .0f;
    clamp_range.max = .95f;
    // clamp_range.UpdateAfterEdit_NoGui();

    vm_range.max = 2.0f;
    vm_range.min = 0.0f;
    // vm_range.UpdateAfterEdit_NoGui();

    g_bar.e = 1.0f;
    g_bar.l = 0.2f;
    g_bar.i = 1.0f;
    e_rev.e = 1.0f;
    e_rev.l = 0.3f;
    e_rev.i = 0.25f;

    e_rev_sub_thr.e = e_rev.e - act.thr;
    e_rev_sub_thr.l = e_rev.l - act.thr;
    e_rev_sub_thr.i = e_rev.i - act.thr;
    //  thr_sub_e_rev_i = g_bar.i * (act.thr - e_rev.i);
    thr_sub_e_rev_i = (act.thr - e_rev.i);
    thr_sub_e_rev_e = (act.thr - e_rev.e);
  }

};

#endif // LeabraUnitSpec_core_h
