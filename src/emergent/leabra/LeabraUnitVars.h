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

#ifndef LeabraUnitVars_h
#define LeabraUnitVars_h 1

// parent includes:
#include <UnitVars>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraUnitVars);

class E_API LeabraUnitVars : public UnitVars {
  // #STEM_BASE ##CAT_Leabra Leabra unit variables, point-neuron approximation
public:
  float      bias_fwt;       // #NO_SAVE #CAT_Learning bias weight: fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
  float      bias_swt;       // #NO_SAVE #CAT_Learning bias weight: slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
  float      act_eq;         // #VIEW_HOT #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act for rate code equation, NXX1) -- this includes any short-term plasticity in synaptic efficacy (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters)
  float      act_nd;         // #VIEW_HOT #CAT_Activation non-depressed rate-code equivalent activity value (act_eq) -- this is the rate code prior to any short-term plasticity effects (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters) -- this reflects the rate of actual action potentials fired by the neuron, but not the net effect of these AP's on postsynaptic receiving neurons
  float      spike;          // #CAT_Activation discrete spiking event, is 1.0 when the neuron spikes and 0.0 otherwise, and corresponds to act for spike activation -- for rate code equation (NXX1), spikes are triggered identically to spiking mode based on the vm membrane potential dynamics, even though act* is computed through the rate code equation
  float      act_q0;         // #VIEW_HOT #CAT_Activation records the activation state at the very start of the current alpha-cycle (100 msec / 10 Hz) trial, prior to any trial-level decay -- is either act_eq or act_nd depending on act_misc.rec_nd setting -- needed for leabra TI context weight learning in the LeabraTICtxtConspec connection -- this is equivalent to old p_act_p variable -- the activation in the previous plus phase
  float      act_q1;         // #VIEW_HOT #CAT_Activation records the activation state after the first gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_q2;         // #VIEW_HOT #CAT_Activation records the activation state after the second gamma-frequency (25 msec / 40Hz) quarter (first half) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_q3;         // #VIEW_HOT #CAT_Activation records the activation state after the third gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_q4;         // #VIEW_HOT #CAT_Activation records the activation state after the fourth gamma-frequency (25 msec / 40Hz) quarter (end) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_m;         // #VIEW_HOT #CAT_Activation records the traditional posterior-cortical minus phase activation, as act_q3 activation after third quarter of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_p;         // #VIEW_HOT #CAT_Activation records the traditional posterior-cortical plus_phase activation, as act_q4 activation at end of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_dif;        // #VIEW_HOT #CAT_Activation act_p - act_m -- difference between plus and minus phase acts, -- reflects the individual error gradient for this neuron in standard error-driven learning terms
  float      net_prv_q;      // #CAT_Activation net input from the previous quarter -- this is used for delayed inhibition as specified in del_inhib on layer spec
  float      net_prv_trl;    // #CAT_Activation net input from the previous trial -- this is used for delayed inhibition as specified in del_inhib on layer spec
  float      da;             // #NO_SAVE #NO_SAVE #CAT_Activation delta activation: change in act_nd from one cycle to next -- can be useful to track where changes are taking place -- only updated when gui active
  float      avg_ss;         // #CAT_Activation super-short time-scale activation average -- provides the lowest-level time integration -- for spiking this integrates over spikes before subsequent averaging, and it is also useful for rate-code to provide a longer time integral overall
  float      avg_s;          // #CAT_Activation short time-scale activation average -- tracks the most recent activation states (integrates over avg_ss values), and represents the plus phase for learning in XCAL algorithms
  float      avg_m;          // #CAT_Activation medium time-scale activation average -- integrates over avg_s values, and represents the minus phase for learning in XCAL algorithms
  float      avg_l;          // #CAT_Activation long time-scale average of medium-time scale (trial level) activation, used for the BCM-style floating threshold in XCAL
  float      act_avg;        // #CAT_Activation average activation (of final plus phase activation state) over long time intervals (time constant = act_mid.avg_time -- typically 200) -- useful for finding hog units and seeing overall distribution of activation
  float      thal;           // #NO_SAVE #VIEW_HOT #CAT_Activation thalamic activation value, driven by a ThalUnitSpec -- used by cifer params in LeabraUnitSpec and MatrixConSpecs, and possibly other specs, to respond to thalamic inputs
  float      thal_prv;       // #NO_SAVE #CAT_Activation previous thalamic activation value (updated at end of phase or trial depending on cifer.phase), driven by a ThalUnitSpec -- used by cifer params in LeabraUnitSpec and MatrixConSpecs, and possibly other specs, to respond to thalamic inputs
  float      deep5b;         // #NO_SAVE #VIEW_HOT #CAT_Activation deep layer 5b activation values -- these reflect the output gating signal modulated by the thal and act values of the unit (microcolumn) -- deep5b is thresholded and represents the active foreground channel of information, while act reflects superficial layer 2/3 background information -- see cifer params in LeabraUnitSpec
  float      d5b_net;        // #NO_SAVE #CAT_Activation net input from deep layer 5b activation values
  float      ti_ctxt;        // #VIEW_HOT #NO_SAVE #CAT_Activation leabra TI context netinput value -- computed from LeabraTICtxtConspec connection -- computed from sending act or deep5b values (see cifer flags), which then project to layer 6 which integrates the contextualized value -- this is just added into overall net input
  float      lrnmod;         // #NO_SAVE #CAT_Activation learning modulation variable -- set by LearnModUnitSpec units (or possibly other types) -- used for special learning modulation variable, e.g., to represent the special status of the US/PV in the PVLV model as a learning enabler -- see LearnModDeltaConSpec and LearnModHebbConSpec 
  float      gc_i;           // #NO_SAVE #CAT_Activation total inhibitory conductance -- does NOT include the g_bar.i
  float      I_net;         // #NO_SAVE #CAT_Activation net current produced by all channels -- only updated when gui is active
  float      v_m;           // #NO_SAVE #CAT_Activation membrane potential -- integrates I_net current over time -- is reset by spiking (even when using rate code activations -- see v_m_eq)
  float      v_m_eq;        // #NO_SAVE #CAT_Activation equilibrium membrane potential -- this is NOT reset by spiking, so it reaches equilibrium values asymptotically -- it is used for rate code activation in sub-threshold range (whenever v_m_eq < act.thr) -- the gelin activation function does not otherwise provide useful dynamics in this subthreshold range
  float      adapt;         // #NO_SAVE #CAT_Activation adaptation factor -- driven by both sub-threshold membrane potential and spiking activity -- subtracts directly from the membrane potential on every time step
  float      gi_syn;        // #NO_SAVE #CAT_Activation aggregated synaptic inhibition (from inhib connections) -- time integral of gi_raw -- this is added with layer-level inhibition (fffb) to get the full inhibition in gc.i
  float      gi_self;       // #NO_SAVE #CAT_Activation self inhibitory current -- requires temporal integration dynamics and thus its own variable
  float      gi_ex;         // #NO_SAVE #CAT_Activation extra inhibitory current, e.g., from previous trial or phase -- only updated when gui active
  float      E_i;           // #NO_SAVE #CAT_Activation inhibitory reversal potential -- this adapts with activity, producing advantage for active neurons
  float      syn_tr;        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: total amount of transmitter ready to release = number of vesicles ready to release (syn_nr) x probability of release (syn_pr) (controlled by short-term-plasticity equations, stp) -- this multiplies activations to produce net sending effect
  float      syn_nr;        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: number of vesicles ready to release at next spike -- vesicles are depleated when released, resulting in short-term depression of net synaptic efficacy, and recover with both activity dependent and independent rate constants (controlled by short-term-plasticity equations, stp)
  float      syn_pr;         // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: probability of vesicle release at next spike -- probability varies as a function of local calcium available to drive the release process -- this increases with recent synaptic activity (controlled by short-term-plasticity equations, stp)
  float      syn_kre;        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: dynamic time constant for rate of recovery of number of vesicles ready to release -- this dynamic time constant increases with each action potential, and decays back down over time, and makes the response to higher-frequency spike trains more linear (controlled by short-term-plasticity equations, stp)
  float      noise;          // #NO_SAVE #CAT_Activation noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  float      dav;            // #NO_SAVE #VIEW_HOT #CAT_Activation dopamine value (da is delta activation) which modulates activations (e.g., via accom and hyst currents) to then drive learning
  float      sev;            // #NO_SAVE #CAT_Activation serotonin value 

  float      net_scale;      // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation total netinput scaling basis
  float      bias_scale;     // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation bias weight scaling factor
  float      ctxt_scale;     // #NO_SAVE #EXPERT #CAT_Activation TI context rescaling factor -- needed for rescaling ti_ctxt to compensate for variable deep5b input on different quarters

  float      act_sent;       // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last activation value sent (only send when diff is over threshold)
  float      net_raw;        // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw net input received from sending units (send delta delta's are added to this value)
  float      gi_raw;         // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw inhib net input received from sending units (increments the deltas in send_delta)
  float      d5b_sent;       // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last deep5b activation value sent (only send when diff is over threshold)

  float      misc_1;         // #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  int        spk_t;          // #NO_SAVE #CAT_Activation time in tot_cycle units when spiking last occurred (-1 for not yet)

};

#endif // LeabraUnitVars_h
