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

#ifndef ActrNBase_h
#define ActrNBase_h 1

// parent includes:
#include <taNBase>
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class ActrModel; //
class ActrModule; //
class ActrProceduralModule; //
class ActrDeclarativeModule; //
class ActrGoalModule; //
class ActrBuffer; //

eTypeDef_Of(ActrNBase);

class E_API ActrNBase : public taNBase {
  // ##CAT_ActR ##SCOPE_ActrModel base named object for ActR -- provides fast and convenient access to main objs
INHERITED(taNBase)
public:

  ActrModel*              Model() { return own_model; }
  // #CAT_ActrAccess get the model that owns me

  ActrProceduralModule*   ProceduralModule();
  // #CAT_ActrAccess get the procedural module for this model
  ActrDeclarativeModule*  DeclarativeModule();
  // #CAT_ActrAccess get the declarative module for this model
  ActrGoalModule*         GoalModule();
  // #CAT_ActrAccess get the goal module for this model
  ActrModule*             ModuleNamed(const String& nm);
  // #CAT_ActrAccess get a module with the given name
  ActrBuffer*             BufferNamed(const String& nm);
  // #CAT_ActrAccess get a buffer with the given name (use module->buffer for better access to major buffers)

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS_NOCOPY(ActrNBase);
protected:
  ActrModel*            own_model; // the model that owns us -- init in InitLinks

private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrNBase_h
