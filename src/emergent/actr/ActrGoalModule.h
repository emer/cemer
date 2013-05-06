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

#ifndef ActrGoalModule_h
#define ActrGoalModule_h 1

// parent includes:
#include <ActrModule>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrGoalModule);

class E_API ActrGoalModule : public ActrModule {
  // goal module -- responsible for maintainence of the goal buffer
INHERITED(ActrModule)
public:
  ActrChunk_List        init_chunk; // initial chunk to start pre-loaded into goal buffer

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override void  Init();

  TA_SIMPLE_BASEFUNS(ActrGoalModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrGoalModule_h
