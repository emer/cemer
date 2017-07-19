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

#ifndef LeabraLayer_h
#define LeabraLayer_h 1

// parent includes:
#include <Layer>
#include <LeabraInhib>

// member includes:
#include <LeabraLayerSpec>
#include <AvgMaxVals>
#include <LeabraUnGpData_List>
#include <DMemAggVars>
#include <float_Matrix>


// declare all other types mentioned but not required to include:
class LeabraUnGpData; // 
class LeabraNetwork; // 

eTypeDef_Of(LeabraLayer);

class E_API LeabraLayer : public Layer, public LeabraInhib {
  // #STEM_BASE ##CAT_Leabra #AKA_TwoDValLeabraLayer a Leabra Layer, which defines the primary scope of inhibitory competition among the units and unit groups that it contains
INHERITED(Layer)
public:
  enum TwoDXY {			// x-y two-d vals
    TWOD_X,			// the horizontal (X) value encoded in the layer
    TWOD_Y,			// the vertical (Y) value encoded in the layer
    TWOD_XY,			// number of xy vals (2)
  };

  enum TwoDValTypes {		// different values encoded in the twod_vals matrix
    TWOD_EXT,			// external inputs
    TWOD_TARG,			// target values
    TWOD_ACT,			// current activation
    TWOD_ACT_M,			// minus phase activations
    TWOD_ACT_P,			// plus phase activations
    TWOD_ACT_DIF,		// difference between plus and minus phase activations
    TWOD_ACT_M2,		// second minus phase activations
    TWOD_ACT_P2,		// second plus phase activations
    TWOD_ACT_DIF2,		// difference between second plus and minus phase activations
    TWOD_ERR,			// error from target: targ - act_m
    TWOD_SQERR,			// squared error from target: (targ - act_m)^2
    TWOD_N,			// number of val types to encode
  };
  
  LeabraLayerSpec_SPtr	spec;	// #CAT_Structure the spec for this layer: controls all functions of layer
  float         adapt_gi;   // #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS adaptive  inhibitory gain value -- this is an *extra* multiplier on top of existing gi value in the layer, unit inhib specs, starts out at 1 and moves from there -- adjusted by adaptive inhibition function -- saved with weight files
  float         margin_low_thr;   // #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS low threshold for marginal activation, in terms of v_m_eq -- adapts so that roughly acts_p_avg units on average are above this low threshold
  float         margin_med_thr;   // #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS medium threshold for marginal activation, in terms of v_m_eq -- adapts so that marginal units are roughly split in half by this threshold
  float         margin_hi_thr;   // #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS high threshold for marginal activation, in terms of v_m_eq -- adapts so that roughly margin.pct_marg * acts_p_avg units on average are between low and high threshold
  float         margin_low_avg; // #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS running-average computed proportion of units above the low_thr
  float         margin_med_avg; // #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS running-average computed proportion of units between the low and medium thresholds
  float         margin_hi_avg; // #READ_ONLY #SHOW #CAT_Activation #SAVE_WTS running-average computed proportion of units above the hi_thr
  bool		hard_clamped;	// #NO_SAVE #READ_ONLY #SHOW #CAT_Activation if true, indicates that this layer was actually hard clamped -- this is normally set by the Compute_HardClamp function called by Quarter_Init() or NewInputData_Init() -- see LayerSpec clamp.hard parameter to determine whether layer is hard clamped or not -- this flag is not to be manipulated directly
  String        minus_output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW name for the output produced by the network in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)
  float		da_p;           // #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning positive valence oriented dopamine-like modulatory value (where applicable)
  float		da_n;           // #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning positive valence oriented dopamine-like modulatory value (where applicable)
  float		sev;            // #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning serotonin-like modulatory value (where applicable)
  AvgMaxVals	avg_netin;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation minus-phase net input values for the layer, averaged over an epoch-level timescale
  AvgMaxVals	avg_netin_sum;	// #NO_SAVE #READ_ONLY #HIDDEN #CAT_Activation #DMEM_AGG_SUM sum of net input values for the layer, for computing average over an epoch-level timescale
  int		avg_netin_n;	// #NO_SAVE #READ_ONLY #HIDDEN #CAT_Activation #DMEM_AGG_SUM number of times sum is updated for computing average
  float		bin_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic binary  error value for this layer -- 1 if sse > network.stats.cnt_err_thr, else 0 -- this is useful for producing a cnt_err measure by aggregating trial log data
  float		max_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic max activity error value for this layer -- is 0 if the most active unit in the layer (or across unit groups if unit groups are present and inhibition uses them) has a non-zero target value -- otherwise 1 -- in other words, is the most active unit a target unit?  this only really makes sense for localist single-unit activity layers (although multiple units can be set to targets to allow for multiple options).  it is a highly sensitive measure, allowing for any other kinds of activity in the layer
  float		norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic normalized binary error value for this layer, computed subject to the parameters on the network
  Average	avg_norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average normalized binary error value (computed over previous epoch)
  float		cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) error on this trial for this layer, comparing targ vs. act_m
  float		cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) activation difference between act_p and act_m on this trial for this layer -- computed by Compute_CosDiff -- must be called after Quarter_Final in plus phase to get act_p values
  Average	avg_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) diff (computed over previous epoch)
  float		cos_diff_avg;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic running average of cosine (normalized dot product) difference between act_p and act_m -- computed with layerspec cos_diff.avg_tau time constant in Quarter_Final, and used for modulating hebbian learning (see cos_diff_avg_lrn) and overall learning rate
  float		cos_diff_var;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic running variance of cosine (normalized dot product) difference between act_p and act_m -- computed with layerspec cos_diff.diff_avg_tau time constant in Quarter_Final, used for modulating overall learning rate
  float		cos_diff_avg_lrn; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic 1.0 - cos_diff_avg and 0 for non-HIDDEN layers -- this is the value of cos_diff_avg used for avg_l.err_mod_l modulation of the avg_l_lrn factor if enabled
  float		lrate_mod;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning learning rate modulation factor based on layer_lrate for this layer, and cos_diff.lrate_mod result for this layer
  float		cos_err_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) error on this trial for this layer, comparing targ on this trial against activations from previous trial (act_q0) -- computed automatically during TI
  float		cos_err_vs_prv;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cos_err - cos_err_prv -- how much better is cosine error on this trial relative to just saying the same thing as was output last time -- for TI
  Average	avg_cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error (computed over previous epoch)
  Average	avg_cos_err_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on prv (see cos_err_prv) (computed over previous epoch)
  Average	avg_cos_err_vs_prv; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) error on vs prv (see cos_err_vs_prv) (computed over previous epoch)
  float		avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic average act_diff (act_p - act_m) for this layer -- this is an important statistic to track overall 'main effect' differences across phases 
  Average	avg_avg_act_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average avg_act_diff (computed over previous epoch)
  float		trial_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) trial-level activation difference between act_q4 and act_q0 on this trial for this layer -- computed by Compute_TrialCosDiff -- must be called after Quarter_Final in plus phase to get act_q4 values
  Average	avg_trial_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) trial diff (computed over previous epoch)
  float		net_sd;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail
  Average	avg_net_sd;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average net_sd (computed over previous epoch) -- standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail
  float         hog_pct;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic the percentage of units in the layer that have a long-time-averaged activitation level that is above the layerspec hog_thr threshold, indicating that they are 'hogging' the representational space (because this is computed on a time average, there is no epoch average of this statistic)
  float         dead_pct;           // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic the percentage of units in the layer that have a long-time-averaged activitation level that is below the layerspec dead_thr threshold, indicating that they are effectively 'dead' and not participating in any representations (because this is computed on a time average, there is no epoch average of this statistic)
  LeabraUnGpData_List ungp_data; // #NO_SAVE #NO_COPY #TREE_SHOW #HIDDEN #CAT_Activation unit group data (for inhibition computation and other things) -- allows actual unit groups to be virtual (virt_groups flag)
  LeabraUnGpData_List multigp_data; // #NO_SAVE #NO_COPY #TREE_SHOW #HIDDEN #CAT_Activation unit group data (for multi-unit-group inhibition computation and other things)
  LeabraUnGpData      laygp_data;   // #NO_SAVE #NO_COPY #TREE_SHOW #HIDDEN #CAT_Activation layer-group inhibition data -- first layer in a layer group that has lay_gp_inhib.on active has the data for the entire layer group
  float_Matrix	      twod_vals;    // #TREE_SHOW matrix of layer-encoded values, dimensions: [gp_y][gp_x][n_vals][TWOD_N][TWOD_XY] (outer to inner) -- gp_y and gp_x are group indices, size 1,1, for a layer with no unit groups


#ifdef DMEM_COMPILE
  DMemAggVars	dmem_agg_sum;		// #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void 	DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void	DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem 
#endif

  void  CheckSpecs() override;
  void	BuildUnits() override;

  LeabraUnGpData* 	UnGpData(int gpidx)
  { return ungp_data.SafeEl(gpidx); }
  // #CAT_Structure get unit group data structure for given unit group index
  LeabraUnGpData* 	MultiGpData(int gpidx)
  { return multigp_data.SafeEl(gpidx); }
  // #CAT_Structure get multi unit group data structure for given unit group index
  LeabraUnGpData* 	UnGpDataUn(Unit* un)
  { return ungp_data.SafeEl(un->ug_idx); }
  // #CAT_Structure get unit group data structure for unit group for given unit
  LeabraUnGpData* 	MultiGpDataUn(Unit* un)
  { return multigp_data.SafeEl(un->ug_idx); }
  // #CAT_Structure get unit group data structure for unit group for given unit
  bool          	HasUnitGpInhib()
  { return spec->HasUnitGpInhib(this); }
  // #CAT_Structure does the layer have unit group inhibition engaged?

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	Init_Weights_Layer(Network* net) override
  { if(spec) spec->Init_Weights_Layer(this, (LeabraNetwork*)net); }
  // #CAT_Learning layer-level initialization taking place after Init_Weights on units
  virtual void	Init_Acts_Layer(LeabraNetwork* net)
  { if(spec) spec->Init_Acts_Layer(this, net); }
  // #CAT_Activation layer-level initialization taking place after Init_Acts on units
  virtual void	Init_AdaptInhib(LeabraNetwork* net)
  { if(spec) spec->Init_AdaptInhib(this, net); }
  // #CAT_Activation initialize adaptive inhibition parameters -- undoes any existing adaptation of inhibitory gain for the layer

  virtual void	DecayState(LeabraNetwork* net, float decay)
  { if(spec) spec->DecayState(this, net, decay); }
  // #CAT_Activation decay the state of this layer -- not normally called but available for programs etc to control specific layers

  void  Init_InputData(Network* net) override;

  void  Init_Stats(Network* net) override
  { return spec->Init_Stats(this, (LeabraNetwork*)net); }

  void	CheckInhibCons(LeabraNetwork* net);
  // #CAT_Structure check for inhibitory connections -- sets flag on network
  
  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  void	Trial_Init_Specs(LeabraNetwork* net) { spec->Trial_Init_Specs(this, net); }
  // #CAT_Learning initialize specs and specs update network flags 
  void	Trial_Init_Layer(LeabraNetwork* net) { spec->Trial_Init_Layer(this, net); }
  // #CAT_Learning layer-level trial init
  void	Compute_HardClamp_Layer(LeabraNetwork* net)
  { spec->Compute_HardClamp_Layer(this, net); }
  // #CAT_Activation hard clamp, layer level

  ///////////////////////////////////////////////////////////////////////
  //	QuarterInit -- at start of settling

  void	Quarter_Init_Layer(LeabraNetwork* net)	{ spec->Quarter_Init_Layer(this, net); }
  // #CAT_Activation initialize start of a setting phase: all layer-level misc init takes place here (calls TargFlags_Layer) -- other stuff all done directly in Quarter_Init_Units call

  void	Quarter_Init_TargFlags_Layer(LeabraNetwork* net)
  { spec->Quarter_Init_TargFlags_Layer(this, net); }
  // #IGNORE layer-level initialize start of a setting phase, set input flags appropriately, etc

  void	ExtToComp(LeabraNetwork* net)		{ spec->ExtToComp(this, net); }
  // #CAT_Activation change external inputs to comparisons (remove input)
  void	TargExtToComp(LeabraNetwork* net)	{ spec->TargExtToComp(this, net); }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  // main computation is direct Send_NetinDelta call on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  void	Compute_Inhib(LeabraNetwork* net, int thread_no=-1)
  { spec->Compute_Inhib(this, net, thread_no); }
  // #CAT_Activation compute the inhibition for layer -- called threaded
  void	Compute_LayInhibToGps(LeabraNetwork* net)
  { spec->Compute_LayInhibToGps(this, net); }
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  void	Compute_CycleStats_Pre(LeabraNetwork* net)
  { spec->Compute_CycleStats_Pre(this, net); }
  // #CAT_Statistic pre-cycle-stats -- done in single thread prior to cycle stats -- good place to intervene for whole-layer dynamics
  void	Compute_OutputName(LeabraNetwork* net)
  { spec->Compute_OutputName(this, net); }
  // #CAT_Statistic compute output name based on most active unit name

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats -- optional non-default guys


  ///////////////////////////////////////////////////////////////////////
  //	Quarter_Final

  void	Quarter_Final_Pre(LeabraNetwork* net)	{ spec->Quarter_Final_Pre(this, net); }
  // #CAT_Activation perform computations in layers at end of settling -- this is a pre-stage that occurs prior to final Quarter_Final -- use this for anything that needs to happen prior to the standard Quarter_Final across layers (called by Quarter_Final)
  // #CAT_Activation after settling, keep track of phase variables, etc.
  void	Quarter_Final_Layer(LeabraNetwork* net)	{ spec->Quarter_Final_Layer(this, net); }
  // #CAT_Activation after settling, keep track of phase variables, etc.


  ///////////////////////////////////////////////////////////////////////
  //	TrialFinal


  ///////////////////////////////////////////////////////////////////////
  //	Learning

  void	Compute_dWt_Layer_pre(LeabraNetwork* net)  { spec->Compute_dWt_Layer_pre(this, net); }
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  ///////////////////////////////////////////////////////////////////////
  //	Trial-level Stats

  float Compute_SSE(Network* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false) override
  { return spec->Compute_SSE(this, (LeabraNetwork*)net, n_vals, unit_avg, sqrt); }

  float Compute_NormErr(LeabraNetwork* net)
  { return spec->Compute_NormErr(this, net); }
  // #CAT_Statistic compute normalized binary error across layer (returns normalized value or -1 for not applicable, averaged at network level -- see layerspec for more info)

  float Compute_MaxErr(LeabraNetwork* net)
  { return spec->Compute_MaxErr(this, net); }
  // #CAT_Statistic compute max_err, across unit groups (if present and used) and the entire layer

  float Compute_CosErr(LeabraNetwork* net, int& n_vals)
  { return spec->Compute_CosErr(this, net, n_vals); }
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m over the layer -- n_vals is number of units contributing

  float Compute_CosDiff(LeabraNetwork* net)
  { return spec->Compute_CosDiff(this, net); }
  // #CAT_Statistic compute cosine (normalized dot product) of phase activation difference in this layer: act_p compared to act_m -- must be called after Quarter_Final for plus phase to get the act_p values
  void Compute_CosDiff_post(LeabraNetwork* net)
  { spec->Compute_CosDiff_post(this, net); }
  // #CAT_Statistic post step of cos_diff -- needed for sharing cos_diff based lrate mod
  float Compute_AvgActDiff(LeabraNetwork* net)
  { return spec->Compute_AvgActDiff(this, net); }
  // #CAT_Statistic compute average act_diff (act_p - act_m) for this layer -- must be called after Quarter_Final for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  float Compute_TrialCosDiff(LeabraNetwork* net)
  { return spec->Compute_TrialCosDiff(this, net); }
  // #CAT_Statistic compute cosine (normalized dot product) of trial activaiton difference in this layer: act_q4 compared to act_q0 -- must be called after Quarter_Final for plus phase to get the act_q4 values
  void Compute_ActMargin(LeabraNetwork* net)
  { spec->Compute_ActMargin(this, net); }
  // #CAT_Statistic compute activation margin stats and adapt thresholds
  float Compute_NetSd(LeabraNetwork* net)
  { return spec->Compute_NetSd(this, net); }
  // #CAT_Statistic compute standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail -- must be called at end of minus phase
  void Compute_HogDeadPcts(LeabraNetwork* net)
  { spec->Compute_HogDeadPcts(this, net); }
  // #CAT_Statistic compute percentage of units in the layer that have a long-time-averaged activitation level that is above or below hog / dead thresholds, indicating that they are either 'hogging' the representational space, or 'dead' and not participating in any representations

  void  Compute_EpochStats(Network* net) override
  { return spec->Compute_EpochStats(this, (LeabraNetwork*)net); }

  virtual LeabraUnit* GetMostActiveUnit();
  // #CAT_Statistic return the most active unit in the layer according to current activation statistics -- could be NULL if no acts.max_i set right now

  ////////////////////////////////////////////////////////////////////////////////
  //	Parameter Adaptation over longer timesales

  void	Compute_AbsRelNetin(LeabraNetwork* net)	{ spec->Compute_AbsRelNetin(this, net); }
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  void	Compute_AvgAbsRelNetin(LeabraNetwork* net) { spec->Compute_AvgAbsRelNetin(this, net); }
  // #CAT_Statistic compute the average absolute layer-level and relative netinput from different projections into this layer (over an epoch-level timescale)

  void	ClearDeepActs(LeabraNetwork* net)	{ spec->ClearDeepActs(this, net); }
  // #CAT_TI clear the deep lamina variables -- can be useful to do at discontinuities of experience

  ////////////////////////////////////////////
  //	TwoD Misc structural routines

  virtual bool   TwoDValMode();
  // #CAT_TwoD are we operating in TwoD value spec mode?  i.e., the layerspec is set to a TwoDValLayerSpec
  
  inline float	GetTwoDVal(TwoDXY xy, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    return twod_vals.SafeElAsFloat(xy, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD get a two-d value encoded in the twod_vals data 
  inline void	GetTwoDVals(float& x_val, float& y_val, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    x_val = twod_vals.SafeElAsFloat(TWOD_X, val_typ, val_no, gp_x, gp_y);
    y_val = twod_vals.SafeElAsFloat(TWOD_Y, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD get a two-d value encoded in the twod_vals data 

  inline void	SetTwoDVal(const Variant& val, TwoDXY xy, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    twod_vals.SetFmVar(val, xy, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD set a two-d value encoded in the twod_vals data 
  inline void	SetTwoDVals(const Variant& x_val, const Variant& y_val, TwoDValTypes val_typ, int val_no, int gp_x=0, int gp_y=0) {
    twod_vals.SetFmVar(x_val, TWOD_X, val_typ, val_no, gp_x, gp_y);
    twod_vals.SetFmVar(y_val, TWOD_Y, val_typ, val_no, gp_x, gp_y);
  }
  // #CAT_TwoD set both two-d values encoded in the twod_vals data 

  virtual void		UpdateTwoDValsGeom();
  // update the twod_vals geometry based on current layer and layer spec settings

  void	ApplyInputData_2d(taMatrix* data, UnitVars::ExtFlags ext_flags,
				  Random* ran, const taVector2i& offs, bool na_by_range=false) override;
  void	ApplyInputData_Flat4d(taMatrix* data, UnitVars::ExtFlags ext_flags,
				      Random* ran, const taVector2i& offs, bool na_by_range=false) override;
  void	ApplyInputData_Gp4d(taMatrix* data, UnitVars::ExtFlags ext_flags,
				    Random* ran, bool na_by_range=false) override;
  
  ////////////////////////////////////////////
  //	Misc structural routines

  bool          SetLayerSpec(LayerSpec* sp) override;
  LayerSpec*    GetLayerSpec() override	{ return (LayerSpec*)spec.SPtr(); }
  
  String        GetToolbarName() const override { return "layer"; }

  inline float  GetTotalActEq()  { return acts_eq.avg * (float)units.size; }
  // Get the total act_eq activation in the layer based on average and number of units
  inline float  GetTotalActQ0()  { return acts_q0.avg * (float)units.size; }
  // Get the total act_q0 activation in the layer based on average and number of units

  void	InitLinks() override;
  void	CutLinks() override;
  void	Copy_(const LeabraLayer& cp);
  TA_BASEFUNS(LeabraLayer);
protected:
  void	UpdateAfterEdit_impl() override;
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void	Initialize();
  void	Destroy()		{ CutLinks(); }
};

TA_SMART_PTRS(E_API, LeabraLayer);

#endif // LeabraLayer_h
