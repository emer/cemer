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


class PDP_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE definition of a program element
INHERITED(taOBase)
public:
  virtual ProgEl*   parent() {return GET_MY_OWNER(ProgEl);}
  
  virtual const String	GenCss(int indent_level = 0); // generate the Css code for this object (usually override _impl's)
  
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
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
  ScriptVar_List	script_vars;
  
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


class PDP_API MethodCallEl: public ProgEl { 
  // ProgEl for a call to an object method
INHERITED(ProgEl)
public:
  ObjectScriptVar*	script_obj; // the script object that has the method
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


class PDP_API LoopEl: public ProgList { 
  // ProgEl for an iteration over the elements
INHERITED(ProgList)
public:
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


class PDP_API ProgramCallEl: public ProgEl { 
  // ProgEl to invoke another program
INHERITED(ProgEl)
public:
  Program*		target; // the program to be called
  ScriptVar_List	global_args; // arguments to the global program--copied to prog before call
  UserScriptEl		fail_el; // #EDIT_INLINE what to do if can't compile or run--default is cerr and Stop
  
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
  virtual void		UpdateGlobalArgs(); // called when target changed
private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API Program: public taNBase, public AbstractScriptBase {
  // #VIRT_BASE #HIDDEN #NO_INSTANCE a program, with global vars and its own program run space
INHERITED(taNBase)
public:
  ScriptVar_List	global_vars; // global variables accessible outside and inside script
  
  bool			isDirty() {return m_dirty;}
  void			setDirty(bool value); // indicates a component has changed
  
  virtual bool		Run(); // run the program

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
  int			m_our_hardvar_base_index; // each time we recompile, we reinstall our vars
  virtual void		DirtyChanged_impl() {} // called when m_dirty was changed 
  override void		InitScriptObj_impl();
  override void		PreCompileScript_impl(); // #IGNORE add/update the global vars
  override void 	ScriptCompiled(); // #IGNORE
  virtual void		UpdateScriptVars(); // put global vars in script, set values
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
