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
  enum ModuleFlags { // #BITS ActR module flags for settings and state
    MF_NONE             = 0, // #NO_BIT
    BUSY                = 0x0001,  // module is busy processing something
    ERROR               = 0x0002,  // module is in an error state
    PREP                = 0x0004,  // module is in a preparation state
    PROC                = 0x0008,  // module is in a processing state
    EXEC                = 0x0010,  // module is in a execution state
  };

  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this module
  ActrBufferRef         buffer; // the main buffer for this module (can add other ones in subclasses)
  ModuleFlags           flags;  // flags for various settings and state of module
  String                last_cmd; // #READ_ONLY #NO_SAVE #SHOW last command (chunk type) executed by module

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
  bool  IsPrep()        { return HasModuleFlag(PREP); }
  bool  IsProc()        { return HasModuleFlag(PROC); }
  bool  IsExec()        { return HasModuleFlag(EXEC); }

  virtual void  InitModule() { };
  // #CAT_ActR initialize the module -- ensure we have our appropriate buffer in model, and set our buffer pointer, etc
  virtual void  ProcessEvent(ActrEvent& event) { };
  // #CAT_ActR process a given event, defined in a module-specific way
  virtual bool  ProcessQuery(ActrBuffer* buf, const String& query,
                             bool why_not = false);
  // #CAT_ActR process queries directed at the given buffer -- buffers delegate queries to their module so that modules can process more advanced queries
  virtual bool  SetParam(const String& param_nm, Variant par1, Variant par2)
  { return false; }
  // #CAT_ActR set given parameter name to given values -- returns true if param was recognized by this module, otherwise false
  virtual void  Init();
  // #CAT_ActR perform run-time initialization at start of processing -- derived classes should call parent which resets basic stuff including buffer and calls InitModule -- don't call that!

  virtual bool  ProcessEvent_std(ActrEvent& event);
  // #CAT_ActR process standard events dealing with state of module or buffer: BUFFER_READ_ACTION, CLEAR_BUFFER, MOD_BUFFER_CHUNK, SET_BUFFER_CHUNK
  virtual bool  ProcessQuery_std(ActrBuffer* buf, const String& query,
                                 bool why_not = false);
  // #CAT_ActR process standard queries directed at the given buffer -- buffers delegate queries to their module so that modules can process more advanced queries

  virtual bool  RequestBufferClear(ActrBuffer* buf);
  // #CAT_ActR call this at start of a request event

  String       GetDesc() const override {return desc;}
  String       GetTypeDecoKey() const override { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrModule);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrModule_h
