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

#ifndef OtherProgramVar_h
#define OtherProgramVar_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <Program>
#include <ProgVar>

// declare all other types mentioned but not required to include:
class Program; // 


class TA_API OtherProgramVar : public ProgEl { 
  // A program element for exchanging variable information between programs -- variables must have the same names in both programs
INHERITED(ProgEl)
public:
  ProgramRef	other_prog; 	// the other program with variables that you want to get or set
  bool		set_other;	// if true, values in other program are set from our variable values, otherwise our variables get values from those in other program
  ProgVarRef	var_1;		// #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser program variable to operate on -- name must match name of variable in other program!
  ProgVarRef	var_2;		// #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser program variable to operate on -- name must match name of variable in other program!
  ProgVarRef	var_3;		// #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser program variable to operate on -- name must match name of variable in other program!
  ProgVarRef	var_4;		// #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser program variable to operate on -- name must match name of variable in other program!

  virtual Program*	GetOtherProg();
  // safe call to get other program: emits error if other_prog is null (used by program)

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Program"; }
  override String	GetToolbarName() const { return "oth prg var"; }

  PROGEL_SIMPLE_BASEFUNS(OtherProgramVar);
protected:
  override void	CheckThisConfig_impl(bool quiet, bool& rval);

  override void	GenCssPre_impl(Program* prog); 
  override void	GenCssBody_impl(Program* prog);
  override void	GenCssPost_impl(Program* prog); 
  virtual bool	GenCss_OneVar(Program* prog, ProgVarRef& var, int var_no);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // OtherProgramVar_h
