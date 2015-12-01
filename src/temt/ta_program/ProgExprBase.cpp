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
#include <Program_Group>
#include <Function>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taString>
#include <css_machine.h>
#include <css_ta.h>
#include <css_c_ptr_types.h>
#include <taiWidgetTokenChooserMultiType>
#include <LocalVars>
#include <taiWidgetGroupElChooser>
#include <taiWidgetListElChooser>
#include <taiWidgetMemberDefChooser>
#include <taiWidgetMemberMethodDefChooser>
#include <taiWidgetEnumStaticChooser>
#include <taiWidgetTokenChooser>
#include <MemberProgEl>
#include <SigLinkSignal>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(ProgExprBase);
TA_BASEFUNS_CTORS_DEFN(ProgExprShort);


cssProgSpace* ProgExprBase::parse_prog = NULL;
cssSpace* ProgExprBase::parse_tmp = NULL;

static ProgEl* expr_lookup_cur_base = NULL;

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
    if (pel) {
      if(!taMisc::is_loading && bad_vars.size > 0) {
        for(int i=0; i<bad_vars.size; i++) {
          pel->FindVarNameInScope(bad_vars[i], true);
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
        taMisc::CheckError
          ("ProgExpr in program element:", pel->GetDisplayName(),"\n in program:", prg->name," Errors in expression -- the following variable names could not be found:",
           bad_vars[0],
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
  if(sls != SLS_ITEM_UPDATED || !obj || !obj->InheritsFrom(&TA_ProgVar)) {
    return;
  }
  ProgVar* pv = (ProgVar*)obj;
  if (!pv->schemaChanged()) {
    // taMisc::DebugInfo("updating expr:", expr, "b/c of ProgVar:", pv->name,
    //                   "schema NOT changed!, sig:", String(sls));
    return;
  }
  // taMisc::DebugInfo("updating expr:", expr, "b/c of ProgVar:", pv->name,
  //                   "sig:", String(sls));

  expr = GetFullExpr();         // update our expr to reflect any changes in variables.
  SigEmitUpdated();             // update any specific display reference to us
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(pel) {
    pel->SigEmitUpdated();      // update display is usually sufficient
  }
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
      if(ptyp->InheritsFrom(&TA_DynEnumBase)) {
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

  if(parse_expr.endsWith("};"))                            // cleanup this for CssExpr
    parse_expr = parse_expr.before(parse_expr.length()-1); // cut off last ;

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
    if(!TestError((!vrf->ptr()), "ParseExpr", "vrf->ptr() == NULL -- this shouldn't happen -- report as a bug!")) {
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
  if(vars.size == 0) {
    if(var_expr.empty()) return expr; // haven't been parsed yet -- allows display name to still show something for OFF guys
    return var_expr;                  // have, its ok.
  }
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

bool ProgExprBase::ExprLookupVarFilter(void* base_, void* var_) {
  if(!base_)
    return true;
  Program* prog = dynamic_cast<Program*>(static_cast<taBase*>(base_));
  if(!prog)
    return true;
  ProgVar* var = dynamic_cast<ProgVar*>(static_cast<taBase*>(var_));
  if(!var || !var->HasVarFlag(ProgVar::LOCAL_VAR))
    return true; // definitely all globals
  Function* varfun = GET_OWNER(var, Function);
  if(!varfun)
    return true;      // not within a function, always go -- can't really tell scoping very well at this level -- could actually do it but it would be recursive and hairy
  if(!expr_lookup_cur_base)
    return true; // no filter possible
  Function* basefun = GET_OWNER(expr_lookup_cur_base, Function);
  if(basefun != varfun)
    return false; // different function scope
  return true;
}

bool ProgExprBase::FindPathSeparator(const String& path, int& separator_start, int& separator_end, bool backwards) {
  int dot_pos = path.index('.', -1);
  int arrow_tip_pos = path.index('>', -1);
  
  if (arrow_tip_pos != -1 && path[arrow_tip_pos-1] == '-' && arrow_tip_pos > dot_pos) {
    separator_end = arrow_tip_pos;
    separator_start = separator_end-1;
    return true;
  }
  if (dot_pos != -1) {
    separator_end = dot_pos;
    separator_start = separator_end;
    return true;
  }
  return false;
}

ProgExprBase::LookUpType ProgExprBase::ParseForLookup(const String& cur_txt, int cur_pos, String& prepend_txt, String& path_prepend_txt, String& append_txt, String& prog_el_txt, String& base_path, String& lookup_seed, String& path_var, String& path_rest, bool path_base_not_null, int& expr_start, bool& lookup_group_default) {
  
  String txt = cur_txt.before(cur_pos);
  String extra_txt = cur_txt.from(cur_pos);
  
  int       prog_el_start_pos = -1;
  int       c = '\0';
  int       c_next = '\0';
  int       c_previous = '\0';
  int       left_parens_cnt = 0;
  int       right_parens_cnt = 0;
  int       delims_used = 0;
  int_Array delim_pos;
  
  // ** Working backwards - delimiters will be in reverse order **
  for(int i=cur_pos-1; i>= 0; i--) {
    c = txt[i];
    if(isdigit(c) || (c == '_')) {
      continue;
    }
    if(c == ' ') {
      if (i != txt.length()-1) {  // not the last char
        c_next = txt[i+1];
        if (c_next == '(' || c_next == ',' || c_next == ' ') {
          continue;
        }
      }
      if (i > 0) {
        c_previous = txt[i-1];
        if (c_previous == '(' || c_previous == ',' || c_previous == ' ') {
          continue;
        }
      }
      else {
        expr_start = i+1;
        break;
      }
    }
    if(isalpha(c)) {
      if (delim_pos.size > 0) {  // we only collect chars just before the preceeding position
        continue;
      }
      else {
        prog_el_start_pos = i;
        continue;
      }
    }
    if(c == ']' || c == '[' || c == '.' || c == '>' || c == '-' || c == ':') {
      delim_pos.Add(i);
      continue;
    }
    if (c == '(') {
      left_parens_cnt++;
      if (left_parens_cnt > right_parens_cnt) {
        expr_start = i+1;
        break;
      }
      else {
        delim_pos.Add(i);
        continue;
      }
    }
    if (c == ')') {
      right_parens_cnt++;
      delim_pos.Add(i);
      continue;
    }
    expr_start = i+1;           // anything else is a bust
    break;
  }
  
  if (prog_el_start_pos == 0 && delims_used == 0) {
    // we have something like 'Print some_var'
    // expr start will be the some_var part
    expr_start = txt.index(' ', -1) + 1;
  }
  
  int xtra_st = extra_txt.length();
  for(int i=0;i<extra_txt.length(); i++) {
    c = extra_txt[i];
    if(isalpha(c) || isdigit(c) || (c == '_')) continue;
    xtra_st = i;
    break;
  }
  if(xtra_st < extra_txt.length()) {
    append_txt = extra_txt.from(xtra_st);
  }
  
  ProgExprBase::LookUpType lookup_type = NOT_SET;
  
  if(delim_pos.size > 0) {
    if(delim_pos.size > 1 && txt[delim_pos[1]] == ')' && txt[delim_pos[0]] == '.') { // path sep = .
      base_path = txt.at(expr_start, delim_pos[0]-expr_start);
      int length = base_path.length();
      base_path = triml(base_path);
      int shift = length - base_path.length(); // shift to compensate for trim
      expr_start += shift;
      prepend_txt = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = ProgExprBase::METHOD;
      delims_used = delim_pos.size;  // this will keep us from falling into the conditional "delim_pos.size > delims_used" until that bit is eliminated
      
      int left_parens_pos = base_path.index('(', -1);
      path_rest = base_path.before(left_parens_pos);
      
      int separator_start = -1;
      int separator_end = -1;
      // check for both '.' and "->" delimiters
      bool has_separator = FindPathSeparator(path_rest, separator_start, separator_end, true);
      if (has_separator) {
        path_rest = path_rest.after(separator_end);
        path_var = base_path.before(separator_start);
      }
      // path_var is at the start of the path - handles cases where path has more than one separator
      // we still don't handle var.member.method. or similar paths with intermediate members/methods
      if (FindPathSeparator(path_var, separator_start, separator_end, false)) {
        path_var = path_var.before(separator_start);
      }
    }
    else if(txt[delim_pos[0]] == '.') { // path sep = .
      base_path = txt.at(expr_start, delim_pos[0]-expr_start);
      int length = base_path.length();
      base_path = triml(base_path);
      int shift = length - base_path.length(); // shift to compensate for trim
      expr_start += shift;
      prepend_txt = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = ProgExprBase::OBJ_MEMB_METH;
      delims_used = 1;
      if (delim_pos.size > 1 && txt[delim_pos[1]] == ']') {
        lookup_group_default = true;
      }
    }
    else if(delim_pos.size > 2 && txt[delim_pos[2]] == ')' && txt[delim_pos[0]] == '>' && txt[delim_pos[1]] == '-'
            && (delim_pos[0] == delim_pos[1] + 1)) { // path sep = ->
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      int length = base_path.length();
      base_path = triml(base_path);
      int shift = length - base_path.length(); // shift to compensate for trim
      expr_start += shift;
      prepend_txt = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = ProgExprBase::METHOD;
      delims_used = delim_pos.size;  // this will keep us from falling into the conditional "delim_pos.size > delims_used" until that bit is eliminated
      
      // check for both '.' and "->" delimiters
      int left_parens_pos = base_path.index('(', -1);
      path_rest = base_path.before(left_parens_pos);
      
      int separator_start = -1;
      int separator_end = -1;
      // check for both '.' and "->" delimiters
      bool has_separator = FindPathSeparator(path_rest, separator_start, separator_end, true);
      if (has_separator) {
        path_rest = path_rest.after(separator_end);
        path_var = base_path.before(separator_start);
      }
      // path_var is at the start of the path - handles cases where path has more than one separator
      // we still don't handle var.member.method. or similar paths with intermediate members/methods
      if (FindPathSeparator(path_var, separator_start, separator_end, false)) {
        path_var = path_var.before(separator_end);
      }
    }
    else if(delim_pos.size > 1 && txt[delim_pos[0]] == '>' && txt[delim_pos[1]] == '-'
            && (delim_pos[0] == delim_pos[1] + 1)) { // path sep = ->
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      int length = base_path.length();
      base_path = triml(base_path);
      int shift = length - base_path.length(); // shift to compensate for trim
      expr_start += shift;
      prepend_txt = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = ProgExprBase::OBJ_MEMB_METH;
      delims_used = 2;
      if (delim_pos.size > 2 && txt[delim_pos[2]] == ']') {
        lookup_group_default = true;
      }
    }
    else if(delim_pos.size > 1 && txt[delim_pos[0]] == ':' && txt[delim_pos[1]] == ':'
            && (delim_pos[0] == delim_pos[1] + 1)) { // path sep = ::
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      int length = base_path.length();
      base_path = triml(base_path);
      int shift = length - base_path.length(); // shift to compensate for trim
      expr_start += shift;
      prepend_txt = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = ProgExprBase::SCOPED;
      delims_used = 2;
    }
    else if(delim_pos.size > 1 && txt[delim_pos[0]] == ')' && txt[delim_pos[1]] == '(') { // program () which can be followed by a function in that program
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      int length = base_path.length();
      base_path = triml(base_path);
      int shift = length - base_path.length(); // shift to compensate for trim
      expr_start += shift;
      prepend_txt = txt.before(txt[delim_pos[1]]);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = ProgExprBase::PROGRAM_FUNCTION;
      delims_used = 2;
    }
    else if(txt[delim_pos[0]] == '(') { // handles method arguments and more - cases such as my_method(xxx or if(
      base_path = txt.at(expr_start, delim_pos[0]-expr_start);
      int length = base_path.length();
      base_path = triml(base_path);
      int shift = length - base_path.length(); // shift to compensate for trim
      expr_start += shift;
      prepend_txt = txt.through(txt[delim_pos[0]]);
      lookup_seed = txt.after(delim_pos[0]);
      
      // we might have a string of arguments - only use text after last comma as seed
      int last_comma_pos = lookup_seed.index(',', -1);
      String addl_prepend = lookup_seed.through(last_comma_pos);
      prepend_txt += addl_prepend;
      lookup_seed = lookup_seed.after(last_comma_pos);
      
      lookup_type = ProgExprBase::VARIOUS;
      delims_used = 1;
    }
  }
  else {
    prog_el_txt = txt.at(prog_el_start_pos, txt.length() - prog_el_start_pos);
    
    if (prog_el_start_pos > -1 && ExprLookupIsFunc(prog_el_txt)) {
      lookup_type = ProgExprBase::CALL;
      expr_start = txt.length();
      prepend_txt = txt.before(expr_start);
    }
    // rohrlich - this will be true if the code is already compiled and that is causing a problem - see bug 2453
    // - honestly I don't know of any situation that uses this and I think we should always take the code as uncompiled
    // so commenting out to see if there are repercussions
//    else if(path_base_not_null) {
//      lookup_type = ProgExprBase::OBJ_MEMB_METH;
//      lookup_seed = txt.from(expr_start);
//      prepend_txt = txt.before(expr_start);
//    }
    else {
      lookup_type = ProgExprBase::VARIOUS;
      lookup_seed = txt.from(expr_start);
      prepend_txt = txt.before(expr_start);
    }
  }
  
  // cases such as var.member.<lookup>
  
  if(delim_pos.size > delims_used) {
    // is one of the delimiters a '(' - we want to start from there unless balanced with right parens
    int left_parens_pos = base_path.index('(', cur_pos-base_path.length());
    if (left_parens_pos != -1) {
      int parens_index = 0;
      // which delimiter is it
      parens_index = delim_pos.FindEl(left_parens_pos);
      // path_var will be the string between left_parens_pos and the next delimiter pos to the right
      path_var = base_path.at(left_parens_pos + 1, delim_pos[parens_index - 1] - left_parens_pos - 1);
      path_var = path_var.trim();
      
      if(delim_pos.size > delims_used+1 && delim_pos.SafeEl(-2) == delim_pos.SafeEl(-1)+1)
        path_rest = base_path.after(delim_pos.SafeEl(-2)-expr_start);
      else
        path_rest = base_path.after(delim_pos.SafeEl(parens_index-1)-expr_start);
    }
    else {
      // note: any ref to base path needs to subtract expr_start relative to delim_pos!
      path_var = base_path.before(delim_pos.SafeEl(-1)-expr_start); // use last one = first in list
      path_var = path_var.trim();
      if(delim_pos.size > delims_used+1 && delim_pos.SafeEl(-2) == delim_pos.SafeEl(-1)+1)
        path_rest = base_path.after(delim_pos.SafeEl(-2)-expr_start);
      else
        path_rest = base_path.after(delim_pos.SafeEl(-1)-expr_start);
    }
  }
  
  if(delim_pos.size > 0) {
    path_prepend_txt = txt.through(delim_pos[0]);
  }
  
  lookup_seed.trim();
  return lookup_type;
}


String ProgExprBase::ExprLookupFun(const String& cur_txt, int cur_pos, int& new_pos,
                                   taBase*& path_own_obj, TypeDef*& path_own_typ,
                                   MemberDef*& path_md, ProgEl* own_pel,
                                   Program* own_prg, Function* own_fun,
                                   taBase* path_base, TypeDef* path_base_typ) {
  
  String txt = cur_txt.before(cur_pos);
  String append_txt;
  String prepend_txt;
  String path_prepend_txt;
  String prog_el_txt;
  String path_var;
  String path_rest;
  String base_path;             // path to base element(s) if present
  String lookup_seed;           // start of text to seed lookup process

  int   lookup_type;
  int   expr_start = 0;
  bool  lookup_group_default = false;
  
  bool path_base_not_null = false;
  // Rohrlich - see note in the parse code - remove this variable if all goes well
//  bool path_base_not_null = (path_base_typ != NULL || path_base != NULL);
  lookup_type = ParseForLookup(cur_txt, cur_pos, prepend_txt, path_prepend_txt, append_txt, prog_el_txt, base_path, lookup_seed, path_var, path_rest, path_base_not_null, expr_start, lookup_group_default);

  String rval = _nilString;
  path_own_obj = NULL;
  path_own_typ = NULL;
  path_md = NULL;

  switch(lookup_type) {
    case ProgExprBase::VARIOUS: {  // multiple possibilities
      taiWidgetTokenChooserMultiType* varlkup =  new taiWidgetTokenChooserMultiType
      (&TA_ProgVar, NULL, NULL, NULL, 0, lookup_seed);
      varlkup->setNewObj1(&(own_prg->vars), " New Global Var");
      if(own_pel) {
        LocalVars* pvs = own_pel->FindLocalVarList();
        if(pvs) {
          varlkup->setNewObj2(&(pvs->local_vars), " New Local Var");
        }
      }
      varlkup->item_filter = (item_filter_fun)ProgExprBase::ExprLookupVarFilter;
      expr_lookup_cur_base = own_pel;
      varlkup->type_list.Link(&TA_ProgVar);
      varlkup->type_list.Link(&TA_DynEnumItem);
      varlkup->type_list.Link(&TA_Function);
      if (expr_start == 0) {  // program calls must be at beginning of line
        varlkup->type_list.Link(&TA_Program);
      }
      varlkup->GetImageScoped(NULL, &TA_ProgVar, own_prg, &TA_Program);
      bool okc = varlkup->OpenChooser();
      if(okc && varlkup->token()) {
        rval = prepend_txt + varlkup->token()->GetName();
        String type_name = varlkup->token()->GetTypeName();
        if (type_name == "Program" || type_name == "Function") {
          rval += "()";
          new_pos = rval.length();
        }
        else {
          new_pos = rval.length();
          rval += append_txt;
        }
      }
      delete varlkup;
      expr_lookup_cur_base = NULL;
      break;
    }
      
    case ProgExprBase::OBJ_MEMB_METH: {                     // members/methods
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
        if (path_var.empty()) {
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
          else if(st_var->var_type == ProgVar::T_DynEnum) {
            lookup_td = &TA_DynEnum;
          }
        }
        else {
          taMisc::Info("Var lookup: cannot find variable:", path_var, "as start of lookup path:", base_path);
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
        if(md)
          lookup_td = md->type;
        
        if (lookup_td && lookup_group_default) { // lookup is for group member vs group - i.e. group[0]. vs group.
          taBase* tab = tabMisc::root->GetTemplateInstance(lookup_td);
          if (tab && tab->InheritsFrom(&TA_taList_impl)) {
            taList_impl* some_list = (taList_impl*)tab;
            lookup_td = some_list->GetElType();
          }
        }
      }
      if(!lookup_td) {
        taMisc::Info("Var lookup: cannot find path:", path_rest, "in variable:",
                     path_var);
      }
      if(tal) {
        if(tal->InheritsFrom(&TA_taGroup_impl)) {
          taiWidgetGroupElChooser* lilkup = new taiWidgetGroupElChooser
            (lookup_td, NULL, NULL, NULL, 0, lookup_seed);
          lilkup->GetImage((taGroup_impl*)tal, NULL);
          bool okc = lilkup->OpenChooser();
          if(okc && lilkup->item()) {
            path_own_obj = lilkup->item();
            path_own_typ = path_own_obj->GetTypeDef();
            rval = path_prepend_txt + path_own_obj->GetName();
            new_pos = rval.length();
            rval += append_txt;
          }
          delete lilkup;
        }
        else {
          taiWidgetListElChooser* lilkup = new taiWidgetListElChooser
            (lookup_td, NULL, NULL, NULL, 0, lookup_seed);
          lilkup->GetImage(tal, NULL);
          bool okc = lilkup->OpenChooser();
          if(okc && lilkup->item()) {
            path_own_obj = lilkup->item();
            path_own_typ = path_own_obj->GetTypeDef();
            rval = path_prepend_txt + path_own_obj->GetName();
            new_pos = rval.length();
            rval += append_txt;
          }
          delete lilkup;
        }
      }
      else if(lookup_td) {
        TypeItem* lookup_md = NULL;
        if(path_base || path_base_typ) {          // can only lookup members, not methods
          taiWidgetMemberDefChooser* mdlkup = new taiWidgetMemberDefChooser
            (lookup_td, NULL, NULL, NULL, 0, lookup_seed);
          mdlkup->GetImage((MemberDef*)NULL, lookup_td);
          bool okc = mdlkup->OpenChooser();
          if(okc && mdlkup->md()) {
            lookup_md = mdlkup->md();
          }
          delete mdlkup;
        }
        else {
          taiWidgetMemberMethodDefChooser* mdlkup = new taiWidgetMemberMethodDefChooser
            (lookup_td, NULL, NULL, NULL, 0, lookup_seed);
          mdlkup->GetImage((MemberDef*)NULL, lookup_td);
          bool okc = mdlkup->OpenChooser();
          if(okc && mdlkup->md()) {
            lookup_md = mdlkup->md();
          }
          delete mdlkup;
        }
        if(lookup_md) {
          rval = path_prepend_txt + lookup_md->name;
          if(lookup_md->TypeInfoKind() == TypeItem::TIK_METHOD)
            rval += "()";
          new_pos = rval.length();
          rval += append_txt;
          path_own_typ = lookup_td;
          if(lookup_md->TypeInfoKind() == TypeItem::TIK_MEMBER) {
            path_md = (MemberDef*)lookup_md;
          }
        }
      }
      break;
    }
      
    case ProgExprBase::METHOD: {
      TypeDef* lookup_td = NULL;
      ProgVar* st_var = NULL;
      st_var = own_prg->FindVarName(path_var);
      if (st_var) {
        TypeDef* td = st_var->object_type;
        if (td) {
          MethodDef* md = td->methods.FindName(path_rest);
          if (md) {
            lookup_td = md->type;  // get the return type
          }
        }
      }
      if (lookup_td) {
        TypeItem* lookup_md = NULL;
        taiWidgetMemberMethodDefChooser* mdlkup = new taiWidgetMemberMethodDefChooser
        (lookup_td, NULL, NULL, NULL, 0, lookup_seed);
        mdlkup->GetImage((MemberDef*)NULL, lookup_td);
        bool okc = mdlkup->OpenChooser();
        if(okc && mdlkup->md()) {
          lookup_md = mdlkup->md();
        }
        delete mdlkup;
        
        if(lookup_md) {
          rval = path_prepend_txt + lookup_md->name;
          if(lookup_md->TypeInfoKind() == TypeItem::TIK_METHOD)
            rval += "()";
          new_pos = rval.length();
          rval += append_txt;
        }
      }
      break;
    }
      
    case ProgExprBase::SCOPED: {                      // enums
      TypeDef* lookup_td = TypeDef::FindGlobalTypeName(base_path, false);
      if(lookup_td) {
        taiWidgetEnumStaticChooser* eslkup = new taiWidgetEnumStaticChooser
          (lookup_td, NULL, NULL, NULL, 0, lookup_seed);
        eslkup->GetImage((MemberDef*)NULL, lookup_td);
        bool okc = eslkup->OpenChooser();
        if(okc && eslkup->md()) {
          rval = path_prepend_txt + eslkup->md()->name;
          if(eslkup->md()->TypeInfoKind() == TypeItem::TIK_METHOD)
            rval += "()";
          new_pos = rval.length();
          rval += append_txt;
        }
        delete eslkup;
      }
      else {                      // now try for local enums
        ProgType* pt = own_prg->types.FindName(base_path);
        if(pt && pt->InheritsFrom(&TA_DynEnumBase)) {
          taiWidgetTokenChooser* varlkup = new taiWidgetTokenChooser
            (&TA_DynEnumItem, NULL, NULL, NULL, 0, lookup_seed);
          varlkup->GetImageScoped(NULL, &TA_DynEnumItem, pt, &TA_DynEnumBase); // scope to this guy
          bool okc = varlkup->OpenChooser();
          if(okc && varlkup->token()) {
            rval = prepend_txt + varlkup->token()->GetName();
            new_pos = rval.length();
            rval += append_txt;
          }
          delete varlkup;
        }
      }
      break;
    }
    case ProgExprBase::ARRAY_INDEX: {
      taMisc::Info("lookup an array index from path:", base_path, "seed:", lookup_seed);
      break;
    }
      
    case ProgExprBase::CALL: {                 // ProgEl
      String trimmed_txt = trim(prog_el_txt);
      String el = trimmed_txt; // the program element
      if(trimmed_txt.contains(' ')) {
        lookup_seed = trimmed_txt.after(' ',-1);
        el = trimmed_txt.before(' ');
      }
      if (el.downcase() == "call" || el.downcase().startsWith("prog")) {
        taiWidgetTokenChooser* pgrm_look_up =  new taiWidgetTokenChooser
          (&TA_Program, NULL, NULL, NULL, 0, lookup_seed);
        pgrm_look_up->GetImageScoped(NULL, &TA_Program, NULL, &TA_Program); // scope to this guy
        bool okc = pgrm_look_up->OpenChooser();
        if(okc && pgrm_look_up->token()) {
          rval = pgrm_look_up->token()->GetName();
          rval += "()";
        }
        new_pos = rval.length();
        delete pgrm_look_up;
        break;
      }
      else if (el.downcase().startsWith("fun")) {
        taiWidgetTokenChooser* func_look_up =  new taiWidgetTokenChooser
          (&TA_Function, NULL, NULL, NULL, 0, lookup_seed);
        func_look_up->GetImageScoped(NULL, &TA_Function, NULL, &TA_Function); // scope to this guy
        bool okc = func_look_up->OpenChooser();
        if(okc && func_look_up->token()) {
          rval = prepend_txt.repl(prog_el_txt, func_look_up->token()->GetName());
          rval += "()";
        }
        new_pos = rval.length();
        delete func_look_up;
        break;
      }
    }
      
    case ProgExprBase::PROGRAM_FUNCTION: {                 // call a function in a specific program
      taiWidgetTokenChooser* func_look_up =  new taiWidgetTokenChooser
      (&TA_Function, NULL, NULL, NULL, 0, lookup_seed);
      // scope functions to the containing program - not this program
      taProject* my_proj = own_prg->GetMyProj();
      String scoped_prog_name = trim(prepend_txt);
      Program* scope_program = (Program*)my_proj->programs.FindLeafName_(scoped_prog_name);
      func_look_up->GetImageScoped(NULL, &TA_Function, scope_program, &TA_Program); // scope to this guy
      bool okc = func_look_up->OpenChooser();
      if(okc && func_look_up->token()) {
        taBase* tok = func_look_up->token();
        rval = prepend_txt + "() " + tok->GetName();
        rval += "()";
      }
      new_pos = rval.length();
      delete func_look_up;
      break;
    }
  }
  
  return rval;
}

String ProgExprBase::StringFieldLookupFun(const String& cur_txt, int cur_pos,
                                          const String& mbr_name, int& new_pos) {
  
  ProgEl* own_pel = GET_MY_OWNER(ProgEl);
  if(!own_pel)
    return _nilString;
  Program* own_prg = GET_OWNER(own_pel, Program);
  if(!own_prg)
    return _nilString;
  Function* own_fun = GET_OWNER(own_pel, Function);
  taBase* path_own_obj = NULL;
  TypeDef* path_own_typ = NULL;
  MemberDef* path_md = NULL;
  
  return ProgExprBase::ExprLookupFun(cur_txt, cur_pos, new_pos,
                                     path_own_obj, path_own_typ, path_md,
                                     own_pel, own_prg, own_fun);
}

bool ProgExprBase::ExprLookupIsFunc(const String& txt) {
  String trimmed_txt = trim(txt);
  trimmed_txt.downcase();
  if(trimmed_txt.contains(' ')) {
    trimmed_txt = trimmed_txt.before(' ');
  }
  return (trimmed_txt == "call");
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
                                     this, own_prg, own_fun, path_base, path_base_typ);

  if(path_own_typ) {
    obj_type = path_own_typ;
  }

  return rval;
}

int ProgExprBase::Test_ParseForLookup(const String test_name, const String input_text, const int cursor_pos,
                                      String& lookup_seed, String& prepend_txt, String& append_txt,
                                      String& prog_el_txt, String& path_var, String& path_prepend_txt,
                                      String& path_rest, String& base_path, bool& lookup_group_default) {
  int     lookup_type = 0;
  bool    path_base_not_null = false;
  int     expr_start = 0;
  
  lookup_type = ParseForLookup(input_text, cursor_pos, prepend_txt, path_prepend_txt, append_txt, prog_el_txt, base_path, lookup_seed, path_var, path_rest, path_base_not_null, expr_start, lookup_group_default);
  
//  taMisc::DebugInfo("lookup_type = ", (String)lookup_type);
//  taMisc::DebugInfo("lookup_seed = ", lookup_seed);
//  taMisc::DebugInfo("prepend_txt = ", prepend_txt);
//  taMisc::DebugInfo("path_prepend_txt = ", path_prepend_txt);
//  taMisc::DebugInfo("append_txt = ", append_txt);
//  taMisc::DebugInfo("prog_el_txt = ", prog_el_txt);
//  taMisc::DebugInfo("prepend_txt = ", prepend_txt);
//  taMisc::DebugInfo("path_var = ", path_var);
//  taMisc::DebugInfo("path_rest = ", path_rest);
  
  return lookup_type;
}



