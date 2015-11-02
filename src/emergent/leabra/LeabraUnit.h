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
#include <LeabraUnitVars>

// member includes:
#include <LeabraUnitSpec>
#include <LeabraConGroup>
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

  inline float& act_eq()
  { return ((LeabraUnitVars*)GetUnitVars())->act_eq; }
  // #VIEW_HOT #CAT_UnitVar rate-code equivalent activity value (time-averaged spikes or just act for rate code equation, NXX1) -- this includes any short-term plasticity in synaptic efficacy (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters)
  inline float& act_nd()
  { return ((LeabraUnitVars*)GetUnitVars())->act_nd; }
  // #VIEW_HOT #CAT_UnitVar non-depressed rate-code equivalent activity value (act_eq) -- this is the rate code prior to any short-term plasticity effects (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters) -- this reflects the rate of actual action potentials fired by the neuron, but not the net effect of these AP's on postsynaptic receiving neurons
  inline float& spike()
  { return ((LeabraUnitVars*)GetUnitVars())->spike; }
  // #CAT_UnitVar discrete spiking event, is 1.0 when the neuron spikes and 0.0 otherwise, and corresponds to act for spike activation -- for rate code equation (NXX1), spikes are triggered identically to spiking mode based on the vm membrane potential dynamics, even though act* is computed through the rate code equation
  inline float& act_q0()
  { return ((LeabraUnitVars*)GetUnitVars())->act_q0; }
  // #VIEW_HOT #CAT_UnitVar records the activation state at the very start of the current alpha-cycle (100 msec / 10 Hz) trial, prior to any trial-level decay -- is either act_eq or act_nd depending on act_misc.rec_nd setting -- needed for leabra TI context weight learning in the LeabraTICtxtConspec connection -- this is equivalent to old p_act_p variable -- the activation in the previous plus phase
  inline float& act_q1()
  { return ((LeabraUnitVars*)GetUnitVars())->act_q1; }
  // #VIEW_HOT #CAT_UnitVar records the activation state after the first gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  inline float& act_q2()
  { return ((LeabraUnitVars*)GetUnitVars())->act_q2; }
  // #VIEW_HOT #CAT_UnitVar records the activation state after the second gamma-frequency (25 msec / 40Hz) quarter (first half) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  inline float& act_q3()
  { return ((LeabraUnitVars*)GetUnitVars())->act_q3; }
  // #VIEW_HOT #CAT_UnitVar records the activation state after the third gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  inline float& act_q4()
  { return ((LeabraUnitVars*)GetUnitVars())->act_q4; }
  // #VIEW_HOT #CAT_UnitVar records the activation state after the fourth gamma-frequency (25 msec / 40Hz) quarter (end) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  inline float& act_m()
  { return ((LeabraUnitVars*)GetUnitVars())->act_m; }
  // #VIEW_HOT #CAT_UnitVar records the traditional posterior-cortical minus phase activation, as act_q3 activation after third quarter of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  inline float& act_p()
  { return ((LeabraUnitVars*)GetUnitVars())->act_p; }
  // #VIEW_HOT #CAT_UnitVar records the traditional posterior-cortical plus_phase activation, as act_q4 activation at end of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  inline float& act_dif()
  { return ((LeabraUnitVars*)GetUnitVars())->act_dif; }
  // #VIEW_HOT #CAT_UnitVar act_p - act_m -- difference between plus and minus phase acts, -- reflects the individual error gradient for this neuron in standard error-driven learning terms
  inline float& net_prv_q()
  { return ((LeabraUnitVars*)GetUnitVars())->net_prv_q; }
  // #CAT_UnitVar net input from the previous quarter -- this is used for delayed inhibition as specified in del_inhib on layer spec
  inline float& net_prv_trl()
  { return ((LeabraUnitVars*)GetUnitVars())->net_prv_trl; }
  // #CAT_UnitVar net input from the previous trial -- this is used for delayed inhibition as specified in del_inhib on layer spec
  inline float& da()
  { return ((LeabraUnitVars*)GetUnitVars())->da; }
  // #CAT_UnitVar delta activation: change in act_nd from one cycle to next -- can be useful to track where changes are taking place -- only updated when gui active
  inline float& avg_ss()
  { return ((LeabraUnitVars*)GetUnitVars())->avg_ss; }
  // #CAT_UnitVar super-short time-scale activation average -- provides the lowest-level time integration -- for spiking this integrates over spikes before subsequent averaging, and it is also useful for rate-code to provide a longer time integral overall
  inline float& avg_s()
  { return ((LeabraUnitVars*)GetUnitVars())->avg_s; }
  // #CAT_UnitVar short time-scale activation average -- tracks the most recent activation states (integrates over avg_ss values), and represents the plus phase for learning in XCAL algorithms
  inline float& avg_s_eff()
  { return ((LeabraUnitVars*)GetUnitVars())->avg_s_eff; }
  // #CAT_UnitVar effective short time-scale activation average that is actually used for learning -- typically includes a small contribution from avg_m in addition to mostly avg_s -- important to ensure that when unit turns off in plus phase (short time scale), enough medium-phase trace remains so that learning signal doesn't just go all the way to 0, at which point no learning would take place
  inline float& avg_m()
  { return ((LeabraUnitVars*)GetUnitVars())->avg_m; }
  // #CAT_UnitVar medium time-scale activation average -- integrates over avg_s values, and represents the minus phase for learning in XCAL algorithms
  inline float& avg_l()
  { return ((LeabraUnitVars*)GetUnitVars())->avg_l; }
  // #CAT_UnitVar long time-scale average of medium-time scale (trial level) activation, used for the BCM-style floating threshold in XCAL
  inline float& avg_l_lrn()
  { return ((LeabraUnitVars*)GetUnitVars())->avg_l_lrn; }
  // #CAT_UnitVar how much to learn based on the long-term floating threshold (avg_l) for BCM-style Hebbian learning -- is modulated level of avg_l itself (stronger hebbian as average activation goes higher) and optionally the average amount of error experienced in the layer (to retain a common proportionality with the level of error-driven learning across layers)
  inline float& r_lrate()
  { return ((LeabraUnitVars*)GetUnitVars())->r_lrate; }
  // #CAT_UnitVar learning rate dynamics based on activity profile of the receiving unit -- can implement trace-like learning to support development of invariant representations
  inline float& act_avg()
  { return ((LeabraUnitVars*)GetUnitVars())->act_avg; }
  // #CAT_UnitVar average activation (of final plus phase activation state) over long time intervals (time constant = act_mid.avg_time -- typically 200) -- useful for finding hog units and seeing overall distribution of activation
  inline float& act_raw()
  { return ((LeabraUnitVars*)GetUnitVars())->act_raw; }
  // #CAT_UnitVar raw superficial-layer activation prior to mutliplication by deep_norm -- this may reflect layer 4 activation -- used in computing new deep_raw values
  inline float& deep_raw()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_raw; }
  // #VIEW_HOT #CAT_UnitVar deep layer raw activation values -- these reflect the raw output from a microcolumn, in the form of layer 5b tufted neurons that project to the thalamus -- they integrate local thresholded input from superficial layer and top-down deep-layer input from other areas, to provide raw attentional and output signal from an area -- this signal drives deep_ctxt temporal integration (TI) for predictive learning, in addition to attention
  inline float& deep_raw_prv()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_raw_prv; }
  // #CAT_UnitVar previous value of the deep layer raw activation values -- used for temporal context learning
  inline float& deep_ctxt()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_ctxt; }
  // #CAT_UnitVar temporally-delayed local lateral integration of deep_raw signals sent via DeepCtxtConSpec connections to provide context for temporal integration (TI) learning -- added into net input of superficial neurons -- computed at start of new alpha trial
  inline float& deep_mod()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_mod; }
  // #CAT_UnitVar current modulating value of deep layer 6 corticothalamic, regular spiking neurons that represents the net attentional filter applied to the superficial layers -- value is computed from deep_mod_net received via SendDeepModConSpec projections from deep layer units, and directly multiplies the superficial activations (act)
  inline float& deep_lrn()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_lrn; }
  // #CAT_UnitVar net influence of deep layer dynamics on learning rate for connections into this unit -- typically set to deep_mod prior to enforcing the mod_min floor value, so that baseline deep_mod=0 units get lowest background learning rate
  inline float& deep_mod_net()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_mod_net; }
  // #CAT_UnitVar modulatory net input from deep layer activations (representing lamina 6 regular spiking, thalamocortical projecting neurons) via SendDeepModConSpec, drives deep mod of superficial neurons
  inline float& deep_raw_net()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_raw_net; }
  // #CAT_UnitVar deep_raw net input from deep layer activations (representing lamina 5b intrinsic bursting neurons), typically for driver inputs into thalamic relay cells via SendDeepRawConSpec
  inline float& thal()
  { return ((LeabraUnitVars*)GetUnitVars())->thal; }
  // #VIEW_HOT #CAT_UnitVar thalamic activation value, driven by a ThalSendUnitSpec or GpiInvUnitSpec -- used by deep params in LeabraUnitSpec and MSNConSpecs, and possibly other specs, to respond to thalamic inputs
  inline float& thal_cnt()
  { return ((LeabraUnitVars*)GetUnitVars())->thal_cnt; }
  // #CAT_UnitVar counter for thalamic activation value -- increments for active maintenance in PFCUnitSpec
  inline float& gc_i()
  { return ((LeabraUnitVars*)GetUnitVars())->gc_i; }
  // #CAT_UnitVar total inhibitory conductance -- does NOT include the g_bar.i
  inline float& I_net()
  { return ((LeabraUnitVars*)GetUnitVars())->I_net; }
  // #CAT_UnitVar net current produced by all channels -- only updated when gui is active
  inline float& v_m()
  { return ((LeabraUnitVars*)GetUnitVars())->v_m; }
  // #CAT_UnitVar membrane potential -- integrates I_net current over time -- is reset by spiking (even when using rate code activations -- see v_m_eq)
  inline float& v_m_eq()
  { return ((LeabraUnitVars*)GetUnitVars())->v_m_eq; }
  // #CAT_UnitVar equilibrium membrane potential -- this is NOT reset by spiking, so it reaches equilibrium values asymptotically -- it is used for rate code activation in sub-threshold range (whenever v_m_eq < act.thr) -- the gelin activation function does not otherwise provide useful dynamics in this subthreshold range
  inline float& adapt()
  { return ((LeabraUnitVars*)GetUnitVars())->adapt; }
  // #CAT_UnitVar adaptation factor -- driven by both sub-threshold membrane potential and spiking activity -- subtracts directly from the membrane potential on every time step
  inline float& gi_syn()
  { return ((LeabraUnitVars*)GetUnitVars())->gi_syn; }
  // #CAT_UnitVar aggregated synaptic inhibition (from inhib connections) -- time integral of gi_raw -- this is added with layer-level inhibition (fffb) to get the full inhibition in gc.i
  inline float& gi_self()
  { return ((LeabraUnitVars*)GetUnitVars())->gi_self; }
  // #CAT_UnitVar self inhibitory current -- requires temporal integration dynamics and thus its own variable
  inline float& gi_ex()
  { return ((LeabraUnitVars*)GetUnitVars())->gi_ex; }
  // #CAT_UnitVar extra inhibitory current, e.g., from previous trial or phase -- only updated when gui active
  inline float& E_i()
  { return ((LeabraUnitVars*)GetUnitVars())->E_i; }
  // #CAT_UnitVar inhibitory reversal potential -- this adapts with activity, producing advantage for active neurons
  inline float& syn_tr()
  { return ((LeabraUnitVars*)GetUnitVars())->syn_tr; }
  // #CAT_UnitVar presynaptic (sending) synapse value: total amount of transmitter ready to release = number of vesicles ready to release (syn_nr) x probability of release (syn_pr) (controlled by short-term-plasticity equations, stp) -- this multiplies activations to produce net sending effect
  inline float& syn_nr()
  { return ((LeabraUnitVars*)GetUnitVars())->syn_nr; }
  // #CAT_UnitVar presynaptic (sending) synapse value: number of vesicles ready to release at next spike -- vesicles are depleated when released, resulting in short-term depression of net synaptic efficacy, and recover with both activity dependent and independent rate constants (controlled by short-term-plasticity equations, stp)
  inline float& syn_pr()
  { return ((LeabraUnitVars*)GetUnitVars())->syn_pr; }
  // #CAT_UnitVar presynaptic (sending) synapse value: probability of vesicle release at next spike -- probability varies as a function of local calcium available to drive the release process -- this increases with recent synaptic activity (controlled by short-term-plasticity equations, stp)
  inline float& syn_kre()
  { return ((LeabraUnitVars*)GetUnitVars())->syn_kre; }
  // #CAT_UnitVar presynaptic (sending) synapse value: dynamic time constant for rate of recovery of number of vesicles ready to release -- this dynamic time constant increases with each action potential, and decays back down over time, and makes the response to higher-frequency spike trains more linear (controlled by short-term-plasticity equations, stp)
  inline float& noise()
  { return ((LeabraUnitVars*)GetUnitVars())->noise; }
  // #CAT_UnitVar noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  inline float& da_p()
  { return ((LeabraUnitVars*)GetUnitVars())->da_p; }
  // #VIEW_HOT #CAT_UnitVar positive-valence oriented dopamine value -- this typically exhibits phasic bursts (positive values) with unanticipated increases in reward outcomes / expectations, and phasic dips (negative values) with unanticipated decreases thereof.  This value can drive dopaminergic learning rules and activation changes in receiving neurons -- typically sent by VTAUnitSpec units -- see also da_n
  inline float& da_n()
  { return ((LeabraUnitVars*)GetUnitVars())->da_n; }
  // #VIEW_HOT #CAT_UnitVar negative-valence oriented dopamine value -- this typically exhibits phasic bursts (positive values) with unanticipated increases in negative outcomes / expectations, and phasic dips (negative values) with unanticipated decreases thereof.  This value can drive dopaminergic learning rules and activation changes in receiving neurons -- typically sent by VTAUnitSpec units with appropriate flags set -- see also da_p
  inline float& sev()
  { return ((LeabraUnitVars*)GetUnitVars())->sev; }
  // #CAT_UnitVar serotonin value 

  inline float& misc_1()
  { return ((LeabraUnitVars*)GetUnitVars())->misc_1; }
  // #CAT_UnitVar miscellaneous variable for other algorithms that need it
  inline int& spk_t()
  { return ((LeabraUnitVars*)GetUnitVars())->spk_t; }
  // #CAT_UnitVar time in tot_cycle units when spiking last occurred (-1 for not yet)

  inline float& bias_scale()
  { return ((LeabraUnitVars*)GetUnitVars())->bias_scale; }
  // #NO_VIEW #EXPERT #CAT_UnitVar bias weight scaling factor

  inline float& act_sent()
  { return ((LeabraUnitVars*)GetUnitVars())->act_sent; }
  // #NO_VIEW #EXPERT #CAT_UnitVar last activation value sent (only send when diff is over threshold)
  inline float& net_raw()
  { return ((LeabraUnitVars*)GetUnitVars())->net_raw; }
  // #NO_VIEW #EXPERT #CAT_UnitVar raw net input received from sending units (send delta's are added to this value)
  inline float& gi_raw()
  { return ((LeabraUnitVars*)GetUnitVars())->gi_raw; }
  // #NO_VIEW #EXPERT #CAT_UnitVar raw inhib net input received from sending units (increments the deltas in send_delta)
  inline float& deep_raw_sent()
  { return ((LeabraUnitVars*)GetUnitVars())->deep_raw_sent; }
  // #NO_VIEW #EXPERT #CAT_UnitVar last deep_raw activation value sent in computing deep_raw_net

 // TODO: move these to a subclass, with fixed max_delay of float[] guys right inline

 // float_CircBuffer* act_buf;      // #NO_VIEW #NO_SAVE #CAT_UnitVar buffer of activation states for synaptic delay computation
 // float_CircBuffer* spike_e_buf;      // #NO_VIEW #NO_SAVE #CAT_UnitVar buffer of excitatory net input from spikes for synaptic integration over discrete spikes
 // float_CircBuffer* spike_i_buf; // #NO_VIEW #NO_SAVE #CAT_UnitVar buffer of inhibitory net input from spikes for synaptic integration over discrete spikes

 // inline void      AddToActBuf(SynDelaySpec& sds) {
 //   if(sds.on) act_buf->CircAddLimit(act, sds.delay);
 // }
 // add current activation to act buf if synaptic delay is on

 inline LeabraLayer*      own_lay() const {return (LeabraLayer*)inherited::own_lay();}
 inline LeabraNetwork*      own_net() const {return (LeabraNetwork*)inherited::own_net();}

  void      BuildUnits() override;

  void      CutLinks();
  void      Copy_(const LeabraUnit& cp);
  TA_BASEFUNS(LeabraUnit);
private:
  void       Initialize();
  void      Destroy()            { CutLinks(); }
};

#endif // LeabraUnit_h
