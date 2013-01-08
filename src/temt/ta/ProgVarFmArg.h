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

#ifndef ProgVarFmArg_h
#define ProgVarFmArg_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgramRef>

// declare all other types mentioned but not required to include:
class Program; // 


class TA_API ProgVarFmArg: public ProgEl { 
  // sets a variable (vars or args) in a program from a startup command-line argument (if arg was not set by user, nothing happens). IMPORTANT: must also include a RegisterArgs program element BEFORE this item in the program code to register this argument and process the command list
INHERITED(ProgEl)
public:
  ProgramRef		prog; 	// program that you want to set variable from argument in
  String		var_name; // name of variable in program to set
  String		arg_name; // argument name -- this will be passed on the command line as <arg_name>=<value> (no spaces) (e.g., if arg_name is "rate" then command line would be rate=0.01 and internal arg name is just "rate" -- can be accessed using taMisc arg functions using that name)

  virtual Program*	GetOtherProg();
  // safe call to get other program: emits error if other_prog is null (used by program)

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  override String	GetToolbarName() const { return "var=arg"; }

  void	GenRegArgs(Program* prog);
  // #IGNORE generate RegisterArgs code

  PROGEL_SIMPLE_BASEFUNS(ProgVarFmArg);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckThisConfig_impl(bool quiet, bool& rval);
  override void	GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // ProgVarFmArg_h
