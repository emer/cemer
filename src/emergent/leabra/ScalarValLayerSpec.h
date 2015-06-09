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

#ifndef ScalarValLayerSpec_h
#define ScalarValLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:
#include <MinMaxRange>

// declare all other types mentioned but not required to include:

// NOTE: got rid of sum_bar -- never worked and requires lots of different params
// to support in the future it should probably be a subclass of the main guy
// misc todo: items for SUM_BAR:
// to do equivalent of "clamp_value" (e.g., LV units at end of settle), add a special
// mode where asymptotic Vm is computed based on current params, and act from that, etc.

// also possible: unit subgroups that all have the same gc_i value, but different random
// connectivity from inputs: sending units all send to a fixed # (permute) of these 
// group units, producing a random sensory representation.  not really necc. for S2 
// spikes, because of syndep..

eTypeDef_Of(ScalarValSpec);

class E_API ScalarValSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
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
  bool          lrnmod_clamp;   // if true, then any lrnmod signals received in the plus phase will cause the lrnmod value in the first unit to be clamped over the whole layer -- this is a way for other layers to send clamping values to this layer, outside of external clamping
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

  virtual void	InitRange(float umin, float urng);
  // initialize range values (also sets un_width_eff)
  virtual void	InitVal(float sval, int ugp_size, float umin, float urng);
  // initialize implementational values for subsequently computing GetUnitAct to represent scalar val sval over unit group of ugp_size
  virtual float	GetUnitAct(int unit_idx);
  // get activation under current representation for unit at given index: MUST CALL InitVal first!
  virtual float	GetUnitVal(int unit_idx);
  // get target value associated with unit at given index: MUST CALL InitVal first!

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(ScalarValSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(ScalarValBias);

class E_API ScalarValBias : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra initial bias for given activation value for scalar value units
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

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  SIMPLE_COPY(ScalarValBias);
  TA_BASEFUNS(ScalarValBias);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { }; 	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(ScalarValLayerSpec);

class E_API ScalarValLayerSpec : public LeabraLayerSpec {
  // represents a scalar value using a coarse-coded distributed code over units.  the external input to the first unit is used to generate distributed inputs to the rest of the units, but unlike in earlier versions, all the units represent the distributed representation - the first unit is not just for display anymore, though it does contain the scalar readout val in act_eq and in act_m and act_p
INHERITED(LeabraLayerSpec)
public:
  ScalarValSpec	 scalar;	// specifies how values are represented in terms of distributed patterns of activation across the layer
  MinMaxRange	 unit_range;	// range of values represented across the units; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  ScalarValBias	 bias_val;	// specifies bias for given value (as gaussian bump) 
  MinMaxRange    avg_act_range; // #CONDSHOW_ON_scalar.rep:AVG_ACT range of variability of the average layer activity, used for AVG_ACT type to renormalize acts.avg before projecting it into the unit_range of values
  MinMaxRange	 val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  virtual void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal initialize the bias value 
    virtual void Compute_WtBias_Val(LeabraLayer* lay, LeabraNetwork* net,
                                    Layer::AccessMode acc_md, int gpidx, float val);
    // #IGNORE
    virtual void Compute_UnBias_Val(LeabraLayer* lay, LeabraNetwork* net,
                                    Layer::AccessMode acc_md, int gpidx, float val);
    // #IGNORE
    virtual void Compute_UnBias_NegSlp(LeabraLayer* lay, LeabraNetwork* net,
                                       Layer::AccessMode acc_md, int gpidx);
    // #IGNORE
    virtual void Compute_UnBias_PosSlp(LeabraLayer* lay, LeabraNetwork* net,
                                       Layer::AccessMode acc_md, int gpidx);
    // #IGNORE

  virtual void	ClampValue_ugp(LeabraLayer* lay, LeabraNetwork* net,
                               Layer::AccessMode acc_md, int gpidx, float rescale=1.0f);
  // #CAT_ScalarVal clamp value in the first unit's ext field to the units in the group
  virtual float	ClampAvgAct(int ugp_size);
  // #CAT_ScalarVal computes the average activation for a clamped unit pattern (for computing rescaling)
  virtual void	ReadValue(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal read out current value represented by activations in layer
    virtual float ReadValue_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                Layer::AccessMode acc_md, int gpidx);
    // #CAT_ScalarVal unit group version: read out current value represented by activations in layer
  virtual void 	Compute_ExtToPlus_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                      Layer::AccessMode acc_md, int gpidx);
  // #CAT_ScalarVal copy ext values to act_p -- used for internally-generated training signals for learning in several subclasses
  virtual void 	Compute_LrnModToExt_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                         Layer::AccessMode acc_md, int gpidx);
  // #CAT_ScalarVal copy lrnmod values to ext -- used for lrnmod_clamp option
  virtual void 	Compute_ExtToAct_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                     Layer::AccessMode acc_md, int gpidx);
  // #CAT_ScalarVal copy ext values to act -- used for dynamically computed clamped layers
  virtual void HardClampExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal hard clamp current ext values (on all units, after ClampValue called) to all the units
    virtual void HardClampExt_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                  Layer::AccessMode acc_md, int gpidx);
    // #IGNORE

  virtual void	LabelUnits(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_ScalarVal label units in given layer with their underlying values
    virtual void LabelUnits_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                Layer::AccessMode acc_md, int gpidx);
    // #CAT_ScalarVal label units with their underlying values
  virtual void	LabelUnitsNet(LeabraNetwork* net);
  // #BUTTON #CAT_ScalarVal label all layers in given network using this spec

  void  Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net) override;
  void	Quarter_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) override;
  void	Compute_HardClamp_Layer(LeabraLayer* lay, LeabraNetwork* net) override;

  void Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net) override;

  float Compute_SSE(LeabraLayer* lay, LeabraNetwork* net, int& n_vals,
                    bool unit_avg = false, bool sqrt = false) override;
  virtual float Compute_SSE_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                Layer::AccessMode acc_md, int gpidx, int& n_vals);
    // #IGNORE
  float Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) override;
  virtual float Compute_NormErr_ugp(LeabraLayer* lay, LeabraNetwork* net,
                                    Layer::AccessMode acc_md, int gpidx);
    // #IGNORE

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(ScalarValLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // ScalarValLayerSpec_h
