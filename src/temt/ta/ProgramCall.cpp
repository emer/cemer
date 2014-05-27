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

#include "ProgramCall.h"
#include <Program>
#include <taMisc>
#include <Program_List>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(ProgramCall);


void ProgramCall::Initialize() {
}

void ProgramCall::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading) {
    if((bool)target) {
      String targ_ld_i = targ_ld_init.between("*", "*");
      if(targ_ld_init.empty() || !target.ptr()->GetName().contains(targ_ld_i)) {
        targ_ld_init = String("*") + target.ptr()->GetName() + "*"; // make it wild!
      }
    }
  }
}

void ProgramCall::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!target, quiet, rval, "target is NULL");
}

bool ProgramCall::CallsProgram(Program* prg) {
  return target.ptr() == prg;
}

Program* ProgramCall::GetTarget() {
  if(!target) {
    taMisc::CheckError("Program target is NULL in ProgramCall:",
                       desc, "in program:", program()->name);
  }
  // NOTE: we are assuming that it has been compiled from the Init process
  // calling here causes many recompiles because of the dirty bit propagation
  // during running
//   if(!target->CompileScript()) {
//     taMisc::CheckError("Program target script did not compile correctly in ProgramCall:",
//                     desc, "in program:", program()->name);
//   }
  return target.ptr();
}

Program* ProgramCall::GetTarget_Compile() {
  return target.ptr();
}

void ProgramCall::PreGenMe_impl(int item_id) {
  if(HasProgFlag(OFF)) return;  // this should already have been caught earlier
  // register as a subproc
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen

  Program* trg = target.ptr();
  if(!trg || (prog->sub_progs_dir.FindEl(trg) < 0)) {
    // link in the call if targ is unique or null
    prog->sub_prog_calls.LinkUnique(this);
    if(trg) prog->sub_progs_dir.LinkUnique(trg); // add direct sub-prog
  }
}

void ProgramCall::AddTargetsToListAll(Program_List& all_lst) {
  Program* trg = target.ptr();
  if(trg) {
    all_lst.LinkUnique(trg);
  }
}

void ProgramCall::SetTarget(Program* target_) {
  target = target_;
}

bool ProgramCall::WillGenCompileScript(Program* prg) {
  return (bool)target;
}

void ProgramCall::GenCompileScript(Program* prog) {
  // note: do not pass 'this' in addline -- don't want this to be the main association for this
  if(!target) return;
  prog->AddLine(prog, String("target = this") + GetPath(NULL, prog) + "->GetTarget();");
  prog->AddLine(prog, "target->CompileScript(true); // true = force!");
}

void ProgramCall::GenCallInit(Program* prog) {
  // note: do not pass 'this' in addline -- don't want this to be the main association for this
  if(!target) return;
  prog->AddLine(prog, String("target = this") + GetPath(NULL, prog) + "->GetTarget();");
  // set args for guys that are just passing our existing args/vars along
  for (int j = 0; j < prog_args.size; ++j) {
    ProgArg* ths_arg = prog_args.FastEl(j);
    ProgVar* prg_var = target->args.FindName(ths_arg->name);
    String argval = ths_arg->expr.GetFullExpr();
    if (!prg_var || argval.empty()) continue;
    // check to see if the value of this guy is an arg or var of this guy -- if so, propagate it
    ProgVar* arg_chk = prog->args.FindName(argval);
    ProgVar* var_chk = prog->vars.FindName(argval);
    if(!arg_chk && !var_chk) continue;
    prog->AddLine(prog, String("target->SetVar(\"") + prg_var->name + "\", " + argval + ");");
  }
  prog->AddLine(prog, "ret_val = target->CallInit(this);");
}

void ProgramCall::GenCssPre_impl(Program* prog) {
  prog->AddLine(this, String("{ // call program: ") + (target ? target->name : "<no target>"),
                ProgLine::MAIN_LINE); // best we have for main line in this case
  prog->IncIndent();
}

void ProgramCall::GenCssBody_impl(Program* prog) {
  if (!target) return;
  prog->AddLine(this, String("Program* target = this") + GetPath(NULL, program())+ "->GetTarget();");
  prog->AddLine(this, "if(target) {");
  prog->IncIndent();

  prog->AddVerboseLine(this, false, "\"calling program:\",target->name"); // not start
  GenCssArgSet_impl(prog, "target");

  prog->AddLine(this, "{ target->Call(this); }");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

void ProgramCall::GenCssPost_impl(Program* prog) {
  prog->DecIndent();
  prog->AddLine(this, "} // call program");
}

String ProgramCall::GetDisplayName() const {
  String rval = "Call ";
  if (target) {
    rval += target->GetName();
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
    rval += "(no program set)";
  return rval;
}

bool ProgramCall::LoadInitTarget() {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;

  target.set(NULL);             // default is always to start off empty
  bool got = false;
  if(targ_ld_init.contains(',')) {
    String nm = targ_ld_init;
    while(nm.contains(',')) {
      String snm = nm.before(',');
      nm = nm.after(',');
      while(nm.firstchar() == ' ') nm = nm.after(' ');
      got = LoadInitTarget_impl(snm);
      if(got) break;
      if(!nm.contains(','))     // get last guy
        got = LoadInitTarget_impl(nm);
    }
  }
  else {
    got = LoadInitTarget_impl(targ_ld_init);
  }
  if(!got) {
    taMisc::Warning("ProgramCall in program:", prg->name,
                    "could not find load init target program to call named:",
                    targ_ld_init, "target is set to NULL and must be fixed manually!");
  }
  return got;
}

bool ProgramCall::LoadInitTarget_impl(const String& nm) {
  if(nm.empty()) return false;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;

  Program* tv = NULL;
  if(nm.contains("*")) {
    String nnm = nm;
    nnm.gsub("*","");
    tv = prg->FindProgramNameContains(nnm, false);
  }
  else {
    tv = prg->FindProgramName(nm, false);
  }
  if(tv) {
    target.set(tv);
    return true;
  }
  return false;
}

bool ProgramCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String code_dn = code; code_dn.downcase();
  if(code_dn.startsWith("call ")) return true; // definitely
  if(code_dn.startsWith("prog ")) return true; // definitely
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String funm = lhs;
  if(lhs.contains('=')) return false; // no rval for progcall
  if((funm.freq('.') + funm.freq("->")) > 0) return false; // exclude method call
  if(!scope_el) return false;
  taProject* proj = GET_OWNER(scope_el, taProject);
  if(!proj) return false;
  Program* prg = proj->programs.FindLeafName(funm);
  if(prg) return true;
  return false;
}

bool ProgramCall::CvtFmCode(const String& code) {
  String cd = code;
  if(cd.startsWith("Call "))
    cd = cd.after("Call ");
  if(cd.startsWith("call "))
    cd = cd.after("call ");
  String lhs = cd;
  if(lhs.contains('('))
    lhs = lhs.before('(');
  String funm = lhs;
  taProject* proj = GET_OWNER(this, taProject);
  if(!proj) return false;
  Program* prg = proj->programs.FindLeafName(funm);
  if(!prg) return false;
  target = prg;
  UpdateAfterEdit_impl();                          // update based on targ
  // now tackle the args
  String args = trim(cd.after('('));
  if(args.endsWith(')')) args = trim(args.before(')',-1));
  if(args.endsWith(';')) args = trim(args.before(';',-1));
  for(int i=0; i<prog_args.size; i++) {
    ProgArg* pa = prog_args.FastEl(i);
    String arg;
    if(args.contains(',')) {
      arg = trim(args.before(','));
      args = trim(args.after(','));
    }
    else {
      arg = args;
      args = "";                // all done
    }
    pa->expr.SetExpr(arg);
    if(args.empty()) break;
  }
  UpdateAfterEdit_impl();
  return true;
}
