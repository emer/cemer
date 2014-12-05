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

#ifndef CA1UnitSpec_h
#define CA1UnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

// timing of ThetaPhase dynamics -- based on quarter structure:
// [  q1      ][  q2  q3  ][     q4     ]
// [ ------ minus ------- ][ -- plus -- ]
// [   auto-  ][ recall-  ][ -- plus -- ]

//  DG -> CA3 -> CA1
//  /    /      /    \
// [----ECin---] -> [ ECout ]

// minus phase: ECout unclamped, driven by CA1
// auto-   CA3 -> CA1 = 0, ECin -> CA1 = 1
// recall- CA3 -> CA1 = 1, ECin -> CA1 = 0

// plus phase: ECin -> ECout auto clamped
// CA3 -> CA1 = 0, ECin -> CA1 = 1
// (same as auto- -- training signal for CA3 -> CA1 is what EC would produce!

// act_q1 = auto encoder minus phase state (in both CA1 and ECout
//   used by HippoEncoderConSpec relative to act_p plus phase)
// act_q3 / act_m = recall minus phase (normal minus phase dynamics for CA3 recall learning)
// act_a4 / act_p = plus (serves as plus phase for both auto and recall)

// learning just happens at end of trial as usual, but encoder projections use
// the act_q2, act_p variables to learn on the right signals

// todo: implement a two-trial version of the code to produce a true theta rhythm
// integrating over two adjacent alpha trials..


eTypeDef_Of(CA1UnitSpec);

class E_API CA1UnitSpec : public LeabraUnitSpec {
  // unit spec for CA1 layers that implements ThetaPhase learning -- modulates ECin and CA1 weight scale strengths (wt_scale.abs = 0 or 1) in conspecs -- must use unique conspecs for these projections -- ECin = 1, CA3 = 0 for 1st quarter, then ECin = 0, CA3 = 1 until q4, where it goes back to ECin = 1, CA3 = 0 for plus phase
INHERITED(LeabraUnitSpec)
public:
  float		recall_decay; 		// #DEF_1 proportion to decay activations at start of recall phase
  float		plus_decay; 		// #DEF_1 proportion to decay activations at start of plus phase
  bool		use_test_mode;		// #DEF_true if network train_mode == TEST, then don't modulate CA3 off in plus phase, and keep ECin -> CA1 on, and don't decay -- makes it more likely to at least get input parts right

  void Compute_NetinScale(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  bool CheckConfig_Unit(Unit* un, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(CA1UnitSpec);
protected:
  SPEC_DEFAULTS;

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

#endif // CA1UnitSpec_h
