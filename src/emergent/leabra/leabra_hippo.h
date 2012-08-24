// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_extra.h"

#ifndef leabra_hippo_h
#define leabra_hippo_h

///////////////////////////////////////
// Special Hippocampal Layerspecs

// timing of quadphase dynamics -- split minus and normal plus:
// [ ------ minus ------- ][ ---- plus ---- ]
// [   auto-  ][ recall-  ][ -- both plus-- ]

//  DG -> CA3 -> CA1
//  /    /      /    \
// [----EC_in---] -> [ EC_out ]

// minus phase: EC_out unclamped, driven by CA1
// auto-   CA3 -> CA1 = 0, EC_in -> CA1 = 1
// recall- CA3 -> CA1 = 1, EC_in -> CA1 = 0

// plus phase: EC_in -> EC_out auto clamped
// CA3 -> CA1 = 0, EC_in -> CA1 = 1
// (same as auto- -- training signal for CA3 -> CA1 is what EC would produce!

// act_m2 = auto encoder minus phase state (in both CA1 and EC_out
//   used by HippoEncoderConSpec relative to act_p plus phase)
// act_m = recall minus phase (normal minus phase dynamics for CA3 recall learning)
// act_p = plus (serves as plus phase for both auto and recall)

// learning just happens at end of trial as usual, but encoder projections use
// the act_m2, act_p variables to learn on the right signals

class LEABRA_API HippoQuadLayerSpec : public LeabraLayerSpec {
  // base layer spec for hippocampal layers that implements quad phase learning
INHERITED(LeabraLayerSpec)
public:
  int		auto_m_cycles;	// #DEF_20:80 number of cycles for auto-encoder minus phase, at which point act_m2 is recorded for training the EC <-> CA1 auto-encoder -- this should be just long enough for information to reach EC_in and flow through CA1 to EC_out -- will set network min_cycles to be this number plus 20 cycles, which is a minimum for combined assoc and recall minus phases

  virtual void 	RecordActM2(LeabraLayer* lay, LeabraNetwork* net);
  // save current act_nd values as act_m2 -- minus phase for auto-encoder learning
  virtual void 	Compute_AutoEncStats(LeabraLayer* lay, LeabraNetwork* net);
  // compute act_dif2 as act_eq - act_m2, and based on that compute error stats as user data on layer (enc_sse, enc_norm_err)

  TA_SIMPLE_BASEFUNS(HippoQuadLayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

class LEABRA_API ECinLayerSpec : public HippoQuadLayerSpec {
  // layer spec for EC input layers that implements quad phase learning -- this serves only as a marker for ECout layers to search for -- no new functionality over LeabraLayerSpec
INHERITED(HippoQuadLayerSpec)
public:
  // following is main hook into code:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  TA_SIMPLE_BASEFUNS(ECinLayerSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};

class LEABRA_API CA3LayerSpec : public HippoQuadLayerSpec {
  // layer spec for CA3 layers that implements quad phase learning -- this serves only as a marker for CA1 layers to search for -- no new functionality over LeabraLayerSpec
INHERITED(HippoQuadLayerSpec)
public:
  TA_SIMPLE_BASEFUNS(CA3LayerSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};

class LEABRA_API ECoutLayerSpec : public HippoQuadLayerSpec {
  // layer spec for EC out layers that implements quad phase learning -- automatically clamps to EC in activations in plus phase and records act_m2 mid minus -- must use HippoEncoderConSpec for connections to learn from first half of minus phase (act_m2)
INHERITED(HippoQuadLayerSpec)
public:
  // following is main hook into code:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override bool CheckConfig_Layer(Layer* lay, bool quiet=false);
  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) { };
  // no mid minus -- could overwrite!

  virtual void 	ClampFromECin(LeabraLayer* lay, LeabraNetwork* net);
  // clamp ECout values from ECin values

  TA_SIMPLE_BASEFUNS(ECoutLayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

class LEABRA_API CA1LayerSpec : public HippoQuadLayerSpec {
  // layer spec for CA1 layers that implements quad phase learning -- modulates EC_in and CA1 weight scale strengths, and records act_m2 mid minus for auto encoder
INHERITED(HippoQuadLayerSpec)
public:
  float		recall_decay; 		// #DEF_1 proportion to decay layer activations at start of recall phase
  bool		use_test_mode;		// #DEF_true if network train_mode == TEST, then keep EC_in -> CA1 on, and don't do recall_decay -- makes it more likely to at least get input parts right

  // following are main hook into code:
  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) { };
  // no mid minus -- could overwrite!

  override bool CheckConfig_Layer(Layer* lay, bool quiet=false);
  virtual void 	ModulateCA3Prjn(LeabraLayer* lay, LeabraNetwork* net, bool ca3_on);
  // control the strength of the CA3 -> CA1 projection according to ca3_on arg
  virtual void 	ModulateECinPrjn(LeabraLayer* lay, LeabraNetwork* net, bool ecin_on);
  // control the strength of the EC_in -> CA1 projection according to ecin_on arg
  virtual void 	FinalizePrjnMods(LeabraLayer* lay, LeabraNetwork* net);
  // broadcast modifications to projection strengths to make them actually take effect (Network::Compute_NetinScale_Senders and Network::DecayState(0))

  TA_SIMPLE_BASEFUNS(CA1LayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

class LEABRA_API SubiculumNoveltySpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra how to compute novelty from normalized error, and then modulate learning rate as a function of novelty
INHERITED(SpecMemberBase)
public:
  float		max_norm_err;	// #MAX_1 #MIN_0 maximum effective norm err value for computing novelty -- novelty is linear between 0 and this max value, renormalized to 0-1 range
  float		min_lrate;	// #MIN_0 lowest possible learning rate multiplier -- for fully familiar item -- note this is a multiplier on lrate that the spec otherwise has

  float		nov_rescale;	// #READ_ONLY #NO_SAVE 1/max_norm_err -- rescale novelty after clipping to max
  float		lrate_factor;	// #READ_ONLY #NO_SAVE (1.0 - min_lrate) -- to convert 0-1 novelty into learning rate


  inline float	ComputeNovelty(float norm_err) {
    float eff_nov = nov_rescale * MIN(norm_err, max_norm_err);
    return eff_nov;
  }
  inline float	ComputeLrate(float novelty) {
    float lrate = min_lrate + novelty * lrate_factor;
    return lrate;
  }

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(SubiculumNoveltySpec);
protected:
  SPEC_DEFAULTS;
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

class LEABRA_API SubiculumLayerSpec : public ScalarValLayerSpec {
  // layer spec for subiculum layer that computes an online novely signal based on the mismatch between EC_in and EC_out, and optionally modulates learning rate in conspec (for perforant path etc) as a function of novelty -- recv prjns must be sequential matched pairs of ECin and ECout layers
INHERITED(ScalarValLayerSpec)
public:
  SubiculumNoveltySpec	novelty;
  // parameters for computing novelty from norm err over ECout compared to ECin targets, and adapting learning rate in lrate_mod_con_spec from this novelty value
  ConSpec_SPtr		lrate_mod_con_spec;
  // LeabraConSpec to modulate the learning rate of based on novelty value -- actually sets the learning rate schedule multiplier value, so diff specs can have diff lrates (but cannot have actual schedules -- this is in effect an automatic schedule)

  virtual float Compute_ECNormErr_ugp(LeabraLayer* lin, LeabraLayer* lout,
				     Layer::AccessMode acc_md, int gpidx,
				     LeabraNetwork* net);
  // impl routine for computing EC norm error across in and out layers

  virtual void 	Compute_ECNovelty(LeabraLayer* lay, LeabraNetwork* net);
  // compute novelty based on EC_in vs. out discrepancy -- sets USER_DATA values on layer to reflect norm_err, novelty value, and lrate, and activation in layer is always clamped scalar val to reflect novelty
  virtual void 	Compute_SetLrate(LeabraLayer* lay, LeabraNetwork* net);
  // set the learning rate for conspec according to final novelty computed value -- called in PostSettle

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override bool CheckConfig_Layer(Layer* lay, bool quiet=false);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(SubiculumLayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

class LEABRA_API HippoEncoderConSpec : public LeabraConSpec {
  // for EC <-> CA1 connections: CHL learning on encoder variables (ru_act_p vs. ru_act_m2) -- soft bounding as specified in spec
INHERITED(LeabraConSpec)
public:
#ifdef __MAKETA__
  LearnMixSpec	lmix;		// #CAT_Learning mixture of hebbian & err-driven learning 
#endif

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_SAvgCor(cg, su);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      float lin_wt = LinFmSigWt(cn->wt);
      C_Compute_dWt(cn, ru, 
		    C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
		    C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m2,
					    su->act_p, su->act_m2));  
    }
  }

  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_dWt_LeabraCHL(cg, su);
  }

  override void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_Weights_LeabraCHL(cg, su);
  }
  override void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    inherited::Compute_Weights_LeabraCHL(cg, su);
  }



  TA_BASEFUNS_NOCOPY(HippoEncoderConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ Initialize(); }
};

class LEABRA_API XCalCHLConSpec : public LeabraConSpec {
  // does CHL-based Leabra learning under XCAL and CAL learning framework -- sometimes CHL performs better, e.g., in the hippocampus..
INHERITED(LeabraConSpec)
public:
#ifdef __MAKETA__
  LearnMixSpec	lmix;		// #CAT_Learning mixture of hebbian & err-driven learning 
#endif
  bool		use_chl;	// use LeabraCHL learning instead of XCAL or CAL learning, even when rest of network is using those other algorithms

  override void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_dWt_LeabraCHL(cg, su);
    else
      inherited::Compute_dWt_CtLeabraXCAL(cg, su);
  }

  override void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_Weights_LeabraCHL(cg, su);
    else
      inherited::Compute_Weights_CtLeabraXCAL(cg, su);
  }

  override void Compute_dWt_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_dWt_LeabraCHL(cg, su);
    else
      inherited::Compute_dWt_CtLeabraCAL(cg, su);
  }

  override void	Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su) {
    if(use_chl)
      inherited::Compute_Weights_LeabraCHL(cg, su);
    else
      inherited::Compute_Weights_CtLeabraCAL(cg, su);
  }

  override void	B_Compute_dWt_CtLeabraXCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    if(use_chl)
      inherited::B_Compute_dWt_LeabraCHL(cn, ru);
    else
      inherited::B_Compute_dWt_CtLeabraXCAL(cn, ru, rlay);
  }
  override void	B_Compute_dWt_CtLeabraCAL(LeabraCon* cn, LeabraUnit* ru, LeabraLayer* rlay) {
    if(use_chl)
      inherited::B_Compute_dWt_LeabraCHL(cn, ru);
    else
      inherited::B_Compute_dWt_CtLeabraCAL(cn, ru, rlay);
  }

  TA_SIMPLE_BASEFUNS(XCalCHLConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};


#endif // leabra_hippo_h
