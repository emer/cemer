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

// ta_program_els.h -- gui-based executables -- specific program elements

#ifndef TA_PROGRAM_ELS_H
#define TA_PROGRAM_ELS_H

#include "ta_program.h"

class TA_API CodeBlock: public ProgEl { 
  // a block of code (list of program elements), each executed in sequence
INHERITED(ProgEl)
public:
  ProgEl_List	    	prog_code; // list of Program elements: the block of code

 virtual ProgEl*	AddProgCode(TypeDef* el_type)	{ return (ProgEl*)prog_code.New(1, el_type); }
  // #BUTTON #TYPE_ProgEl add a new program code element

//no  override taList_impl*	children_() {return &prog_code;}	
  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(CodeBlock);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level);
  override const String	GenCssPost_impl(int indent_level); 
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};

class TA_API ProgVars: public ProgEl {
  // ##DEF_CHILD_local_vars local program variables -- these variables do NOT update their values as shown here -- they exist only as script variables (unlike global args and vars)
INHERITED(ProgEl)
public:
  ProgVar_List		local_vars;	// the list of variables -- these variables do NOT update their values as shown here -- they exist only as script variables (unlike global args and vars)
  
 virtual ProgVar*	AddVar()	{ return (ProgVar*)local_vars.New(1); }
  // #BUTTON add a new variable

  override ProgVar*	FindVarName(const String& var_nm) const;

  override taList_impl*	children_() {return &local_vars;}
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS(ProgVars);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy();
};


class TA_API UserScript: public ProgEl { 
  // a user-defined css script (can access all program variables, etc)
INHERITED(ProgEl)
public:
  ProgExpr		script;	// the css (C++ syntax) code to be executed

  virtual void	    	ImportFromFile(istream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css import script from file
  virtual void	    	ImportFromFileName(const String& fnm); // import script from file
  virtual void	    	ExportToFile(ostream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css export script to file
  virtual void	    	ExportToFileName(const String& fnm); // export script to file
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(UserScript);
protected:
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{}
};

class TA_API WhileLoop: public Loop { 
  // Repeat loop_code while loop_test expression is true (test first): while(loop_test) do loop_code
INHERITED(Loop)
public:
  ProgExpr		test; // a test expression for whether to continue looping (e.g., 'i < max')
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(WhileLoop);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
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
  ProgExpr		test; // a test expression for whether to continue looping (e.g., 'i < max')
  
  override String	GetDisplayName() const;
  TA_SIMPLE_BASEFUNS(DoLoop);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
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
  ProgExprBase	    	init; // initialization expression (e.g., declare the loop variable; 'int i')
  ProgExprBase		test; // a test expression for whether to continue looping (e.g., 'i < max')
  ProgExprBase	    	iter; // the iteration operation run after each loop (e.g., increment the loop variable; 'i++')

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
  ProgExpr		cond; 		// conditionalizing expression for continuing loop

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

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
  ProgExpr		cond; 		// conditionalizing expression for breaking loop

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  TA_SIMPLE_BASEFUNS(IfBreak);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API IfReturn: public ProgEl { 
  // if condition is true, return (from void function or stop further execution of code or init segments of Program)
INHERITED(ProgEl)
public:
  ProgExpr		cond; 		// conditionalizing expression for returning

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  TA_SIMPLE_BASEFUNS(IfReturn);
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
  ProgExpr	    cond; 	// condition expression to test for true or false
  ProgEl_List	    true_code; 	// #SHOW_TREE items to execute if condition true
  ProgEl_List	    false_code; // #SHOW_TREE items to execute if condition false

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  TA_SIMPLE_BASEFUNS(IfElse);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

class TA_API Switch: public ProgEl { 
  // switches execution based on the value of given variable -- each case expression is matched to a corresponding case_code item one-to-one
INHERITED(ProgEl)
public:
  ProgVarRef	    switch_var;	// variable to switch on
  ProgExpr_List	    case_exprs;	// #SHOW_TREE expressions for the different cases to test for
  ProgEl_List	    case_code; 	// #SHOW_TREE code to execute for each case, in one-to-one correspondence with the case_exprs (automatically filled with CodeBlocks)

  virtual void	    NewCase() 	{ case_exprs.New(1); UpdateAfterEdit(); }
  // #BUTTON make a new case item
  virtual void	    CasesFmEnum();
  // #BUTTON #CONFIRM add all the cases for an enumerated type (switch_var must be either HARD_ENUM or DYN_ENUM)

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(Switch, Program);
protected:
  override void		UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level); 
  override const String	GenCssPost_impl(int indent_level); 
  override const String	GenListing_children(int indent_level);

  virtual void	    CasesFmEnum_hard(); // switch_var is a hard enum
  virtual void	    CasesFmEnum_dyn();	// switch_var is a dynamic enum


private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

class TA_API AssignExpr: public ProgEl { 
  // assign an expression to a variable (use method call for simple assignment to function call)
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // where to store the result of the expression (the variable)
  ProgExpr		expr;
  // expression to assign to variable
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(AssignExpr, Program);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API VarIncr: public ProgEl { 
  // increment a variable's value by given amount
INHERITED(ProgEl)
public:
  ProgVarRef		var;
  // variable to increment
  ProgExpr		expr;
  // expression for how much to add to variable (use a negative sign to decrement)
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(VarIncr, Program);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MethodCall: public ProgEl { 
  // ##DEF_CHILD_meth_args call a method (member function) on an object
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // where to store the result of the method call (optional -- can be NULL)
  ProgVarRef		obj;
  // #APPLY_IMMED program variable that points to the object with the method to call
  TypeDef*		obj_type;
  // #NO_SHOW #NO_SAVE temp copy of obj.object_type
  MethodDef*		method;
  // #TYPE_ON_obj_type #APPLY_IMMED the method to call on object obj
  ProgArg_List		meth_args;
  // #SHOW_TREE arguments to be passed to the method

  override taList_impl*	children_() {return &meth_args;}	
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(MethodCall, Program);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MemberAssign: public ProgEl { 
  //  set a member (attribute) on an object
INHERITED(ProgEl)
public:
  ProgVarRef		obj;
  // #APPLY_IMMED program variable that points to the object with the method to call
  TypeDef*		obj_type;
  // #NO_SHOW #NO_SAVE temp copy of obj.object_type
  MemberDef*		member;
  // #TYPE_ON_obj_type #APPLY_IMMED the member to assign on object obj
  ProgExpr		expr; // the expression to compute and assign to the member
  bool			update_after; // call UpdateAfterEdit after setting the member
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(MemberAssign, Program);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

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
  // print out (to the console) the value of a variable -- useful for debugging
INHERITED(ProgEl)
public:
  ProgVarRef		print_var; 	// print out (to console) the value of this variable
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(PrintVar, Program);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API PrintExpr: public ProgEl { 
  // print out (to the console) an expression -- e.g., an informational message for the user
INHERITED(ProgEl)
public:
  ProgExpr		expr;
  // print out (to console) this expression -- it just does 'cerr << expr << endl;' so you can put multiple << segments in the expression to print out multiple things
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  TA_SIMPLE_BASEFUNS(PrintExpr);

protected:
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API Comment: public ProgEl { 
  // insert a highlighted (possibly) multi-line comment -- useful for describing an upcoming chunk of code
INHERITED(ProgEl)
public:
  override String	GetDisplayName() const;
  override String	GetTypeDecoKey() const { return "Comment"; }

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
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }
  TA_SIMPLE_BASEFUNS(StopStepPoint);

protected:
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API ReturnExpr: public ProgEl { 
  // return from a function with a given expression (can be empty to return from a void function) -- you can return from the code or init segments of a program to end execution at that point
INHERITED(ProgEl)
public:
  ProgExpr		expr;
  // expression to return from function with (can be empty to return from a void function)
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }
  TA_SIMPLE_BASEFUNS(ReturnExpr);

protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif
