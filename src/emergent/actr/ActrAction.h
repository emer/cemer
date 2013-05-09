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

#ifndef ActrAction_h
#define ActrAction_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <ActrBuffer>
#include <ActrChunk>
#include <ProgVar>
#include <Program>

// declare all other types mentioned but not required to include:
class ActrModel; //
class ActrProceduralModule; //
class ActrProduction; //


eTypeDef_Of(ActrAction);

class E_API ActrAction : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_ActR one action that is performed when an Act-R production fires
INHERITED(taOBase)
public:
  enum ActionFlags { // #BITS ActR action flags
    AF_NONE             = 0, // #NO_BIT
    OFF                 = 0x0001, // turn this action off -- do not apply it
  };

  enum ActType {
    UPDATE,              // =buffer -- update buffer state of ActR chunk in a buffer with chunk info
    REQUEST,             // +buffer -- request info from buffer (and associated module) according to chunk info
    CLEAR,               // -buffer -- clear chunk from buffer
    OUTPUT,              // print out the val value follwed by contents of chunk if chunk type is set
    STOP,                // stop execution of the model
    PROG_VAR,            // set value of a program variable
    PROG_RUN,            // run a Program (not for official ActR models)
    OBJ_MEMBER,                 // match against a member in an arbitrary object
    DATA_CELL,                  // match against a cell in a data table
  };

  ActionFlags   flags;          // current flags
  ActType       action;         // what type of action to perform
  ActrBufferRef buffer;         // #CONDSHOW_ON_action:UPDATE,REQUEST,CLEAR what buffer to operate on
  ActrChunk     chunk;          // #SHOW_TREE #CONDSHOW_ON_action:UPDATE,REQUEST chunk information for the action
  ProgVarRef    prog_var;       // #CONDSHOW_ON_action:PROG_VAR #PROJ_SCOPE program variable to set to val -- can be in any program -- typically one that is called in another action
  String        val;            // #CONDSHOW_ON_action:PROG_VAR,OUTPUT value to print out or to set program variable to -- can use =var for outputting variable values -- just uses string replace of =var with current bound value -- can intersperse other literal information
  ProgramRef    program;        // #CONDSHOW_ON_action:PROG_RUN program to run -- does not set any arg variables prior to running -- just calls Run on it -- use prior actions to set program variables

  inline void           SetActionFlag(ActionFlags flg)
  { flags = (ActionFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearActionFlag(ActionFlags flg)
  { flags = (ActionFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasActionFlag(ActionFlags flg) const
  { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetActionFlagState(ActionFlags flg, bool on)
  { if(on) SetActionFlag(flg); else ClearActionFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleActionFlag(ActionFlags flg)
  { SetActionFlagState(flg, !HasActionFlag(flg)); }
  // #CAT_Flags toggle flag

  inline bool           IsOff() { return HasActionFlag(OFF); }

  virtual void     SetVarsChunk(ActrProduction& prod, ActrChunk* ck);
  // #CAT_ActR set any variables from current vars in production into chunk
  virtual void     SetVarsString(ActrProduction& prod, String& str);
  // #CAT_ActR set any variables from current vars in production into string

  virtual bool     DoAction(ActrProduction& prod,
                            ActrProceduralModule* proc_mod, ActrModel* model);
  // #CAT_ActR perform the action as specified

  override String  GetTypeDecoKey() const { return "Function"; }
  override String  GetDisplayName() const;
  override String& Print(String& strm, int indent = 0) const;
  override String  GetDesc() const;
  override int     GetEnabled() const { return !HasActionFlag(OFF); }
  override void    SetEnabled(bool value) { SetActionFlagState(OFF, !value); }

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ActrAction);
private:
  SIMPLE_COPY(ActrAction);
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SmartRef_Of(ActrAction); // ActrActionRef

#endif // ActrAction_h
