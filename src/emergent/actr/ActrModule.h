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

#ifndef ActrModule_h
#define ActrModule_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ActrModuleRef>
#include <ActrBuffer>

// declare all other types mentioned but not required to include:
class ActrModel; //
class ActrEvent; //

eTypeDef_Of(ActrModule);

class E_API ActrModule : public taNBase {
  // ##INSTANCE ##CAT_ActR base class for ACT-R modules
INHERITED(taNBase)
public:
  enum ModuleState {
    MS_FREE,                    // module is free to take on new tasks
    MS_BUSY,                    // module is busy processing something
    MS_ERROR,                   // module is in an error state
  };
 
  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this module
  ActrBufferRef         buffer; // the main buffer for this module (can add other ones in subclasses)
  ModuleState           state;  // #READ_ONLY #SHOW current state of the module

  virtual void  InitModule() { };
  // #CAT_ActR initialize the module -- ensure we have our appropriate buffer in model, and set our buffer pointer, etc
  virtual void  ProcessEvent(ActrEvent& event) { };
  // #CAT_ActR process a given event, defined in a module-specific way
  virtual void  Init() { };
  // #CAT_ActR perform run-time initialization at start of processing

  //  virtual bool  ProcessEvent_State(ActrEvent& event);
  // #CAT_ActR process events dealing with state of module or buffer

  ActrModel*            Model() { return own_model; }
  // #CAT_ActR get the model that owns me
  override String       GetDesc() const {return desc;}
  override String       GetTypeDecoKey() const { return "Program"; }

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ActrModule);
protected:
  ActrModel*            own_model; // the model that owns us -- init in InitLinks
  
private:
  SIMPLE_COPY(ActrModule);
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrModule_h
