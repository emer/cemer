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
  float		i;		// inhibitory
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
  // #STEM_BASE ##CAT_Leabra Leabra unit, point-neuron approximation
INHERITED(Unit)
public:
  float		act_eq;		// #VIEW_HOT #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act)
  float		act_nd;		// #VIEW_HOT #CAT_Activation non-depressed rate-code equivalent activity value (time-averaged spikes or just act) -- used for final phase-based variables used in learning and stats
  float         act_lrn;        // #CAT_Activation activation for learning -- potentially higher contrast than that for processing -- only contributes to XCAL learning, not CHL
  float		act_avg;	// #CAT_Activation average activation (of final plus phase activation state) over long time intervals (dt = act.avg_dt)
  float		act_m;		// #VIEW_HOT #CAT_Activation minus_phase activation (act_nd), set after settling, used for learning and performance stats 
  float		act_p;		// #VIEW_HOT #CAT_Activation plus_phase activation (act_nd), set after settling, used for learning and performance stats
  float		act_dif;	// #VIEW_HOT #CAT_Activation difference between plus and minus phase acts, gives unit err contribution
  float		act_mid;	// #CAT_Activation mid minus_phase -- roughly half-way through minus phase -- used in hippocampal ThetaPhase (for auto-encoder CA1 training) 
  float		da;		// #NO_SAVE #CAT_Activation delta activation: change in act from one cycle to next, used to stop settling
  float		avg_ss;		// #CAT_Activation super-short time-scale activation average -- provides the lowest-level time integration, important specifically for spiking networks using the XCAL_C algorithm -- otherwise ss_dt = 1 and this is just the current activation
  float		avg_s;		// #CAT_Activation short time-scale activation average -- tracks the most recent activation states, and represents the plus phase for learning in XCAL algorithms
  float		avg_m;		// #CAT_Activation medium time-scale activation average -- integrates over entire trial of activation, and represents the minus phase for learning in XCAL algorithms
  float		avg_l;		// #CAT_Activation long time-scale average of medium-time scale (trial level) activation, used for the BCM-style floating threshold in XCAL
  float 	thal;		// #VIEW_HOT #CAT_Activation thalamic activation value, driven by a ThalamicLayerSpec -- used by thal params in LeabraUnitSpec and possibly other specs to respond to thalamic inputs
  float         deep5b;         // #VIEW_HOT #CAT_Activation deep layer 5b activation values -- these reflect the output gating signal modulated by the thal and act values of the unit (microcolumn) -- deep5b is thresholded and represents the active foreground channel of information, while act reflects superficial layer 2/3 background information
  float         deep5b_net;     // #CAT_Activation net input from deep layer 5b activation values
  float         act_ctxt;       // #VIEW_HOT #CAT_Activation leabra TI context activation value -- computed from LeabraTICtxtConspec connection when network ti_mode is on -- this is equivalent to net_ctxt except for PFC units, where gating and other factors modulate it
  float         net_ctxt;       // #CAT_Activation leabra TI context netinput value for computing act_ctxt -- computed from LeabraTICtxtConspec connection when network ti_mode is on -- computed from sending act or deep5b values (see cifer flags), which then project to layer 6 which integrates the contextualized value
  float         p_act_p;        // #CAT_Activation prior trial act_p value -- needed for leabra TI context weight learning in the LeabraTICtxtConspec connection
  float		davg;		// #CAT_Activation delta average activation -- computed from changes in the short time-scale activation average (avg_s) -- used for detecting jolts or transitions in the network, to drive learning
  VChanBasis	vcb;		// #CAT_Activation voltage-gated channel basis variables
  LeabraUnitChans gc;		// #NO_SAVE #CAT_Activation current unit channel conductances
  float		I_net;		// #NO_SAVE #CAT_Activation net current produced by all channels
  float		v_m;		// #NO_SAVE #CAT_Activation membrane potential
  float		vm_dend;	// #NO_SAVE #CAT_Activation dendritic membrane potential -- reflects back-propagated spike values in spiking mode -- these are not subject to immediate AHP and thus decay exponentially, and are used for learning
  float		adapt;		// #NO_SAVE #CAT_Activation adaptation factor -- driven by both sub-threshold membrane potential and spiking activity -- subtracts directly from the membrane potential on every time step
  float		noise;		// #NO_SAVE #CAT_Activation noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  float 	dav;		// #VIEW_HOT #CAT_Activation dopamine value (da is delta activation) which modulates activations (e.g., via accom and hyst currents) to then drive learning
  float 	sev;		// #CAT_Activation serotonin value 

  float		net_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation total netinput scaling basis
  float		bias_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation bias weight scaling factor
  float		ctxt_scale;	// #NO_SAVE #EXPERT #CAT_Activation total context netinput weight scaling factor -- computed from TI context projections, and used for auto-rescaling of act_ctxt in plus phase to keep proportions the same

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
  inline LeabraNetwork*	own_net() const {return (LeabraNetwork*)inherited::own_net();}

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

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  void  Compute_Act_l(LeabraNetwork* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Act(this, (Network*)net, thread_no); }
  // #CAT_Activation compute activation value: what we send to others -- leabra version

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

  void	TI_Compute_Deep5bAct(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Compute_Deep5bAct(this, net); }
  // #CAT_TI compute deep 5b activations
  void	TI_Send_Deep5bNetin(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Send_Deep5bNetin(this, net, thread_no); }
  // #CAT_TI send deep5b netinputs through Deep5bConSpec connections
  void	TI_Send_Deep5bNetin_Post(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Send_Deep5bNetin_Post(this, net, thread_no); }
  // #CAT_TI send deep5b netinputs through Deep5bConSpec connections -- post processing rollup
  void	TI_Send_CtxtNetin(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Send_CtxtNetin(this, net, thread_no); }
  // #CAT_TI send context netinputs through LeabraTICtxtConSpec connections
  void	TI_Send_CtxtNetin_Post(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->TI_Send_CtxtNetin_Post(this, net, thread_no); }
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

  void	Compute_SRAvg_Cons(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_SRAvg_Cons(this, net, thread_no); }
  // #CAT_Learning compute sending-receiving activation coproduct averages for the connections -- not used for XCAL typically -- just for CtLeabra_CAL

  void  Compute_dWt_l(LeabraNetwork* net, int thread_no=-1)
  { GetUnitSpec()->Compute_dWt(this, (Network*)net, thread_no); }
  // #CAT_Learning compute weight changes: the essence of learning -- leabra version
  void  Compute_Weights_l(LeabraNetwork* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Weights(this,(Network*)net, thread_no); }
  // #CAT_Learning update weight values from weight change variables -- leabra version

  void 	Compute_dWt_Norm(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_dWt_Norm(this, net, thread_no); }
  // #CAT_Learning compute normalization of dwt values -- must be done as a separate stage after dwt


  ///////////////////////////////////////////////////////////////////////
  //	Stats

  float	Compute_NormErr(LeabraNetwork* net)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_NormErr(this, net); }
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off from target) according to settings on the network (returns a 1 or 0)

  ///////////////////////////////////////////////////////////////////////
  //	Misc Housekeeping, non Compute functions

  void	BuildUnits() override;
  int   CountCons(Network* net) override
  { return ((LeabraUnitSpec*)GetUnitSpec())->CountCons(this, (LeabraNetwork*)net); }

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
