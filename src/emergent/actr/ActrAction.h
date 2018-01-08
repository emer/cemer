// Copyright 2013-2017, Regents of the University of Colorado,
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
#include <taSmartRefT>

// member includes:
#include <ActrBuffer>
#include <ActrChunk>

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
    OVERWRITE,           // =buffer =value -- directly overwrite contents of buffer with given variable, specified in val
    REQUEST,             // +buffer -- request info from buffer (and associated module) according to chunk info
    REQUEST_DIR,         // +buffer =value -- a direct chunk request -- full request is provided in chunk specified in val
    CLEAR,               // -buffer -- clear chunk from buffer
    STOP,                // stop execution of the model
    PROG_RUN,            // run a Program -- typically this is NOT the program that owns and manages this model -- use a separate helper program to perform actions -- set variables using PROG_VAR before calling
    OUTPUT,              // print out the val value follwed by contents of chunk if chunk type is set
    PROG_VAR,            // set value of a program variable
    OBJ_MEMBER,          // set value in a member in an arbitrary object
    DATA_CELL,           // set value in a cell in a data table
  };

  ActionFlags   flags;          // current flags
  ActType       action;         // what type of action to perform
  TypeDef*      dest_type;      // #CONDSHOW_ON_action:OBJ_MEMBER #NO_NULL #TYPE_taBase type of object with dest data to write to
  taBaseRef     dest;           // #CONDSHOW_OFF_action:OUTPUT,STOP #TYPE_ON_dest_type #PROJ_SCOPE the destination object to write val to or call when production fires -- a buffer, prog var, program, object, or data table..
  String        obj_path;      // #CONDSHOW_ON_action:OBJ_MEMBER path within object to obtain comparison value from
  String        dt_col_name;   // #CONDSHOW_ON_action:DATA_CELL name of column within data table cell to obtain value from
  int           dt_row;        // #CONDSHOW_ON_action:DATA_CELL row number within data table cell to obtain value from (use -1 for last row)
  int           dt_cell;        // #CONDSHOW_ON_action:DATA_CELL cell index within data row,column within data table cell to obtain value from
  String        val;            // #CONDSHOW_OFF_action:UPDATE,REQUEST,CLEAR,STOP,PROG_RUN value to print out or to set program variable to -- can use =var for outputting variable values -- just uses string replace of =var with current bound value -- can intersperse other literal information
  String        params;         // #CONDSHOW_ON_action:UPDATE,REQUEST extra module-specific parameters for UPDATE and REQUEST actions -- these are prefixed with a colon : in ACT-R -- do not include the colon here, and separate with commas if multiple -- refer to specific module docs for available options
  ActrChunk     chunk;          // #CONDSHOW_ON_action:UPDATE,REQUEST #TREE_SHOW #CONDTREE_ON_action:UPDATE,REQUEST chunk information for the action
  

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

  virtual bool     SetChunkTypeFromCond();
  // #CAT_ActR if our chunk type is not set, set it from any corresponding production conditional that matches against the same buffer as we do -- called in UAE and during loading from actr file
  virtual bool     SetBangAction(const String& act);
  // #CAT_ActR set action from !action! keyword (minus the ! ! surrounds) -- used in parsing

  virtual void     InitProg();
  // #CAT_ActR initialize program if in use

  String  GetTypeDecoKey() const override { return "Function"; }
  String  GetDisplayName() const override;
  String& Print(String& strm, int indent = 0) const override;
  String  GetDesc() const override;
  int     GetEnabled() const override { return !HasActionFlag(OFF); }
  void    SetEnabled(bool value) override { SetActionFlagState(OFF, !value); }

  void  InitLinks() override;
  void  CutLinks() override;
  TA_BASEFUNS(ActrAction);
protected:
  void  UpdateAfterEdit_impl() override;
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  SIMPLE_COPY(ActrAction);
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SMARTREF_OF(E_API, ActrAction); // ActrActionRef

#endif // ActrAction_h
