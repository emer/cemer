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

class PDP_API ProgEl: public taOBase {
  // #NO_INSTANCE #VIRT_BASE definition of a program element
INHERITED(taOBase)
public:
  static String	    indent(int indent_level); // generally 2 spaces per level
  
  virtual ProgEl*   parentEl() {return GET_MY_OWNER(ProgEl);}
  virtual String    GenCss(int indent_level = 0); // generate the Css code for this object (usually override _impl's)
  
  TA_ABSTRACT_BASEFUNS(ProgEl);

protected:
  virtual String    GenCssPre_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css prefix code (if any) for this object
  virtual String    GenCssBody_impl(int indent_level) = 0; // #IGNORE generate the Css body code for this object
  virtual String    GenCssPost_impl(int indent_level) {return _nilString;} // #IGNORE generate the Css postfix code (if any) for this object

private:
  void	Initialize();
  void	Destroy()	{}
};

class PDP_API ProgEl_List: public taList<ProgEl> {
INHERITED(taList<ProgEl>)
public:
  virtual String    GenCss(int indent_level = 0); // generate the Css code for this object
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
  override String    GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};


class PDP_API ProgVar: public ProgEl { // #INLINE a program variable, or function argument
INHERITED(ProgEl)
public:
  String	    var_name; // name of the variable
  TypeDef*	    var_type; // #DEF_TA_Variant #NO_NULL type of the variable
  String	    init_val; // (optional) initial value of variable
  
  void	Copy_(const ProgVar& cp);
  COPY_FUNS(ProgVar, ProgEl);
  TA_BASEFUNS(ProgVar);
 
protected:
  override String    GenCssBody_impl(int indent_level); // note: no separator/terminator, ident ignored
 
private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API ProgVar_List: public ProgEl_List {
INHERITED(ProgEl_List)
public:
  enum VarContext {
    VC_ProgVars,  // program variables
    VC_FuncArgs  // function arguments
  };
  
  VarContext	    var_context; // #HIDDEN context of vars, set by owner
  
  override String    GenCss(int indent_level = 0); // generate the Css code for this object
  TA_BASEFUNS(ProgVar_List);

private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API UserScriptEl: public ProgEl { 
  // ProgEl for a user scriptlet
INHERITED(ProgEl)
public:
  String	    user_script; // #EDITOR content of the user scriptlet
  
  void	Copy_(const UserScriptEl& cp);
  COPY_FUNS(UserScriptEl, ProgEl);
  TA_BASEFUNS(UserScriptEl);

protected:
  override String    GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy()	{}
};


class PDP_API LoopEl: public ProgList { 
  // ProgEl for an iteration over the elements
INHERITED(ProgList)
public:
  String	    loop_var; // the loop variable
  String	    init_val; // initial value of loop variable
  String	    loop_test; // the test each time
  String	    loop_iter; // the iteration operation
  
  void	Copy_(const LoopEl& cp);
  COPY_FUNS(LoopEl, ProgList);
  TA_BASEFUNS(LoopEl);

protected:
  override String    GenCssPre_impl(int indent_level); 
  override String    GenCssBody_impl(int indent_level); 
  override String    GenCssPost_impl(int indent_level); 

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
  override String    GenCssPre_impl(int indent_level); 
  override String    GenCssBody_impl(int indent_level); 
  override String    GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};


class PDP_API Program: public ProgList, public ScriptBase {
INHERITED(ProgList)
public:
  String	    name;

  bool 		SetName(const char* nm)    	{return SetName(String(nm));}
  bool 		SetName(const String& nm)    	{ name = nm; return true; }
  String	GetName() const		{ return name; }

  void	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Program& cp);
  COPY_FUNS(Program, ProgList);
  TA_BASEFUNS(Program);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class PDP_API Program_MGroup : public taGroup<Program> {
INHERITED(taGroup<Program>)
public:

  TA_BASEFUNS(Program_MGroup);

private:
  void	Initialize();
  void 	Destroy()		{Reset(); };
};

#endif
