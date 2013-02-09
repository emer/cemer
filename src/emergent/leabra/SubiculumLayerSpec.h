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

#ifndef SubiculumLayerSpec_h
#define SubiculumLayerSpec_h 1

// parent includes:
#include <ScalarValLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(SubiculumNoveltySpec);

class E_API SubiculumNoveltySpec : public SpecMemberBase {
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

eTypeDef_Of(SubiculumLayerSpec);

class E_API SubiculumLayerSpec : public ScalarValLayerSpec {
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
  void	Defaults_init();
};

#endif // SubiculumLayerSpec_h
