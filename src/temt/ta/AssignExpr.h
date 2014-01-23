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
  // expression to assign to variable
  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const CPP11_OVERRIDE;
  bool		CvtFmCode(const String& code) CPP11_OVERRIDE;

  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "ProgVar"; }
  String	GetToolbarName() const CPP11_OVERRIDE { return "var="; }

  PROGEL_SIMPLE_BASEFUNS(AssignExpr);
protected:
  void 	CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // AssignExpr_h
