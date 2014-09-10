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

#ifndef LeabraUnitSpec_h
#define LeabraUnitSpec_h 1

// parent includes:
#include "network_def.h"
#include <UnitSpec>
#include <SpecMemberBase>

// member includes:
#include <MinMaxRange>
#include <RandomSpec>
#include <Schedule>
#include <FunLookup>
#include <taMath_float>

// declare all other types mentioned but not required to include:
class Unit; // 
class Network; // 
class LeabraNetwork; // 
class LeabraUnit; // 
class DataTable; // 
class LeabraLayerSpec; //
class LeabraInhib; //

eTypeDef_Of(LeabraActFunSpec);

class E_API LeabraActFunSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation function specifications, using the gelin (g_e linear) activation function by default
INHERITED(SpecMemberBase)
public:
  float		thr;		// #DEF_0.5 threshold value Theta (Q) for firing output activation (.5 is more accurate value based on AdEx biological parameters and normalization -- see BioParams button)
  float		gain;		// #DEF_100;40 #MIN_0 gain (gamma) of the rate-coded activation functions -- 100 is default for gelin = true with NOISY_XX1, but 40 is closer to the actual spiking behavior of the AdEx model -- use lower values for more graded signals, generaly in lower input/sensory layers of the network
  float		nvar;		// #DEF_0.005;0.01 #MIN_0 variance of the Gaussian noise kernel for convolving with XX1 in NOISY_XX1 and NOISY_LINEAR -- determines the level of curvature of the activation function near the threshold -- increase for more graded responding there -- note that this is not actual stochastic noise, just constant convolved gaussian smoothness to the activation function

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraActFunSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraActMiscSpec);

class E_API LeabraActMiscSpec : public SpecMemberBase {
  //##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra miscellaneous activation computation parameters and specs
INHERITED(SpecMemberBase)
public:
  enum ActLrnVal {   // what activation value to use for learning, stored in act_lrn variable
    ACT_EQ,          // use the rate-code equivalent activations, act_eq, which are subject to depression -- not recommeded for CHL-based learning rule, but can work otherwise, although the risk of significant differences between minus and plus phase could be problematic
    ACT_ND,          // use the non-depressed activations, act_nd -- this is particularly useful when using CHL-style learning, which can be distorted by effects of depression, though it can also affect XCAL -- note that adaptation effects are always still included in act_nd
  };

  ActLrnVal     act_lrn;        // which activation variable should be used for learning?  gets stored in unit act_lrn variable, and is then used for the phase activation states (act_m, act_p), which drive CHL learning, and for driving the time-averages (avg_s, avg_m) that drive XCAL learning

  float         act_max_hz;     // #DEF_100 #MIN_1 for translating rate-code activations into discrete spiking, what is the maximum firing rate associated with a maximum activation value (max act is typically 1.0 -- depends on act_range)
  float         time_unit;      // #DEF_1000 for translating rate-code activations into discrete spiking, what is the time unit for computing intervals between spikes from a Hz firing rate?  default is 1000 msec -- computation also takes into account the dt.integ setting
  float		avg_time;	// #DEF_200 #MIN_1 for integrating activation average (act_avg), time constant in trials (roughly, how long it takes for value to change significantly) -- used mostly for visualization and tracking "hog" units
  float		avg_init;	// #DEF_0.15 #MIN_0 initial activation average value -- used for act_avg, avg_s, avg_m, avg_l
  bool          rescale_ctxt;   // #DEF_true re-scale the TI context net input in the minus phase, according to how much the relative scaling might have changed across phases -- preserves correct relative scaling levels when there are different relative scaling parameters in plus and minus phases
  float		avg_dt;		// #READ_ONLY #EXPERT rate = 1 / time

  inline int    ActToInterval(const float integ, const float act)
  { return (int) (time_unit / (integ * act * act_max_hz)); }

  TA_SIMPLE_BASEFUNS(LeabraActMiscSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(SpikeFunSpec);

class E_API SpikeFunSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra spiking activation function specs -- conductance is computed postsynaptically using an alpha function based on spike pulses sent presynaptically -- for clamped layers, spiking probability is proportional to external input controlled by the clamp_type and clamp_max_p values -- soft clamping may still be a better option though
INHERITED(SpecMemberBase)
public:
  float		rise;		// #DEF_0 #MIN_0 exponential rise time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to only include decay time (1/decay e^(-t/decay)), which is highly optimized (doesn't use window -- just uses recursive exp decay) and thus the default!
  float		decay;		// #DEF_5 #MIN_0 exponential decay time (in cycles) of the synaptic conductance according to the alpha function 1/(decay - rise) [e^(-t/decay) - e^(-t/rise)] -- set to 0 to implement a delta function (not very useful)
  float		g_gain;		// #DEF_9 #MIN_0 multiplier for the spike-generated conductances when using alpha function which is normalized by area under the curve -- needed to recalibrate the alpha-function currents relative to rate code net input which is overall larger -- in general making this the same as the decay constant works well, effectively neutralizing the area normalization (results in consistent peak current, but differential integrated current over time as a function of rise and decay)
  int		window;		// #DEF_3 #MIN_0 spike integration window -- when rise==0, this window is used to smooth out the spike impulses similar to a rise time -- each net contributes over the window in proportion to 1/window -- for rise > 0, this is used for computing the alpha function -- should be long enough to incorporate the bulk of the alpha function, but the longer the window, the greater the computational cost
  float		eq_gain;	// #DEF_8 #MIN_0 gain for computing act_eq relative to actual average: act_eq = eq_gain * (spikes/cycles)
  float		eq_time;	// #DEF_50 #MIN_0 if non-zero, compute act_eq as a continuous running average instead of explicit spikes / cycles -- this is the time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly)

  float		gg_decay;	// #READ_ONLY #NO_SAVE g_gain/decay
  float		gg_decay_sq;	// #READ_ONLY #NO_SAVE g_gain/decay^2
  float		gg_decay_rise; // #READ_ONLY #NO_SAVE g_gain/(decay-rise)
  float		oneo_decay;	// #READ_ONLY #NO_SAVE 1.0/decay
  float		oneo_rise;	// #READ_ONLY #NO_SAVE 1.0/rise
  float         eq_dt;          // #READ_ONLY #EXPERT rate = 1 / time

  float	ComputeAlpha(float t) {
    if(decay == 0.0f) return (t == 0.0f) ? g_gain : 0.0f; // delta function
    if(rise == 0.0f) return gg_decay * taMath_float::exp_fast(-t * oneo_decay);	 // exponential
    if(rise == decay) return t * gg_decay_sq * taMath_float::exp_fast(-t * oneo_decay); // symmetric alpha
    return gg_decay_rise * (taMath_float::exp_fast(-t * oneo_decay) - taMath_float::exp_fast(-t * oneo_rise)); // full alpha
  }

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(SpikeFunSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(SpikeMiscSpec);

class E_API SpikeMiscSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra extra misc spiking parameters 
INHERITED(SpecMemberBase)
public:
  enum ClampType {		// how to generate spikes during hard clamp conditions
    POISSON,			// generate spikes according to Poisson distribution with probability = clamp_max_p * u->ext
    UNIFORM,			// generate spikes according to Uniform distribution with probability = clamp_max_p * u->ext
    REGULAR,			// generate spikes every 1 / (clamp_max_p * u->ext) cycles -- this works the best, at least in smaller networks, due to the lack of additional noise, and the synchrony of the inputs for driving synchrony elsewhere
    CLAMPED,			// just use the straight clamped activation value -- do not do any further modifications
  };

  float		exp_slope;	// #DEF_0.02;0 slope in v_m (2 mV = .02 in normalized units) for extra exponential excitatory current that drives v_m rapidly upward for spiking as it gets past its nominal firing threshold (act.thr) -- nicely captures the Hodgkin Huxley dynamics of Na and K channels -- uses Brette & Gurstner 2005 AdEx formulation -- a value of 0 disables this mechanism
  float		spk_thr;	// #DEF_0.5;1.2 membrane potential threshold for actually triggering a spike -- the nominal threshold in act.thr enters into the exponential mechanism, but this value is actually used for spike thresholding (if not using exp_slope > 0, then must set this to act.thr -- 0.5 std)
  float		clamp_max_p;	// #DEF_0.12 #MIN_0 #MAX_1 maximum probability of spike rate firing for hard-clamped external inputs -- multiply ext value times this to get overall probability of firing a spike -- distribution is determined by clamp_type
  ClampType	clamp_type;	// how to generate spikes when layer is hard clamped -- in many cases soft clamping may work better
  float		vm_r;		// #DEF_0;0.15;0.3 #AKA_v_m_r post-spiking membrane potential to reset to, produces refractory effect if lower than vm_init -- 0.30 is apropriate biologically-based value for AdEx (Brette & Gurstner, 2005) parameters
  int		t_r;		// #DEF_0;6 post-spiking explicit refractory period, in cycles -- prevents v_m updating for this number of cycles post firing

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(SpikeMiscSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(OptThreshSpec);

class E_API OptThreshSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra optimization thresholds for faster processing
INHERITED(SpecMemberBase)
public:
  float		send;		// #DEF_0.1 don't send activation when act <= send -- greatly speeds processing
  float		delta;		// #DEF_0.005 don't send activation changes until they exceed this threshold: only for when LeabraNetwork::send_delta is on!
  float         xcal_lrn;       // #DEF_0.01 xcal learning threshold -- don't learn when sending unit activation is below this value in both phases -- due to the nature of the learning function being 0 when the sr coproduct is 0, it should not affect learning in any substantial way

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(OptThreshSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(LeabraDtSpec);

class E_API LeabraDtSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra time and rate constants for temporal derivatives in Leabra (Vm, net input)
INHERITED(SpecMemberBase)
public:
  float		integ;		// #DEF_1;0.5 #MIN_0 overall rate constant for numerical integration, for all equations at the unit level -- all time constants are specified in millisecond units, with one cycle = 1 msec -- if you instead want to make one cycle = 2 msec, you can do this globaly by setting this integ value to 2 (etc).  However, stability issues will likely arise if you go too high.  For improved numerical stability, you may even need to reduce this value to 0.5 or possibly even lower (typically however this is not necessary)
  float		vm_time;	// #DEF_2.81:10 [3.3 std for rate code, 2.81 for spiking] #MIN_1 membrane potential and rate-code activation time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly) -- reflects the capacitance of the neuron in principle -- biological default for AeEx spiking model C = 281 pF = 2.81 normalized -- for rate-code activation, this also determines how fast to integrate computed activation values over time
  float		net_time;	// #DEF_1.4 #MIN_1 net input time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly) -- this is important for damping oscillations -- generally reflects time constants associated with synaptic channels which are not modeled in the most abstract rate code models (set to 1 for detailed spiking models with more realistic synaptic currents)
  bool		midpoint;	// use the midpoint method in computing the vm value -- better avoids oscillations and allows a faster vm_time constant parameter to be used -- this is critical to use with SPIKE mode
  int		fast_cyc;	// #AKA_vm_eq_cyc #DEF_0 number of cycles at start of a trial to run units in a fast integration mode -- the rate-code activations have no effective time constant and change immediately to the new computed value (vm_time is ignored) and vm is computed as an equilibirium potential given current inputs: set to 1 to quickly activate soft-clamped input layers (primary use); set to 100 to always use this computation

  float		vm;		// #READ_ONLY #SHOW rate = 1 / time
  float		net;		// #READ_ONLY #SHOW rate = 1 / time

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraDtSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraActAvgSpec);

class E_API LeabraActAvgSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra rate constants for averaging over activations -- only used in XCAL learning rules
INHERITED(SpecMemberBase)
public:
  float		l_up_inc;	// #AKA_l_up_dt #DEF_0.05:0.25 [0.2 std] amount to increment the long time-average activation (avg_l) at the trial level, after multiplying by the minus phase activation -- this is an additive increase for all units with act_m > opt_thresh.send -- larger values drive higher long time-average values, which acts as the threshold for XCAL self-organizing BCM-style learning, meaning that learning will shift to weight decreases more quickly as activity persists -- if weights are steadily decreasing too much, then lower values are needed -- typically works best with the highest value that does not result in excessive weight loss
  float		l_dn_time;	// #DEF_2.5 time constant in trials (roughly, how long it takes for value to change significantly) for decreases in long time-average activation (avg_l), which occur when activity < opt_thresh.send -- the resulting rate constant value (1/time) is multiplied by layer kwta.pct target activity, to keep things normalized relative to the expected sparseness of activity in a layer -- therfore it is important to make the layer kwta.pct value accurate
  bool          cascade;        // use cascading, continuously updating running average computations -- each average builds upon the next, and everything is continuously updated regardless of phase or other sravg timing signals set at the network level -- this is the most biologically plausible version
  float		ss_time;	// #CONDSHOW_ON_cascade #DEF_1;10 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly), for continuously updating the super-short time-scale avg_ss value -- this is primarily of use for discrete spiking models to integrate over the discrete spikes before integrating into the avg_s short time scale, and should generally be set to 1 for rate-code activations
  float		s_time;		// #CONDSHOW_ON_cascade #DEF_5;50 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly), for continuously updating the short time-scale avg_s value from the super-short avg_ss value (cascade mode) -- avg_s represents the plus phase learning signal that reflects the most recent past information
  float		m_time;		// #CONDSHOW_ON_cascade #DEF_10;100 #MIN_1 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly), for continuously updating the medium time-scale avg_m value from the short avg_s value (cascade mode) -- avg_m represents the minus phase learning signal that reflects the expectation representation prior to experiencing the outcome (in addition to the outcome)

  float		l_dn_dt;	// #READ_ONLY #EXPERT rate = 1 / time
  float		ss_dt;		// #READ_ONLY #EXPERT rate = 1 / time
  float		s_dt;		// #READ_ONLY #EXPERT rate = 1 / time
  float		m_dt;		// #READ_ONLY #EXPERT rate = 1 / time

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraActAvgSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(LeabraChannels);

class E_API LeabraChannels : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra channels used in Leabra
INHERITED(taOBase)
public:
  float		e;		// excitatory (sodium (Na) channel), synaptic glutamate AMPA activated
  float		l;		// constant leak (potassium, K+) channel 
  float		i;		// inhibitory (chloride, Cl-) channel, synaptic GABA activated

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  void 	Copy_(const LeabraChannels& cp); // used in units, so optimized copy needed
  TA_BASEFUNS(LeabraChannels);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActAdaptSpec);

class E_API ActAdaptSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation-driven adaptation dynamics -- negative feedback on v_m based on sub- and super-threshold activation -- relatively rapid time-scale and especially relevant for spike-based models -- drives the adapt variable on the unit
INHERITED(SpecMemberBase)
public:
  bool		on;		// apply adaptation?
  float		time;	        // #CONDSHOW_ON_on #DEF_144 adaptation dynamics time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly)
  float		vm_gain;	// #CONDSHOW_ON_on #MIN_0 #DEF_0.04 gain on the membrane potential v_m driving the adapt adaptation variable -- default of 0.04 reflects 4nS biological value converted into normalized units
  float		spike_gain;	// #CONDSHOW_ON_on #DEF_0.00805 value to add to the adapt adaptation variable after spiking -- default of 0.00805 is normalized version of .0805 nA in biological values -- for rate code activations, uses act value weighting and only computes every interval
  float		dt;		// #READ_ONLY #EXPERT rate = 1 / time

  float	Compute_dAdapt(float vm, float e_rev_l, float adapt)
  { return dt * (vm_gain * (vm - e_rev_l) - adapt); }
  // compute the change in adapt given vm, resting reversal potential (leak reversal), and adapt inputs

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(ActAdaptSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(ShortPlastSpec);

class E_API ShortPlastSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra short-term plasticity specifications -- uses standard equations summarized in Hennig, 2013 (eq 6) to capture both facilitation and depression dynamics as a function of presynaptic firing -- models interactions between number of vesicles available to release, and probability of release, and a time-varying recovery rate
INHERITED(SpecMemberBase)
public:
  bool		on;		// synaptic depression is in effect: multiplies normal activation computed by current activation function in effect
  float         f_r_ratio;      // #CONDSHOW_ON_on #DEF_0.01:3 ratio of facilitating (t_fac) to depression recovery (t_rec) time constants -- influences overall nature of response balance (ratio = 1 is balanced, > 1 is facilitating, < 1 is depressing).  Wang et al 2006 found: ~2.5 for strongly facilitating PFC neurons (E1), ~0.02 for strongly depressing PFC and visual cortex (E2), and ~1.0 for balanced PFC (E3)
  float		t_rec;		// #CONDSHOW_ON_on #DEF_100:1000 #MIN_1 time constant (milliseconds) for the constant form of the recovery of number of available vesicles to release at each action potential -- one factor influencing how strong and long-lasting depression is: nr += (1-nr)/t_rec.  Wang et al 2006 found: ~200ms for strongly depressing in visual cortex and facilitating PFC (E1), 600ms for depressing PFC (E2), and between 200-600 for balanced (E3)
  float         t_fac;          // #CONDSHOW_ON_on #READ_ONLY #SHOW computed from f_r_ratio and t_rec: time constant (milliseconds) for the dynamics of facilitation of release probability: pr += (p0 - pr) / t_fac. Wang et al 2006 found: 6ms for visual cortex, 10-20ms strongly depressing PFC (E2), ~500ms for strongly facilitating (E1), and between 200-600 for balanced (E3)
  float         p0;             // #CONDSHOW_ON_on #DEF_0.1:0.4 baseline probability of release -- lower values around .1 produce more strongly facilitating dynamics, while .4 makes depression dominant -- interacts with f_r_ratio time constants as well
  float		fac;            // #CONDSHOW_ON_on #DEF_0.2:0.5 #MIN_0 strength of facilitation effect -- how much each action potential facilitates the probability of release toward a maximum of one: pr += fac (1-pr) -- typically right around 0.3 in Wang et al, 2006
  float		t_kre;	        // #CONDSHOW_ON_on #DEF_100 time constant (milliseconds) on dynamic enhancement of time constant of recovery due to activation -- recovery time constant increases as a function of activity, helping to linearize response (reduce level of depression) at higher frequencies -- supported by multiple sources of biological data (Hennig, 2013)
  float         re;             // #CONDSHOW_ON_on #DEF_0.002:0 how much the dynamic enhancement of recovery time constant increases for each action potential -- determines how strong this dynamic component is -- set to 0 to turn off this extra adaptation
  float         act_hz;	        // #CONDSHOW_ON_on #DEF_100 factor for converting rate-coded activations into a firing rate, which is then used to simulate discrete spiking for purposes of updating these equations -- e.g, 100 hz = 10 cycle (msec) interval spiking for activation = 1
  float		dt_rec;		// #CONDSHOW_ON_on #READ_ONLY #SHOW integration multiplier for recovery = 1/t_rec 
  float         dt_fac;         // #CONDSHOW_ON_on #READ_ONLY #SHOW integration multiplier for facilitation = 1/t_fac
  float         dt_kre;         // #CONDSHOW_ON_on #READ_ONLY #SHOW integration multiplier for recovery enhancement = 1/t_kre

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(ShortPlastSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(SynDelaySpec);

class E_API SynDelaySpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra synaptic delay -- activation sent to other units is delayed by a given number of cycles
INHERITED(SpecMemberBase)
public:
  bool		on;		// is synaptic delay active?
  int		delay;		// #CONDSHOW_ON_on #MIN_0 number of cycles to delay for

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(SynDelaySpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { }; // note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(CIFERSpec);

class E_API CIFERSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for Cortical Information Flow via Extra Range theory, simulating effects of thalamic drive on cortical neurons, including superficial and deep components of a Unit-level microcolumn -- thalamic input modulates superficial netin and is used thresholded to determine deep5b activation
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable the CIFER mechanisms (otherwise, deep5b == act and thal is ignored)
  bool          phase_updt;     // #CONDSHOW_ON_on TI context and deep layer activations update at the end of every phase (e.g., for PFC) -- otherwise update is at the end of every trial (posterior cortex)
  float         super_gain;     // #CONDSHOW_ON_on #MIN_0 gain on modulation of superficial (2/3 = act) netin -- thal only increases netin otherwise recv'd: netin = (1 + gain * thal) * netin_raw
  float	        thal_5b_thr;    // #CONDSHOW_ON_on #MIN_0 threshold on thal value for deep5b neurons to fire -- neurons below this level have deep5b = 0 -- above this level, deep5b = thal * act or 1 depending on binary_5b flag
  float	        act_5b_thr;	// #CONDSHOW_ON_on #MIN_0 threshold on act_eq value for deep5b neurons to fire -- neurons below this level have deep5b = 0 -- above this level, deep5b = thal * act or 1 depending on binary_5b flag
  bool          binary5b;       // #CONDSHOW_ON_on make deep5b binary (1.0 or 0.0) -- otherwise it is thal * act
  float         ti_5b;          // #CONDSHOW_ON_on #MIN_0 #MAX_1 how much of deep5b to use for TI context information -- 1-ti_5b comes from act_eq -- biologically both sources of info can be mixed into layer 6 context signal
  float         bg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for background cortico-cortical activations: lrate_eff = lrate * (bg_lrate + fg_lrate * thal)
  float         fg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for foreground deep activations: lrate_eff = lrate * (bg_lrate + fg_lrate * thal)

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(CIFERSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(DaModSpec);

class E_API DaModSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for effects of da-based modulation: plus-phase = learning effects
INHERITED(SpecMemberBase)
public:
  enum ModType {
    PLUS_CONT,			// da modulates plus-phase activations (only) in a continuous manner
    PLUS_POST,			// da modulates plus-phase activations (only), at the end of the plus phase
  };

  bool		on;		// whether to actually modulate activations by da values
  ModType	mod;		// #CONDSHOW_ON_on #DEF_PLUS_CONT how to apply DA modulation
  float		gain;		// #CONDSHOW_ON_on #MIN_0 gain multiplier of da values

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(DaModSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(NoiseAdaptSpec);

class E_API NoiseAdaptSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for adapting the noise variance over time as a function of different variables
INHERITED(SpecMemberBase)
public:
  enum AdaptMode {
    FIXED_NOISE,		// no adaptation of noise: remains fixed at the noise.var value in the unit spec
    SCHED_CYCLES,		// use noise_sched over cycles of settling
    SCHED_EPOCHS,		// use noise_sched over epochs of learning
    PVLV_PVI,			// use PVLV PVi reward prediction values -- predicts primary value (PV) and inhibits it (i) -- only if a PVLV config is in place -- appropriate for output-oriented layers that are important for generating responses associated with rewards
    PVLV_LVE,			// use PVLV LVe reward prediction values -- learned value excitatory reward associations -- this can be active outside the time when primary rewards are expected, and is appropriate for working memory or other internal or sensory-oriented processing
    PVLV_MIN,			// use the minimum of PVi and LVe values -- an overall worst case appraisal of the state of the system
  };

  bool		trial_fixed;	// keep the same noise value over the entire trial -- prevents noise from being washed out and produces a stable effect that can be better used for learning -- this is strongly recommended for most learning situations
  bool		k_pos_noise;	// #CONDSHOW_ON_trial_fixed a special kind of trial_fixed noise, where k units (according to computed kwta function) chosen at random (permuted list) are given a positive noise.var value of noise, while the remainder get nothing -- approximates a k-softmax in some respects
  AdaptMode 	mode;		// how to adapt noise variance over time
  float		min_pct;	// #CONDSHOW_OFF_mode:FIXED_NOISE,SCHED_CYCLES,SCHED_EPOCHS #DEF_0.5 minimum noise as a percentage (proportion) of overall maximum noise value (which is noise.var in unit spec)
  float		min_pct_c;	// #READ_ONLY 1-min_pct

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(NoiseAdaptSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraUnitSpec);

class E_API LeabraUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_Leabra Leabra unit specifications, point-neuron approximation
INHERITED(UnitSpec)
public:
  enum ActFun {
    NOISY_XX1,			// x over x plus 1 convolved with Gaussian noise (noise is nvar)
    SPIKE,			// discrete spiking activations (spike when > thr) -- default params produce adaptive exponential (AdEx) model
    NOISY_LINEAR,		// simple linear output function (still thesholded) convolved with Gaussian noise (noise is nvar)
    XX1,			// x over x plus 1, hard threshold, no noise convolution
    LINEAR,			// simple linear output function (still thesholded)
  };

  enum NoiseType {
    NO_NOISE,			// no noise added to processing
    VM_NOISE,			// noise in the value of v_m (membrane potential)
    NETIN_NOISE,		// noise in the net input (g_e)
    ACT_NOISE,			// noise in the activations
  };

  ActFun	act_fun;	// #CAT_Activation activation function to use -- typically NOISY_XX1 or SPIKE -- others are for special purposes or testing
  LeabraActFunSpec act;         // #CAT_Activation activation function parameters -- very important for determining the shape of the selected act_fun
  LeabraActMiscSpec act_misc;   // #CAT_Activation miscellaneous activation parameters
  SpikeFunSpec	spike;		// #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation spiking function specs (only for act_fun = SPIKE)
  SpikeMiscSpec	spike_misc;	// #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation misc extra spiking function specs (only for act_fun = SPIKE)
  OptThreshSpec	opt_thresh;	// #CAT_Learning optimization thresholds for speeding up processing when units are basically inactive
  MinMaxRange	clamp_range;	// #CAT_Activation range of clamped activation values (min, max, 0, .95 std), don't clamp to 1 because acts can't reach, so .95 instead
  MinMaxRange	vm_range;	// #CAT_Activation membrane potential range (min, max, 0-2 for normalized)
  RandomSpec	v_m_init;	// #CAT_Activation what to initialize the membrane potential to (mean = .3, var = 0 std)
  RandomSpec	act_init;	// #CAT_Activation what to initialize the activation to (mean = 0 var = 0 std)
  LeabraDtSpec	dt;		// #CAT_Activation time constants (rate of updating): membrane potential (vm) and net input (net)
  LeabraActAvgSpec act_avg;	// #CAT_Activation time constants (rate of updating) for computing activation averages -- used in XCAL learning rules
  LeabraChannels g_bar;		// #CAT_Activation [Defaults: 1, .1, 1] maximal conductances for channels
  LeabraChannels e_rev;		// #CAT_Activation [Defaults: 1, .3, .25] reversal potentials for each channel
  ActAdaptSpec 	adapt;		// #CAT_Activation activation-driven adaptation factor that drives spike rate adaptation dynamics based on both sub- and supra-threshold membrane potentials
  ShortPlastSpec stp;           // #CAT_Activation short term presynaptic plasticity specs -- can implement full range between facilitating vs. depresssion
  SynDelaySpec	syn_delay;	// #CAT_Activation synaptic delay -- if active, activation sent to other units is delayed by a given amount
  CIFERSpec	cifer;		// #CAT_Learning cortical information flow via extra range -- uses thalmic input to drive a foreground active processing pattern (in deep5b acts) on top of distributed corticocortical background activations (in superficial acts)
  DaModSpec	da_mod;		// #CAT_Learning da modulation of activations (for da-based learning, and other effects)
  NoiseType	noise_type;	// #CAT_Activation where to add random noise in the processing (if at all)
  RandomSpec	noise;		// #CONDSHOW_OFF_noise_type:NO_NOISE #CAT_Activation distribution parameters for random added noise
  NoiseAdaptSpec noise_adapt;	// #CONDSHOW_OFF_noise_type:NO_NOISE #CAT_Activation how to adapt the noise variance (var) value
  Schedule	noise_sched;	// #CONDSHOW_OFF_noise_type:NO_NOISE #CAT_Activation schedule of noise variance -- time scale depends on noise_adapt parameter (cycles, epochs, etc)

  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  void  Init_dWt(Unit* u, Network* net, int thread_no=-1) override;
  void  Init_Weights(Unit* u, Network* net, int thread_no=-1) override;
  void  Init_Weights_sym(Unit* u, Network* net, int thread_no=-1) override;
  void  Init_Weights_post(Unit* u, Network* net, int thread_no=-1) override;

  void	Init_Acts(Unit* u, Network* net) override;
  virtual void 	Init_ActAvg(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation initialize average activation values, used to control learning

  virtual void	Init_Netins(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
  virtual void	DecayState(LeabraUnit* u, LeabraNetwork* net, float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  virtual void	Init_SpikeBuff(LeabraUnit* u);
  // #CAT_Activation initialize spike buffers based on whether they are needed
  virtual void	Init_ActBuff(LeabraUnit* u);
  // #CAT_Activation initialize activation buffer based on whether they are needed

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void	Trial_Init_Specs(LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags 

  virtual void	Trial_Init_Unit(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation trial unit-level initialization functions: DecayState, NoiseInit, Trial_Init_SRAvg

    virtual void Trial_DecayState(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation decay activation states towards initial values: at trial-level boundary
    virtual void Trial_NoiseInit(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation init trial-level noise -- ONLY called if noise_adapt.trial_fixed is set
    virtual void Trial_Init_SRAvg(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Learning reset the sender-receiver coproduct average -- call at start of trial

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  virtual void Settle_Init_Unit(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation settle unit-level initialization functions: Init_TargFlags, DecayState, NetinScale

    virtual void Settle_Init_TargFlags(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation initialize external input flags based on phase
    virtual void Settle_DecayState(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation decay activation states towards initial values: at phase-level boundary -- start of settling
    virtual void Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation compute net input scaling values -- call at start of settle just to be sure

  virtual void Compute_NetinScale_Senders(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Settle_Init_Unit stage after all the recv ones are computed

  virtual void	Compute_HardClamp(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation force units to external values provided by environment
  virtual void	Compute_HardClampNoClip(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for freezing activation states for example, e.g., in second plus phase)

  virtual void	ExtToComp(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  virtual void 	Send_NetinDelta(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold -- only this delta form is supported
  virtual void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
    virtual void Compute_NetinInteg_Spike_e(LeabraUnit* u, LeabraNetwork* net);
    // #IGNORE called by Compute_NetinInteg for spiking units: compute actual excitatory netin conductance value for spiking units by integrating over spike
    virtual void Compute_NetinInteg_Spike_i(LeabraUnit* u, LeabraNetwork* net);
    // #IGNORE called by Compute_NetinInteg for spiking units: compute actual inhibitory netin conductance value for spiking units by integrating over spike

    inline float Compute_IThresh(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation #IGNORE called by Compute_NetinInteg: compute inhibitory value that would place unit directly at threshold
      inline float Compute_IThreshNetinOnly(float netin);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, using only the provided net input value, along with the g_bar.l leak current in the unit spec

  inline float Compute_EThresh(LeabraUnit* u);
  // #CAT_Activation #IGNORE compute excitatory value that would place unit directly at threshold

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition: these are actually called by Compute_Act to integrate
  //            inhibition computed at the layer level

  inline LeabraInhib* GetInhib(LeabraUnit* u);
  // #CAT_Activation #IGNORE get the inhib that applies to this unit (either unit group or entire layer, depending on layer spec setting)

  inline void	Compute_SelfInhib(LeabraUnit* u, LeabraLayerSpec* lspec, 
                                   LeabraNetwork* net);
  // #CAT_Activation #IGNORE compute self inhibition value, for fffb g_i_self inhib val
  inline void	Compute_ApplyInhib(LeabraUnit* u, LeabraLayerSpec* lspec, 
                                   LeabraNetwork* net, float inhib_val);
  // #CAT_Activation #IGNORE apply computed inhibition value to unit inhibitory conductance


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls all the various sub-functions, including Compute_SRAvg
  void	Compute_Act(Unit* u, Network* net, int thread_no=-1) override;

    inline void Compute_Conduct(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation #IGNORE Act Step 1: compute input conductance values in the gc variables

    virtual void Compute_Vm(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 2: compute the membrane potential from input conductances
      inline float Compute_EqVm(LeabraUnit* u);
      // #CAT_Activation #IGNORE compute the equilibrium (asymptotic) membrante potential from input conductances (assuming they remain fixed as they are)

    virtual void Compute_ActFun(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 2: compute the activation function
      virtual void Compute_ActFun_rate(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation from g_e vs. threshold -- rate code functions
      virtual float Compute_ActFun_rate_impl(float val_sub_thr);
      // #CAT_Activation raw activation function: computes an activation value from given value subtracted from its relevant threshold value

      virtual void Compute_ActFun_spike(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation from membrane potential -- discrete spiking
      virtual void Compute_ClampSpike(LeabraUnit* u, LeabraNetwork* net, float spike_p);
      // #CAT_Activation compute spiking activation according to spike.clamp_type with given probability (typically spike.clamp_max_p * u->ext) -- includes depression and other active factors as done in Compute_ActFun_spike -- used for hard clamped inputs in spiking nets
      virtual void Compute_RateCodeSpike(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute spiking activation (u->spike) based off of rate-code activation value

    virtual void Compute_SelfReg_Cycle(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 3: compute self-regulatory dynamics at the cycle time scale -- adapt, etc
    virtual void Compute_SelfReg_Trial(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation compute self-regulatory at the trial time scale
      virtual void Compute_ActAdapt_Cycle(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation-based adaptation value based on spiking and membrane potential
      virtual void Compute_ShortPlast_Cycle(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute whole-neuron (presynaptic) short-term plasticity at the cycle level, using the stp parameters -- updates the spk_amp variable

    virtual float Compute_Noise(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation utility fun to generate and return the noise value based on current settings -- will set unit->noise value as appropriate (generally excludes effect of noise_sched schedule)

  virtual void 	Compute_SRAvg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute sending-receiving activation product averages -- unit level only, used for XCAL
  virtual void 	Compute_SRAvg_Cons(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute sending-receiving activation coproduct averages for the connections -- not used for XCAL typically -- just for CtLeabra_CAL

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  virtual void	Compute_MidMinus(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycle parameter, if > 0 -- currently used for the PBWM algorithm and ff weighting -- stores act_mid

  virtual void 	Compute_CycSynDep(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate conspec subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on -- threaded direct to units

  ///////////////////////////////////////////////////////////////////////
  //	Settle Final

  virtual void	PostSettle(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation set stuff after settling is over (act_m, act_p, etc), ActTimeAvg, DaMod_PlusPost
    virtual void Compute_ActTimeAvg(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation compute time-averaged activation of unit (using act.avg_dt time constant), typically done at end of settling in PostSettle function
    virtual void Compute_DaMod_PlusPost(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation post-plus dav modulation

  ///////////////////////////////////////////////////////////////////////
  //	LeabraTI / CIFER thalmocortical computations

  virtual void	TI_Compute_Deep5bAct(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_LeabraTI compute deep5b activations using cifer specs -- integrates thal and act_eq
  virtual void	TI_Send_Deep5bNetin(LeabraUnit* u, LeabraNetwork* net,
                                  int thread_no=-1);
  // #CAT_LeabraTI send deep5b netinputs through Deep5bConSpec connections
  virtual void	TI_Send_Deep5bNetin_Post(LeabraUnit* u, LeabraNetwork* net,
                                         int thread_no=-1);
  // #CAT_LeabraTI send context netinputs through Deep5bConSpec connections -- post processing rollup
  virtual void	TI_Send_CtxtNetin(LeabraUnit* u, LeabraNetwork* net,
                                  int thread_no=-1);
  // #CAT_LeabraTI send context netinputs through LeabraTICtxtConSpec connections
  virtual void	TI_Send_CtxtNetin_Post(LeabraUnit* u, LeabraNetwork* net,
                                  int thread_no=-1);
  // #CAT_LeabraTI send context netinputs through LeabraTICtxtConSpec connections -- post processing rollup
  virtual void	TI_Compute_CtxtAct(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_LeabraTI compute context activations
  virtual void	TI_ClearContext(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_TI clear the act_ctxt and net_ctxt context variables -- can be useful to do at clear discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //	Trial Update and Final

  virtual void	EncodeState(LeabraUnit*, LeabraNetwork*) { };
  // #CAT_Learning encode current state information after end of current trial (hook for time-based learning)

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  void 	        Compute_dWt(Unit* u, Network* net, int thread_no=-1) override;

  virtual void	Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute normalization of dwt values -- must be done as a separate stage after dwt

  void	Compute_Weights(Unit* u, Network* net, int thread_no=-1) override;

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  float Compute_SSE(Unit* u, Network* net, bool& has_targ) override;
  bool	 Compute_PRerr(Unit* u, Network* net, float& true_pos, float& false_pos, float& false_neg, float& true_neg) override;
  virtual float  Compute_NormErr(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off of act_m vs target) according to settings on the network (returns a 1 or 0) -- if (net->on_errs && act_m > .5 && targ < .5) return 1; if (net->off_errs && act_m < .5 && targ > .5) return 1; else return 0

  ///////////////////////////////////////////////////////////////////////
  //	Misc Housekeeping, non Compute functions

  virtual void	CreateNXX1Fun(LeabraActFunSpec& act_spec, FunLookup& nxx1_fl,
                              FunLookup& noise_fl);
  // #CAT_Activation create convolved gaussian and x/x+1 function lookup tables

  virtual void 	BioParams
    (float norm_sec=0.001f, float norm_volt=0.1f, float volt_off=-0.1f,
     float norm_amp=1.0e-8f, float C_pF=281.0f, float gbar_l_nS=10.0f,
     float gbar_e_nS=100.0f, float gbar_i_nS=100.0f,
     float erev_l_mV=-70.0f, float erev_e_mV=0.0f, float erev_i_mV=-75.0f,
     float act_thr_mV=-50.0f, float spk_thr_mV=20.0f, float exp_slope_mV=2.0f,
     float adapt_dt_time_ms=144.0f, float adapt_vm_gain_nS=4.0f,
     float adapt_spk_gain_nA=0.0805);
  // #BUTTON set parameters based on biologically-based values, using normalization scaling to convert into typical Leabra standard parameters.  norm_x are normalization values to convert from SI units to normalized values (defaults are 1ms = .001 s, 100mV with -100 mV offset to bring into 0-1 range between -100..0 mV, 1e-8 amps (makes g_bar, C, etc params nice).  other defaults are based on the AdEx model of Brette & Gurstner (2005), which the SPIKE mode implements exactly with these default parameters -- last bit of name indicates the units in which this value must be provided (mV = millivolts, ms = milliseconds, pF = picofarads, nS = nanosiemens, nA = nanoamps)

  virtual void	GraphVmFun(DataTable* graph_data, float g_i = .5, float min = 0.0,
                           float max = 1.0, float incr = .01);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph membrane potential (v_m) as a function of excitatory net input (net) for given inhib conductance (g_i) (NULL = new graph data)
  virtual void	GraphActFmNetFun(DataTable* graph_data, float g_i = .5, float min = 0.0,
				 float max = 1.0, float incr = .001, float g_e_thr = 0.5,
				 float lin_gain = 10);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the activation function as a function of net input -- this is the direct activation function, computed relative to the g_e_thr threshold value provided -- a linear comparison with lin_gain slope is also provided for reference -- always computed as lin_gain * (net - g_e_thr) (NULL = new graph data)
  virtual void	GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha=false);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the spike alpha function for conductance integration over time window given in spike parameters -- last data point is the sum over the whole window (total conductance of a single spike) -- force_alpha means use explicit alpha function even when rise=0 (otherewise it simulates actual recursive exp decay used in optimized code)
//   virtual void	GraphSLNoiseAdaptFun(DataTable* graph_data, float incr = 0.05f);
//   // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the short and long-term noise adaptation function, which integrates both short-term and long-term performance values
  virtual void TimeExp(int mode, int nreps=100000000);
  // #EXPERT time how long it takes to compute various forms of exp() function: mode=0 = double sum ctrl (baseline), mode=1 = std double exp(), mode=2 = taMath_double::exp_fast, mode=3 = float sum ctrl (float baseline), mode=4 = expf, mode=5 = taMath_float::exp_fast -- this is the dominant cost in spike alpha function computation, so we're interested in optimizing it..

  bool  CheckConfig_Unit(Unit* un, bool quiet=false) override;

  void	InitLinks();
  SIMPLE_COPY(LeabraUnitSpec);
  TA_BASEFUNS(LeabraUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();	// to set _impl sig
  void 	CheckThisConfig_impl(bool quiet, bool& rval);

  LeabraChannels e_rev_sub_thr;	// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN e_rev - act.thr for each item -- used for compute_ithresh
  float		thr_sub_e_rev_i;// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.i * (act.thr - e_rev.i) used for compute_ithresh
  float		thr_sub_e_rev_e;// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.e * (act.thr - e_rev.e) used for compute_ethresh

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(E_API, LeabraUnitSpec);

#endif // LeabraUnitSpec_h
