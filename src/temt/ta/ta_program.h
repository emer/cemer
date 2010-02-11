// Carnegie Mellon University, Princeton University.
// Copyright, 1995-2007, Regents of the University of Colorado,
//
// This file is part of The Emergent Toolkit
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

// ta_program.h -- gui-based executables

#ifndef TA_PROGRAM_H
#define TA_PROGRAM_H

#include "ta_group.h"
#include "ta_script.h"
#include "ta_viewer.h"

#include "ta_def.h"
#include "ta_TA_type.h"

class cssElPtr;		// #IGNORE
class cssSpace;		// #IGNORE

// external refs
class DataTable; //

// forwards
class DynEnumType;
class Program;
TA_SMART_PTRS(Program); // ProgramRef
class Program_Group;
class Program_List;
class ProgLibEl;
class ProgLib;
class iProgramPanel;
class Function;
class ProgramCallBase;
class ProgramCall;
class ProgramCallVar; //

/////////////////////////////////////////////////////////////////////
//		IMPORTANT CODING NOTES:

// you must use PROGEL_SIMPLE_BASEFUNS macro instead of TA_SIMPLE_BASEFUNS
// for all ProgEl types that contain a ProgVarRef or a ProgExpr
// or, use PROGEL_SIMPLE_COPY or directly call UpdateAfterCopy if defining
// custom copy functions.

// Note: ProgEl now automatically does update stuff in UpdateAfterMove_impl
// and does CheckProgVarRef in CheckThisConfig, so these calls are not necc

///////////////////////////////////////////////////////////
//		Program Types
///////////////////////////////////////////////////////////

class TA_API ProgType: public taNBase {
  // #STEM_BASE #NO_INSTANCE #VIRT_BASE ##EDIT_INLINE ##SCOPE_Program ##CAT_Program a program type -- base class for defining new types within a program
INHERITED(taNBase)
public:
  String	desc;	// #EDIT_DIALOG Description of this type
  
  virtual const String	GenCssType() const; // type name
  virtual const String	GenCss(int indent_level); // generate css code
  virtual const String	GenListing(int indent_level); // generate listing of program
  
  virtual taBase* FindTypeName(const String& nm) const;
  // find given type name (e.g., dynamic enum type or value) on variable

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  override String 	GetDesc() const { return desc; }
  override String 	GetTypeDecoKey() const { return "ProgType"; }
  override void   	SetDefaultName() {} // make it local to list, set by list
  TA_SIMPLE_BASEFUNS(ProgType);
protected:
  override void 	UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  virtual const String	GenCssPre_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css prefix code (if any) for this object	
  virtual const String	GenCssBody_impl(int indent_level) { return _nilString; } // #IGNORE generate the Css body code for this object
  virtual const String	GenCssPost_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css postfix code (if any) for this object
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ProgType_List : public taList<ProgType> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of script variables
INHERITED(taList<ProgType>)
public:
  virtual const String 	GenCss(int indent_level) const; // generate css script code for the context
  virtual const String 	GenListing(int indent_level) const; // generate the listing of program

  virtual DynEnumType* NewDynEnum();
  // #BUTTON #MENU_CONTEXT create a new DynEnumType (shortcut)

  virtual taBase* FindTypeName(const String& nm) const;
  // find given type name (e.g., dynamic enum type or value) on list
  
  override String GetTypeDecoKey() const { return "ProgType"; }

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  void	setStale();
  TA_BASEFUNS_NOCOPY(ProgType_List);
protected:
  override void	El_SetIndex_(void*, int);
  
private:
  void	Initialize();
  void	Destroy() {Reset();}
};

SmartRef_Of(ProgType,TA_ProgType); // ProgTypeRef


///////////////////////////////////////////////////////////
//		DynEnum type
///////////////////////////////////////////////////////////

class TA_API DynEnumItem : public taNBase {
  // ##EDIT_INLINE ##CAT_Program ##SCOPE_Program dynamic enumerated type value (name and numerical int value)
INHERITED(taNBase)
public:
  int		value;		// numerical (integer) value of this enum
  String	desc;		// #EDIT_DIALOG description of item

  override String	GetDisplayName() const;
  override String 	GetDesc() const { return desc; }

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  inline void 	Initialize() 			{ value = 0; }
  inline void 	Destroy()			{ };
  inline void 	Copy_(const DynEnumItem& cp)	{ value = cp.value; desc = cp.desc; }
  TA_BASEFUNS(DynEnumItem);
protected:
  override void 	UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
};

class TA_API DynEnumItem_List : public taList<DynEnumItem> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program ##SCOPE_Program list of dynamic enumerated type items
INHERITED(taList<DynEnumItem>)
public:

  virtual int	FindNumIdx(int val) const; // find index of given numerical value

  virtual void	OrderItems();
  // ensure that the item values are sequentially increasing

  override void DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);

  override int	El_Compare_(void* a, void* b) const
  { int rval=-1; if(((DynEnumItem*)a)->value > ((DynEnumItem*)b)->value) rval=1;
    else if(((DynEnumItem*)a)->value == ((DynEnumItem*)b)->value) rval=0; return rval; }

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  TA_BASEFUNS_NOCOPY(DynEnumItem_List);
private:
  void	Initialize();
  void	Destroy() {Reset();}
};

class TA_API DynEnumType : public ProgType {
  // #NO_UPDATE_AFTER ##DEF_CHILD_enums ##CAT_Program ##SCOPE_Program dynamic enumerated type -- user-definable list of labeled values that make code easier to read and write
INHERITED(ProgType)
public:
  DynEnumItem_List	enums;	// enumerated values for this type
  bool			bits;	// each item represents a separate bit value, which can be orthogonally set from each other, instead of mutually exclusive alternatives

  virtual DynEnumItem*	NewEnum();
  // #BUTTON create a new enum item
  virtual DynEnumItem*  AddEnum(const String& nm, int val);
  // add a new enum item with given name/label and value
  virtual void		SeqNumberItems(int first_val = 0);
  // #BUTTON assign values to items sequentially, starting with given first value
  virtual bool		CopyToAllProgs();
  // #BUTTON #CONFIRM copy this type information to all programs that have an enum with this same name in their types section -- provides a convenient way to update when multiple programs use the same dynamic enum types
  
  virtual int	FindNumIdx(int val) const { return enums.FindNumIdx(val); }
  // find index of given numerical value
  virtual int	FindNameIdx(const String& nm) const { return enums.FindNameIdx(nm); }
  // find index of given name value

  virtual ostream& OutputType(ostream& fh) const;
  // output type information in C++ syntax

  override taList_impl*	children_() {return &enums;}	
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return enums.GetTA_Element(i, eltd); }

  override void DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);

  override taBase*	FindTypeName(const String& nm) const;
  override String	GetDisplayName() const;

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  TA_SIMPLE_BASEFUNS(DynEnumType);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level);
  override const String	GenCssBody_impl(int indent_level);
  override const String	GenCssPost_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{}
};

SmartRef_Of(DynEnumType,TA_DynEnumType); // DynEnumTypeRef

class TA_API DynEnum : public taOBase {
  // #STEM_BASE #NO_TOKENS #NO_UPDATE_AFTER ##EDIT_INLINE ##CAT_Program ##SCOPE_Program dynamic enumerated value -- represents one item from a list of enumerated alternative labeled values
INHERITED(taOBase)
public:
  DynEnumTypeRef	enum_type; // enum type information (list of enum labels)
  int			value;     // #DYNENUM_ON_enum_type current value, which for normal mutually-exclusive options is index into list of enums (-1 = not set), and for bits is the bit values

  virtual bool	IsSet() const
  { return ((bool)enum_type && (value >= 0)); }
  // check whether there is a value set (enum_type is set and value >= 0)
  virtual int 	NumVal() const;
  // current numerical (integer) value of enum (-1 = no value set)
  virtual const String NameVal() const;
  // current name (string) value of enum ("" = no value set)

  virtual bool	SetNumVal(int val);
  // set current enum value by numerical value (for bits mode, literally set value); false (and error msg) if not found
  virtual bool	SetNameVal(const String& nm);
  // set current enum value by name (for bits mode, set bit for name); false (and error msg) if not found
  virtual bool	ClearBitName(const String& val);
  // only for bits type, clear bit with given name

  override String	GetDisplayName() const;//
/*TEMP  override String	GetValStr(void* par = NULL, MemberDef* md = NULL,
				  TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
				  bool force_inline = false) const;*/

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(DynEnum, Program);
protected:
  override void CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void	Initialize();
  void	Destroy();
};

///////////////////////////////////////////////////////////
//		Program Variables
///////////////////////////////////////////////////////////

class TA_API ProgVar: public taOBase {
  // ##INSTANCE ##INLINE #STEM_BASE ##SCOPE_Program ##CAT_Program a program variable, accessible from the outer system, and inside the script in .vars and args
INHERITED(taOBase)
public:
  enum VarType {
    T_Int,			// #LABEL_Int integer
    T_Real,			// #LABEL_Real real-valued number (double precision)
    T_String,			// #LABEL_String string of characters
    T_Bool,			// #LABEL_Bool boolean true/false 
    T_Object,			// #LABEL_Object* pointer to a C++ (hard coded) object -- this is not the object itself, just a pointer to it -- object must exist somewhere.  if it is in this program's .objs, then the name will be automatically set
    T_HardEnum,			// #LABEL_Enum enumerated list of options (existing C++ hard-coded one)
    T_DynEnum,			// #LABEL_DynEnum enumerated list of labeled options (from a dynamically created list)
  };

  enum VarFlags { // #BITS flags for modifying program variables
    PV_NONE		= 0, // #NO_BIT
    CTRL_PANEL		= 0x0001, // #CONDSHOW_OFF_flags:LOCAL_VAR show this variable in the control panel
    CTRL_READ_ONLY      = 0x0002, // #CONDSHOW_ON_flags:CTRL_PANEL variable is read only (display but not edit) in the control panel
    NULL_CHECK		= 0x0004, // #CONDSHOW_ON_var_type:T_Object complain if object variable is null during checkconfig (e.g., will get assigned during run)
    LOCAL_VAR		= 0x0008, // #NO_SHOW this is a local variable which does not set or update values!
    FUN_ARG		= 0x0010, // #NO_SHOW this is a function argument variable
    USED		= 0x0020, // #NO_SHOW whether this variable is currently being used in the program (set automatically)
    EDIT_VAL		= 0x0040, // #NO_SHOW allow value to be edited -- only if !LOCAL_VAR && !init_from
  };

  String	name;		// name of the variable
  VarType	var_type;	// type of variable -- determines which xxx_val(s) is/are used
  int		int_val;	// #CONDSHOW_ON_var_type:T_Int,T_HardEnum #CONDEDIT_ON_flags:EDIT_VAL integer value (also for enum types)
  double	real_val;	// #CONDSHOW_ON_var_type:T_Real #CONDEDIT_ON_flags:EDIT_VAL real value
  String	string_val;	// #CONDSHOW_ON_var_type:T_String #CONDEDIT_ON_flags:EDIT_VAL #EDIT_DIALOG string value
  bool		bool_val;	// #CONDSHOW_ON_var_type:T_Bool #CONDEDIT_ON_flags:EDIT_VAL boolean value
  TypeDef*	object_type; 	// #CONDSHOW_ON_var_type:T_Object #NO_NULL #TYPE_taBase #LABEL_min_type the minimum acceptable type of the object
  taBaseRef	object_val;	// #CONDSHOW_ON_var_type:T_Object #CONDEDIT_ON_flags:EDIT_VAL #TYPE_ON_object_type #SCOPE_taProject object pointer value -- this is not the object itself, just a pointer to it -- object must exist somewhere.  if it is in this program's .objs, then the name will be automatically set
  TypeDef*	hard_enum_type;	// #CONDSHOW_ON_var_type:T_HardEnum #ENUM_TYPE #TYPE_taBase #LABEL_enum_type type information for hard enum (value goes in int_val)
  DynEnum 	dyn_enum_val; 	// #CONDSHOW_ON_var_type:T_DynEnum #LABEL_enum_val dynamic enum value
  bool		objs_ptr;	// #HIDDEN this is a pointer to a variable in the objs list of a program
  VarFlags	flags;		// flags controlling various things about how the variable appears and is used
  bool		reference;	// #CONDSHOW_ON_flags:FUN_ARG make this a reference variable (only for function arguments) which allows the function to modify the argument value, making it in effect a return value from the function when you need multiple return values
  String	desc;		// #EDIT_DIALOG Description of what this variable is for
  ProgramRef	init_from;	// #CONDSHOW_OFF_flags:LOCAL_VAR initialize this variable from one with the same name in another program -- value is initialized at the start of the Init and Run functions -- useful to maintain a set of global parameter variables that are used in various sub programs

  cssEl*	parse_css_el;	// #IGNORE css el for parsing
  
  bool			schemaChanged(); // true if schema for most recent change differed from prev change
  void			Cleanup(); // #IGNORE we call this after changing value, to cleanup unused
  Program*		program() {return GET_MY_OWNER(Program);} 

  virtual const String	GenCssType() const; // type name
  virtual const String	GenCssInitVal() const; // intial value

  virtual const String	GenCss(bool is_arg = false); // css code (terminated if Var);
  virtual const String	GenListing(bool is_arg = false, int indent_level = 0); // generate listing of program
  virtual const String	GenCssInitFrom(int indent_level);
  // generate css code to initialize from other variable
  virtual Program*	GetInitFromProg(); // get the init_from program for use in program css code -- emits warning if NULL (shouldn't happen)
  
  virtual cssEl*	NewCssEl();
  // #IGNORE get a new cssEl of an appropriate type, name/value initialized
  virtual void		SetParseCssEl();
  // #IGNORE set parse_css_el to NewCssEl() if NULL
  virtual void		FreeParseCssEl();
  // #IGNORE free parse_css_el
  virtual void		ResetParseStuff();
  // #IGNORE reset all parsing stuff

  virtual void	SetInt(int val); // set variable type to INT and set value
  virtual void	SetReal(double val);  // set variable type to REAL and set value
  virtual void	SetString(const String& val);  // set variable type to STRING and set value
  virtual void	SetBool(bool val);  // set variable type to BOOL and set value
  virtual void	SetObject(taBase* val); // #DROP1 set variable type to OBJECT and set value
  virtual void	SetHardEnum(TypeDef* enum_type, int val); // set variable type to HARD_ENUM and set value
  virtual void	SetDynEnum(int val);  // set variable type to DYN_ENUM and set value
  virtual void	SetDynEnumName(const String& val); //  // set variable type to DYN_ENUM and set value

  virtual void	SetVar(const Variant& value);
  // set from variant value (general purpose variable setting) -- does not change type of variable, just sets from variant value
  virtual Variant GetVar();
  // get value as a variant value -- for hard-code use of the variable value

  ProgVar* operator=(const Variant& value);
 
  override void GetSelectText(MemberDef* mbr, String xtra_lbl,
    String& full_lbl, String& desc) const;
  bool 		SetName(const String& nm)    	{ name = nm; return true; }
  String	GetName() const			{ return name; }
  override String GetDesc() const { return desc; }
  override String GetDisplayName() const;
  override String GetTypeDecoKey() const { return "ProgVar"; }

  virtual TypeDef*	act_object_type() const; // #IGNORE the actual object type; never NULL (taBase min)
  virtual MemberDef* 	GetValMemberDef();
  // #IGNORE get member def that represents the value for this type of variable
  
  inline void		SetVarFlag(VarFlags flg)   { flags = (VarFlags)(flags | flg); }
  // set flag state on
  inline void		ClearVarFlag(VarFlags flg) { flags = (VarFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool		HasVarFlag(VarFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void		SetVarFlagState(VarFlags flg, bool on)
  { if(on) SetVarFlag(flg); else ClearVarFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline void	CtrlPanel() 	{ SetVarFlag(CTRL_PANEL); }
  // #MENU #DYN1 #CAT_Display put this variable in the control panel
  inline void	NoCtrlPanel() 	{ ClearVarFlag(CTRL_PANEL); }
  // #MENU #DYN1 #CAT_Display take this variable out of the control panel
  inline void	CtrlReadOnly() 	{ SetVarFlag(CTRL_READ_ONLY); }
  // #MENU #DYN1 #CAT_Display make this variable read only in the control panel
  inline void	NoCtrlReadOnly() { ClearVarFlag(CTRL_READ_ONLY); }
  // #MENU #DYN1 #CAT_Display make this variable editable in the control panel

  virtual void		SetFlagsByOwnership();
  // #IGNORE auto-set the LOCAL_VAR and FUN_ARG flags based on my owners
  virtual bool		UpdateUsedFlag();
  // #IGNORE update the USED flag based on datalink refs

  virtual ProgVar*	GetInitFromVar(bool warn = true);
  // get the program variable to initialize from in the init_from program -- warn = emit a warning if the variable is not found

  override int		GetEnabled() const;
  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); // don't save the unused vals
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  override void 	SetDefaultName() {} // make it local to list, set by list
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(ProgVar);
protected:
  String		m_this_sig; // the sig from most recent change
  String		m_prev_sig; // the sig last time it changed
  
  override void 	UpdateAfterEdit_impl();
  virtual String	GetSchemaSig() const; // #IGNORE make a string that is the schema signature of obj; as long as schema stays the same, we don't stale on changes (ex, to value)
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void 	CheckChildConfig_impl(bool quiet, bool& rval); //object, if any
  virtual const String	GenCssArg_impl();
  virtual const String	GenCssVar_impl();
private:
  void	Copy_(const ProgVar& cp);
  void	Initialize();
  void	Destroy();
};

class TA_API ProgVar_List : public taList<ProgVar> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of script variables
INHERITED(taList<ProgVar>)
public:
  enum VarContext {
    VC_ProgVars,  // #LABEL_ProgramVariables program variables
    VC_FuncArgs  //  #LABEL_FunctionArguments function arguments
  };
  
  VarContext	var_context; // #DEF_VC_ProgVars #HIDDEN #NO_SAVE context of vars, set by owner
  
  virtual const String 	GenCss(int indent_level) const; // generate css script code for the context
  virtual const String	GenCssInitFrom(int indent_level) const;
  // init_from code for all vars in list
  virtual const String 	GenListing(int indent_level) const; // generate listing of program

  virtual void	AddVarTo(taNBase* src);
  // #DROPN add a var to the given object
  virtual void	CreateDataColVars(DataTable* src);
  // #DROP1 #BUTTON create column variables for given database object (only for scalar vals -- not matrix ones)

  virtual ProgVar* FindVarType(ProgVar::VarType vart, TypeDef* td = NULL);
  // find first variable of given type (if hard enum or object type, td specifies type of object to find if not null)

  override String GetTypeDecoKey() const { return "ProgVar"; }

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  void	setStale();
  TA_BASEFUNS(ProgVar_List);
  
protected:
  override void	El_SetIndex_(void*, int);
  
private:
  void	Copy_(const ProgVar_List& cp);
  void	Initialize();
  void	Destroy() {Reset();}
};

SmartRef_Of(ProgVar,TA_ProgVar); // ProgVarRef

class TA_API ProgVarRef_List: public taPtrList<ProgVarRef> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of program variable references
INHERITED(taList<ProgVarRef>)
public:
  ProgVarRef*	FindVar(ProgVar* var, int& idx) const;
  // return ref pointing to given var pointer (NULL if not found)
  ProgVarRef*	FindVarName(const String& var_nm, int& idx) const;
  // return ref pointing to given var name (NULL if not found)

  virtual int	UpdatePointers_NewPar(taBase* lst_own, taBase* old_par, taBase* new_par);
  // lst_own is the owner of this list
  virtual int	UpdatePointers_NewParType(taBase* lst_own, TypeDef* par_typ, taBase* new_par);
  // lst_own is the owner of this list
  virtual int	UpdatePointers_NewObj(taBase* lst_own, taBase* ptr_owner, taBase* old_ptr, taBase* new_ptr);
  // lst_own is the owner of this list

  void		operator=(const ProgVarRef_List& cp) { Reset(); Duplicate(cp); }

  ProgVarRef_List() {Initialize();}
  ProgVarRef_List(const ProgVarRef_List& cp) { Reset(); Duplicate(cp); }
  ~ProgVarRef_List();
protected:
  String	El_GetName_(void* it) const { if(((ProgVarRef*)it)->ptr()) return ((ProgVarRef*)it)->ptr()->GetName(); return _nilString; }
  void		El_Done_(void* it)	{ delete (ProgVarRef*)it; }
  void*		El_MakeToken_(void* it) { return new ProgVarRef; }
  void*		El_Copy_(void* trg, void* src)
  { *((ProgVarRef*)trg) = *((ProgVarRef*)src); return trg; }

private:
  void Initialize();
};

class TA_API ProgExprBase : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Program an expression in a program -- manages variable references so they are always updated when program variables change -- base doesn't have any lookup functionality
INHERITED(taOBase)
public:
  enum ExprFlags { // #BITS flags for program expression setting
    PE_NONE		= 0, // #NO_BIT
    NO_VAR_ERRS		= 0x0001, // do not generate error messages for variables that cannot be found (e.g., for more complex expressions that might create local variables)
    FULL_STMT		= 0x0002, // expression is full css statement(s), not just isolated expressions -- this affects how the parsing works
    FOR_LOOP_EXPR	= 0x0004, // expression is an initializer or increment for a for loop -- requires different parsing due to possibility of commas..
  };

  String	expr;		// #EDIT_DIALOG #EDIT_WIDTH_40 #LABEL_ enter the expression here -- use Ctrl-L to pull up a lookup dialog for members, methods, types, etc -- or you can just type in names of program variables or literal values.  enclose strings in double quotes.  variable names will be checked and automatically updated

  ExprFlags	flags;		// #HIDDEN #NO_SAVE Flags for controlling expression behavior -- should not be saved because they are set by the owning program every time
  String	var_expr;	// #READ_ONLY #HIDDEN #NO_SAVE expression with variables listed as $#1#$, etc. used for generating the actual code (this is the 'official' version that generates the full expr)

  ProgVarRef_List vars;		// #READ_ONLY #HIDDEN #NO_SAVE list of program variables that appear in the expression
  String_Array	var_names;	// #READ_ONLY #HIDDEN #NO_SAVE original variable names associated with vars list -- useful for user info if a variable goes out of existence..
  String_Array	bad_vars;	// #READ_ONLY #HIDDEN #NO_SAVE list of variable names that are not found in the expression (may be fine if declared locally elsewhere, or somewhere hidden -- just potentially bad)

  static cssProgSpace	parse_prog; // #IGNORE program space for parsing
  static cssSpace	parse_tmp;  // #IGNORE temporary el's created during parsing (for types)
  int			parse_ve_off; // #IGNORE offset to position information (for expressions = 10, otherwise 0)
  int			parse_ve_pos; // #IGNORE position within expr during parsing for copying to var_expr

  bool		empty() const {return expr.empty();} 
    // #IGNORE quicky test for whether has anything or not, without needing to render
  bool		nonempty() const {return expr.nonempty();} 
    // #IGNORE quicky test for whether has anything or not, without needing to render
    
  virtual bool	SetExpr(const String& ex);
  // set to use given expression -- calls ParseExpr followed by UpdateAfterEdit_impl

  static int 	cssExtParseFun_pre(void* udata, const char* nm, cssElPtr& el_ptr);
  // external parsing function for css: pre for initial parsing
  static int 	cssExtParseFun_post(void* udata, const char* nm, cssElPtr& el_ptr);
  // external parsing function for css: post if nothing else gets it (bad var)

  virtual bool	ParseExpr();
  // parse the current expr for variables and update vars and var_expr accordingly (returns false if there are some bad_vars)
  virtual String GetFullExpr() const;
  // get full expression with variable names substituted appropriately

  inline void		SetExprFlag(ExprFlags flg)   { flags = (ExprFlags)(flags | flg); }
  // set flag state on
  inline void		ClearExprFlag(ExprFlags flg) { flags = (ExprFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool		HasExprFlag(ExprFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void		SetExprFlagState(ExprFlags flg, bool on)
  { if(on) SetExprFlag(flg); else ClearExprFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  virtual void	UpdateProgExpr_NewOwner();
  // update program expression after it has been moved/copied to a new owner -- this will identify any variables that are not present in the new program and copy them from the old owner -- must be called before messing with any of the vars progvarref's pointers (should be a copy/same as prior ones)

  override int	UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  override int	UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  override int	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);

  override String GetDisplayName() const;
  override String GetName() const;
  override String GetTypeDecoKey() const { return "ProgExpr"; }

  static String	  ExprLookupFun(const String& cur_txt, int cur_pos, int& new_pos,
				taBase*& path_own_obj, TypeDef*& path_own_typ,
				MemberDef*& path_md, ProgExprBase* expr_base,
				Program* own_prg, Function* own_fun,
				taBase* path_base=NULL, TypeDef* path_base_typ=NULL);
  // generic lookup function for any kind of expression -- very powerful!  takes current text and position where the lookup function was called, and returns the new text filled in with whatever the user looked up, with a new cursor position (new_pos) -- if this is a path expression then path_own_typ is the type of object that owns the member path_md at the end of the path -- if path_md is NULL then path_own_typ is an object in a list or other container where member def is not relevant.  path_base is a base anchor point for paths if that is implied instead of needing to be fully contained within the expression (path_base_typ is type of that guy, esp needed if base is null) -- in this case only path expressions are allowed.

  override String StringFieldLookupFun(const String& cur_txt, int cur_pos,
				       const String& mbr_name, int& new_pos);

  static bool	ExprLookupVarFilter(void* base, void* var); // #IGNORE special filter used in ExprLookupFun

  void 	InitLinks();
  void 	CutLinks();
  TA_BASEFUNS(ProgExprBase);
protected:

  virtual void	ParseExpr_SkipPath(int& pos);
  // skip over a path expression

  override void	UpdateAfterEdit_impl();
  override void CheckThisConfig_impl(bool quiet, bool& rval);
  override void	SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  override void	SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
					     int dcr, void* op1_, void* op2_);

private:
  void	Copy_(const ProgExprBase& cp);
  void	Initialize();
  void	Destroy();
};

class TA_API ProgExpr : public ProgExprBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Program an expression in a program -- manages variable references so they are always updated when program variables change -- includes variable lookup functions
INHERITED(ProgExprBase)
public:
  static bool		StdProgVarFilter(void* base, void* var); // generic progvar filter -- excludes variables from functions if not itself in same function -- use this for most progvars in ITEM_FILTER comment directive

  ProgVar*	var_lookup;	// #NULL_OK #NO_SAVE #NO_EDIT #NO_UPDATE_POINTER #ITEM_FILTER_StdProgVarFilter lookup a program variable and add it to the current expression (this field then returns to empty/NULL)

  void 	CutLinks();
  TA_BASEFUNS_NOCOPY(ProgExpr);
protected:
  override void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ProgExpr_List : public taList<ProgExpr> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of program expressions
INHERITED(taList<ProgExpr>)
public:

  override String GetTypeDecoKey() const { return "ProgExpr"; }
  
  virtual void	UpdateProgExpr_NewOwner();
  // calls UpdateProgExpr_NewOwner() on all the prog expr's in the list

  TA_BASEFUNS_NOCOPY(ProgExpr_List);
protected:
  override void CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy() {Reset();}
};

//////////////////////////////////////////////////////////////////
//		Prog Arg
//////////////////////////////////////////////////////////////////

class TA_API ProgArg: public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Program a program or method argument
INHERITED(taOBase)
public:
  TypeDef*		arg_type; // #READ_ONLY typedef of the target arg, where available
  String                type; // #SHOW #READ_ONLY the type of the argument (automatically set from the target function)
  String                name; // #SHOW #READ_ONLY the name of the argument (automatically set from the target function)
  bool			required; // #SHOW #READ_ONLY if a value is required (i.e., it is not a default argument)
  String                def_val; // #SHOW #READ_ONLY for default arguments, what will get passed by default -- this is for reference only (leave expr blank for default)
  ProgExpr		expr; // the expression to compute and pass as the argument -- enter <no_arg> to specify a null or empty argument for program calls -- does not set this arg value

  virtual bool		UpdateFromVar(const ProgVar& cp); 
  // updates our type information given variable that we apply to -- returns true if any changes
  virtual bool		UpdateFromType(TypeDef* td); 
  // updates our type information from method typedef that we apply to -- returns true if any changes

  bool  	SetName(const String& nm) 	{ name = nm; return true; } 
  String 	GetName() const			{ return name; } 

  override String GetDisplayName() const;
  override String GetTypeDecoKey() const { return "ProgArg"; }

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  void 	InitLinks();
  void 	CutLinks();
  TA_BASEFUNS(ProgArg);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Copy_(const ProgArg& cp);
  void	Initialize();
  void	Destroy();
};


class TA_API ProgArg_List : public taList<ProgArg> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of arguments
INHERITED(taList<ProgArg>)
public:

  virtual bool	UpdateFromVarList(ProgVar_List& targ);
  // update our list of args based on target variable list -- returns true if updated
  virtual bool	UpdateFromMethod(MethodDef* md);
  // update our list of args based on method def arguments -- returns true if updated
  
  override String GetTypeDecoKey() const { return "ProgArg"; }
  virtual const String	GenCssBody_impl(int indent_level); 
  
  virtual void	UpdateProgExpr_NewOwner();
  // calls UpdateProgExpr_NewOwner() on all the prog expr's in the list

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  TA_BASEFUNS_NOCOPY(ProgArg_List);
protected:
  override void CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy() {Reset();}
};

#define PROGEL_SIMPLE_COPY(T) \
  void Copy_(const T& cp) {T::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp); \
    UpdateAfterCopy(cp); }

#define PROGEL_SIMPLE_BASEFUNS(T) \
  PROGEL_SIMPLE_COPY(T);  \
  SIMPLE_LINKS(T); \
  TA_BASEFUNS(T)

class TA_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE #STEM_BASE ##EDIT_INLINE ##SCOPE_Program ##CAT_Program base class for a program element
friend class ProgExprBase;
INHERITED(taOBase)
public:
  static bool		StdProgVarFilter(void* base, void* var); // generic progvar filter -- excludes variables from functions if not itself in same function -- use this for most progvars in ITEM_FILTER comment directive
  static bool		ObjProgVarFilter(void* base, void* var); // Object* progvar filter -- only shows Object* items -- use in ITEM_FILTER comment directive
  static bool		DataProgVarFilter(void* base, void* var); // data table* progvar filter -- only shows DataTable* items -- use in ITEM_FILTER comment directive
  static bool		DynEnumProgVarFilter(void* base, void* var); // DynEnum progvar filter -- only shows DynEnum items -- use in ITEM_FILTER comment directive

  enum ProgFlags { // #BITS flags for modifying program element function or other information
    PEF_NONE		= 0, // #NO_BIT
    OFF 		= 0x0001, // inactivated: does not generate code
    NON_STD 		= 0x0002, // non-standard: not part of the standard code for this program -- a special purpose modification (just for user information/highlighting)
    NEW_EL 		= 0x0004, // new element: this element was recently added to the program (just for user information/highlighting)
    VERBOSE		= 0x0008, // print informative message about the operation of this program element to std output (e.g., css console or during -nogui startup) -- useful for debugging and for logging key steps during startup
  };

  String		desc; // #EDIT_DIALOG #HIDDEN_INLINE optional brief description of element's function; included as comment in script
  ProgFlags		flags;	// flags for modifying program element function or providing information about the status of this program element

  virtual ProgEl*   	parent() const
    {return (ProgEl*)const_cast<ProgEl*>(this)->GetOwner(&TA_ProgEl);}
  Program*		program() {return GET_MY_OWNER(Program);} 
  
  void			PreGen(int& item_id); //recursive walk of items before code gen; each item bumps its id and calls subitems; esp. used to discover subprogs in order
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object (usually override _impl's)
  virtual const String	GenListing(int indent_level = 0); // generate a listing of the program

  inline void		SetProgFlag(ProgFlags flg)   { flags = (ProgFlags)(flags | flg); }
  // set flag state on
  inline void		ClearProgFlag(ProgFlags flg) { flags = (ProgFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool		HasProgFlag(ProgFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void		SetProgFlagState(ProgFlags flg, bool on)
  { if(on) SetProgFlag(flg); else ClearProgFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)
  inline void		ToggleProgFlag(ProgFlags flg) { SetProgFlagState(flg, !HasProgFlag(flg)); }
  // toggle program flag

  void			SetOffFlag(bool off);
  // set the OFF flag to given state: flag indicates whether code element should be run or not
  void			ToggleOffFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle the OFF flag to opposite of current state: flag indicates whether code element should be run or not
  void			SetNonStdFlag(bool non_std);
  // set non standard flag to given state: flag indicates that this is not part of the standard code for this program -- a special purpose modification (just for user information/highlighting)
  void			ToggleNonStdFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle non standard flag to opposite of current state: flag indicates that this is not part of the standard code for this program -- a special purpose modification (just for user information/highlighting)
  void			SetNewElFlag(bool new_el);
  // set new element flag to given state: flag indicates that this element was recently added to the program (just for user information/highlighting)
  void			ToggleNewElFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle new element flag to opposite of current state: flag indicates that this element was recently added to the program (just for user information/highlighting)
  void			SetVerboseFlag(bool new_el);
  // set verbose flag to given state: when this part of the program is run, an informational message will be printed out on the css Console -- very useful for debugging
  void			ToggleVerboseFlag();
  // #MENU #MENU_ON_Object #DYN1 toggle verbose flag to opposite of current state: when this part of the program is run, an informational message will be printed out on the css Console -- very useful for debugging

  virtual ProgVar*	FindVarName(const String& var_nm) const;
  // find given variable within this program element -- NULL if not found

  virtual void		SetProgExprFlags() { };
  // special temporary function to set flags for any ProgExpr objects -- needed for new css parsing and loading of old projects which saved these flags causes errors, so this fixes that.. todo: remove me after a few releases (introduced in 4.0.10)

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  override String 	GetStateDecoKey() const;
  override int		GetEnabled() const;
  // note: it is our own, plus disabled if parent is
  override void		SetEnabled(bool value);
  override String 	GetDesc() const {return desc;}
  override const String	GetToolTip(const KeyString& key) const;
  override String	GetColText(const KeyString& key, int itm_idx = -1) const;
  bool			IsVerbose() const { return HasProgFlag(VERBOSE); } 

  virtual String	GetToolbarName() const;
  // name of the program element as represented in the programming toolbar

  TA_BASEFUNS(ProgEl);

protected:
  override void		UpdateAfterMove_impl(taBase* old_owner);
  virtual void		UpdateAfterCopy(const ProgEl& cp);
  // uses type information to do a set of automatic updates of pointers (smart refs) after copy
  override void		CheckError_msg(const char* a, const char* b=0, const char* c=0,
				       const char* d=0, const char* e=0, const char* f=0,
				       const char* g=0, const char* h=0) const;
  virtual bool		CheckEqualsError(String& condition, bool quiet, bool& rval);
  // check for common mistake of using = instead of == for logical equals
  virtual bool		CheckProgVarRef(ProgVarRef& pvr, bool quiet, bool& rval);
  // check program variable reference to make sure it is in same Program scope as this progel
  virtual bool		UpdateProgVarRef_NewOwner(ProgVarRef& pvr);
  // if program variable reference is not in same Program scope as this progel (because progel was moved to a new program), then try to find the same progvar in new owner (by name), emit warning if not found -- auto called by UpdateAfterMove and UpdateAfterCopy

  override bool 	CheckConfig_impl(bool quiet);
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
					     int dcr, void* op1_, void* op2_);

  virtual bool		useDesc() const {return true;} // hack for CommentEl

  virtual void		PreGenMe_impl(int item_id) {}
  virtual void		PreGenChildren_impl(int& item_id) {}
  virtual const String	GenCssPre_impl(int indent_level) {return _nilString;}
  // #IGNORE generate the Css prefix code (if any) for this object	
  virtual const String	GenCssBody_impl(int indent_level) { return _nilString; }
  // #IGNORE generate the Css body code for this object
  virtual const String	GenCssPost_impl(int indent_level) {return _nilString;}
  // #IGNORE generate the Css postfix code (if any) for this object
  virtual const String	GenListing_children(int indent_level) {return _nilString;}
  // generate listing of any children of this progel

private:
  void	Copy_(const ProgEl& cp);
  void	Initialize();
  void	Destroy();
};


class TA_API ProgEl_List: public taList<ProgEl> {
  // #TREEFILT_ProgGp ##CAT_Program list of program elements: a block of code
INHERITED(taList<ProgEl>)
public:
  virtual void		PreGen(int& item_id); // iterates over all items
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object
  virtual const String	GenListing(int indent_level = 0); // generate the listing of this program
  
  virtual ProgVar*	FindVarName(const String& var_nm) const;
  // find given variable within this progel list -- NULL if not found

  override int		NumListCols() const {return 2;} 
  override const 	KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  SIMPLE_LINKS(ProgEl_List);
  TA_BASEFUNS(ProgEl_List);
private:
  void 	Copy_(const ProgEl_List& cp);
  void	Initialize();
  void	Destroy();
};


class TA_API Loop: public ProgEl { 
  // #VIRT_BASE base class for loops
INHERITED(ProgEl)
public:
  ProgEl_List		loop_code; // #SHOW_TREE the items to execute in the loop

 virtual ProgEl*	AddLoopCode(TypeDef* el_type)	{ return (ProgEl*)loop_code.New(1, el_type); }
  // #BUTTON #TYPE_ProgEl add a new loop code element

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; } //
//no  override taList_impl* children_() {return &loop_code;}

  SIMPLE_COPY(Loop);
  SIMPLE_LINKS(Loop);
  TA_ABSTRACT_BASEFUNS(Loop);

protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize() {}
  void	Destroy()	{CutLinks();}
};

class TA_API StaticMethodCall: public ProgEl { 
  // ##DEF_CHILD_meth_args call a static method (member function) on a type 
INHERITED(ProgEl)
public:
  ProgVarRef		result_var; // #ITEM_FILTER_StdProgVarFilter result variable (optional -- can be NULL)
  TypeDef*		min_type; // #NO_SHOW #NO_SAVE #TYPE_taBase minimum object type to choose from -- anchors selection of object_type (derived versions can set this)
  TypeDef*		object_type; // #TYPE_ON_min_type The object type to look for methods on
  MethodDef*		method; //  #TYPE_ON_object_type the method to call
  ProgArg_List		meth_args;
  // #SHOW_TREE arguments to be passed to the method

  override taList_impl*	children_() {return &meth_args;}	
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }
  override String	GetToolbarName() const { return "static()"; }

  PROGEL_SIMPLE_BASEFUNS(StaticMethodCall);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API Function: public ProgEl { 
  // a user-defined function that can be called within the program where it is defined -- must live in the functions of a Program, not in init_code or prog_code 
INHERITED(ProgEl)
public:
  String		name;
  // The function name
  ProgVar::VarType	return_type;
  // The return type for the function -- what kind of variable does it return
  TypeDef*		object_type; 	// #CONDSHOW_ON_return_type:T_Object #NO_NULL #TYPE_taBase for Object* return types, the type of object to return
  ProgVar_List		args;
  // The arguments to the function
  ProgEl_List	    	fun_code;
  // the function code (list of program elements)

  virtual void  UpdateCallerArgs();
  // #BUTTON #CAT_Code run UpdateArgs on all the function calls to me, and also display all these calls in the Find dialog (searching on this function's name) so you can make sure the args are correct for each call

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }
  override bool 	SetName(const String& nm)    	{ name = nm; return true; }
  override String	GetName() const			{ return name; }
  override String	GetToolbarName() const { return "fun def"; }

  override void		InitLinks();
  PROGEL_SIMPLE_COPY(Function);
  TA_BASEFUNS(Function);
protected:
  override void		UpdateAfterEdit_impl();
  override void		UpdateAfterCopy(const ProgEl& cp);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level);
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};

SmartRef_Of(Function,TA_Function);

class TA_API Function_List: public taList<Function> {
  // #TREEFILT_ProgGp list of functions defined within a program
INHERITED(taList<Function>)
public:
  virtual void		PreGen(int& item_id); // iterates over all items
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object
  virtual const String	GenListing(int indent_level = 0); // generate the listing of this program

  virtual ProgVar*	FindVarName(const String& var_nm) const;
  // find given variable within this progel list -- NULL if not found

  override int		NumListCols() const {return 2;} 
  override const 	KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;

  override bool		BrowserSelectMe();
  override bool		BrowserExpandAll();
  override bool		BrowserCollapseAll();

  SIMPLE_LINKS(Function_List);
  TA_BASEFUNS(Function_List);
private:
  void 	Copy_(const Function_List& cp);
  void	Initialize();
  void	Destroy();
};


class TA_API FunctionCall: public ProgEl { 
  // ##DEF_CHILD_fun_args call a function
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // #ITEM_FILTER_StdProgVarFilter where to store the result (return value) of the function (optional -- can be NULL)
  FunctionRef		fun;
  // the function to be called
  ProgArg_List		fun_args;
  // #SHOW_TREE arguments to the function: passed when called

  virtual void		UpdateArgs(); 
  // #BUTTON updates the argument list based on the function being called

  override taList_impl*	children_() {return &fun_args;}
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }
  override String	GetToolbarName() const { return "fun()"; }

  PROGEL_SIMPLE_BASEFUNS(FunctionCall);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 
private:
  void	Initialize();
  void	Destroy()	{}
};

//		End of Prog Els!
///////////////////////////////////////////////////////////////////

//Note: object operations per se don't affect Program::stale, but
// they will indirectly to the extent that adding/removing them
// causes a corresponding var to get created

class TA_API ProgObjList: public taBase_List {
  // ##CAT_Program A list of program objects (just a taBase list with proper update actions to update variables associated with objects)
INHERITED(taBase_List)
public:

  virtual DataTable* NewDataTable(int n_tables = 1);
  // #BUTTON make new DataTable object(s) for local use in Program

  override void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  virtual void	GetVarsForObjs();
  // automatically create variables for objects in parent program

  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_BASEFUNS_NOCOPY(ProgObjList);
protected:
  void*		El_Own_(void* it); // give anon objs a name

private:
  void Initialize() { SetBaseType(&TA_taNBase); } // need name for var, owner to stream!
  void Destroy() { }
};

class TA_API Program_List : public taList<Program> {
  // ##CAT_Program a list of programs
  INHERITED(taList<Program>)
public:
  
  override String 	GetTypeDecoKey() const { return "Program"; }
  TA_BASEFUNS_NOCOPY(Program_List);
private:
  void	Initialize();
  void 	Destroy()		{Reset(); }; //
}; //


class TA_API Program: public taNBase, public AbstractScriptBase {
  // #STEM_BASE ##TOKENS ##INSTANCE ##EXT_prog ##FILETYPE_Program ##CAT_Program a structured gui-buildable program that generates css script code to actually run
INHERITED(taNBase)
public:
  enum ProgFlags { // #BITS program flags
    PF_NONE		= 0, // #NO_BIT
    NO_STOP_STEP	= 0x0001, // #AKA_NO_STOP this program cannot be stopped by Stop or Step buttons -- set this flag for simple helper programs to prevent them from showing up in the step list of other programs
    SELF_STEP		= 0x0002, // #NO_BIT this program has a StopStepPoint program element within it, and thus it shows up within its own list of Step programs -- this flag is set automatically during Init
    STARTUP_RUN		= 0x0004, // run this prgram at startup (after project is fully loaded and everything else has been initialized) -- if multiple programs are so marked, they will be run in the order they appear in the browser (depth first)
  };
  
  enum ReturnVal { // system defined return values (<0 are for user defined)
    RV_OK	= 0, 	// program finished successfully
    RV_COMPILE_ERR, 	// script couldn't be compiled
    RV_CHECK_ERR,	// program or its dependencies failed CheckConfig
    RV_INIT_ERR, 	// initialization failed (note: user prog may use its own value)
    RV_RUNTIME_ERR,	// misc runtime error (ex, null pointer ref, etc.)
    RV_NO_PROGRAM, 	// no program was available to run
  };
   
  enum RunState { // current run state for this program
    DONE = 0, 	// there is no program running or stopped; any previous run completed
    INIT,	// program is running its init_code
    RUN,	// program is running its prog_code
    STOP,	// the program is stopped (note: NOT the same as "DONE") -- check stop_reason etc for more information
    NOT_INIT,	// init has not yet been run
  };

  enum StopReason {		// reason why the program was stopped
    SR_NONE,			// no stop reason set (initialized value)
    SR_USER_STOP,		// Stop button was pressed by user -- stop_msg is name of program where Stop() was hit
    SR_USER_ABORT,		// Abort button was pressed by user -- stop_msg is name of program where Abort() was hit
    SR_USER_INTR,		// Ctrl-c was pressed in the console to interrupt processing -- stop_msg is name of css program space that was top at the time
    SR_STEP_POINT,		// the program reached the stopping point associated with Step mode -- stop_msg has program name
    SR_BREAKPOINT,		// a css breakpoint was reached -- stop_msg has info
    SR_ERROR,			// some form of runtime error occurred -- stop_msg has text
  };

  enum ProgLibs {
    USER_LIB,			// user's personal library
    SYSTEM_LIB,			// local system library
    WEB_LIB,			// web-based library
    SEARCH_LIBS,		// search through the libraries (for loading)
  };

  static ProgLib* 	prog_lib; // #NO_SHOW_TREE #NO_SAVE library of available programs
  static String_Array	forbidden_names;
  // #NO_SAVE #READ_ONLY #HIDDEN names that should not be used for variables and other such things because they are already in use

  static RunState	GetGlobalRunState(); // gets the global run state, i.e. is ANY program running, stopped, etc.
  
  Program_Group*	prog_gp;
  // #NO_SHOW #READ_ONLY #NO_SAVE #NO_SET_POINTER our owning program group -- needed for control panel stuff

  RunState		run_state;
  // #READ_ONLY #NO_SAVE this program's run state
  static bool		stop_req;
  // #READ_ONLY #NO_SAVE a stop was requested by someone -- stop at next chance
  static StopReason	stop_reason;
  // #READ_ONLY #NO_SAVE reason for the stop request
  static String		stop_msg;
  // #READ_ONLY #NO_SAVE text message associated with stop reason (e.g., err msg, breakpoint info, etc)
  static bool		step_mode;
  // #READ_ONLY #NO_SAVE the program was run in step mode -- check for stepping
  static ProgramRef	cur_step_prog;
  // #READ_ONLY #NO_SAVE the current program to be single-stepped -- set by the Step call of the program that was last run
  static int		cur_step_n;
  // #READ_ONLY #NO_SAVE current number of steps to take -- set by the Step call of the program that was last run
  static int		cur_step_cnt;
  // #READ_ONLY #NO_SAVE current step count -- incremented until cur_step_n is reached

  String		short_nm;
  // short name for this program -- as brief as possible -- used for Step display info
  String		tags;
  // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this program -- should be listed in hierarchical order, with most important/general tags first, as this is how they will be sorted in the program library
  String		desc;
  // #EDIT_DIALOG #HIDDEN_INLINE description of what this program does and when it should be used (used for searching in prog_lib -- be thorough!)
  ProgFlags		flags;
  // control flags, for display and execution control
  ProgObjList		objs;
  // #TREEFILT_ProgGp create persistent objects of any type here that are needed for the program -- each object will automatically create an associated variable 
  ProgType_List		types;
  // user-defined types for this program (new enumerated types and class objects)
  ProgVar_List		args;
  // global variables that are parameters (arguments) for callers
  ProgVar_List		vars;
  // global variables accessible outside and inside script
  Function_List		functions;
  // function code (for defining subroutines): goes at top of script and can be called from init or prog code
  ProgEl_List		load_code;
  // #EXPERT_TREE load initialization code: run when the program is loaded from the program library or other external sources (does not appear in standard program -- is compiled and run in a separate css program space). Note: ProgramCall's are automatically initialized according to targ_ld_init_name
  ProgEl_List		init_code;
  // initialization code: run when the Init button is pressed
  ProgEl_List		prog_code;
  // program code: run when the Run/Step button is pressed: this is the main code
  
  int			ret_val;
  // #HIDDEN #GUI_READ_ONLY #NO_SAVE return value: 0=ok, +ve=sys-defined err, -ve=user-defined err; also accessible inside program
  ProgEl_List		sub_prog_calls;
  // #HIDDEN #NO_SAVE the ProgramCall direct subprogs of this one, enumerated in the PreGen phase (note: these are ProgramCallBase's, not the actual Program's)
  Program_List		sub_progs_dir;
  // #HIDDEN #NO_SAVE direct sub-programs -- called by sub_prog_calls within this program -- also populated during the PreGen phase
  Program_List		sub_progs_all;
  // #HIDDEN #NO_SAVE the full set of all sub-programs, including sub-programs of sub-programs..
  Program_List		sub_progs_step;
  // #HIDDEN #NO_SAVE all the sub programs eligible for single-stepping
  bool			sub_progs_updtd;
  // #READ_ONLY #NO_SAVE the sub programs were updated -- this is set when updated and cleared after a datachanged
  bool		    	m_stale;
  // #READ_ONLY #NO_SAVE dirty bit -- needs to be public for activating the Compile button
  String		view_script;
  // #READ_ONLY #NO_SAVE current view of script 
  String		view_listing;
  // #READ_ONLY #NO_SAVE current view of listing 

  ProgramRef		step_prog;
  // #FROM_GROUP_sub_progs_step The default program to single step for the Step function in this program
  int			step_n;
  // #MIN_1 how many steps to take when stepping at this program level (i.e., when this program name is clicked on the Step button of any other program) -- also set by the step button dynamically

  inline void		SetProgFlag(ProgFlags flg)   { flags = (ProgFlags)(flags | flg); }
  // #CAT_Flags set flag state on
  inline void		ClearProgFlag(ProgFlags flg) { flags = (ProgFlags)(flags & ~flg); }
  // #CAT_Flags clear flag state (set off)
  inline bool		HasProgFlag(ProgFlags flg) const { return (flags & flg); }
  // #CAT_Flags check if flag is set
  inline void		SetProgFlagState(ProgFlags flg, bool on)
  { if(on) SetProgFlag(flg); else ClearProgFlag(flg); }
  // #CAT_Flags set flag state according to on bool (if true, set flag, if false, clear it)

  static void		SetStopReq(StopReason stop_rsn, const String& stop_message = "");
  // #CAT_Run request that the currently-running program stop at its earliest convenience..
  static void		ClearStopReq();
  // #CAT_Run reset the stop request information

  static const String	GetDescString(const String& dsc, int indent_level);
  // #IGNORE get an appropriately formatted version of the description string for css code

  bool			isStale() const {return m_stale;}
  override void		setStale(); // indicates a component has changed
  void			setRunState(RunState value); // sets and updates gui
  override ScriptSource	scriptSource() {return ScriptString;}
  override const String	scriptString();
  virtual const String	ProgramListing();
  // #CAT_Code generate the listing of the program (NOT the underlying CSS code -- just the program)
  
  virtual void  Init();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run set the program state back to the beginning
  virtual void  Run();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT #CAT_Run run the program
  virtual void	Step(Program* step_prg = NULL);
  // #BUTTON #STEP_BUTTON #CAT_Run step the program at the level of the given program -- if NULL then step_prog default value will be used
  virtual void	Stop();
  // #BUTTON #GHOST_OFF_run_state:RUN #CAT_Run stop the current program at its next natural stopping point (i.e., cleanly stopping when appropriate chunks of computation have completed)
  virtual void	Abort();
  // #BUTTON #GHOST_OFF_run_state:RUN #CAT_Run stop the current program immediately, regardless of where it is

  virtual bool	StopCheck();
  // #CAT_Run calls event loop, then checks for STOP state, true if so
  virtual bool	IsStepProg();
  // #CAT_Run is this program the currently selected step_prog? only true if in step_mode too
  
  virtual void  Compile();
  // #BUTTON #GHOST_ON_script_compiled:true #CAT_Code generate and compile the script code that actually runs (if this button is available, you have changed something that needs to be recompiled)
  virtual void	CmdShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP #CAT_Code set css command shell to operate on this program, so you can run, debug, etc this script from the command line
  virtual void	ExitShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP #CAT_Code exit the command shell for this program (shell returns to previous script)
  virtual void  UpdateCallerArgs();
  // #BUTTON #CAT_Code run UpdateArgs on all the other programs that call me, and also display all these calls in the Find dialog (searching on this program's name) so you can make sure the args are correct for each such program

  int			Call(Program* caller); 
  // #CAT_Run runs the program as a subprogram called from another running program, 0=success
  int			CallInit(Program* caller); 
  // #CAT_Run runs the program's Init from a superProg Init, 0=success
  virtual bool		SetVar(const String& var_nm, const Variant& value);
  // #CAT_Variables set the value of a program variable (only top-level variables in vars or args) -- can be called from within a running program
  virtual bool		SetVarFmArg(const String& arg_nm, const String& var_nm, bool quiet = false);
  // #CAT_Variables set the value of a program variable (using SetVar) based on the value of startup argument arg_nm -- typically called from startup scripts -- displays information about variable set if !quiet
  virtual Variant	GetVar(const String& var_nm);
  // #CAT_Variables get the value of a program variable (only top-level variables in vars or args) -- can be called from within a running program
  bool			HasVar(const String& var_nm);
  // EXPERT #CAT_Variables true if has a var/arg called var_nm (only top-level variables in vars or args) -- can be called from within a running program
  static bool		IsForbiddenName(const String& chk_nm, bool warn=true);
  // #CAT_Code check given name against list of forbidden names -- variables and other objects should check and if forbidden, add an extra character or something

  virtual void		Reset();
  // #MENU #MENU_ON_Object #MENU_CONTEXT #MENU_SEP_BEFORE #CONFIRM #CAT_Code reset (remove) all program elements -- typically in preparation for loading a new program over this one

  static String		GetProgLibPath(ProgLibs library);
  // #CAT_ProgLib get path to given program library

  virtual void		SaveToProgLib(ProgLibs library = USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #CAT_ProgLib save the program to given program library -- file name = object name -- be sure to add good desc comments!!
  virtual void		LoadFromProgLib(ProgLibEl* prog_type);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_GROUP_prog_lib #ARG_VAL_FM_FUN #CAT_ProgLib (re)load the program from the program library element of given type

  virtual void		RunLoadInitCode();
  // #CAT_Run Run the initialization code for object pointer variables and program calls -- to resolve pointers after loading

  virtual ProgVar*	FindVarName(const String& var_nm) const;
  // #CAT_Find find given variable within this program -- NULL if not found
  virtual taBase* 	FindTypeName(const String& nm) const;
  // #CAT_Find find given type name (e.g., dynamic enum type or value) on list
  virtual Program*	FindProgramName(const String& prog_nm, bool warn_not_found=false) const;
  // #CAT_Find find program of given name, first looking within the group that this program belongs in, and then looking for all programs within the project.  if warn_not_found, then issue a warning if not found
  virtual Program*	FindProgramNameContains(const String& prog_nm, bool warn_not_found=false) const;
  // #CAT_Find find program whose name contains given name, first looking within the group that this program belongs in, and then looking for all programs within the project.  if warn_not_found, then issue a warning if not found
  virtual ProgramCallBase*	FindSubProgTarget(Program* prg);
  // #IGNORE find sub_prog_calls ProgramCallBase that calls given target program

  virtual void		SaveScript(ostream& strm);
  // #MENU #MENU_ON_Script #MENU_CONTEXT #BUTTON #CAT_File save the css script generated by the program to a file
#ifdef TA_GUI
public: // XxxGui versions provide feedback to the user 
  virtual void		ViewScript();
  // #MENU #MENU_CONTEXT #NO_BUSY  #CAT_Code view the css script generated by the program
  virtual void		ViewScript_Editor();
  // #MENU #MENU_CONTEXT #CAT_Code open css script in editor defined by taMisc::edit_cmd -- saves to a file based on name of object first
#endif

  virtual void		SaveListing(ostream& strm);
  // #MENU #MENU_SEP_BEFORE #MENU_CONTEXT #BUTTON #CAT_Code save the program listing to a file
#ifdef TA_GUI
public: // XxxGui versions provide feedback to the user 
  virtual void		ViewListing();
  // #MENU #MENU_CONTEXT #NO_BUSY #CAT_Code view the listing of the program
  virtual void		ViewListing_Editor();
  // #MENU #MENU_CONTEXT #CAT_Code open listing of the program in editor defined by taMisc::edit_cmd -- saves to a file based on name of object first
#endif

  static Program*	MakeTemplate(); // #IGNORE make a template instance (with children) suitable for root.templates
  static void		MakeTemplate_fmtype(Program* prog, TypeDef* td); // #IGNORE make from typedef
  
  virtual bool		SelectCtrlFunsForEdit(SelectEdit* editor,
	      const String& extra_label = "", const String& sub_gp_nm = "");
  // #MENU #MENU_ON_SelectEdit #MENU_SEP_BEFORE #NULL_OK_0  #NULL_TEXT_0_NewEditor #CAT_Display add the program control functions (Init, Run, Step, Stop) to a select edit dialog that collects selected members and methods from different objects (if editor is NULL, a new one is created in .edits). returns false if method was already selected.   extra_label is prepended to item names, and if sub_gp_nm is specified, items will be put in this sub-group (new one will be made if it does not yet exist)

  virtual iProgramPanel* FindMyProgramPanel();
  // #IGNORE find my program panel, which contains the program editor -- useful for browser-specific operations
  virtual iDataPanelSet* FindMyDataPanelSet();
  // #IGNORE find my data panel set, which contains all the more specific data panels

  virtual bool		BrowserSelectMe_ProgItem(taOBase* itm);
  // #IGNORE perform BrowserSelectMe function for program sub-item (prog el, etc)
  virtual bool		BrowserExpandAll_ProgItem(taOBase* itm);
  // #IGNORE perform BrowserExpandAll function for program sub-item (prog el, etc)
  virtual bool		BrowserCollapseAll_ProgItem(taOBase* itm);
  // #IGNORE perform BrowserCollapseAll function for program sub-item (prog el, etc)

  virtual bool		ViewCtrlPanel();
  // #CAT_Display select the edit/middle panel view of this object to be for the control panel
  virtual bool		ViewProgEditor();
  // #CAT_Display select the edit/middle panel view of this object to be for the program editor
  virtual bool		ViewProperties();
  // #CAT_Display select the edit/middle panel view of this object to be for the program properties

  override String 	GetTypeDecoKey() const { return "Program"; }
  override Variant 	GetGuiArgVal(const String& fun_name, int arg_idx);
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);

  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(Program);

public: // ScriptBase i/f
  override TypeDef*	GetThisTypeDef() const {return GetTypeDef();}
  // #IGNORE
  override void*	GetThisPtr() { return (void*)this; }
  // #IGNORE

protected:
  static int		m_global_run_ct;
  
  String		m_scriptCache; // cache of script, managed by implementation
  String		m_listingCache; // cache of listing, managed by implementation
  bool			m_checked; // flag to help us avoid doing CheckConfig twice

  override void		UpdateAfterEdit_impl();
  override bool 	CheckConfig_impl(bool quiet);
  override void 	CheckChildConfig_impl(bool quiet, bool& rval);
  override void		InitScriptObj_impl(); // no "this" and install
  override bool		PreCompileScript_impl(); // CheckConfig & add/update the global vars

  virtual void		Stop_impl(); 
  virtual int		Run_impl(); 
  virtual int		Cont_impl(); 
  override void 	ScriptCompiled(); // #IGNORE
  virtual void		UpdateProgVars(); // put global vars in script, set values
  void 			ShowRunError(); // factored error msg code
#ifdef TA_GUI
  virtual void		ViewScript_impl();
#endif
  virtual void		GetSubProgsAll(int depth=0);
  // populate the sub_progs_all lists of all sub programs including self
  virtual void		GetSubProgsStep(int depth=0);
  // populate the sub_progs_step lists of all sub programs including self

  static void		InitForbiddenNames();

private:
  void	Copy_(const Program& cp);
  void	Initialize();
  void	Destroy();
};


//////////////////////////////////
// 	Program Library 	//
//////////////////////////////////

class TA_API ProgLibEl: public taNBase {
  // #INSTANCE #INLINE #CAT_Program an element in the program library
INHERITED(taNBase)
public:
  bool		is_group;	// this is a group of related programs
  String	desc; 		// #EDIT_DIALOG description of what this program does and when it should be used
  String	tags;		// #EDIT_DIALOG list of comma-separated tags describing function of program
  String_Array	tags_array;	// #HIDDEN #READ_ONLY #NO_SAVE parsed version of the tags list, for internal use to actually operate on the tags
  String	lib_name;	// #EDIT_DIALOG name of library that contains this program
  String	URL;		// #EDIT_DIALOG full URL to find this program
  String	filename;	// #EDIT_DIALOG file name given to this program
  String	date;		// #EDIT_DIALOG last modify date for this program
  
  virtual taBase* NewProgram(Program_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT create a new program of this type (return value could be a Program or a Program_Group)
  virtual bool 	LoadProgram(Program* prog);
  // load into given program; true if loaded, false if not
  virtual bool 	LoadProgramGroup(Program_Group* prog_gp);
  // load into given program; true if loaded, false if not

  virtual bool  ParseProgFile(const String& fnm, const String& path);
  // get program information from program or program group file. is_group is set based on extension of file name (.prog or .progp)

  virtual void	ParseTags(); // parse list of tags into tags_array

  override String GetDesc() const { return desc; }
  override String GetTypeName() const { return lib_name; }
  // This shows up in chooser instead of ProgLibEl!

  TA_SIMPLE_BASEFUNS(ProgLibEl);
protected:

private:
  void	Initialize();
  void	Destroy();
};

class TA_API ProgLibEl_List : public taList<ProgLibEl> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of program library elements
INHERITED(taList<ProgLibEl>)
public:
  TA_SIMPLE_BASEFUNS(ProgLibEl_List);
protected:
  
private:
  void	Initialize();
  void	Destroy() { Reset(); CutLinks(); }
};

class TA_API ProgLib: public ProgLibEl_List {
  // #INSTANCE #INLINE #CAT_Program the program library
INHERITED(ProgLibEl_List)
public:
  bool			not_init; // list has not been initialized yet

  void	FindPrograms();		// search paths to find all available programs
  taBase* NewProgram(ProgLibEl* prog_type, Program_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #NO_SAVE_ARG_VAL create a new program in new_owner of given type (return value could be a Program or a Program_Group);  new_owner is group where program will be created
  taBase* NewProgramFmName(const String& prog_nm, Program_Group* new_owner);
  // create a new program (lookup by name) (return value could be a Program or a Program_Group, or NULL if not found); new_owner is group where program will be created
  
  TA_SIMPLE_BASEFUNS(ProgLib);
protected:

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

class TA_API Program_Group : public taGroup<Program> {
  // ##EXT_progp ##FILETYPE_ProgramGroup #CAT_Program a collection of programs sharing common global variables and a control panel interface
INHERITED(taGroup<Program>)
public:
  String		tags;
  // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this program -- should be listed in hierarchical order, with most important/general tags first, as this is how they will be sorted in the program library
  String		desc; // #EDIT_DIALOG description of what this program group does and when it should be used (used for searching in prog_lib -- be thorough!)
  ProgramRef		step_prog;
  // #READ_ONLY #NO_SAVE #OBSOLETE this is just here for loading prior versions and is no longer used in any fashion

  static ProgLib	prog_lib; // #HIDDEN_TREE library of available programs

  taBase* NewFromLib(ProgLibEl* prog_type);
  // #BUTTON #MENU_CONTEXT #FROM_GROUP_prog_lib #NO_SAVE_ARG_VAL #CAT_Program create a new program from a library of existing program types
  taBase* NewFromLibByName(const String& prog_nm);
  // #CAT_Program create a new program from a library of existing program types, looking up by name (NULL if name not found)

  void		SaveToProgLib(Program::ProgLibs library = Program::USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #CAT_Program save the program group to given program library -- file name = object name -- be sure to add good desc comments!!
  virtual void	LoadFromProgLib(ProgLibEl* prog_type);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_GROUP_prog_lib #ARG_VAL_FM_FUN #CAT_Program (re)load the program from the program library element of given type

  virtual bool	RunStartupProgs();
  // run programs marked as STARTUP_RUN -- typically only done by system at startup -- returns true if any run

  void		SetProgsStale(); // set all progs in this group/subgroup to be dirty

  override String 	GetTypeDecoKey() const { return "Program"; }

  override Variant 	GetGuiArgVal(const String& fun_name, int arg_idx);

  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(Program_Group);

private:
  void	Copy_(const Program_Group& cp);
  void	Initialize();
  void 	Destroy()		{Reset(); };
};

TA_SMART_PTRS(Program_Group); // Program_GroupRef

class TA_API ProgramCallBase: public ProgEl { 
  // ##DEF_CHILD_prog_args #VIRT_BASE base class for prog el that calls a program -- manages args and provides interface that Program's require for managing sub-progs
INHERITED(ProgEl)
public:
  ProgArg_List		prog_args; // #SHOW_TREE arguments to the program--copied to prog before call -- all programs in prog_group must accept these same args

  virtual void		UpdateArgs(); 
  // #BUTTON updates the arguments based on the target args (also automatically called in updateafteredit)

  virtual Program*	GetTarget() { return NULL; }
  // safe call to get target: emits error if target is null (used by program) -- call during runtime
  virtual Program*	GetTarget_Compile() { return NULL; }
  // safe call to get target during compile time -- fail silently
  virtual void		AddTargetsToListAll(Program_List& all_lst) { };
  // add any actual targets of this program to the all list-- use LinkUnique -- only non-null!
  virtual void		AddTargetsToListStep(Program_List& step_lst) { };
  // add any actual targets of this program to the step list -- use LinkUnique -- only non-null!
  virtual bool		CallsProgram(Program* prg) { return false; }
  // return true if this program call calls given program
  virtual const String	GenCompileScript(Program* prg) { return _nilString; }
  // generate code to compile script on target
  virtual const String	GenCallInit(Program* prg) { return _nilString; }
  // generate code to call init on target

  virtual bool		LoadInitTarget() { return false; }
  // initialize target based on targ_ld_init information

  override taList_impl*	children_() {return &prog_args;}	
  override String 	GetTypeDecoKey() const { return "Program"; }

  PROGEL_SIMPLE_BASEFUNS(ProgramCallBase);
protected:
  override void		UpdateAfterEdit_impl();
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  virtual const String	GenCssArgSet_impl(const String trg_var_nm, int indent_level); 
private:
  void	Initialize();
  void	Destroy()	{}
};

class TA_API ProgramCall: public ProgramCallBase { 
  // call (run) another program, setting any arguments before hand
INHERITED(ProgramCallBase)
public:
  ProgramRef		target; // the program to be called
  String		targ_ld_init; // #EDIT_DIALOG name(s) of target programs to search for to set this target pointer when program is loaded from program library or other external sources -- if not found, a warning message is emitted.  if empty, it defaults to name of current target. use commas to separate multiple options (tried in order) and an * indicates use the "contains" searching function (not full regexp support yet)

  override Program*	GetTarget();
  override Program*	GetTarget_Compile();
  override void		AddTargetsToListAll(Program_List& all_lst);
  override void		AddTargetsToListStep(Program_List& step_lst);
  override bool		CallsProgram(Program* prg);
  override const String	GenCompileScript(Program* prg);
  override const String	GenCallInit(Program* prg);

  void			SetTarget(Program* target); // #DROP1

  override bool		LoadInitTarget();
  // initialize target based on targ_ld_init information
  virtual bool		LoadInitTarget_impl(const String& nm);
  // initialize target based on targ_ld_init information

  override taList_impl*	children_() {return &prog_args;}	
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Program"; }
  override String	GetToolbarName() const { return "prog()"; }

  PROGEL_SIMPLE_BASEFUNS(ProgramCall);
protected:
  override void		PreGenMe_impl(int item_id); // register the target as a subprog of this one
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 
private:
  void	Initialize();
  void	Destroy()	{}
};

class TA_API ProgramCallVar: public ProgramCallBase { 
  // ##DEF_CHILD_prog_args call (run) another program by name based on a string variable, setting any arguments before hand
INHERITED(ProgramCallBase)
public:
  Program_GroupRef	prog_group; // sub-group of programs to look in for program to call -- ALL of the programs in this group MUST have the same set of args, and all are considered potential candidates to be called (e.g., they are all Init'd when the calling program is Init'd)
  ProgVarRef		prog_name_var; // #ITEM_FILTER_StdProgVarFilter variable that contains name of program within prog_group to call -- this is only used at the time the program call is made when the program is running

  override Program*	GetTarget();
  override Program*	GetTarget_Compile();
  override void		AddTargetsToListAll(Program_List& all_lst);
  override void		AddTargetsToListStep(Program_List& step_lst);
  override bool		CallsProgram(Program* prg);
  override const String	GenCompileScript(Program* prg);
  override const String	GenCallInit(Program* prg);

  virtual Program_Group* GetProgramGp();
  // get prog_group pointer value in a safe way

  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "prog var()"; }

  PROGEL_SIMPLE_BASEFUNS(ProgramCallVar);
protected:
  override void		PreGenMe_impl(int item_id); // register the target as a subprog of this one
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 
private:
  void	Initialize();
  void	Destroy()	{}
};

class TA_API ProgramToolBar: public ToolBar {
// thin subclass to define custom tb for Programs
INHERITED(ToolBar)
public:
  TA_DATAVIEWFUNS(ProgramToolBar, ToolBar) //
protected:
#ifdef TA_GUI
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // in _qt file
#endif
private:
  NOCOPY(ProgramToolBar)
  void Initialize() {}
  void Destroy() {}
};

#endif
