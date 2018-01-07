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

// this contains all full classes that appear as members of _core specs
// it must be included directly in LeabraUnitSpec.h, _cpp.h, _cuda.h
// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda

#ifdef __MAKETA__
#pragma maketa_file_is_target LeabraUnitSpec
#endif


class STATE_CLASS(LeabraActFunSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra activation function specifications, using the gelin (g_e linear) activation function by default
INHERITED(SpecMemberBase)
public:
  float         thr;                // #DEF_0.5 threshold value Theta (Q) for firing output activation (.5 is more accurate value based on AdEx biological parameters and normalization -- see BioParams button)
  float         gain;                // #DEF_80;100;40;20 #MIN_0 gain (gamma) of the rate-coded activation functions -- 100 is default, 80 works better for larger models, and 20 is closer to the actual spiking behavior of the AdEx model -- use lower values for more graded signals, generally in lower input/sensory layers of the network
  float         nvar;                // #DEF_0.005;0.01 #MIN_0 variance of the Gaussian noise kernel for convolving with XX1 in NOISY_XX1 and NOISY_LINEAR -- determines the level of curvature of the activation function near the threshold -- increase for more graded responding there -- note that this is not actual stochastic noise, just constant convolved gaussian smoothness to the activation function
  float         avg_correct;         // correction factor (multiplier) for average activation level in this layer -- e.g., if using adaptation or stp, may be lower than usual -- taken into account in netinput scaling out of this layer
  float         vm_act_thr;          // #DEF_0.01 threshold on activation below which the direct vm - act.thr is used -- this should be low -- once it gets active should use net - g_e_thr ge-linear dynamics (gelin)
  
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


  INLINE float  XX1GainCor_gain(float x, float gain_arg) {
    float gain_cor_fact = (gain_cor_range - (x / nvar)) / gain_cor_range;
    if(gain_cor_fact < 0.0f) {
      return XX1(gain_arg * x);
    }
    float new_gain = gain_arg * (1.0f - gain_cor * gain_cor_fact);
    return XX1(new_gain * x);
  }
  // x/(x+1) with gain correction within gain_cor_range to compensate for convolution effects
  
  INLINE float  NoisyXX1_gain(float x, float gain_arg) {
    if(x < interp_range) {
      float sig_mult_eff_arg = sig_mult * powf(gain_arg * nvar, sig_mult_pow);
      float sig_val_at_0_arg = 0.5f * sig_mult_eff_arg;
    
      if(x < 0.0f) {        // sigmoidal for < 0
        return sig_mult_eff_arg / (1.0f + expf(-(x * sig_gain_nvar)));
      }
      else { // else x < interp_range
        float interp = 1.0f - ((interp_range - x) / interp_range);
        return sig_val_at_0_arg + interp * interp_val;
      }
    }
    else {
      return XX1GainCor_gain(x, gain_arg);
    }
  }
  // noisy x/(x+1) function -- directly computes close approximation to x/(x+1) convolved with a gaussian noise function with variance nvar -- no need for a lookup table -- very reasonable approximation for standard range of parameters (nvar = .01 or less -- higher values of nvar are less accurate with large gains, but ok for lower gains)

  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraActFunSpec);
  
  STATE_UAE( UpdateParams(); );
  
private:
  INLINE void UpdateParams() {  // too many to duplicate..
    sig_gain_nvar = sig_gain / nvar;
    sig_mult_eff = sig_mult * powf(gain * nvar, sig_mult_pow);
    sig_val_at_0 = 0.5f * sig_mult_eff;
    interp_val = XX1GainCor(interp_range) - sig_val_at_0;
  }

  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    thr = 0.5f;  gain = 100.0f;  nvar = 0.005f;  vm_act_thr = 0.01f; avg_correct = 1.0f;
    sig_mult = 0.33f; sig_mult_pow = 0.8f; sig_gain = 3.0f;
    interp_range = 0.01f; gain_cor_range = 10.0f; gain_cor = 0.1f;
    UpdateParams();
  }
};


class STATE_CLASS(LeabraActMiscSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra miscellaneous activation computation parameters and specs
INHERITED(SpecMemberBase)
public:
  bool          rec_nd;         // record the act_nd non-depressed activation variable (instead of act_eq) for the act_q* quarter-trial and phase (act_m, act_p) activation state variables -- these are used primarily for statistics, or possibly for specialized learning mechanisms
  bool          avg_nd;         // use the act_nd non-depressed activation variable (instead of act_eq) for the time-average activation values (avg_ss, avg_s, avg_m, avg_l) used in the XCAL learning mechanism -- this is appropriate for action-potential driven learning dynamics, as compared to synaptic efficacy, when short term plasticity is present
  bool          dif_avg;        // compute act_dif as avg_s_eff - avg_m (difference of average values that actually drive learning) -- otherwise it is act_p - act_m (difference of final activation states in plus phase minus minus phase -- the typical error signal)
  float         net_gain;       // #DEF_1 #MIN_0 multiplier on total synaptic net input -- this multiplies the net_raw, but AFTER the net_raw variable is saved (upon which the netin_raw statistics are computed)

  bool          avg_trace;      // #DEF_false set act_avg unit variable to the exponentially decaying trace of activation -- used for TD (temporal differences) reinforcement learning for example -- lambda parameter determines how much of the prior trace carries over into the new trace 
  float         lambda;         // #CONDSHOW_ON_avg_trace determines how much of the prior trace carries over into the new trace (act_avg = lambda * act_avg + new_act)
  float         avg_tau;        // #CONDSHOW_OFF_avg_trace #DEF_200 #MIN_1 for integrating activation average (act_avg), time constant in trials (roughly, how long it takes for value to change significantly) -- used mostly for visualization and tracking "hog" units
  float         avg_init;        // #DEF_0.15 #MIN_0 initial activation average value -- used for act_avg, avg_s, avg_m, avg_l
  float         avg_dt;          // #READ_ONLY #EXPERT rate = 1 / tau

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraActMiscSpec);

  STATE_UAE(avg_dt = 1.0f / avg_tau;);

private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init()   {
    rec_nd = true; avg_nd = true; dif_avg = false; net_gain = 1.0f;
    avg_trace = false; lambda = 0.0f; avg_tau = 200.0f;
    avg_init = 0.15f;

    avg_dt = 1.0f / avg_tau;
  }
};


class STATE_CLASS(SpikeFunSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra spiking activation function specs -- conductance is computed postsynaptically using an alpha function based on spike pulses sent presynaptically -- for clamped layers, spiking probability is proportional to external input controlled by the clamp_type and clamp_max_p values -- soft clamping may still be a better option though
INHERITED(SpecMemberBase)
public:
  float         rise;            // #DEF_0 #MIN_0 exponential rise time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to only include decay time (1/decay e^(-t/decay)), which is highly optimized (doesn't use window -- just uses recursive exp decay) and thus the default!
  float         decay;           // #DEF_5 #MIN_0 exponential decay time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to implement a delta function (not very useful)
  float         g_gain;          // #DEF_9 #MIN_0 multiplier for the spike-generated conductances when using alpha function which is normalized by area under the curve -- needed to recalibrate the alpha-function currents relative to rate code net input which is overall larger -- in general making this the same as the decay constant works well, effectively neutralizing the area normalization (results in consistent peak current, but differential integrated current over time as a function of rise and decay)
  int           window;          // #DEF_3 #MIN_0 #MAX_10 spike integration window -- when rise==0, this window is used to smooth out the spike impulses similar to a rise time -- each net contributes over the window in proportion to 1/window -- for rise > 0, this is used for computing the alpha function -- should be long enough to incorporate the bulk of the alpha function, but the longer the window, the greater the computational cost (max of 10 imposed by fixed buffer required in LeabraUnitState_cpp structure)
  float         act_max_hz;      // #DEF_180 #MIN_1 for translating spiking interval (rate) into rate-code activation equivalent (and vice-versa, for clamped layers), what is the maximum firing rate associated with a maximum activation value (max act is typically 1.0 -- depends on act_range)
  float         int_tau;         // #DEF_5 #MIN_1 time constant for integrating the spiking interval in estimating spiking rate

  float         gg_decay;        // #READ_ONLY #NO_SAVE g_gain/decay
  float         gg_decay_sq;     // #READ_ONLY #NO_SAVE g_gain/decay^2
  float         gg_decay_rise;   // #READ_ONLY #NO_SAVE g_gain/(decay-rise)
  float         oneo_decay;      // #READ_ONLY #NO_SAVE 1.0/decay
  float         oneo_rise;       // #READ_ONLY #NO_SAVE 1.0/rise
  float         int_dt;          // #READ_ONLY #EXPERT rate = 1 / tau

  INLINE float  ComputeAlpha(float t) {
    if(decay == 0.0f) return (t == 0.0f) ? g_gain : 0.0f; // delta function
    // todo: replace with exp_fast -- and benchmark!
    // if(rise == 0.0f) return gg_decay * expf(-t * oneo_decay);         // exponential
    // if(rise == decay) return t * gg_decay_sq * expf(-t * oneo_decay); // symmetric alpha
    // return gg_decay_rise * (expf(-t * oneo_decay) - expf(-t * oneo_rise)); // full alpha
    if(rise == 0.0f) return gg_decay * STATE_CLASS(taMath_float)::exp_fast(-t * oneo_decay);         // exponential
    if(rise == decay) return t * gg_decay_sq * STATE_CLASS(taMath_float)::exp_fast(-t * oneo_decay); // symmetric alpha
    return gg_decay_rise * (STATE_CLASS(taMath_float)::exp_fast(-t * oneo_decay) - STATE_CLASS(taMath_float)::exp_fast(-t * oneo_rise)); // full alpha
  }

  INLINE int    ActToInterval(const float time_inc, const float integ, const float act)
  { return (int) (1.0f / (time_inc * integ * act * act_max_hz)); }
  // #CAT_Activation compute spiking interval based on network time_inc, dt.integ, and unit act -- note that network time_inc is usually .001 = 1 msec per cycle -- this depends on that being accurately set

  INLINE float  ActFromInterval(float spike_isi, const float time_inc, const float integ) {
    if(spike_isi == 0.0f) {
      return 0.0f;              // rate is 0
    }
    float max_hz_int = 1.0f / (time_inc * integ * act_max_hz); // interval at max hz..
    return max_hz_int / spike_isi; // normalized
  }
  // #CAT_Activation compute rate-code activation from estimated spiking interval

  INLINE void   UpdateSpikeInterval(float& spike_isi, float cur_int) {
    if(spike_isi == 0.0f) {
      spike_isi = cur_int;      // use it
    }
    else if(cur_int < 0.8f * spike_isi) {
      spike_isi = cur_int;      // if significantly less than we take that
    }
    else {                                         // integrate on slower
      spike_isi += int_dt * (cur_int - spike_isi); // running avg updt
    }
  }
  // #CAT_Activation update running-average spike interval estimate

  INLINE void   UpdateRates() {
    if(window <= 0) window = 1;
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
    int_dt = 1.0f / int_tau;
  }
  // #IGNORE update derive rates
  
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(SpikeFunSpec);

  STATE_UAE( UpdateRates(); );
  
private:
  void        Initialize()    { Defaults_init(); }
  void        Defaults_init() {
    g_gain = 9.0f; rise = 0.0f; decay = 5.0f; window = 3;
    act_max_hz = 180.0f;  int_tau = 5.0f;
    UpdateRates();
  }
};


class STATE_CLASS(SpikeMiscSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra extra misc spiking parameters 
INHERITED(SpecMemberBase)
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

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(SpikeMiscSpec);

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


class STATE_CLASS(OptThreshSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra optimization thresholds for faster processing
INHERITED(SpecMemberBase)
public:
  float         send;                   // #DEF_0.1 don't send activation when act <= send -- greatly speeds processing
  float         delta;                  // #DEF_0.005 don't send activation changes until they exceed this threshold: only for when LeabraNetwork::send_delta is on!

  STATE_DECO_KEY("UnitSpec");
    STATE_TA_STD_CODE_SPEC(OptThreshSpec);
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init()   { send = .1f; delta = 0.005f; }
};


class STATE_CLASS(LeabraInitSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra initial values for key network state variables -- initialized at start of trial with Init_Acts or DecayState
INHERITED(SpecMemberBase)
public:
  float       v_m;        // #DEF_0.4 initial membrane potential -- see e_rev.l for the resting potential (typically .3) -- often works better to have a somewhat elevated initial membrane potential relative to that
  float       act;        // #DEF_0 initial activation value -- typically 0
  float       netin;      // #DEF_0 baseline level of excitatory net input -- netin is initialized to this value, and it is added in as a constant background level of excitatory input -- captures all the other inputs not represented in the model, and intrinsic excitability, etc

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraInitSpec);
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init()   { act = 0.0f;  v_m = 0.4f;  netin = 0.0f; }
};


class STATE_CLASS(LeabraDtSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra time and rate constants for temporal derivatives in Leabra (Vm, net input)
INHERITED(SpecMemberBase)
public:
  float         integ;           // #DEF_1;0.5 #MIN_0 overall rate constant for numerical integration, for all equations at the unit level -- all time constants are specified in millisecond units, with one cycle = 1 msec -- if you instead want to make one cycle = 2 msec, you can do this globaly by setting this integ value to 2 (etc).  However, stability issues will likely arise if you go too high.  For improved numerical stability, you may even need to reduce this value to 0.5 or possibly even lower (typically however this is not necessary).  MUST also coordinate this with network.time_inc variable to ensure that global network.time reflects simulated time accurately
  float         vm_tau;          // #AKA_vm_time #DEF_2.81:10 [3.3 std for rate code, 2.81 for spiking] #MIN_1 membrane potential and rate-code activation time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) -- reflects the capacitance of the neuron in principle -- biological default for AeEx spiking model C = 281 pF = 2.81 normalized -- for rate-code activation, this also determines how fast to integrate computed activation values over time
  float         net_tau;         // #AKA_net_time #DEF_1.4;3;5 #MIN_1 net input time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) -- this is important for damping oscillations -- generally reflects time constants associated with synaptic channels which are not modeled in the most abstract rate code models (set to 1 for detailed spiking models with more realistic synaptic currents) -- larger values (e.g., 3) can be important for models with higher netinputs that otherwise might be more prone to oscillation, and is default for GPiInvUnitSpec
  int           vm_cyc;          // #MIN_1 number of steps to integrate membrane potential vm -- each cycle is a midpoint method integration step
  int           fast_cyc;        // #AKA_vm_eq_cyc #DEF_0 number of cycles at start of a trial to run units in a fast integration mode -- the rate-code activations have no effective time constant and change immediately to the new computed value (vm_time is ignored) and vm is computed as an equilibirium potential given current inputs: set to 1 to quickly activate soft-clamped input layers (primary use); set to 100 to always use this computation

  float         vm_dt;           // #READ_ONLY #EXPERT nominal rate = 1 / tau
  float         vm_dt_cyc;       // #READ_ONLY #EXPERT dt / vm_dt_cyc -- actual effective dt
  float         net_dt;          // #READ_ONLY #EXPERT rate = 1 / tau

  INLINE void   UpdtDts()
  { vm_dt = 1.0f / vm_tau;  vm_dt_cyc = vm_dt / (float)vm_cyc;  net_dt = 1.0f / net_tau; }

  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraDtSpec);
  
  STATE_UAE( UpdtDts(); );
  
private:
  void        Initialize()      { fast_cyc = 0; Defaults_init(); }
  void        Defaults_init() {
    integ = 1.0f;  vm_tau = 3.3f;  vm_cyc = 1; net_tau = 1.4f;
    UpdtDts();
  }
};


class STATE_CLASS(LeabraActAvgSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra rate constants for averaging over activations -- only used in XCAL learning rules
INHERITED(SpecMemberBase)
public:
  float         ss_tau;                // #DEF_2;4;7 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life), for continuously updating the super-short time-scale avg_ss value -- this is provides a pre-integration step before integrating into the avg_s short time scale -- it is particularly important for spiking -- in general 4 is the largest value without starting to impair learning, but a value of 7 can be combined with m_in_s = 0 with somewhat worse results
  float         s_tau;                // #DEF_2 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life), for continuously updating the short time-scale avg_s value from the super-short avg_ss value (cascade mode) -- avg_s represents the plus phase learning signal that reflects the most recent past information
  float         m_tau;                // #DEF_10 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life), for continuously updating the medium time-scale avg_m value from the short avg_s value (cascade mode) -- avg_m represents the minus phase learning signal that reflects the expectation representation prior to experiencing the outcome (in addition to the outcome) -- the default value of 10 generally cannot be exceeded without impairing learning
  float         m_in_s;                // #DEF_0.1;0 #MIN_0 #MAX_1 how much of the medium term average activation to include at the short (plus phase) avg_s_eff variable that is actually used in learning -- important to ensure that when unit turns off in plus phase (short time scale), enough medium-phase trace remains so that learning signal doesn't just go all the way to 0, at which point no learning would take place -- typically need faster time constant for updating s such that this trace of the m signal is lost -- can set ss_tau=7 and set this to 0 but learning is generally somewhat worse

  float         ss_dt;               // #READ_ONLY #EXPERT rate = 1 / tau
  float         s_dt;                // #READ_ONLY #EXPERT rate = 1 / tau
  float         m_dt;                // #READ_ONLY #EXPERT rate = 1 / tau
  float         s_in_s;              // #READ_ONLY #EXPERT 1-m_in_s

  INLINE void   UpdtDts() 
  { ss_dt = 1.0f / ss_tau;  s_dt = 1.0f / s_tau;  m_dt = 1.0f / m_tau;    s_in_s = 1.0f - m_in_s; }
  
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraActAvgSpec);
  
  STATE_UAE( UpdtDts(); );
  
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    ss_tau = 4.0f;  s_tau = 2.0f;  m_tau = 10.0f;  m_in_s = 0.1f;
    UpdtDts();
  }
};


class STATE_CLASS(LeabraAvgLSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra parameters for computing the long-term floating average value, avg_l, which is used for driving BCM-style hebbian learning in XCAL -- this form of learning increases contrast of weights and generally decreases overall activity of neuron, to prevent "hog" units -- it is computed as a running average of the (gain multiplied) medium-time-scale average activation at the end of the trial
INHERITED(SpecMemberBase)
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

  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraAvgLSpec);
  
  STATE_UAE(dt = 1.0f / tau;  lrn_fact = (lrn_max - lrn_min) / (gain - min); );
  
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    init = 0.4f;  gain = 2.5f;  min = 0.2f;  tau = 10.0f;  lrn_max = 0.5f;
    lrn_min = 0.0001f;
  
    dt = 1.0f / tau;
    lrn_fact = (lrn_max - lrn_min) / (gain - min);
  }
};


class STATE_CLASS(LeabraAvgL2Spec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra additional parameters for computing the long-term floating average value, avg_l, which is used for driving BCM-style hebbian learning in XCAL -- this form of learning increases contrast of weights and generally decreases overall activity of neuron, to prevent "hog" units
INHERITED(SpecMemberBase)
public:
  bool          err_mod;        // #DEF_true if true, then we multiply avg_l_lrn factor by layer.cos_diff_avg_lrn to make hebbian term roughly proportional to amount of error driven learning signal across layers -- cos_diff_avg computes the running average of the cos diff value between act_m and act_p (no diff is 1, max diff is 0), and cos_diff_avg_lrn = 1 - cos_diff_avg (and 0 for non-HIDDEN layers), so the effective lrn value is high when there are large error signals (differences) in a layer, and low when error signals are low, producing a more consistent mix overall -- typically this error level tends to be stable for a given layer, so this is really just a quick shortcut for setting layer-specific mixes by hand (which the brain can do) -- see LeabraLayerSpec cos_diff.avg_tau rate constant for integrating cos_diff_avg value
  float         err_min;        // #DEF_0.01:0.1 #CONDSHOW_ON_err_mod minimum layer.cos_diff_avg_lrn value (for non-zero cases, i.e., not for target or input layers) -- ensures a minimum amount of self-organizing learning even for layers that have a very small level of error signal
  float         lay_act_thr;    // #DEF_0.01 threshold of layer average activation on this trial, in order to update avg_l values -- setting to 0 disables this check
  
  STATE_DECO_KEY("UnitSpec");
    STATE_TA_STD_CODE_SPEC(LeabraAvgL2Spec);

private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    err_mod = true;  err_min = 0.01f;  lay_act_thr = 0.01f;
  }

};


class STATE_CLASS(LeabraChannels) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra channels used in Leabra
INHERITED(SpecMemberBase)
public:
  float         e;                // excitatory sodium (Na) AMPA channels activated by synaptic glutamate
  float         l;                // constant leak (potassium, K+) channels -- determines resting potential (typically higher than resting potential of K)
  float         i;                // inhibitory chloride (Cl-) channels activated by synaptic GABA
  float         k;                // gated / active potassium channels -- typicaly hyperpolarizing relative to leak / rest

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraChannels);
  
private:
  void        Initialize()      { e = l = i = k = 0.0f;  }
  void        Defaults_init()   { }
};


class STATE_CLASS(KNaAdaptSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sodium-gated potassium channel adaptation mechanism -- evidence supports at least 3 different time constants: M-type (fast), Slick (medium), and Slack (slow)
INHERITED(SpecMemberBase)
public:
  bool          on;             // apply K-Na adaptation overall?
  float         rate_rise;      // #CONDSHOW_ON_on #DEF_0.8 extra multiplier for rate-coded activations on rise factors -- adjust to match discrete spiking
  bool          f_on;           // #CONDSHOW_ON_on use fast time-scale adaptation
  float         f_rise;         // #CONDSHOW_ON_on&&f_on #DEF_0.05 rise rate of fast time-scale adaptation as function of Na concentration -- directly multiplies -- 1/rise = tau for rise rate
  float         f_max;          // #CONDSHOW_ON_on&&f_on #DEF_0.1 maximum potential conductance of fast K channels -- divide nA biological value by 10 for the normalized units here
  float         f_tau;          // #CONDSHOW_ON_on&&f_on #DEF_50 time constant in cycles for decay of fast time-scale adaptation, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life)
  bool          m_on;           // #CONDSHOW_ON_on use medium time-scale adaptation
  float         m_rise;         // #CONDSHOW_ON_on&&m_on #DEF_0.02 rise rate of medium time-scale adaptation as function of Na concentration -- directly multiplies -- 1/rise = tau for rise rate
  float         m_max;          // #CONDSHOW_ON_on&&m_on #DEF_0.1 maximum potential conductance of medium K channels -- divide nA biological value by 10 for the normalized units here
  float         m_tau;          // #CONDSHOW_ON_on&&m_on #DEF_200 time constant in cycles for medium time-scale adaptation, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life)
  bool          s_on;           // #CONDSHOW_ON_on use slow time-scale adaptation
  float         s_rise;         // #CONDSHOW_ON_on&&s_on #DEF_0.001 rise rate of slow time-scale adaptation as function of Na concentration -- directly multiplies -- 1/rise = tau for rise rate
  float         s_max;          // #CONDSHOW_ON_on&&s_on #DEF_1 maximum potential conductance of slow K channels -- divide nA biological value by 10 for the normalized units here
  float         s_tau;          // #CONDSHOW_ON_on&&s_on #DEF_1000 time constant in cycles for slow time-scale adaptation, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life)

  float         f_dt;           // #READ_ONLY #EXPERT rate = 1 / tau
  float         m_dt;           // #READ_ONLY #EXPERT rate = 1 / tau
  float         s_dt;           // #READ_ONLY #EXPERT rate = 1 / tau

  INLINE void  Compute_dKNa_spike_impl
    (bool con, bool spike, float& gc_kna, float rise, float gmax, float decay_dt)
  { if(!con )           gc_kna = 0.0f;
    else if(spike)      gc_kna += rise * (gmax - gc_kna);
    else                gc_kna -= decay_dt * gc_kna; }
  // compute the change in K channel conductance gc_kna for spiking and channel params

  INLINE void  Compute_dKNa_spike
    (bool spike, float& gc_kna_f, float& gc_kna_m, float& gc_kna_s)
  {
    Compute_dKNa_spike_impl(on && f_on, spike, gc_kna_f, f_rise, f_max, f_dt);
    Compute_dKNa_spike_impl(on && m_on, spike, gc_kna_m, m_rise, m_max, m_dt);
    Compute_dKNa_spike_impl(on && s_on, spike, gc_kna_s, s_rise, s_max, s_dt);
  }
  // update K channel conductances per params for discrete spiking

  INLINE void  Compute_dKNa_rate_impl
    (bool con, float act, float& gc_kna, float rise, float gmax, float decay_dt)
  { if(!con )   gc_kna = 0.0f;
    else        gc_kna += act * rate_rise * rise * (gmax - gc_kna) - decay_dt * gc_kna; }
  // compute the change in K channel conductance gc_kna for given activation and channel params

  INLINE void  Compute_dKNa_rate
    (float act, float& gc_kna_f, float& gc_kna_m, float& gc_kna_s) {
    Compute_dKNa_rate_impl(on && f_on, act, gc_kna_f, f_rise, f_max, f_dt);
    Compute_dKNa_rate_impl(on && m_on, act, gc_kna_m, m_rise, m_max, m_dt);
    Compute_dKNa_rate_impl(on && s_on, act, gc_kna_s, s_rise, s_max, s_dt);
  }
  // update K channel conductances per params for rate-code activation

  INLINE void   UpdtDts()
  { f_dt = 1.0f / f_tau; m_dt = 1.0f / m_tau; s_dt = 1.0f / s_tau; }
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(KNaAdaptSpec);
  
  STATE_UAE( UpdtDts(); );
  
private:
  void        Initialize()      { on = false; Defaults_init(); }
  void        Defaults_init() {
    rate_rise = 0.8f; 
    f_on = true; f_tau = 50.0f;   f_rise = .05f;  f_max = .1f;
    m_on = true; m_tau = 200.0f;  m_rise = .02f;  m_max = .1f;
    s_on = true; s_tau = 1000.0f; s_rise = .001f; s_max = 1.0f;
    UpdtDts();
  }
};


class STATE_CLASS(KNaAdaptMiscSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra extra params associated with sodium-gated potassium channel adaptation mechanism
INHERITED(SpecMemberBase)
public:
  bool          clamp;          // #DEF_true apply adaptation even to clamped layers -- only happens if kna_adapt.on is true
  bool          invert_nd;      // #DEF_true invert the adaptation effect for the act_nd (non-depressed) value that is typically used for learning-drivng averages (avg_ss, _s, _m) -- only happens if kna_adapt.on is true
  float         max_gc;         // #CONDSHOW_ON_clamp||invert_nd #DEF_0.2 for clamp or invert_nd, maximum k_na conductance that we expect to get (prior to multiplying by g_bar.k) -- apply a proportional reduction in clamped activation and/or enhancement of act_nd based on current k_na conductance -- default is appropriate for default kna_adapt params
  float         max_adapt;      // #CONDSHOW_ON_clamp||invert_nd has opposite effects for clamp and invert_nd (and only operative when kna_adapt.on in addition): for clamp on clamped layers, this is the maximum amount of adaptation to apply to clamped activations when conductance is at max_gc -- biologically, values around .5 correspond generally to strong adaptation in primary visual cortex (V1) -- for invert_nd, this is the maximum amount of adaptation to invert, which is key for allowing learning to operate successfully despite the depression of activations due to adaptation -- values around .2 to .4 are good for g_bar.k = .2, depending on how strongly inputs are depressed -- need to experiment to find the best value for a given config
  bool          no_targ;        // #DEF_true automatically exclude units in TARGET layers and also TRC (Pulvinar) thalamic neurons from adaptation effects -- typically such layers should not be subject to these effects, so this makes it easier to not have to manually set those override params

  INLINE float Compute_Clamped(float clamp_act, float gc_kna_f, float gc_kna_m, float gc_kna_s) {
    float gc_kna = gc_kna_f + gc_kna_m + gc_kna_s;
    float pct_gc = fminf(gc_kna / max_gc, 1.0f);
    return clamp_act * (1.0f - pct_gc * max_adapt);
  }
  // apply adaptation directly to a clamped activation value, reducing in proportion to amount of k_na current

  INLINE float Compute_ActNd(float act, float gc_kna_f, float gc_kna_m, float gc_kna_s) {
    float gc_kna = gc_kna_f + gc_kna_m + gc_kna_s;
    float pct_gc = fminf(gc_kna / max_gc, 1.0f);
    return act * (1.0f + pct_gc * max_adapt);
  }
  // apply inverse of adaptation to activation value, increasing in proportion to amount of k_na current

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(KNaAdaptMiscSpec);
  
  // STATE_UAE( UpdtDts(); );
  
private:
  void        Initialize()      { Defaults_init(); }
  void        Defaults_init() {
    clamp = true;  invert_nd = true;  max_gc = .2f;  max_adapt = 0.3f;  no_targ = true;
  }
};


class STATE_CLASS(ShortPlastSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra short-term plasticity specifications -- different algorithms are available to update the syn_tr amount of neurotransmitter available to release, which multiplies computed firing rate or spiking (but not act_nd) to produce a net sending activation efficacy in the act and act_eq variables
INHERITED(SpecMemberBase)
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
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(ShortPlastSpec);
  
  STATE_UAE
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
  
    thresh = 0.5f;
    n_trials = 1;
    rec_prob = 0.1f;
  }    
};

class STATE_CLASS(SynDelaySpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra synaptic delay -- activation sent to other units is delayed by a given number of cycles
INHERITED(SpecMemberBase)
public:
  bool          on;                // is synaptic delay active?
  int           delay;             // #CONDSHOW_ON_on #MIN_0 number of cycles to delay for

  STATE_DECO_KEY("UnitSpec");
    STATE_TA_STD_CODE_SPEC(SynDelaySpec);
private:
  void        Initialize()      { on = false; delay = 4; Defaults_init(); }
  void        Defaults_init()   { }; // note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};


class STATE_CLASS(DeepSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for DeepLeabra deep neocortical layer dynamics, which capture attentional, thalamic auto-encoder, and temporal integration mechanisms 
INHERITED(SpecMemberBase)
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

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(DeepSpec);
  
  STATE_UAE(mod_range = 1.0f - mod_min;  ctxt_new = 1.0f - ctxt_prv;
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


class STATE_CLASS(TRCSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for DeepLeabra thalamic relay cells -- engaged only for deep.on and deep.role == TRC
INHERITED(SpecMemberBase)
public:
  bool       p_only_m;          // TRC plus-phase (clamping) for TRC units only occurs if the minus phase max activation for given unit group is above .1
  bool       thal_gate;         // apply thalamic gating to TRC activations -- multiply netin by current thal parameter
  bool       clamp_net;         // #CONDSHOW_OFF_avg_clamp directly clamp the deep netin instead of settling -- appropriate for one-to-one projections to exactly reproduce the activation state
  bool       avg_clamp;         // #CONDSHOW_OFF_clamp_net TRC plus-phase netinput is weighted average (see deep_gain) of current plus-phase deep netin and standard netin -- produces a better clamping dynamic
  float      deep_gain;         // #CONDSHOW_ON_avg_clamp how much to weight the deep netin relative to standard netin  (1.0-deep_gain) for avg_clamp
  bool       clip;              // clip the deep netin to clip_max value -- produces more of an OR-like behavior for TRC reps
  float      clip_max;          // #CONDSHOW_ON_clip maximum netin value to clip deep raw netin in trc plus-phase clamping -- prevents strong from dominating weak too much..

  float      std_gain;          // #READ_ONLY #HIDDEN 1-deep_gain

  INLINE float  TRCClampNet(float deep_raw_net, const float net_syn)
  { if(clip)      deep_raw_net = fminf(deep_raw_net, clip_max);
    if(avg_clamp) return deep_gain * deep_raw_net + std_gain * net_syn;
    else          return deep_raw_net; }
  // compute TRC plus-phase clamp netinput
  
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(TRCSpec);
  
  STATE_UAE(std_gain = 1.0f - deep_gain;  if(clamp_net) avg_clamp = false; if(avg_clamp) clamp_net = false; );

private:
  void        Initialize()
  { clamp_net = false; avg_clamp = false; clip = false; clip_max = 0.4f;  Defaults_init(); }

  void        Defaults_init() {
    p_only_m = false;
    thal_gate = false;
    deep_gain = 0.2f;
    std_gain = 1.0f - deep_gain;
  }
};


class STATE_CLASS(DaModSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for effects of da-based modulation: plus-phase = learning effects
INHERITED(SpecMemberBase)
public:
  bool          on;               // whether to add dopamine factor to net input
  bool          mod_gain;         // modulate gain instead of net input
  float         minus;            // #CONDSHOW_ON_on how much to multiply da_p in the minus phase to add to netinput -- use negative values for NoGo/indirect pathway/D2 type neurons
  float         plus;             // #CONDSHOW_ON_on #AKA_gain how much to multiply da_p in the plus phase to add to netinput -- use negative values for NoGo/indirect pathway/D2 type neurons
  float         da_neg_gain;      // #CONDSHOW_ON_on&&mod_gain for negative dopamine, how much to change the default gain value as a function of dopamine: gain_eff = gain * (1 + da * da_neg_gain) -- da is multiplied by minus or plus depending on phase
  float         da_pos_gain;      // #CONDSHOW_ON_on&&mod_gain for positive dopamine, how much to change the default gain value as a function of dopamine: gain_eff = gain * (1 + da * da_pos_gain) -- da is multiplied by minus or plus depending on phase

  INLINE bool   DoDaModNetin() { return on && !mod_gain; }
  // are we doing netin modulation
  INLINE bool   DoDaModGain() { return on && mod_gain; }
  // are we doing gain modulation

  INLINE float  DaModGain(float da, float gain, bool plus_phase) {
    float da_eff = da;
    if(plus_phase)
      da_eff *= plus;
    else
      da_eff *= minus;
    if(da < 0.0f) {
      return gain * (1.0f + da_eff * da_neg_gain);
    }
    else {
      return gain * (1.0f + da_eff * da_pos_gain);
    }
  }
  // get da-modulated gain value
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(DaModSpec);
private:
  void        Initialize()
  { on = false;  mod_gain = false;  minus = 0.0f;  plus = 0.01f;
    da_neg_gain = 0.1f; da_pos_gain = 0.1f; Defaults_init(); }
  void        Defaults_init() { };
};


class STATE_CLASS(LeabraNoiseSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for noise type etc
INHERITED(SpecMemberBase)
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

  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(LeabraNoiseSpec);

private:
  void        Initialize()      { type = NO_NOISE; trial_fixed = true; }
  void        Defaults_init()   { };
};

