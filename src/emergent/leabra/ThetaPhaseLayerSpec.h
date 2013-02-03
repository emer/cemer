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

#ifndef ThetaPhaseLayerSpec_h
#define ThetaPhaseLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

// timing of ThetaPhase dynamics -- split minus and normal plus:
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

// act_mid = auto encoder minus phase state (in both CA1 and EC_out
//   used by HippoEncoderConSpec relative to act_p plus phase)
// act_m = recall minus phase (normal minus phase dynamics for CA3 recall learning)
// act_p = plus (serves as plus phase for both auto and recall)

// learning just happens at end of trial as usual, but encoder projections use
// the act_mid, act_p variables to learn on the right signals

TypeDef_Of(ThetaPhaseLayerSpec);

class LEABRA_API ThetaPhaseLayerSpec : public LeabraLayerSpec {
  // #AKA_HippoQuadLayerSpec base layer spec for hippocampal layers that implements theta phase learning
INHERITED(LeabraLayerSpec)
public:
  int		auto_m_cycles;	// #DEF_20:80 number of cycles for auto-encoder minus phase, at which point act_mid is recorded for training the EC <-> CA1 auto-encoder -- this should be just long enough for information to reach EC_in and flow through CA1 to EC_out -- will set network min_cycles to be this number plus 20 cycles, which is a minimum for combined assoc and recall minus phases

  virtual void 	RecordActM2(LeabraLayer* lay, LeabraNetwork* net);
  // save current act_nd values as act_mid -- minus phase for auto-encoder learning
  virtual void 	Compute_AutoEncStats(LeabraLayer* lay, LeabraNetwork* net);
  // compute act_dif2 as act_eq - act_mid, and based on that compute error stats as user data on layer (enc_sse, enc_norm_err)

  TA_SIMPLE_BASEFUNS(ThetaPhaseLayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // ThetaPhaseLayerSpec_h
