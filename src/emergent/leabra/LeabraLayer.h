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
#include <LeabraUnit_Group>
#include <LeabraLayerSpec>
#include <LeabraSort>
#include <AvgMaxVals>
#include <CtSRAvgVals>
#include <LeabraUnGpData_List>
#include <int_Array>
#include <KwtaSortBuff_List>
#include <DMemAggVars>


// declare all other types mentioned but not required to include:
class LeabraUnGpData; // 
class LeabraNetwork; // 

eTypeDef_Of(LeabraLayer);

class E_API LeabraLayer : public Layer, public LeabraInhib {
  // #STEM_BASE ##CAT_Leabra layer that implements the Leabra algorithms
INHERITED(Layer)
public:
  LeabraLayerSpec_SPtr	spec;	// #CAT_Structure the spec for this layer: controls all functions of layer
  bool		hard_clamped;	// #NO_SAVE #READ_ONLY #SHOW #CAT_Activation if true, indicates that this layer was actually hard clamped -- this is normally set by the Compute_HardClamp function called by Settle_Init() or NewInputData_Init() -- see LayerSpec clamp.hard parameter to determine whether layer is hard clamped or not -- this flag is not to be manipulated directly
  CtSRAvgVals	sravg_vals;	// #NO_SAVE #CAT_Learning sender-receiver average activation accumulation values -- for normalizing averages and state field provides ultimate determination of when and how sravg is computed
  float		avg_l_avg;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation layer-wise average of avg_l values in the layers
  float		dav;		// #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning dopamine-like modulatory value (where applicable)
  AvgMaxVals	avg_netin;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation net input values for the layer, averaged over an epoch-level timescale
  AvgMaxVals	avg_netin_sum;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation #DMEM_AGG_SUM sum of net input values for the layer, for computing average over an epoch-level timescale
  int		avg_netin_n;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Activation #DMEM_AGG_SUM number of times sum is updated for computing average
  float		norm_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic normalized binary error value for this layer, computed subject to the parameters on the network
  float		cos_err;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic cosine (normalized dot product) error on this trial for this layer
  int		da_updt;	// #NO_SAVE #READ_ONLY #EXPERT #CAT_Learning true if da triggered an update (either + to store or - reset)
  LeabraUnGpData_List ungp_data; // #NO_SAVE #NO_COPY #SHOW_TREE #HIDDEN #CAT_Activation unit group data (for kwta computation and other things) -- allows actual unit groups to be virtual (virt_groups flag)
  int_Array	unit_idxs;	// #NO_SAVE #HIDDEN #CAT_Activation -- set of unit indexes typically used for permuted selection of units (e.g., k_pos_noise) -- can be used by other functions too

  KwtaSortBuff_List lay_kbuffs;	// #NO_SAVE #HIDDEN #NO_COPY #CAT_Activation layer-wide kwta computation buffers
  KwtaSortBuff_List gp_kbuffs;	// #NO_SAVE #HIDDEN #NO_COPY #CAT_Activation subgroup-specific computation buffers

#ifdef DMEM_COMPILE
  DMemAggVars	dmem_agg_sum;		// #IGNORE aggregation of network variables using SUM op (currently only OP in use -- add others as needed)
  virtual void 	DMem_InitAggs();
  // #IGNORE initialize aggregation stuff
  virtual void	DMem_ComputeAggs(MPI_Comm comm);
  // #IGNORE aggregate network variables across procs for trial-level dmem 
#endif

  override void CheckSpecs();
  override void	BuildUnits();
  override void BuildUnits_Threads(Network* net)
  { if(spec) spec->BuildUnits_Threads(this, (LeabraNetwork*)net); }
  virtual  void	BuildKwtaBuffs();
  // #IGNORE build kwta buffers etc -- needs to be done at load and build time

  KwtaSortBuff* 	SortBuff(AccessMode acc_md, KwtaSortBuff_List::StdSortBuffs buff) {
    if(acc_md == ACC_GP) return gp_kbuffs.FastEl(buff);
    return lay_kbuffs.FastEl(buff);
  }
  // #CAT_Activation #IGNORE get kwta sort buffer for given access mode (gp or layer) and buffer type
  KwtaSortBuff_List* 	SortBuffList(AccessMode acc_md) {
    if(acc_md == ACC_GP) return &gp_kbuffs;
    return &lay_kbuffs;
  }
  // #CAT_Activation get kwta sort buffer list for given access mode (gp or layer)
  LeabraUnGpData* 	UnGpData(int gpidx) {
    return ungp_data.SafeEl(gpidx);
  }
  // #CAT_Structure get unit group data structure for given unit group index

  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  void	SetLearnRule(LeabraNetwork* net) 	{ if(spec) spec->SetLearnRule(this, net); }
  // #CAT_Learning set current learning rule from the network

  override void	Init_Weights(Network* net)
  { if(spec) spec->Init_Weights(this, (LeabraNetwork*)net); }
  // #CAT_Learning initialize weight values and other permanent state

  void	Init_ActAvg(LeabraNetwork* net) 	{ spec->Init_ActAvg(this, net); }
  // #CAT_Activation initialize act_avg values
  void	Init_Netins(LeabraNetwork* net)		{ spec->Init_Netins(this, net); }
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)

  override void  Init_InputData(Network* net);

  override void	Init_Acts(Network* net)
  { if(spec) spec->Init_Acts(this, (LeabraNetwork*)net); }
  // #CAT_Activation initialize unit-level dynamic state variables (activations, etc)

  void	DecayState(LeabraNetwork* net, float decay) { spec->DecayState(this, net, decay); }
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  void	CheckInhibCons(LeabraNetwork* net);
  // #CAT_Structure check for inhibitory connections -- sets flag on network
  
  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  void	SetCurLrate(LeabraNetwork* net, int epoch) { spec->SetCurLrate(this, net, epoch); }
  // #CAT_Learning set current learning rate based on epoch
  void	Trial_Init_Layer(LeabraNetwork* net) { spec->Trial_Init_Layer(this, net); }
  // #CAT_Learning layer-level trial init
    void	Trial_DecayState(LeabraNetwork* net)
    { spec->Trial_DecayState(this, net); }
    // #CAT_Activation NOT CALLED DURING STD PROCESSING decay activations and other state between events
    void	Trial_NoiseInit(LeabraNetwork* net)
    { spec->Trial_NoiseInit(this, net); }
    // #CAT_Activation NOT CALLED DURING STD PROCESSING initialize various noise factors at start of trial
    void 	Trial_Init_SRAvg(LeabraNetwork* net)
    { spec->Trial_Init_SRAvg(this, net); }
    // #CAT_Learning NOT CALLED DURING STD PROCESSING initialize sending-receiving activation product averages (CtLeabra_X/CAL)

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  void	Compute_Active_K(LeabraNetwork* net)	{ spec->Compute_Active_K(this, net); }
  // #CAT_Activation prior to settling: compute actual activity levels based on spec, inputs, etc

  void	Settle_Init_Layer(LeabraNetwork* net)	{ spec->Settle_Init_Layer(this, net); }
  // #CAT_Activation initialize start of a setting phase: all layer-level misc init takes place here (calls TargFlags_Layer) -- other stuff all done directly in Settle_Init_Units call

  void	Settle_Init_TargFlags(LeabraNetwork* net) { spec->Settle_Init_TargFlags(this, net); }
  // #CAT_Activation initialize start of a setting phase, set input flags appropriately, etc
    void	Settle_Init_TargFlags_Layer(LeabraNetwork* net)
    { spec->Settle_Init_TargFlags_Layer(this, net); }
    // #IGNORE layer-level initialize start of a setting phase, set input flags appropriately, etc
  void	Compute_HardClamp(LeabraNetwork* net) 	{ spec->Compute_HardClamp(this, net); }
  // #CAT_Activation prior to settling: hard-clamp inputs

  void	ExtToComp(LeabraNetwork* net)		{ spec->ExtToComp(this, net); }
  // #CAT_Activation change external inputs to comparisons (remove input)
  void	TargExtToComp(LeabraNetwork* net)	{ spec->TargExtToComp(this, net); }
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  // main computation is direct Send_NetinDelta call on units through threading mechanism

  void	Compute_ExtraNetin(LeabraNetwork* net) { spec->Compute_ExtraNetin(this, net); }
  // #CAT_Activation compute extra netinput based on any kind of algorithmic computation -- goes to the layerspec and stops there -- not much overhead if not used
  void	Compute_NetinStats(LeabraNetwork* net)  { spec->Compute_NetinStats(this, net); }
  // #CAT_Activation compute AvgMax stats on netin and i_thr values computed during netin computation -- used for various regulatory and monitoring functions

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  void	Compute_Inhib(LeabraNetwork* net) 	{ spec->Compute_Inhib(this, net); }
  // #CAT_Activation compute the inhibition for layer
  void	Compute_LayInhibToGps(LeabraNetwork* net) { spec->Compute_LayInhibToGps(this, net); }
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups
  void	Compute_ApplyInhib(LeabraNetwork* net)	{ spec->Compute_ApplyInhib(this, net); }
  // #CAT_Activation Stage 2.3: apply computed inhib value to individual unit inhibitory conductances

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  void	Compute_CycleStats(LeabraNetwork* net)
  { return spec->Compute_CycleStats(this, net); }
  // #CAT_Statistic compute cycle-level stats -- acts AvgMax, MaxDa, OutputName, etc

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats -- optional non-default guys

  float	Compute_TopKAvgAct(LeabraNetwork* net)  { return spec->Compute_TopKAvgAct(this, net); }
  // #CAT_Statistic compute the average activation of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values
  float	Compute_TopKAvgNetin(LeabraNetwork* net)  { return spec->Compute_TopKAvgNetin(this, net); }
  // #CAT_Statistic compute the average netinput of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  void	Compute_MidMinus(LeabraNetwork* net)	{ spec->Compute_MidMinus(this, net); }
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycle parameter, if > 0 -- currently used for the PBWM algorithm

  ///////////////////////////////////////////////////////////////////////
  //	SettleFinal

  void	PostSettle_Pre(LeabraNetwork* net)	{ spec->PostSettle_Pre(this, net); }
  // #CAT_Activation perform computations in layers at end of settling -- this is a pre-stage that occurs prior to final PostSettle -- use this for anything that needs to happen prior to the standard PostSettle across layers (called by Settle_Final)
  // #CAT_Activation after settling, keep track of phase variables, etc.
  void	PostSettle(LeabraNetwork* net)	{ spec->PostSettle(this, net); }
  // #CAT_Activation after settling, keep track of phase variables, etc.

  void	Compute_ActM_AvgMax(LeabraNetwork* net) { spec->Compute_ActM_AvgMax(this, net); }
  // #CAT_Activation compute acts_m.avg from act_m
  void	Compute_ActP_AvgMax(LeabraNetwork* net) { spec->Compute_ActP_AvgMax(this, net); }
  // #CAT_Activation compute acts_p.avg from act_p

  void	TI_Compute_CtxtInhib(LeabraNetwork* net)
  { spec->TI_Compute_CtxtInhib(this, net); }
  // #CAT_Activation compute context inhibition

  ///////////////////////////////////////////////////////////////////////
  //	TrialFinal

  void	EncodeState(LeabraNetwork* net)		{ spec->EncodeState(this, net); }
  // #CAT_Learning encode final state information at end of trial for time-based learning across trials
  void	Compute_SelfReg_Trial(LeabraNetwork* net) { spec->Compute_SelfReg_Trial(this, net); }
  // #CAT_Activation update self-regulation (accommodation, hysteresis) at end of trial


  ///////////////////////////////////////////////////////////////////////
  //	Learning

  void	Compute_SRAvg_State(LeabraNetwork* net)
  { spec->Compute_SRAvg_State(this, net); }
  // #CAT_Learning compute state flag setting for sending-receiving activation averages (CtLeabra_X/CAL) -- called at the Cycle_Run level by network Compute_SRAvg_State -- unless manual_sravg flag is on, it just copies the network-level sravg_vals.state setting
  void	Compute_SRAvg_Layer(LeabraNetwork* net)
  { spec->Compute_SRAvg_Layer(this, net); }
  // #CAT_Learning compute layer-level sravg values -- called in advance of the unit level call -- updates the sravg_vals for this layer based on the sravg_vals.state flag
  bool	Compute_SRAvg_Test(LeabraNetwork* net)
  { return spec->Compute_SRAvg_Test(this, net); }
  // #CAT_Learning test whether to compute sravg values -- default is true, but some layers might opt out for various reasons

  void	Compute_dWt_Layer_pre(LeabraNetwork* net)  { spec->Compute_dWt_Layer_pre(this, net); }
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  bool	Compute_dWt_FirstMinus_Test(LeabraNetwork* net)
  { return spec->Compute_dWt_FirstMinus_Test(this, net); }
  // #CAT_Learning test whether to compute weight change after first minus phase has been encountered: for out-of-phase LeabraTI context layers (or anything similar)
  bool	Compute_dWt_FirstPlus_Test(LeabraNetwork* net)
  { return spec->Compute_dWt_FirstPlus_Test(this, net); }
  // #CAT_Learning test whether to compute weight change after first plus phase has been encountered: standard layers do a weight change here, except under CtLeabra_X/CAL
  bool	Compute_dWt_Nothing_Test(LeabraNetwork* net)
  { return spec->Compute_dWt_Nothing_Test(this, net); }
  // #CAT_Learning test whether to compute weight change after final nothing phase: standard layers do a weight change here under both learning rules

  ///////////////////////////////////////////////////////////////////////
  //	Trial-level Stats

  override float Compute_SSE(Network* net, int& n_vals,
			     bool unit_avg = false, bool sqrt = false)
  { return spec->Compute_SSE(this, (LeabraNetwork*)net, n_vals, unit_avg, sqrt); }

  float Compute_NormErr(LeabraNetwork* net)
  { return spec->Compute_NormErr(this, net); }
  // #CAT_Statistic compute normalized binary error across layer (returns normalized value or -1 for not applicable, averaged at network level -- see layerspec for more info)

  float Compute_M2SSE(LeabraNetwork* net, int& n_vals)
  { return spec->Compute_M2SSE(this, net, n_vals); }
  // #CAT_Statistic compute sum squared error of act_m2 activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value

  float Compute_CosErr(LeabraNetwork* net, int& n_vals)
  { return spec->Compute_CosErr(this, net, n_vals); }
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m over the layer -- n_vals is number of units contributing
  float Compute_M2CosErr(LeabraNetwork* net, int& n_vals)
  { return spec->Compute_M2CosErr(this, net, n_vals); }
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m2 instead of act_m -- n_vals is number of units contributing

  float Compute_CosDiff(LeabraNetwork* net)
  { return spec->Compute_CosDiff(this, net); }
  // #CAT_Statistic compute cosine (normalized dot product) of phase difference in this layer: act_p compared to act_m
  float Compute_CosDiff2(LeabraNetwork* net)
  { return spec->Compute_CosDiff2(this, net); }
  // #CAT_Statistic compute cosine (normalized dot product) of phase difference 2 in this layer: act_p compared to act_m2

  ////////////////////////////////////////////////////////////////////////////////
  //	Parameter Adaptation over longer timesales

  void	AdaptKWTAPt(LeabraNetwork* net) { spec->AdaptKWTAPt(this, net); }
  // #CAT_Activation adapt the kwta point based on average activity

  void	Compute_AbsRelNetin(LeabraNetwork* net)	{ spec->Compute_AbsRelNetin(this, net); }
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  void	Compute_AvgAbsRelNetin(LeabraNetwork* net) { spec->Compute_AvgAbsRelNetin(this, net); }
  // #CAT_Statistic compute the average absolute layer-level and relative netinput from different projections into this layer (over an epoch-level timescale)

  void	Compute_TrgRelNetin(LeabraNetwork* net) { spec->Compute_TrgRelNetin(this, net); }
  // #CAT_Statistic compute target rel netin based on projection direction information plus the adapt_rel_net values in the conspec
  void	Compute_AdaptRelNetin(LeabraNetwork* net) { spec->Compute_AdaptRelNetin(this, net); }
  // #CAT_Statistic adapt the relative input values by changing the conspec wt_scale.rel parameter; See Compute_AdaptAbsNetin for adaptation of wt_scale.abs parameters to achieve good netinput values overall
  void	Compute_AdaptAbsNetin(LeabraNetwork* net) { spec->Compute_AdaptAbsNetin(this, net); }
  // #CAT_Statistic adapt the absolute net input values by changing the conspec wt_scale.abs parameter

  ////////////////////////////////////////////
  //	Misc structural routines

  virtual void	ResetSortBuf();
  override void	TriggerContextUpdate();

  bool		SetLayerSpec(LayerSpec* sp);
  LayerSpec*	GetLayerSpec()		{ return (LayerSpec*)spec.SPtr(); }

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LeabraLayer& cp);
  TA_BASEFUNS(LeabraLayer);
protected:
  void	UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy()		{ CutLinks(); }
};

TA_SMART_PTRS(LeabraLayer);

#endif // LeabraLayer_h
