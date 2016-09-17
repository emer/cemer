// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ProgEl_h
#define ProgEl_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <ProgVarRef>

// declare all other types mentioned but not required to include:
class Program; // 
taTypeDef_Of(Program);
class LocalVars; // 
class taiWidgetItemChooser; //
taTypeDef_Of(taiWidgetItemChooser);


/////////////////////////////////////////////////////////////////////
//              IMPORTANT CODING NOTES:

// you must use PROGEL_SIMPLE_BASEFUNS macro instead of TA_SIMPLE_BASEFUNS
// for all ProgEl types that contain a ProgVarRef or a ProgExpr
// or, use PROGEL_SIMPLE_COPY or directly call UpdateAfterCopy if defining
// custom copy functions.

// Note: ProgEl now automatically does update stuff in UpdateAfterMove_impl
// and does CheckProgVarRef in CheckThisConfig, so these calls are not necc

#define PROGEL_SIMPLE_COPY(T) \
  void Copy_(const T& cp) {T::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp); \
}

#define PROGEL_SIMPLE_BASEFUNS(T) \
  PROGEL_SIMPLE_COPY(T);  \
  SIMPLE_LINKS(T); \
  TA_BASEFUNS(T)


taTypeDef_Of(ProgEl);

class TA_API ProgEl: public taOBase {
  // #VIRT_BASE #STEM_BASE ##EDIT_INLINE ##SCOPE_Program ##CAT_Program base class for a program element
friend class ProgExprBase;
INHERITED(taOBase)
public:
  int          edit_move_after; // #IGNORE where to move after browser edit effects

  // Signature of following functions must match that of the item_filter_fun typedef.
  static bool           StdProgVarFilter(void* base, void* var);
  // generic progvar filter -- excludes variables from functions if not itself in same function -- use this for most progvars in ITEM_FILTER comment directive
  static bool            NewProgVarCustChooser(taBase* base, taiWidgetItemChooser* chooser); 
  // add NewGlobalVar and NewLocalVar options to the chooser
  static bool           ObjProgVarFilter(void* base, void* var); // Object* progvar filter -- only shows Object* items -- use in ITEM_FILTER comment directive
  static bool           DataProgVarFilter(void* base, void* var); // data table* progvar filter -- only shows DataTable* items -- use in ITEM_FILTER comment directive
  static bool           DynEnumProgVarFilter(void* base, void* var); // DynEnum progvar filter -- only shows DynEnum items -- use in ITEM_FILTER comment directive

  enum ProgFlags { // #BITS flags for modifying program element function or other information
    PEF_NONE            = 0, // #NO_BIT
    OFF                 = 0x0001, // inactivated: does not generate code
    NON_STD             = 0x0002, // non-standard: not part of the standard code for this program -- a special purpose modification (just for user information/highlighting)
    NEW_EL              = 0x0004, // new element: this element was recently added to the program (just for user information/highlighting)
    VERBOSE             = 0x0008, // print informative message about the operation of this program element to std output (e.g., css console or during -nogui startup) -- useful for debugging and for logging key steps during startup
    QUIET               = 0x0010, // turn off warning messages if they are not relevant
    PROG_ERROR          = 0x0200, // #NO_SHOW css error was triggered at this prog el
    WARNING             = 0x0400, // #NO_SHOW css warning was triggered at this prog el
    CAN_REVERT_TO_CODE  = 0x0800, // #NO_SHOW can revert to program code string -- computes whether this prog el can revert back to a ProgCode -- used for enabling button
    BREAKPOINT_ENABLED  = 0x1000, // #NO_SHOW breakpoint (ENABLED & DISABLED are mutually exclusive)
    BREAKPOINT_DISABLED = 0x2000, // #NO_SHOW breakpoint exists but is currently disabled
};

  String                desc; // #EDIT_DIALOG #HIDDEN_INLINE optional brief description of element's function; included as comment in script
  ProgFlags             flags;  // flags for modifying program element function or providing information about the status of this program element
  String                code_string; // #AKA_orig_prog_code #READ_ONLY #SHOW most recently uncompiled program code -- used for reverting
  String                pre_compile_code_string; // #HIDDEN hold on to this for updating code_string if compile successful
  virtual ProgEl*       parent() const
  { return (ProgEl*)const_cast<ProgEl*>(this)->GetOwner(&TA_ProgEl); }
  Program*              program() { return GET_MY_OWNER(Program); }
  virtual int           ProgElChildrenCount() const { return 0; }
  // number of program element children under this object -- overload for containers (loops, conditionals, etc)

  void                  PreGen(int& item_id); //recursive walk of items before code gen; each item bumps its id and calls subitems; esp. used to discover subprogs in order
  virtual void          GenCss(Program* prog); // generate the Css code for this object (usually override _impl's)
  virtual const String  GenListing(int indent_level = 0); // generate a listing of the program

  inline void           SetProgFlag(ProgFlags flg)   { flags = (ProgFlags)(flags | flg); }
  // set flag state on
  inline void           ClearProgFlag(ProgFlags flg) { flags = (ProgFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasProgFlag(ProgFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetProgFlagState(ProgFlags flg, bool on)
  { if(on) SetProgFlag(flg); else ClearProgFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           ToggleProgFlag(ProgFlags flg)
  { SetProgFlagState(flg, !HasProgFlag(flg)); }
  // toggle program flag

  virtual bool          EditProgramEl();
  // #BUTTON #MENU #MENU_CONTEXT #NO_BUSY #CAT_Code view the program code lines associated with this program element, as highlighted lines within the overall program code
  virtual bool          ViewScript();
  // #BUTTON #MENU #MENU_CONTEXT #NO_BUSY #CAT_Code view the css script associated with this program element, as highlighted lines within the overall program code
  virtual bool          ScriptLines(int& start_ln, int& end_ln);
  // #CAT_Code get the line numbers that this program element generated within the program script
  void                  SetOffFlag(bool off);
  // set the OFF flag to given state: flag indicates whether code element should be run or not
  void                  ToggleOffFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle the OFF flag to opposite of current state: flag indicates whether code element should be run or not
  void                  SetNonStdFlag(bool non_std);
  // set non standard flag to given state: flag indicates that this is not part of the standard code for this program -- a special purpose modification (just for user information/highlighting)
  void                  ToggleNonStdFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle non standard flag to opposite of current state: flag indicates that this is not part of the standard code for this program -- a special purpose modification (just for user information/highlighting)
  void                  SetNewElFlag(bool new_el);
  // set new element flag to given state: flag indicates that this element was recently added to the program (just for user information/highlighting)
  void                  ToggleNewElFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle new element flag to opposite of current state: flag indicates that this element was recently added to the program (just for user information/highlighting)
  void                  SetVerboseFlag(bool new_el);
  // set verbose flag to given state: when this part of the program is run, an informational message will be printed out on the css Console -- very useful for debugging
  void                  ToggleVerboseFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle verbose flag to opposite of current state: when this part of the program is run, an informational message will be printed out on the css Console -- very useful for debugging
  void                  ToggleBreakpoint();
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #DYN1 add or remove a breakpoint: a breakpoint will stop execution of the program at this point, so program variables can be examined, etc -- use the CmdShell button to access debugging information in the css console for this program
  void                  ToggleBreakEnable();
  // #MENU #MENU_ON_Object #DYN1 toggle whether a breakpoint is enabled or not -- if no breakpoint is set, then it will set a breakpoint in the first place, otherwise it will toggle the enabled status of that breakpoint -- see ToggleBreakpoint for more info
  virtual bool          RevertToCode();
  // #BUTTON #GHOST_OFF_flags:CAN_REVERT_TO_CODE revert this program element back to a ProgCode element -- use this if the conversion did not proceed as expected

  virtual ProgVar*      FindVarName(const String& var_nm) const;
  // find given variable within this program element -- NULL if not found
  virtual LocalVars*     FindLocalVarList() const;
  // find local variable list at the closest level of scope to this program element
  virtual ProgVar*      MakeLocalVar(const String& var_nm);
  // make a new local variable with the given name -- creates a local vars if none found
  virtual ProgVar*      FindVarNameInScope(String& var_nm, bool else_make = false);
  // find variable name at the closest level of scope to this program element -- if else_make, then offer the option of creating the variable in global or local scope if not found

  virtual void          SetProgExprFlags() { };
  // special temporary function to set flags for any ProgExpr objects -- needed for new css parsing and loading of old projects which saved these flags causes errors, so this fixes that.. todo: remove me after a few releases (introduced in 4.0.10)

  virtual  bool         CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  // #IGNORE can this program element type be converted from given code (ProgCode) text string -- code has had whitespace trimmed at start -- scope_el provides scope information for relevant variables etc where this new prog el would be created -- use FindVarNameInScope etc on that obj
  virtual  bool         CvtFmCode(const String& code);
  // #IGNORE go ahead and convert the code (ProgCode) text string into this program element type  -- code has had whitespace trimmed at start
  virtual  bool         CvtFmSavedCode();
  // call CvtFmCode on code_string string -- for a callback
  virtual  bool         CvtCodeToVar(String& code_str);
  // attempt to convert the code to a new variable declaration -- prompts for var location -- if true, then it was interpreted as a var decl, and var decl is removed from code -- e.g., if input is "int i = 20" then remaining code will be "i = 20" -- if nothing but a decl (e.g., "int i"), then code is empty, and nothing left to do
  virtual bool          CvtFmCodeCheckNames(const String& code) const;
  // #IGNORE check the toolbar name and the type def name returning true if match and make sure it isn't just the start of some variable name
  bool                  BrowserEditEnable() override { return true; }
  bool                  BrowserEditSet(const String& code, int move_after = 0) override;
  virtual  String       CodeGetDesc(const String& code);
  // #IGNORE get description (comment) from the code string -- returns code without the comment
  virtual bool          BrowserEditTest();
  // #EXPERT test the parsing functions of this program element -- just compares output of BrowserEditString before and after running BrowserEditSet on BrowserEditString -- does the thing parse its own output string?
  virtual bool          BrowserEditTest_impl();
  // #IGNORE basic compare of just this element -- just compares output of BrowserEditString before and after running BrowserEditSet on BrowserEditString -- does the thing parse its own output string?

  virtual  bool         IsCtrlProgEl()  { return false; }
  // #IGNORE set this to true for any program element that is a basic control element, such as loops (for, while), if, switch, etc -- these have special parsing status

  // void                  UpdateAfterMove(taBase* old_owner) override;
  // #IGNORE called after object has been moved from one location to another in the object hierarchy (i.e., list Transfer fun) -- actual functions should be put in the _impl version which should call inherited:: etc just as for UAE -- use for updating pointers etc
  // void                 UpdatePointersAfterCopy_(const taBase& cp) override;

  virtual void UpdateProgElVars(const taBase* old_scope, taBase* new_scope);
  // update our progvar elements after change in scope (move, copy)
  
  bool         BrowserSelectMe() override;
  bool         BrowserExpandAll() override;
  bool         BrowserCollapseAll() override;

  String       GetStateDecoKey() const override;
  int          GetEnabled() const override;
  // note: it is our own, plus disabled if parent is
  void         SetEnabled(bool value) override;
  String       GetDesc() const override {return desc;}
  const String GetToolTip(const KeyString& key) const override;
  String       GetColText(const KeyString& key, int itm_idx = -1) const override;
  bool         IsVerbose() const { return HasProgFlag(VERBOSE); }

  void         EnableBreakpoint();
  // #IGNORE just sets the flags
  void         DisableBreakpoint();
  // #IGNORE just sets the flags
  void         SetBreakpoint();
  // #IGNORE just sets the flags
  void         ClearBreakpoint();
  // #IGNORE just sets the flags
  
  virtual bool          InDebugMode();
  // test whether we are in debug mode or not -- calls this method on parent program group

  String StringFieldLookupFun(const String& cur_txt, int cur_pos,
                              const String& mbr_name, int& new_pos) override;

  TA_BASEFUNS(ProgEl);

protected:
  void         UpdateAfterEdit_impl() override;
  void         UpdateAfterMove_impl(taBase* old_owner) override;
  // uses type information to do a set of automatic updates of pointers (smart refs) after copy
  void         UpdatePointersAfterCopy_impl(const taBase& cp) override;
  // copies progvar references when copying expressions using vars when copying a program element and not a parent e.g. an entire program
  void         CheckError_msg(const String& a, const String& b=NULLStr, const String& c=NULLStr,
                           const String& d=NULLStr, const String& e=NULLStr, const String& f=NULLStr,
                           const String& g=NULLStr, const String& h=NULLStr) const override;
  virtual bool          CheckEqualsError(String& condition, bool quiet, bool& rval);
  // check for common mistake of using = instead of == for logical equals
  virtual bool          CheckProgVarRef(ProgVarRef& pvr, bool quiet, bool& rval);
  // check program variable reference to make sure it is in same Program scope as this progel
  virtual ProgVar*      FindVarNameInScope_impl(const String& var_nm) const;
    // #IGNORE impl

  virtual void          UpdateProgFlags();
  // #IGNORE update program element flags


  bool         CheckConfig_impl(bool quiet) override;
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
  void         SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
                                int sls, void* op1_, void* op2_) override;

  virtual bool          useDesc() const {return true;} // hack for CommentEl

  virtual void          PreGenMe_impl(int item_id) {}
  virtual void          PreGenChildren_impl(int& item_id) {}
  virtual void          GenCssPre_impl(Program* prog) {};
  // #IGNORE generate the Css prefix code (if any) for this object
  virtual void          GenCssBody_impl(Program* prog) {};
  // #IGNORE generate the Css body code for this object
  virtual void          GenCssPost_impl(Program* prog) {};
  // #IGNORE generate the Css postfix code (if any) for this object
  virtual const String  GenListing_children(int indent_level) {return _nilString;}
  // generate listing of any children of this progel

private:
  void  Copy_(const ProgEl& cp);
  void  Initialize();
  void  Destroy();
};

SMARTREF_OF(TA_API, ProgEl);

#endif // ProgEl_h
