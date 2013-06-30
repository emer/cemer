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

#ifndef ActrProceduralModule_h
#define ActrProceduralModule_h 1

// parent includes:
#include <ActrModule>

// member includes:
#include <ActrProduction_Group>
#include <Random>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrUtilityParams);

class E_API ActrUtilityParams : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_ActR procedural parameters for utility learning etc
INHERITED(taOBase)
public:
  bool          learn;  // :ul in ACT-R -- do utilities learn based on reward or not
  float         lrate;  // #CONDSHOW_OFF_util_learn #DEF_0.2 :alpha in ACT-R -- production utility learning rate -- how quickly production utilities change from reward values
  float         init;   // #DEF_0 :iu in ACT-R -- initial utility value for productions
  float         noise;  // :egs in ACT-R -- variance in noise value (s or sigma) added to utilities -- 0 means no noise added to utilities
  float         thresh; // :ut in ACT-R -- threshold utility value to consider a production at all -- set to a very negative number to consider all productions

  float         noise_eff;      // #READ_ONLY #NO_SAVE effective noise value -- transform to form usable by GAUSS rnd noise function
  
  TA_SIMPLE_BASEFUNS(ActrUtilityParams);
protected:
  override void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

eTypeDef_Of(ActrProceduralModule);

class E_API ActrProceduralModule : public ActrModule {
  // procedural module -- responsible for productions
INHERITED(ActrModule)
public:
  enum TraceLevel {
    NO_TRACE,                   // don't add special trace for conflict resolution process -- just shows the one that was selected in the end
    TRACE_ALL,                  // :crt in ACT-R -- log the 'why not' information for all productions (a lot of information!) for each conflict resolution pass
    TRACE_ELIG,                // :cst in ACT-R -- log only the eligible (conflict set) productions for each conflict resolution pass
    UTIL_LEARN,                 // :ult in ACT-R -- log when reward is received and utilities updated
  };

  TraceLevel            trace_level; // how much detail to provide on the conflict resolution process for production selection
  Random                mp_time;    // :dat in ACT-R -- how long in seconds it to match a production -- can specify random time or just mean default of 0.05 seconds = 50 msec
  ActrUtilityParams     util;        // production utility parameters 
  float                 last_rew_time; // #READ_ONLY #SHOW time when last reward was applied -- used to determine which productions to update
  float                 last_rew;    // #READ_ONLY #SHOW last reward that was applied -- just for display /info purposes, not used in computation

  ActrProduction_Group  productions;  // all the productions defined for the model
  ActrProduction_Group  eligible;     // #HIDDEN #NO_SAVE all the eligible productions for current conflict res stage
  ActrProductionRef     fired;          // #HIDDEN #NO_SAVE final selected production to fire

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override bool  SetParam(const String& param_nm, Variant par1, Variant par2);
  override void  Init();

  virtual void  InitUtils();
  // #CAT_ActR init production utilities
  virtual void  AddConflictResEvent();
  // #CAT_ActR add a conflict resolution event to model event stack -- what to do when nothing else going on..
  virtual void  ConflictResolution();
  // #CAT_ActR perform conflict resolution and identify the current production to fire
  virtual void  ChooseFromEligible();
  // #CAT_ActR choose when multiple productions are eligible to fire (based on utilities)
  virtual void  ProductionFired();
  // #CAT_ActR fire off the fired production
  virtual void  ComputeReward(float rew);
  // #CAT_ActR compute reward -- all productions that have fired since the last reward will have their utilities updated

  TA_SIMPLE_BASEFUNS(ActrProceduralModule);
protected:
  override void	 CheckChildConfig_impl(bool quiet, bool& rval);

private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrProceduralModule_h
