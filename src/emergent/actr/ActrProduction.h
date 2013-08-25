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

#ifndef ActrProduction_h
#define ActrProduction_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ActrCondition_List>
#include <ActrAction_List>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrProdUtilVals);

class E_API ActrProdUtilVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR vision parameters
INHERITED(taOBase)
public:
  float                 init; // initial utility of this production -- if non-zero then this production will have a custom initial utility instead of the default one set in the procedural module
  float                 cur;  // #READ_ONLY #SHOW current utility of this production -- updated by reward-based learning
  float                 choice;  // #READ_ONLY #SHOW actual utility value used when choosing a production -- reflects added noise, if any, on top of util value
  float                 rew;  // #DEF_0 reward value associated with the firing of this production -- if non-zero, will drive a ComputeReward function call with this value

  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrProdUtilVals);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrProdTimeVals);

class E_API ActrProdTimeVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR vision parameters
INHERITED(taOBase)
public:
  float                 act; // #DEF_0 action time associated with this production -- if this is non-zero, then it will be used instead of the default value
  float                 last_fire; // #READ_ONLY #SHOW time when this production last fired

  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrProdTimeVals);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrProduction);

class E_API ActrProduction : public taNBase {
  // ##INSTANCE ##CAT_ActR ##SCOPE_ActrModel a production that matches conditions and produces actions
INHERITED(taNBase)
public:
  enum ProdFlags { // #BITS ActR production flags
    PF_NONE             = 0, // #NO_BIT
    OFF                 = 0x0001, // turn this production off -- useful for temporarily disabling different productions
    PSTAR               = 0x0002, // this is a p* production that can have variablized slot names to match against
    FIRED               = 0x0004, // this production just fired at last conflict resolution step
    ELIGIBLE            = 0x0008, // this production was eligible to fire at last conflict resolution step
  };

  ProdFlags             flags;  // flag state of the production
  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this production -- what does it do?
  ActrProdUtilVals      util;  // utility values for this production
  ActrProdTimeVals      time;  // time values for this production
  ActrCondition_List    conds; // conditions that must be matched to fire this production
  ActrAction_List       acts;  // actions that this production causes when it fires
  ActrSlot_List         vars;  // #NO_SAVE #NO_EXPAND_ALL variable bindings used in matching and instantiating actions -- automatically generated from =varname variable names listed in the productions

  inline void           SetProdFlag(ProdFlags flg)
  { flags = (ProdFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void           ClearProdFlag(ProdFlags flg)
  { flags = (ProdFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool           HasProdFlag(ProdFlags flg) const
  { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void           SetProdFlagState(ProdFlags flg, bool on)
  { if(on) SetProdFlag(flg); else ClearProdFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleProdFlag(ProdFlags flg)
  { SetProdFlagState(flg, !HasProdFlag(flg)); }
  // #CAT_Flags toggle flag

  bool          IsOff() const { return HasProdFlag(OFF); }

  inline float  Compute_Util(float rewval, float lrate) {
    util.cur += lrate * (rewval - util.cur);
    return util.cur;
  }
  // #CAT_ActR update the utility of this production from given reward value and learning rate

  virtual void          Init();
  // #CAT_ActR initialize production for start of a new run -- runs check config and updates vars etc

  virtual ActrSlot*     FindVar(const String& nm, ActrBuffer*& buf, bool err_msg = true);
  // #CAT_ActR find given variable by name (name does NOT have = at front) -- if variable name is not found, then available buffer names are checked and returned in buf pointer if found -- if err_msg then an error message is emitted if no variable is found
  virtual String        PrintVars() const;
  // #CAT_ActR print current values of the variables
  virtual void          UpdateVars();
  // #CAT_ActR update the vars based on what shows up in the conditions -- called automatically in UAE
  virtual void          UpdateNames();
  // #CAT_ActR update names of chunks within conds and acts, based on name of production
  virtual void          InitActionProgs();
  // #CAT_ActR initialize all the programs called directly by actions -- called by Init

  virtual bool          Matches(bool why_not = false);
  // #CAT_ActR do the conditions match now or not? called by procedural module
  virtual bool          WhyNot();
  // #BUTTON #CAT_ActR explain why this production does not match right now (report given on the css console output)

  virtual void          SendBufferReads(ActrProceduralModule* proc_mod, ActrModel* model);
  // #CAT_ActR send BUFFER-READ-ACTION events to all the buffers we read when we fire, sent from procedural module to buffer owning module

  virtual bool          DoActions(ActrProceduralModule* proc_mod, ActrModel* model);
  // #CAT_ActR the production fired -- perform all the actions

  virtual ActrCondition* FindCondOnBuffer(ActrBuffer* buf);
  // #CAT_ActR find a condition that matches on given buffer

  virtual bool          SetParam(const String& par_nm, float val);
  // #CAT_ActR set production paramter -- u = util, r = reward
 
  override String GetDesc() const {return desc;}
  override String GetTypeDecoKey() const { return "ProgCtrl"; }
  override int    GetEnabled() const { return !IsOff(); }
  override void   SetEnabled(bool value) { SetProdFlagState(OFF, !value); }
  override int    GetSpecialState() const;

  TA_SIMPLE_BASEFUNS(ActrProduction);
protected:
  override void  UpdateAfterEdit_impl();
  override void  CheckThisConfig_impl(bool quiet, bool& rval);
  override void	 CheckChildConfig_impl(bool quiet, bool& rval);

private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SmartRef_Of(ActrProduction); // ActrProductionRef

#endif // ActrProduction_h
