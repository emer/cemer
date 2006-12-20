// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// ta_program.h -- gui-based executables

#ifndef TA_PROGRAM_H
#define TA_PROGRAM_H

#include "ta_group.h"
#include "ta_script.h"
#include "ta_dynenum.h"
#include "ta_viewer.h"

#include "ta_def.h"
#include "ta_TA_type.h"

// forwards

class Program;
class ProgramRef;
class Program_Group;
class Program_List;
class ProgLibEl;
class ProgLib;

class TA_API ProgVar: public taNBase {
  // ##INSTANCE #INLINE #SCOPE_Program ##CAT_Program a program variable, accessible from the outer system, and inside the script in .vars and args
INHERITED(taNBase)
public:
  enum VarType {
    T_Int,			// integer
    T_Real,			// real-valued number (double precision)
    T_String,			// string of characters
    T_Bool,			// boolean true/false 
    T_Object,			// pointer to a C++ object 
    T_HardEnum,			// enumerated list of options (existing C++ hard-coded one)
    T_DynEnum,			// enumerated list of options (from my dynamically created list)
  };

  VarType	var_type;	// type of variable -- determines which xxx_val(s) is/are used
  int		int_val;	// #CONDEDIT_ON_var_type:T_Int,T_HardEnum integer value (also for enum types)
  double	real_val;	// #CONDEDIT_ON_var_type:T_Real real value
  String	string_val;	// #CONDEDIT_ON_var_type:T_String string value
  bool		bool_val;	// #CONDEDIT_ON_var_type:T_Bool boolean value
  TypeDef*	object_type; 	// #CONDEDIT_ON_var_type:T_Object #NO_NULL #TYPE_taBase the minimum acceptable type of the object
  taBaseRef	object_val;	// #CONDEDIT_ON_var_type:T_Object #TYPE_ON_object_type object pointer value
  TypeDef*	hard_enum_type;	// #CONDEDIT_ON_var_type:T_HardEnum #ENUM_TYPE #TYPE_taBase type information for hard enum (value goes in int_val)
  DynEnum	dyn_enum_val;	// #CONDEDIT_ON_var_type:T_DynEnum #HIDDEN_TREE value and type information for dynamic enum
  bool		objs_ptr;	// #HIDDEN this is a pointer to a variable in the objs list of a program
  String	desc;		// #EDIT_DIALOG Description of what this variable is for
  
  void			Cleanup(); // #IGNORE we call this after changing value, to cleanup unused
  virtual const String	GenCssType() const; // type name
  virtual const String	GenCssInitVal() const; // intial value

  virtual const String	GenCss(bool is_arg = false); // css code (terminated if Var);
  
  virtual cssEl*	NewCssEl();
  // get a new cssEl of an appropriate type, name/value initialized
  virtual cssEl*	NewCssType();
  // if object defines new type information (dyn_enum), generate a type object

  virtual void	SetInt(int val);
  virtual void	SetReal(double val);
  virtual void	SetString(const String& val);
  virtual void	SetBool(bool val);
  virtual void	SetObject(taBase* val);
  virtual void	SetHardEnum(TypeDef* enum_type, int val);
  virtual void	SetDynEnum(int val);
  virtual void	SetDynEnumName(const String& val);
 
  override String GetDesc() const { return desc; }
  override String GetDisplayName() const;

  TypeDef*		act_object_type() const; // #IGNORE the actual object type; never NULL (taBase min)
  
  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); // don't save the unused vals
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void 	SetDefaultName() {} // make it local to list, set by list
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgVar& cp);
  COPY_FUNS(ProgVar, inherited);
  TA_BASEFUNS(ProgVar);
protected:
  override void UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void 	CheckChildConfig_impl(bool quiet, bool& rval); //object, if any
  virtual const String	GenCssArg_impl();
  virtual const String	GenCssVar_impl();
private:
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

  void	setDirty(bool value);
  void	Copy_(const ProgVar_List& cp);
  COPY_FUNS(ProgVar_List, inherited);
  TA_BASEFUNS(ProgVar_List);
  
protected:
  override void	El_SetIndex_(void*, int);
  
private:
  void	Initialize();
  void	Destroy() {Reset();}
};

SmartRef_Of(ProgVar); // ProgVarRef

class TA_API ProgArg: public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Program a program or method argument
INHERITED(taOBase)
public:
  String                name; // #SHOW #READ_ONLY the name of the argument (always same as the target) 
  String		value; // #EDIT_DIALOG the value passed to the argument, can be a literal, or refer to other things in the program; string values must be quoted
  
  virtual void		Freshen(const ProgVar& cp); 
  // updates our value/type information and commensurable fields from compatible type (but not name)

  bool  	SetName(const String& nm) 	{ name = nm; return true; } 
  String 	GetName() const			{ return name; } 
  String 	GetDisplayName() const;

  void	Copy_(const ProgArg& cp);
  COPY_FUNS(ProgArg, inherited);
  TA_BASEFUNS(ProgArg);
private:
  void	Initialize();
  void	Destroy();
};


class TA_API ProgArg_List : public taList<ProgArg> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of arguments
INHERITED(taList<ProgArg>)
public:
  virtual void		ConformToTarget(ProgVar_List& targ); // make us conform to the target
  
  TA_BASEFUNS(ProgArg_List);
  
private:
  void	Initialize();
  void	Destroy() {Reset();}
};


class TA_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE ##EDIT_INLINE ##SCOPE_Program ##CAT_Program base class for a program element
INHERITED(taOBase)
public:
  static const String	DisplayNameFromLongString(const String& verbose);
    // get a reasonable short display name from long program text or comments
//TODO: remove the AKA from desc, and this line, at some point    
  String		desc; // #EDIT_DIALOG #HIDDEN_INLINE #AKA_comment optional brief description of element's function; included as comment in script
  bool			off;	// #DEF_false turn off this program element: do not include in script

  virtual ProgEl*   	parent() {return GET_MY_OWNER(ProgEl);}
  Program*		program() {return GET_MY_OWNER(Program);} 
  
  void			PreGen(int& item_id); //recursive walk of items before code gen; each item bumps its id and calls subitems; esp. used to discover subprogs in order
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object (usually override _impl's)
  
  override String GetDesc() const {return desc;}
  void	Copy_(const ProgEl& cp);
  COPY_FUNS(ProgEl, inherited);
  TA_BASEFUNS(ProgEl);

protected:
  virtual bool		useDesc() const {return true;} // hack for CommentEl
  override bool 	CheckConfig_impl(bool quiet);
  virtual void		PreGenMe_impl(int item_id) {}
  virtual void		PreGenChildren_impl(int& item_id) {}
  virtual const String	GenCssPre_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css prefix code (if any) for this object	
  virtual const String	GenCssBody_impl(int indent_level) { return _nilString; } // #IGNORE generate the Css body code for this object
  virtual const String	GenCssPost_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css postfix code (if any) for this object

private:
  void	Initialize();
  void	Destroy();
};


class TA_API ProgEl_List: public taList<ProgEl> {
  // #TREEFILT_ProgGp ##CAT_Program list of program elements: a block of code
INHERITED(taList<ProgEl>)
public:
  virtual void		PreGen(int& item_id); // iterates over all items
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object
  
  override int		NumListCols() const {return 2;} 
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;
  TA_BASEFUNS(ProgEl_List);

private:
  void	Initialize();
  void	Destroy();
};


class TA_API CodeBlock: public ProgEl { 
  // a block of code (list of program elements), each executed in sequence
INHERITED(ProgEl)
public:
  ProgEl_List	    	prog_code; // list of Program elements: the block of code
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(CodeBlock);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};


class TA_API ProgVars: public ProgEl {
  // local program variables (not globally accessible)
INHERITED(ProgEl)
public:
  ProgVar_List		local_vars;	// the list of variables
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(ProgVars);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy();
};


class TA_API UserScript: public ProgEl { 
  // a user-defined css script (can access all program variables, etc)
INHERITED(ProgEl)
public:
  String	    user_script; // #EDIT_DIALOG content of the user script

  virtual void	    ImportFromFile(istream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css import script from file
  virtual void	    ImportFromFileName(const String& fnm); // import script from file
  virtual void	    ExportToFile(ostream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css export script to file
  virtual void	    ExportToFileName(const String& fnm); // export script to file
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(UserScript);
protected:
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{}
};

class TA_API Loop: public ProgEl { 
  // #VIRT_BASE base class for loops
INHERITED(ProgEl)
public:
  ProgEl_List		loop_code; // #SHOW_TREE the items to execute in the loop
  String	    	loop_test; // #EDIT_DIALOG a test expression for whether to continue looping (e.g., 'i < max')
  
  SIMPLE_LINKS(Loop);
  SIMPLE_COPY(Loop);
  COPY_FUNS(Loop, inherited);
  TA_ABSTRACT_BASEFUNS(Loop);

protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{CutLinks();}
};

class TA_API WhileLoop: public Loop { 
  // Repeat loop_code while loop_test expression is true (test first): while(loop_test) do loop_code
INHERITED(Loop)
public:
  
  override String	GetDisplayName() const;

  TA_BASEFUNS(WhileLoop);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{}
};

class TA_API DoLoop: public Loop { 
  // Do loop_code repatedly while loop_test expression is true (test-after): do loop_code while(loop_test);
INHERITED(Loop)
public:
  
  override String	GetDisplayName() const;
  TA_BASEFUNS(DoLoop);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{}
};

class TA_API ForLoop: public Loop { 
  // Standard C 'for loop' over loop_code: for(init_expr; loop_test; loop_iter) loop_code\n -- runs the init_expr, then does loop_code and the loop_iter expression, and continues if loop_test is true
INHERITED(Loop)
public:
  String	    	init_expr; // #EDIT_DIALOG initialization expression (e.g., declare the loop variable; 'int i')
  String	    	loop_iter; // #EDIT_DIALOG the iteration operation run after each loop (e.g., increment the loop variable; 'i++')
  
  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(ForLoop);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{}
};

class TA_API IfContinue: public ProgEl { 
  // if condition is true, continue looping (skip any following code and loop back to top of loop)
INHERITED(ProgEl)
public:
  String	    	condition; // #EDIT_DIALOG #AKA_cond_expr conditionalizing expression for continuing loop
  
  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(IfContinue);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API IfBreak: public ProgEl { 
  // if condition is true, break out of current loop
INHERITED(ProgEl)
public:
  String	    	condition; // #EDIT_DIALOG #AKA_cond_expr conditionalizing expression for breaking out of loop
  
  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(IfBreak);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API IfElse: public ProgEl { 
  // a conditional test element: if(condition) then true_code; else false_code
INHERITED(ProgEl)
public:
  String	    condition; // #EDIT_DIALOG  #AKA_cond_test condition expression to test
  ProgEl_List	    true_code; // #SHOW_TREE items to execute if condition true
  ProgEl_List	    false_code; // #SHOW_TREE items to execute if condition false
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(IfElse);

protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

class TA_API AssignExpr: public ProgEl { 
  // assign a variable to an expression (use method call for simple assignment to function call)
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // where to store the result of the epxression
  String		expr;
  // #EDIT_DIALOG expression to assign variable to
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(AssignExpr);

protected:
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MethodCall: public ProgEl { 
  // call a method (member function) on an object
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // where to store the result of the method call (optional -- can be NULL)
  ProgVarRef		obj;
  // #AKA_script_obj program variable that points to the object with the method to call
  TypeDef*		obj_type;
  // #NO_SHOW #NO_SAVE temp copy of obj.object_type
  MethodDef*		method;
  // #TYPE_ON_obj_type the method to call on object obj
  SArg_Array		args;
  // arguments to the method

  static void		UpdateArgs(SArg_Array& ar, MethodDef* md);
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(MethodCall);

protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API StaticMethodCall: public ProgEl { 
  // call a static method (member function) on a type 
INHERITED(ProgEl)
public:
  ProgVarRef		result_var; // result variable (optional -- can be NULL)
  TypeDef*		min_type; // #NO_SHOW #NO_SAVE #TYPE_taBase minimum object type to choose from -- anchors selection of object_type (derived versions can set this)
  TypeDef*		object_type; // #TYPE_ON_min_type The object type to look for methods on
  MethodDef*		method; //  #TYPE_ON_object_type the method to call
  SArg_Array		args; // arguments to the method
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(StaticMethodCall);

protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MathCall : public StaticMethodCall { 
  // call a taMath function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS(MathCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

class TA_API RandomCall : public StaticMethodCall { 
  // call a Random number generation function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS(RandomCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

class TA_API MiscCall : public StaticMethodCall { 
  // call a taMisc function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS(MiscCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

class TA_API PrintVar: public ProgEl { 
  // print out the value of a variable
INHERITED(ProgEl)
public:
  ProgVarRef		print_var; 	// print out the value of this variable
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(PrintVar);

protected:
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

// todo: highlight this in diff color:
class TA_API Comment: public ProgEl { 
  // insert a highlighted (possibly) multi-line comment -- useful for describing an upcoming chunk of code
INHERITED(ProgEl)
public:
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(Comment);

protected:
  override bool		useDesc() const {return false;} 
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API StopStepPoint: public ProgEl { 
  // this is a point in the program where the Stop button will stop execution, and the Step button will act for single stepping (e.g., place inside of a loop) -- otherwise this only happens at the end of programs
INHERITED(ProgEl)
public:
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(StopStepPoint);

protected:
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API Function: public ProgEl { 
  // a user-defined function that can be called within the program where it is defined -- must live in the functions of a Program, not in init_code or prog_code 
INHERITED(ProgEl)
public:
  ProgVar		return_val;
  // The return value of the function
  String		name;
  // The function name
  ProgVar_List		args;
  // The arguments to the function
  ProgEl_List	    	fun_code;
  // the function code (list of program elements)
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(Function);
protected:
  override void		UpdateAfterEdit_impl();
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};

SmartRef_Of(Function);

class TA_API FunctionCall: public ProgEl { 
  // call a function
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // where to store the result (return value) of the function (optional -- can be NULL)
  FunctionRef		fun;
  // the function to be called
  ProgArg_List		fun_args;
  // arguments to the function: passed when called

  virtual void		UpdateArgs(); 
  // updates the arguments (automatically called in updateafteredit)

  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(FunctionCall);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level); 
private:
  void	Initialize();
  void	Destroy()	{}
};

class ProgObjList: public taBase_List {
  // ##CAT_Program A list of program objects (just a taBase list with proper update actions to update variables associated with objects)
INHERITED(taBase_List)
public:

  override void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  virtual void	GetVarsForObjs();
  // automatically create variables for objects in parent program

  TA_BASEFUNS(ProgObjList);
protected:
  void*		El_Own_(void* it); // give anon objs a name

private:
  void Initialize() { SetBaseType(&TA_taNBase); } // need name for var, owner to stream!
  void Destroy() { }
};


class TA_API Program: public taNBase, public AbstractScriptBase {
  // ##TOKENS ##INSTANCE ##EXT_prog ##FILETYPE_Program ##CAT_Program a structured gui-buildable program that generates css script code to actually run
INHERITED(taNBase)
public:
  enum ProgFlags { // #BITS mode flags
    PF_NONE		= 0, // #NO_BIT
    NO_STOP		= 0x0001 // this program cannot be stopped by Stop or Step buttons
  };
  
  enum ReturnVal { // system defined return values (<0 are for user defined)
    RV_OK	= 0, 	// program finished successfully
    RV_COMPILE_ERR, 	// script couldn't be compiled
    RV_CHECK_ERR,	// program or its dependencies failed CheckConfig
    RV_INIT_ERR, 	// initialization failed (note: user prog may use its own value)
    RV_RUNTIME_ERR,	// misc runtime error (ex, null pointer ref, etc.)
    RV_PROG_CALL_FAILED, // a program call failed (probably an error in that program)
    RV_ALREADY_RUNNING, // attempt to run a new program chain when a program chain is already running
    RV_NO_PROGRAM // no program was available to run
  };
   
  enum RunState { // current run state for this program
    DONE = 0, 	// there is no program running or stopped; any previous run completed
    INIT,	// program is running its init_code
    RUN,	// program is running its prog_code
    STOP,	// the program is stopped (note: NOT the same as "DONE")
    NOT_INIT,	// init has not yet been run
  };

  enum ProgLibs {
    USER_LIB,			// user's personal library
    SYSTEM_LIB,			// local system library
    WEB_LIB,			// web-based library
    SEARCH_LIBS,		// search through the libraries (for loading)
  };

  static ProgLib* 	prog_lib; // #HIDDEN_TREE library of available programs

  Program_Group*	prog_gp;
  // #READ_ONLY #NO_SAVE our owning program group -- needed for control panel stuff

  RunState		run_state;
  // #READ_ONLY #NO_SAVE this program's run state
  static bool		stop_req;
  // #READ_ONLY a stop was requested by someone -- stop at next chance
  static bool		step_mode;
  // #READ_ONLY the program was run in step mode -- check for stepping
  
  String		desc;
  // #EDIT_DIALOG #HIDDEN_INLINE description of what this program does and when it should be used (used for searching in prog_lib -- be thorough!)
  ProgFlags		flags;
  // control flags, for display and execution control
  ProgObjList		objs;
  // #TREEFILT_ProgGp create persistent objects of any type here that are needed for the program -- each object will automatically create an associated variable 
  ProgVar_List		args;
  // global variables that are parameters (arguments) for callers
  ProgVar_List		vars;
  // global variables accessible outside and inside script
  ProgEl_List		functions;
  // function code (for defining subroutines): goes at top of script and can be called from init or prog code
  ProgEl_List		init_code;
  // initialization code: run when the Init button is pressed
  ProgEl_List		prog_code;
  // program code: run when the Run/Step button is pressed: this is the main code
  
  int			ret_val;
  // #HIDDEN #GUI_READ_ONLY #NO_SAVE return value: 0=ok, +ve=sys-defined err, -ve=user-defined err; also accessible inside program
  ProgEl_List		sub_progs;
  // #HIDDEN #NO_SAVE the direct subprogs of this one, enumerated in the PreGen phase (note: these are ProgramCall's, not the actual Program's)
  bool		    	m_dirty;
  // #READ_ONLY #NO_SAVE dirty bit -- needs to be public for activating the Compile button
  
  bool			isDirty() {return m_dirty;}
  override void		setDirty(bool value); // indicates a component has changed
  void			setRunState(RunState value); // sets and updates gui
  override ScriptSource	scriptSource() {return ScriptString;}
  override const String	scriptString();
  
  virtual void  Init();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT set the program state back to the beginning
  virtual void  Run();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP run the program
  virtual void	Step();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP step the program, at the previously selected step level (see SetAsStep or the program group control panel)
  virtual void	Stop();
  // #BUTTON #GHOST_OFF_run_state:RUN stop the current program at its next natural stopping point (i.e., cleanly stopping when appropriate chunks of computation have completed)
  virtual void	Abort();
  // #BUTTON #GHOST_OFF_run_state:RUN stop the current program immediately, regardless of where it is
  
  virtual void	SetAsStep();
  // #BUTTON set this program as the step level for this set of programs -- this is the grain size of stepping when the Step button is pressed (for a higher-level program)
  virtual void  Compile();
  // #BUTTON #GHOST_OFF_m_dirty:true generate and compile the script code that actually runs (if this button is available, you have changed something that needs to be recompiled)
  virtual void	CmdShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP set css command shell to operate on this program, so you can run, debug, etc this script from the command line
  virtual void	ExitShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP exit the command shell for this program (shell returns to previous script)

  int			Call(Program* caller); 
  // runs the program as a subprogram called from another running program, 0=success
  int			CallInit(Program* caller); 
  // runs the program's Init from a superProg Init, 0=success
  virtual bool		SetVar(const String& var_nm, const Variant& value);
  // set the value of a program variable (in the cssProgSpace) prior to calling Run
  bool			StopCheck(); // calls event loop, then checks for STOP state, true if so

  virtual void		Reset();
  // #MENU #MENU_ON_Object #MENU_CONTEXT #MENU_SEP_BEFORE reset (remove) all program elements -- typically in preparation for loading a new program over this one

  static String		GetProgLibPath(ProgLibs library);
  // get path to given program library

  virtual void		SaveToProgLib(ProgLibs library = USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT save the program to given program library -- file name = object name -- be sure to add good desc comments!!
  virtual void		LoadFromProgLib(ProgLibEl* prog_type);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_GROUP_prog_lib (re)load the program from the program library element of given type

  virtual void		SaveScript(ostream& strm);
  // #MENU #MENU_ON_Script #MENU_CONTEXT #BUTTON save the css script generated by the program to a file

#ifdef TA_GUI
public: // XxxGui versions provide feedback to the usbool no_gui = falseer
  virtual void		ViewScript();
    // #MENU #MENU_CONTEXT #BUTTON #NO_BUSY view the css script generated by the program
  virtual void		EditScript();
    // #MENU #MENU_CONTEXT #BUTTON open css script in editor defined by taMisc::edit_cmd -- saves to a file based on name of object first
#endif

  static Program*	MakeTemplate(); // #IGNORE make a template instance (with children) suitable for root.templates
  static void		MakeTemplate_fmtype(Program* prog, TypeDef* td); // #IGNORE make from typedef
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Program& cp);
  COPY_FUNS(Program, inherited);
  TA_BASEFUNS(Program);

public: // ScriptBase i/f
  override TypeDef*	GetThisTypeDef() const {return GetTypeDef();}
  // #IGNORE
  override void*	GetThisPtr() { return (void*)this; }
  // #IGNORE

protected:
  String		m_scriptCache; // cache of script, managed by implementation
  bool			m_checked; // flag to help us avoid doing CheckConfig twice
  override void		UpdateAfterEdit_impl();
  override bool 	CheckConfig_impl(bool quiet);
  override void 	CheckChildConfig_impl(bool quiet, bool& rval);
  virtual void		DirtyChanged_impl() {} // called when m_dirty was changed 
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

private:
  void	Initialize();
  void	Destroy();
};

SmartRef_Of(Program); // ProgramRef

class TA_API Program_List : public taList<Program> {
  // ##CAT_Program a list of programs
  INHERITED(taList<Program>)
public:
  
  TA_BASEFUNS(Program_List);
private:
  void	Initialize();
  void 	Destroy()		{Reset(); }; //
}; //


//////////////////////////////////
// 	Program Library 	//
//////////////////////////////////

class TA_API ProgLibEl: public taNBase {
  // #INSTANCE #INLINE #CAT_Program an element in the program library
INHERITED(taNBase)
public:
  bool		is_group;	// this is a group of related programs
  String	desc; 		// #EDIT_DIALOG description of what this program does and when it should be used
  String	lib_name;	// #EDIT_DIALOG name of library that contains this program
  String	URL;		// #EDIT_DIALOG full URL to find this program
  String	filename;	// #EDIT_DIALOG file name given to this program
  
  virtual taBase* NewProgram(Program_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT create a new program of this type (return value could be a Program or a Program_Group)
  virtual bool 	LoadProgram(Program* prog);
  // load into given program; true if loaded, false if not

  virtual bool ParseProgFile(const String& fnm, const String& path);
  // get program information from program or program group file. is_group is set based on extension of file name (.prog or .progp)

  override String GetDesc() const { return desc; }
  override String GetTypeName() const { return lib_name; }
  // This shows up in chooser instead of ProgLibEl!

  TA_SIMPLE_BASEFUNS(ProgLibEl);
protected:

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
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
  // #MENU #MENU_ON_Object #MENU_CONTEXT create a new program in new_owner of given type (return value could be a Program or a Program_Group);  new_owner is group where program will be created
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
  ProgramRef		step_prog; // the program that will be stepped when the Step button is pressed
  String		desc; // #EDIT_DIALOG description of what this program group does and when it should be used (used for searching in prog_lib -- be thorough!)

  static ProgLib	prog_lib; // #HIDDEN_TREE library of available programs

  taBase* NewFromLib(ProgLibEl* prog_type);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_GROUP_prog_lib create a new program from a library of existing program types
  taBase* NewFromLibByName(const String& prog_nm);
  // create a new program from a library of existing program types, looking up by name (NULL if name not found)

  void		SaveToProgLib(Program::ProgLibs library = Program::USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT save the program group to given program library -- file name = object name -- be sure to add good desc comments!!

  void		SetProgsDirty(); // set all progs in this group/subgroup to be dirty
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Program_Group& cp);
  COPY_FUNS(Program_Group, inherited)
  TA_BASEFUNS(Program_Group);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); };
};

class TA_API ProgramCall: public ProgEl { 
  // call (run) another program, setting any arguments before hand
INHERITED(ProgEl)
public:
  ProgramRef		target; // the program to be called
  bool			call_init; // if true, run the init_code on that program, not prog_code
  ProgArg_List		prog_args; // arguments to the program--copied to prog before call

  virtual void		UpdateArgs(); 
  // updates the arguments (automatically called in updateafteredit)

  virtual Program*	GetTarget();
  // safe call to get target: emits error if target is null (used by program)

  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(ProgramCall);
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
  void Initialize() {}
  void Destroy() {}
};

#endif
