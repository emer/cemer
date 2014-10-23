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

eTypeDef_Of(LeabraUnit);

class E_API LeabraUnit : public Unit {
  // #STEM_BASE ##CAT_Leabra Leabra unit, point-neuron approximation
INHERITED(Unit)
public:
  float		act_eq;	        // #VIEW_HOT #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act for rate code equation, NXX1) -- this includes any short-term plasticity in synaptic efficacy (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters)
  float		act_nd;	        // #VIEW_HOT #CAT_Activation non-depressed rate-code equivalent activity value (act_eq) -- this is the rate code prior to any short-term plasticity effects (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters) -- this reflects the rate of actual action potentials fired by the neuron, but not the net effect of these AP's on postsynaptic receiving neurons
  float         spike;          // #CAT_Activation discrete spiking event, is 1.0 when the neuron spikes and 0.0 otherwise, and corresponds to act for spike activation -- for rate code equation (NXX1), spikes are triggered identically to spiking mode based on the vm membrane potential dynamics, even though act* is computed through the rate code equation
  float		act_q0;	        // #VIEW_HOT #CAT_Activation records the activation state at the very start of the current alpha-cycle (100 msec / 10 Hz) trial, prior to any trial-level decay -- is either act_eq or act_nd depending on act_misc.rec_nd setting -- needed for leabra TI context weight learning in the LeabraTICtxtConspec connection -- this is equivalent to old p_act_p variable -- the activation in the previous plus phase
  float		act_q1;	        // #VIEW_HOT #CAT_Activation records the activation state after the first gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float		act_q2;	        // #VIEW_HOT #CAT_Activation records the activation state after the second gamma-frequency (25 msec / 40Hz) quarter (first half) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float		act_q3;	        // #VIEW_HOT #CAT_Activation records the activation state after the third gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float		act_q4;	        // #VIEW_HOT #CAT_Activation records the activation state after the fourth gamma-frequency (25 msec / 40Hz) quarter (end) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float		act_m;	        // #VIEW_HOT #CAT_Activation records the traditional posterior-cortical minus phase activation, as act_q3 activation after third quarter of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float		act_p;	        // #VIEW_HOT #CAT_Activation records the traditional posterior-cortical plus_phase activation, as act_q4 activation at end of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float		act_dif;        // #VIEW_HOT #CAT_Activation act_p - act_m -- difference between plus and minus phase acts, -- reflects the individual error gradient for this neuron in standard error-driven learning terms
  float         net_prv_q;      // #CAT_Activation net input from the previous quarter -- this is used for delayed inhibition as specified in del_inhib on layer spec
  float         net_prv_trl;    // #CAT_Activation net input from the previous trial -- this is used for delayed inhibition as specified in del_inhib on layer spec
  float		da;	        // #NO_SAVE #NO_SAVE #CAT_Activation delta activation: change in act_nd from one cycle to next -- can be useful to track where changes are taking place
  float		avg_ss;	        // #CAT_Activation super-short time-scale activation average -- provides the lowest-level time integration -- for spiking this integrates over spikes before subsequent averaging, and it is also useful for rate-code to provide a longer time integral overall
  float		avg_s;	        // #CAT_Activation short time-scale activation average -- tracks the most recent activation states (integrates over avg_ss values), and represents the plus phase for learning in XCAL algorithms
  float		avg_m;	        // #CAT_Activation medium time-scale activation average -- integrates over avg_s values, and represents the minus phase for learning in XCAL algorithms
  float		avg_l;	        // #CAT_Activation long time-scale average of medium-time scale (trial level) activation, used for the BCM-style floating threshold in XCAL
  float		act_avg;        // #CAT_Activation average activation (of final plus phase activation state) over long time intervals (time constant = act_mid.avg_time -- typically 200) -- useful for finding hog units and seeing overall distribution of activation
  float 	thal;		// #NO_SAVE #VIEW_HOT #CAT_Activation thalamic activation value, driven by a ThalUnitSpec -- used by cifer params in LeabraUnitSpec and MatrixConSpecs, and possibly other specs, to respond to thalamic inputs
  float 	thal_prv;       // #NO_SAVE #CAT_Activation previous thalamic activation value (updated at end of phase or trial depending on cifer.phase), driven by a ThalUnitSpec -- used by cifer params in LeabraUnitSpec and MatrixConSpecs, and possibly other specs, to respond to thalamic inputs
  float         deep5b;         // #NO_SAVE #VIEW_HOT #CAT_Activation deep layer 5b activation values -- these reflect the output gating signal modulated by the thal and act values of the unit (microcolumn) -- deep5b is thresholded and represents the active foreground channel of information, while act reflects superficial layer 2/3 background information -- see cifer params in LeabraUnitSpec
  float         d5b_net;        // #NO_SAVE #CAT_Activation net input from deep layer 5b activation values
  float         ti_ctxt;        // #VIEW_HOT #NO_SAVE #CAT_Activation leabra TI context netinput value -- computed from LeabraTICtxtConspec connection -- computed from sending act or deep5b values (see cifer flags), which then project to layer 6 which integrates the contextualized value -- this is just added into overall net input
  float         lrnmod;         // #NO_SAVE #CAT_Activation learning modulation variable -- set by LearnModUnitSpec units (or possibly other types) -- used for special learning modulation variable, e.g., to represent the special status of the US/PV in the PVLV model as a learning enabler -- see LearnModDeltaConSpec and LearnModHebbConSpec 
  float         gc_i;           // #NO_SAVE #CAT_Activation total inhibitory conductance
  float         gc_l;           // #NO_SAVE #CAT_Activation total leak conductance
  float		I_net;	        // #NO_SAVE #CAT_Activation net current produced by all channels
  float		v_m;	        // #NO_SAVE #CAT_Activation membrane potential -- integrates I_net current over time -- is reset by spiking (even when using rate code activations -- see v_m_eq)
  float		v_m_eq;	        // #NO_SAVE #CAT_Activation equilibrium membrane potential -- this is NOT reset by spiking, so it reaches equilibrium values asymptotically -- it is used for rate code activation in sub-threshold range (whenever v_m_eq < act.thr) -- the gelin activation function does not otherwise provide useful dynamics in this subthreshold range
  float		adapt;	        // #NO_SAVE #CAT_Activation adaptation factor -- driven by both sub-threshold membrane potential and spiking activity -- subtracts directly from the membrane potential on every time step
  float		gi_syn;	        // #NO_SAVE #CAT_Activation aggregated synaptic inhibition (from inhib connections) -- time integral of gi_raw -- this is added with layer-level inhibition (fffb) to get the full inhibition in gc.i
  float         gi_self;        // #NO_SAVE #CAT_Activation self inhibitory current -- requires temporal integration dynamics and thus its own variable
  float         gi_ex;          // #NO_SAVE #CAT_Activation extra inhibitory current, e.g., from previous trial or phase
  float         E_i;            // #NO_SAVE #CAT_Activation inhibitory reversal potential -- this adapts with activity, producing advantage for active neurons
  float		syn_tr;	        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: total amount of transmitter ready to release = number of vesicles ready to release (syn_nr) x probability of release (syn_pr) (controlled by short-term-plasticity equations, stp) -- this multiplies activations to produce net sending effect
  float		syn_nr;	        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: number of vesicles ready to release at next spike -- vesicles are depleated when released, resulting in short-term depression of net synaptic efficacy, and recover with both activity dependent and independent rate constants (controlled by short-term-plasticity equations, stp)
  float		syn_pr;	        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: probability of vesicle release at next spike -- probability varies as a function of local calcium available to drive the release process -- this increases with recent synaptic activity (controlled by short-term-plasticity equations, stp)
  float		syn_kre;        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: dynamic time constant for rate of recovery of number of vesicles ready to release -- this dynamic time constant increases with each action potential, and decays back down over time, and makes the response to higher-frequency spike trains more linear (controlled by short-term-plasticity equations, stp)
  float		noise;		// #NO_SAVE #CAT_Activation noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  float 	dav;		// #NO_SAVE #VIEW_HOT #CAT_Activation dopamine value (da is delta activation) which modulates activations (e.g., via accom and hyst currents) to then drive learning
  float 	sev;		// #NO_SAVE #CAT_Activation serotonin value 

  float		net_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation total netinput scaling basis
  float		bias_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation bias weight scaling factor
  float		ctxt_scale;	// #NO_SAVE #EXPERT #CAT_Activation TI context rescaling factor -- needed for rescaling ti_ctxt to compensate for variable deep5b input on different quarters

  float		act_sent;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last activation value sent (only send when diff is over threshold)
  float		net_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw net input received from sending units (send delta delta's are added to this value)
  float		gi_raw;	        // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw inhib net input received from sending units (increments the deltas in send_delta)
  float		d5b_sent;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last deep5b activation value sent (only send when diff is over threshold)

  float		misc_1;		// #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  int		spk_t;		// #NO_SAVE #CAT_Activation time in tot_cycle units when spiking last occurred (-1 for not yet)

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
  //	Quarter_Init -- at start of settling

  void	Quarter_Init_Unit(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Quarter_Init_Unit(this, net, thread_no); }
  // #CAT_Activation quarter unit-level initialization functions: Init_TargFlags, DecayState, NetinScale

    void Quarter_Init_TargFlags(LeabraNetwork* net)
    { ((LeabraUnitSpec*)GetUnitSpec())->Quarter_Init_TargFlags(this, net); }
    // #CAT_Activation initialize external input flags based on phase
    void Compute_NetinScale(LeabraNetwork* net, int)
    { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinScale(this, net); }
    // #CAT_Activation compute net input scaling values

  void Compute_NetinScale_Senders(LeabraNetwork* net, int)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_NetinScale_Senders(this, net); }
  // #CAT_Activation compute net input scaling values for sending cons -- copies from values computed in the recv guys -- has to be done as a second phase of the Quarter_Init_Unit stage after all the recv ones are computed

  void Compute_HardClamp(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClamp(this, net); }
    // #CAT_Activation force units to external values provided by environment: also optimizes settling by only computing netinput once
  void	Compute_HardClampNoClip(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_HardClampNoClip(this, net); }
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for second plus phase clamping to quarterd values)

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
  float Compute_EThresh()
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_EThresh(this); }
  // #CAT_Activation #IGNORE compute excitatory value that would place unit directly at threshold
  void	Send_Deep5bNetin(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_Deep5bNetin(this, net, thread_no); }
  // #CAT_TI send deep5b netinputs through Deep5bConSpec connections
  void	Send_Deep5bNetin_Post(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_Deep5bNetin_Post(this, net, thread_no); }
  // #CAT_TI send deep5b netinputs through Deep5bConSpec connections -- post processing rollup

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  void	Compute_ApplyInhib(LeabraLayerSpec* lspec, LeabraNetwork* net, LeabraInhib* thr,
                           float ival)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_ApplyInhib(this, lspec, net, thr, ival); }
  // #CAT_Activation #IGNORE apply computed inhibitory value to unit inhibitory conductance

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  void  Compute_Act_l(LeabraNetwork* net, int thread_no=-1)
  { GetUnitSpec()->Compute_Act(this, (Network*)net, thread_no); }
  // #CAT_Activation compute activation value: what we send to others -- leabra version

  float Compute_ActFun_rate_impl(float val_sub_thr)
  { return ((LeabraUnitSpec*)GetUnitSpec())->Compute_ActFun_rate_impl(val_sub_thr); }
  // #CAT_Activation raw activation function: computes an activation value from given value subtracted from its relevant threshold value

  void  Compute_Act_Post(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Compute_Act_Post(this, net, thread_no); }
  // #CAT_Activation post-processing step after activations are computed -- calls Compute_CIFER (applies threshold to the thal variable), and Compute_SRAvg by default

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats


  ///////////////////////////////////////////////////////////////////////
  //	Quarter_Final

  void	Quarter_Final(LeabraNetwork* net)
  { ((LeabraUnitSpec*)GetUnitSpec())->Quarter_Final(this, net); }
  // #CAT_Activation set stuff after settling is over (act_m, act_p etc)


  ///////////////////////////////////////////////////////////////////////
  //	LeabraTI

  void	Send_TICtxtNetin(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_TICtxtNetin(this, net, thread_no); }
  // #CAT_TI send context netinputs through LeabraTICtxtConSpec connections
  void	Send_TICtxtNetin_Post(LeabraNetwork* net, int thread_no=-1)
  { ((LeabraUnitSpec*)GetUnitSpec())->Send_TICtxtNetin_Post(this, net, thread_no); }
  // #CAT_TI send context netinputs through LeabraTICtxtConSpec connections -- post processing rollup
  void	ClearTICtxt(LeabraNetwork* net) 
  { ((LeabraUnitSpec*)GetUnitSpec())->ClearTICtxt(this, net); }
  // #CAT_TI clear the ti_ctxt context variables -- can be useful to do at discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //	Trial Final

  ///////////////////////////////////////////////////////////////////////
  //	Learning

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
