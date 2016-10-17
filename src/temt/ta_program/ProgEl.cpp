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

#include "ProgEl.h"
#include <Program>
#include <Function>
#include <taiWidgetItemChooser>
#include <MemberDef>
#include <ProgExprBase>
#include <ProgArg_List>
#include <ProgExpr_List>
#include <LocalVars>
#include <Program_Group>
#include <taProject>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <ProgCode>
#include <ProgElChoiceDlg>
#include <SigLinkSignal>

#include <taMisc>

#include <css_machine.h>

TA_BASEFUNS_CTORS_DEFN(ProgEl);
SMARTREF_OF_CPP(ProgEl);

bool ProgEl::StdProgVarFilter(void* base_, void* var_) {
  if (!base_)
    return true;
  if (!var_)
    return true;
  ProgEl* base = static_cast<ProgEl*>(base_);
  ProgVar* var = static_cast<ProgVar*>(var_);
  if (!var->IsLocal())
    return true; // definitely all globals
  Function* varfun = GET_OWNER(var, Function);
  if (!varfun)
    return true;        // not within a function, always go -- can't really tell scoping very well at this level -- could actually do it but it would be recursive and hairy
  Function* basefun = GET_OWNER(base, Function);
  if (basefun != varfun)
    return false; // different function scope
  return true;
}

bool ProgEl::NewProgVarCustChooser(taBase* base, taiWidgetItemChooser* chooser) {
  if(!chooser || !base)
    return false;
  Program* own_prg = GET_OWNER(base, Program);
  if(!own_prg)
    return false;
  chooser->setNewObj1(&(own_prg->vars), " CREATE GLOBAL VAR");
  ProgEl* pel = NULL;
  if(base->InheritsFrom(TA_ProgEl))
    pel = (ProgEl*)base;
  else
    pel = GET_OWNER(base, ProgEl);
  if(pel) {
    LocalVars* pvs = pel->FindLocalVarList();
    if(pvs) {
      chooser->setNewObj2(&(pvs->local_vars), " CREATE LOCAL VAR");
    }
    else {
      ProgEl_List* pelst = GET_OWNER(base, ProgEl_List);
      if(pelst) {
        pvs = new LocalVars;
        if (pvs && pelst->Insert(pvs, 0)) {
          chooser->setNewObj2(&(pvs->local_vars), " CREATE LOCAL VAR");
        }
      }
    }
  }
  return true;
}

bool ProgEl::ObjProgVarFilter(void* base_, void* var_) {
  bool rval = StdProgVarFilter(base_, var_);
  if(!rval) return false;
  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->var_type == ProgVar::T_Object);
}

bool ProgEl::DataProgVarFilter(void* base_, void* var_) {
  bool rval = ObjProgVarFilter(base_, var_);
  if(!rval) return false; // doesn't pass basic test

  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->object_type && var->object_type->InheritsFrom(&TA_DataTable));
}

bool ProgEl::DynEnumProgVarFilter(void* base_, void* var_) {
  bool rval = StdProgVarFilter(base_, var_);
  if(!rval) return false;
  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->var_type == ProgVar::T_DynEnum);
}

void ProgEl::Initialize() {
  edit_move_after = 0;
  flags = PEF_NONE;
  setUseStale(true);
}

void ProgEl::Destroy() {
}

void ProgEl::Copy_(const ProgEl& cp) {
  SetBaseFlag(COPYING); // ala Copy__
  desc = cp.desc;
  flags = cp.flags;
  code_string = cp.code_string;
  pre_compile_code_string = cp.pre_compile_code_string;
  ClearBaseFlag(COPYING); // ala Copy__
}

void ProgEl::UpdateProgFlags() {
  if(code_string.nonempty()) { // && ProgElChildrenCount() == 0) {
    SetProgFlag(CAN_REVERT_TO_CODE);
  }
  else {
    ClearProgFlag(CAN_REVERT_TO_CODE);
  }
}

void ProgEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgFlags();
  pre_compile_code_string = BrowserEditString();  // hold on to the current code; if compile successful copy to
  SigEmitUpdated();

  // update used flags on variables early and often!
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFromName("ProgVarRef")) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      if(pvr->ptr()) {
        ProgVar* pv = pvr->ptr();
        pv->UpdateUsedFlag_gui();
      }
    }
  }
}

void ProgEl::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);
  UpdateProgElVars(old_owner, owner); // just use our owner as new scope
}
  
void ProgEl::UpdatePointersAfterCopy_impl(const taBase& cp) {
  inherited::UpdatePointersAfterCopy_impl(cp);
  UpdateProgElVars(&cp, owner); // just use our owner as new scope
}

void ProgEl::UpdateProgElVars(const taBase* old_scope, taBase* new_scope) {
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_ProgExprBase)) {
      ProgExprBase* peb = (ProgExprBase*)md->GetOff((void*)this);
      peb->UpdateProgElVars(old_scope, new_scope);
    }
    else if(md->type->InheritsFromName("ProgVarRef")) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      if(pvr->ptr()) {
        ProgVar* new_var = Program::FindMakeProgVarInNewScope(pvr->ptr(), old_scope, new_scope);
        if(new_var) {
          pvr->set(new_var);
        }
      }
    }
    else if(md->type->InheritsFrom(&TA_ProgEl_List)) {
      ProgEl_List* pel = (ProgEl_List*)md->GetOff((void*)this);
      pel->UpdateProgElVars(old_scope, new_scope);
    }
    else if(md->type->InheritsFrom(&TA_ProgArg_List)) {
      ProgArg_List* pal = (ProgArg_List*)md->GetOff((void*)this);
      pal->UpdateProgElVars(old_scope, new_scope);
    }
    else if(md->type->InheritsFrom(&TA_ProgExpr_List)) {
      ProgExpr_List* pel = (ProgExpr_List*)md->GetOff((void*)this);
      pel->UpdateProgElVars(old_scope, new_scope);
    }
  }
}

void ProgEl::CheckError_msg(const String& a, const String& b, const String& c,
                            const String& d, const String& e, const String& f,
                            const String& g, const String& h) const {
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  String objinfo = "Config Error in Program " + prognm + ": " + GetTypeDef()->name
    + " " + GetDisplayName() + "\npath: " + GetPathNames() + "\n";
  taMisc::CheckError(objinfo, a, b, c, d, e, f, g, h);
}

bool ProgEl::CheckEqualsError(String& condition, bool quiet, bool& rval) {
  String cond = condition;
  if(cond.freq('\"') >= 2) {
    cond = cond.before('\"') + cond.after('\"',-1);
  }
  if(CheckError((cond.freq('=') == 1) && !(cond.contains(">=")
                                           || cond.contains("<=")
                                           || cond.contains("!=")),
                quiet, rval,
                "condition contains a single '=' assignment operator -- this is not the equals operator: == .  Fixed automatically")) {
    condition.gsub("=", "==");
    return true;
  }
  return false;
}

bool ProgEl::CheckProgVarRef(ProgVarRef& pvr, bool quiet, bool& rval) {
  if(!pvr) return false;
  Program* myprg = GET_MY_OWNER(Program);
  Program* otprg = GET_OWNER(pvr.ptr(), Program);
  if(CheckError(myprg != otprg, quiet, rval,
                "program variable:",pvr.ptr()->GetName(),"not in same program as me -- must be fixed!")) {
    return true;
  }
  return false;
}

bool ProgEl::CheckConfig_impl(bool quiet) {
  UpdateProgFlags();
  if(HasProgFlag(OFF)) {
    ClearCheckConfig();
    return true;
  }
  return inherited::CheckConfig_impl(quiet);
}

void ProgEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  UpdateProgFlags();
  inherited::CheckThisConfig_impl(quiet, rval);
  // automatically perform all necessary housekeeping functions!
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFromName("ProgVarRef")) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      CheckProgVarRef(*pvr, quiet, rval);
    }
  }
}

void ProgEl::SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
                              int sls, void* op1_, void* op2_) {
  if(sls != SLS_ITEM_UPDATED || !obj || !obj->InheritsFrom(&TA_ProgVar)) {
    return;
  }
  ProgVar* pv = (ProgVar*)obj;
  pre_compile_code_string = BrowserEditString();  // get any change - e.g. the var was renamed
  if (!pv->schemaChanged()) {
    // taMisc::DebugInfo("updating progel:", GetDisplayName(), "b/c of ProgVar:", pv->name,
    //                   "schema NOT changed!, sig:", String(sls));
    return;
  }
  // taMisc::DebugInfo("updating progel:", GetDisplayName(), "b/c of ProgVar:", pv->name,
  //                   "sig:", String(sls));
  SigEmitUpdated();             // update our display
}

void ProgEl::GenCss(Program* prog) {
  if(HasProgFlag(OFF)) return;
  if(useDesc()) {
    prog->AddDescString(this, desc);
  }
  GenCssPre_impl(prog);
  GenCssBody_impl(prog);
  GenCssPost_impl(prog);
}

const String ProgEl::GenListing(int indent_level) {
  String rval = Program::GetDescString(desc, indent_level);
  rval += cssMisc::Indent(indent_level) + GetDisplayName() + "\n";
  rval += GenListing_children(indent_level);
  return rval;
}

int ProgEl::GetEnabled() const {
  if(HasProgFlag(OFF)) return 0;
  ProgEl* par = parent();
  if (!par) return 1;
  if (par->GetEnabled())
    return 1;
  else return 0;
}

void ProgEl::SetEnabled(bool value) {
  SetProgFlagState(OFF, !value);
}

String ProgEl::GetStateDecoKey() const {
  String rval = inherited::GetStateDecoKey();
  if(rval.empty()) {
    if(HasProgFlag(PROG_ERROR))
      return "ProgElError";
    if(HasProgFlag(WARNING))
      return "ProgElWarning";
    if(HasProgFlag(BREAKPOINT_ENABLED))
      return "ProgElBreakpoint";
    if(HasProgFlag(BREAKPOINT_DISABLED))
      return "ProgElBreakpointDisabled";
    if(HasProgFlag(NON_STD))
      return "ProgElNonStd";
    if(HasProgFlag(NEW_EL))
      return "ProgElNewEl";
    if(HasProgFlag(VERBOSE))
      return "ProgElVerbose";
  }
  return rval;
}

bool ProgEl::EditProgramEl() {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;
  return prg->EditProgramEl(this);
}

bool ProgEl::ViewScript() {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;
  return prg->ViewScriptEl(this);
}

bool ProgEl::ScriptLines(int& start_ln, int& end_ln) {
  start_ln = -1; end_ln = -1;
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;
  return prg->ScriptLinesEl(this, start_ln, end_ln);
}


void ProgEl::SetOffFlag(bool off) {
  SetProgFlagState(OFF, off);
  SigEmitUpdated();
}

void ProgEl::ToggleOffFlag() {
  ToggleProgFlag(OFF);
  SigEmitUpdated();
}

void ProgEl::SetNonStdFlag(bool non_std) {
  SetProgFlagState(NON_STD, non_std);
  SigEmitUpdated();
}

void ProgEl::ToggleNonStdFlag() {
  ToggleProgFlag(NON_STD);
  SigEmitUpdated();
}

void ProgEl::SetNewElFlag(bool new_el) {
  SetProgFlagState(NEW_EL, new_el);
  SigEmitUpdated();
}

void ProgEl::ToggleNewElFlag() {
  ToggleProgFlag(NEW_EL);
  SigEmitUpdated();
}

void ProgEl::SetVerboseFlag(bool new_el) {
  SetProgFlagState(VERBOSE, new_el);
  SigEmitUpdated();
}

void ProgEl::ToggleVerboseFlag() {
  ToggleProgFlag(VERBOSE);
  SigEmitUpdated();
}

void ProgEl::ToggleBreakpoint() {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return;
  prg->ToggleBreakpoint(this);
}

void ProgEl::ToggleBreakEnable() {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return;
  prg->ToggleBreakEnable(this);
}

void ProgEl::EnableBreakpoint() {
  this->SetProgFlag(BREAKPOINT_ENABLED);
  this->ClearProgFlag(BREAKPOINT_DISABLED); // clear, it might be enabled
  SigEmitUpdated();
}

void ProgEl::DisableBreakpoint() {
  this->ClearProgFlag(BREAKPOINT_ENABLED);
  this->SetProgFlag(BREAKPOINT_DISABLED);
  SigEmitUpdated();
}

void ProgEl::SetBreakpoint() {
  this->SetProgFlag(BREAKPOINT_ENABLED);
  SigEmitUpdated();
}

void ProgEl::ClearBreakpoint() {
  this->ClearProgFlag(BREAKPOINT_ENABLED); // clear both, either could be enabled
  this->ClearProgFlag(BREAKPOINT_DISABLED);
  SigEmitUpdated();
}

void ProgEl::PreGen(int& item_id) {
  if(HasProgFlag(OFF)) return;
  PreGenMe_impl(item_id);
  ++item_id;
  PreGenChildren_impl(item_id);
}

ProgVar* ProgEl::FindVarName(const String& var_nm) const {
  return NULL;
}

LocalVars* ProgEl::FindLocalVarList() const {
  ProgEl_List* pelst = GET_MY_OWNER(ProgEl_List);
  if(!pelst) return NULL;
  int myidx = -1;
  for(int i=pelst->size-1; i>= 0; i--) {
    ProgEl* pe = pelst->FastEl(i);
    if(myidx >= 0) {
      if(pe->InheritsFrom(&TA_LocalVars)) {
        return (LocalVars*)pe;
      }
    }
    else {
      if(pe == this) {
        myidx = i;
      }
    }
  }
  ProgEl* ownpe = GET_OWNER(pelst, ProgEl);
  if(ownpe)
    return ownpe->FindLocalVarList();
  return NULL;
}

ProgVar* ProgEl::MakeLocalVar(const String& var_nm) {
  LocalVars* locvars = FindLocalVarList();
  if(!locvars) {
    ProgEl_List* pelst = GET_MY_OWNER(ProgEl_List);
    if(pelst) {
      locvars = new LocalVars;
      pelst->Insert(locvars, 0);
    }
  }
  if(locvars) {
    ProgVar* nwvar = (ProgVar*)locvars->local_vars.New(1, NULL, var_nm);
    locvars->SigEmitUpdated();
    return nwvar;
  }
  return NULL;
}

ProgVar* ProgEl::FindVarNameInScope(String& var_nm, bool else_make) {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg)
    return NULL;
  
  ProgVar* rval = FindVarNameInScope_impl(var_nm);
  if (rval)
    return rval;
  
  if (!rval && this->GetTypeDef()->DerivesFromName("PrintExpr")) {
    return NULL;
  }
  
  // in cases like AssignTo we don't pop the choice dialog
  if(prg) {
    taProject* proj = prg->GetMyProj();
    if(proj && proj->no_dialogs) {
      return NULL;
    }
  }

  if(!rval && else_make) {
    if(taMisc::is_loading) return rval;
    
    ProgElChoiceDlg dlg;
    taBase::Ref(dlg);
    int choice = 2;
    ProgVar::VarType var_type = ProgVar::T_UnDef;
    int result = dlg.GetLocalGlobalChoice(var_nm, choice, var_type);
    if (result == 1) {
      if(choice == 0) {
        rval = ((ProgEl*)this)->MakeLocalVar(var_nm);
        // if(taMisc::gui_active)
        //   tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
      }
      else if(choice == 1) {
        rval = (ProgVar*)prg->vars.New(1, NULL, var_nm);
        // if(taMisc::gui_active)
        //   tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
      }
      if(rval) {
        rval->var_type = var_type;
        rval->UpdateAfterEdit();
      }
    }
  }
  return rval;
}

ProgVar* ProgEl::FindVarNameInScope_impl(const String& var_nm) const {
  if(InheritsFrom(&TA_Function)) { // we bubbled up to function object
    ProgVar* rval = FindVarName(var_nm);
    if(rval) return rval;
  }
  LocalVars* loc = FindLocalVarList();
  if(loc) {
    ProgVar* rval = loc->FindVarName(var_nm);
    if(rval) return rval;
    ProgEl* loc_own = GET_OWNER(loc, ProgEl);
    if(loc_own)
      return loc_own->FindVarNameInScope_impl(var_nm);
  }
  else {
    Function* myfun = GET_MY_OWNER(Function);
    if(myfun) {
      ProgVar* rval = myfun->FindVarName(var_nm);
      if(rval) return rval;
    }
  }
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return NULL;
  return prog->FindVarName(var_nm); // go all they way back down..
}

bool ProgEl::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool ProgEl::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool ProgEl::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}

String ProgEl::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_disp_name) {
    String rval = GetDisplayName();
    if(desc.nonempty())
      rval +=  " // " + desc;
    return rval;
  }
  return inherited::GetColText(key, itm_idx);
}

const String ProgEl::GetToolTip(const KeyString& key) const {
  String rval = inherited::GetColText(key); // get full col text from tabase
  // include type names for further reference
  return String("(") + GetToolbarName() + " " + GetTypeName() + ") : " + rval;
}


bool ProgEl::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if (CvtFmCodeCheckNames(code))
    return true;
  
  return false;
}

bool ProgEl::CvtFmCodeCheckNames(const String& code) const {
  String dc = code; dc.downcase();
  String tool_bar_name = GetToolbarName(); tool_bar_name.downcase();
  String type_name = GetTypeDef()->name; type_name.downcase();

  String first_word = taMisc::ParseStr_CName(dc);
  if(first_word == tool_bar_name || first_word == type_name)
    return true;

  if(tool_bar_name.contains(' ') && dc.startsWith(tool_bar_name)) {
    return true;
  }
  
  return false;
}

bool ProgEl::CvtFmCode(const String& code) {
  // nothing to initialize
  return true;
}

bool ProgEl::CvtFmSavedCode() {
  bool rval = CvtFmCode(code_string);
  SigEmitUpdated();
  return rval;
}

bool ProgEl::BrowserEditSet(const String& code, int move_after) {
  if(move_after != -11) {
    Program* prog = GET_MY_OWNER(Program);
    if(prog) {
      taProject* proj = prog->GetMyProj();
      if(proj) {
        proj->undo_mgr.SaveUndo(this, "BrowserEditSet", prog);
      }
    }
  }
  edit_move_after = 0;
  String cd = CodeGetDesc(code);
  if(CanCvtFmCode(cd, this)) {
    bool rval;
    rval = CvtCodeToVar(cd);
    rval = CvtFmCode(cd);
    UpdateAfterEdit();
    return rval;
  }
  code_string = cd;
  edit_move_after = move_after;
  TestWarning(move_after == -11, "BrowserEditSet",
              "Reverting Code -- it failed to pass the CanCvtFmCode step!\n",
              code);
  tabMisc::DelayedFunCall_gui(this, "RevertToCode"); // do it later..
  return false;
}

bool ProgEl::BrowserEditTest() {
  return BrowserEditTest_impl();
}

bool ProgEl::BrowserEditTest_impl() {
  String pre_str = BrowserEditString();
  bool setok = BrowserEditSet(pre_str, -11); // -11 is special code..
  if(TestWarning(!setok, "BrowserEditTest", "set failed to parse for\nexpr:",
                 pre_str)) {
    return false;
  }
  String post_str = BrowserEditString();
  if(TestWarning(pre_str != post_str, "BrowserEditTest", "MISMATCH!\npre_str:",
                 pre_str, "\npst_str:", post_str)) {
    return false;
  }
  return true;
}

String ProgEl::CodeGetDesc(const String& code) {
  if(code.contains("//")) {
    desc = trim(code.after("//"));
    return trim(code.before("//"));
  }
  if(code.contains("/*")) {
    desc = trim(code.after("/*"));
    if(desc.contains("*/"))
      desc = trim(desc.before("*/",-1));
    return trim(code.before("/*"));
  }
  return code;
}

bool ProgEl::RevertToCode() {
  UpdateProgFlags();		// make sure
  if(!HasProgFlag(CAN_REVERT_TO_CODE)) {
    SigEmitUpdated(); // trigger update of our gui -- obviously out of whack
    return false;
  }
  ProgEl_List* own = GET_MY_OWNER(ProgEl_List);
  if(!own) return false;
  ProgCode* cvt = new ProgCode;
  cvt->desc = desc;
  cvt->code.expr = code_string;
  if (HasSubCode()) {
    cvt->sub_code.Copy(*children_());  // save the subcode in the ProgCode object for later replacement in new ProgEl
  }
  int myidx = own->FindEl(this);
  SetBaseFlag(BF_MISC4); // indicates that we're done..
  own->ReplaceLater(cvt, myidx, "");
  return true;
}

String ProgEl::StringFieldLookupFun(const String& cur_txt, int cur_pos,
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

bool ProgEl::CvtCodeToVar(String& code) {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;
  
  if(!code.contains(' ')) return false;

  String ckcode = code;
  if(ckcode.contains('\"')) {   // exclude quoted expressions
    ckcode = ckcode.before('\"'); // we don't care about after for types -- can't decl type after..
  }

  // the parser likes a space after a left parens :)
  ckcode.gsub('(', "( ");

  String vtype;
  TypeDef* td;
  String_Array tokens;
  tokens.Split(ckcode, " ");
  for (int i=0; i<tokens.size; i++) {
    vtype = tokens[i].chars();
    bool ref;
    td = ProgVar::GetTypeDefFromString(vtype, ref);
    if (td)
      break;
  }
  if(!td)
    return false;

  // if you're going to allow variable type anywhere, you can't cut out everything before!!
  ckcode = trim(code.after(vtype)); // use this for looking for variable name

  if(ckcode.empty())
    return false;

  String var_nm;
  int pos = 0;
  char c = ckcode[pos];
  while(isalnum(c) || c == '_') {
    var_nm += c;
    if(ckcode.length() > pos)
      c = ckcode[++pos];
    else
      break;
  }
  
  if (var_nm.empty()) // no var_name
    return true;      // return true??

  // bad var name - replace with good one
  if (!taMisc::IsLegalCVar(var_nm)){
    String good_var = taMisc::StringCVar(var_nm);
    ckcode = ckcode.repl(var_nm, good_var);
    var_nm = good_var;
  }
  
  bool exists = prg->FindVarName(var_nm);
  if (exists) {
    code = code.before(vtype) + ckcode; // this is the code minus the variable type
    return true;
  }

  // ONLY modify the code if we're actually going to prompt!
  code = code.before(vtype) + ckcode; // this is the code minus the variable type
  
  ProgVar::VarType var_type = ProgVar::GetTypeFromTypeDef(td);
  ProgElChoiceDlg dlg;
  taBase::Ref(dlg);
  int choice = 2;
  int result = 0;
  if(var_type == ProgVar::T_HardEnum) {
    result = 1;
    choice = 1;                 // can only be in globals
  }
  else {
    result = dlg.GetLocalGlobalChoice(var_nm, choice, var_type,
                                      ProgElChoiceDlg::LOCALGLOBAL, true);
  }
  // true = "make new.." instructions
  ProgVar* rval = NULL;
  if (result == 1) {
    if(choice == 0) {
      rval = MakeLocalVar(var_nm);
      if(taMisc::gui_active)
        tabMisc::DelayedFunCall_gui(rval, "BrowserExpandAll");
    }
    else if(choice == 1) {
      rval = (ProgVar*)prg->vars.New(1, NULL, var_nm);
      prg->vars.SigEmitUpdated();
      // if(taMisc::gui_active)
      //   tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
    }
    if(rval) {
      rval->SetTypeFromTypeDef(td);
      rval->UpdateAfterEdit();
    }
  }
  return true;
}

bool ProgEl::InDebugMode() {
  Program* prg = GET_MY_OWNER(Program);
  if (prg) {
    Program_Group* prg_grp = GET_MY_OWNER(Program_Group);
    if (prg_grp) {
      return prg_grp->InDebugMode();
    }
  }
  return false;  // shouldn't get here
}
