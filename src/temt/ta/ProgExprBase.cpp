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

#include "ProgExprBase.h"

cssProgSpace ProgExprBase::parse_prog;
cssSpace ProgExprBase::parse_tmp;

void ProgExprBase::Initialize() {
  flags = PE_NONE;
  parse_ve_off = 11;
  parse_ve_pos = 0;
}

void ProgExprBase::Destroy() {
  CutLinks();
}

void ProgExprBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(var_names, this);
  taBase::Own(bad_vars, this);
}

void ProgExprBase::CutLinks() {
  vars.Reset();
  var_names.CutLinks();
  bad_vars.CutLinks();
  inherited::CutLinks();
}

void ProgExprBase::Copy_(const ProgExprBase& cp) {
  expr = cp.expr;
  flags = cp.flags;
  var_expr = cp.var_expr;
  var_names = cp.var_names;
  bad_vars = cp.bad_vars;

  //  vars = cp.vars;
  // the above copy does not set the owner!
  vars.Reset();
  for(int i=0;i<cp.vars.size;i++) {
    ProgVarRef* pvr = new ProgVarRef;
    vars.Add(pvr);
    pvr->Init(this);
    pvr->set(cp.vars[i]->ptr());
  }
}

void ProgExprBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(HasExprFlag(NO_PARSE)) return;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg || isDestroying()) return;
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(pel && (pel->GetEnabled() == 0)) return;
  ParseExpr();
  if(!HasExprFlag(NO_VAR_ERRS)) {
    ProgEl* pel = GET_MY_OWNER(ProgEl);
    if(!taMisc::is_loading && bad_vars.size > 0) {
      String chs_str = "ProgExpr in program element:" + pel->GetDisplayName() +
        "\n in program: " + prg->name +
        " Errors in expression -- the following variable names could not be found: ";
      for(int i=0; i<bad_vars.size; i++) {
        chs_str += " " + bad_vars[i];
      }
      int chs = taMisc::Choice(chs_str, "Create as Globals", "Create as Locals", "I will fix Expr");
      if(chs == 0) {
        for(int i=0; i<bad_vars.size; i++) {
          ProgVar* nwvar = (ProgVar*)prg->vars.New(1, NULL, bad_vars[i]);
          if(taMisc::gui_active && i==0)
            tabMisc::DelayedFunCall_gui(nwvar, "BrowserSelectMe");
        }
      }
      if(chs == 1) {
        for(int i=0; i<bad_vars.size; i++) {
          ProgVar* nwvar = pel->MakeLocalVar(bad_vars[i]);
          if(taMisc::gui_active && i==0)
            tabMisc::DelayedFunCall_gui(nwvar, "BrowserSelectMe");
        }
      }
    }
  }
}

void ProgExprBase::UpdateProgExpr_NewOwner() {
  // note: this is assumed to be called *prior* to any updating of pointers!
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(!pel) return;
  for(int i=0;i<vars.size;i++) {
    ProgVarRef* pvr = vars[i];
    pel->UpdateProgVarRef_NewOwner(*pvr);
  }
}

void ProgExprBase::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!HasExprFlag(NO_VAR_ERRS)) {
    Program* prg = GET_MY_OWNER(Program);
    ProgEl* pel = GET_MY_OWNER(ProgEl);
    if(prg && pel && bad_vars.size > 0) {
      rval = false;
      if(!quiet)
        taMisc::CheckError("ProgExpr in program element:", pel->GetDisplayName(),"\n in program:", prg->name," Errors in expression -- the following variable names could not be found:", bad_vars[0],
                           (bad_vars.size > 1 ? bad_vars[1] : _nilString),
                           (bad_vars.size > 2 ? bad_vars[2] : _nilString),
                           (bad_vars.size > 3 ? bad_vars[3] : _nilString)
                           // (bad_vars.size > 4 ? bad_vars[4] : _nilString),
                           // (bad_vars.size > 5 ? bad_vars[5] : _nilString),
                           // (bad_vars.size > 6 ? bad_vars[6] : _nilString)
                           );
    }
  }
}

int ProgExprBase::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = inherited::UpdatePointers_NewPar(old_par, new_par);
  nchg += vars.UpdatePointers_NewPar(this, old_par, new_par);
  return nchg;
}

int ProgExprBase::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  int nchg = inherited::UpdatePointers_NewParType(par_typ, new_par);
  nchg += vars.UpdatePointers_NewParType(this, par_typ, new_par);
  return nchg;
}

int ProgExprBase::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = inherited::UpdatePointers_NewObj(old_ptr, new_ptr);
  nchg += vars.UpdatePointers_NewObj(this, this, old_ptr, new_ptr);
  return nchg;
}

void ProgExprBase::SmartRef_DataDestroying(taSmartRef* ref, taBase* obj) {
  inherited::SmartRef_DataDestroying(ref, obj); // does UAE
}

void ProgExprBase::SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
                                    int dcr, void* op1_, void* op2_)
{
  // we only update ourself if the schema of a var changed
  if (obj && obj->InheritsFrom(&TA_ProgVar)) {
    ProgVar* pv = (ProgVar*)obj;
    if (!pv->schemaChanged())
      return;
  }

  expr = GetFullExpr();         // update our expr to reflect any changes in variables.
  if(owner)                     // usu we are inside something else
    owner->UpdateAfterEdit();
  else
    UpdateAfterEdit();
}

bool ProgExprBase::SetExpr(const String& ex) {
  expr = ex;
  UpdateAfterEdit();            // does parse
  return (bad_vars.size == 0);  // do we have any bad variables??
}

String ProgExprBase::GetName() const {
  if(owner) return owner->GetName();
  return _nilString;
}

String ProgExprBase::GetDisplayName() const {
  return expr;
}

void ProgExprBase::ParseExpr_SkipPath(int& pos) {
  int len = expr.length();
  int c;
  while((pos < len) && (isalnum(c=expr[pos]) || (c=='_') || (c=='.') || (c==':')
                        || (c=='[') || (c==']')))
    { var_expr.cat((char)c); pos++; }
}


int ProgExprBase::cssExtParseFun_pre(void* udata, const char* nm, cssElPtr& el_ptr) {
  String vnm = nm;
  if(vnm == "__tmp") return 0;  // skip

  ProgExprBase* pe = (ProgExprBase*)udata;
  ProgEl* pel = GET_OWNER(pe, ProgEl);
  Program* prog = GET_OWNER(pe, Program);
  int idx = 0;
  ProgVar* var = NULL;
  Function* fun = GET_OWNER(pe, Function); // first look inside any function we might live within

  if(vnm == "this") {
    cssEl* el = new cssTA_Base((void*)prog, 1, &TA_Program, "this");
    pe->parse_tmp.Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }
  if(vnm == "run_state") {
    cssEl* el = new cssCPtr_enum(&(prog->run_state), 1, "run_state",
                                 TA_Program.sub_types.FindName("RunState"));
    pe->parse_tmp.Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }
  if(vnm == "ret_val") {
    cssEl* el = new cssCPtr_int(&(prog->ret_val), 1, "ret_val");
    pe->parse_tmp.Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }

  if(pel) {
    var = pel->FindVarNameInScope(vnm, false); // no make
  }
  else {
    if(fun)
      var = fun->FindVarName(vnm);
    if(!var)
      var = prog->FindVarName(vnm);
  }
  if(var) {
    if(!pe->vars.FindVar(var, idx)) {
      ProgVarRef* prf = new ProgVarRef;
      prf->Init(pe);    // we own it
      prf->set(var);
      pe->vars.Add(prf);
      pe->var_names.Add(vnm);
      idx = pe->vars.size-1;
    }
    var->SetParseCssEl();
    el_ptr.SetDirect(var->parse_css_el);
    // update var_expr
    String subst = "$#" + (String)idx + "#$";
    String add_chunk = pe->expr.at(pe->parse_ve_pos-pe->parse_ve_off, parse_prog.src_pos - pe->parse_ve_pos - vnm.length());
    pe->var_expr += add_chunk;
    pe->var_expr += subst;
    pe->parse_ve_pos = parse_prog.src_pos;

    return var->parse_css_el->GetParse();
  }
  else {
    // not found -- check to see if it is some other thing:
    taBase* ptyp = prog->FindTypeName(vnm);
    if(ptyp) {
      if(ptyp->InheritsFrom(&TA_DynEnumType)) {
        cssEnumType* etyp = new cssEnumType(ptyp->GetName());
        pe->parse_tmp.Push(etyp);
        el_ptr.SetDirect(etyp);
        return CSS_TYPE;
      }
      else if(ptyp->InheritsFrom(&TA_DynEnumItem)) {
        DynEnumItem* eit = (DynEnumItem*)ptyp;
        cssEnum* eval = new cssEnum(eit->value, eit->name);
        pe->parse_tmp.Push(eval);
        el_ptr.SetDirect(eval);
        return CSS_VAR;
      }
    }
    else {
      Function* fun = prog->functions.FindName(vnm);
      if(fun && fun->name == vnm) { // findname will do crazy inherits thing on types, giving wrong match, so you need to make sure it is actually of the same name
        cssScriptFun* sfn = new cssScriptFun(vnm);
        pe->parse_tmp.Push(sfn);
        sfn->argc = fun->args.size;
        el_ptr.SetDirect(sfn);
        return CSS_FUN;
      }
    }
  }
  return 0;                     // not found!
}

int ProgExprBase::cssExtParseFun_post(void* udata, const char* nm, cssElPtr& el_ptr) {
  String vnm = nm;
  if(vnm == "__tmp" || vnm == "this") return 0; // skip that guy
  ProgExprBase* pe = (ProgExprBase*)udata;
  pe->bad_vars.AddUnique(vnm);  // this will trigger err msg later..
//   cerr << "added bad var: " << vnm << endl;
  return 0;                             // we don't do any cleanup -- return false
}

bool ProgExprBase::ParseExpr() {
  Program_Group* pgp = GET_MY_OWNER(Program_Group);
  if(TestError(!pgp, "ParseExpr", "no parent Program_Group found -- report to developers as bug"))
    return false;
  String pnm = GetPathNames(NULL, pgp);

  // todo: temporary fix for saved wrong flags, remove after a while (4.0.10)
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(pel) {
    pel->SetProgExprFlags();
  }

  vars.Reset();
  var_names.Reset();
  bad_vars.Reset();
  var_expr = _nilString;
  String parse_expr;
  if(HasExprFlag(FULL_STMT)) {
    parse_expr = expr;
    parse_ve_off = 0;
  }
  else if(HasExprFlag(FOR_LOOP_EXPR)) {
    parse_expr = "for(" + expr + ";;) .i;";
    parse_ve_off = 4;
  }
  else {
    if((bool)taMisc::types.FindName(expr)) {
      var_expr = expr;
      return true; // just a type name -- good!
    }
    parse_expr = "int __tmp= " + expr + ";";
    parse_ve_off = 11;          // offset is 11 due to 'int _tmp= '
  }
  parse_ve_pos = parse_ve_off;
  if(expr.empty() || expr == "<no_arg>") return true; // <no_arg> is a special flag..

  parse_prog.SetName(pnm);
  parse_prog.ClearAll();
  parse_prog.ext_parse_fun_pre = &cssExtParseFun_pre;
  parse_prog.ext_parse_fun_post = &cssExtParseFun_post;
  parse_prog.ext_parse_user_data = (void*)this;
  // use this for debugging exprs:
  //  parse_prog.SetDebug(6);

  parse_prog.CompileCode(parse_expr);   // use css to do all the parsing!

  for(int i=0;i<vars.size;i++)  {
    ProgVarRef* vrf = vars.FastEl(i);
    if(!TestError(!vrf->ptr(), "ParseExpr", "vrf->ptr() == NULL -- this shouldn't happen -- report as a bug!")) {
      ProgVar* var = (ProgVar*)vrf->ptr();
      var->ResetParseStuff();
    }
  }
  // get the rest
  if(parse_ve_pos-parse_ve_off < expr.length()) {
    String end_chunk = expr.at(parse_ve_pos-parse_ve_off, expr.length() - (parse_ve_pos-parse_ve_off));
    var_expr += end_chunk;
  }

  parse_tmp.Reset();
  parse_prog.ClearAll();

  return (bad_vars.size == 0);  // do we have any bad variables??
}

String ProgExprBase::GetFullExpr() const {
  if(vars.size == 0) return var_expr;
  String rval = var_expr;
  for(int i=0;i<vars.size;i++) {
    ProgVarRef* vrf = vars.FastEl(i);
    if(vrf->ptr()) {
      rval.gsub("$#" + (String)i + "#$", ((ProgVar*)vrf->ptr())->name);
    }
    else {
      rval.gsub("$#" + (String)i + "#$", var_names[i]); // put in original name, as a cue..
    }
  }
  return rval;
}

// StringFieldLookupFun is in ta_program_qt.cpp
