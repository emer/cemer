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

#ifndef ProgramCall_h
#define ProgramCall_h 1

// parent includes:
#include <ProgramCallBase>

// member includes:
#include <ProgramRef>

// declare all other types mentioned but not required to include:
class ProgEl; // 


taTypeDef_Of(ProgramCall);

class TA_API ProgramCall: public ProgramCallBase {
  // call (run) another program, setting any arguments before hand
INHERITED(ProgramCallBase)
public:
  ProgramRef            target; // the program to be called
  String                targ_ld_init; // #EDIT_DIALOG name(s) of target programs to search for to set this target pointer when program is loaded from program library or other external sources -- if not found, a warning message is emitted.  if empty, it defaults to name of current target. use commas to separate multiple options (tried in order) and an * indicates use the "contains" searching function (not full regexp support yet)

  Program*     GetTarget() override;
  Program*     GetTarget_Compile() override;
  void         AddTargetsToListAll(Program_List& all_lst) override;
  bool         CallsProgram(Program* prg) override;
  bool         WillGenCompileScript(Program* prg) override;
  void         GenCompileScript(Program* prg) override;
  void         GenCallInit(Program* prg) override;

  void         SetTarget(Program* target); // #DROP1

  bool         LoadInitTarget() override;
  // initialize target based on targ_ld_init information
  virtual bool LoadInitTarget_impl(const String& nm);
  // initialize target based on targ_ld_init information

  bool         CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool         CvtFmCode(const String& code) override;

  taList_impl* children_() override {return &prog_args;}
  String       GetDisplayName() const override;
  String       GetTypeDecoKey() const override { return "Program"; }
  String       GetToolbarName() const override { return "prog()"; }

  PROGEL_SIMPLE_BASEFUNS(ProgramCall);
protected:
  void         PreGenMe_impl(int item_id) override; // register the target as a subprog of this one
  void         UpdateAfterEdit_impl() override;
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
  void         GenCssPre_impl(Program* prog) override;
  void         GenCssBody_impl(Program* prog) override;
  void         GenCssPost_impl(Program* prog) override;
private:
  void  Initialize();
  void  Destroy()       {}
};

#endif // ProgramCall_h
