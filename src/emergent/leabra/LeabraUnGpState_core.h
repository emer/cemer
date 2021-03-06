// this contains core shared code, and is included directly in LeabraUnGpState _cpp.h, _cuda.h
//{

  float        acts_m_avg;       // #GUI_READ_ONLY #SHOW #CAT_Activation #SAVE_WTS time-averaged minus-phase activation stats for the layer -- this is used for adaptive inhibition tuning (inhib_adapt in LeabraLayerSpec) -- time constant in layer spec avg_act.tau and initialized to avg_act.init -- saved with weights
  float        acts_p_avg;        // #GUI_READ_ONLY #SHOW #CAT_Activation #SAVE_WTS time-averaged plus-phase activation stats for the layer -- time constant in layer spec avg_act.tau and initialized to avg_act.init -- this is used for netinput scaling (via _eff version) and should match reasonably well with act_avg.init value -- saved with weights
  float        acts_p_avg_eff;    // #GUI_READ_ONLY #SHOW #CAT_Activation #SAVE_WTS acts_p_avg * avg_act.adjust factor -- this is the effective value actually used for netinput scaling based on layer activation levels -- saved with weights
  STATE_CLASS(LeabraInhibVals) i_val;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation computed inhibitory values
  LEABRA_AVG_MAX    netin;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum net input values for the layer
  LEABRA_AVG_MAX    netin_m;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum net input values for the layer, minus phase
  LEABRA_AVG_MAX    netin_raw;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum raw net input values for the layer (synaptic netinput only -- no extras)
  LEABRA_AVG_MAX    acts;         // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum activation values for the layer
  LEABRA_AVG_MAX    acts_eq;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum act_eq values for the layer
  LEABRA_AVG_MAX    acts_raw;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum act_raw values for the layer
  LEABRA_AVG_MAX    acts_prvq;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum act_eq values for the layer, for the previous quarter (updated every quarter)
  LEABRA_AVG_MAX    acts_m;       // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum minus-phase activation stats for the layer (from acts_eq at end of minus phase)
  LEABRA_AVG_MAX    acts_p;       // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum plus-phase activation stats for the layer (from acts_eq at end of plus phase)
  LEABRA_AVG_MAX    acts_q0;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum previous-trial activation stats for the layer (from acts_p at start of new trial)
  LEABRA_AVG_MAX    un_g_i;       // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum unit total inhibitory conductance values (optionally updated -- use for unit inhibition case)
  LEABRA_AVG_MAX    am_avg_l;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum long-term average activation, avg_l, used for dynamic floating threshold in BCM hebbian learning 
  LEABRA_AVG_MAX    am_avg_l_lrn; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum learning rate based on long-term average activation, avg_l_lrn -- determines how much BCM hebbian learning occurs
  LEABRA_AVG_MAX    am_deep_raw;  // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum deep_raw values for the layer
  LEABRA_AVG_MAX    am_deep_raw_net;  // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum deep_raw_net values for the layer
  LEABRA_AVG_MAX    am_deep_mod_net; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum deep_mod_net values for the layer
  LEABRA_AVG_MAX    am_deep_ctxt;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum deep_ctxt values for the layer
  float        max_err;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic max activity error value for this unit group -- is 0 if the most active unit in the unit group has a non-zero target value -- otherwise 1 -- in other words, is the most active unit a target unit?  this only really makes sense for localist single-unit activity layers (although multiple units can be set to targets to allow for multiple options).  it is a highly sensitive measure, allowing for any other kinds of activity in the layer
  STATE_CLASS(LeabraTwoDVals)  twod; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation holds two-dimensional decoding / encoding values for spatial representational layers (TwoDValLayerSpec)
// todo: could potentially decode multiple locations per unit group -- replicate twod_2, twod_3?


  INLINE LEABRA_LAYER_STATE* GetLayerState(NETWORK_STATE* net) {
    return (LEABRA_LAYER_STATE*)inherited::GetLayerState(net);
  }
  INLINE LEABRA_UNIT_STATE*  GetUnitState(NETWORK_STATE* net, int un_idx) {
    return (LEABRA_UNIT_STATE*)inherited::GetUnitState(net, un_idx);
  }
  INLINE LEABRA_UNIT_STATE*  GetUnitStateSafe(NETWORK_STATE* net, int un_idx) {
    return (LEABRA_UNIT_STATE*)inherited::GetUnitStateSafe(net, un_idx);
  }

  INLINE float  GetTotalActEq(int n_un)  { return acts_eq.avg * (float)n_un; }
  // Get the total act_eq activation in the layer based on average and number of units
  INLINE float  GetTotalActQ0(int n_un)  { return acts_q0.avg * (float)n_un; }
  // Get the total act_q0 activation in the layer based on average and number of units

  INLINE void    UnGp_SetVals(float val)    { i_val.g_i = val; i_val.g_i_orig = val; }
  // set computed inhibition values -- sets original too

  INLINE void    UnGp_Init_Acts() {
    i_val.InitVals();    netin.InitVals();    netin_m.InitVals();    netin_raw.InitVals();
    acts.InitVals();    acts_eq.InitVals();    acts_raw.InitVals();    un_g_i.InitVals();
    am_avg_l.InitVals();      am_avg_l_lrn.InitVals();
    am_deep_raw.InitVals();   am_deep_raw_net.InitVals();    am_deep_mod_net.InitVals();
    am_deep_ctxt.InitVals();  twod.InitVals();
  }
  // init activations
  
  INLINE void    UnGp_Initialize() {
    i_val.InitVals();  acts_m_avg = .1f;  acts_p_avg = .1f;  acts_p_avg_eff = 1.f;
    max_err = 0.0f;
  }

  INLINE void   Initialize_ungp_core() { UnGp_Init_Acts(); }

  INLINE void   Init_UnGp_State(float m_avg, float p_avg, float p_avg_eff) {
    acts_m_avg = m_avg; acts_p_avg = p_avg; acts_p_avg_eff = p_avg_eff;
    UnGp_Init_Acts();
    acts_prvq.InitVals(); acts_m.InitVals(); acts_p.InitVals(); acts_q0.InitVals();
    max_err = 0.0f; twod.InitVals();
  }
  // called during Init_Weights -- init everything
