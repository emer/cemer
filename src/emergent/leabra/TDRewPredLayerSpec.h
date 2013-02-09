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

#ifndef TDRewPredLayerSpec_h
#define TDRewPredLayerSpec_h 1

// parent includes:
#include <ScalarValLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(TDRewPredLayerSpec);

class E_API TDRewPredLayerSpec : public ScalarValLayerSpec {
  // predicts rewards: minus phase = clamped prior expected reward V^(t), plus = settles on expectation of future reward V^(t+1)
INHERITED(ScalarValLayerSpec)
public:
  virtual void 	Compute_SavePred(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				 LeabraNetwork* net); // save current prediction to misc_1 for later clamping
  virtual void 	Compute_ClampPred(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				  LeabraNetwork* net); // clamp misc_1 to ext 
  virtual void 	Compute_ClampPrev(LeabraLayer* lay, LeabraNetwork* net);
  // clamp minus phase to previous act value
  virtual void 	Compute_TdPlusPhase(LeabraLayer* lay, LeabraNetwork* net);
  // compute plus phase activations for learning including the td values
    virtual void Compute_TdPlusPhase_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
					 LeabraNetwork* net);
    // #IGNORE 

  override void	Init_Acts(LeabraLayer* lay, LeabraNetwork* net);
  override void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net);
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void	HelpConfig();	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_BASEFUNS_NOCOPY(TDRewPredLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // TDRewPredLayerSpec_h
