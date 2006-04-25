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


class PDP_API ProgVar: public taNBase { // ##NO_TOKENS ##INSTANCE a script variable, accessible from the outer system, and inside the script;\n this class handles simple values like Ints and Strings
INHERITED(taNBase)
public:
  bool			ignore; // don't use this variable
  Variant		value; // the actual variable
  
  virtual int		cssType(); // int value of cssEl::Type generated
  
  virtual void		Freshen(const ProgVar& cp); 
    // updates our value/type information and commensurable fields from compatible type (but not name or ignore)
  virtual const String	GenCss(bool is_arg = false); // css code (terminated if Var);
  
  cssEl*		NewCssEl(); // get a new cssEl of an appropriate type, name/value initialized
  
  void 	SetDefaultName() {} // make it local to list, set by list
  void	UpdateAfterEdit(); // we always nuke the cached cssEl -- it will get regenerated
  void	Copy_(const ProgVar& cp);
  COPY_FUNS(ProgVar, taNBase);
  TA_BASEFUNS(ProgVar);
protected:
  virtual const String	GenCssArg_impl();
  virtual const String	GenCssVar_impl(bool make_new = false, TypeDef* val_type = NULL);
  virtual cssEl*	NewCssEl_impl(); // make a new cssEl of an appropriate type, name/value initialized
private:
  void	Initialize();
  void	Destroy();
};


class PDP_API EnumProgVar: public ProgVar { // a script variable to hold enums
INHERITED(ProgVar)
public:
  TypeDef*		enum_type; // #ENUM_TYPE #TYPE_taBase the type of the enum
  bool			init; // when true, initialize the enum value
  
  override int		cssType(); // int value of cssEl::Type generated
  const String		enumName(); // ex, taBase::Orientation
  
  const String		ValToId(int val);
  
  override void		Freshen(const ProgVar& cp); 
  
  void	Copy_(const EnumProgVar& cp);
  COPY_FUNS(EnumProgVar, ProgVar);
  TA_BASEFUNS(EnumProgVar);
protected:
  override const String	GenCssArg_impl();
  override const String	GenCssVar_impl(bool, TypeDef*);
  override cssEl*	NewCssEl_impl(); 
private:
  void	Initialize();
  void	Destroy();
};

class PDP_API ObjectProgVar: public ProgVar { // a script variable to hold taBase objects
INHERITED(ProgVar)
public:
  TypeDef*		val_type; // #NO_NULL #TYPE_taBase the minimum acceptable type of the value 
  bool			make_new; // #LABEL_new create a new instance
  
  override int		cssType(); // int value of cssEl::Type generated
  
  override void		Freshen(const ProgVar& cp); 
  override const String	GenCss(bool is_arg = false) 
    {return is_arg ? GenCssArg_impl() : GenCssVar_impl(make_new, val_type) ;} // css code (no terminator or newline);
  
  void	Copy_(const ObjectProgVar& cp);
  COPY_FUNS(ObjectProgVar, ProgVar);
  TA_BASEFUNS(ObjectProgVar);
  
protected:
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
  
  VarContext	    	var_context; // #DEF_VC_ProgVars #HIDDEN context of vars, set by owner
  
  virtual const String 	GenCss(int indent_level) const; // generate css script code for the context
  
  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS(ProgVar_List);
  
protected:
  override void	El_SetIndex_(void*, int);
  
private:
  void	Initialize();
  void	Destroy() {Reset();}
};


class PDP_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE definition of a program element
INHERITED(taOBase)
public:
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


class PDP_API LoopEl: public ProgList { 
  // #EDIT_INLINE ProgEl for an iteration over the elements
INHERITED(ProgList)
public:
  String	    	loop_var_type; // the loop variable CSS type
  String	    	loop_var; // the loop variable
  String	    	init_val; // initial value of loop variable
  String	    	loop_test; // the test each time
  String	    	loop_iter; // the iteration operation
  
  void	Copy_(const LoopEl& cp);
  COPY_FUNS(LoopEl, ProgList);
  TA_BASEFUNS(LoopEl);

protected:
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
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
  void	Destroy()	{CutLinks();}
};


class PDP_API MethodCallEl: public ProgEl { 
  // ProgEl for a call to an object method
INHERITED(ProgEl)
public:
  ObjectProgVar*	script_obj; // the script object that has the method
  MethodDef*		method; // the method to call
  
  void	UpdateAfterEdit();
  void	CutLinks();
  void	Copy_(const MethodCallEl& cp);
  COPY_FUNS(MethodCallEl, ProgEl);
  TA_BASEFUNS(MethodCallEl);

protected:
  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API ProgramCallEl: public ProgEl { 
  // ProgEl to invoke another program
INHERITED(ProgEl)
public:
  static const String prfx; // #READ_ONLY the prefix we apply to our names of global vars in target
  
  Program*		target; // the program to be called
  ProgVar_List	global_args; // arguments to the global program--copied to prog before call
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
  // #VIRT_BASE #HIDDEN #NO_INSTANCE a program, with global vars and its own program run space
INHERITED(taNBase)
public:
  ProgVar_List	global_vars; // global variables accessible outside and inside script
  
  bool			isDirty() {return m_dirty;}
  void			setDirty(bool value); // indicates a component has changed
  
  virtual bool		Run(); // run the program
  virtual bool		SetGlobalVar(const String& nm, const Variant& value);
    // set the value of a global variable (in the cssProgSpace) prior to calling Run

#ifdef TA_GUI
public: // XxxGui versions provide feedback to the user
  virtual void		RunGui();
    // #MENU #LABEL_Run #MENU_ON_Actions #MENU_CONTEXT #BUTTON run the script
  void			ViewScript();
    // #MENU #MENU_ON_Actions #MENU_CONTEXT #BUTTON view the script
#endif
    
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Program& cp);
  COPY_FUNS(Program, taNBase);
  TA_ABSTRACT_BASEFUNS(Program);

public: // ScriptBase i/f
  override TypeDef*	GetThisTypeDef() {return GetTypeDef();}
  // #IGNORE
  override void*	GetThisPtr() { return (void*)this; }
  // #IGNORE

protected:
  bool		    	m_dirty;
  String		m_scriptCache; // cache of script, managed by implementation
  virtual void		DirtyChanged_impl() {} // called when m_dirty was changed 
  override void		InitScriptObj_impl();
  override void		PreCompileScript_impl(); // #IGNORE add/update the global vars
  override void 	ScriptCompiled(); // #IGNORE
  virtual void		UpdateProgVars(); // put global vars in script, set values
#ifdef TA_GUI
  virtual void		ViewScript_impl() = 0;
#endif

private:
  void	Initialize();
  void	Destroy();
};

class PDP_API Program_MGroup : public taGroup<Program> {
INHERITED(taGroup<Program>)
public:

  TA_BASEFUNS(Program_MGroup);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); };
};

class PDP_API ProgElProgram: public Program {
  // a program based on ProgEls, with global vars and its own program run space
INHERITED(Program)
public:
  ProgEl_List		prog_els;
  
  override ScriptSource	scriptSource() {return ScriptString;}
  override const String	scriptString();
    
  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ProgElProgram& cp);
  COPY_FUNS(ProgElProgram, Program);
  TA_BASEFUNS(ProgElProgram);
protected:
  override void		ViewScript_impl();

private:
  void	Initialize();
  void	Destroy();
};

class PDP_API FileProgram: public Program {
  // a program defined in a file, with global vars and its own program run space
INHERITED(Program)
public:
  taFiler*	script_file;		// file to use for the script
  
  override ScriptSource	scriptSource();
  override const String	scriptFilename();
    
  void	Copy_(const FileProgram& cp);
  COPY_FUNS(FileProgram, Program);
  TA_BASEFUNS(FileProgram);
protected:
  override void		ViewScript_impl();
private:
  void	Initialize();
  void	Destroy();
};

#endif
