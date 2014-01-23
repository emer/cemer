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

#ifndef IfBreak_h
#define IfBreak_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:


taTypeDef_Of(IfBreak);

class TA_API IfBreak: public ProgEl { 
  // if condition is true, break out of current loop (can leave condition empty to just insert an unconditional break
INHERITED(ProgEl)
public:
  ProgExpr		cond; 		// conditionalizing expression for breaking loop

  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const CPP11_OVERRIDE;
  bool		CvtFmCode(const String& code) CPP11_OVERRIDE;
  bool		IsCtrlProgEl() 	CPP11_OVERRIDE { return true; }

  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "ProgCtrl"; }
  String	GetToolbarName() const CPP11_OVERRIDE { return "if.break"; }

  PROGEL_SIMPLE_BASEFUNS(IfBreak);
protected:
  void		CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // IfBreak_h
