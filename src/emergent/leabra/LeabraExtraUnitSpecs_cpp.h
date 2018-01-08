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

#ifndef LeabraExtraUnitSpecs_cpp_h
#define LeabraExtraUnitSpecs_cpp_h 1

// parent includes:
#include <LeabraUnitSpec_cpp>

#include <State_cpp>

// member includes:
#include <LeabraExtraUnitSpecs_mbrs>

// declare all other types mentioned but not required to include:

class E_API LayerActUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // Layer-driven activation unit spec -- use this for any layer that computes activation values at the layer-level, instead of using the usual net input, currents etc -- saves on computational costs by negating most functions
INHERITED(LeabraUnitSpec)
public:

#include <LayerActUnitSpec>

  LayerActUnitSpec_cpp() { Initialize_core(); }
};


class E_API DeepCopyUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // #AKA_Deep5bUnitSpec units that continuously copy deep layer activation values from a one-to-one receiving projection -- should typically just be used for visualization convenience -- use SendDeepRawConSpec or SendDeepNormConSpec to send these variables directly from a given layer (although these only operate at restricted times)
INHERITED(LeabraUnitSpec)
public:

#include <DeepCopyUnitSpec>
  
  DeepCopyUnitSpec_cpp() { Initialize_core(); }
};


class E_API ThalSendUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // #AKA_ThalUnitSpec Models the dorsal and ventral (TRN) thalamus as it interacts with cortex (mainly secondary cortical areas, not primary sensory areas) -- simply sends current activation to thal variable in units we project to -- also if deep.on is active, we only recv deep_raw_net in the plus phase, like ThalAutoEncodeUnitSpec
INHERITED(LeabraUnitSpec)
public:

#include <ThalSendUnitSpec>
  
  ThalSendUnitSpec_cpp() { Initialize_core(); }
};


class E_API PoolInputsUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // a unit that continuously computes the pooled activation value over its inputs -- does not participate in normal netinput -- use MarkerConSpecs for inputs -- does not use weights, just activations
INHERITED(LeabraUnitSpec)
public:

#include <PoolInputsUnitSpec>

  PoolInputsUnitSpec_cpp() { Initialize_core(); }
};

   
////////////////////////////////////////////////////////////////////
//              TD

class E_API TDRewPredUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // predicts rewards: minus phase = clamped prior expected reward V^(t), plus = settles on expectation of future reward V^(t+1) -- cons should learn based on da_p * prev sending act (act_q0) -- should recv da_p from TDDeltaUnitSpec
INHERITED(LeabraUnitSpec)
public:

#include <TDRewPredUnitSpec>
  
  TDRewPredUnitSpec_cpp()  { Initialize_core(); }
};


class E_API TDRewIntegUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // integrates perceived and external rewards: delta over phases = DA td-like signal. minus phase = prev exp rew V^(t), plus phase = extrew (r) + tdrewpred computing V(t+1)
INHERITED(LeabraUnitSpec)
public:

#include <TDRewIntegUnitSpec>

  TDRewIntegUnitSpec_cpp() { Initialize_core(); }
};


class E_API TDDeltaUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // computes activation = temporal derivative (act_eq - act_m) of sending units in plus phases: note, act will go negative!
INHERITED(LeabraUnitSpec)
public:

#include <TDDeltaUnitSpec>

    TDDeltaUnitSpec_cpp() { Initialize_core(); }
};


////////////////////////////////////////////////////////////////////
//              PVLV -- Amyg

class E_API D1D2UnitSpec_cpp : public LeabraUnitSpec_cpp {
  // base class for basal ganglia and amygdala neurons that express dopamine D1 vs. D2 receptors and thus learn differentially from dopamine
INHERITED(LeabraUnitSpec)
public:

#include <D1D2UnitSpec>
  
  D1D2UnitSpec_cpp() { Initialize_core(); }
};


class E_API BasAmygUnitSpec_cpp : public D1D2UnitSpec_cpp {
  // #OBSOLETE OLD PVLV: Basal Amygdala units -- specifies the subtype for valence X acquisition vs.  extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:

#include <BasAmygUnitSpec>
  
  BasAmygUnitSpec_cpp() { Initialize_core(); }
};


class E_API BLAmygUnitSpec_cpp : public D1D2UnitSpec_cpp {
  // Basal Lateral Amygdala units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:

#include <BLAmygUnitSpec>

  BLAmygUnitSpec_cpp() { Initialize_core(); }
};

  

class E_API CElAmygUnitSpec_cpp : public D1D2UnitSpec_cpp {
  // Central Amygdala (lateral) units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:

#include <CElAmygUnitSpec>
  
  CElAmygUnitSpec_cpp() { Initialize_core(); }
};


////////////////////////////////////////////////////////////////////
//              PVLV -- DA

class E_API PPTgUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // represents the PPTg -- drives burst firing of dopamine neurons as a function of the positive-only temporal derivative of its net input from one trial to the next -- misc_1 holds the prior trial net input
INHERITED(LeabraUnitSpec)
public:

#include <PPTgUnitSpec>

  PPTgUnitSpec_cpp() { Initialize_core(); }
};


class E_API LHbRMTgUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // combined lateral habenula and RMTg units -- receives from Patch and Matrix Direct and Indirect pathways, along with primary value (PV) positive and negative valence drivers, and computes dopamine dip signals, represented as positive activations in these units, which are then translated into dips through a projection to the VTAUnitSpec
INHERITED(LeabraUnitSpec)
public:

#include <LHbRMTgUnitSpec>

  LHbRMTgUnitSpec_cpp() { Initialize_core(); }
};


class E_API VTAUnitSpec_cpp : public LeabraUnitSpec_cpp {
// Models DA cells of the Ventral Tegmental Area: if DA_P models classic Schultzian behavior and computes PVLV dopamine (Da) signal from PPTg and LHbRMTg input projections, and also a direct input from a positive valence PV layer, and shunting inhibition from VS Patch Indirect -- uses deep_qtr to determine when to send dopamine values; if DA_N models newly discovered small subpopulation that bursts for negative valence and projects to ONLY dedicated fear-related circuits
INHERITED(LeabraUnitSpec)
public:

#include <VTAUnitSpec>
  
  VTAUnitSpec_cpp() { Initialize_core(); }
};

class E_API DRNUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // Models the Dorsal Raphe Nucleus which drives serotonin (se, 5HT) as a long-term running average of bad primary value outcomes and bodily state variables (optionally as a the balance between good and bad), and also integrates dopamine values over time if given
INHERITED(LeabraUnitSpec)
public:

#include <DRNUnitSpec>

  DRNUnitSpec_cpp() { Initialize_core(); }
};


class E_API ClampDaUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // a dopamine unit that you can just clamp to any value and it will send it to other unit's da_p or da_v values
INHERITED(LeabraUnitSpec)
public:

#include <ClampDaUnitSpec>

  ClampDaUnitSpec_cpp() { Initialize_core(); }
};


class E_API BFCSUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // Models basal forebrain cholinergic system (BFCS) according to a Pearce-Hall style saliency dynamic, where lack of predictability in reward predictions drives increased ACh attentional modulation output -- updates and sends ACh at start of trial, based on trial-level delta values (act - act_q0) over its inputs
INHERITED(LeabraUnitSpec)
public:

#include <BFCSUnitSpec>

  BFCSUnitSpec_cpp() { Initialize_core(); }
};


////////////////////////////////////////////////////////////////////
//              PVLV -- BG / VS

class E_API MSNUnitSpec_cpp : public D1D2UnitSpec_cpp {
  // Medium Spiny Neuron, which is principal cell of the striatum -- determines the dopamine receptor type and patch / matrix specialization of the neuron, and overall anatomical location (dorsal / ventral), which are then used by the MSNConSpec and other areas in determining learning and other dynamics
INHERITED(D1D2UnitSpec)
public:

#include <MSNUnitSpec>

  MSNUnitSpec_cpp() { Initialize_core(); }
};


class E_API TANUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // Models the Tonically Active Neurons of the Striatum, which are driven in part by the parafasicular pathway conveying projections from the CNA, PPTG, and OFC, and exhibit a burst-pause firing dynamic at the time of actual rewards and expected rewards, which has a net disinhibitory effect on MSN's -- we hypothesize that this drives consolidation of MSN synaptic traces and updates the sign with concurrent dopamine firing, and then clears the trace -- only sends ACh during deep_raw_qtr
INHERITED(LeabraUnitSpec)
public:

#include <TANUnitSpec>

  TANUnitSpec_cpp() { Initialize_core(); }
};


class E_API PatchUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // Dorsal striatum patch neurons, which shunt dopamine values in all neurons that they project to (shortcut for shunting SNc dopamine which then projects to other target striatum neurons) -- typically driven by PFC maintenance inputs, blocking learning for anything with ongoing maintenance -- threshold for shunting is opt_thresh.send threshold -- sends to shunt var on recv units
INHERITED(LeabraUnitSpec)
public:

#include <PatchUnitSpec>

  PatchUnitSpec_cpp() { Initialize_core(); }
};


class E_API GPiInvUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // #AKA_GPiUnitSpec Inverted GPi globus pallidus internal segment, analogous with SNr -- major output pathway of the basal ganglia.  This integrates Go and NoGo inputs, computing netin = Go - go_nogo.nogo * NoGo -- unlike real GPi units are typically off, and the most active wins through inhibitory competition -- is responsible for determining when gating happens, sends act to thal field on layers that it sends to -- can be used directly as a sole SNrThal gating layer, or indrectly with InvertUnitSpec to mimic actual non-inverted GPi in a projection to thalamus layer
INHERITED(LeabraUnitSpec)
public:

#include <GPiInvUnitSpec>
  
  GPiInvUnitSpec_cpp() { Initialize_core(); }
};


class E_API InvertUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // a unit that continuously copies activations from input (use one to one prjns, weights don't matter) and sets our activations to 1.0 - input->act
INHERITED(LeabraUnitSpec)
public:
  
#include <InvertUnitSpec>

  InvertUnitSpec_cpp() { Initialize_core(); }
};


////////////////////////////////////////////////////////////////////
//              PBWM

class E_API PFCUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // PFC unit spec -- thal_gate and thal signals received typically during Q1,Q3 e.g., from GPiInvUnitSpec drives updating of deep_raw during deep_raw_qtr quaters (typically Q2,Q4) as gated deep5b activations, sent as deep_ctxt to DEEP units that send back deep_lrn to SUPER, and drive top-down outputs -- output gating is transient
INHERITED(LeabraUnitSpec)
public:

#include <PFCUnitSpec>

  PFCUnitSpec_cpp() { Initialize_core(); }
};


////////////////////////////////////////////////////////////////////
//              Hippo

class E_API ECoutUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // unit spec for EC out layers that implements ThetaPhase learning -- automatically clamps to ECin activations in plus phase, based on MarkerConSpec one-to-one prjn from ECin, -- must use HippoEncoderConSpec for connections to learn based on encoder phase of theta cycle -- records encoder error as misc_1 unit variable
INHERITED(LeabraUnitSpec)
public:
  
#include <ECoutUnitSpec>
  
  ECoutUnitSpec_cpp() { Initialize_core(); }
};


class E_API CA1UnitSpec_cpp : public LeabraUnitSpec_cpp {
  // unit spec for CA1 layers that implements ThetaPhase learning -- modulates ECin and CA1 weight scale strengths (wt_scale.abs = 0 or 1) in conspecs -- must use unique conspecs for these projections -- ECin = 1, CA3 = 0 for 1st quarter, then ECin = 0, CA3 = 1 until q4, where it goes back to ECin = 1, CA3 = 0 for plus phase
INHERITED(LeabraUnitSpec)
public:

#include <CA1UnitSpec>

  CA1UnitSpec_cpp() { Initialize_core(); }
};


////////////////////////////////////////////////////////////////////
//              Cerebellum

class E_API CerebGranuleUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // cerebellum granule unit -- enforces phasic temporal activity and provides lagged learning signals
INHERITED(LeabraUnitSpec)
public:

#include <CerebGranuleUnitSpec>

  CerebGranuleUnitSpec_cpp() { Initialize_core(); }
};


////////////////////////////////////////////////////////////////////
//              Misc

class E_API LeabraMultCopyUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // unit that copies activations from one layer and multiplies them by values from another -- i.e., multiplicative gating -- must recv from 2 prjns (any more ignored) -- first is copy activation, second is multiplication activation
INHERITED(LeabraUnitSpec)
public:

#include <LeabraMultCopyUnitSpec>
  
  LeabraMultCopyUnitSpec_cpp() { Initialize_core(); }
};


class E_API LeabraContextUnitSpec_cpp : public LeabraUnitSpec_cpp {
  // context units that copy from their recv projection, which should have one-to-one connections with a source layer -- this is one way to achieve simple recurrent network (SRN) behavior, that has more flexibility and uses a separate context layer for visualization purposes -- also consider using DeepCtxtConSpec connections within a single layer, which implements this same form of computation more efficiently, but less transparently and with fewer options
INHERITED(LeabraUnitSpec)
public:

#include <LeabraContextUnitSpec>
  
  LeabraContextUnitSpec_cpp() { Initialize_core(); }
};

#endif // LeabraExtraUnitSpecs_cpp_h
