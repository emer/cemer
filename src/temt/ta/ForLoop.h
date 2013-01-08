// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ForLoop_h
#define ForLoop_h 1

// parent includes:
#include <Loop>

// member includes:
#include <ProgExprBase>

// declare all other types mentioned but not required to include:
class ProgEl; // 


class TA_API ForLoop: public Loop { 
  // Standard C 'for loop' over loop_code: for(init_expr; loop_test; loop_iter) loop_code\n -- runs the init_expr, then does loop_code and the loop_iter expression, and continues if loop_test is true
INHERITED(Loop)
public:
  ProgExprBase	    	init; // initialization expression (e.g., 'i=0' -- can also declare a new variable, but you won't be able to access it in other program code)
  ProgExprBase		test; // a test expression for whether to continue looping (e.g., 'i < max')
  ProgExprBase	    	iter; // the iteration operation run after each loop (e.g., increment the loop variable; 'i++')

  virtual void		ChangeLoopVar(const String& to_var);
  // #BUTTON change looping variable name from its current name to new variable name -- just does simple search & replace by text

  virtual String	GetLoopVar(bool& is_local) const;
  // this is a fuzzy "best guess" at the loop var -- it is used esp for creating a new one (j,k, etc.) in new nested loops; is_local is true if the var is declared in the init

  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  override bool		CvtFmCode(const String& code);
  override bool		IsCtrlProgEl() 	{ return true; }

  override String	GetDisplayName() const;
  override void		SetProgExprFlags();
  override String	GetToolbarName() const { return "for"; }

  PROGEL_SIMPLE_COPY(ForLoop);
  void InitLinks();
  SIMPLE_CUTLINKS(ForLoop);
  TA_BASEFUNS(ForLoop)
protected:
  virtual void	MakeIndexVar(const String& var_nm);
  // make default 'i' variable in program.vars -- just makes it easier to deal with loops in default case..

  override void	UpdateAfterEdit_impl();
  override void	UpdateOnInsert_impl(); // check for being nested, and update def var 
  bool		ParentForLoopVarClashes(const String& loop_var); // true if a parent For loop is also using the loop_var
  void		MorphVar(String& cur_loop_var); // typically i,j,k, etc. or var2, var3, etc
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssPre_impl(Program* prog); 
  override void		GenCssPost_impl(Program* prog); 
private:
  void	Initialize();
  void	Destroy()	{}
};

#endif // ForLoop_h
