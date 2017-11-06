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

#ifndef LeabraExtraConSpecs_h
#define LeabraExtraConSpecs_h 1

// this file includes all the non-base LeabraConSpec subtypes for the main
// compile space

// parent includes:
#include <LeabraConSpec>
#include <LeabraUnGpState_cpp>

#include <State_main>

// member includes:
eTypeDef_Of(ChlSpecs);
eTypeDef_Of(LatAmygGains);
eTypeDef_Of(BasAmygLearnSpec);
eTypeDef_Of(BLAmygLearnSpec);
eTypeDef_Of(CElAmygLearnSpec);
eTypeDef_Of(MSNTraceSpec);
eTypeDef_Of(MSNTraceThalLrates);

#include <LeabraExtraConSpecs_mbrs>

#include <State_main>

// declare all other types mentioned but not required to include:


eTypeDef_Of(DeepCtxtConSpec);

class E_API DeepCtxtConSpec : public LeabraConSpec {
  // #AKA_LeabraTICtxtConSpec sends deep layer deep_raw activation values to deep_ctxt_net variable on receiving units -- typically used to integrate across the local context within a layer, providing both temporal integration (TI) learning, and the basis for normalizing attentional signals -- use for SELF projection in a layer -- wt_scale should be set to 1, 1
INHERITED(LeabraConSpec)
public:

#include <DeepCtxtConSpec>
  
  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_SIMPLE_BASEFUNS(DeepCtxtConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void Initialize();
  void Defaults_init();
  void Destroy()     { };
};


eTypeDef_Of(SendDeepRawConSpec);

class E_API SendDeepRawConSpec : public LeabraConSpec {
  // #AKA_Deep5bConSpec sends deep_raw activation values instead of usual act values -- stored into deep_raw_net var on recv unit -- used e.g., in projections to thalamus
INHERITED(LeabraConSpec)
public:

#include <SendDeepRawConSpec>
  
  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_SIMPLE_BASEFUNS(SendDeepRawConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void Initialize();
  void Defaults_init();
  void Destroy()     { };
};


eTypeDef_Of(SendDeepModConSpec);

class E_API SendDeepModConSpec : public LeabraConSpec {
  // sends regular activation values to deep_net variable on recv units, which is then used to drive deep_mod activation state directly -- used for predictive auto-encoder units -- no defined learning rule at this point -- use fixed one-to-one cons usually
INHERITED(LeabraConSpec)
public:

#include <SendDeepModConSpec>

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_SIMPLE_BASEFUNS(SendDeepModConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void Initialize();
  void Defaults_init();
  void Destroy()     { };
};


eTypeDef_Of(MarkerConSpec);

class E_API MarkerConSpec : public LeabraConSpec {
  // connection spec that marks special projections: doesn't send netin or adapt weights
INHERITED(LeabraConSpec)
public:

#include <MarkerConSpec>

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_BASEFUNS_NOCOPY(MarkerConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(LeabraLimPrecConSpec);

class E_API LeabraLimPrecConSpec : public LeabraConSpec {
  // ##CAT_Leabra Leabra limited precision connection specs: limits weight values to specified level of precision between 0-1
INHERITED(LeabraConSpec)
public:

#include <LeabraLimPrecConSpec>

  TA_SIMPLE_BASEFUNS(LeabraLimPrecConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init()		{ };
  void	Destroy()		{ };
};


eTypeDef_Of(LeabraDeltaConSpec);

class E_API LeabraDeltaConSpec : public LeabraConSpec {
  // basic delta-rule learning (plus - minus) * sender, with sender in the minus phase -- soft bounding as specified in spec -- no hebbian or anything else
INHERITED(LeabraConSpec)
public:

#include <LeabraDeltaConSpec>

  TA_SIMPLE_BASEFUNS(LeabraDeltaConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(DaDeltaConSpec);

class E_API DaDeltaConSpec : public LeabraConSpec {
  // basic delta-rule learning (plus - minus) * sender, with sender in the minus phase -- soft bounding as specified in spec -- no hebbian or anything else
INHERITED(LeabraConSpec)
public:

#include <DaDeltaConSpec>

  TA_SIMPLE_BASEFUNS(DaDeltaConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(DaHebbConSpec);

class E_API DaHebbConSpec : public LeabraConSpec {
  // basic dopamine-modulated hebbian learning -- dwt = da * ru_act * su_act
INHERITED(LeabraConSpec)
public:

#include <DaHebbConSpec>

  TA_SIMPLE_BASEFUNS(DaHebbConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(CHLConSpec);

class E_API CHLConSpec : public LeabraConSpec {
  // #AKA_XCalCHLConSpec does CHL-based Leabra learning under XCAL and CAL learning framework -- sometimes CHL performs better, e.g., in the hippocampus..
INHERITED(LeabraConSpec)
public:

#include <CHLConSpec>

  TA_SIMPLE_BASEFUNS(CHLConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init()		{ };
  void	Destroy()		{ };
};


///////////////////////////////////////////////////////////////////
//              Special Algos: TD, PVLV, PBWM, Cereb

eTypeDef_Of(TDRewPredConSpec);

class E_API TDRewPredConSpec : public LeabraConSpec {
  // Reward Prediction connections: for TD RewPred Unit, uses TD algorithm for predicting rewards -- learns on da_p (TD) * sending trace activation from prev timestep (act_q0)
INHERITED(LeabraConSpec)
public:

#include <TDRewPredConSpec>
  
  TA_BASEFUNS_NOCOPY(TDRewPredConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(LatAmygConSpec);

class E_API LatAmygConSpec : public LeabraConSpec {
  // #OBSOLETE OLD PVLV: simulates learning in the lateral amygdala, based on CS-specific input weights, with learning modulated by phasic dopamine from either da_p (positive-valence) or da_n (negative valence), but predominantly the positive values of these signals. To prevent CS self-training positive feedback, the CS must generally have been active in the prior trial, using act_q0.  there is no dependence on postsynaptic activation
INHERITED(LeabraConSpec)
public:

#include <LatAmygConSpec>
  
  TA_SIMPLE_BASEFUNS(LatAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(BasAmygConSpec);

class E_API BasAmygConSpec : public LeabraConSpec {
  // #OBSOLETE OLD PVLV: simulates learning in the basal amygdala, with separate equations for acquisition vs. extinction subpoplations -- acquisition recv from LatAmyg, learn from da_p and postsynaptic activity -- extinction recv from context / pfc, and learn from ACQ up-state signal and da_p using D2 receptors
INHERITED(LeabraConSpec)
public:

#include <BasAmygConSpec>

  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(BasAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(BLAmygConSpec);

class E_API BLAmygConSpec : public LeabraConSpec {
  // simulates learning in the Basal Lateral Amygdala using a simple delta-rule between this trial and previous trial, modulated also by absolute value of phasic dopamine -- delta influence comes from unit spec dopamine da_mod and US drive
INHERITED(LeabraConSpec)
public:
  #include <BLAmygConSpec>

  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(BLAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(CElAmygConSpec);

class E_API CElAmygConSpec : public LeabraConSpec {
  // simulates learning in the central amygdala (lateral) using a simple delta-rule between this trial and previous trial, modulated also by absolute value of phasic dopamine -- delta influence comes from unit spec dopamine da_mod and US drive
INHERITED(LeabraConSpec)
public:

#include <CElAmygConSpec>

  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(CElAmygConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};


eTypeDef_Of(MSNConSpec);

class E_API MSNConSpec : public LeabraConSpec {
  // Learning of striatal medium spiny neurons input (afferent) connections -- must have an MSNUnitSpec on recv neuron -- based on dopamine, sender * receiver activation product, and (optionally) thal gating activation signal -- supports a trace mechanism which accumulates an ongoing synaptic trace over time, which is then multiplied by a later dopamine da_p value that is typically driven by primary value (US) outcome at end of a sequence of actions -- dwt = da_p * tr; tr = [thal] * su * ru - otr_lrate * su * ru, representing a contrast between gated activations that go one way, and other non-gated activations that go the opposite way (which supports engagement of alternative gating strategies, and avoids overall reductions in weights) -- the trace is reset when this weight change is computed, as a result of an over-threshold level of dopamine.  Patch units shunt dopamine from actively maintaining stripes / information processing channels, to prevent this clearing.
INHERITED(LeabraConSpec)
public:

#include <MSNConSpec>
  
  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;
  
  TA_SIMPLE_BASEFUNS(MSNConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(HippoEncoderConSpec);

class E_API HippoEncoderConSpec : public LeabraConSpec {
  // for EC <-> CA1 connections: CHL learning on encoder variables (act_p vs. act_q1)
INHERITED(LeabraConSpec)
public:

#include <HippoEncoderConSpec>

  TA_BASEFUNS_NOCOPY(HippoEncoderConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(CerebPfPcConSpec);

class E_API CerebPfPcConSpec : public LeabraConSpec {
  // The parallel-fiber to Purkinje cell connection spec -- special learning rule driven by IO error values clamped onto the Purkinje cell -- if targ value is 0, then no error (weights slowly increase) else an error and LTD occurs
INHERITED(LeabraConSpec)
public:

#include <CerebPfPcConSpec>

  TA_SIMPLE_BASEFUNS(CerebPfPcConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void	Defaults_init();
  void  Destroy()     { };
};

#endif // LeabraExtraConSpecs_h
