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

#ifndef IfReturn_h
#define IfReturn_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:


class TA_API IfReturn: public ProgEl { 
  // if condition is true, return (from void function or stop further execution of code or init segments of Program) (can leave condition empty to just insert an unconditional return)
INHERITED(ProgEl)
public:
  ProgExpr		cond; 		// conditionalizing expression for returning

  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  override bool		CvtFmCode(const String& code);
  override bool		IsCtrlProgEl() 	{ return true; }

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }
  override String	GetToolbarName() const { return "if.return"; }

  PROGEL_SIMPLE_BASEFUNS(IfReturn);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // IfReturn_h
