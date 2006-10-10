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

// program.h -- gui-based executables

#ifndef PROGRAM_H
#define PROGRAM_H

#include "ta_group.h"
#include "ta_script.h"
#include "dynenum.h"
#include "ta_viewer.h"

#include "ta_def.h"
#include "ta_TA_type.h"

// forwards

class Program;
class ProgramRef;
class Program_Group;
class Program_List;

class TA_API ProgVar: public taNBase {
  // ##INSTANCE #INLINE a program variable, accessible from the outer system, and inside the script in .vars and args
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
  virtual const String	GenCssType(); // type name
  virtual const String	GenCssInitVal(); // intial value

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

  override bool		CheckConfig(bool quiet=false);
  
  override String GetDesc() const { return desc; }
  override String GetDisplayName() const;

  TypeDef*		act_object_type() const; // #IGNORE the actual object type; never NULL (taBase min)
  
  override bool		Dump_QuerySaveMember(MemberDef* md); // don't save the unused vals
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void 	SetDefaultName() {} // make it local to list, set by list
  void	UpdateAfterEdit(); // we always nuke the cached cssEl -- it will get regenerated
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgVar& cp);
  COPY_FUNS(ProgVar, inherited);
  TA_BASEFUNS(ProgVar);
protected:
  virtual const String	GenCssArg_impl();
  virtual const String	GenCssVar_impl();
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ProgVar_List : public taList<ProgVar> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE list of script variables
INHERITED(taList<ProgVar>)
public:
  enum VarContext {
    VC_ProgVars,  // #LABEL_ProgramVariables program variables
    VC_FuncArgs  //  #LABEL_FunctionArguments function arguments
  };
  
  VarContext	var_context; // #DEF_VC_ProgVars #HIDDEN #NO_SAVE context of vars, set by owner
  
  virtual const String 	GenCss(int indent_level) const; // generate css script code for the context

  override bool		CheckConfig(bool quiet=false);	// return false if not properly configured for generating a program
  
  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
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
  // ##NO_TOKENS ##INSTANCE a program or method argument
INHERITED(taOBase)
public:
  String		name; // #SHOW #READ_ONLY the name of the argument (always same as the target)
  String		value; // the value passed to the argument, can be a literal, or refer to other things in the program; string values must be quoted
  
  virtual void		Freshen(const ProgVar& cp); 
  // updates our value/type information and commensurable fields from compatible type (but not name)
  
  void 	SetDefaultName() {} // name is always the same as the referent
  void	Copy_(const ProgArg& cp);
  bool 	SetName(const String& nm) {name = nm; return true;}
  String GetName() const{ return name; }
  COPY_FUNS(ProgArg, inherited);
  TA_BASEFUNS(ProgArg);
private:
  void	Initialize();
  void	Destroy();
};


class TA_API ProgArg_List : public taList<ProgArg> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE list of arguments
INHERITED(taList<ProgArg>)
public:
  virtual void		ConformToTarget(ProgVar_List& targ); // make us conform to the target
  
  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS(ProgArg_List);
  
private:
  void	Initialize();
  void	Destroy() {Reset();}
};


class TA_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE definition of a program element
INHERITED(taOBase)
public:
  String		desc; // #EDIT_DIALOG #HIDDEN_INLINE optional brief description of element's function; included as comment in script
  bool			off;	// #DEF_false turn off this program element: do not include in script

  virtual ProgEl*   	parent() {return GET_MY_OWNER(ProgEl);}
  Program*		program() {return GET_MY_OWNER(Program);} 
  
  void			PreGen(int& item_id); //recursive walk of items before code gen; each item bumps its id and calls subitems; esp. used to discover subprogs in order
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object (usually override _impl's)
  
  virtual bool		CheckConfig(bool quiet=false);	// return false if not properly configured for generating a program

  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	ChildUpdateAfterEdit(TAPtr child, bool& handled); // detect children of our subclasses changing

  override String GetDesc() const {return desc;}
  void	Copy_(const ProgEl& cp);
  COPY_FUNS(ProgEl, inherited);
  TA_BASEFUNS(ProgEl);

protected:
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
INHERITED(taList<ProgEl>)
public:
  virtual void		PreGen(int& item_id); // iterates over all items
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object
  
  virtual bool		CheckConfig(bool quiet=false);	// return false if not properly configured for generating a program

  override int		NumListCols() const {return 2;} 
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS(ProgEl_List);

private:
  void	Initialize();
  void	Destroy();
};


class TA_API ProgList: public ProgEl { 
  // list of ProgEl's, each executed in sequence
INHERITED(ProgEl)
public:
  ProgEl_List	    	prog_code; // list of ProgEl's
  
  override bool		CheckConfig(bool quiet=false);

  override String	GetDisplayName() const;
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgList& cp);
  COPY_FUNS(ProgList, inherited);
  TA_BASEFUNS(ProgList);

protected:
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};


class TA_API ProgVars: public ProgEl {
INHERITED(ProgEl)
public:
  ProgVar_List	script_vars;
  
  override bool		CheckConfig(bool quiet=false);
  override String	GetDisplayName() const;
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(ProgVars);

protected:
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy();
};


class TA_API UserScript: public ProgEl { 
  // ProgEl for a user scriptlet
INHERITED(ProgEl)
public:
  String	    user_script; // #EDIT_DIALOG content of the user scriptlet

  virtual void	    ImportFromFile(istream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css import script from file
  virtual void	    ImportFromFileName(const String& fnm); // import script from file
  virtual void	    ExportToFile(ostream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css export script to file
  virtual void	    ExportToFileName(const String& fnm); // export script to file
  
  override String	GetDisplayName() const;
  void	Copy_(const UserScript& cp);
  COPY_FUNS(UserScript, inherited);
  TA_BASEFUNS(UserScript);

protected:
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{}
};

class TA_API Loop: public ProgEl { 
  // #VIRT_BASE ##EDIT_INLINE base class for loops
INHERITED(ProgEl)
public:
  ProgEl_List		loop_code; // #BROWSE the items to execute in the loop
  String	    	loop_test; // #EDIT_DIALOG a test expression for whether to continue looping (e.g., 'i < max')
  
  override bool		CheckConfig(bool quiet=false);
  SIMPLE_LINKS(Loop);
  SIMPLE_COPY(Loop);
  COPY_FUNS(Loop, inherited);
  TA_ABSTRACT_BASEFUNS(Loop);

protected:
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
  override bool		CheckConfig(bool quiet=false);

  TA_SIMPLE_BASEFUNS(ForLoop);
protected:
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
  override bool		CheckConfig(bool quiet=false);

  TA_SIMPLE_BASEFUNS(IfContinue);
protected:
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
  override bool		CheckConfig(bool quiet=false);

  TA_SIMPLE_BASEFUNS(IfBreak);
protected:
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
  ProgEl_List	    true_code; // #BROWSE items to execute if condition true
  ProgEl_List	    false_code; // #BROWSE items to execute if condition false
  
  override bool		CheckConfig(bool quiet=false);
  override String	GetDisplayName() const;
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const IfElse& cp);
  COPY_FUNS(IfElse, inherited);
  TA_BASEFUNS(IfElse);

protected:
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};


class TA_API MethodSpec: public taOBase { 
  // #EDIT_INLINE #HIDDEN #NO_TOKENS helper obj for MethodCall; has custom taiData
INHERITED(taOBase)
public:
  ProgVarRef		script_obj; // #SCOPE_Program_Group the previously defined script object that has the method
  TypeDef*		object_type; // #NO_SHOW #NO_SAVE temp copy of script_obj.object_type
  MethodDef*		method; //  #TYPE_ON_object_type the method to call
  
  virtual bool		CheckConfig(bool quiet=false);	// return false if not properly configured for generating a program

  void	UpdateAfterEdit();
  void	CutLinks();
  void	Copy_(const MethodSpec& cp);
  COPY_FUNS(MethodSpec, inherited);
  TA_BASEFUNS(MethodSpec);
  
private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MethodCall: public ProgEl { 
  // call a method (member function) on an object
INHERITED(ProgEl)
friend class MethodSpec;
public:
  String		result_var; // result variable (optional)
  MethodSpec		method_spec; //  the method to call
  SArg_Array		args; // arguments to the method
  
  override bool		CheckConfig(bool quiet=false);
  override String	GetDisplayName() const;
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const MethodCall& cp);
  COPY_FUNS(MethodCall, inherited);
  TA_BASEFUNS(MethodCall);

protected:
  ProgVar*		lst_script_obj; 
  MethodDef*		lst_method; 
  
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object
  virtual void		CheckUpdateArgs(bool force = false); // called when method changes

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API Program_List : public taList<Program> {
INHERITED(taList<Program>)
public:
  
  virtual bool		CheckConfig(bool quiet=false);	// return false if not properly configured for generating a program
  TA_BASEFUNS(Program_List);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); }; //
}; //


class TA_API Program: public taNBase, public AbstractScriptBase {
  // ##TOKENS ##INSTANCE ##EXT_prog a structured gui-buildable program that generates css script code to actually run
INHERITED(taNBase)
public:
  enum ProgFlags { // #BITS mode flags
    PF_NONE		= 0, // #NO_BIT
    NO_STOP		= 0x0001 // this program cannot be stopped by Stop or Step buttons
  };
  
  enum ReturnVal { // system defined return values (<0 are for user defined)
    RV_OK	= 0, 	// program finished successfully
    RV_COMPILE_ERR, 	// script couldn't be compiled
    RV_INIT_ERR, 	// initialization failed (note: user prog may use its own value)
    RV_RUNTIME_ERR,	// misc runtime error (ex, null pointer ref, etc.)
    RV_PROG_CALL_FAILED, // a program call failed (probably an error in that program)
    RV_ALREADY_RUNNING, // attempt to run a new program chain when a program chain is already running
    RV_NO_PROGRAM // no program was available to run
  };
   
  // TODO: need to clarify difference between current state and requested state, ex RUN but user wants to STOP, while running, still not stopped -- may need a separate var for requests to stop -- but if split between two vars, makes gui state control a lot more complicated (custom code vs. baked in gui enabling)
  enum RunState { // current run state, is global to all active programs
    DONE = 0, 	// there is no program running or stopped; any previous run completed
    INIT,	// tells the prog to reset its state to the beginning; this is a "running" state
    STOP,	// the program is stopped (note: NOT the same as "DONE")
    RUN,	// normal running state; this is a "running" state
    STEP,	// state when we are executing a Step; this is a "running" state
    STOP_REQ,	// a stopping request has been issued -- next StopCheck should find and stop!
    NOT_INIT	// init has not yet been run
  };
  
  static RunState	run_state; // #READ_ONLY the one and only global run mode for current running prog
  
  static Program*	MakeTemplate(); // make a template instance (with children) suitable for root.templates
  
  Program_Group*	prog_gp;   // #READ_ONLY #NO_SAVE our owning program group -- needed for control panel stuff
  
  String		desc; // #EDIT_DIALOG description of what this program does and when it should be used (used for searching in prog_lib -- be thorough!)
  ProgFlags		flags;  // control flags, for display and execution control
  taBase_List		objs; // sundry objects that are used in this program
  ProgVar_List		args; // global variables that are parameters (arguments) for callers
  ProgVar_List		vars; // global variables accessible outside and inside script
  ProgEl_List		init_code; // the prog els for initialization (done once); use a "return" if an error occurs 
  ProgEl_List		prog_code; // the prog els for the main program
  
  int			ret_val; // #HIDDEN #GUI_READ_ONLY #NO_SAVE return value: 0=ok, +ve=sys-defined err, -ve=user-defined err; also accessible inside program
  ProgEl_List		sub_progs; // #HIDDEN #NO_SAVE the direct subprogs of this one, enumerated in the PreGen phase (note: these are ProgramCall's, not the actual Program's)
  bool		    	m_dirty; // #READ_ONLY #NO_SAVE dirty bit -- needs to be public for activating the Compile button
  
  bool			isDirty() {return m_dirty;}
  void			setDirty(bool value); // indicates a component has changed
  void			setRunState(RunState value); // sets and updates gui
  override ScriptSource	scriptSource() {return ScriptString;}
  override const String	scriptString();
  
  virtual void  Init();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP,NOT_INIT set the program state back to the beginning
  virtual void  Run();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP run the programs
  virtual void	Step();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP step the program, at the selected step level
  virtual void	Stop();
  // #BUTTON #GHOST_OFF_run_state:RUN,STEP stop the running programs
  
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
  virtual bool		SetGlobalVar(const String& nm, const Variant& value);
  // set the value of a global variable (in the cssProgSpace) prior to calling Run
  bool			StopCheck(); // calls event loop, then checks for STOP state, true if so

  virtual bool		CheckConfig(bool quiet=false);	// return false if not properly configured for generating a program

  virtual void		SaveScript(ostream& strm);
  // #MENU #MENU_ON_Actions #MENU_CONTEXT #BUTTON save the css script generated by the program to a file

#ifdef TA_GUI
public: // XxxGui versions provide feedback to the usbool no_gui = falseer
  virtual void		ViewScript();
    // #MENU #MENU_ON_Actions #MENU_CONTEXT #BUTTON view the css script generated by the program
  virtual void		EditScript();
    // #MENU #MENU_ON_Actions #MENU_CONTEXT #BUTTON open css script in editor defined by taMisc::edit_cmd -- saves to a file based on name of object first
#endif

  void	UpdateAfterEdit();
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
  virtual void		DirtyChanged_impl() {} // called when m_dirty was changed 
  override bool		PreCompileScript_impl(); // CheckConfig & add/update the global vars
  virtual void		Stop_impl(); 
  virtual int		Run_impl(); 
  virtual int		Cont_impl(); 
  override void 	ScriptCompiled(); // #IGNORE
  virtual void		UpdateProgVars(); // put global vars in script, set values
  virtual void	       	GetVarsForObjs(); // automatically create variables for objects in objs
#ifdef TA_GUI
  virtual void		ViewScript_impl();
#endif

private:
  void	Initialize();
  void	Destroy();
};

SmartRef_Of(Program); // ProgramRef

//////////////////////////////////
// 	Program Library 	//
//////////////////////////////////

class TA_API ProgLibEl: public taNBase {
  // #INSTANCE #INLINE an element in the program library
INHERITED(taNBase)
public:
  bool		is_group;	// this is a group of related programs
  String	desc; 		// #EDIT_DIALOG description of what this program does and when it should be used
  String	URL;		// full URL to find this program
  String	filename;	// file name given to this program
  
  virtual taBase* NewProgram(Program_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT create a new program of this type (return value could be a Program or a Program_Group)

  virtual bool ParseProgFile(const String& fnm, const String& path);
  // get program information from program or program group file. is_group is set based on extension of file name (.prog or .progp)

  override String GetDesc() const { return desc; }

  TA_SIMPLE_BASEFUNS(ProgLibEl);
protected:

private:
  void	Initialize();
  void	Destroy() { CutLinks(); }
};

class TA_API ProgLibEl_List : public taList<ProgLibEl> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE list of program library elements
INHERITED(taList<ProgLibEl>)
public:
  TA_SIMPLE_BASEFUNS(ProgLibEl_List);
protected:
  
private:
  void	Initialize();
  void	Destroy() { Reset(); CutLinks(); }
};

class TA_API ProgLib: public ProgLibEl_List {
  // #INSTANCE #INLINE the program library
INHERITED(ProgLibEl_List)
public:
  String_Array		paths;	// list of paths to search for programs
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
  // ##EXT_progp a collection of programs sharing common global variables and a control panel interface
INHERITED(taGroup<Program>)
public:
  ProgramRef		step_prog; // the program that will be stepped when the Step button is pressed
  String		desc; // #EDIT_DIALOG description of what this program group does and when it should be used (used for searching in prog_lib -- be thorough!)
  ProgVar_List		global_vars; // global vars in all progs in this group and subgroups

  static ProgLib	prog_lib; // #HIDDEN_TREE library of available programs

  virtual bool		CheckConfig(bool quiet=false);	// return false if not properly configured for generating a program

  taBase* NewFromLib(ProgLibEl* prog_type);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_GROUP_prog_lib create a new program from a library of existing program types
  taBase* NewFromLibByName(const String& prog_nm);
  // create a new program from a library of existing program types, looking up by name (NULL if name not found)

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
  ProgArg_List		prog_args; // arguments to the program--copied to prog before call

  virtual void		UpdateGlobalArgs(); 
  // #MENU #MENU_ON_Object #BUTTON called when target changed, or manually by user

  virtual Program*	GetTarget(); // safe call to get target: emits error if target is null

  override bool		CheckConfig(bool quiet=false);
  override String	GetDisplayName() const;

  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgramCall& cp);
  COPY_FUNS(ProgramCall, inherited);
  TA_BASEFUNS(ProgramCall);

protected:
  Program*		old_target; // the last target, used to detect changes
  override void		PreGenMe_impl(int item_id); // register the target as a subprog of this one
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
