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

#ifndef ProgramCallVar_h
#define ProgramCallVar_h 1

// parent includes:
#include <ProgramCallBase>

// member includes:
#include <Program_Group>
#include <ProgVarRef>

// declare all other types mentioned but not required to include:
class Program; // 
class Program_Group; // 


taTypeDef_Of(ProgramCallVar);

class TA_API ProgramCallVar: public ProgramCallBase {
  // ##DEF_CHILD_prog_args call (run) another program by name based on a string variable, setting any arguments before hand
INHERITED(ProgramCallBase)
public:
  Program_GroupRef      prog_group; // sub-group of programs to look in for program to call -- ALL of the programs in this group MUST have the same set of args, and all are considered potential candidates to be called (e.g., they are all Init'd when the calling program is Init'd)
  ProgVarRef            prog_name_var; // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser variable that contains name of program within prog_group to call -- this is only used at the time the program call is made when the program is running

  override Program*     GetTarget();
  override Program*     GetTarget_Compile();
  override void         AddTargetsToListAll(Program_List& all_lst);
  override bool         CallsProgram(Program* prg);
  override bool         WillGenCompileScript(Program* prg);
  override void         GenCompileScript(Program* prg);
  override void         GenCallInit(Program* prg);

  virtual Program_Group* GetProgramGp();
  // get prog_group pointer value in a safe way

  override String       GetDisplayName() const;
  override String       GetToolbarName() const { return "prog var()"; }

  PROGEL_SIMPLE_BASEFUNS(ProgramCallVar);
protected:
  override void         PreGenMe_impl(int item_id); // register the target as a subprog of this one
  override void         UpdateAfterEdit_impl();
  override void         CheckThisConfig_impl(bool quiet, bool& rval);
  override void         GenCssPre_impl(Program* prog);
  override void         GenCssBody_impl(Program* prog);
  override void         GenCssPost_impl(Program* prog);
private:
  void  Initialize();
  void  Destroy()       {}
};

#endif // ProgramCallVar_h
