// this is directly included in LeabraUnitState_cpp and LeabraUnitState_cuda etc
//{
  // important: coordinate with any changes to UnitState_core UnitFlags -- ends at UN_FLAG_4 at 0x0010
  enum LeabraUnitFlags {        // #BITS extra flags on top of ext flags for leabra
    D1R         = 0x00100,     // has predominantly D1 receptors
    D2R         = 0x00200,     // has predominantly D2 receptors
    ACQUISITION = 0x00400,     // involved in Acquisition
    EXTINCTION  = 0x00800,     // involved in Extinction
    APPETITIVE  = 0x01000,     // appetitive (positive valence) coding
    AVERSIVE    = 0x02000,     // aversive (negative valence) coding
    PATCH       = 0x04000,     // patch-like structure (striosomes)
    MATRIX      = 0x08000,     // matrix-like structure 
    DORSAL      = 0x10000,     // dorsal 
    VENTRAL     = 0x20000,     // ventral
  };
  
  float      bias_fwt;       // #NO_SAVE #CAT_Learning bias weight: fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
  float      bias_swt;       // #NO_SAVE #CAT_Learning bias weight: slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
  float      ext_orig;       // #NO_SAVE #CAT_Activation original external input value (ext) -- need to save this in case ext gets transformed in various ways in the clamping process e.g., for ScalarValue layers
  float      act_eq;         // #VIEW_HOT #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act for rate code equation, NXX1) -- this includes any short-term plasticity in synaptic efficacy (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters)
  float      act_nd;         // #CAT_Activation non-depressed rate-code equivalent activity value (act_eq) -- this is the rate code prior to any short-term plasticity effects (e.g., depression or enhancement -- see LeabraUnitSpec::stp parameters) -- this reflects the rate of actual action potentials fired by the neuron, but not the net effect of these AP's on postsynaptic receiving neurons
  float      spike;          // #CAT_Activation discrete spiking event, is 1.0 when the neuron spikes and 0.0 otherwise, and corresponds to act for spike activation -- for rate code equation (NXX1), spikes are triggered identically to spiking mode based on the vm membrane potential dynamics, even though act* is computed through the rate code equation
  float      act_q0;         // #CAT_Activation records the activation state at the very start of the current alpha-cycle (100 msec / 10 Hz) trial, prior to any trial-level decay -- is either act_eq or act_nd depending on act_misc.rec_nd setting -- needed for leabra TI context weight learning in the LeabraTICtxtConspec connection -- this is equivalent to old p_act_p variable -- the activation in the previous plus phase
  float      act_q1;         // #CAT_Activation records the activation state after the first gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_q2;         // #CAT_Activation records the activation state after the second gamma-frequency (25 msec / 40Hz) quarter (first half) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_q3;         // #CAT_Activation records the activation state after the third gamma-frequency (25 msec / 40Hz) quarter of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_q4;         // #CAT_Activation records the activation state after the fourth gamma-frequency (25 msec / 40Hz) quarter (end) of the current alpha-cycle (100 msec / 10 Hz) trial -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_m;         // #VIEW_HOT #CAT_Activation records the traditional posterior-cortical minus phase activation, as act_q3 activation after third quarter of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_p;         // #VIEW_HOT #CAT_Activation records the traditional posterior-cortical plus_phase activation, as act_q4 activation at end of current alpha cycle -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      act_dif;        // #VIEW_HOT #CAT_Activation act_p - act_m -- difference between plus and minus phase acts, -- reflects the individual error gradient for this neuron in standard error-driven learning terms
  float      net_prv_q;      // #CAT_Activation net input from the previous quarter -- this is used for delayed inhibition as specified in del_inhib on layer spec
  float      net_prv_trl;    // #CAT_Activation net input from the previous trial -- this is used for delayed inhibition as specified in del_inhib on layer spec
  float      da;             // #NO_SAVE #NO_SAVE #CAT_Activation delta activation: change in act_nd from one cycle to next -- can be useful to track where changes are taking place -- only updated when gui active
  float      avg_ss;         // #CAT_Learning super-short time-scale activation average -- provides the lowest-level time integration -- for spiking this integrates over spikes before subsequent averaging, and it is also useful for rate-code to provide a longer time integral overall
  float      avg_s;          // #CAT_Learning short time-scale activation average -- tracks the most recent activation states (integrates over avg_ss values), and represents the plus phase for learning in XCAL algorithms
  float      avg_s_eff;      // #CAT_Learning effective short time-scale activation average that is actually used for learning -- typically includes a small contribution from avg_m in addition to mostly avg_s -- important to ensure that when unit turns off in plus phase (short time scale), enough medium-phase trace remains so that learning signal doesn't just go all the way to 0, at which point no learning would take place
  float      avg_m;          // #CAT_Learning medium time-scale activation average -- integrates over avg_s values, and represents the minus phase for learning in XCAL algorithms
  float      avg_l;          // #CAT_Learning long time-scale average of medium-time scale (trial level) activation, used for the BCM-style floating threshold in XCAL
  float      avg_l_lrn;      // #CAT_Learning how much to learn based on the long-term floating threshold (avg_l) for BCM-style Hebbian learning -- is modulated level of avg_l itself (stronger hebbian as average activation goes higher) and optionally the average amount of error experienced in the layer (to retain a common proportionality with the level of error-driven learning across layers)
  float      act_avg;        // #VIEW_HOT #CAT_Activation average activation (of final plus phase activation state) over long time intervals (time constant = act_misc.avg_tau -- typically 200) -- useful for finding hog units and seeing overall distribution of activation -- if act_misc.avg_trace is active, then it is instead an exponentially decaying trace -- used in TD reinforcement learning
  float      margin;         // #CAT_Activation relative status of this unit for the overall activation state / attractor, used for favoring units on the edges or margins of the attractor: -2 = below the low threshold -- solidly OFF, -1.0 = above the low threshold but below the midway threshold for the marginal units, +1.0 = above the midway threshold but still in the margin, +2.0 = above the high threshold and solidly within the main attractor state
  float      act_raw;        // #CAT_Activation raw superficial-layer activation prior to mutliplication by deep_norm -- this may reflect layer 4 activation -- used in computing new deep_raw values
  float      deep_raw;       // #NO_SAVE #CAT_Activation deep layer raw activation values -- these reflect the raw output from a microcolumn, in the form of layer 5b tufted neurons that project to the thalamus -- they integrate local thresholded input from superficial layer and top-down deep-layer input from other areas, to provide raw attentional and output signal from an area -- this signal drives deep_ctxt temporal integration (TI) for predictive learning, in addition to attention
  float      deep_raw_prv;    // #NO_SAVE #CAT_Activation previous value of the deep layer raw activation values -- used for temporal context learning
  float      deep_ctxt;       // #NO_SAVE #CAT_Activation temporally-delayed local lateral integration of deep_raw signals sent via DeepCtxtConSpec connections to provide context for temporal integration (TI) learning -- added into net input of superficial neurons -- computed at start of new alpha trial (quarter after deep_raw_qtr)
  float      deep_mod;        // #NO_SAVE #CAT_Activation current modulating value of deep layer 6 corticothalamic, regular spiking neurons that represents the net attentional filter applied to the superficial layers -- value is computed from deep_net received via SendDeepModConSpec projections from deep layer units, and directly multiplies the superficial activations (act)
  float      deep_lrn;       // #NO_SAVE #CAT_Activation net influence of deep layer dynamics on learning rate for connections into this unit -- typically set to deep_mod prior to enforcing the mod_min floor value, so that baseline deep_mod=0 units get lowest background learning rate
  float      deep_mod_net;   // #NO_SAVE #CAT_Activation modulatory net input from deep layer activations (representing lamina 6 regular spiking, thalamocortical projecting neurons) via SendDeepModConSpec, drives deep mod of superficial neurons
  float      deep_raw_net;   // #NO_SAVE #CAT_Activation deep_raw net input from deep layer activations (representing lamina 5b intrinsic bursting neurons), typically for driver inputs into thalamic relay cells via SendDeepRawConSpec
  float      thal;           // #NO_SAVE #CAT_Activation thalamic activation value, driven by a ThalSendUnitSpec or GpiInvUnitSpec -- used by deep params in LeabraUnitSpec and MSNConSpecs, and possibly other specs, to respond to thalamic inputs
  float      thal_gate;      // #NO_SAVE #CAT_Activation discrete thalamic gating signal -- typically activates to 1 when thalamic pathway gates, and is 0 otherwise -- PFC and BG layers receive this signal to drive updating etc at the proper time -- other layers can use the LeabraNetwork times.thal_gate_cycle signal
  float      thal_cnt;       // #NO_SAVE #CAT_Activation counter for thalamic activation value -- increments for active maintenance in PFCUnitSpec
  float      act_g;          // #NO_SAVE #CAT_Activation records the activation state when gating occurs -- for PFC and BG units this is based on direct thal_gate signal, and for other units it is based on LeabraNetwork times.thal_gate signal, which is actvated when thalamic layers gate and are configured to update the global signal -- is either act_eq or act_nd depending on act_misc.rec_nd setting
  float      gc_i;           // #NO_SAVE #CAT_Activation total inhibitory conductance -- does NOT include the g_bar.i
  float      gc_kna_f;       // #NO_SAVE #CAT_Activation sodium-gated potassium channel activation -- drives adapatation -- fast time constant
  float      gc_kna_m;       // #NO_SAVE #CAT_Activation sodium-gated potassium channel activation -- drives adapatation -- medium time constant
  float      gc_kna_s;       // #NO_SAVE #CAT_Activation sodium-gated potassium channel activation -- drives adapatation -- slow time constant
  float      I_net;          // #NO_SAVE #CAT_Activation net current produced by all channels -- only updated when gui is active
  float      v_m;            // #NO_SAVE #CAT_Activation membrane potential -- integrates I_net current over time -- is reset by spiking (even when using rate code activations -- see v_m_eq)
  float      v_m_eq;         // #NO_SAVE #CAT_Activation equilibrium membrane potential -- this is NOT reset by spiking, so it reaches equilibrium values asymptotically -- it is used for rate code activation in sub-threshold range (whenever v_m_eq < act.thr) -- the gelin activation function does not otherwise provide useful dynamics in this subthreshold range
  float      gi_syn;         // #NO_SAVE #CAT_Activation aggregated synaptic inhibition (from inhib connections) -- time integral of gi_raw -- this is added with layer-level inhibition (fffb) to get the full inhibition in gc.i
  float      gi_self;        // #NO_SAVE #CAT_Activation self inhibitory current -- requires temporal integration dynamics and thus its own variable
  float      gi_ex;          // #NO_SAVE #CAT_Activation extra inhibitory current, e.g., from previous trial or phase -- only updated when gui active
  float      syn_tr;         // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: total amount of transmitter ready to release = number of vesicles ready to release (syn_nr) x probability of release (syn_pr) (controlled by short-term-plasticity equations, stp) -- this multiplies activations to produce net sending activation (also affects act_eq, but not act_nd)
  float      syn_nr;         // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: number of vesicles ready to release at next spike -- vesicles are depleated when released, resulting in short-term depression of net synaptic efficacy, and recover with both activity dependent and independent rate constants (controlled by short-term-plasticity equations, stp)
  float      syn_pr;         // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: probability of vesicle release at next spike -- probability varies as a function of local calcium available to drive the release process -- this increases with recent synaptic activity (controlled by short-term-plasticity equations, stp)
  float      syn_kre;        // #NO_SAVE #CAT_Activation presynaptic (sending) synapse value: dynamic time constant for rate of recovery of number of vesicles ready to release -- this dynamic time constant increases with each action potential, and decays back down over time, and makes the response to higher-frequency spike trains more linear (controlled by short-term-plasticity equations, stp)
  float      noise;          // #NO_SAVE #CAT_Activation noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  float      da_p;           // #NO_SAVE #CAT_Activation positive-valence oriented dopamine value -- this typically exhibits phasic bursts (positive values) with unanticipated increases in reward outcomes / expectations, and phasic dips (negative values) with unanticipated decreases thereof.  This value can drive dopaminergic learning rules and activation changes in receiving neurons -- typically sent by VTAUnitSpec units -- see also da_n
  float      da_n;           // #NO_SAVE #CAT_Activation negative-valence oriented dopamine value -- this typically exhibits phasic bursts (positive values) with unanticipated increases in negative outcomes / expectations, and phasic dips (negative values) with unanticipated decreases thereof.  This value can drive dopaminergic learning rules and activation changes in receiving neurons -- typically sent by VTAUnitSpec units with appropriate flags set -- see also da_p
  float      sev;            // #NO_SAVE #CAT_Activation serotonin value -- driven by Dorsal Raphe Nucleus (DRNUnitSpec) or other sources -- generally thought to reflect longer time-averages of overall progress or lack thereof
  float      ach;            // #NO_SAVE #CAT_Activation acetylcholine value -- driven by Tonically Active Neurons (TAN's) in the Striatum, or Basal Forebrain Cholenergic System (BFCS) potentially other sources -- effects depend strongly on types of receptors present
  float      shunt;          // #NO_SAVE #CAT_Activation shunting value -- modulatory signal that shunts activity in other layers -- currently sent by PatchUnitSpec to MSNUnitSpecs in the BG to shunt dopamine and ach

  float      misc_1;         // #NO_SAVE #CAT_Activation miscellaneous variable for special algorithms / subtypes that need it
  float      misc_2;         // #NO_SAVE #CAT_Activation miscellaneous variable for special algorithms / subtypes that need it
  int        spk_t;          // #NO_SAVE #CAT_Activation time in tot_cycle units when spiking last occurred (-1 for not yet)

  float      act_sent;       // #NO_SAVE #EXPERT #CAT_Activation last activation value sent (only send when diff is over threshold)
  float      net_raw;        // #NO_SAVE #EXPERT #CAT_Activation raw net input received from sending units (send delta's are added to this value)
  float      gi_raw;         // #NO_SAVE #EXPERT #CAT_Activation raw inhib net input received from sending units (increments the deltas in send_delta)
  float      deep_raw_sent;  // #NO_SAVE #EXPERT #CAT_Activation last deep_raw activation value sent in computing deep_raw_net

  
  float      spike_e_buf[LEABRA_MAX_SPIKE_INTEG_WIN];
  // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation buffer for integrating spike current over time, for excitatory inputs
  int        spike_e_st;      // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation starting index in buffer for integrating spike current over time, for excitatory inputs
  int        spike_e_len;      // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation length of  buffer for integrating spike current over time, for excitatory inputs

  float      spike_i_buf[LEABRA_MAX_SPIKE_INTEG_WIN];
  // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation buffer for integrating spike current over time, for inhibitory inputs
  int        spike_i_st;      // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation starting index in buffer for integrating spike current over time, for inhibitory inputs
  int        spike_i_len;      // #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation length of  buffer for integrating spike current over time, for inhibitory inputs

  INLINE LEABRA_UNIT_SPEC_CPP* GetUnitSpec(NETWORK_STATE* nnet) const {
    return (LEABRA_UNIT_SPEC_CPP*)inherited::GetUnitSpec(nnet);
  }
  INLINE LEABRA_LAYER_STATE* GetOwnLayer(NETWORK_STATE* nnet) const {
    return (LEABRA_LAYER_STATE*)inherited::GetOwnLayer(nnet);
  }
  INLINE LEABRA_UNGP_STATE* GetOwnUnGp(NETWORK_STATE* nnet) const {
    return (LEABRA_UNGP_STATE*)inherited::GetOwnUnGp(nnet);
  }

  INLINE LEABRA_CON_STATE*  RecvConState(NETWORK_STATE* nnet, int rcg_idx) const
  { return (LEABRA_CON_STATE*)inherited::RecvConState(nnet, rcg_idx); }

  INLINE LEABRA_CON_STATE*  RecvConStateSafe(NETWORK_STATE* nnet, int rcg_idx) const
  { return (LEABRA_CON_STATE*)inherited::RecvConStateSafe(nnet, rcg_idx); }

  INLINE LEABRA_CON_STATE*  SendConState(NETWORK_STATE* nnet, int scg_idx) const
  { return (LEABRA_CON_STATE*)inherited::SendConState(nnet, scg_idx); }

  INLINE LEABRA_CON_STATE*  SendConStateSafe(NETWORK_STATE* nnet, int scg_idx) const
  { return (LEABRA_CON_STATE*)inherited::SendConStateSafe(nnet, scg_idx); }

  INLINE void  Initialize_core() { }
