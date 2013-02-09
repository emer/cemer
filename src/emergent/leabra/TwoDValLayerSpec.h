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

#ifndef TwoDValLayerSpec_h
#define TwoDValLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:
#include <TwoDValLeabraLayer>
#include <MinMaxRange>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TwoDValSpec);

class E_API TwoDValSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for two-dimensional values
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

  virtual void	InitRange(float xmin, float xrng, float ymin, float yrng);
  // initialize range values, including normalized unit width values per axis
  virtual void	InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual void	GetUnitVal(int unit_idx, float& x_cur, float& y_cur);
  // get target values associated with unit at given index: MUST CALL InitVal first!

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  SIMPLE_COPY(TwoDValSpec);
  TA_BASEFUNS(TwoDValSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

eTypeDef_Of(TwoDValBias);

class E_API TwoDValBias : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
INHERITED(SpecMemberBase)
public:
  enum UnitBias {		// bias on individual units
    NO_UN,			// no unit bias
    GC,				// bias value enters as a conductance in gc.h or gc.a
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

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  SIMPLE_COPY(TwoDValBias);
  TA_BASEFUNS(TwoDValBias);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(TwoDValLayerSpec);

class E_API TwoDValLayerSpec : public LeabraLayerSpec {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid -- requires TwoDValLeabraLayer to encode values (no longer using first row of units)
INHERITED(LeabraLayerSpec)
public:
  TwoDValSpec	 twod;		// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 x_range;	// range of values represented across the X (horizontal) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  MinMaxRange	 y_range;	// range of values represented across the Y (vertical) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  TwoDValBias	 bias_val;	// specifies bias values
  MinMaxRange	 x_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)
  MinMaxRange	 y_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	ClampValue_ugp(TwoDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
			       LeabraNetwork* net, float rescale=1.0f);
  // #CAT_TwoDVal clamp value in the first unit's ext field to the units in the group
  virtual void	ReadValue(TwoDValLeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal read out current value represented by activations in layer
    virtual void ReadValue_ugp(TwoDValLeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
			       LeabraNetwork* net);
    // #CAT_TwoDVal unit group version: read out current value represented by activations in layer
  virtual void	HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal hard clamp current ext values (on all units, after ClampValue called) to all the units (calls ResetAfterClamp)

  virtual void	LabelUnits(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal label units in given layer with their underlying values
    virtual void LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx);
    // #CAT_TwoDVal label units with their underlying values
  virtual void	LabelUnitsNet(LeabraNetwork* net);
  // #BUTTON #CAT_TwoDVal label all layers in given network using this spec

  virtual void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TwoDVal initialize the bias value 
    virtual void Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				    float x_val, float y_val);
    // #IGNORE
    virtual void Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				    float x_val, float y_val);
    // #IGNORE

  override void Init_Weights(LeabraLayer* lay, LeabraNetwork* net);
  override void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
    override void Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net);
    virtual void Settle_Init_TargFlags_Layer_ugp(TwoDValLeabraLayer* lay,
						 Layer::AccessMode acc_md, int gpidx,
						 LeabraNetwork* net);
    // #IGNORE
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
    virtual void PostSettle_ugp(TwoDValLeabraLayer* lay,
				Layer::AccessMode acc_md, int gpidx, LeabraNetwork* net);
    // #CAT_TwoDVal unit group version: update variables based on phase

  override float Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false);
    virtual float Compute_SSE_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				  int& n_vals);
    // #IGNORE
  override float Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
    virtual float Compute_NormErr_ugp(LeabraLayer* lay,
				      Layer::AccessMode acc_md, int gpidx,
				      LeabraInhib* thr, LeabraNetwork* net);
    // #IGNORE

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(TwoDValLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // TwoDValLayerSpec_h
