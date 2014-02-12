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

#include "ProgramCallVar.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ProgramCallVar);


void ProgramCallVar::Initialize() {
}

void ProgramCallVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ProgramCallVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!prog_group, quiet, rval, "prog_group is NULL");
  CheckError(!prog_name_var, quiet, rval, "prog_name_var is NULL");
  CheckError(prog_name_var->var_type != ProgVar::T_String, quiet, rval,
             "prog_name_var is not a String type -- must be");
  // todo: check all args!
}

bool ProgramCallVar::CallsProgram(Program* prg) {
  if(!prog_group) return false;
  return (prog_group->IsParentOf(prg));
}

Program_Group* ProgramCallVar::GetProgramGp() {
  if(!prog_group) {
    taMisc::CheckError("Program_Group prog_group is NULL in ProgramCallVar:",
                       desc, "in program:", program()->name);
    return NULL;
  }
  return prog_group;
}

Program* ProgramCallVar::GetTarget() {
  if(!prog_group) {
    taMisc::CheckError("Program_Group prog_group is NULL in ProgramCallVar:",
                       desc, "in program:", program()->name);
    return NULL;
  }
  if(!prog_name_var) {
    taMisc::CheckError("prog_name_var is NULL in ProgramCallVar:",
                       desc, "in program:", program()->name);
    return NULL;
  }
  String pnm = prog_name_var->string_val;
  Program* rval = prog_group->FindLeafName(pnm);
  if(!rval) {
    taMisc::CheckError("Program named:", pnm, "not found in Program_Group:",
                       prog_group->name, "path:", prog_group->GetPathNames(),
                       "in ProgramCallVar in program:", program()->name);
    return NULL;
  }
  return rval;
}

Program* ProgramCallVar::GetTarget_Compile() {
  if(!prog_group) {
    return NULL;
  }
  if(prog_group->leaves == 0) return NULL;
  return prog_group->Leaf(0);   // just return first guy
}

void ProgramCallVar::PreGenMe_impl(int item_id) {
  // register as a subproc
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen

  Program* trg = GetTarget_Compile();
  if(!trg || (prog->sub_progs_dir.FindEl(trg) < 0)) {
    // link in the call if targ is unique or null
    prog->sub_prog_calls.LinkUnique(this);
    if(trg) {
      for(int j=0;j<prog_group->leaves;j++) {
        Program* strg = prog_group->Leaf(j);
        prog->sub_progs_dir.LinkUnique(strg); // add direct sub-progs
      }
    }
  }
}

void ProgramCallVar::AddTargetsToListAll(Program_List& all_lst) {
  if(!prog_group) return;
  for(int j=0;j<prog_group->leaves;j++) {
    Program* strg = prog_group->Leaf(j);
    all_lst.LinkUnique(strg);
  }
}

bool ProgramCallVar::WillGenCompileScript(Program* prg) {
  return (bool)prog_group;
}

void ProgramCallVar::GenCompileScript(Program* prog) {
  // note: do not pass 'this' in addline -- don't want this to be the main association for this
  if(!prog_group) return;
  prog->AddLine(prog, "{");
  prog->IncIndent();
  prog->AddLine(prog, String("Program_Group* spgp = this") + GetPath(NULL, prog) + "->GetProgramGp();");
  prog->AddLine(prog, "for(int spi=0; spi<spgp->leaves; spi++) {");
  prog->IncIndent();
  prog->AddLine(prog, "Program* prg = spgp->Leaf(spi);");
  prog->AddLine(prog, "prg->CompileScript(true); // true = force!");
  prog->DecIndent();
  prog->AddLine(prog, "}");
  prog->DecIndent();
  prog->AddLine(prog, "}");
}

void ProgramCallVar::GenCallInit(Program* prog) {
  // note: do not pass 'this' in addline -- don't want this to be the main association for this
  if(!prog_group) return;
  Program* trg = GetTarget_Compile();
  if(!trg) return;
  prog->AddLine(prog, "{");
  prog->IncIndent();
  prog->AddLine(prog, String("Program_Group* spgp = this") + GetPath(NULL, prog) + "->GetProgramGp();");
  prog->AddLine(prog, "for(int spi=0; spi<spgp->leaves; spi++) {");
  prog->IncIndent();
  prog->AddLine(prog, "Program* prg = spgp->Leaf(spi);");

  for (int j = 0; j < prog_args.size; ++j) {
    ProgArg* ths_arg = prog_args.FastEl(j);
    ProgVar* prg_var = trg->args.FindName(ths_arg->name);
    String argval = ths_arg->expr.GetFullExpr();
    if (!prg_var || argval.empty()) continue;
    // check to see if the value of this guy is an arg or var of this guy -- if so, propagate it
    ProgVar* arg_chk = prog->args.FindName(argval);
    ProgVar* var_chk = prog->vars.FindName(argval);
    if(!arg_chk && !var_chk) continue;
    prog->AddLine(prog, String("prg->SetVar(\"") + prg_var->name + "\", " + argval + ");");
  }
  prog->AddLine(prog, "ret_val = prg->CallInit(this);");
  prog->DecIndent();
  prog->AddLine(prog, "}");
  prog->DecIndent();
  prog->AddLine(prog, "}");
}

void ProgramCallVar::GenCssPre_impl(Program* prog) {
  prog->AddLine(this, String("{ // call program from var (name) in group: ")
                + (prog_group ? prog_group->name : "<no prog_group>"),
                ProgLine::MAIN_LINE);
  prog->IncIndent();
}

void ProgramCallVar::GenCssBody_impl(Program* prog) {
  if(!prog_group) return;
  prog->AddLine(this, String("Program* target = this") + GetPath(NULL, program())+ "->GetTarget();");
  prog->AddLine(this, "if(target) {");
  prog->IncIndent();

  prog->AddVerboseLine(this, false, "\"calling program:\",target->name"); // not start

  GenCssArgSet_impl(prog, "target");

  prog->AddLine(this, "{ target->Call(this); }");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void ProgramCallVar::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "} // call program fm var");
}

String ProgramCallVar::GetDisplayName() const {
  String rval = "Call Fm ";
  if (prog_group) {
    rval += prog_group->GetName();
    if(prog_name_var) {
      rval += " " + prog_name_var->name;
    }
    if(prog_args.size > 0) {
      rval += "(";
      for(int i=0;i<prog_args.size;i++) {
        ProgArg* pa = prog_args.FastEl(i);
        if(i > 0) rval += ", ";
        rval += pa->expr.expr;   // GetDisplayName();
      }
      rval += ")";
    }
  }
  else
    rval += "(no program group set)";
  return rval;
}
