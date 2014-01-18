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

eTypeDef_Of(LeabraActFunSpec);

class E_API LeabraActFunSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation function specifications
INHERITED(SpecMemberBase)
public:
  bool		gelin;		// #DEF_true IMPORTANT: Use BioParams button with all default settings if turning this on in an old project to set other important params to match.  Computes rate-code activations directly off of the g_e excitatory conductance (i.e., net = netinput) compared to the g_e value that would put the unit right at its firing threshold (g_e_thr) -- this reproduces the empirical rate-code behavior of a discrete spiking network much better than computing from the v_m - thr value.  other conductances (g_i, g_l, g_a, g_h) enter via their effects on the effective threshold (g_e_thr).  the activation dynamics update over time using the dt.vm time constant, only after v_m itself is over threshold -- if v_m is under threshold, driving act is zero
  float		thr;		// #DEF_0.5 threshold value Theta (Q) for firing output activation (.5 is more accurate value based on AdEx biological parameters and normalization -- see BioParams button -- use this for gelin)
  float		gain;		// #DEF_100;40 #MIN_0 gain (gamma) of the rate-coded activation functions -- 100 is default for gelin = true with NOISY_XX1, but 40 is closer to the actual spiking behavior of the AdEx model -- use lower values for more graded signals, generaly in lower input/sensory layers of the network
  float		nvar;		// #DEF_0.005;0.01 #MIN_0 variance of the Gaussian noise kernel for convolving with XX1 in NOISY_XX1 and NOISY_LINEAR -- determines the level of curvature of the activation function near the threshold -- increase for more graded responding there -- note that this is not actual stochastic noise, just constant convolved gaussian smoothness to the activation function

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraActFunSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraActFunExSpec);

class E_API LeabraActFunExSpec : public LeabraActFunSpec {
  // leabra activation function specifications with extra stuff
INHERITED(LeabraActFunSpec)
public:
  enum IThrFun {	       
    STD,			// include all currents (except bias weights) in inhibitory threshold computation
    NO_A,			// exclude gc.a current: allows accommodation to knock out units from kwta active list, without other units coming in to take their place
    NO_H,			// exclude gc.h current: prevent hysteresis current from affecting inhibitory threshold computation
    NO_AH,			// exclude gc.a and gc.h currents: prevent both accommodation and hysteresis currents from affecting inhibitory threshold computation
    ALL,			// include all currents INCLUDING bias weights
  };

  float		avg_dt;		// #DEF_0.005 #MIN_0 time constant for integrating activation average (act_avg -- computed across trials) -- used mostly for visualization purposes
  float		avg_init;	// #DEF_0.15 #MIN_0 initial activation average value
  IThrFun	i_thr;		// [STD or NO_AH for da mod units] how to compute the inhibitory threshold for kWTA functions (what currents to include or exclude in determining what amount of inhibition would keep the unit just at threshold firing) -- for units with dopamine-like modulation using the a and h currents, NO_AH makes learning much more reliable because otherwise kwta partially compensates for the da modulation

  TA_SIMPLE_BASEFUNS(LeabraActFunExSpec);
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
  float		eq_dt;		// #DEF_0.02 #MIN_0 #MAX_1 if non-zero, eq is computed as a running average with this time constant

  float		gg_decay;	// #READ_ONLY #NO_SAVE g_gain/decay
  float		gg_decay_sq;	// #READ_ONLY #NO_SAVE g_gain/decay^2
  float		gg_decay_rise; // #READ_ONLY #NO_SAVE g_gain/(decay-rise)
  float		oneo_decay;	// #READ_ONLY #NO_SAVE 1.0/decay
  float		oneo_rise;	// #READ_ONLY #NO_SAVE 1.0/rise

  float	ComputeAlpha(float t) {
    if(decay == 0.0f) return (t == 0.0f) ? g_gain : 0.0f; // delta function
    if(rise == 0.0f) return gg_decay * taMath_float::exp_fast(-t * oneo_decay);	 // exponential
    if(rise == decay) return t * gg_decay_sq * taMath_float::exp_fast(-t * oneo_decay); // symmetric alpha
    return gg_decay_rise * (taMath_float::exp_fast(-t * oneo_decay) - taMath_float::exp_fast(-t * oneo_rise)); // full alpha
  }

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

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
  float		vm_dend;	// #DEF_0.3 how much to add to vm_dend value after every spike
  float		vm_dend_dt;	// #DEF_0.16 rate constant for updating the vm_dend value (used for spike-based learning)
  float		vm_dend_time;	// #READ_ONLY #SHOW time constant (in cycles, 1/vm_dend_dt) for updating the vm_dend value (used for spike-based learning)

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(SpikeMiscSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(ActAdaptSpec);

class E_API ActAdaptSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation-driven adaptation dynamics -- negative feedback on v_m based on sub- and super-threshold activation -- relatively rapid time-scale and especially relevant for spike-based models -- drives the adapt variable on the unit
INHERITED(SpecMemberBase)
public:
  bool		on;		// apply adaptation?
  float		dt;		// #CONDSHOW_ON_on #MIN_0 #DEF_0.007 rate constant of the adaptation dynamics -- for 1 ms normalized units, default is 1/144 msec = .007
  float		vm_gain;	// #CONDSHOW_ON_on #MIN_0 #DEF_0.04 gain on the membrane potential v_m driving the adapt adaptation variable -- default of 0.04 reflects 4nS biological value converted into normalized units
  float		spike_gain;	// #CONDSHOW_ON_on #DEF_0.00805 value to add to the adapt adaptation variable after spiking -- default of 0.00805 is normalized version of .0805 nA in biological values -- for rate code activations, uses act value weighting and only computes every interval
  int		interval;	// #CONDSHOW_ON_on how many time steps between applying spike_gain for rate-coded activation function -- simulates the intrinsic delay obtained with spiking dynamics
  float		dt_time;	// #CONDSHOW_ON_on #READ_ONLY #SHOW time constant (in cycles = 1/dt_rate) of the adaptation dynamics

  float	Compute_dAdapt(float vm, float e_rev_l, float adapt) {
    return dt * (vm_gain * (vm - e_rev_l) - adapt);
  }
  // compute the change in adapt given vm, resting reversal potential (leak reversal), and adapt inputs

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(ActAdaptSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(DepressSpec);

class E_API DepressSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra activation/spiking conveyed to other units is subject to synaptic depression: depletes a given amount per spike, and recovers with exponential recovery rate (also subject to trial/phase decay = recovery proportion)
INHERITED(SpecMemberBase)
public:
  bool		on;		// synaptic depression is in effect: multiplies normal activation computed by current activation function in effect
  float		rec;		// #CONDSHOW_ON_on #DEF_0.2;0.015;0.005 #MIN_0 #MAX_1 rate of recovery of spike amplitude (determines overall time constant of depression function)
  float		asymp_act;	// #CONDSHOW_ON_on #DEF_0.2:0.5 #MIN_0 #MAX_1 asymptotic activation value (as proportion of 1) for a fully active unit (determines depl value)
  float		depl;		// #CONDSHOW_ON_on #READ_ONLY #SHOW rate of depletion of spike amplitude as a function of activation output (computed from rec, asymp_act)
  int		interval;	// #CONDSHOW_ON_on #MIN_1 only update synaptic depression at given interval (in terms of cycles, using ct_cycle) -- this can be beneficial in producing a more delayed overall effect, as is observed with discrete spiking
  float		max_amp;	// #CONDSHOW_ON_on #MIN_0 maximum spike amplitude (spk_amp, which is the multiplier factor for activation values) -- values greater than 1 create an extra reservoir where depletion does not yet affect the sending activations, because spk_amp is capped at a maximum of 1 -- this can be useful for creating a more delayed effect of depletion, where an initial wave of activity can propagate unimpeded, followed by actual depression as spk_amp goes below 1

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(DepressSpec);
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

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(SynDelaySpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { }; // note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(OptThreshSpec);

class E_API OptThreshSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra optimization thresholds for faster processing
INHERITED(SpecMemberBase)
public:
  float		send;		// #DEF_0.1 don't send activation when act <= send -- greatly speeds processing
  float		delta;		// #DEF_0.005 don't send activation changes until they exceed this threshold: only for when LeabraNetwork::send_delta is on!
  float		phase_dif;	// #DEF_0 don't learn when +/- phase difference ratio (- / +) < phase_dif (.8 when used, but off by default)
  float         xcal_lrn;       // #DEF_0.01 xcal learning threshold -- don't learn when sending unit activation is below this value in both phases -- due to the nature of the learning function being 0 when the sr coproduct is 0, it should not affect learning in any substantial way

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

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
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra rate constants for temporal derivatives in Leabra (Vm, net input)
INHERITED(SpecMemberBase)
public:
  float		integ;		// #DEF_1;0.5;0.001;0.0005 #MIN_0 overall rate constant for numerical integration -- affected by the timescale of the parameters and numerical stability issues -- typically 1 cycle = 1 ms, and if using ms normed units, this should be 1, otherwise 0.001 (1 ms in seconds) or possibly .5 or .0005 if there are stability issues
  float		vm;		// #DEF_0.1:0.357 #MIN_0 membrane potential rate constant -- reflects the capacitance of the neuron in principle -- biological default for AeEx spiking model C = 281 pF = 2.81 normalized = .356 rate constant
  float		net;		// #DEF_0.7 #MIN_0 net input time constant -- how fast to update net input (damps oscillations) -- generally reflects time constants associated with synaptic channels which are not modeled in the most abstract rate code models (set to 1 for detailed spiking models with more realistic synaptic currents)
  bool		midpoint;	// use the midpoint method in computing the vm value -- better avoids oscillations and allows a larger dt.vm parameter to be used -- this is critical to use with SPIKE mode
  float		d_vm_max;	// #DEF_100 #MIN_0 maximum change in vm at any timestep (limits blowup) -- this is a crude but effective safety valve for numerical integration problems (no longer necessary in gelin-based compuation)
  int		vm_eq_cyc;	// #AKA_cyc0_vm_eq #DEF_0 number of cycles to compute the vm as equilibirium potential given current inputs: set to 1 to quickly activate input layers; set to 100 to always use this computation
  float		vm_eq_dt;	// #DEF_1 #MIN_0 time constant for integrating the vm_eq values: how quickly to move toward the current eq value from previous vm value
  float		integ_time;	// #READ_ONLY #SHOW 1/integ rate constant = time constant for each cycle of updating for numerical integration
  float		vm_time;	// #READ_ONLY #SHOW 1/vm rate constant = time in cycles for vm to reach 1/e of asymptotic value
  float		net_time;	// #READ_ONLY #SHOW 1/net rate constant = time in cycles for net to reach 1/e of asymptotic value

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

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
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra rate constants for averaging over activations -- used in XCAL learning rules
INHERITED(SpecMemberBase)
public:
  bool          l_up_add;       // #DEF_true l_up is an additive constant, added every time the activation is above opt_thresh.send (in proportion to activity) -- if false, then it is multiplicative rate of approaching act -- key difference is that additive has no upper bound, so over-active units will keep increasing until the effects on the xcal BCM learning component cause it to be turned off
  bool          l_dn_pct;       // #DEF_true l_dn is multiplied by the layer's kwta.pct target activity level -- this means that lower expected activation layers decay less quickly, producing a rough normalization of the long-term running averages -- when using this, l_dn_dt defaults to .4 instead of .05
  float		l_up_dt;	// #DEF_0.2;0.25;0.6;0.006 [.25 std for l_up_add, else .6] rate constant for increases in long time-average activation (avg_l) -- if l_up_add then this is an additive increase for all units with activity > opt_thresh.send (in proportion to act), else it is rate constant for approaching act -- see l_up_add for more info
  float		l_dn_dt;	// #DEF_0.4;0.05;0.0005 [.4 std for l_dn_pct, else .05] rate constant for decreases in long time-average activation (avg_l) -- if l_dn_pct then this is multiplied by layer kwta.pct target activity (important to make that value accurate) 
  float		m_dt;		// #DEF_0.1;0.017 #MIN_0 #MAX_1 (only used for CTLEABRA_XCAL_C) time constant (rate) for continuous updating the medium time-scale avg_m value
  float		s_dt;		// #DEF_0.2;0.02 #MIN_0 #MAX_1 (only used for CTLEABRA_XCAL_C) time constant (rate) for continuously updating the short time-scale avg_s value
  float		ss_dt;		// #DEF_1;0.1;0.08 #MIN_0 #MAX_1 (only used for CTLEABRA_XCAL_C) time constant (rate) for continuously updating the super-short time-scale avg_ss value
  bool		use_nd;		// #DEF_false use the act_nd variables (non-depressed) for computing averages (else use raw act, which is raw spikes in spiking mode, and subject to depression if in place)

  float		l_time;		// #READ_ONLY #SHOW time constant (in trials for XCAL, cycles for XCAL_C, 1/l_dn_dt) for continuously updating the long time-scale avg_l value -- only for the down time as up is typically quite rapid
  float		m_time;		// #READ_ONLY #SHOW (only used for CTLEABRA_XCAL_C) time constant (in cycles, 1/m_dt) for continuously updating the medium time-scale avg_m value
  float		s_time;		// #READ_ONLY #SHOW (only used for CTLEABRA_XCAL_C) time constant (in cycles, 1/s_dt) for continuously updating the short time-scale avg_s value
  float		ss_time;	// #READ_ONLY #SHOW (only used for CTLEABRA_XCAL_C) time constant (in cycles, 1/ss_dt) for continuously updating the super-short time-scale avg_ss value

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

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
  float		e;		// Excitatory (glutamatergic synaptic sodium (Na) channel)
  float		l;		// Constant leak (potassium, K+) channel 
  float		i;		// inhibitory
  float		h;		// hysteresis (Ca)
  float		a;		// accomodation (k)

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  void 	Copy_(const LeabraChannels& cp); // used in units, so optimized copy needed
  TA_BASEFUNS(LeabraChannels);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(VChanSpec);

class E_API VChanSpec : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra voltage gated channel specs
INHERITED(taOBase)
public:
  bool		on;		// #DEF_false true if channel is on
  float		b_inc_dt;	// #CONDSHOW_ON_on #AKA_b_dt time constant for increases in basis variable (basis ~ intracellular calcium which builds up slowly as function of activation)
  float		b_dec_dt;	// #CONDSHOW_ON_on time constant for decreases in basis variable (basis ~ intracellular calcium which builds up slowly as function of activation)
  float		a_thr;		// #CONDSHOW_ON_on activation threshold of the channel: when basis > a_thr, conductance starts to build up (channels open)
  float		d_thr;		// #CONDSHOW_ON_on deactivation threshold of the channel: when basis < d_thr, conductance diminshes (channels close)
  float		g_dt;		// #CONDSHOW_ON_on time constant for changing conductance (activating or deactivating) -- if = 1, then gc is equal to the basis if channel is on
  bool		init;		// #CONDSHOW_ON_on initialize variables when state is intialized between trials (else with weights)
  bool		trl;		// #CONDSHOW_ON_on update after every trial instead of every cycle -- time constants need to be much larger in general

  void	UpdateBasis(float& basis, bool& on_off, float& gc, float act) {
    float del = act - basis;
    if(del > 0.0f)
      basis += b_inc_dt * del;
    else
      basis += b_dec_dt * del;
    if(basis > a_thr)
      on_off = true;
    if(on_off && (basis < d_thr))
      on_off = false;
    if(g_dt == 1.0f)
      gc = (on_off) ? basis : 0.0f;
    else
      gc += g_dt * ((float)on_off - gc);
  }

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(VChanSpec);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(MaxDaSpec);

class E_API MaxDaSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra how to compute the maxda value, which serves as a stopping criterion for settling
INHERITED(SpecMemberBase)
public:
  enum dAValue {
    NO_MAX_DA,			// these units do not contribute to maxda computation at all -- value always zero
    DA_ONLY,			// just use da
    INET_ONLY,			// just use inet
    INET_DA			// use inet if no activity, then use da
  };

  dAValue	val;		// #DEF_INET_DA value to use for computing delta-activation (change in activation over cycles of settling).
  float		inet_scale;	// #DEF_1 #MIN_0 how to scale the inet measure to be like da
  float		lay_avg_thr;	// #DEF_0.01 #MIN_0 threshold for layer average activation to switch to da fm Inet

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(MaxDaSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
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

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

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

  override String       GetTypeDecoKey() const { return "UnitSpec"; }

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
  LeabraActFunExSpec act;       // #CAT_Activation activation function parameters -- very important for determining the shape of the selected act_fun
  bool               act_lrn_on; // #CAT_Activation use different activation function parameters for the variables that enter into learning equations, compared to what is used for processing (enables act_lrn params on next line)
  LeabraActFunSpec   act_lrn;   // #CONDSHOW_ON_act_lrn_on #CAT_Activation learning activations -- potentially different parameters for activations that drive learning vs. those that drive processing -- allows learning to be higher contrast while processing is lower processing -- more graded -- these act_lrn activations only participate in XCAL learning, not in CHL
  SpikeFunSpec	spike;		// #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation spiking function specs (only for act_fun = SPIKE)
  SpikeMiscSpec	spike_misc;	// #CONDSHOW_ON_act_fun:SPIKE #CAT_Activation misc extra spiking function specs (only for act_fun = SPIKE)
  OptThreshSpec	opt_thresh;	// #CAT_Learning optimization thresholds for speeding up processing when units are basically inactive
  MaxDaSpec	maxda;		// #CAT_Activation maximum change in activation (da) computation -- regulates settling
  MinMaxRange	clamp_range;	// #CAT_Activation range of clamped activation values (min, max, 0, .95 std), don't clamp to 1 because acts can't reach, so .95 instead
  MinMaxRange	vm_range;	// #CAT_Activation membrane potential range (min, max, 0-2 for normalized)
  RandomSpec	v_m_init;	// #CAT_Activation what to initialize the membrane potential to (mean = .3, var = 0 std)
  RandomSpec	act_init;	// #CAT_Activation what to initialize the activation to (mean = 0 var = 0 std)
  LeabraDtSpec	dt;		// #CAT_Activation time constants (rate of updating): membrane potential (vm) and net input (net)
  LeabraActAvgSpec act_avg;	// #CAT_Activation time constants (rate of updating) for computing activation averages -- used in XCAL learning rules
  LeabraChannels g_bar;		// #CAT_Activation [Defaults: 1, .1, 1, .1, .5] maximal conductances for channels
  LeabraChannels e_rev;		// #CAT_Activation [Defaults: 1, .3, .25, 1, 0] reversal potentials for each channel
  VChanSpec	hyst;		// #CAT_Activation [Defaults: .05, .8, .7, .1] hysteresis (excitatory) v-gated chan (Ca2+, NMDA)
  VChanSpec	acc;		// #CAT_Activation [Defaults: .01, .5, .1, .1] accomodation (inhibitory) v-gated chan (K+)
  ActAdaptSpec 	adapt;		// #CAT_Activation activation-driven adaptation factor that drives spike rate adaptation dynamics based on both sub- and supra-threshold membrane potentials
  DepressSpec	depress;	// #CAT_Activation depressing synapses specs -- multiplies activation value by a spike amplitude/probability value that depresses with use and recovers exponentially
  SynDelaySpec	syn_delay;	// #CAT_Activation synaptic delay -- if active, activation sent to other units is delayed by a given amount
  DaModSpec	da_mod;		// #CAT_Learning da modulation of activations (for da-based learning, and other effects)
  NoiseType	noise_type;	// #CAT_Activation where to add random noise in the processing (if at all)
  RandomSpec	noise;		// #CONDSHOW_OFF_noise_type:NO_NOISE #CAT_Activation distribution parameters for random added noise
  NoiseAdaptSpec noise_adapt;	// #CONDSHOW_OFF_noise_type:NO_NOISE #CAT_Activation how to adapt the noise variance (var) value
  Schedule	noise_sched;	// #CONDSHOW_OFF_noise_type:NO_NOISE #CAT_Activation schedule of noise variance -- time scale depends on noise_adapt parameter (cycles, epochs, etc)

  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution
  FunLookup	lrn_nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	lrn_noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  virtual void	SetLearnRule(LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  override void Init_Weights(Unit* u, Network* net);
  override void	Init_Acts(Unit* u, Network* net);
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

  virtual void	SetCurLrate(LeabraNetwork* net, int epoch);
  // #CAT_Learning set current learning rate based on epoch

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
  virtual void 	Send_NetinDelta_Post(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation send netinput post-processing -- integrate deltas across multiple threads and deal with NETIN_PER_PRJN
  virtual void	Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
    virtual void Compute_NetinInteg_Spike_e(LeabraUnit* u, LeabraNetwork* net);
    // #IGNORE called by Compute_NetinInteg for spiking units: compute actual excitatory netin conductance value for spiking units by integrating over spike
    virtual void Compute_NetinInteg_Spike_i(LeabraUnit* u, LeabraNetwork* net);
    // #IGNORE called by Compute_NetinInteg for spiking units: compute actual inhibitory netin conductance value for spiking units by integrating over spike

    virtual float Compute_IThresh(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation called by Compute_NetinInteg: compute inhibitory value that would place unit directly at threshold
      inline float Compute_IThreshStd(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, using all currents EXCEPT bias.wt
      inline float Compute_IThreshNoA(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, excluding gc.a current
      inline float Compute_IThreshNoH(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, excluding gc.a current
      inline float Compute_IThreshNoAH(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, excluding any gc.a, gc.h currents
      inline float Compute_IThreshNoAHB(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, excluding any gc.a, gc.h currents, not subtracting bias weights
      inline float Compute_IThreshAll(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, using all currents INCLUDING bias.wt
      inline float Compute_IThreshNetinOnly(float netin);
      // #IGNORE called by Compute_IThresh: compute inhibitory value that would place unit directly at threshold, using only the provided net input value, along with the g_bar.l leak current in the unit spec

  inline float Compute_EThresh(LeabraUnit* u);
  // #CAT_Activation #IGNORE compute excitatory value that would place unit directly at threshold
  inline float Compute_EThreshLrn(LeabraUnit* u);
  // #CAT_Activation #IGNORE compute excitatory value that would place unit directly at threshold -- uses act_lrn.thr

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  inline void	Compute_SelfInhib(LeabraUnit* u, LeabraLayerSpec* lspec, 
                                   LeabraNetwork* net);
  // #CAT_Activation #IGNORE compute self inhibition value, for fffb g_i_self inhib val
  inline void	Compute_ApplyInhib(LeabraUnit* u, LeabraLayerSpec* lspec, 
                                   LeabraNetwork* net, float inhib_val);
  // #CAT_Activation #IGNORE apply computed (kwta) inhibition value to unit inhibitory conductance
  inline void	Compute_ApplyInhib_LoserGain(LeabraUnit* u, LeabraLayerSpec* lspec, 
                                             LeabraNetwork* net,
                                             float inhib_thr,
					     float inhib_top, float inhib_loser);
  // #CAT_Activation #IGNORE apply computed (kwta) inhibition value to unit inhibitory conductance -- when eff_loser_gain in effect


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls all the various sub-functions
  override void	Compute_Act(Unit* u, Network* net, int thread_no=-1);

    virtual void Compute_Conduct(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 1: compute input conductance values in the gc variables
      virtual void Compute_DaMod_PlusCont(LeabraUnit* u, LeabraNetwork* net);
      // #IGNORE called by Compute_Conduct compute da modulation as plus-phase continuous gc.h/.a

    virtual void Compute_Vm(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 2: compute the membrane potential from input conductances
      inline float Compute_EqVm(LeabraUnit* u);
      // #CAT_Activation #IGNORE compute the equilibrium (asymptotic) membrante potential from input conductances (assuming they remain fixed as they are)

    virtual void Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 2: compute the activation from membrane potential (actually g_e in case of gelin)
      virtual void Compute_ActFmVm_rate(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation from membrane potential (actually g_e in case of gelin) -- rate code functions
      virtual void Compute_ActLrnFmVm_rate(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the act_lrn learning value from membrane potential (actually g_e in case of gelin) -- rate code functions
      virtual float Compute_ActValFmVmVal_rate_impl(float val_sub_thr,
                         LeabraActFunSpec& act_spec, FunLookup& nxx1_fl);
        // #CAT_Activation raw activation function: computes an activation value from given value subtracted from its relevant threshold value
      inline float Compute_ActValFmVmVal_rate(float val_sub_thr) 
      { return Compute_ActValFmVmVal_rate_impl(val_sub_thr, act, nxx1_fun); }
      // #CAT_Activation raw activation function: computes an activation value from given value subtracted from its relevant threshold value
      virtual void Compute_ActAdapt_rate(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation-based adaptation value based on activation (spiking rate) and membrane potential -- rate code functions

      virtual void Compute_ActFmVm_spike(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation from membrane potential -- discrete spiking
      virtual void Compute_ActAdapt_spike(LeabraUnit* u, LeabraNetwork* net);
      // #CAT_Activation compute the activation-based adaptation value based on spiking and membrane potential -- spike functions
      virtual void Compute_ClampSpike(LeabraUnit* u, LeabraNetwork* net, float spike_p);
      // #CAT_Activation compute spiking activation according to spike.clamp_type with given probability (typically spike.clamp_max_p * u->ext) -- includes depression and other active factors as done in Compute_ActFmVm_spike -- used for hard clamped inputs in spiking nets
    virtual void Compute_SelfReg_Cycle(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation Act Step 3: compute self-regulatory currents (hysteresis, accommodation) -- at the cycle time scale
    virtual void Compute_SelfReg_Trial(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation compute self-regulatory currents (hysteresis, accommodation) -- at the trial time scale

    virtual float Compute_Noise(LeabraUnit* u, LeabraNetwork* net);
    // #CAT_Activation utility fun to generate and return the noise value based on current settings -- will set unit->noise value as appropriate (generally excludes effect of noise_sched schedule)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual float Compute_MaxDa(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Activation statistic function to compute the maximum delta-activation (change in activation); used to control settling -- not threadable as it is used to update net, layer, and Inhib values

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
  //	LeabraTI

  virtual void	TI_Send_CtxtNetin(LeabraUnit* u, LeabraNetwork* net,
                                  int thread_no=-1);
  // #CAT_LeabraTI send context netinputs through LeabraTICtxtConSpec connections
  virtual void	TI_Send_CtxtNetin_Post(LeabraUnit* u, LeabraNetwork* net);
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

  virtual void 	Compute_SRAvg(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabraX/CAL)

  virtual void 	Compute_dWt_FirstMinus(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute weight change after first minus phase has been encountered: for out-of-phase LeabraTI context layers (or anything similar)
  virtual void 	Compute_dWt_FirstPlus(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  virtual void	Compute_dWt_Nothing(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules
  virtual void	Compute_dWt_Norm(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute normalization of dwt values -- must be done as a separate stage after dwt

  override void	Compute_Weights(Unit* u, Network* net, int thread_no=-1);

  virtual void	Compute_StableWeights(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Learning compute the stable weights for connections that have separate stable weights (see LeabraStableConSpec) -- simulates synaptic consolidation to gene-expression-dependent long term plasticity -- typically done after every epoch or multiple thereof

  ///////////////////////////////////////////////////////////////////////
  //	Stats

  override float Compute_SSE(Unit* u, Network* net, bool& has_targ);
  override bool	 Compute_PRerr(Unit* u, Network* net, float& true_pos, float& false_pos, float& false_neg);
  virtual float  Compute_NormErr(LeabraUnit* u, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off of act_m vs target) according to settings on the network (returns a 1 or 0) -- if (net->on_errs && act_m > .5 && targ < .5) return 1; if (net->off_errs && act_m < .5 && targ > .5) return 1; else return 0
  virtual float  Compute_M2SSE(LeabraUnit* u, LeabraNetwork* net, bool& has_targ);
  // #CAT_Statistic compute sum-squared error of target compared to act_m2 instead of act_m 

  ///////////////////////////////////////////////////////////////////////
  //	Misc Housekeeping, non Compute functions

  virtual void	CreateNXX1Fun(LeabraActFunSpec& act_spec, FunLookup& nxx1_fl,
                              FunLookup& noise_fl);
  // #CAT_Activation create convolved gaussian and x/x+1 function lookup tables

  virtual void 	BioParams(bool gelin=true, float norm_sec=0.001f, float norm_volt=0.1f, float volt_off=-0.1f, float norm_amp=1.0e-8f,
	  float C_pF=281.0f, float gbar_l_nS=10.0f, float gbar_e_nS=100.0f, float gbar_i_nS=100.0f,
	  float erev_l_mV=-70.0f, float erev_e_mV=0.0f, float erev_i_mV=-75.0f,
	  float act_thr_mV=-50.0f, float spk_thr_mV=20.0f, float exp_slope_mV=2.0f,
	  float adapt_dt_time_ms=144.0f, float adapt_vm_gain_nS=4.0f, float adapt_spk_gain_nA=0.0805);
  // #BUTTON set parameters based on biologically-based values, using normalization scaling to convert into typical Leabra standard parameters.  gelin = configure for gelin rate-code activations instead of discrete spiking (dt = 0.3, gain = 80, gelin flags on), norm_x are normalization values to convert from SI units to normalized values (defaults are 1ms = .001 s, 100mV with -100 mV offset to bring into 0-1 range between -100..0 mV, 1e-8 amps (makes g_bar, C, etc params nice).  other defaults are based on the AdEx model of Brette & Gurstner (2005), which the SPIKE mode implements exactly with these default parameters -- last bit of name indicates the units in which this value must be provided (mV = millivolts, ms = milliseconds, pF = picofarads, nS = nanosiemens, nA = nanoamps)

  virtual void	GraphVmFun(DataTable* graph_data, float g_i = .5, float min = 0.0, float max = 1.0, float incr = .01);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph membrane potential (v_m) as a function of excitatory net input (net) for given inhib conductance (g_i) (NULL = new graph data)
  virtual void	GraphActFmVmFun(DataTable* graph_data, float min = .15, float max = .50, float incr = .001);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the activation function as a function of membrane potential (v_m) (NULL = new graph data) -- note: only valid if act.gelin = false -- if act.gelin is true, then use GraphActFmNetFun instead
  virtual void	GraphActFmNetFun(DataTable* graph_data, float g_i = .5, float min = 0.0,
				 float max = 1.0, float incr = .001, float g_e_thr = 0.5,
				 float lin_gain = 10);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the activation function as a function of net input -- if act.gelin = true then this is the direct activation function, computed relative to the g_e_thr threshold value provided -- otherwise, the net input value is projected through membrane potential vm to get the net overall activation function -- a linear comparison with lin_gain slope is also provided for reference -- always computed as lin_gain * (net - g_e_thr) (NULL = new graph data)
  virtual void	GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha=false);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the spike alpha function for conductance integration over time window given in spike parameters -- last data point is the sum over the whole window (total conductance of a single spike) -- force_alpha means use explicit alpha function even when rise=0 (otherewise it simulates actual recursive exp decay used in optimized code)
//   virtual void	GraphSLNoiseAdaptFun(DataTable* graph_data, float incr = 0.05f);
//   // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the short and long-term noise adaptation function, which integrates both short-term and long-term performance values
  virtual void TimeExp(int mode, int nreps=100000000);
  // #MENU_BUTTON #MENU_ON_Graph ime how long it takes to compute various forms of exp() function: mode=0 = double sum ctrl (baseline), mode=1 = std double exp(), mode=2 = taMath_double::exp_fast, mode=3 = float sum ctrl (float baseline), mode=4 = expf, mode=5 = taMath_float::exp_fast -- this is the dominant cost in spike alpha function computation, so we're interested in optimizing it..

  override bool  CheckConfig_Unit(Unit* un, bool quiet=false);

  void	InitLinks();
  SIMPLE_COPY(LeabraUnitSpec);
  TA_BASEFUNS(LeabraUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();	// to set _impl sig
  void 	CheckThisConfig_impl(bool quiet, bool& rval);

  LeabraChannels e_rev_sub_thr;	// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN e_rev - act.thr for each item -- used for compute_ithresh
  LeabraChannels e_rev_sub_thr_lrn; // #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN e_rev - act_lrn.thr for each item -- used for compute_ithresh
  float		thr_sub_e_rev_i;// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.i * (act.thr - e_rev.i) used for compute_ithresh
  float		thr_sub_e_rev_e;// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.e * (act.thr - e_rev.e) used for compute_ethresh
  float		lrn_thr_sub_e_rev_e;// #CAT_Activation #READ_ONLY #NO_SAVE #HIDDEN g_bar.e * (act_lrn.thr - e_rev.e) used for compute_ethreshlrn

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(LeabraUnitSpec);

#endif // LeabraUnitSpec_h
