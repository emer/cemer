// Copyright 2017, Regents of the University of Colorado,
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

#ifndef ReturnExpr_h
#define ReturnExpr_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:


taTypeDef_Of(ReturnExpr);

class TA_API ReturnExpr: public ProgEl { 
  // ##PROGEL_COMPLETION return from a function with a given expression (can be empty to return from a void function) -- you can return from the code or init segments of a program to end execution at that point
INHERITED(ProgEl)
public:
  ProgExpr		expr;
  // #BROWSER_EDIT_LOOKUP expression to return from function with (can be empty to return from a void function)
  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgCtrl"; }
  String	GetToolbarName() const override { return "return"; }

  PROGEL_SIMPLE_BASEFUNS(ReturnExpr);
protected:
  void		CheckChildConfig_impl(bool quiet, bool& rval) override;
  void		GenCssBody_impl(Program* prog) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // ReturnExpr_h
