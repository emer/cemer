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

#ifndef ActrCondition_h
#define ActrCondition_h 1

// parent includes:
#include <taOBase>
#include <taSmartRefT>

// member includes:
#include <ActrChunk>
#include <Relation>
#include <DataTable>

// declare all other types mentioned but not required to include:
class ActrProceduralModule; //
class ActrModel; //
class ActrProduction; //

eTypeDef_Of(ActrCondition);

class E_API ActrCondition : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_ActR one condition that must be met to fire an Act-R production
INHERITED(taOBase)
public:
  enum CondFlags { // #BITS ActR condition flags
    CF_NONE             = 0, // #NO_BIT
    OFF                 = 0x0001, // turn this condition off -- do not use it for matching
    BUF_UPDT_ACT        = 0x0002, // for BUFFER_EQ, buffer has an update action, so don't send a BUFFER-READ-ACTION
  };

  enum CondSource {
    BUFFER_EQ,                  // = match against an ActR chunk in a buffer (for regular ActR model) 
    BUFFER_QUERY,               // ? check state of buffer or module -- std vals are: buffer: full, empty, requested, unrequested, state (or preparation, processor, execution): busy, free, error -- can chain multiple with ; separator
    PROG_VAR,                   // match against value of a program variable (for use in controlling system) -- must be a global (args or vars) variable, not a local var
    NET_UNIT,                   // match against unit activation in a network
    OBJ_MEMBER,                 // match against a member in an arbitrary object
    DATA_CELL,                  // match against a cell in a data table
  };

  CondFlags     flags;         // current flags
  CondSource    cond_src;      // what is the source of the data to match against
  TypeDef*      src_type;      // #CONDSHOW_ON_cond_src:OBJ_MEMBER #NO_NULL #TYPE_taBase type of object with source data to match against
  taBaseRef     src;           // #TYPE_ON_src_type #PROJ_SCOPE the source object to obtain data to match against (e.g., buffer, etc)
  String        unit_name;     // #CONDSHOW_ON_cond_src:NET_UNIT name of unit within layer to obtain unit value from -- can only access named units
  String	unit_val;      // #CONDSHOW_ON_cond_src:NET_UNIT variable name on unit to obtain unit value from
  String        obj_path;      // #CONDSHOW_ON_cond_src:OBJ_MEMBER path within object to obtain comparison value from
  String        dt_col_name;   // #CONDSHOW_ON_cond_src:DATA_CELL name of column within data table cell to obtain value from
  int           dt_row;        // #CONDSHOW_ON_cond_src:DATA_CELL row number within data table cell to obtain value from (use -1 for last row)
  int           dt_cell;       // #CONDSHOW_ON_cond_src:DATA_CELL cell index within data row,column within data table cell to obtain value from
  Relation::Relations  rel;    // #CONDSHOW_OFF_cond_src:BUFFER_EQ,BUFFER_QUERY relationship between source value and comparison value
  String        cmp_val;       // #CONDSHOW_OFF_cond_src:BUFFER_EQ comparison value -- for query std options are buffer: full, empty, requested, unrequested, state (or preparation, processor, execution): busy, free, error -- can chain multiple with ; separator
  ActrChunk     cmp_chunk;     // #CONDSHOW_ON_cond_src:BUFFER_EQ #CONDTREE_ON_cond_src:BUFFER_EQ #SHOW_TREE comparison chunk -- fill in chunk type and all chunk values that should match.  use =name for variable copying

  inline void           SetCondFlag(CondFlags flg)
  { flags = (CondFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearCondFlag(CondFlags flg)
  { flags = (CondFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasCondFlag(CondFlags flg) const
  { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetCondFlagState(CondFlags flg, bool on)
  { if(on) SetCondFlag(flg); else ClearCondFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleCondFlag(CondFlags flg)
  { SetCondFlagState(flg, !HasCondFlag(flg)); }
  // #CAT_Flags toggle flag

  inline bool           IsOff() { return HasCondFlag(OFF); }

  virtual void  UpdateVars(ActrProduction& prod);
  // #CAT_ActR update the production variable list based on what shows up in the conditions 

  virtual bool  MatchVarVal(const Variant& var, bool why_not = false);
  // #IGNORE match variant value against cmp_val using rel -- used by matches
  virtual bool  Matches(ActrProduction& prod, bool why_not = false);
  // #CAT_ActR does this condition match?
  virtual bool  MatchVars(ActrProduction& prod, bool why_not = false);
  // #CAT_ActR second pass on matching -- check for any tests against cur vars -- return true if OK and false if fails variable comparison test

  virtual void  SendBufferReads(ActrProceduralModule* proc_mod, ActrModel* model);
  // #CAT_ActR send BUFFER-READ-ACTION events for all BUFFER_EQ cases

  virtual bool  SetVal(ActrSlot* slt, const String& val,
                       Relation::Relations rl = Relation::EQUAL);
  // #IGNORE for parsing -- set comparison value, either for given slot or if null then cmp_val
  virtual bool  SetQuery(const String& sys, const String& val, bool neg = false);
  // #IGNORE for parsing -- set query value, e.g., "state", "free"

  override String  GetTypeDecoKey() const { return "ProgCtrl"; }
  override String  GetDisplayName() const;
  override String& Print(String& strm, int indent = 0) const;
  override String  GetDesc() const;
  override int     GetEnabled() const { return !HasCondFlag(OFF); }
  override void    SetEnabled(bool value) { SetCondFlagState(OFF, !value); }

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ActrCondition);
protected:
  override void  UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);

private:
  SIMPLE_COPY(ActrCondition);
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SmartRef_Of(ActrCondition); // ActrConditionRef

#endif // ActrCondition_h
