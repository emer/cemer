// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
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

  PROGEL_SIMPLE_BASEFUNS(CodeBlock);
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
  // ##DEF_CHILD_local_vars local program variables -- these variables do NOT use or update the values that are shown -- they exist only as script variables (unlike global args and vars)
INHERITED(ProgEl)
public:
  ProgVar_List		local_vars;	// the list of variables -- these variables do NOT update their values as shown here -- they exist only as script variables (unlike global args and vars)
  
 virtual ProgVar*	AddVar()	{ return (ProgVar*)local_vars.New(1); }
  // #BUTTON add a new variable

  override ProgVar*	FindVarName(const String& var_nm) const;

  override taList_impl*	children_() {return &local_vars;}
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  PROGEL_SIMPLE_BASEFUNS(ProgVars);
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
  
  override void		SetProgExprFlags();
  override String	GetDisplayName() const;
  PROGEL_SIMPLE_BASEFUNS(UserScript);
protected:
  override void UpdateAfterEdit_impl();
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
  PROGEL_SIMPLE_BASEFUNS(WhileLoop);
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
  PROGEL_SIMPLE_BASEFUNS(DoLoop);
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
  ProgExprBase	    	init; // initialization expression (e.g., 'i=0' -- can also declare a new variable, but you won't be able to access it in other program code)
  ProgExprBase		test; // a test expression for whether to continue looping (e.g., 'i < max')
  ProgExprBase	    	iter; // the iteration operation run after each loop (e.g., increment the loop variable; 'i++')

  override String	GetDisplayName() const;
  override void		SetProgExprFlags();

  PROGEL_SIMPLE_BASEFUNS(ForLoop);
protected:
  virtual void	GetIndexVar(); // make default 'i' variable in program.vars -- just makes it easier to deal with loops in default case..

  override void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
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

  PROGEL_SIMPLE_BASEFUNS(IfContinue);
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

  PROGEL_SIMPLE_BASEFUNS(IfBreak);
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

  PROGEL_SIMPLE_BASEFUNS(IfReturn);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API If: public ProgEl { 
  // a conditional test element: if(condition) then true_code
INHERITED(ProgEl)
public:
  ProgExpr	    cond; 	// condition expression to test for true or false
  ProgEl_List	    true_code; 	// #SHOW_TREE items to execute if condition true

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  PROGEL_SIMPLE_BASEFUNS(If);
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

class TA_API IfElse: public If { 
  // a conditional test element: if(condition) then true_code; else false_code
INHERITED(If)
public:
  ProgEl_List	    	false_code; // #SHOW_TREE items to execute if condition false

  override ProgVar*	FindVarName(const String& var_nm) const;
  
  PROGEL_SIMPLE_BASEFUNS(IfElse);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override const String	GenCssBody_impl(int indent_level); //replaces If
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

class TA_API IfGuiPrompt: public ProgEl { 
  // if in gui mode, prompt user prior to performing a given operation -- if user says OK then run the code, otherwise do nothing -- if not in gui mode (e.g., running in batch mode) then always run the code -- allows interactive control over otherwise default operations
INHERITED(ProgEl)
public:
  String	prompt; 	// prompt to display to user in gui mode
  String	yes_label;	// label to display for the Yes/Ok answer
  String	no_label;	// label to display for the No/Cancel answer

  ProgEl_List	yes_code; 	// #SHOW_TREE items to execute if user says Yes/Ok to prompt in gui mode, or to always execute in nogui mode

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  PROGEL_SIMPLE_BASEFUNS(IfGuiPrompt);
protected:
  override void		UpdateAfterEdit_impl();
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

class TA_API CaseBlock: public CodeBlock { 
  // one case element of a switch: if switch variable is equal to case_val, then this chunk of code is run
  INHERITED(CodeBlock)
public:
  ProgExpr		case_val; // value of the switch variable -- if switch_var is equal to this, then this code is run (must use literal expression here) -- if case_val is empty, then this represents the default case (run when no other case matches)

  override String	GetDisplayName() const;

  PROGEL_SIMPLE_BASEFUNS(CaseBlock);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level);
  override const String	GenCssPost_impl(int indent_level); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};


class TA_API Switch: public ProgEl { 
  // switches execution based on the value of given variable -- each case expression is matched to a corresponding case_code item one-to-one
INHERITED(ProgEl)
public:
  ProgVarRef	    switch_var;	// #ITEM_FILTER_StdProgVarFilter variable to switch on

  ProgEl_List	    cases; 	// #SHOW_TREE variable value and code to execute for each case (list of CaseBlock objects)

  virtual void	    NewCase() 	{ cases.New(1); }
  // #BUTTON make a new case item
  virtual void	    CasesFmEnum();
  // #BUTTON #CONFIRM add all the cases for an enumerated type (switch_var must be either HARD_ENUM or DYN_ENUM)

  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  PROGEL_SIMPLE_BASEFUNS(Switch);
protected:
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
  // #ITEM_FILTER_StdProgVarFilter where to store the result of the expression (the variable)
  ProgExpr		expr;
  // expression to assign to variable
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  PROGEL_SIMPLE_BASEFUNS(AssignExpr);
protected:
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
  // #ITEM_FILTER_StdProgVarFilter variable to increment
  ProgExpr		expr;
  // expression for how much to add to variable (use a negative sign to decrement)
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  PROGEL_SIMPLE_BASEFUNS(VarIncr);
protected:
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
  static bool		ShowVarFilter(void* base, void* var); // filter for button, only obj types
  
  ProgVarRef		result_var;
  // #ITEM_FILTER_StdProgVarFilter where to store the result of the method call (optional -- can be NULL)
  ProgVarRef		obj;
  // #APPLY_IMMED #ITEM_FILTER_ShowVarFilter program variable that points to the object with the method to call
  TypeDef*		obj_type;
  // #NO_SHOW #NO_SAVE temp copy of obj.object_type
  MethodDef*		method;
  // #TYPE_ON_obj_type #APPLY_IMMED the method to call on object obj
  ProgArg_List		meth_args;
  // #SHOW_TREE arguments to be passed to the method

  override taList_impl*	children_() {return &meth_args;}	
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }

  PROGEL_SIMPLE_BASEFUNS(MethodCall);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MemberProgEl: public ProgEl { 
  // #VIRT_BASE base class for dealing with members of objects
INHERITED(ProgEl)
public:
  static bool		ShowVarFilter(void* base, void* var); // filter for button, only obj types

  ProgVarRef		obj;
  // #APPLY_IMMED #ITEM_FILTER_ShowVarFilter program variable that points to the object with the method to call
  TypeDef*		obj_type;
  // #NO_SHOW #NO_SAVE temp copy of obj.object_type
  String		path;
  // path to the member -- can just be member name (use member_lookup to lookup and enter here) -- you can also enter in multiple sub-path elements for object members that themselves have members
  MemberDef*		member_lookup;
  // #TYPE_ON_obj_type #APPLY_IMMED #NULL_OK #NO_SAVE #NO_EDIT #NO_UPDATE_POINTER lookup a member name -- after you choose, it will copy the name into the path and reset this lookup to NULL
  
  virtual bool		GetTypeFromPath(bool quiet = false);
  // get obj_type from current path (also gives warnings about bad paths unless quiet = true)

  PROGEL_SIMPLE_BASEFUNS(MemberProgEl);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MemberAssign: public MemberProgEl { 
  // set a member (attribute) on an object to a value given by an expression
INHERITED(MemberProgEl)
public:
  ProgExpr		expr; // the expression to compute and assign to the member
  bool			update_after; // call UpdateAfterEdit after setting the member: useful for updating displays and triggering other computations based on changed value, but this comes at a performance cost 
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  PROGEL_SIMPLE_BASEFUNS(MemberAssign);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MemberFmArg: public MemberProgEl { 
  // set a member (attribute) on an object to a value given by a startup argument passed to overall program when it was run -- if argument was not set by user, nothing happens.  IMPORTANT: must also include a RegisterArgs program element BEFORE this item in the program code to register this argument and process the command list
INHERITED(MemberProgEl)
public:
  String		arg_name; // argument name -- this will be passed on the command line as <arg_name>=<value> (no spaces) (e.g., if arg_name is "rate" then command line would be rate=0.01 and internal arg name is just "rate" -- can be accessed using taMisc arg functions using that name)
  bool			update_after; // call UpdateAfterEdit after setting the member: useful for updating displays and triggering other computations based on changed value, but this comes at a performance cost 
  bool			quiet;	      // do not emit a message when arg is set and member value is assigned (otherwise, informational msg is printed -- useful for startup code output)
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  //  PROGEL_SIMPLE_BASEFUNS(MemberFmArg);
  void Copy_(const MemberFmArg& cp);
  SIMPLE_LINKS(MemberFmArg);
  TA_BASEFUNS(MemberFmArg);
protected:
  ProgVar* 		prv_obj; // #IGNORE used to track changes in obj type to clear expr
  
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API MemberMethodCall: public MemberProgEl { 
  // ##DEF_CHILD_meth_args call a method on a member of an object
INHERITED(MemberProgEl)
public:
  ProgVarRef		result_var;
  // #ITEM_FILTER_StdProgVarFilter where to store the result of the method call (optional -- can be NULL)
  MethodDef*		method;
  // #TYPE_ON_obj_type #APPLY_IMMED the method to call on object obj->path
  ProgArg_List		meth_args;
  // #SHOW_TREE arguments to be passed to the method

  override taList_impl*	children_() {return &meth_args;}	
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }

  PROGEL_SIMPLE_BASEFUNS(MemberMethodCall);
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
  TA_BASEFUNS_NOCOPY(MathCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

class TA_API RandomCall : public StaticMethodCall { 
  // call a Random number generation function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS_NOCOPY(RandomCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

class TA_API MiscCall : public StaticMethodCall { 
  // call a taMisc function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS_NOCOPY(MiscCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

class TA_API PrintVar: public ProgEl { 
  // print out (to the console) the value of variable(s) -- useful for debugging
INHERITED(ProgEl)
public:
  String		message;	// initial message to print
  ProgVarRef		print_var; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var2; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var3; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var4; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var5; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var6; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }

  PROGEL_SIMPLE_BASEFUNS(PrintVar);
protected:
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
  PROGEL_SIMPLE_BASEFUNS(PrintExpr);

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

  PROGEL_SIMPLE_BASEFUNS(Comment);

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
  PROGEL_SIMPLE_BASEFUNS(StopStepPoint);

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
  PROGEL_SIMPLE_BASEFUNS(ReturnExpr);

protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API OtherProgramVar : public ProgEl { 
  // A program element for exchanging variable information between programs -- variables must have the same names in both programs
INHERITED(ProgEl)
public:
  ProgramRef	other_prog; 	// #APPLY_IMMED the other program with variables that you want to get or set
  bool		set_other;	// if true, values in other program are set from our variable values, otherwise our variables get values from those in other program
  ProgVarRef	var_1;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of variable in other program!
  ProgVarRef	var_2;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of variable in other program!
  ProgVarRef	var_3;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of variable in other program!
  ProgVarRef	var_4;		// #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of variable in other program!

  virtual Program*	GetOtherProg();
  // safe call to get other program: emits error if other_prog is null (used by program)

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Program"; }

  PROGEL_SIMPLE_BASEFUNS(OtherProgramVar);
protected:
  override void	CheckThisConfig_impl(bool quiet, bool& rval);

  override const String	GenCssPre_impl(int indent_level); 
  override const String	GenCssBody_impl(int indent_level);
  override const String	GenCssPost_impl(int indent_level); 
  virtual bool	GenCss_OneVar(String& rval, ProgVarRef& var,
			      const String& il, int var_no);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

class TA_API ProgVarFmArg: public ProgEl { 
  // sets a variable (vars or args) in a program from a startup command-line argument (if arg was not set by user, nothing happens). IMPORTANT: must also include a RegisterArgs program element BEFORE this item in the program code to register this argument and process the command list
INHERITED(ProgEl)
public:
  ProgramRef		prog; 	// #APPLY_IMMED program that you want to set variable from argument in
  String		var_name; // name of variable in program to set
  String		arg_name; // argument name -- this will be passed on the command line as <arg_name>=<value> (no spaces) (e.g., if arg_name is "rate" then command line would be rate=0.01 and internal arg name is just "rate" -- can be accessed using taMisc arg functions using that name)

  virtual Program*	GetOtherProg();
  // safe call to get other program: emits error if other_prog is null (used by program)

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  PROGEL_SIMPLE_BASEFUNS(ProgVarFmArg);

protected:
  override void UpdateAfterEdit_impl();
  override void CheckThisConfig_impl(bool quiet, bool& rval);
  override const String	GenCssBody_impl(int indent_level);

  void		AddArgsFmCode(String& gen_code, ProgEl_List& progs);
  // main function: iterates recursively through progs, adding any that add args to gen_code

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

class TA_API RegisterArgs: public ProgEl { 
  // register command-line arguments for any MemberFmArg or ProgVarFmArg program elements contained in the prog_code of the program that this item appears in.  calls taMisc::UpdateArgs(), so any any other taMisc::AddArgName MiscCall's placed before this will also be processed
INHERITED(ProgEl)
public:
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  PROGEL_SIMPLE_BASEFUNS(RegisterArgs);

protected:
  override const String	GenCssBody_impl(int indent_level);

  void		AddArgsFmCode(String& gen_code, ProgEl_List& progs, int indent_level);
  // main function: iterates recursively through progs, adding any that add args to gen_code

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif
