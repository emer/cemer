// this contains core shared code, and is included directly in LeabraPrjnState _cpp.h, _cuda.h
// {
  float		netin_avg;		// #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic average netinput values for the recv projections into this layer
  float		netin_rel;		// #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic relative netinput values for the recv projections into this layer

  float		avg_netin_avg;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Statistic average netinput values for the recv projections into this layer, averaged over an epoch
  float		avg_netin_avg_sum;	// #NO_SAVE #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic average netinput values for the recv projections into this layer, sum over an epoch
  float		avg_netin_rel;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Statistic relative netinput values for the recv projections into this layer, averaged over an epoch
  float		avg_netin_rel_sum;	// #NO_SAVE #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic relative netinput values for the recv projections into this layer, sum over an epoch (for computing average)
  int		avg_netin_n;		// #NO_SAVE #READ_ONLY #HIDDEN #DMEM_AGG_SUM #CAT_Statistic count for computing epoch-level averages
  float         wt_avg_max;            // #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic maximum average weight across recv con groups of units with this projection -- only computed if weight balance mechanism is on and network.lstats.wt_bal is on
  float         wt_avg_avg;            // #NO_SAVE #READ_ONLY #SHOW #CAT_Statistic average of weight averages across recv con group of units with this projection -- only computed if weight balance mechanism is on and network.lstats.wt_bal is on

  INLINE void	Init_Stats() {
    netin_avg = 0.0f;    netin_rel = 0.0f;
    avg_netin_avg = 0.0f;    avg_netin_avg_sum = 0.0f;    avg_netin_rel = 0.0f;
    avg_netin_rel_sum = 0.0f;    avg_netin_n = 0;
    wt_avg_max = 0.0f;    wt_avg_avg = 0.0f;
  }
  // #CAT_Statistic intialize statistic counters

  INLINE void Initialize_core() {
    Init_Stats();
  }
  // #IGNORE
  
