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
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies how inhibition is computed in Leabra system -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any unit-level inhibition is just added on top of this computed inhibition
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable this form of inhibition (layer or unit group) -- if only using inhibitory interneurons, both can be turned off
  float         gi;             // #CONDSHOW_ON_on #MIN_0 #AKA_lay_gi [1.5-2.3 typical, can go much lower or higher as needed] overall inhibition gain -- this is main paramter to adjust to change overall activation levels -- it scales both the the ff and fb factors uniformly
  float		ff;		// #CONDSHOW_ON_on #MIN_0 #DEF_1 overall inhibitory contribution from feedforward inhibition -- just multiplies average netinput (i.e., synaptic drive into layer) -- this anticipates upcoming changes in excitation, but if set too high, it can make activity slow to emerge
  float		fb;		// #CONDSHOW_ON_on #MIN_0 #DEF_0.5;1 overall inhibitory contribution from feedback inhibition -- just multiplies average activation -- this reacts to layer activation levels and works more like a thermostat (turnign up when the 'heat' in the layer is to high)
  float         fb_tau;         // #CONDSHOW_ON_on #MIN_0 #DEF_1.4 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for integrating feedback inhibitory values -- prevents oscillations that otherwise occur -- relatively rapid 1.4 typically works, but may need to go longer if oscillations are a problem
  float         ff0;            // #CONDSHOW_ON_on #DEF_0.1 feedforward zero point in terms of average netinput -- below this level, no FF inhibition is computed -- the 0.1 default should be good for most cases (and helps FF_FB produce k-winner-take-all dynamics), but if average netinputs are lower than typical, you may need to lower it

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
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LayerAvgActSpec);

class E_API LayerAvgActSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra expected average activity levels in the layer -- used to initialize running-average computation that is then used for netinput scaling, also specifies time constant for updating average
INHERITED(SpecMemberBase)
public:
  float		init;	    // #AKA_pct #MIN_0 [typically 0.1 - 0.2] initial estimated average activity level in the layer -- this is used as a starting point for running average actual activity level (acts_m_avg and acts_p_avg) -- acts_m_avg is used primarily for automatic netinput scaling, to balance out layers that have different activity levels -- thus it is important that init be relatively accurate -- good idea to update from recorded acts_m_avg levels (see LayerAvgAct button, here and on network) -- see also adjust parameter
  bool          fixed;      // #DEF_false if true, then the init value is used as a constant for acts_p_avg_eff (the effective value used for netinput rescaling), instead of using the actual running average activation
  float         tau;        // #CONDSHOW_OFF_fixed #DEF_100 #MIN_1 time constant in trials for integrating time-average values at the layer level -- used for computing acts_m_avg and acts_p_avg
  float         adjust;     // #CONDSHOW_OFF_fixed #DEF_1 adjustment multiplier on the computed acts_p_avg value that is used to compute acts_p_avg_eff, which is actually used for netinput rescaling -- if based on connectivity patterns or other factors the actual running-average value is resulting in netinputs that are too high or low, then this can be used to adjust the effective average activity value -- reducing the average activity with a factor < 1 will increase netinput scaling (stronger net inputs from layers that receive from this layer), and vice-versa for increasing (decreases net inputs)
  
  float		dt;		// #READ_ONLY #EXPERT rate = 1 / tau

  String        GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LayerAvgActSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
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
  float		self_fb;	// #MIN_0 #DEF_0.5;0.02;0;1 individual unit self feedback inhibition -- can produce proportional activation behavior in individual units for specialized cases (e.g., scalar val or BG units), but not so good for typical hidden layers
 float          self_tau;       // #CONDSHOW_OFF_self_fb:0 #MIN_0 #DEF_1.4 time constant in cycles, which should be milliseconds typically (roughly, how long it takes for value to change significantly -- 1.4x the half-life) for integrating unit self feedback inhibitory values -- prevents oscillations that otherwise occur -- relatively rapid 1.4 typically works, but may need to go longer if oscillations are a problem
  bool          Ei_dyn;         // does the inhibitory reversal potential (E_i) update dynamically over time in response to activation of the receiving neuron (backpropagating action potentials), or is it static -- dynamics are important when using adaptation, as this compensates for adaptation and allows active neurons to remain so
  float         Ei_gain;        // #CONDSHOW_ON_Ei_dyn #MIN_0 #DEF_0.001 multiplier on postsynaptic cell activation (act_eq), driving increases in E_i reversal potential for Cl- -- this factor determines how strong the e_rev change effect is
  float         Ei_tau;         // #CONDSHOW_ON_Ei_dyn #MIN_1 #DEF_50 decay time constant for decay of inhibitory reversal potential -- active neurons raise their inhibitory reversal potential, giving them an advantage over inactive neurons
  bool          fb_up_immed;    // should the feedback inhibition rise immediately to the driving value, and then decay with fb_tau time constant?  this is important for spiking activation function.  otherwise, all feedback component changes are goverened by fb_tau, which works better for rate-code case

  float		self_dt;        // #READ_ONLY #EXPERT rate = 1 / tau
  float		Ei_dt;          // #READ_ONLY #EXPERT rate = 1 / tau

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraInhibMisc);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
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
  float		event;		// #MIN_0 #MAX_1 [1 to clear] proportion decay of state vars between events
  float		cos_diff_avg_tau;  // #DEF_100 #MIN_1 time constant in trials (roughly how long significant change takes, 1.4 x half-life) for computing running average cos_diff value for the layer, cos_diff_avg = cosine difference between act_m and act_p -- this is an important statistic for how much phase-based difference there is between phases in this layer -- it is used in standard X_COS_DIFF modulation of l_mix in LeabraConSpec

  float         cos_diff_avg_dt; // #READ_ONLY #EXPERT rate constant = 1 / cos_diff_avg_taua

  void	        UpdtDiffAvg(float& diff_avg, const float cos_diff);
  // update the running average diff value

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LayerDecaySpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
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
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(LayGpInhibSpec);

class E_API LayGpInhibSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies how inhibition is communicated among layers within a layer group -- a MAX inhibition value is computed across the layer inhibition levels multiplied by gp_g (typically < 1, which discounts layer contributions to global layer-group level inhibition) -- the final layer inhibition value is then a MAX of the original inhibition and the layer-group level inhibition
INHERITED(SpecMemberBase)
public:
  bool		on;            // compute layer-group level inhibition, only applicable if the layer is within a layer group with other layers having this feature enabled
  float		gp_g;		// #CONDSHOW_ON_on&&!fffb #MIN_0 how much this layer's computed inhibition level contributes to the pooled layer-group-level inhibition MAX value -- the higher the value (closer to 1) the stronger the overall pooled inhibition effect within the group, with 1 being a maximal amount of pooled inhibition

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LayGpInhibSpec);
protected:
  SPEC_DEFAULTS;
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
  LeabraInhibSpec lay_inhib;	// #CAT_Activation #AKA_inhib how to compute layer-wide inhibition -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any inhibitory unit inhibition is just added on top of this computed inhibition -- set gi to 0 to turn off computed -- if unit groups are present and unit_gp inhib.gi > 0, net inhibition is MAX of layer and unit-group level
  LeabraInhibSpec unit_gp_inhib; // #CAT_Activation how to compute unit-group-level inhibition (only relevant if layer actually has unit groups -- set gi = 0 to exclude any group-level inhibition -- net inhibition is MAX of layer and unit group) -- uses feedforward (FF) and feedback (FB) inhibition (FFFB) based on average netinput (FF) and activation (FB) -- any inhibitory unit inhibition is just added on top of this computed inhibition -- set gi to 0 to turn off computed 
  LayerAvgActSpec avg_act;	// #CAT_Activation expected average activity levels in the layer -- used to initialize running-average computation that is then used for netinput scaling, also specifies time constant for updating average
  LeabraInhibMisc inhib_misc;	// #CAT_Activation extra parameters for special forms of inhibition beyond the basic FFFB dynamic specified in inhib
  LeabraClampSpec clamp;        // #CAT_Activation how to clamp external inputs to units (hard vs. soft)
  LayerDecaySpec  decay;        // #CAT_Activation decay of activity state vars between trials and phases, also time constants..
  LeabraDelInhib  del_inhib;	// #CAT_Activation delayed inhibition, as a function of per-unit net input on prior trial and/or phase -- produces temporal derivative effects
  LayGpInhibSpec  lay_gp_inhib;	// #CAT_Activation pooling of inhibition across layers within layer groups -- only applicable if the layer actually lives in a subgroup with other layers (and only in a first-level subgroup, not a sub-sub-group) -- each layer's computed inhib vals contribute with a factor of gp_g (0-1) to a pooled inhibition value, which is the MAX over all these individual scaled inhibition terms -- the final inhibition value for a given layer is then a MAX of the individual layer's original inhibition and this pooled value -- depending on the gp_g factor, this can cause more weak layers to drop out

  ///////////////////////////////////////////////////////////////////////
  //	Access, status functions

  inline bool   HasUnitGpInhib(Layer* lay)
  { return ((unit_gp_inhib.gi > 0.0f) && lay->unit_groups); }
  // does this layer have unit-group level inhibition?
  inline bool   HasLayerInhib(Layer* lay)
  { return (lay_inhib.gi > 0.0f); }
  // does this layer have layer level inhibition


  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  virtual void	Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning layer-level initialization taking place after Init_Weights on units
    virtual void Init_Inhib(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation called in Init_Weights_Layer initialize the inhibitory state values
    virtual void Init_Stats(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic called in Init_Weights_Layer intialize statistic variables

  virtual void	Init_Acts_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize unit-level dynamic state variables (activations, etc)

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
      (LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx, 
       LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of inhibition computation for either layer or unit group

    virtual void Compute_Inhib_FfFb
      (LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
       LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of feed-forward, feed-back inhibition computation

  virtual void	Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

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

  virtual float	Compute_NormErr_ugp(LeabraLayer* lay, 
				    Layer::AccessMode acc_md, int gpidx,
				    LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error for given unit group -- just gets the raw sum over unit group -- sum += per unit: if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0
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

  virtual void	Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  virtual void	Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute time-average relative netinput from different projections into this layer (e.g., every epoch)

  virtual void	ClearTICtxt(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TI clear the ti_ctxt context variables -- can be useful to do at discontinuities of experience

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

  void	InitLinks();
  SIMPLE_COPY(LeabraLayerSpec);
  TA_BASEFUNS(LeabraLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Defaults_init();
};

SPECPTR_OF(LeabraLayerSpec);

#endif // LeabraLayerSpec_h
