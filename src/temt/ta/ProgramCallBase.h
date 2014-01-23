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

#ifndef ProgramCallBase_h
#define ProgramCallBase_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgArg_List>

// declare all other types mentioned but not required to include:
class Program; // 
class Program_List; //


taTypeDef_Of(ProgramCallBase);

class TA_API ProgramCallBase: public ProgEl {
  // ##DEF_CHILD_prog_args #VIRT_BASE base class for prog el that calls a program -- manages args and provides interface that Program's require for managing sub-progs
INHERITED(ProgEl)
public:
  ProgArg_List          prog_args; // #SHOW_TREE arguments to the program--copied to prog before call -- all programs in prog_group must accept these same args

  virtual void          UpdateArgs();
  // #BUTTON updates the arguments based on the target args (also automatically called in updateafteredit)

  virtual Program*      GetTarget() { return NULL; }
  // safe call to get target: emits error if target is null (used by program) -- call during runtime
  virtual Program*      GetTarget_Compile() { return NULL; }
  // safe call to get target during compile time -- fail silently
  virtual void          AddTargetsToListAll(Program_List& all_lst) { };
  // #IGNORE add any actual targets of this program to the all list-- use LinkUnique -- only non-null!
  virtual bool          CallsProgram(Program* prg) { return false; }
  // #IGNORE return true if this program call calls given program
  virtual bool          WillGenCompileScript(Program* prg) { return false; }
  // #IGNORE generate code to compile script on target
  virtual void          GenCompileScript(Program* prg) { };
  // #IGNORE generate code to compile script on target
  virtual void          GenCallInit(Program* prg) { };
  // #IGNORE generate code to call init on target

  virtual bool          LoadInitTarget() { return false; }
  // initialize target based on targ_ld_init information

  taList_impl* children_() CPP11_OVERRIDE {return &prog_args;}
  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

  PROGEL_SIMPLE_BASEFUNS(ProgramCallBase);
protected:
  void         UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void         CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  virtual void          GenCssArgSet_impl(Program* prog, const String trg_var_nm);
private:
  void  Initialize();
  void  Destroy()       {}
};

#endif // ProgramCallBase_h
