// this is included in LeabraLayerState _cpp.h, _cuda.h
// {

  float      bias_scale;     // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation bias weight scaling factor -- computed in Compute_NetinScale and common to all units in this layer
  STATE_CLASS(LeabraInhibVals)  laygp_i_val;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation layer-group level computed inhibitory values
  STATE_CLASS(LeabraAvgMax)	laygp_netin;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum net input values for the layer group
  STATE_CLASS(LeabraAvgMax)	laygp_acts_eq;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Activation average, maximum act_eq values for the layer group

  INLINE void  Initialize_core() { bias_scale = 1.0f; }
