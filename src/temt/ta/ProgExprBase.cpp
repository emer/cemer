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
#include <ProgEl>
#include <Program>
#include <Function>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <css_machine.h>
#include <css_ta.h>
#include <css_c_ptr_types.h>
#include <taiWidgetTokenPtrMultiType>
#include <LocalVars>
#include <taiGroupElsButton>
#include <taiListElsButton>
#include <taiWidgetMemberDefPtr>
#include <taiMemberMethodDefButton>
#include <taiWidgetEnumStaticChooser>
#include <taiWidgetTokenPtr>
#include <MemberProgEl>


cssProgSpace* ProgExprBase::parse_prog = NULL;
cssSpace* ProgExprBase::parse_tmp = NULL;

static ProgExprBase* expr_lookup_cur_base = NULL;

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

void ProgExprBase::SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) {
  inherited::SmartRef_SigDestroying(ref, obj); // does UAE
}

void ProgExprBase::SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
                                    int sls, void* op1_, void* op2_)
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
    pe->parse_tmp->Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }
  if(vnm == "run_state") {
    cssEl* el = new cssCPtr_enum(&(prog->run_state), 1, "run_state",
                                 TA_Program.sub_types.FindName("RunState"));
    pe->parse_tmp->Push(el);
    el_ptr.SetDirect(el);
    return el->GetParse();
  }
  if(vnm == "ret_val") {
    cssEl* el = new cssCPtr_int(&(prog->ret_val), 1, "ret_val");
    pe->parse_tmp->Push(el);
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
    String add_chunk = pe->expr.at(pe->parse_ve_pos-pe->parse_ve_off, parse_prog->src_pos - pe->parse_ve_pos - vnm.length());
    pe->var_expr += add_chunk;
    pe->var_expr += subst;
    pe->parse_ve_pos = parse_prog->src_pos;

    return var->parse_css_el->GetParse();
  }
  else {
    // not found -- check to see if it is some other thing:
    taBase* ptyp = prog->FindTypeName(vnm);
    if(ptyp) {
      if(ptyp->InheritsFrom(&TA_DynEnumType)) {
        cssEnumType* etyp = new cssEnumType(ptyp->GetName());
        pe->parse_tmp->Push(etyp);
        el_ptr.SetDirect(etyp);
        return CSS_TYPE;
      }
      else if(ptyp->InheritsFrom(&TA_DynEnumItem)) {
        DynEnumItem* eit = (DynEnumItem*)ptyp;
        cssEnum* eval = new cssEnum(eit->value, eit->name);
        pe->parse_tmp->Push(eval);
        el_ptr.SetDirect(eval);
        return CSS_VAR;
      }
    }
    else {
      Function* fun = prog->functions.FindName(vnm);
      if(fun && fun->name == vnm) { // findname will do crazy inherits thing on types, giving wrong match, so you need to make sure it is actually of the same name
        cssScriptFun* sfn = new cssScriptFun(vnm);
        pe->parse_tmp->Push(sfn);
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
    if((bool)TypeDef::FindGlobalTypeName(expr, false)) {
      var_expr = expr;
      return true; // just a type name -- good!
    }
    parse_expr = "int __tmp= " + expr + ";";
    parse_ve_off = 11;          // offset is 11 due to 'int _tmp= '
  }
  parse_ve_pos = parse_ve_off;
  if(expr.empty() || expr == "<no_arg>") return true; // <no_arg> is a special flag..

  if(!parse_prog) {
    parse_prog = new cssProgSpace;
  }
  if(!parse_tmp) {
    parse_tmp = new cssSpace;
  }
  parse_prog->SetName(pnm);
  parse_prog->ClearAll();
  parse_prog->ext_parse_fun_pre = &cssExtParseFun_pre;
  parse_prog->ext_parse_fun_post = &cssExtParseFun_post;
  parse_prog->ext_parse_user_data = (void*)this;
  // use this for debugging exprs:
  //  parse_prog->SetDebug(6);

  parse_prog->CompileCode(parse_expr);   // use css to do all the parsing!

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

  parse_tmp->Reset();
  parse_prog->ClearAll();

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

bool ProgExprBase::ExprLookupVarFilter(void* base_, void* var_) {
  if(!base_) return true;
  Program* prog = dynamic_cast<Program*>(static_cast<taBase*>(base_));
  if(!prog) return true;
  ProgVar* var = dynamic_cast<ProgVar*>(static_cast<taBase*>(var_));
  if(!var || !var->HasVarFlag(ProgVar::LOCAL_VAR)) return true; // definitely all globals
  Function* varfun = GET_OWNER(var, Function);
  if(!varfun) return true;      // not within a function, always go -- can't really tell scoping very well at this level -- could actually do it but it would be recursive and hairy
  if(!expr_lookup_cur_base) return true; // no filter possible
  Function* basefun = GET_OWNER(expr_lookup_cur_base, Function);
  if(basefun != varfun) return false; // different function scope
  return true;
}


String ProgExprBase::ExprLookupFun(const String& cur_txt, int cur_pos, int& new_pos,
                                   taBase*& path_own_obj, TypeDef*& path_own_typ,
                                   MemberDef*& path_md, ProgExprBase* expr_base,
                                   Program* own_prg, Function* own_fun,
                                   taBase* path_base, TypeDef* path_base_typ) {
  path_own_obj = NULL;
  path_own_typ = NULL;
  path_md = NULL;

  String txt = cur_txt.before(cur_pos);
  String extra_txt = cur_txt.from(cur_pos);
  String append_at_end;
  String prepend_before;

  String base_path;             // path to base element(s) if present
  String lookup_seed;           // start of text to seed lookup process
  String rval = _nilString;

  int lookup_type = -1; // 1 = var name (no path, delim), 2 = obj memb/meth,
  // 3 = type scoped, 4 = array index

  int_Array delim_pos;
  int delims_used = 0;
  int expr_start = 0;
  int c = '\0';
  for(int i=cur_pos-1;i>= 0; i--) {
    c = txt[i];
    if(isalpha(c) || isdigit(c) || (c == '_')) continue;
    if(c == ']' || c == '[' || c == '.' || c == '>' || c == '-' || c == ':') {
      delim_pos.Add(i);
      continue;
    }
    expr_start = i+1;           // anything else is a bust
    break;
  }

  int xtra_st = extra_txt.length();
  for(int i=0;i<extra_txt.length(); i++) {
    c = extra_txt[i];
    if(isalpha(c) || isdigit(c) || (c == '_')) continue;
    xtra_st = i;
    break;
  }
  if(xtra_st < extra_txt.length()) {
    append_at_end = extra_txt.from(xtra_st);
  }

  if(delim_pos.size > 0) {
    if(txt[delim_pos[0]] == '.') { // path sep = .
      base_path = txt.at(expr_start, delim_pos[0]-expr_start);
      prepend_before = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = 2;
      delims_used = 1;
    }
    else if(txt[delim_pos[0]] == '>' && delim_pos.size > 1 && txt[delim_pos[1]] == '-'
            && (delim_pos[0] == delim_pos[1] + 1)) { // path sep = ->
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      prepend_before = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = 2;
      delims_used = 2;
    }
    else if(txt[delim_pos[0]] == ':' && delim_pos.size > 1 && txt[delim_pos[1]] == ':'
            && (delim_pos[0] == delim_pos[1] + 1)) { // path sep = ::
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      prepend_before = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = 3;
      delims_used = 2;
    }
    // todo: []
  }
  else {
    if(path_base || path_base_typ) {
      lookup_type = 2;
    }
    else {
      lookup_type = 1;
    }
    lookup_seed = txt.from(expr_start);
    prepend_before = txt.before(expr_start);
  }

  String path_prepend_before;   // for path operations
  if(delim_pos.size > 0) {
    path_prepend_before = txt.through(delim_pos[0]);
  }

  switch(lookup_type) {
  case 1: {// lookup variables
    taiWidgetTokenPtrMultiType* varlkup =  new taiWidgetTokenPtrMultiType
      (&TA_ProgVar, NULL, NULL, NULL, 0, lookup_seed);
    varlkup->setNewObj1(&(own_prg->vars), " New Global Var");
    if(expr_base) {
      ProgEl* pel = GET_OWNER(expr_base, ProgEl);
      if(pel) {
        LocalVars* pvs = pel->FindLocalVarList();
        if(pvs) {
          varlkup->setNewObj2(&(pvs->local_vars), " New Local Var");
        }
      }
    }
    varlkup->item_filter = (item_filter_fun)ProgExprBase::ExprLookupVarFilter;
    expr_lookup_cur_base = expr_base;
    varlkup->type_list.Link(&TA_ProgVar);
    varlkup->type_list.Link(&TA_DynEnumItem);
    varlkup->GetImageScoped(NULL, &TA_ProgVar, own_prg, &TA_Program);
    bool okc = varlkup->OpenChooser();
    if(okc && varlkup->token()) {
      rval = prepend_before + varlkup->token()->GetName();
      new_pos = rval.length();
      rval += append_at_end;
    }
    delete varlkup;
    expr_lookup_cur_base = NULL;
    break;
  }
  case 2: {                     // members/methods
    String path_var, path_rest;
    TypeDef* lookup_td = NULL;
    taList_impl* tal = NULL;
    taBase* base_base = NULL;
    TypeDef* own_td = NULL;
    if(path_base) {
      base_base = path_base;
      path_rest = base_path;
      own_td = path_base_typ;
    }
    else if(path_base_typ) {
      own_td = path_base_typ;
      path_rest = base_path;
    }
    else {
      if(delim_pos.size > delims_used) {
        // note: any ref to base path needs to subtract expr_start relative to delim_pos!
        path_var = base_path.before(delim_pos.SafeEl(-1)-expr_start); // use last one = first in list
        if(delim_pos.size > delims_used+1 && delim_pos.SafeEl(-2) == delim_pos.SafeEl(-1)+1)
          path_rest = base_path.after(delim_pos.SafeEl(-2)-expr_start);
        else
          path_rest = base_path.after(delim_pos.SafeEl(-1)-expr_start);
      }
      else {
        path_var = base_path;
      }
      ProgVar* st_var = NULL;
      if(own_fun)
        st_var = own_fun->FindVarName(path_var);
      if(!st_var)
        st_var = own_prg->FindVarName(path_var);
      if(st_var) {
        if(st_var->var_type == ProgVar::T_Object) {
          if(!st_var->object_type) {
            taMisc::Info("Var lookup: cannot lookup anything about variable:", path_var,
                         "because it is an Object* but has no type set yet!");
          }
          else {
            own_td = st_var->object_type;
            if(path_rest.empty()) {
              lookup_td = st_var->object_type;
            }
            else {
              base_base = st_var->object_val;
            }
          }
        }
        else if(st_var->var_type == ProgVar::T_String) {
          lookup_td = &TA_taString;
        }
      }
      else {
        taMisc::Info("Var lookup: cannot find variable:", path_var,
                     "as start of lookup path:", base_path);
      }
    }
    if(base_base && !lookup_td) {
      MemberDef* md = NULL;
      taBase* mb_tab = base_base->FindFromPath(path_rest, md);
      if(mb_tab) {
        lookup_td = mb_tab->GetTypeDef();
        if(lookup_td->InheritsFrom(&TA_taList_impl))
          tal = (taList_impl*)mb_tab;
      }
      else {
        if(md) lookup_td = md->type;
      }
    }
    if(!lookup_td && own_td) {
      int net_base_off=0;
      ta_memb_ptr net_mbr_off=0;
      MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off,
                                                    net_mbr_off, path_rest, false);
      // no warn
      if(md) lookup_td = md->type;
    }
    if(!lookup_td) {
      taMisc::Info("Var lookup: cannot find path:", path_rest, "in variable:",
                   path_var);
    }
    if(tal) {
      if(tal->InheritsFrom(&TA_taGroup_impl)) {
        taiGroupElsButton* lilkup = new taiGroupElsButton(lookup_td, NULL, NULL, NULL,
                                                          0, lookup_seed);
        lilkup->GetImage((taGroup_impl*)tal, NULL);
        bool okc = lilkup->OpenChooser();
        if(okc && lilkup->item()) {
          path_own_obj = lilkup->item();
          path_own_typ = path_own_obj->GetTypeDef();
          rval = path_prepend_before + path_own_obj->GetName();
          new_pos = rval.length();
          rval += append_at_end;
        }
        delete lilkup;
      }
      else {
        taiListElsButton* lilkup = new taiListElsButton(lookup_td, NULL, NULL, NULL,
                                                        0, lookup_seed);
        lilkup->GetImage(tal, NULL);
        bool okc = lilkup->OpenChooser();
        if(okc && lilkup->item()) {
          path_own_obj = lilkup->item();
          path_own_typ = path_own_obj->GetTypeDef();
          rval = path_prepend_before + path_own_obj->GetName();
          new_pos = rval.length();
          rval += append_at_end;
        }
        delete lilkup;
      }
    }
    else if(lookup_td) {
      TypeItem* lookup_md = NULL;
      if(path_base || path_base_typ) {          // can only lookup members, not methods
        taiWidgetMemberDefPtr* mdlkup = new taiWidgetMemberDefPtr(lookup_td, NULL, NULL,
                                                            NULL, 0, lookup_seed);
        mdlkup->GetImage((MemberDef*)NULL, lookup_td);
        bool okc = mdlkup->OpenChooser();
        if(okc && mdlkup->md()) {
          lookup_md = mdlkup->md();
        }
        delete mdlkup;
      }
      else {
        taiMemberMethodDefButton* mdlkup =  new taiMemberMethodDefButton(lookup_td, NULL, NULL,
                                                                         NULL, 0, lookup_seed);
        mdlkup->GetImage((MemberDef*)NULL, lookup_td);
        bool okc = mdlkup->OpenChooser();
        if(okc && mdlkup->md()) {
          lookup_md = mdlkup->md();
        }
        delete mdlkup;
      }
      if(lookup_md) {
        rval = path_prepend_before + lookup_md->name;
        if(lookup_md->TypeInfoKind() == TypeItem::TIK_METHOD)
          rval += "()";
        new_pos = rval.length();
        rval += append_at_end;
        path_own_typ = lookup_td;
        if(lookup_md->TypeInfoKind() == TypeItem::TIK_MEMBER) {
          path_md = (MemberDef*)lookup_md;
        }
      }
    }
    break;
  }
  case 3: {
    TypeDef* lookup_td = TypeDef::FindGlobalTypeName(base_path, false);
    if(lookup_td) {
      taiWidgetEnumStaticChooser* eslkup =  new taiWidgetEnumStaticChooser(lookup_td, NULL, NULL,
                                                             NULL, 0, lookup_seed);
      eslkup->GetImage((MemberDef*)NULL, lookup_td);
      bool okc = eslkup->OpenChooser();
      if(okc && eslkup->md()) {
        rval = path_prepend_before + eslkup->md()->name;
        if(eslkup->md()->TypeInfoKind() == TypeItem::TIK_METHOD)
          rval += "()";
        new_pos = rval.length();
        rval += append_at_end;
      }
      delete eslkup;
    }
    else {                      // now try for local enums
      ProgType* pt = own_prg->types.FindName(base_path);
      if(pt && pt->InheritsFrom(&TA_DynEnumType)) {
        taiWidgetTokenPtr* varlkup =  new taiWidgetTokenPtr(&TA_DynEnumItem, NULL, NULL,
                                                            NULL, 0, lookup_seed);
        varlkup->GetImageScoped(NULL, &TA_DynEnumItem, pt, &TA_DynEnumType); // scope to this guy
        bool okc = varlkup->OpenChooser();
        if(okc && varlkup->token()) {
          rval = prepend_before + varlkup->token()->GetName();
          new_pos = rval.length();
          rval += append_at_end;
        }
        delete varlkup;
      }
    }
    break;
  }
  case 4: {
    taMisc::Info("lookup an array index from path:", base_path, "seed:", lookup_seed);
    break;
  }
  }

  return rval;
}

String ProgExprBase::StringFieldLookupFun(const String& cur_txt, int cur_pos,
                                          const String& mbr_name, int& new_pos) {

  Program* own_prg = GET_MY_OWNER(Program);
  if(!own_prg) return _nilString;
  Function* own_fun = GET_MY_OWNER(Function);
  taBase* path_own_obj = NULL;
  TypeDef* path_own_typ = NULL;
  MemberDef* path_md = NULL;
  return ProgExprBase::ExprLookupFun(cur_txt, cur_pos, new_pos,
                                     path_own_obj, path_own_typ, path_md,
                                     this, own_prg, own_fun);
}

String MemberProgEl::StringFieldLookupFun(const String& cur_txt, int cur_pos,
                                          const String& mbr_name, int& new_pos) {

  String rval = _nilString;
  if(!obj) {
    obj_type = &TA_taBase; // placeholder
    return rval;
  }
  TypeDef* path_base_typ = obj->act_object_type();
  taBase* path_base = NULL;
  if((bool)obj->object_val) {
    path_base = obj->object_val;
  }

  Program* own_prg = GET_MY_OWNER(Program);
  if(!own_prg) return _nilString;
  Function* own_fun = GET_MY_OWNER(Function);
  taBase* path_own_obj = NULL;
  TypeDef* path_own_typ = NULL;
  MemberDef* path_md = NULL;
  rval = ProgExprBase::ExprLookupFun(cur_txt, cur_pos, new_pos,
                                     path_own_obj, path_own_typ, path_md,
                                     NULL, own_prg, own_fun, path_base, path_base_typ);

  if(path_own_typ) {
    obj_type = path_own_typ;
  }

  return rval;
}
