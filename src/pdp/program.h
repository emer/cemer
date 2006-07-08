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

#include "pdp_def.h"
#include "pdp_TA_type.h"

// forwards

class Program;
class ProgramRef;
class Program_MGroup;
class Program_List;

class PDP_API ProgVar: public taNBase { // ##INSTANCE a program variable, accessible from the outer system, and inside the script in .prog_vars;\n This class handles simple atomic values like Ints and Strings
INHERITED(taNBase)
public:
  Variant		value; // the actual variable
  
  virtual int		cssType(); // int value of cssEl::Type generated
  
  virtual const String	GenCss(bool is_arg = false); // css code (terminated if Var);
  
  cssEl*		NewCssEl(); // get a new cssEl of an appropriate type, name/value initialized
  
  void 	SetDefaultName() {} // make it local to list, set by list
  void	UpdateAfterEdit(); // we always nuke the cached cssEl -- it will get regenerated
  void	Copy_(const ProgVar& cp);
  COPY_FUNS(ProgVar, taNBase);
  TA_BASEFUNS(ProgVar);
protected:
  virtual const String	GenCssArg_impl();
  virtual const String	GenCssVar_impl();
  virtual cssEl*	NewCssEl_impl(); // make a new cssEl of an appropriate type, name/value initialized
private:
  void	Initialize();
  void	Destroy();
};


class PDP_API EnumProgVar: public ProgVar { // a program variable to hold enums
INHERITED(ProgVar)
public:
  TypeDef*		enum_type; // #ENUM_TYPE #TYPE_taBase the type of the enum
  bool			init; // when true, initialize the enum value
  
  override int		cssType(); // int value of cssEl::Type generated
  const String		enumName(); // ex, taBase::Orientation
  
  const String		ValToId(int val);
  
  void	Copy_(const EnumProgVar& cp);
  COPY_FUNS(EnumProgVar, ProgVar);
  TA_BASEFUNS(EnumProgVar);
protected:
  override const String	GenCssArg_impl();
  override const String	GenCssVar_impl();
  override cssEl*	NewCssEl_impl(); 
private:
  void	Initialize();
  void	Destroy();
};

class PDP_API ObjectProgVar: public ProgVar { // ##SCOPE_ProgElProgram a program variable to hold taBase objects
INHERITED(ProgVar)
public:
  TypeDef*		val_type; // #NO_NULL #TYPE_taBase the minimum acceptable type of the value 
  
  override int		cssType(); // int value of cssEl::Type generated
  
  void	Copy_(const ObjectProgVar& cp);
  COPY_FUNS(ObjectProgVar, ProgVar);
  TA_BASEFUNS(ObjectProgVar);
  
protected:
  override const String	GenCssArg_impl();
  override const String	GenCssVar_impl();
  override cssEl*	NewCssEl_impl(); 
  
private:
  void	Initialize();
  void	Destroy();
};

class PDP_API ProgVar_List : public taList<ProgVar> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE list of script variables
INHERITED(taList<ProgVar>)
public:
  enum VarContext {
    VC_ProgVars,  // #LABEL_ProgramVariables program variables
    VC_FuncArgs  //  #LABEL_FunctionArguments function arguments
  };
  
  VarContext	    	var_context; // #DEF_VC_ProgVars #HIDDEN #NO_SAVE context of vars, set by owner
  
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


class PDP_API ProgArg: public taOBase { // ##NO_TOKENS ##INSTANCE a program or method argument (NOTE: v3.9 preliminary version)
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


class PDP_API ProgArg_List : public taList<ProgArg> {
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


class PDP_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE definition of a program element
INHERITED(taOBase)
public:
  String		desc; // optional brief description of element's function; included as comment in script
  virtual ProgEl*   parent() {return GET_MY_OWNER(ProgEl);}
  
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object (usually override _impl's)
  
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  void	ChildUpdateAfterEdit(TAPtr child, bool& handled); // detect children of our subclasses changing
  TA_ABSTRACT_BASEFUNS(ProgEl);

protected:
  virtual const String    GenCssPre_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css prefix code (if any) for this object	
  virtual const String    GenCssBody_impl(int indent_level) = 0; // #IGNORE generate the Css body code for this object
  virtual const String    GenCssPost_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css postfix code (if any) for this object

private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API ProgEl_List: public taList<ProgEl> {
INHERITED(taList<ProgEl>)
public:
  virtual const String    GenCss(int indent_level = 0); // generate the Css code for this object
  
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS(ProgEl_List);

private:
  void	Initialize() {SetBaseType(&TA_ProgEl);}
  void	Destroy()	{Reset();}
};


class PDP_API ProgList: public ProgEl { 
  // list of ProgEl's, each executed in sequence
INHERITED(ProgEl)
public:
  ProgEl_List	    prog_els; // list of ProgEl's
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgList& cp);
  COPY_FUNS(ProgList, ProgEl);
  TA_BASEFUNS(ProgList);

protected:
  override const String    GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};


class PDP_API ProgVars: public ProgEl {
INHERITED(ProgEl)
public:
  ProgVar_List	script_vars;
  
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(ProgVars);

protected:
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy();
};


class PDP_API UserScriptEl: public ProgEl { 
  // ProgEl for a user scriptlet
INHERITED(ProgEl)
public:
  String	    user_script; // #EDIT_DIALOG content of the user scriptlet
  
  void	Copy_(const UserScriptEl& cp);
  COPY_FUNS(UserScriptEl, ProgEl);
  TA_BASEFUNS(UserScriptEl);

protected:
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API LoopEl: public ProgEl { 
  // #VIRT_BASE #EDIT_INLINE #NO_TOKENS ProgEl base for loops
INHERITED(ProgEl)
public:
  ProgEl_List		loop_els; // #BROWSE the items to execute in the loop
  String	    	loop_var_type; // the loop variable CSS type to create, or blank if exists
  String	    	loop_var; // the loop variable
  String	    	init_val; // initial value of loop variable. blank if default or none
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LoopEl& cp);
  COPY_FUNS(LoopEl, ProgEl);
  TA_ABSTRACT_BASEFUNS(LoopEl);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{CutLinks();}
};


class PDP_API ForLoopEl: public LoopEl { 
  // #EDIT_INLINE #TOKENS LoopEl for an iteration over the elements
INHERITED(LoopEl)
public:
  String	    	loop_test; // the test each time
  String	    	loop_iter; // the iteration operation
  
  void	Copy_(const ForLoopEl& cp);
  COPY_FUNS(ForLoopEl, LoopEl);
  TA_BASEFUNS(ForLoopEl);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API WhileLoopEl: public LoopEl { 
  // #EDIT_INLINE #TOKENS LoopEl for a 'while' (pre-test) iteration over the elements
INHERITED(LoopEl)
public:
  
  TA_BASEFUNS(WhileLoopEl);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{}
};


class PDP_API UntilLoopEl: public LoopEl { 
  // #EDIT_INLINE LoopEl for a 'while' (pre-test) iteration over the elements
INHERITED(LoopEl)
public:
  
  TA_BASEFUNS(UntilLoopEl);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize() {}
  void	Destroy()	{}
};


class PDP_API CondEl: public ProgEl { 
  // ProgEl for a user scriptlet
INHERITED(ProgEl)
public:
  String	    cond_test; // condition test
  ProgEl_List	    true_els; // #BROWSE items to execute if condition true
  ProgEl_List	    false_els; // #BROWSE items to execute if condition false
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const CondEl& cp);
  COPY_FUNS(CondEl, ProgEl);
  TA_BASEFUNS(CondEl);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};


class PDP_API MethodSpec: public taOBase { 
  // #EDIT_INLINE #HIDDEN #NO_TOKENS helper obj for MethodCallEl; has custom taiData
INHERITED(taOBase)
public:
  ObjectProgVar*	script_obj; // #SCOPE_ProgElProgram the previously defined script object that has the method
  TypeDef*		var_type; // #NO_SHOW #NO_SAVE temp copy of script_obj.var_type
  MethodDef*		method; //  #TYPE_ON_var_type the method to call
  
  void	UpdateAfterEdit();
  void	CutLinks();
  void	Copy_(const MethodSpec& cp);
  COPY_FUNS(MethodSpec, taOBase);
  TA_BASEFUNS(MethodSpec);
  
private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class PDP_API MethodCallEl: public ProgEl { 
  // ProgEl for a call to an object method
INHERITED(ProgEl)
friend class MethodSpec;
public:
  String		result_var; // result variable (optional)
  MethodSpec		method_spec; //  the method to call
  SArg_Array		args; // arguments to the method
  
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const MethodCallEl& cp);
  COPY_FUNS(MethodCallEl, ProgEl);
  TA_BASEFUNS(MethodCallEl);

protected:
  ObjectProgVar*	lst_script_obj; 
  MethodDef*		lst_method; 
  
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object
  virtual void		CheckUpdateArgs(bool force = false); // called when method changes

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 


class PDP_API ProgramCallEl: public ProgEl { 
  // ProgEl to invoke another program
INHERITED(ProgEl)
public:
  Program*		target; // the program to be called
  ProgArg_List		prog_args; // arguments to the program--copied to prog before call
  UserScriptEl		fail_el; // #EDIT_INLINE what to do if can't compile or run--default is cerr and Stop
  
  virtual void		UpdateGlobalArgs(); 
    // #MENU #MENU_ON_Object #BUTTON called when target changed, or manually by user
  
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgramCallEl& cp);
  COPY_FUNS(ProgramCallEl, ProgEl);
  TA_BASEFUNS(ProgramCallEl);

protected:
  Program*		old_target; // the last target, used to detect changes
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 
private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API Program: public taNBase, public AbstractScriptBase {
  // #TOKENS #INSTANCE a program, with global vars and its own program run space
INHERITED(taNBase)
public:
  enum ProgFlags { // #BITS mode flags
    PF_NONE		= 0, // #NO_BIT
    ROOT_OK		= 0x0001 // allowed to be a root program (can be called by user)
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
    DONE = 0, 	// there is no program running or stopped
    INIT,	// tells the prog to reset its state to the beginning; this is a "running" state
    STOP,	// the program is stopped (note: NOT the same as "DONE")
    RUN,	// normal running state; this is a "running" state
    STEP	// state when we are executing a Step; this is a "running" state
  };
  
  static RunState	run_state; // the one and only global run mode for current running prog
  static ProgramRef	top_prog; // the top level program that was run
  static ProgramRef	step_prog; // #SHOW the step prog (NULL if not stepping)
  
  ProgFlags		flags;  // control flags, for display and execution control
  taBase_List		prog_objs; // sundry objects that are used in this program
  ProgVar_List		param_vars; // global variables that are parameters for callers
  ProgVar_List		global_vars; // global variables accessible outside and inside script
  ProgEl_List		init_els; // the prog els for initialization (done once); use a "return" if an error occurs 
  ProgEl_List		prog_els; // the prog els for the main program
  
  int			ret_val; // #HIDDEN #IV_READ_ONLY #NO_SAVE return value: 0=ok, +ve=sys-defined err, -ve=user-defined err; also accessible inside program
  
  bool			isDirty() {return m_dirty;}
  void			setDirty(bool value); // indicates a component has changed
  inline bool		rootOk() {return (flags & ROOT_OK);}
  void			setRunState(RunState value); // sets and updates gui
  override ScriptSource	scriptSource() {return ScriptString;}
  override const String	scriptString();
  
  virtual void  Init();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP set the program state back to the beginning
  virtual void  Run();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP run the programs
  virtual void	Step();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP step the program, at the selected step level
  virtual void	Stop();
  // #BUTTON #GHOST_OFF_run_state:INIT,RUN,STEP stop the running programs
  
  virtual void	CmdShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP set css command shell to operate on this program, so you can run, debug, etc this script from the command line
  virtual void	ExitShell();
  // #BUTTON #GHOST_OFF_run_state:DONE,STOP exit the command shell for this program (shell returns to previous script)

 //TODO: the button functions are gui functions, with failure dialogs, etc
 // need purely nongui ones for scripted calls
  
  int			Call(Program* caller); 
    // runs the program as a subprogram called from another running program, 0=success
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
  bool		    	m_dirty;
  String		m_scriptCache; // cache of script, managed by implementation
  virtual void		DirtyChanged_impl() {} // called when m_dirty was changed 
  override void		InitScriptObj_impl();
  override void		PreCompileScript_impl(); // #IGNORE add/update the global vars
  virtual int		Run_impl(); 
    // run in the current mode, returning the ReturnVal (0=success); 
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


class PDP_API Program_MGroup : public taGroup<Program> {
INHERITED(taGroup<Program>)
public:
  ProgVar_List		global_vars; // global vars in all progs in this group and subgroups

  void		SetProgsDirty(); // set all progs in this group/subgroup to be dirty
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Program_MGroup& cp);
  COPY_FUNS(Program_MGroup, taGroup<Program>)
  TA_BASEFUNS(Program_MGroup);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); };
};


class PDP_API Program_List : public taList<Program> {
INHERITED(taList<Program>)
public:
  
  TA_BASEFUNS(Program_List);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); }; //
}; //

#endif
