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

#ifndef IfContinue_h
#define IfContinue_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:


taTypeDef_Of(IfContinue);

class TA_API IfContinue: public ProgEl { 
  // if condition is true, continue looping (skip any following code and loop back to top of loop) (can leave condition empty to just insert an unconditional continue)
INHERITED(ProgEl)
public:
  ProgExpr		cond; 		// conditionalizing expression for continuing loop

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgCtrl"; }
  String	GetToolbarName() const override { return "if.cont"; }

  PROGEL_SIMPLE_BASEFUNS(IfContinue);
protected:
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssBody_impl(Program* prog) override;

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // IfContinue_h
