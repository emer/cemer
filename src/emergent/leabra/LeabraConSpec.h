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

#ifndef LeabraConSpec_h
#define LeabraConSpec_h 1

// parent includes:
#include "network_def.h"
#include <ConSpec>
#include <SpecMemberBase>

// member includes:
#include <Schedule>
#include <LeabraCon>

#include "ta_vector_ops.h"

#include <LeabraNetworkState_cpp>
#include <LeabraConState_cpp>
#include <LeabraUnitState_cpp>
#include <LeabraLayerState_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:
class LeabraUnit; // 
class LeabraLayer; // 
class LeabraConState_cpp; // 
class LeabraNetwork; // 
class DataTable; // 


eTypeDef_Of(WtScaleSpec);
eTypeDef_Of(XCalLearnSpec);
eTypeDef_Of(WtSigSpec);
eTypeDef_Of(LeabraMomentum);
eTypeDef_Of(WtBalanceSpec);
eTypeDef_Of(AdaptWtScaleSpec);
eTypeDef_Of(SlowWtsSpec);
eTypeDef_Of(DeepLrateSpec);
eTypeDef_Of(MarginLearnSpec);

#include <LeabraConSpec_mbrs>

eTypeDef_Of(LeabraConSpec);

class E_API LeabraConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Leabra Leabra connection specs
INHERITED(ConSpec)
public:

#include <LeabraConSpec_core>  
  
  bool          use_lrate_sched; // #CAT_Learning #CONDSHOW_ON_learn use the lrate_sched learning rate schedule if one exists -- allows learning rate to change over time as a function of epoch count
  Schedule	lrate_sched;	// #CAT_Learning schedule of learning rate over training epochs (NOTE: these factors (lrs_mult) multiply lrate to give the cur_lrate value)


  /////////////////////////////////////
  // General 

  virtual void	Trial_Init_Specs(LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags -- e.g., set current learning rate based on schedule given epoch (or error value)

  virtual void	LogLrateSched(int epcs_per_step = 50, int n_steps=5, int bump_step=-1);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks -- if bump_step > 0 (3 is a good default), the lrate bumps back up to 1 and back down to that step level, and then proceeds from there -- this can pop a model out of a local minimum and result in better final performance
  virtual void	ExpLrateSched(int epcs_per_step = 50, int n_steps=7, float pct_per_step = 0.5);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish an exponentially-decreasing learning rate schedule with given total number of steps (including first step at lrate) and epochs per step -- learning rate changes by pct_per_step at each step in epoch increment
  virtual void	LinearLrateSched(int epcs_per_step = 50, int n_steps=7, float final_lrate_factor = 0.01, bool interpolate = false);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish a linearly-decreasing learning rate schedule with given total number of steps (including first step at lrate) and epochs per step -- learning rate changes linearly to achieve the final lrate factor at the last step -- if interpolate is set then lrate changes every epoch according to linear slope, otherwise it just moves in discrete steps
  virtual void	TriangleLrateSched(int epcs_per_step = 50, int n_cycles=8, float low_factor = 0.1, int log_drop_cycles = 2, bool interpolate = true);
  // #MENU_BUTTON #MENU_ON_LrateSched #CAT_Learning establish a triangle "bump" lrate schedule that oscillates between 1 and low_factor lrate multipliers every 2 * epcs_per_step cycles, for given number of cycles, with a log-lrate-sched drop after given number of cycles -- based on Leslie Smith paper on triangle schedule, and expanding on our prior bump steps  idea -- interpolate allows lrate to change linearly every epoch to produce triangle effect -- otherwise it is just discrete oscillations -- first step is flat initial high lrate before cycling starts

  virtual void	GraphWtSigFun(DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the sigmoidal weight contrast enhancement function (NULL = new data table)
  virtual void	GraphXCaldWtFun(DataTable* graph_data = NULL, float thr_p = 0.25);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the xcal dWt function for given threshold value (NULL = new data table)
  virtual void	GraphXCalSoftBoundFun(DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the xcal soft weight bounding function (NULL = new data table)
  virtual void	GraphSlowWtsFun(int trials = 6000, DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the trajectory of fast and slow weight change dynamics over trials, in response to a single .1 dwt change (NULL = new data table)
  virtual void	GraphLrateSched(DataTable* graph_data = NULL);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the current learning rate schedule

  virtual void 	WtScaleCvt(float slay_kwta_pct=.25, int slay_n_units=100,
                           int n_recv_cons=5, bool norm_con_n=true);
  // #MENU_BUTTON #MENU_ON_Graph helper for converting from old wt_scale computation to new one -- enter parameters for the sending layer kwta pct (overall layer activit), number of receiving connections, and whether the norm_con_n flag was on or off (effectively always on in new version) -- it reports what the effective weight scaling was in the old version, what it is in the new version, and what you should set the wt_scale.abs to to get equivalent performance, assuming wt_scale.abs reflects what was set previously

  bool          SaveConVarToWeights(Network* net, ConState_cpp* cg, MemberDef* md) override;
  
  bool          CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;
  // check for for misc configuration settings required by different algorithms

  void          GetPrjnName(Projection& prjn, String& nm) override;
  String        GetToolbarName() const override { return "connect spec"; }

  void	InitLinks() override;
  SIMPLE_COPY(LeabraConSpec);
  TA_BASEFUNS(LeabraConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(E_API, LeabraConSpec);

#endif // LeabraConSpec_h
