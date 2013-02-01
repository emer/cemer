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

#ifndef ProgVar_h
#define ProgVar_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ProgVarRef>
#include <Program>
#include <DynEnum>

// declare all other types mentioned but not required to include:


TypeDef_Of(ProgVar);

class TA_API ProgVar: public taNBase {
  // ##INSTANCE ##INLINE #STEM_BASE ##SCOPE_Program ##CAT_Program a program variable, accessible from the outer system, and inside the script in .vars and args
INHERITED(taNBase)
public:
  enum VarType {
    T_Int,                      // #LABEL_Int integer
    T_Real,                     // #LABEL_Real real-valued number (double precision)
    T_String,                   // #LABEL_String string of characters
    T_Bool,                     // #LABEL_Bool boolean true/false
    T_Object,                   // #LABEL_Object* pointer to a C++ (hard coded) object -- this is not the object itself, just a pointer to it -- object must exist somewhere.  if it is in this program's .objs, then the name will be automatically set
    T_HardEnum,                 // #LABEL_Enum enumerated list of options (existing C++ hard-coded one)
    T_DynEnum,                  // #LABEL_DynEnum enumerated list of labeled options (from a dynamically created list)
    T_UnDef,                    // #LABEL_UnDef undefined -- automatically-created variables start with an undefined type -- must specify a valid type before using
  };

  enum VarFlags { // #BITS flags for modifying program variables
    PV_NONE             = 0, // #NO_BIT
    CTRL_PANEL          = 0x0001, // #CONDSHOW_OFF_flags:LOCAL_VAR show this variable in the control panel
    CTRL_READ_ONLY      = 0x0002, // #CONDSHOW_ON_flags:CTRL_PANEL variable is read only (display but not edit) in the control panel
    NULL_CHECK          = 0x0004, // #CONDSHOW_ON_var_type:T_Object complain if object variable is null during checkconfig (e.g., will get assigned during run)
    SAVE_VAL            = 0x0008, // #CONDSHOW_OFF_flags:LOCAL_VAR save the value of the variable in the project -- good idea to turn off saving for variables that don't require persistence across saving and loading (and that change for each run and thus might affect merging of project files)
    NEW_OBJ             = 0x0010, // #CONDSHOW_ON_var_type:T_Object&&flags:LOCAL_VAR automatically create a new object of given type when local variable is first initialized -- object will be automatically destroyed when this variable goes out of scope -- do not use if you are assigning this variable from a function return value -- only when you need a new temporary object of this type
    QUIET               = 0x0020, // turn off warning messages if they are not relevant (e.g., regarding global matrix vars)
    LOCAL_VAR           = 0x0040, // #NO_SHOW this is a local variable which does not set or update values!
    FUN_ARG             = 0x0080, // #NO_SHOW this is a function argument variable
    USED                = 0x0100, // #NO_SHOW whether this variable is currently being used in the program (set automatically)
    EDIT_VAL            = 0x0200, // #NO_SHOW allow value to be edited -- only if !LOCAL_VAR && !init_from
  };

  VarType       var_type;       // type of variable -- determines which xxx_val(s) is/are used
  int           int_val;        // #CONDSHOW_ON_var_type:T_Int,T_HardEnum #CONDEDIT_ON_flags:EDIT_VAL integer value -- this is the current actual value of the variable at all times for global variables, and is used as an initialization value for local variables (they start with this value, but what you see here is NOT their current value as the program runs)
  double        real_val;       // #CONDSHOW_ON_var_type:T_Real #CONDEDIT_ON_flags:EDIT_VAL real value -- this is the current actual value of the variable at all times for global variables, and is used as an initialization value for local variables (they start with this value, but what you see here is NOT their current value as the program runs)
  String        string_val;     // #CONDSHOW_ON_var_type:T_String #CONDEDIT_ON_flags:EDIT_VAL #EDIT_DIALOG string value -- this is the current actual value of the variable at all times for global variables, and is used as an initialization value for local variables (they start with this value, but what you see here is NOT their current value as the program runs)
  bool          bool_val;       // #CONDSHOW_ON_var_type:T_Bool #CONDEDIT_ON_flags:EDIT_VAL boolean value -- this is the current actual value of the variable at all times for global variables, and is used as an initialization value for local variables (they start with this value, but what you see here is NOT their current value as the program runs)
  TypeDef*      object_type;    // #CONDSHOW_ON_var_type:T_Object #NO_NULL #TYPE_taBase #LABEL_min_type the minimum acceptable type of the object
  taBaseRef     object_val;     // #CONDSHOW_ON_var_type:T_Object #CONDEDIT_ON_flags:EDIT_VAL #TYPE_ON_object_type #SCOPE_taProject object pointer value -- this is not the object itself, just a pointer to it -- object must exist somewhere.  if it is in this program's .objs, then the name will be automatically set -- this is the current actual value of the variable at all times for global variables, and is used as an initialization value for local variables (they start with this value, but what you see here is NOT their current value as the program runs)
  TypeDef*      hard_enum_type; // #CONDSHOW_ON_var_type:T_HardEnum #ENUM_TYPE #TYPE_taBase #LABEL_enum_type type information for hard enum (value goes in int_val)
  DynEnum       dyn_enum_val;   // #CONDSHOW_ON_var_type:T_DynEnum #LABEL_enum_val dynamic enum value -- this is the current actual value of the variable at all times for global variables, and is used as an initialization value for local variables (they start with this value, but what you see here is NOT their current value as the program runs)
  bool          objs_ptr;       // #HIDDEN this is a pointer to a variable in the objs list of a program
  VarFlags      flags;          // flags controlling various things about how the variable appears and is used
  bool          reference;      // #CONDSHOW_ON_flags:FUN_ARG make this a reference variable (only for function arguments) which allows the function to modify the argument value, making it in effect a return value from the function when you need multiple return values
  String        desc;           // #EDIT_DIALOG Description of what this variable is for
  ProgramRef    init_from;      // #CONDSHOW_OFF_flags:LOCAL_VAR initialize this variable from one with the same name in another program -- value is initialized at the start of the Init and Run functions -- useful to maintain a set of global parameter variables that are used in various sub programs
  int           css_idx;        // #IGNORE index within script->prog_vars for this variable, valid once it has been created -- used for updating prog var when gui value changes

  cssEl*        parse_css_el;   // #IGNORE css el for parsing

  bool                  schemaChanged(); // true if schema for most recent change differed from prev change
  void                  Cleanup(); // #IGNORE we call this after changing value, to cleanup unused
  Program*              program() {return GET_MY_OWNER(Program);}

  virtual const String  GenCssType() const; // type name
  virtual const String  GenCssInitVal() const; // intial value

  virtual const String  GenCss(bool is_arg = false); // css code (terminated if Var);
  virtual const String  GenListing(bool is_arg = false, int indent_level = 0); // generate listing of program
  virtual void          GenCssInitFrom(Program* prog);
  // generate css code to initialize from other variable
  virtual Program*      GetInitFromProg(); // get the init_from program for use in program css code -- emits warning if NULL (shouldn't happen)

  virtual cssEl*        NewCssEl();
  // #IGNORE get a new cssEl of an appropriate type, name/value initialized
  virtual void          SetParseCssEl();
  // #IGNORE set parse_css_el to NewCssEl() if NULL
  virtual void          FreeParseCssEl();
  // #IGNORE free parse_css_el
  virtual void          ResetParseStuff();
  // #IGNORE reset all parsing stuff

  virtual void  SetInt(int val); // set variable type to INT and set value
  virtual void  SetReal(double val);  // set variable type to REAL and set value
  virtual void  SetString(const String& val);  // set variable type to STRING and set value
  virtual void  SetBool(bool val);  // set variable type to BOOL and set value
  virtual void  SetObject(taBase* val); // #DROP1 set variable type to OBJECT and set value
  virtual void  SetObjectType(TypeDef* obj_typ); // #DROP1 set variable type to OBJECT and set object_type to given value
  virtual void  SetHardEnum(TypeDef* enum_type, int val); // set variable type to HARD_ENUM and set value
  virtual void  SetDynEnum(int val);  // set variable type to DYN_ENUM and set value
  virtual void  SetDynEnumName(const String& val); //  // set variable type to DYN_ENUM and set value

  virtual void  SetVar(const Variant& value);
  // set from variant value (general purpose variable setting) -- does not change type of variable, just sets from variant value
  virtual Variant GetVar();
  // get value as a variant value -- for hard-code use of the variable value

  ProgVar* operator=(const Variant& value);

  override void GetSelectText(MemberDef* mbr, String xtra_lbl,
    String& full_lbl, String& desc) const;
  override String GetDesc() const { return desc; }
  override String GetDisplayName() const;
  override String GetTypeDecoKey() const { return "ProgVar"; }

  virtual TypeDef*      act_object_type() const; // #IGNORE the actual object type; never NULL (taBase min)
  virtual MemberDef*    GetValMemberDef();
  // #IGNORE get member def that represents the value for this type of variable

  inline void           SetVarFlag(VarFlags flg)   { flags = (VarFlags)(flags | flg); }
  // set flag state on
  inline void           ClearVarFlag(VarFlags flg) { flags = (VarFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasVarFlag(VarFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetVarFlagState(VarFlags flg, bool on)
  { if(on) SetVarFlag(flg); else ClearVarFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline void   CtrlPanel()     { SetVarFlag(CTRL_PANEL); }
  // #MENU #DYN1 #CAT_Display put this variable in the control panel
  inline void   NoCtrlPanel()   { ClearVarFlag(CTRL_PANEL); }
  // #MENU #DYN1 #CAT_Display take this variable out of the control panel
  inline void   CtrlReadOnly()  { SetVarFlag(CTRL_READ_ONLY); }
  // #MENU #DYN1 #CAT_Display make this variable read only in the control panel
  inline void   NoCtrlReadOnly() { ClearVarFlag(CTRL_READ_ONLY); }
  // #MENU #DYN1 #CAT_Display make this variable editable in the control panel

  virtual void          SetFlagsByOwnership();
  // #IGNORE auto-set the LOCAL_VAR and FUN_ARG flags based on my owners
  virtual bool          UpdateUsedFlag();
  // #IGNORE update the USED flag based on siglink refs
  virtual bool          UpdateCssObjVal();
  // #IGNORE update cssSmartRef if we are a non-local object pointer

  virtual ProgVar*      GetInitFromVar(bool warn = true);
  // get the program variable to initialize from in the init_from program -- warn = emit a warning if the variable is not found

  override int          GetEnabled() const;
  override int          GetSpecialState() const;
  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); // don't save the unused vals
  override void         SigEmit(int dcr, void* op1 = NULL, void* op2 = NULL);
  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ProgVar);
protected:
  String                m_this_sig; // the sig from most recent change
  String                m_prev_sig; // the sig last time it changed

  virtual bool          CheckUndefType(const String& function_context) const;
  // #IGNORE check if var_type == T_UnDef and emit a warning if so -- returns true if undefined..
  override void         UpdateAfterEdit_impl();
  virtual String        GetSchemaSig() const;
  // #IGNORE make a string that is the schema signature of obj; as long as schema stays the same, we don't stale on changes (ex, to value)
  override void         CheckThisConfig_impl(bool quiet, bool& rval);
  override void         CheckChildConfig_impl(bool quiet, bool& rval); //object, if any
  virtual const String  GenCssArg_impl();
  virtual const String  GenCssVar_impl();
private:
  void  Copy_(const ProgVar& cp);
  void  Initialize();
  void  Destroy();
};

#endif // ProgVar_h
