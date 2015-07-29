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


// declare all other types mentioned but not required to include:
class LeabraUnGpData; // 
class LeabraNetwork; // 

eTypeDef_Of(LeabraLayer);

class E_API LeabraLayer : public Layer, public LeabraInhib {
  // #STEM_BASE ##CAT_Leabra a Leabra Layer, which defines the primary scope of inhibitory competition among the units and unit groups that it contains
INHERITED(Layer)
public:
  LeabraLayerSpec_SPtr	spec;	// #CAT_Structure the spec for this layer: controls all functions of layer
  bool		hard_clamped;	// #NO_SAVE #READ_ONLY #SHOW #CAT_Activation if true, indicates that this layer was actually hard clamped -- this is normally set by the Compute_HardClamp function called by Quarter_Init() or NewInputData_Init() -- see LayerSpec clamp.hard parameter to determine whether layer is hard clamped or not -- this flag is not to be manipulated directly
  String        minus_output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW name for the output produced by the network in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)
  float		da_p;           // #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning positive valence oriented dopamine-like modulatory value (where applicable)
  float		da_n;           // #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning positive valence oriented dopamine-like modulatory value (where applicable)
  float		sev;            // #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning serotonin-like modulatory value (where applicable)
  AvgMaxVals	avg_netin;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation net input values for the layer, averaged over an epoch-level timescale
  AvgMaxVals	avg_netin_sum;	// #NO_SAVE #READ_ONLY #HIDDEN #CAT_Activation #DMEM_AGG_SUM sum of net input values for the layer, for computing average over an epoch-level timescale
  int		avg_netin_n;	// #NO_SAVE #READ_ONLY #HIDDEN #CAT_Activation #DMEM_AGG_SUM number of times sum is updated for computing average
  float		norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic normalized binary error value for this layer, computed subject to the parameters on the network
  Average	avg_norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average normalized binary error value (computed over previous epoch)
  float		cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) error on this trial for this layer, comparing targ vs. act_m
  float		cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) activation difference between act_p and act_m on this trial for this layer -- computed by Compute_CosDiff -- must be called after Quarter_Final in plus phase to get act_p values
  Average	avg_cos_diff;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average cosine (normalized dot product) diff (computed over previous epoch)
  float		cos_diff_avg;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic running average cosine (normalized dot product) difference between act_p and act_m -- computed with decay.cos_diff_avg_tau time constant in Quarter_Final 
  float		cos_diff_avg_lrn; // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic 1.0 - cos_diff_avg and 0 for non-HIDDEN layers -- this is the value of cos_diff_avg used for avg_l.err_mod_l modulation of the avg_l_lrn factor if enabled
  float		lrate_mod;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Learning learning rate modulation factor based on cos_diff_avg_lrn for this layer
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
  LeabraUnGpData_List ungp_data; // #NO_SAVE #NO_COPY #SHOW_TREE #HIDDEN #CAT_Activation unit group data (for inhibition computation and other things) -- allows actual unit groups to be virtual (virt_groups flag)
  LeabraUnGpData_List multigp_data; // #NO_SAVE #NO_COPY #SHOW_TREE #HIDDEN #CAT_Activation unit group data (for multi-unit-group inhibition computation and other things)

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
  virtual void	Init_Acts_Layer(Network* net)
  { if(spec) spec->Init_Acts_Layer(this, (LeabraNetwork*)net); }
  // #CAT_Activation layer-level initialization taking place after Init_Acts on units

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

  float Compute_CosErr(LeabraNetwork* net, int& n_vals)
  { return spec->Compute_CosErr(this, net, n_vals); }
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m over the layer -- n_vals is number of units contributing

  float Compute_CosDiff(LeabraNetwork* net)
  { return spec->Compute_CosDiff(this, net); }
  // #CAT_Statistic compute cosine (normalized dot product) of phase activation difference in this layer: act_p compared to act_m -- must be called after Quarter_Final for plus phase to get the act_p values
  float Compute_AvgActDiff(LeabraNetwork* net)
  { return spec->Compute_AvgActDiff(this, net); }
  // #CAT_Statistic compute average act_diff (act_p - act_m) for this layer -- must be called after Quarter_Final for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  float Compute_TrialCosDiff(LeabraNetwork* net)
  { return spec->Compute_TrialCosDiff(this, net); }
  // #CAT_Statistic compute cosine (normalized dot product) of trial activaiton difference in this layer: act_q4 compared to act_q0 -- must be called after Quarter_Final for plus phase to get the act_q4 values
  float Compute_NetSd(LeabraNetwork* net)
  { return spec->Compute_NetSd(this, net); }
  // #CAT_Statistic compute standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail -- must be called at end of minus phase
  void Compute_HogDeadPcts(LeabraNetwork* net)
  { spec->Compute_HogDeadPcts(this, net); }
  // #CAT_Statistic compute percentage of units in the layer that have a long-time-averaged activitation level that is above or below hog / dead thresholds, indicating that they are either 'hogging' the representational space, or 'dead' and not participating in any representations

  void  Compute_EpochStats(Network* net) override
  { return spec->Compute_EpochStats(this, (LeabraNetwork*)net); }

  
  ////////////////////////////////////////////////////////////////////////////////
  //	Parameter Adaptation over longer timesales

  void	Compute_AbsRelNetin(LeabraNetwork* net)	{ spec->Compute_AbsRelNetin(this, net); }
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  void	Compute_AvgAbsRelNetin(LeabraNetwork* net) { spec->Compute_AvgAbsRelNetin(this, net); }
  // #CAT_Statistic compute the average absolute layer-level and relative netinput from different projections into this layer (over an epoch-level timescale)

  void	ClearDeepActs(LeabraNetwork* net)	{ spec->ClearDeepActs(this, net); }
  // #CAT_TI clear the deep lamina variables -- can be useful to do at discontinuities of experience

  ////////////////////////////////////////////
  //	Misc structural routines

  bool		SetLayerSpec(LayerSpec* sp);
  LayerSpec*	GetLayerSpec()		{ return (LayerSpec*)spec.SPtr(); }

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LeabraLayer& cp);
  TA_BASEFUNS(LeabraLayer);
protected:
  void	UpdateAfterEdit_impl();
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void	Initialize();
  void	Destroy()		{ CutLinks(); }
};

TA_SMART_PTRS(E_API, LeabraLayer);

#endif // LeabraLayer_h
