// Copyright 2017-2018, Regents of the University of Colorado,
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

#ifndef LeabraExtraUnitSpecs_h
#define LeabraExtraUnitSpecs_h 1

// parent includes:
#include <LeabraUnitSpec>
#include <DataTable>

#include <State_main>

// member includes:

// thse are all the types defined in _mbrs:
eTypeDef_Of(TDRewIntegSpec);
eTypeDef_Of(BLAmygDaMod);
eTypeDef_Of(BLAmygAChMod);
eTypeDef_Of(CElAmygDaMod);
eTypeDef_Of(LHbRMTgSpecs);
eTypeDef_Of(LHbRMTgGains);
eTypeDef_Of(PVLVDaSpec);
eTypeDef_Of(PVLVDaGains);
eTypeDef_Of(LVBlockSpec);
eTypeDef_Of(DRN5htSpec);
eTypeDef_Of(BFCSAChSpec);
eTypeDef_Of(MatrixActSpec);
eTypeDef_Of(TANActSpec);
eTypeDef_Of(GPiGateSpec);
eTypeDef_Of(GPiMiscSpec);
eTypeDef_Of(PFCGateSpec);
eTypeDef_Of(PFCMaintSpec);
eTypeDef_Of(PFCDynEl);
eTypeDef_Of(ThetaPhaseSpecs);
eTypeDef_Of(CerebGranuleSpecs);
eTypeDef_Of(CtxtUpdateSpec);
eTypeDef_Of(CtxtNSpec);

#include <LeabraExtraUnitSpecs_mbrs>

#include <State_main>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LayerActUnitSpec);

class E_API LayerActUnitSpec : public LeabraUnitSpec {
  // Layer-driven activation unit spec -- use this for any layer that computes activation values at the layer-level, instead of using the usual net input, currents etc -- saves on computational costs by negating most functions
INHERITED(LeabraUnitSpec)
public:

#include <LayerActUnitSpec>

  TA_SIMPLE_BASEFUNS(LayerActUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize() 		{ };
  void	Defaults_init()		{ };
  void	Destroy()		{ };
};


eTypeDef_Of(DeepCopyUnitSpec);

class E_API DeepCopyUnitSpec : public LeabraUnitSpec {
  // #AKA_Deep5bUnitSpec units that continuously copy deep layer activation values from a one-to-one receiving projection -- should typically just be used for visualization convenience -- use SendDeepRawConSpec or SendDeepNormConSpec to send these variables directly from a given layer (although these only operate at restricted times)
INHERITED(LeabraUnitSpec)
public:

#include <DeepCopyUnitSpec>
  
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(DeepCopyUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(ThalSendUnitSpec);

class E_API ThalSendUnitSpec  : public LeabraUnitSpec {
  // #AKA_ThalUnitSpec Models the dorsal and ventral (TRN) thalamus as it interacts with cortex (mainly secondary cortical areas, not primary sensory areas) -- simply sends current activation to thal variable in units we project to -- also if deep.on is active, we only recv deep_raw_net in the plus phase, like ThalAutoEncodeUnitSpec
INHERITED(LeabraUnitSpec)
public:

#include <ThalSendUnitSpec>
  
  TA_SIMPLE_BASEFUNS(ThalSendUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(PoolInputsUnitSpec);

class E_API PoolInputsUnitSpec : public LeabraUnitSpec {
  // a unit that continuously computes the pooled activation value over its inputs -- does not participate in normal netinput -- use MarkerConSpecs for inputs -- does not use weights, just activations
INHERITED(LeabraUnitSpec)
public:

#include <PoolInputsUnitSpec>

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(PoolInputsUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};

   
////////////////////////////////////////////////////////////////////
//              TD

eTypeDef_Of(TDRewPredUnitSpec);

class E_API TDRewPredUnitSpec : public LeabraUnitSpec {
  // predicts rewards: minus phase = clamped prior expected reward V^(t), plus = settles on expectation of future reward V^(t+1) -- cons should learn based on da_p * prev sending act (act_q0) -- should recv da_p from TDDeltaUnitSpec
INHERITED(LeabraUnitSpec)
public:

#include <TDRewPredUnitSpec>
  
  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_BASEFUNS_NOCOPY(TDRewPredUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(TDRewIntegUnitSpec);

class E_API TDRewIntegUnitSpec : public LeabraUnitSpec {
  // integrates perceived and external rewards: delta over phases = DA td-like signal. minus phase = prev exp rew V^(t), plus phase = extrew (r) + tdrewpred computing V(t+1)
INHERITED(LeabraUnitSpec)
public:

#include <TDRewIntegUnitSpec>

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false)  override;

  TA_SIMPLE_BASEFUNS(TDRewIntegUnitSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(TDDeltaUnitSpec);

class E_API TDDeltaUnitSpec : public LeabraUnitSpec {
  // computes activation = temporal derivative (act_eq - act_m) of sending units in plus phases: note, act will go negative!
INHERITED(LeabraUnitSpec)
public:

#include <TDDeltaUnitSpec>

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(TDDeltaUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


////////////////////////////////////////////////////////////////////
//              PVLV -- Amyg


eTypeDef_Of(D1D2UnitSpec);

class E_API D1D2UnitSpec : public LeabraUnitSpec {
  // base class for basal ganglia and amygdala neurons that express dopamine D1 vs. D2 receptors and thus learn differentially from dopamine
INHERITED(LeabraUnitSpec)
public:

#include <D1D2UnitSpec>
  
  TA_SIMPLE_BASEFUNS(D1D2UnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(BasAmygUnitSpec);

class E_API BasAmygUnitSpec : public D1D2UnitSpec {
  // #OBSOLETE OLD PVLV: Basal Amygdala units -- specifies the subtype for valence X acquisition vs.  extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:

#include <BasAmygUnitSpec>
  
  TA_SIMPLE_BASEFUNS(BasAmygUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(BLAmygUnitSpec);

class E_API BLAmygUnitSpec : public D1D2UnitSpec {
  // Basal Lateral Amygdala units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:

#include <BLAmygUnitSpec>

  TA_SIMPLE_BASEFUNS(BLAmygUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

  

eTypeDef_Of(CElAmygUnitSpec);

class E_API CElAmygUnitSpec : public D1D2UnitSpec {
  // Central Amygdala (lateral) units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:

#include <CElAmygUnitSpec>
  
  TA_SIMPLE_BASEFUNS(CElAmygUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};


////////////////////////////////////////////////////////////////////
//              PVLV -- DA

eTypeDef_Of(PPTgUnitSpec);

class E_API PPTgUnitSpec : public LeabraUnitSpec {
  // represents the PPTg -- drives burst firing of dopamine neurons as a function of the positive-only temporal derivative of its net input from one trial to the next -- misc_1 holds the prior trial net input
INHERITED(LeabraUnitSpec)
public:

#include <PPTgUnitSpec>

  TA_SIMPLE_BASEFUNS(PPTgUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(LHbRMTgUnitSpec);

class E_API LHbRMTgUnitSpec : public LeabraUnitSpec {
  // combined lateral habenula and RMTg units -- receives from Patch and Matrix Direct and Indirect pathways, along with primary value (PV) positive and negative valence drivers, and computes dopamine dip signals, represented as positive activations in these units, which are then translated into dips through a projection to the VTAUnitSpec
INHERITED(LeabraUnitSpec)
public:

#include <LHbRMTgUnitSpec>

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(LHbRMTgUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(VTAUnitSpec);

class E_API VTAUnitSpec : public LeabraUnitSpec {
// Models DA cells of the Ventral Tegmental Area: if DA_P models classic Schultzian behavior and computes PVLV dopamine (Da) signal from PPTg and LHbRMTg input projections, and also a direct input from a positive valence PV layer, and shunting inhibition from VS Patch Indirect -- uses deep_qtr to determine when to send dopamine values; if DA_N models newly discovered small subpopulation that bursts for negative valence and projects to ONLY dedicated fear-related circuits
INHERITED(LeabraUnitSpec)
public:

#include <VTAUnitSpec>
  
  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(VTAUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init() { Initialize(); }
  void  Destroy()     { };
};


eTypeDef_Of(DRNUnitSpec);

class E_API DRNUnitSpec : public LeabraUnitSpec {
  // Models the Dorsal Raphe Nucleus which drives serotonin (se, 5HT) as a long-term running average of bad primary value outcomes and bodily state variables (optionally as a the balance between good and bad), and also integrates dopamine values over time if given
INHERITED(LeabraUnitSpec)
public:

#include <DRNUnitSpec>

  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(DRNUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init() { Initialize(); }
  void  Destroy()     { };
};


eTypeDef_Of(ClampDaUnitSpec);

class E_API ClampDaUnitSpec : public LeabraUnitSpec {
  // a dopamine unit that you can just clamp to any value and it will send it to other unit's da_p or da_v values
INHERITED(LeabraUnitSpec)
public:

#include <ClampDaUnitSpec>

  TA_SIMPLE_BASEFUNS(ClampDaUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init() { };
  void  Destroy()     { };
};


eTypeDef_Of(BFCSUnitSpec);

class E_API BFCSUnitSpec : public LeabraUnitSpec {
  // Models basal forebrain cholinergic system (BFCS) according to a Pearce-Hall style saliency dynamic, where lack of predictability in reward predictions drives increased ACh attentional modulation output -- updates and sends ACh at start of trial, based on trial-level delta values (act - act_q0) over its inputs
INHERITED(LeabraUnitSpec)
public:

#include <BFCSUnitSpec>

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(BFCSUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init() { Initialize(); }
  void  Destroy()     { };
};


////////////////////////////////////////////////////////////////////
//              PVLV -- BG / VS

eTypeDef_Of(MSNUnitSpec);

class E_API MSNUnitSpec : public D1D2UnitSpec {
  // Medium Spiny Neuron, which is principal cell of the striatum -- determines the dopamine receptor type and patch / matrix specialization of the neuron, and overall anatomical location (dorsal / ventral), which are then used by the MSNConSpec and other areas in determining learning and other dynamics
INHERITED(D1D2UnitSpec)
public:

#include <MSNUnitSpec>

  TA_SIMPLE_BASEFUNS(MSNUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(TANUnitSpec);

class E_API TANUnitSpec : public LeabraUnitSpec {
  // Models the Tonically Active Neurons of the Striatum, which are driven in part by the parafasicular pathway conveying projections from the CNA, PPTG, and OFC, and exhibit a burst-pause firing dynamic at the time of actual rewards and expected rewards, which has a net disinhibitory effect on MSN's -- we hypothesize that this drives consolidation of MSN synaptic traces and updates the sign with concurrent dopamine firing, and then clears the trace -- only sends ACh during deep_raw_qtr
INHERITED(LeabraUnitSpec)
public:

#include <TANUnitSpec>

  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(TANUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(PatchUnitSpec);

class E_API PatchUnitSpec  : public LeabraUnitSpec {
  // Dorsal striatum patch neurons, which shunt dopamine values in all neurons that they project to (shortcut for shunting SNc dopamine which then projects to other target striatum neurons) -- typically driven by PFC maintenance inputs, blocking learning for anything with ongoing maintenance -- threshold for shunting is opt_thresh.send threshold -- sends to shunt var on recv units
INHERITED(LeabraUnitSpec)
public:

#include <PatchUnitSpec>

  TA_SIMPLE_BASEFUNS(PatchUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(GPiInvUnitSpec);

class E_API GPiInvUnitSpec : public LeabraUnitSpec {
  // #AKA_GPiUnitSpec Inverted GPi globus pallidus internal segment, analogous with SNr -- major output pathway of the basal ganglia.  This integrates Go and NoGo inputs, computing netin = Go - go_nogo.nogo * NoGo -- unlike real GPi units are typically off, and the most active wins through inhibitory competition -- is responsible for determining when gating happens, sends act to thal field on layers that it sends to -- can be used directly as a sole SNrThal gating layer, or indrectly with InvertUnitSpec to mimic actual non-inverted GPi in a projection to thalamus layer
INHERITED(LeabraUnitSpec)
public:

#include <GPiInvUnitSpec>
  
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(GPiInvUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(InvertUnitSpec);

class E_API InvertUnitSpec : public LeabraUnitSpec {
  // a unit that continuously copies activations from input (use one to one prjns, weights don't matter) and sets our activations to 1.0 - input->act
INHERITED(LeabraUnitSpec)
public:
  
#include <InvertUnitSpec>

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(InvertUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


////////////////////////////////////////////////////////////////////
//              PBWM

eTypeDef_Of(PFCUnitSpec);

class E_API PFCUnitSpec : public LeabraUnitSpec {
  // PFC unit spec -- thal_gate and thal signals received typically during Q1,Q3 e.g., from GPiInvUnitSpec drives updating of deep_raw during deep_raw_qtr quaters (typically Q2,Q4) as gated deep5b activations, sent as deep_ctxt to DEEP units that send back deep_lrn to SUPER, and drive top-down outputs -- output gating is transient
INHERITED(LeabraUnitSpec)
public:

#include <PFCUnitSpec>

  enum DynVals {                // the different values stored in dyn_table -- for rapid access
    DYN_NAME,
    DYN_DESC,
    DYN_INIT,                   // initial value at point when gating starts
    DYN_RISE_TAU,               // time constant for linear rise in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first
    DYN_DECAY_TAU,              // time constant for linear decay in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first
  };

  DataTable     dyn_table;      // #TREE_SHOW #EXPERT table of dynamics parameters for response of deep_raw over time after gating has taken place -- update occurs once each quarter that deep_ctxt is computed -- one set of params per each row, n_dyns rows total (see n_dyns)

  inline float  GetDynVal(DynVals val, int row) {
    return dyn_table.GetValAsFloat(val, row);
  }
  // get specific dyn value for given row
  inline void   SetDynVal(const Variant& vl, DynVals val, int row) {
    dyn_table.SetVal(vl, val, row);
  }
  // set specific dyn value for given row

  virtual void  FormatDynTable();
  // #IGNORE format the dyn table
  virtual void  DefaultDynTable(float std_tau = 10);
  // #BUTTON default initial dynamics table -- specifies flat maint for output gating and all combinations of up / down dynamics for maint gating -- the std_tau parameter defines the number of update steps over which the dynamics occur (e.g., rises in std_tau time steps)
  virtual void  UpdtDynTable();
  // check and make sure table is all OK

  virtual void	GraphPFCDyns(DataTable* graph_data, int n_trials=20);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the pfc dynamics for response of deep_raw over time after gating has taken place -- update occurs once each quarter that deep_raw is computed (typically once per trial)

  virtual void  CopyToState_Dyns(void* state_spec, const char* state_suffix);
  // #IGNORE copy our compiled dyns data to other state spec

  void    CopyToState(void* state_spec, const char* state_suffix) override;
  void    UpdateStateSpecs() override;

  
  TA_SIMPLE_BASEFUNS(PFCUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Defaults_init();
  void	Destroy()		{ CutLinks(); FreeDyns(); }
};



////////////////////////////////////////////////////////////////////
//              Hippo

eTypeDef_Of(ECoutUnitSpec);

class E_API ECoutUnitSpec : public LeabraUnitSpec {
  // unit spec for EC out layers that implements ThetaPhase learning -- automatically clamps to ECin activations in plus phase, based on MarkerConSpec one-to-one prjn from ECin, -- must use HippoEncoderConSpec for connections to learn based on encoder phase of theta cycle -- records encoder error as misc_1 unit variable
INHERITED(LeabraUnitSpec)
public:
  
#include <ECoutUnitSpec>
  
  bool CheckConfig_Unit(Layer* lay, bool quiet=false) override; 

  TA_SIMPLE_BASEFUNS(ECoutUnitSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(CA1UnitSpec);

class E_API CA1UnitSpec : public LeabraUnitSpec {
  // unit spec for CA1 layers that implements ThetaPhase learning -- modulates ECin and CA1 weight scale strengths (wt_scale.abs = 0 or 1) in conspecs -- must use unique conspecs for these projections -- ECin = 1, CA3 = 0 for 1st quarter, then ECin = 0, CA3 = 1 until q4, where it goes back to ECin = 1, CA3 = 0 for plus phase
INHERITED(LeabraUnitSpec)
public:

#include <CA1UnitSpec>

  bool CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(CA1UnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init()		{ };
  void	Destroy()		{ };
};


////////////////////////////////////////////////////////////////////
//              Cerebellum

eTypeDef_Of(CerebGranuleUnitSpec);

class E_API CerebGranuleUnitSpec : public LeabraUnitSpec {
  // cerebellum granule unit -- enforces phasic temporal activity and provides lagged learning signals
INHERITED(LeabraUnitSpec)
public:

#include <CerebGranuleUnitSpec>

  bool CheckConfig_Unit(Layer* lay, bool quiet) override;

  TA_SIMPLE_BASEFUNS(CerebGranuleUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void Initialize();
  void Defaults_init() { };
  void Destroy()     { };
};


////////////////////////////////////////////////////////////////////
//              Misc

eTypeDef_Of(LeabraMultCopyUnitSpec);

class E_API LeabraMultCopyUnitSpec : public LeabraUnitSpec {
  // unit that copies activations from one layer and multiplies them by values from another -- i.e., multiplicative gating -- must recv from 2 prjns (any more ignored) -- first is copy activation, second is multiplication activation
INHERITED(LeabraUnitSpec)
public:

#include <LeabraMultCopyUnitSpec>
  
  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(LeabraMultCopyUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Defaults_init() { Initialize(); }
  void  Destroy()     { };
};


eTypeDef_Of(LeabraContextUnitSpec);

class E_API LeabraContextUnitSpec : public LeabraUnitSpec {
  // context units that copy from their recv projection, which should have one-to-one connections with a source layer -- this is one way to achieve simple recurrent network (SRN) behavior, that has more flexibility and uses a separate context layer for visualization purposes -- also consider using DeepCtxtConSpec connections within a single layer, which implements this same form of computation more efficiently, but less transparently and with fewer options
INHERITED(LeabraUnitSpec)
public:

#include <LeabraContextUnitSpec>
  
  virtual void TriggerUpdate(LeabraLayer* lay, bool update);
  // #CAT_Context manually set update trigger status -- must be both set and un-set manually -- flag remains set until explicitly called with update=false -- tested on first cycle of processing within a trial -- can always be called even if not on MANUAL -- sets layer flag LAY_FLAG_1 that triggers update

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(LeabraContextUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};

#endif // LeabraExtraUnitSpecs_h
