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
#include <ActrNBase>

// member includes:
#include <ActrModuleRef>
#include <ActrBuffer>

// declare all other types mentioned but not required to include:
class ActrEvent; //

eTypeDef_Of(ActrModule);

class E_API ActrModule : public ActrNBase {
  // base class for ACT-R modules
INHERITED(ActrNBase)
public:
  enum ModuleFlags { // #BITS ActR module flags
    MF_NONE             = 0, // #NO_BIT
    BUSY                = 0x0001,  // module is busy processing something
    ERROR               = 0x0002,  // module is in an error state
  };

  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this module
  ActrBufferRef         buffer; // the main buffer for this module (can add other ones in subclasses)
  ModuleFlags           flags;  // flags for various settings of module

  inline void           SetModuleFlag(ModuleFlags flg)
  { flags = (ModuleFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearModuleFlag(ModuleFlags flg)
  { flags = (ModuleFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasModuleFlag(ModuleFlags flg) const
  { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetModuleFlagState(ModuleFlags flg, bool on)
  { if(on) SetModuleFlag(flg); else ClearModuleFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleModuleFlag(ModuleFlags flg)
  { SetModuleFlagState(flg, !HasModuleFlag(flg)); }
  // #CAT_Flags toggle model flag

  bool  IsBusy()        { return HasModuleFlag(BUSY); }
  bool  IsFree()        { return !IsBusy(); }
  bool  IsError()       { return HasModuleFlag(ERROR); }

  virtual void  InitModule() { };
  // #CAT_ActR initialize the module -- ensure we have our appropriate buffer in model, and set our buffer pointer, etc
  virtual void  ProcessEvent(ActrEvent& event) { };
  // #CAT_ActR process a given event, defined in a module-specific way
  virtual void  Init();
  // #CAT_ActR perform run-time initialization at start of processing -- derived classes should call parent which resets basic stuff including buffer and calls InitModule -- don't call that!

  virtual bool  ProcessEvent_std(ActrEvent& event);
  // #CAT_ActR process standard events dealing with state of module or buffer: BUFFER-READ-ACTION, CLEAR-BUFFER, MOD-BUFFER-CHUNK

  override String       GetDesc() const {return desc;}
  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrModule);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrModule_h
