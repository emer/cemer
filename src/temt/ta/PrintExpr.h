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

#ifndef PrintExpr_h
#define PrintExpr_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>
#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(PrintExpr);

class TA_API PrintExpr: public ProgEl { 
  // print out (to the console) an expression -- e.g., an informational message for the user
INHERITED(ProgEl)
public:
  ProgExpr		 expr;
  // #BROWSER_EDIT_LOOKUP print out (to console) this expression -- it just does the equivalent of 'cout << expr << endl;' so you can put multiple << segments in the expression to print out multiple things -- you DO need to include quotes around strings!
  ProgVarRef		my_mask; 	// #ITEM_FILTER_StdProgVarFilter set this to a DynEnum variable with bits flags set for when to actually print this information, in comparison to the current debug_level variable -- if any flags match, then it will be printed
  ProgVarRef		debug_level; 	// #ITEM_FILTER_StdProgVarFilter set this to a DynEnum variable with bits flags set, indicating the current desired debugging level
  bool          debug; // if false just print; if set check Program_Group debug_mode and only print if that is true

  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgVar"; }
  String	GetToolbarName() const override { return "print"; }

  PROGEL_SIMPLE_BASEFUNS(PrintExpr);
protected:
  void 	  UpdateAfterEdit_impl() override;
  void 	  CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssBody_impl(Program* prog) override;

private:
  void	  Initialize();
  void	  Destroy()	{CutLinks();}
}; 

#endif // PrintExpr_h
