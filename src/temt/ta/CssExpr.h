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

#ifndef CssExpr_h
#define CssExpr_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:


TypeDef_Of(CssExpr);

class TA_API CssExpr: public ProgEl { 
  // contains a direct css expression -- can be useful for more complex matrix code -- will be generated from ProgCode for any code that ends with semicolon.  unlike UserScript the variables here are tracked as normal
INHERITED(ProgEl)
public:
  ProgExpr		expr;  // any valid css expression
  
  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  override bool		CvtFmCode(const String& code);

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  override String	GetToolbarName() const { return "css"; }

  PROGEL_SIMPLE_BASEFUNS(CssExpr);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // CssExpr_h
