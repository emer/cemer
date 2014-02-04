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

#ifndef LeabraUnit_h
#define LeabraUnit_h 1

// parent includes:
#include <Unit>

// member includes:
#include <LeabraUnitSpec>
#include <LeabraRecvCons>
#include <LeabraSendCons>
#include <LeabraBiasSpec>
#include <float_CircBuffer>

// declare all other types mentioned but not required to include:
class LeabraLayer; // 
class SynDelaySpec; // 
class LeabraNetwork; // 

eTypeDef_Of(VChanBasis);

class E_API VChanBasis : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER basis variables for vchannels
INHERITED(taBase)
public:
  float		hyst;		// hysteresis
  float		acc;		// fast accomodation
  bool		hyst_on;	// #NO_VIEW binary thresholded mode state variable, hyst
  bool		acc_on;		// #NO_VIEW binary thresholded mode state variable, acc
  float		g_h;		// #NO_VIEW hysteresis conductance
  float		g_a;		// #NO_VIEW accomodation conductance

  String       GetTypeDecoKey() const override { return "Unit"; }

  void 	Copy_(const VChanBasis& cp);
  TA_BASEFUNS(VChanBasis);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(LeabraUnitChans);

class E_API LeabraUnitChans : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra channels used in Leabra units
INHERITED(taBase)
public:
  float		l;		// leak
  float		i;		// #DMEM_SHARE_SET_1 inhibitory
  float		h;		// hysteresis (Ca)
  float		a;		// accomodation (K)

  String       GetTypeDecoKey() const override { return "Unit"; }

  void 	Copy_(const LeabraUnitChans& cp);
  TA_BASEFUNS(LeabraUnitChans);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(LeabraUnit);

class E_API LeabraUnit : public Unit {
  // #STEM_BASE ##DMEM_SHARE_SETS_5 ##CAT_Leabra Leabra unit, point-neuron approximation
INHERITED(Unit)
public:
  float		act_eq;		// #VIEW_HOT #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act)
  float		act_nd;		// #CAT_Activation non-depressed rate-code equivalent activity value (time-averaged spikes or just act) -- used for final phase-based variables used in learning and stats
  float         act_lrn;        // #VIEW_HOT #CAT_Activation activation for learning -- potentially higher contrast than that for processing -- only contributes to XCAL learning, not CHL
  float		act_avg;	// #CAT_Activation average activation (of final plus phase activation state) over long time intervals (dt = act.avg_dt)
  float		act_m;		// #VIEW_HOT #CAT_Activation minus_phase activation (act_nd), set after settling, used for learning and performance stats 
  float		act_p;		// #VIEW_HOT #CAT_Activation plus_phase activation (act_nd), set after settling, used for learning and performance stats
  float		act_dif;	// #VIEW_HOT #CAT_Activation difference between plus and minus phase acts, gives unit err contribution
  float		act_m2;		// #CAT_Activation second minus_phase (e.g., nothing phase) activation (act_nd), set after settling, used for learning and performance stats
  float		act_mid;	// #CAT_Activation mid minus_phase -- roughly half-way through minus phase -- used in hippocampal ThetaPhase (for auto-encoder CA1 training) 
  float		act_dif2;	// #CAT_Activation difference between second set of phases, where relevant (e.g., act_p - act_m2 for MINUS_PLUS_NOTHING)
  float		da;		// #NO_SAVE #CAT_Activation delta activation: change in act from one cycle to next, used to stop settling
  float		avg_ss;		// #CAT_Activation super-short time-scale activation average -- provides the lowest-level time integration, important specifically for spiking networks using the XCAL_C algorithm -- otherwise ss_dt = 1 and this is just the current activation
  float		avg_s;		// #CAT_Activation short time-scale activation average -- tracks the most recent activation states, and represents the plus phase for learning in XCAL algorithms
  float		avg_m;		// #CAT_Activation medium time-scale activation average -- integrates over entire trial of activation, and represents the minus phase for learning in XCAL algorithms
  float		avg_l;		// #CAT_Activation long time-scale average of medium-time scale (trial level) activation, used for the BCM-style floating threshold in XCAL
  float         act_ctxt;       // #VIEW_HOT #CAT_Activation leabra TI context activation value -- computed from LeabraTICtxtConspec connection when network ti_mode is on
  float         net_ctxt;       // #CAT_Activation leabra TI context netinput value for computing act_ctxt -- computed from LeabraTICtxtConspec connection when network ti_mode is on
  float         p_act_p;        // #CAT_Activation prior trial act_p value -- needed for leabra TI context weight learning in the LeabraTICtxtConspec connection
  float		davg;		// #CAT_Activation delta average activation -- computed from changes in the short time-scale activation average (avg_s) -- used for detecting jolts or transitions in the network, to drive learning
  VChanBasis	vcb;		// #CAT_Activation voltage-gated channel basis variables
  LeabraUnitChans gc;		// #DMEM_SHARE_SET_1 #NO_SAVE #CAT_Activation current unit channel conductances
  float		I_net;		// #NO_SAVE #CAT_Activation net current produced by all channels
  float		v_m;		// #NO_SAVE #CAT_Activation membrane potential
  float		vm_dend;	// #NO_SAVE #CAT_Activation dendritic membrane potential -- reflects back-propagated spike values in spiking mode -- these are not subject to immediate AHP and thus decay exponentially, and are used for learning
  float		adapt;		// #NO_SAVE #CAT_Activation adaptation factor -- driven by both sub-threshold membrane potential and spiking activity -- subtracts directly from the membrane potential on every time step
  float		noise;		// #NO_SAVE #CAT_Activation noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  float 	dav;		// #VIEW_HOT #CAT_Activation dopamine value (da is delta activation) which modulates activations (e.g., via accom and hyst currents) to then drive learning
  float 	sev;		// #CAT_Activation serotonin value 

  bool		in_subgp;	// #READ_ONLY #NO_SAVE #CAT_Structure determine if unit is in a subgroup
  float		net_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation total netinput scaling basis
  float		bias_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation bias weight scaling factor

  float		act_sent;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last activation value sent (only send when diff is over threshold)
  float		net_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw net input received from sending units (send delta delta's are added to this value)
  float		net_delta;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation delta net input received from sending units -- only used for non-threaded case
  float		g_i_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw inhib net input received from sending units (increments the deltas in send_delta)
  float		g_i_delta;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation delta inhibitory net input received from sending units
  float		g_i_syn;	// #NO_SAVE #CAT_Activation aggregated synaptic inhibition (from inhib connections) -- time integral of g_i_raw -- this is added with layer-level inhibition (fffb, kwta) to get the full inhibition in gc.i
  float         g_i_self;       // #NO_SAVE #CAT_Activation time-averaged self inhibition value -- needs to be separate variable to allow time-averaging to prevent severe oscillations

  float		i_thr;		// #NO_SAVE #CAT_Activation inhibitory threshold value for computing kWTA
  float		spk_amp;	// #CAT_Activation amplitude/probability of spiking output (for synaptic depression function if unit spec depress.on is on)
  float		misc_1;		// #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  int		spk_t;		// #NO_SAVE #CAT_Activation time in ct_cycle units when spiking last occurred (-1 for not yet)
  float_CircBuffer* act_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of activation states for synaptic delay computation
  float_CircBuffer* spike_e_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of excitatory net input from spikes for synaptic integration over discrete spikes
  float_CircBuffer* spike_i_buf; // #NO_VIEW #NO_SAVE #CAT_Activation buffer of inhibitory net input from spikes for synaptic integration over discrete spikes

  inline void	AddToActBuf(SynDelaySpec& sds) {
    if(sds.on) act_buf->CircAddLimit(act, sds.delay);
  }
  // add current activation to act buf if synaptic delay is on

  inline LeabraLayer*	own_lay() const {return (LeabraLayer*)inherited::own_lay();}

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	Init_ActAvg(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_ActAvg(this, net); }
  // #CAT_Activation initialize average activation

  void Init_Netins(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Init_Netins(this, net); }
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
  void DecayState(LeabraNetwork* net, float decay)
  { ((LeabraUnitSpec*)GetUnitSpec())->DecayState(this, net, decay); }
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  void	Trial_Init_Unit(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Trial_Init_Unit(this, net, thread_no); }
  // #CAT_Activation trial unit-level initialization functions: DecayState, NoiseInit, Trial_Init_SRAvg

    void Trial_DecayState(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Trial_DecayState(this, net); }
    // #CAT_Activation decay activation states towards initial values: at trial-level boundary
    void Trial_NoiseInit(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Trial_NoiseInit(this, net); }
    // #CAT_Activation initialize noise at trial level
    void Trial_Init_SRAvg(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Trial_Init_SRAvg(this, net); }	  
    // #CAT_Learning reset the sender-receiver coproduct average -- do at start of trial

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  void	Settle_Init_Unit(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Settle_Init_Unit(this, net, thread_no); }
  // #CAT_Activation settle unit-level initialization functions: Init_TargFlags, DecayState, NetinScale

    void Settle_Init_TargFlags(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Settle_Init_TargFlags(this, net); }
    // #CAT_Activation initialize external input flags based on phase
    void Settle_DecayState(LeabraNetwork* net, int)
    { ((LeabraUnitSpec*)GetUnitSpec())->Settle_DecayState(this, net); }
    // #CAT_Activation decay activation states towards initial values: at phase-level boundary at start of settling
    void Compute_NetinScale(LeabraNetwork* net, int)
    { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinScale(this, net); }
    // #CAT_Activation compute net input scaling values

  void Compute_NetinScale_Senders(LeabraNetwork* net, int)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinScale_Senders(this, net); }
  // #CAT_Activation compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Settle_Init_Unit stage after all the recv ones are computed

  void Compute_HardClamp(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClamp(this, net); }
    // #CAT_Activation force units to external values provided by environment: also optimizes settling by only computing netinput once
  void	Compute_HardClampNoClip(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClampNoClip(this, net); }
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for second plus phase clamping to settled values)

  void	ExtToComp(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->ExtToComp(this, net); }
  // #CAT_Activation change external inputs to comparisons (remove input)
  void	TargExtToComp(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->TargExtToComp(this, net); }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  void	Send_NetinDelta(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_NetinDelta(this, net, thread_no); }
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold -- only this delta form is supported
  void	Send_NetinDelta_Post(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_NetinDelta_Post(this, net); }
  // #CAT_Activation send netinput post-processing -- integrate deltas across multiple threads and deal with NETIN_PER_PRJN
  void	Compute_NetinInteg(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinInteg(this, net, thread_no); }
  // #CAT_Activation integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
  float Compute_IThresh(LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_IThresh(this, net); }
  // #CAT_Activation called by Compute_NetinInteg: compute inhibitory value that would place unit directly at threshold
  float Compute_EThresh()
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_EThresh(this); }
  // #CAT_Activation #IGNORE compute excitatory value that would place unit directly at threshold

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  void	Compute_ApplyInhib(LeabraLayerSpec* lspec, LeabraNetwork* net, float inhib_val)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_ApplyInhib(this, lspec, net, inhib_val); }
  // #CAT_Activation #IGNORE apply computed inhibitory value (kwta) to unit inhibitory conductance
  void	Compute_ApplyInhib_LoserGain(LeabraLayerSpec* lspec, LeabraNetwork* net,
                                     float inhib_thr,
				     float inhib_top, float inhib_loser)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_ApplyInhib_LoserGain(this, lspec, net,
 inhib_thr, inhib_top, inhib_loser); }
  // #CAT_Activation #IGNORE apply computed inhibitory value (kwta) to unit inhibitory conductance -- when eff_loser_gain in effect

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec

  float Compute_ActValFmVmVal_rate(float val_sub_thr)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_ActValFmVmVal_rate(val_sub_thr); }
  // #CAT_Activation raw activation function: computes an activation value from given value subtracted from its relevant threshold value

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  float	Compute_MaxDa(LeabraNetwork* net) 
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_MaxDa(this, net); }
  // #CAT_Activation compute the maximum delta-activation (change in activation); used to control settling

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  void	Compute_MidMinus(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_MidMinus(this, net); }
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycle parameter, if > 0 -- currently used for the PBWM algorithm and hippocampus -- stores act_mid

  void	Compute_CycSynDep(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_CycSynDep(this, net, thread_no); }
  // #CAT_Activation compute cycle-level synaptic depression (must be defined by appropriate subclass) -- called at end of each cycle of computation if net_misc.cyc_syn_dep is on.

  ///////////////////////////////////////////////////////////////////////
  //	Settle Final

  void	PostSettle(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->PostSettle(this, net); }
  // #CAT_Activation set stuff after settling is over (act_m, act_p etc)


  ///////////////////////////////////////////////////////////////////////
  //	LeabraTI

  void	TI_Send_CtxtNetin(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Send_CtxtNetin(this, net, thread_no); }
  // #CAT_TI send context netinputs through LeabraTICtxtConSpec connections
  void	TI_Send_CtxtNetin_Post(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Send_CtxtNetin_Post(this, net); }
  // #CAT_TI send context netinputs through LeabraTICtxtConSpec connections -- post processing rollup
  void	TI_Compute_CtxtAct(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Compute_CtxtAct(this, net); }
  // #CAT_TI compute context activations
  void	TI_ClearContext(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_ClearContext(this, net); }
  // #CAT_TI clear the act_ctxt and net_ctxt context variables -- can be useful to do at clear discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //	Trial Final

  void 	EncodeState(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->EncodeState(this, net); }
  // #CAT_Learning encode current state information at end of trial (hook for time-based learning)
  void 	Compute_SelfReg_Trial(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_SelfReg_Trial(this, net); }
  // #CAT_Activation compute self-regulation (accommodation, hysteresis) at end of trial


  ///////////////////////////////////////////////////////////////////////
  //	Learning

  void	Compute_SRAvg(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_SRAvg(this, net, thread_no); }
  // #CAT_Learning compute sending-receiving activation product averages (CtLeabra_X/CAL)

  void 	Compute_dWt_FirstMinus(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_FirstMinus(this, net, thread_no); }
  // #CAT_Learning compute weight change after first minus phase has been encountered: not currently used.. 
  void 	Compute_dWt_FirstPlus(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_FirstPlus(this, net, thread_no); }
  // #CAT_Learning compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  void 	Compute_dWt_Nothing(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_Nothing(this, net, thread_no); }
  // #CAT_Learning compute weight change after final nothing phase: standard layers do a weight change here under both learning rules
  void 	Compute_dWt_Norm(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_Norm(this, net, thread_no); }
  // #CAT_Learning compute normalization of dwt values -- must be done as a separate stage after dwt
  void 	Compute_StableWeights(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_StableWeights(this, net, thread_no); }
  // #CAT_Learning compute the stable weights for connections that have separate stable weights (see LeabraStableConSpec) -- simulates synaptic consolidation to gene-expression-dependent long term plasticity -- typically done after every epoch or multiple thereof


  ///////////////////////////////////////////////////////////////////////
  //	Stats

  float	Compute_NormErr(LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_NormErr(this, net); }
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off from target) according to settings on the network (returns a 1 or 0)
  float	Compute_M2SSE(LeabraNetwork* net, bool& has_targ)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_M2SSE(this, net, has_targ); }
  // #CAT_Statistic compute sum-squared error based on act_m2 value vs. target

  ///////////////////////////////////////////////////////////////////////
  //	Misc Housekeeping, non Compute functions

  void	GetInSubGp();
  bool	BuildUnits() override;

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LeabraUnit& cp);
  TA_BASEFUNS(LeabraUnit);
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
};

#ifndef __MAKETA__
typedef void (LeabraUnit::*LeabraUnitMethod)(LeabraNetwork*, int);
// this is required to disambiguate unit thread method guys -- double casting
#endif 


#endif // LeabraUnit_h
