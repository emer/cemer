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

class E_API LeabraInhibSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies how inhibition is computed in Leabra system -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average (or maximum) netinput (FF) and activation (FB) -- any unit-level inhibition is just added on top of this computed inhibition
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable this form of inhibition (layer or unit group) -- if only using inhibitory interneurons, both can be turned off
  float         gi;             // #CONDSHOW_ON_on #MIN_0 #AKA_lay_gi #DEF_1.8 [1.5-2.3 typical, can go lower or higher as needed] overall inhibition gain -- this is main paramter to adjust to change overall activation levels -- it scales both the the ff and fb factors uniformly -- also see inhib_adapt which will adapt an additional multiplier on this overall inhibition to keep layer near target activation value specified in avg_act.targ_init
  float		ff;		// #CONDSHOW_ON_on #MIN_0 #DEF_1 overall inhibitory contribution from feedforward inhibition -- multiplies average netinput (i.e., synaptic drive into layer) -- this anticipates upcoming changes in excitation, but if set too high, it can make activity slow to emerge -- see also ff0 for a zero-point for this value
  float		fb;		// #CONDSHOW_ON_on #MIN_0 #DEF_1 overall inhibitory contribution from feedback inhibition -- multiplies average activation -- this reacts to layer activation levels and works more like a thermostat (turning up when the 'heat' in the layer is too high)
  float         fb_tau;         // #CONDSHOW_ON_on #MIN_0 #DEF_1.4 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for integrating feedback inhibitory values -- prevents oscillations that otherwise occur -- relatively rapid 1.4 typically works, but may need to go longer if oscillations are a problem
  float         ff0;            // #CONDSHOW_ON_on #DEF_0.1 feedforward zero point for average netinput -- below this level, no FF inhibition is computed based on avg netinput, and this value is subtraced from the ff inhib contribution above this value -- the 0.1 default should be good for most cases (and helps FF_FB produce k-winner-take-all dynamics), but if average netinputs are lower than typical, you may need to lower it

  float		fb_dt;		// #READ_ONLY #EXPERT rate = 1 / tau

  inline float    FFInhib(const float avg_netin) {
    float ffi = 0.0f;
    if(avg_netin > ff0) ffi = ff * (avg_netin - ff0);
    return ffi;
  }
  // feedforward inhibition value as function of netinput

  inline float    FBInhib(const float avg_act) {
    float fbi = fb * avg_act;
    return fbi;
  }
  // feedback inhibition value as function of netinput

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraInhibSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraMultiGpSpec);

class E_API LeabraMultiGpSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies how to combine multiple unit groups for multi-group inhibition
INHERITED(SpecMemberBase)
public:
  taVector2i    size;         // total number of unit groups to combine together into a common pool of multi unit-group inhibition
  taVector2i    st_off;       // starting offset -- defines the lower left corner of the square of unit groups of size 'size' that is included within a given unit group's multi-group inhibition window -- for example if size is 3x3 and st_off is -1, -1 (and sub_size = 1) then you will include the immediate neighbors of each unit group on all sides
  taVector2i    sub_size;     // how many unit groups share the same pooled multi-group inhibition -- all the unit groups within this subgroup size share the same inhibitory pool -- i.e., the pool only moves after every gp_subgp unit groups
  bool          wrap;         // wrap around on the edges or not?

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraMultiGpSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LayerAvgActSpec);

class E_API LayerAvgActSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra expected average activity levels in the layer -- used for computing running-average computation that is then used for netinput scaling (also specifies time constant for updating average), and for the target value for adapting inhibition in inhib_adapt
INHERITED(SpecMemberBase)
public:
  float		targ_init;	    // #AKA_init #MIN_0 [typically 0.1 - 0.2] target value for adapting inhibition (see inhib_adapt params) and initial estimated average activity level in the layer -- used as a starting point for running average actual activity level (acts_m_avg and acts_p_avg) -- acts_p_avg is used primarily for automatic netinput scaling, to balance out layers that have different activity levels -- thus it is important that init be relatively accurate -- good idea to update from recorded acts_p_avg levels (see LayerAvgAct button, here and on network) -- see also adjust parameter
  bool          fixed;      // #DEF_false if true, then the init value is used as a constant for acts_p_avg_eff (the effective value used for netinput rescaling), instead of using the actual running average activation
  float         tau;        // #CONDSHOW_OFF_fixed #DEF_100 #MIN_1 time constant in trials for integrating time-average values at the layer level -- used for computing acts_m_avg and acts_p_avg
  float         adjust;     // #CONDSHOW_OFF_fixed #DEF_1 adjustment multiplier on the computed acts_p_avg value that is used to compute acts_p_avg_eff, which is actually used for netinput rescaling -- if based on connectivity patterns or other factors the actual running-average value is resulting in netinputs that are too high or low, then this can be used to adjust the effective average activity value -- reducing the average activity with a factor < 1 will increase netinput scaling (stronger net inputs from layers that receive from this layer), and vice-versa for increasing (decreases net inputs)
  
  float		dt;		// #READ_ONLY #EXPERT rate = 1 / tau

  String        GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LayerAvgActSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraAdaptInhib);

class E_API LeabraAdaptInhib : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra adapt the overal inhibitory gain value (adapt_gi on layer) to keep overall layer activation within a given target range as specified by avg_act.targ_init
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable adaptive inhibition function to adapt overall layer inhibition gain as stored in layer adapt_gi value
  float         tol_pct;        // #CONDSHOW_ON_on #DEF_0.25 tolerance around target average activation of avg_act.targ_init as a proportion of that target value -- only once activations move outside this tolerance are inhibitory values adapted
  int           trial_interval; // #CONDSHOW_ON_on interval in trials between updates of the adaptive inhibition values -- only check and update this often -- typically the same order as the number of trials per epoch used in training the model
  float		tau;		// #CONDSHOW_ON_on #DEF_200;500 time constant for rate of updating the inhibitory gain value, in terms of trial_interval periods (e.g., 100 = adapt gain over 100 trial intervals) -- adaptation rate is 1/tau * (acts_m_avg - trg_avg_act) / trg_avg_act

  float		dt;		// #READ_ONLY #EXPERT rate = 1 / tau


  inline bool   AdaptInhib(float& gi, const float trg_avg_act, const float acts_m_avg) {
    float delta_pct = (acts_m_avg - trg_avg_act) / trg_avg_act;
    if(fabsf(delta_pct) >= tol_pct) {
      gi += dt * delta_pct;
      return true;
    }
    return false;
  }

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraAdaptInhib);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraInhibMisc);

class E_API LeabraInhibMisc : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra extra specifications for how inhibition is computed in Leabra system -- these apply across layer and unit group levels
INHERITED(SpecMemberBase)
public:
  float		net_thr;	// #DEF_0;0.2 threshold on net input for inclusion in the computation of the average netinput, which in turn drives feedforward inhibition -- this is important for preventing units that don't have any significant input from diluting the overall level of inhibition -- prior to version 7.8.7 this was effectively 0 -- set to 0.2 if inhibition is being inappropriately diluted by these off units (not all such cases benefit from this filtering -- experiment) -- see also thr_rel and thr_act options for whether this is a relative vs. absolute threshold, and whether it applies to activations or not
  bool          thr_rel;        // #CONDSHOW_OFF_net_thr:0 #DEF_true if true, net_thr is relative to the maximum net input within the layer (from the previous cycle) -- this is useful because net input values evolve over time and a fixed threshold prevents any ff inhibition early in settling when net inputs are low
  float		self_fb;	// #MIN_0 #DEF_0.5;0.02;0;1 individual unit self feedback inhibition -- can produce proportional activation behavior in individual units for specialized cases (e.g., scalar val or BG units), but not so good for typical hidden layers
 float          self_tau;       // #CONDSHOW_OFF_self_fb:0 #MIN_0 #DEF_1.4 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for integrating unit self feedback inhibitory values -- prevents oscillations that otherwise occur -- relatively rapid 1.4 typically works, but may need to go longer if oscillations are a problem
  bool          fb_up_immed;    // should the feedback inhibition rise immediately to the driving value, and then decay with fb_tau time constant?  this is important for spiking activation function.  otherwise, all feedback component changes are goverened by fb_tau, which works better for rate-code case

  float		self_dt;        // #READ_ONLY #EXPERT rate = 1 / tau

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraInhibMisc);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraClampSpec);

class E_API LeabraClampSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for clamping external inputs on INPUT or TARGET layers
INHERITED(SpecMemberBase)
public:
  bool		hard;		// #DEF_true whether to hard clamp inputs where activation is directly set to external input value (act = ext, computed once at start of quarter) or do soft clamping where ext is added into net input (net += gain * ext)
  float		gain;		// #CONDSHOW_OFF_hard #DEF_0.02:0.5 soft clamp gain factor (net += gain * ext)

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraClampSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LayerDecaySpec);

class E_API LayerDecaySpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds decay values and other layer-level time constants
INHERITED(SpecMemberBase)
public:
  float		trial;		// #AKA_event #MIN_0 #MAX_1 [1 to clear] proportion decay of state vars between trials -- if all layers have 0 trial decay, then the net input does not need to be reset between trials, yielding significantly faster performance

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LayerDecaySpec);
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraDelInhib);

class E_API LeabraDelInhib : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra delayed inhibition, as a function of per-unit net input on prior trial and/or phase -- produces temporal derivative effects
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable delayed inhibition 
  float		prv_trl;	// #CONDSHOW_ON_on proportion of per-unit net input on previous trial to add in as inhibition 
  float		prv_q;	        // #CONDSHOW_ON_on proportion of per-unit net input on previous gamma-frequency quarter to add in as inhibition 

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraDelInhib);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LeabraLayStats);

class E_API LeabraLayStats : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra leabra layer-level statistics parameters
INHERITED(SpecMemberBase)
public:
  float		cos_diff_avg_tau;  // #DEF_100 #MIN_1 time constant in trials (roughly how long significant change takes, 1.4 x half-life) for computing running average cos_diff value for the layer, cos_diff_avg = cosine difference between act_m and act_p -- this is an important statistic for how much phase-based difference there is between phases in this layer -- it is used in standard X_COS_DIFF modulation of l_mix in LeabraConSpec
  float         hog_thr;           // #MIN_0 #MAX_1 #DEF_0.3;0.2 threshold on unit avg_act (long time-averaged activation), above which the unit is considered to be a 'hog' that is dominating the representational space
  float         dead_thr;         // #MIN_0 #MAX_1 #DEF_0.01;0.005 threshold on unit avg_act (long time-averaged activation), above which the unit is considered to be a 'hog' that is dominating the representational space

  float         cos_diff_avg_dt; // #READ_ONLY #EXPERT rate constant = 1 / cos_diff_avg_taua

  inline void	UpdtDiffAvg(float& diff_avg, const float cos_diff) {
    if(diff_avg == 0.0f) {        // first time -- set
      diff_avg = cos_diff;
    }
    else {
      diff_avg += cos_diff_avg_dt * (cos_diff - diff_avg);
    }
  }
  // update the running average diff value

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraLayStats);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(LeabraLayerSpec);

class E_API LeabraLayerSpec : public LayerSpec {
  // #STEM_BASE ##CAT_Leabra Leabra layer specs, computes inhibitory input for all units in layer
INHERITED(LayerSpec)
public:
  LeabraInhibSpec lay_inhib;	// #CAT_Activation #AKA_inhib how to compute layer-wide inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- net inhibition is MAX of all operative inhibition -- any inhibitory unit inhibition is just added on top of this computed inhibition
  LeabraInhibSpec unit_gp_inhib; // #CAT_Activation how to compute unit-group-level inhibition (only relevant if layer actually has unit groups -- net inhibition is MAX of all operative inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any inhibitory unit inhibition is just added on top of this computed inhibition
  LeabraInhibSpec multi_gp_inhib; // #CAT_Activation how to compute inhibition that combines across multiple unit-groups (only relevant if layer actually has unit groups -- net inhibition is MAX of all operative inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any inhibitory unit inhibition is just added on top of this computed inhibition
  LeabraMultiGpSpec multi_gp_geom; // #CAT_Activation #CONDSHOW_ON_multi_gp_inhib.on how to combine multiple unit groups for computing multi-group level inhibition
  LeabraInhibSpec lay_gp_inhib;	// #CAT_Activation inhibition computed across layers within layer groups -- only applicable if the layer actually lives in a subgroup with other layers (and only in a first-level subgroup, not a sub-sub-group) -- only the specs of the FIRST layer in the layer group are used for computing inhib -- net inhibition is MAX of all operative inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any inhibitory unit inhibition is just added on top of this computed inhibition
  LayerAvgActSpec avg_act;	// #CAT_Activation expected average activity levels in the layer -- used for computing running-average computation that is then used for netinput scaling (also specifies time constant for updating average), and for the target value for adapting inhibition in inhib_adapt
  LeabraAdaptInhib inhib_adapt; // #CAT_Activation adapt an extra inhibitory gain value to keep overall layer activation within a given target range, based on avg_act.targ_init target value (TARGET or deep TRC layers use the running-average plus phase average actitvation) -- gain applies to all forms of inhibition (layer, unit group) that are in effect
  LeabraInhibMisc inhib_misc;	// #CAT_Activation extra parameters for special forms of inhibition beyond the basic FFFB dynamic specified in inhib
  LeabraClampSpec clamp;        // #CAT_Activation how to clamp external inputs to units (hard vs. soft)
  LayerDecaySpec  decay;        // #CAT_Activation decay of activity state vars between trials
  LeabraDelInhib  del_inhib;	// #CAT_Activation delayed inhibition, as a function of per-unit net input on prior trial and/or phase -- produces temporal derivative effects
  LeabraLayStats  lstats;       // #CAT_Statistic layer-level statistics parameters
  float           lay_lrate;    // #CAT_Statistic layer-level learning rate modulator, multiplies learning rates for all connections coming into layer(s) that this spec applies to -- sets lrate_mod value on layer

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

    virtual void Compute_MultiGpInhib(LeabraLayer* lay, LeabraNetwork* net, int thr_no);
    // #IGNORE multi-group inhib

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

  virtual float	Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error of unit targ vs. act_m -- layer-level value is already normalized, and network just averages across the layers (each layer contributes equally to overal normalized value, instead of contributing in proportion to number of units) -- returns -1 if not an err target defined in same way as sse -- per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is based on k value per layer: total possible err for both on and off errs is 2 * k (on or off alone is just k)

  virtual float  Compute_CosErr(LeabraLayer* lay, LeabraNetwork* net, int& n_vals);
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m over the layer -- n_vals is number of units contributing

  virtual float  Compute_CosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute cosine (normalized dot product) of phase activation difference in this layer: act_p compared to act_m -- must be called after Quarter_Final for plus phase to get the act_p values
  virtual float  Compute_AvgActDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average act_diff (act_p - act_m) for this layer -- must be called after Quarter_Final for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  virtual float  Compute_TrialCosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute cosine (normalized dot product) of trial activation difference in this layer: act_q4 compared to act_q0 -- must be called after Quarter_Final for plus phase to get the act_q4 values
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
