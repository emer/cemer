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

#ifndef LeabraLayerSpec_h
#define LeabraLayerSpec_h 1

// parent includes:
#include <LayerSpec>
#include <SpecMemberBase>

// member includes:
#include <Layer>
#include <AvgMaxVals>

// declare all other types mentioned but not required to include:
class LeabraLayer; // 
class LeabraNetwork; // 
class LeabraInhib; // 
class KwtaSortBuff; // 
class LeabraUnit; //
eTypeDef_Of(LeabraUnGpData);

eTypeDef_Of(LeabraInhibSpec);
eTypeDef_Of(LayerAvgActSpec);
eTypeDef_Of(LeabraAdaptInhib);
eTypeDef_Of(LeabraActMargin);
eTypeDef_Of(LeabraInhibMisc);
eTypeDef_Of(LeabraClampSpec);
eTypeDef_Of(LayerDecaySpec);
eTypeDef_Of(LeabraDelInhib);
eTypeDef_Of(LeabraCosDiffMod);
eTypeDef_Of(LeabraLayStats);
eTypeDef_Of(LeabraLayerSpec);
eTypeDef_Of(LeabraLayerSpec_core);

// key defines for LeabraUnitSpec_core

#define LAYERSPEC_MEMBER_SUFFIX 
#define LAYERSPEC_MEMBER_BASE SpecMemberBase

#define LAYERSPEC_CLASS_SUFFIX _core
#define LAYERSPEC_BASE LayerSpec

#include <LeabraLayerSpec_core>

class E_API LeabraLayerSpec : public LeabraLayerSpec_core {
  // #STEM_BASE ##CAT_Leabra Leabra layer specs, computes inhibitory input for all units in layer
INHERITED(LeabraLayerSpec_core)
public:

  ///////////////////////////////////////////////////////////////////////
  //	Access, status functions

  inline bool   HasUnitGpInhib(Layer* lay)
  { return (unit_gp_inhib.on && lay->unit_groups); }
  // does this layer have unit-group level inhibition?
  inline bool   HasLayerInhib(Layer* lay)
  { return (lay_inhib.on); }
  // does this layer have layer level inhibition


  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  virtual void	Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning layer-level initialization taking place after Init_Weights on units
    virtual void Init_AdaptInhib(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation called in Init_Weights_Layer initialize the adaptive inhibitory state values
    virtual void Init_Stats(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic called in Init_Weights_Layer intialize statistic variables

  virtual void	Init_Acts_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize unit-level dynamic state variables (activations, etc)

  virtual void	DecayState(LeabraLayer* lay, LeabraNetwork* net, float decay);
  // #CAT_Activation decay the state of this layer -- not normally called but available for programs etc to control specific layers

  ///////////////////////////////////////////////////////////////////////
  //	Trial_Init -- at start of trial

  virtual void	Trial_Init_Specs(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags
  virtual void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning layer level trial init -- overload where needed

  ///////////////////////////////////////////////////////////////////////
  //	Quarter_Init -- at start of settling

  virtual void	Quarter_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize start of a setting phase: all layer-level misc init takes place here (calls TargFlags_Layer) -- other stuff all done directly in Quarter_Init_Units call

  virtual void Quarter_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #IGNORE layer-level initialize start of a setting phase, set input flags appropriately, etc
  virtual void Quarter_Init_AvgAct_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #IGNORE layer-level init avg_act based on fixed, use_ext_act
  virtual float Compute_AvgExt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute average of unit ext or targ values, depending on ext flags
  virtual void	Compute_HardClamp_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: hard-clamp inputs

  virtual void	ExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  // main computation is direct Send_NetinDelta call on units through threading mechanism
  // followed by Compute_NetinInteg on units


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  virtual void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net, int thr_no);
  // #CAT_Activation compute the inhibition for layer -- this is the main call point into this stage of processing
    virtual void Compute_Inhib_impl
      (LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of inhibition computation for either layer or unit group

    virtual void Compute_Inhib_FfFb
      (LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of feed-forward, feed-back inhibition computation

  virtual void	Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual void	Compute_CycleStats_Pre(LeabraLayer* lay, LeabraNetwork* net) { };
  // #CAT_Statistic pre-cycle-stats -- done in single thread prior to cycle stats -- good place to intervene for whole-layer dynamics

  virtual void Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)
  virtual void Compute_OutputName_ugp(LeabraLayer* lay, 
                                      Layer::AccessMode acc_md, int gpidx,
                                      LeabraInhib* thr, LeabraNetwork* net);
  // #IGNORE compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)

  ///////////////////////////////////////////////////////////////////////
  //	Quarter_Final

  virtual void	Quarter_Final_Pre(LeabraLayer* lay, LeabraNetwork* net) { };
  // #CAT_Activation perform computations in layers at end of settling -- this is a pre-stage that occurs prior to final Quarter_Final -- use this for anything that needs to happen prior to the standard Quarter_Final across layers (called by network Quarter_Final)
  virtual void	Quarter_Final_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation after settling, keep track of phase variables, etc.
    virtual void Quarter_Final_GetMinus(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get minus phase act stats
    virtual void Quarter_Final_GetPlus(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get plus phase act stats

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  virtual void	Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) { };
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  ///////////////////////////////////////////////////////////////////////
  //	Trial-level Stats

  virtual void  LayerAvgAct(DataTable* report_table = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewReportData create a data table with the current layer average activations (acts_m_avg, acts_p_avg, acts_p_avg_eff) and the values specified in the layerspec avg_act.init -- this is useful for setting the .init values accurately based on actual levels 

  virtual float	Compute_SSE(LeabraLayer* lay, LeabraNetwork* net,
			    int& n_vals, bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value

  virtual float	Compute_MaxErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute max_err, across unit groups (if present and used) and the entire layer

  virtual float	Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error of unit targ vs. act_m -- layer-level value is already normalized, and network just averages across the layers (each layer contributes equally to overal normalized value, instead of contributing in proportion to number of units) -- returns -1 if not an err target defined in same way as sse -- per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is based on k value per layer: total possible err for both on and off errs is 2 * k (on or off alone is just k)

  virtual float  Compute_CosErr(LeabraLayer* lay, LeabraNetwork* net, int& n_vals);
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m over the layer -- n_vals is number of units contributing

  virtual float  Compute_CosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute cosine (normalized dot product) of phase activation difference in this layer: act_p compared to act_m -- must be called after Quarter_Final for plus phase to get the act_p values
  virtual void   Compute_CosDiff_post(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic post step of cos_diff -- needed for sharing cos_diff based lrate mod
  virtual float  Compute_AvgActDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average act_diff (act_p - act_m) for this layer -- must be called after Quarter_Final for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  virtual float  Compute_TrialCosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute cosine (normalized dot product) of trial activation difference in this layer: act_q4 compared to act_q0 -- must be called after Quarter_Final for plus phase to get the act_q4 values
  virtual void   Compute_ActMargin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute activation margin stats and adapt thresholds
  virtual float   Compute_NetSd(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute standard deviation of the minus phase net inputs across the layer -- this is a key statistic to monitor over time for how much the units are gaining traction on the problem -- they should be getting more differentiated and sd should go up -- if not, then the network will likely fail -- must be called at end of minus phase
  virtual void   Compute_HogDeadPcts(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute percentage of units in the layer that have a long-time-averaged activitation level that is above or below hog / dead thresholds, indicating that they are either 'hogging' the representational space, or 'dead' and not participating in any representations

  virtual void	Compute_AvgNormErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average norm_err (at an epoch-level timescale)
  virtual void	Compute_AvgCosErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average cos_err (at an epoch-level timescale)
  virtual void	Compute_AvgCosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average cos_diff (at an epoch-level timescale)
  virtual void	Compute_AvgTrialCosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average trial_cos_diff (at an epoch-level timescale)
  virtual void	Compute_AvgAvgActDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average avg_act_diff (at an epoch-level timescale)
  virtual void	Compute_AvgNetSd(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average net_sd (at an epoch-level timescale)
  virtual void	Compute_EpochStats(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute epoch-level statistics (averages)

  virtual void	Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  virtual void	Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute time-average relative netinput from different projections into this layer (e.g., every epoch)

  virtual void	ClearDeepActs(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Deep clear the deep layer variables -- can be useful to do at discontinuities of experience

  ////////////////////////////////////////////
  //	Misc structural routines

  virtual LeabraLayer* FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec
  virtual LeabraLayer* FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec: uses exact type match, not inherits!
  static  LeabraLayer* FindLayerFmSpecNet(Network* net, TypeDef* layer_spec);
  // #CAT_Structure find a layer in network based on the type of layer spec

  virtual void	HelpConfig();	// #BUTTON #CAT_Structure get help message for configuring this spec
  bool CheckConfig_Layer(Layer* lay, bool quiet=false) override;
  // check for for misc configuration settings required by different algorithms, including settings on the processes NOTE: this routine augments the default layer checks, it doesn't replace them

  virtual TypeDef* 	UnGpDataType()  { return &TA_LeabraUnGpData; }
  // #CAT_Structure type of unit group data object to create for the layers associated with this layer spec

  String        GetToolbarName() const override { return "layer spec"; }

  void	InitLinks() override;
  SIMPLE_COPY(LeabraLayerSpec);
  TA_BASEFUNS(LeabraLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Defaults_init();
};

SPECPTR_OF(LeabraLayerSpec);

#endif // LeabraLayerSpec_h
