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

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrProceduralModule);

class E_API ActrProceduralModule : public ActrModule {
  // procedural module -- responsible for productions
INHERITED(ActrModule)
public:
  // todo: find official actr parameter names for these things!
  float                 mp_time;    // #DEF_0.05 how long in seconds it to match a production
  float                 util_lrate; // #DEF_0.2 production utility learning rate (alpha)
  float                 util_noise; // noise value (sigma) for the production selection process

  ActrProduction_Group  productions;  // all the productions defined for the model
  ActrProduction_Group  eligible;     // #HIDDEN #NO_SAVE all the eligible productions for current conflict res stage
  ActrProductionRef     fired;          // #HIDDEN #NO_SAVE final selected production to fire

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override void  Init();

  virtual void  AddConflictResEvent();
  // add a conflict resolution event to model event stack -- what to do when nothing else going on..
  virtual void  ConflictResolution();
  // perform conflict resolution and identify the current production to fire
  virtual void  ProductionFired();
  // fire off the fired production

  TA_SIMPLE_BASEFUNS(ActrProceduralModule);
protected:
  override void	 CheckChildConfig_impl(bool quiet, bool& rval);

private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrProceduralModule_h
