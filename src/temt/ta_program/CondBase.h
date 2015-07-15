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

#ifndef CondBase_h
#define CondBase_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>
#include <ProgEl_List>

// declare all other types mentioned but not required to include:

taTypeDef_Of(CondBase);

class TA_API CondBase : public ProgEl {
  // #VIRT_BASE ##DEF_CHILD_true_code base class for If and ElseIf which have both a conditional expression and code to execute when the conditional is true (the true_code)
  INHERITED(ProgEl)
public:
  ProgExpr	    cond; 	// #BROWSER_EDIT_LOOKUP condition expression to test for true or false
  ProgEl_List	  true_code; 	// #SHOW_TREE items to execute if condition true
  
  int 		ProgElChildrenCount() const override
  { return true_code.size; }

  bool		IsCtrlProgEl() 	override { return true; }
  taList_impl*	children_() override { return &true_code; }

  bool          BrowserEditTest() override;

  PROGEL_SIMPLE_BASEFUNS(CondBase);
protected:

private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // CondBase_h
