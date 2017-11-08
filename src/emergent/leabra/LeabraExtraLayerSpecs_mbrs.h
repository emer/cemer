// Copyright 2017, Regents of the University of Colorado,
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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#pragma maketa_file_is_target LeabraExtraLayerSpecs


class STATE_CLASS(ScalarValSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for scalar values
INHERITED(SpecMemberBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST,			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
    AVG_ACT,                    // value is the average activation of the rest of the units, rescaled into desired overall range -- most appropriate for FF_FB inhibition with sufficient self feedback inhibition to ensure proportional activation levels with reasonable range of variability -- can have as few as 1 value-coding unitsa
  };

  RepType	rep;		// type of representation of scalar value to use
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in unit_range min-max units, unless norm_width is true, meaning use normalized 0-1 proportion of unit range)
  bool		norm_width;	// un_width is specified in normalized 0-1 proportion of unit range
  bool		clamp_pat;	// #DEF_false if true, environment provides full set of values to clamp over entire layer (instead of providing single scalar value to clamp on 1st unit, which then generates a corresponding distributed pattern)
  float		min_sum_act;	// #DEF_0.2 minimum total activity of all the units representing a value: when computing weighted average value, this is used as a minimum for the sum that you divide by
  bool		clip_val;	// ensure that value remains within specified range
  bool		send_thr;	// use unitspec.opt_thresh.send threshold to cut off small activation contributions to overall average value (i.e., if unit's activation is below this threshold, it doesn't contribute to weighted average computation)
  bool		init_nms;	// initialize unit names when weights are initialized

  float		min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		range;		// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  float		un_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective unit range

  INLINE virtual void	InitRange(float umin, float urng) {
    min = umin; range = urng;
    un_width_eff = un_width;
    if(norm_width)
      un_width_eff *= range;
  }
  // initialize range values (also sets un_width_eff)
  
  INLINE virtual void	InitVal(float sval, int ugp_size, float umin, float urng) {
    InitRange(umin, urng);
    val = sval;
    if(ugp_size > 1)
      incr = range / (float)(ugp_size - 1); // count end..
    else
      incr = 0.0f;
    //  incr -= .000001f;         // round-off tolerance..
  }
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  
  INLINE virtual float	GetUnitAct(int unit_idx) {
    int eff_idx = unit_idx;
    if(rep == GAUSSIAN) {
      float cur = min + incr * (float)eff_idx;
      float dist = (cur - val) / un_width_eff;
      return STATE_CLASS(taMath_float)::exp_fast(-(dist * dist));
    }
    else if(rep == LOCALIST) {
      float cur = min + incr * (float)eff_idx;
      if(fabs(val - cur) > incr) return 0.0f;
      return 1.0f - (fabs(val - cur) / incr);
    }
    return 0.0f;                  // compiler food
  }
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  
  INLINE virtual float	GetUnitVal(int unit_idx) {
    int eff_idx = unit_idx;
    float cur = min + incr * (float)eff_idx;
    return cur;
  }
  // get target value associated with unit at given index: MUST CALL InitVal first!

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(ScalarValSpec);
private:
  void	Initialize() {
    rep = LOCALIST;    un_width = .3f;    norm_width = false;    clamp_pat = false;
    clip_val = true;    send_thr = false;    init_nms = true;    min = val = 0.0f;
    range = incr = 1.0f;    un_width_eff = un_width;
    Defaults_init();
  }
    
  void	Defaults_init() {   min_sum_act = 0.2f; }
};


class STATE_CLASS(ScalarValBias) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(SpecMemberBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    BWT				// bias value enters as a bias.wt
  };

  enum BiasShape {		// shape of the bias pattern
    VAL,			// bias as a particular value representation
    NEG_SLP,			// bias as an increasingly negative slope (gc.a or -bwt) as unit values increase
    POS_SLP			// bias as an increasingly positive slope (gc.h or +bwt) as unit values increase
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT				// input weights
  };

  UnitBias	un;		// bias on individual units
  BiasShape	un_shp;		// shape of unit bias
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		val;		// value location (center of gaussian bump)

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(ScalarValBias);
private:
  void	Initialize() {
    un = NO_UN;    un_shp = VAL;    un_gain = 1.0f;    wt = NO_WT;    val = 0.0f;
    wt_gain = 1.0f;
  }
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};



class STATE_CLASS(OutErrSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for computing external rewards based on output performance of network
INHERITED(SpecMemberBase)
public:
  float		err_tol;	// #DEF_0.5 error tolerance for counting an activation wrong
  bool		graded;		// #DEF_false compute a graded reward signal as a function of number of correct output values
  bool		no_off_err;	// #DEF_false do not count a unit wrong if it is off but target says on -- only count wrong units that are on but should be off
  float		scalar_val_max;	// #CONDEDIT_ON_graded maximum value for scalar value output layers when using a graded value -- reward is error normalized by this value, and clipped at min/max

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(OutErrSpec);
private:
  void	Initialize()    { Defaults_init(); }
  void	Defaults_init() {
    err_tol = 0.5f;  graded = false;  no_off_err = false;  scalar_val_max = 1.0f;
  }
};


class STATE_CLASS(ExtRewSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for computing external rewards
INHERITED(SpecMemberBase)
public:
  float		err_val;	// #DEF_0 reward value for errors (when network does not respond correctly)
  float		norew_val;	// #DEF_0.5 reward value when no feedback information is present
  float		rew_val;	// #DEF_1 reward value for correct responses (positive rewards)

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(ExtRewSpec);
private:
  void	Initialize()    { Defaults_init(); }
  void	Defaults_init() {
    err_val = 0.0f;  norew_val = 0.5f;  rew_val = 1.0f;
  }
};


class STATE_CLASS(TwoDValSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for two-dimensional values
INHERITED(SpecMemberBase)
public:
  enum	RepType {
    GAUSSIAN,			// gaussian bump, with value = weighted average of tuned unit values
    LOCALIST			// each unit represents a distinct value; intermediate values represented by graded activity of neighbors; overall activity is weighted-average across all units
  };

  RepType	rep;		// type of representation of scalar value to use
  int		n_vals;		// number of distinct sets of X,Y values to represent in layer (i.e., if > 1, then multiple bumps are encoded -- uses peaks to locate values for multiple, and full weighted average for single value
  float		un_width;	// #CONDEDIT_ON_rep:GAUSSIAN sigma parameter of a gaussian specifying the tuning width of the coarse-coded units (in unit_range min-max units, unless norm_width is true, meaning use normalized 0-1 proportion of unit range)
  bool		norm_width;	// un_width is specified in normalized 0-1 proportion of unit range
  bool		clamp_pat;	// #DEF_false if true, environment provides full set of values to clamp over entire layer (instead of providing single scalar value to clamp on 1st unit, which then generates a corresponding distributed pattern)
  float		min_sum_act;	// #DEF_0.2 minimum total activity of all the units representing a value: when computing weighted average value, this is used as a minimum for the sum that you divide by
  float		mn_dst;		// #DEF_0.5 minimum distance factor for reading out multiple bumps: must be at least this times un_width far away from other bumps
  bool		clip_val;	// #DEF_true ensure that value remains within specified range

  float		x_min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		x_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		y_min;		// #READ_ONLY #NO_SAVE #NO_INHERIT minimum unit value
  float		y_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT range of unit values
  float		x_val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		y_val;		// #READ_ONLY #NO_SAVE #NO_INHERIT current val being represented (implementational, computed in InitVal())
  float		x_incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  float		y_incr;		// #READ_ONLY #NO_SAVE #NO_INHERIT increment per unit (implementational, computed in InitVal())
  int		x_size;		// #READ_ONLY #NO_SAVE #NO_INHERIT size of axis
  int		y_size;		// #READ_ONLY #NO_SAVE #NO_INHERIT size of axis
  float		un_width_x;	// #READ_ONLY #NO_SAVE #NO_INHERIT unit width, x axis (use for all computations -- can be normalized)
  float		un_width_y;	// #READ_ONLY #NO_SAVE #NO_INHERIT unit width, y axis (use for all computations -- can be normalized)

  INLINE virtual void   InitRange(float xmin, float xrng, float ymin, float yrng) {
    x_min = xmin; x_range = xrng; y_min = ymin; y_range = yrng;
    un_width_x = un_width;
    un_width_y = un_width;
    if(norm_width) {
      un_width_x *= x_range;
      un_width_y *= y_range;
    }
  }
  // initialize range values, including normalized unit width values per axis
  
  INLINE virtual void   InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng) {
    InitRange(xmin, xrng, ymin, yrng);
    x_val = xval; y_val = yval;
    x_size = xsize; y_size = ysize;
    x_incr = x_range / (float)(x_size - 1); // DON'T skip 1st row, and count end..
    y_incr = y_range / (float)(y_size - 1); // DON'T skip 1st row, and count end..
    //  incr -= .000001f;         // round-off tolerance..
  }
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  
  INLINE virtual float	GetUnitAct(int unit_idx) {
    int x_idx = unit_idx % x_size;
    int y_idx = (unit_idx / x_size);
    if(rep == GAUSSIAN) {
      float x_cur = x_min + x_incr * (float)x_idx;
      float x_dist = (x_cur - x_val) / un_width_x;
      float y_cur = y_min + y_incr * (float)y_idx;
      float y_dist = (y_cur - y_val) / un_width_y;
      float dist = x_dist * x_dist + y_dist * y_dist;
      return expf(-dist);
    }
    else if(rep == LOCALIST) {
      float x_cur = x_min + x_incr * (float)x_idx;
      float y_cur = y_min + y_incr * (float)y_idx;
      float x_dist = fabs(x_val - x_cur);
      float y_dist = fabs(y_val - y_cur);
      if((x_dist > x_incr) && (y_dist > y_incr)) return 0.0f;

      return 1.0f - .5 * ((x_dist / x_incr) + (y_dist / y_incr)); // todo: no idea if this is right.
    }
    return 0.0f;
  }
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  
  INLINE virtual void	GetUnitVal(int unit_idx, float& x_cur, float& y_cur) {
    int x_idx = unit_idx % x_size;
    int y_idx = (unit_idx / x_size);
    x_cur = x_min + x_incr * (float)x_idx;
    y_cur = y_min + y_incr * (float)y_idx;
  }
  // get target values associated with unit at given index: MUST CALL InitVal first!

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(TwoDValSpec);
private:
  void	Initialize()    {
    rep = GAUSSIAN;    n_vals = 1;    un_width = .3f;    norm_width = false;    clamp_pat = false;
    min_sum_act = 0.2f;    mn_dst = 0.5f;    clip_val = true;
    x_min = x_val = y_min = y_val = 0.0f;    x_range = x_incr = y_range = y_incr = 1.0f;
    x_size = y_size = 1;    un_width_x = un_width_y = un_width;
  }
  void	Defaults_init() { };
};


class STATE_CLASS(TwoDValBias) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(SpecMemberBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    BWT,			// bias value enters as a bias.wt
  };

  enum WeightBias {		// bias on weights into units
    NO_WT,			// no weight bias
    WT,				// input weights
  };

  UnitBias	un;		// bias on individual units
  float		un_gain;	// #CONDEDIT_OFF_un:NO_UN #DEF_1 gain multiplier (strength) of bias to apply for units.  WT = .03 as basic weight multiplier
  WeightBias	wt;		// bias on weights: always uses a val-shaped bias
  float		wt_gain;	// #CONDEDIT_OFF_wt:NO_WT #DEF_1 gain multiplier (strength) of bias to apply for weights (gain 1 = .03 wt value)
  float		x_val;		// X axis value location (center of gaussian bump)
  float		y_val;		// Y axis value location (center of gaussian bump)

  STATE_DECO_KEY("LayerSpec");
  STATE_TA_STD_CODE_SPEC(TwoDValBias);
private:
  void	Initialize()    {
    un = NO_UN;    un_gain = 1.0f;    wt = NO_WT;    wt_gain = 1.0f;    x_val = 0.0f;
    y_val = 0.0f;
  }
  void	Defaults_init() { };	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};
