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

#ifndef PrintVar_h
#define PrintVar_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(PrintVar);

class TA_API PrintVar: public ProgEl { 
  // print out (to the console) the value of variable(s) -- useful for debugging
INHERITED(ProgEl)
public:
  String		    message;	// initial message to print (do NOT include quotes!)
  ProgVarRef		print_var; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var2; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var3; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var4; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var5; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		print_var6; 	// #ITEM_FILTER_StdProgVarFilter print out (to console) the value of this variable
  ProgVarRef		my_mask; 	// #ITEM_FILTER_StdProgVarFilter set this to a DynEnum variable with bits flags set for when to actually print this information, in comparison to the current debug_level variable -- if any flags match, then it will be printed
  ProgVarRef		debug_level; 	// #ITEM_FILTER_StdProgVarFilter set this to a DynEnum variable with bits flags set, indicating the current desired debugging level 
    bool        debug; // if false just print; if set check Program_Group debug_mode and only print if that is true
  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgVar"; }
  String	GetToolbarName() const override { return "print var"; }

  PROGEL_SIMPLE_BASEFUNS(PrintVar);
protected:
  void 	  UpdateAfterEdit_impl() override;
  void 	  CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssBody_impl(Program* prog) override;

private:
  void	  Initialize();
  void	  Destroy()	{CutLinks();}
}; 

#endif // PrintVar_h
