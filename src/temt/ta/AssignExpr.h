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

#ifndef AssignExpr_h
#define AssignExpr_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>
#include <ProgExpr>

// declare all other types mentioned but not required to include:


taTypeDef_Of(AssignExpr);

class TA_API AssignExpr: public ProgEl { 
  // assign an expression to a variable (use method call for simple assignment to function call)
INHERITED(ProgEl)
public:
  ProgVarRef		result_var;
  // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser where to store the result of the expression (the variable)
  ProgExpr		expr;
  // #BROWSER_EDIT_LOOKUP expression to assign to variable
  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgVar"; }
  String	GetToolbarName() const override { return "var="; }

  PROGEL_SIMPLE_BASEFUNS(AssignExpr);
protected:
  void 	CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssBody_impl(Program* prog) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // AssignExpr_h
