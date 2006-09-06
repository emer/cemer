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

#include "tamisc_def.h"
#include "tamisc_TA_type.h"

// forwards

class Program;
class ProgramRef;
class Program_Group;
class Program_List;

class TAMISC_API ProgVar: public taNBase {
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
  taBase*	object_val;	// #CONDEDIT_ON_var_type:T_Object #TYPE_ON_object_type object pointer value
  TypeDef*	hard_enum_type;	// #CONDEDIT_ON_var_type:T_HardEnum #ENUM_TYPE #TYPE_taBase type information for hard enum (value goes in int_val)
  DynEnum	dyn_enum_val;	// #CONDEDIT_ON_var_type:T_DynEnum type information for dynamic enum (value goes in int_val)
  
  TypeDef*		act_object_type() const; // the actual object type; never NULL (taBase min)
  
  void			Cleanup(); // #IGNORE we call this after changing value, to cleanup unused
  virtual const String	GenCssType(); // type name
  virtual const String	GenCssInitVal(); // intial value

  virtual const String	GenCss(bool is_arg = false); // css code (terminated if Var);
  
  virtual cssEl*	NewCssEl();
  // get a new cssEl of an appropriate type, name/value initialized
  virtual cssEl*	NewCssType();
  // if object defines new type information (dyn_enum), generate a type object
  
  override bool		Dump_QuerySaveMember(MemberDef* md); // don't save the unused vals
  override void		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void 	SetDefaultName() {} // make it local to list, set by list
  void	UpdateAfterEdit(); // we always nuke the cached cssEl -- it will get regenerated
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgVar& cp);
  COPY_FUNS(ProgVar, taNBase);
  TA_BASEFUNS(ProgVar);
protected:
  virtual const String	GenCssArg_impl();
  virtual const String	GenCssVar_impl();
private:
  void	Initialize();
  void	Destroy();
};

class TAMISC_API ProgVar_List : public taList<ProgVar> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE list of script variables
INHERITED(taList<ProgVar>)
public:
  enum VarContext {
    VC_ProgVars,  // #LABEL_ProgramVariables program variables
    VC_FuncArgs  //  #LABEL_FunctionArguments function arguments
  };
  
  VarContext	var_context; // #DEF_VC_ProgVars #HIDDEN #NO_SAVE context of vars, set by owner
  
  virtual const String 	GenCss(int indent_level) const; // generate css script code for the context
  
  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	Copy_(const ProgVar_List& cp);
  COPY_FUNS(ProgVar_List, taList<ProgVar>);
  TA_BASEFUNS(ProgVar_List);
  
protected:
  override void	El_SetIndex_(void*, int);
  
private:
  void	Initialize();
  void	Destroy() {Reset();}
};

SmartRef_Of(ProgVar); // ProgVarRef

class TAMISC_API ProgArg: public taOBase {
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
  COPY_FUNS(ProgArg, taOBase);
  TA_BASEFUNS(ProgArg);
private:
  void	Initialize();
  void	Destroy();
};


class TAMISC_API ProgArg_List : public taList<ProgArg> {
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


class TAMISC_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE definition of a program element
INHERITED(taOBase)
public:
  String		desc; // #HIDDEN_INLINE optional brief description of element's function; included as comment in script
  bool			off;	// #DEF_false turn off this program element: do not include in script

  virtual ProgEl*   	parent() {return GET_MY_OWNER(ProgEl);}
  Program*		program() {return GET_MY_OWNER(Program);} 
  
  void			PreGen(int& item_id); //recursive walk of items before code gen; each item bumps its id and calls subitems; esp. used to discover subprogs in order
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object (usually override _impl's)
  
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	ChildUpdateAfterEdit(TAPtr child, bool& handled); // detect children of our subclasses changing

  void	Copy_(const ProgEl& cp);
  COPY_FUNS(ProgEl, taOBase);
  TA_BASEFUNS(ProgEl);

protected:
  virtual void		PreGenMe_impl(int item_id) {}
  virtual void		PreGenChildren_impl(int& item_id) {}
  virtual const String	GenCssPre_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css prefix code (if any) for this object	
  virtual const String	GenCssBody_impl(int indent_level) {} // #IGNORE generate the Css body code for this object
  virtual const String	GenCssPost_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css postfix code (if any) for this object

private:
  void	Initialize();
  void	Destroy();
};


class TAMISC_API ProgEl_List: public taList<ProgEl> {
INHERITED(taList<ProgEl>)
public:
  virtual void		PreGen(int& item_id); // iterates over all items
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object
  
  override int		NumListCols() const {return 2;} 
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS(ProgEl_List);

private:
  void	Initialize();
  void	Destroy();
};


class TAMISC_API ProgList: public ProgEl { 
  // list of ProgEl's, each executed in sequence
INHERITED(ProgEl)
public:
  ProgEl_List	    	prog_code; // list of ProgEl's
  
  override String	GetDisplayName() const;
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgList& cp);
  COPY_FUNS(ProgList, ProgEl);
  TA_BASEFUNS(ProgList);

protected:
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};


class TAMISC_API ProgVars: public ProgEl {
INHERITED(ProgEl)
public:
  ProgVar_List	script_vars;
  
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


class TAMISC_API UserScript: public ProgEl { 
  // ProgEl for a user scriptlet
INHERITED(ProgEl)
public:
  String	    user_script; // #EDIT_DIALOG content of the user scriptlet
  
  override String	GetDisplayName() const;
  void	Copy_(const UserScript& cp);
  COPY_FUNS(UserScript, ProgEl);
  TA_BASEFUNS(UserScript);

protected:
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{}
};


class TAMISC_API Loop: public ProgEl { 
  // #VIRT_BASE #EDIT_INLINE #NO_INSTANCE ProgEl base for loops
INHERITED(ProgEl)
public:
  ProgEl_List		loop_code; // #BROWSE the items to execute in the loop
  String	    	loop_var_type; // the loop variable CSS type to create, or blank if exists
  String	    	loop_var; // the loop variable
  String	    	init_val; // initial value of loop variable. blank if default or none
  
  override String	GetDisplayName() const;
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Loop& cp);
  COPY_FUNS(Loop, ProgEl);
  TA_ABSTRACT_BASEFUNS(Loop);

protected:
  virtual const String 	loopHeader(bool display = false) const = 0; 
    // common subcode, we use it as the DisplayName
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{CutLinks();}
};


class TAMISC_API ForLoop: public Loop { 
  // #EDIT_INLINE #TOKENS Loop for an iteration over the elements
INHERITED(Loop)
public:
  String	    	loop_test; // the test each time
  String	    	loop_iter; // the iteration operation
  
  void	Copy_(const ForLoop& cp);
  COPY_FUNS(ForLoop, Loop);
  TA_BASEFUNS(ForLoop);

protected:
  override const String loopHeader(bool display = false) const;
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{}
};


class TAMISC_API WhileLoop: public Loop { 
  // #EDIT_INLINE #TOKENS Loop for a 'while' (pre-test) iteration over the elements
INHERITED(Loop)
public:
  
  TA_BASEFUNS(WhileLoop);

protected:
  override const String loopHeader(bool display = false) const;
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{}
};


class TAMISC_API DoLoop: public Loop { 
  // #EDIT_INLINE Loop for a 'do' (post-test) iteration over the elements
INHERITED(Loop)
public:
  
  TA_BASEFUNS(DoLoop);

protected:
  override const String loopHeader(bool display = false) const;
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{}
};

// todo: how to handle sub-sequences?

class TAMISC_API BasicDataLoop: public Loop { 
  // #EDIT_INLINE #TOKENS loops over items in a DataTable, in different basic orderings
INHERITED(Loop)
public:
  enum Order {
    SEQUENTIAL,			// present events in sequential order
    PERMUTED,			// permute the order of event presentation
    RANDOM 			// pick an event at random (with replacement)
  };

  ProgVarRef	data_var;	// program variable pointing to the data table to use
  Order		order;		// order to process data items in
  int		cur_item_idx;	// index of current item in the data table
  int_Array	item_idx_list;	// #HIDDEN list of item indicies 

  TA_SIMPLE_BASEFUNS(BasicDataLoop);

protected:
  override const String loopHeader(bool display = false) const;
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy();
};


class TAMISC_API IfElse: public ProgEl { 
  // ProgEl for a conditional test element
INHERITED(ProgEl)
public:
  String	    cond_test; // condition test
  ProgEl_List	    true_code; // #BROWSE items to execute if condition true
  ProgEl_List	    false_code; // #BROWSE items to execute if condition false
  
  override String	GetDisplayName() const;
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const IfElse& cp);
  COPY_FUNS(IfElse, ProgEl);
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


class TAMISC_API MethodSpec: public taOBase { 
  // #EDIT_INLINE #HIDDEN #NO_TOKENS helper obj for MethodCall; has custom taiData
INHERITED(taOBase)
public:
  ProgVar*		script_obj; // #SCOPE_Program_Group the previously defined script object that has the method
  TypeDef*		object_type; // #NO_SHOW #NO_SAVE temp copy of script_obj.object_type
  MethodDef*		method; //  #TYPE_ON_object_type the method to call
  
  void	UpdateAfterEdit();
  void	CutLinks();
  void	Copy_(const MethodSpec& cp);
  COPY_FUNS(MethodSpec, taOBase);
  TA_BASEFUNS(MethodSpec);
  
private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TAMISC_API MethodCall: public ProgEl { 
  // ProgEl for a call to an object method
INHERITED(ProgEl)
friend class MethodSpec;
public:
  String		result_var; // result variable (optional)
  MethodSpec		method_spec; //  the method to call
  SArg_Array		args; // arguments to the method
  
  override String	GetDisplayName() const;
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const MethodCall& cp);
  COPY_FUNS(MethodCall, ProgEl);
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

class TAMISC_API Program_List : public taList<Program> {
INHERITED(taList<Program>)
public:
  
  TA_BASEFUNS(Program_List);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); }; //
}; //


class TAMISC_API Program: public taNBase, public AbstractScriptBase {
  // #TOKENS #INSTANCE a program, with global vars and its own program run space
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
  
  static RunState	run_state; // the one and only global run mode for current running prog
  static ProgramRef	top_prog; // the top level program that was run
  static ProgramRef	step_prog; // #SHOW the step prog (NULL if not stepping)
  
  ProgFlags		flags;  // control flags, for display and execution control
  taBase_List		objs; // #AKA_prog_objs sundry objects that are used in this program
  ProgVar_List		args; // #AKA_param_vars global variables that are parameters (arguments) for callers
  ProgVar_List		vars; // #AKA_prog_vars global variables accessible outside and inside script
  ProgEl_List		init_code; // the prog els for initialization (done once); use a "return" if an error occurs 
  ProgEl_List		prog_code; // the prog els for the main program
  
  int			ret_val; // #HIDDEN #IV_READ_ONLY #NO_SAVE return value: 0=ok, +ve=sys-defined err, -ve=user-defined err; also accessible inside program
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

 //TODO: the button functions are gui functions, with failure dialogs, etc
 // need purely nongui ones for scripted calls
  
  int			Call(Program* caller); 
    // runs the program as a subprogram called from another running program, 0=success
  int			CallInit(Program* caller); 
    // runs the program's Init from a superProg Init, 0=success
  virtual bool		SetGlobalVar(const String& nm, const Variant& value);
    // set the value of a global variable (in the cssProgSpace) prior to calling Run
  bool			StopCheck(); // calls event loop, then checks for STOP state, true if so

#ifdef TA_GUI
public: // XxxGui versions provide feedback to the usbool no_gui = falseer
  void			ViewScript();
    // #MENU #MENU_ON_Actions #MENU_CONTEXT #BUTTON view the script
#endif
    
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Program& cp);
  COPY_FUNS(Program, taNBase);
  TA_BASEFUNS(Program);

public: // ScriptBase i/f
  override TypeDef*	GetThisTypeDef() const {return GetTypeDef();}
  // #IGNORE
  override void*	GetThisPtr() { return (void*)this; }
  // #IGNORE

protected:
  String		m_scriptCache; // cache of script, managed by implementation
  virtual void		DirtyChanged_impl() {} // called when m_dirty was changed 
  override void		InitScriptObj_impl();
  override void		PreCompileScript_impl(); // #IGNORE add/update the global vars
  virtual void		Stop_impl(); 
  virtual int		Run_impl(); 
  virtual int		Cont_impl(); 
  override void 	ScriptCompiled(); // #IGNORE
  virtual void		UpdateProgVars(); // put global vars in script, set values
#ifdef TA_GUI
  virtual void		ViewScript_impl();
#endif

private:
  void	Initialize();
  void	Destroy();
};

SmartRef_Of(Program); // ProgramRef

class TAMISC_API Program_Group : public taGroup<Program> {
INHERITED(taGroup<Program>)
public:
  ProgVar_List		global_vars; // global vars in all progs in this group and subgroups

  void		SetProgsDirty(); // set all progs in this group/subgroup to be dirty
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Program_Group& cp);
  COPY_FUNS(Program_Group, taGroup<Program>)
  TA_BASEFUNS(Program_Group);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); };
};

class TAMISC_API ProgramCall: public ProgEl { 
  // ProgEl to invoke another program
  INHERITED(ProgEl)
public:
  ProgramRef		target; // the program to be called
  ProgArg_List		prog_args; // arguments to the program--copied to prog before call

  virtual void		UpdateGlobalArgs(); 
  // #MENU #MENU_ON_Object #BUTTON called when target changed, or manually by user

  virtual Program*	GetTarget(); // 


  override String	GetDisplayName() const;

  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgramCall& cp);
  COPY_FUNS(ProgramCall, ProgEl);
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

#endif
