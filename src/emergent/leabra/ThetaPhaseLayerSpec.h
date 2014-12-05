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

// timing of ThetaPhase dynamics -- based on quarter structure:
// [  q1      ][  q2  q3  ][     q4     ]
// [ ------ minus ------- ][ -- plus -- ]
// [   auto-  ][ recall-  ][ -- plus -- ]

//  DG -> CA3 -> CA1
//  /    /      /    \
// [----EC_in---] -> [ EC_out ]

// minus phase: EC_out unclamped, driven by CA1
// auto-   CA3 -> CA1 = 0, EC_in -> CA1 = 1
// recall- CA3 -> CA1 = 1, EC_in -> CA1 = 0

// plus phase: EC_in -> EC_out auto clamped
// CA3 -> CA1 = 0, EC_in -> CA1 = 1
// (same as auto- -- training signal for CA3 -> CA1 is what EC would produce!

// act_q1 = auto encoder minus phase state (in both CA1 and EC_out
//   used by HippoEncoderConSpec relative to act_p plus phase)
// act_q3 / act_m = recall minus phase (normal minus phase dynamics for CA3 recall learning)
// act_a4 / act_p = plus (serves as plus phase for both auto and recall)

// learning just happens at end of trial as usual, but encoder projections use
// the act_q2, act_p variables to learn on the right signals

// todo: implement a two-trial version of the code to produce a true theta rhythm
// integrating over two adjacent alpha trials..

eTypeDef_Of(ThetaPhaseLayerSpec);

class E_API ThetaPhaseLayerSpec : public LeabraLayerSpec {
  // #AKA_HippoQuadLayerSpec base layer spec for hippocampal layers that implements theta phase learning -- sets global options -- quarter timing: q1 is auto-encoder minus phase, q2, q3 are recall, q4 is common plus phase
INHERITED(LeabraLayerSpec)
public:

  TA_SIMPLE_BASEFUNS(ThetaPhaseLayerSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // ThetaPhaseLayerSpec_h
