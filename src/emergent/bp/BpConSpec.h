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

#ifndef BpConSpec_h
#define BpConSpec_h 1

// parent includes:
#include <ConSpec>

// member includes:
#include <Schedule>

// full standalone C++ implementation State code
#include <BpNetworkState_cpp>
#include <BpUnitState_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BpConSpec);

class E_API BpConSpec : public ConSpec {
  // Backpropagation connection specifications -- learning rules etc
INHERITED(ConSpec)
public:

#include <BpConSpec_core>

  Schedule	lrate_sched;	// schedule of learning rate over training epochs (multiplies lrate to produce cur_lrate factor)
  void 		(*decay_fun)(BpConSpec* spec, float& wt, float& dwt);
  // #OBSOLETE #HIDDEN #READ_ONLY #NO_SAVE #LIST_BpConSpec_WtDecay #CONDEDIT_OFF_decay:0 the weight decay function to use

  virtual void	SetCurLrate(BpNetworkState_cpp* net);
  // set current learning rate based on schedule given epoch

  virtual void	LogLrateSched(int epcs_per_step = 50, int n_steps=7);
  // #BUTTON #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks

  TA_BASEFUNS(BpConSpec);
  SIMPLE_COPY(BpConSpec);
  void InitLinks() override;
protected:
  SPEC_DEFAULTS;
  void UpdateAfterEdit_impl() override;
private:
  void Initialize();
  void Destroy() { };
  void Defaults_init();
};

// These are now OBSOLETE -- remove sometime (as of 8/2016, version 8.0)

// #REG_FUN
E_API void Bp_Simple_WtDecay(BpConSpec* spec, float& wt, float& dwt)
// #LIST_BpConSpec_WtDecay #OBSOLETE -- replaced with enum -- Simple weight decay (subtract decay*wt)
     ;				// term here so maketa picks up comment
// #REG_FUN
E_API void Bp_WtElim_WtDecay(BpConSpec* spec, float& wt, float& dwt)
// #LIST_BpConSpec_WtDecay #OBSOLETE -- replaced with enum -- Weight Elimination (Rumelhart) weight decay
     ;				// term here so maketa picks up comment

#endif // BpConSpec_h
