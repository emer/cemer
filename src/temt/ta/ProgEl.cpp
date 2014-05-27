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
  if (!var->HasVarFlag(ProgVar::LOCAL_VAR))
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
  desc = cp.desc;
  flags = cp.flags;
  orig_prog_code = cp.orig_prog_code;
}

void ProgEl::UpdateProgFlags() {
  if(orig_prog_code.nonempty() && ProgElChildrenCount() == 0) {
    SetProgFlag(CAN_REVERT_TO_CODE);
  }
  else {
    ClearProgFlag(CAN_REVERT_TO_CODE);
  }
}

void ProgEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateProgFlags();
}

void ProgEl::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);

  if(!old_owner) return;
  Program* myprg = GET_MY_OWNER(Program);
  Program* otprg = (Program*)old_owner->GetOwner(&TA_Program);
  if(!myprg || !otprg || myprg == otprg) return;
  // don't update if not relevant

  // todo: this can now theoretically be done by UpdatePointers_NewPar_FindNew
  // but this was written first and it works..
  // automatically perform all necessary housekeeping functions!
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_ProgExprBase)) {
      ProgExprBase* peb = (ProgExprBase*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgArg_List)) {
      ProgArg_List* peb = (ProgArg_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgExpr_List)) {
      ProgExpr_List* peb = (ProgExpr_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFromName("ProgVarRef")) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      UpdateProgVarRef_NewOwner(*pvr);
    }
    else if(md->type->InheritsFromName("ProgramRef")) {
      ProgramRef* pvr = (ProgramRef*)md->GetOff((void*)this);
      if(pvr->ptr()) {
        Program_Group* mygp = GET_MY_OWNER(Program_Group);
        Program_Group* otgp = GET_OWNER(old_owner, Program_Group);
        Program_Group* pvgp = GET_OWNER(pvr->ptr(), Program_Group);
        if(mygp != otgp && (pvgp == otgp)) { // points to old group and we're in a new one
          Program* npg = mygp->FindName(pvr->ptr()->name); // try to find new guy in my group
          if(npg) pvr->set(npg);                    // set it!
        }
      }
    }
  }

  UpdatePointers_NewPar(otprg, myprg); // do the generic function to catch anything else..
  taProject* myproj = GET_OWNER(myprg, taProject);
  taProject* otproj = GET_OWNER(otprg, taProject);
  if(myproj && otproj && (myproj != otproj))
    UpdatePointers_NewPar(otproj, myproj);
  // then do it again if moving between projects
}

void ProgEl::UpdateAfterCopy(const ProgEl& cp) {
  Program* myprg = GET_MY_OWNER(Program);
  Program* otprg = (Program*)cp.GetOwner(&TA_Program);
  if(!myprg || !otprg || myprg == otprg || myprg->HasBaseFlag(taBase::COPYING)) return;
  // don't update if already being taken care of at higher level

  // todo: this can now theoretically be done by UpdatePointers_NewPar_FindNew
  // but this was written first and it works..
  // automatically perform all necessary housekeeping functions!
  TypeDef* td = GetTypeDef();
  for(int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_ProgExprBase)) {
      ProgExprBase* peb = (ProgExprBase*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgArg_List)) {
      ProgArg_List* peb = (ProgArg_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFrom(&TA_ProgExpr_List)) {
      ProgExpr_List* peb = (ProgExpr_List*)md->GetOff((void*)this);
      peb->UpdateProgExpr_NewOwner();
    }
    else if(md->type->InheritsFromName("ProgVarRef")) {
      ProgVarRef* pvr = (ProgVarRef*)md->GetOff((void*)this);
      UpdateProgVarRef_NewOwner(*pvr);
    }
    else if(md->type->InheritsFromName("ProgramRef")) {
      ProgramRef* pvr = (ProgramRef*)md->GetOff((void*)this);
      if(pvr->ptr()) {
        Program_Group* mygp = GET_MY_OWNER(Program_Group);
        Program_Group* otgp = GET_OWNER(otprg, Program_Group);
        Program_Group* pvgp = GET_OWNER(pvr->ptr(), Program_Group);
        if(mygp != otgp && (pvgp == otgp)) { // points to old group and we're in a new one
          Program* npg = mygp->FindName(pvr->ptr()->name); // try to find new guy in my group
          if(npg) pvr->set(npg);                    // set it!
        }
      }
    }
  }

  UpdatePointers_NewPar(otprg, myprg); // do the generic function to catch anything else..
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_taProject);
  // then do it again if moving between projects
}

void ProgEl::CheckError_msg(const char* a, const char* b, const char* c,
                            const char* d, const char* e, const char* f,
                            const char* g, const char* h) const {
  String prognm;
  Program* prg = GET_MY_OWNER(Program);
  if(prg) prognm = prg->name;
  String objinfo = "Config Error in Program " + prognm + ": " + GetTypeDef()->name
    + " " + GetDisplayName() + "\npath: " + GetPathNames() + "\n";
  taMisc::CheckError(objinfo, a, b, c, d, e, f, g, h);
}

bool ProgEl::CheckEqualsError(String& condition, bool quiet, bool& rval) {
  if(CheckError((condition.freq('=') == 1) && !(condition.contains(">=")
                                                || condition.contains("<=")
                                                || condition.contains("!=")),
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

bool ProgEl::UpdateProgVarRef_NewOwner(ProgVarRef& pvr) {
  ProgVar* cur_ptr = pvr.ptr();
  if(!cur_ptr) return false;
  String var_nm = cur_ptr->name;
  Program* my_prg = GET_MY_OWNER(Program);
  Program* ot_prg = GET_OWNER(cur_ptr, Program);
  if(!my_prg || !ot_prg || my_prg->HasBaseFlag(taBase::COPYING)) return false; // not updated
  Function* ot_fun = GET_OWNER(cur_ptr, Function);
  Function* my_fun = GET_MY_OWNER(Function);
  if(ot_fun && my_fun) {               // both in functions
    if(my_fun == ot_fun) return false; // nothing to do
    String cur_path = cur_ptr->GetPath(NULL, ot_fun);
    MemberDef* md;
    ProgVar* pv = (ProgVar*)my_fun->FindFromPath(cur_path, md);
    if(pv && (pv->name == var_nm)) { pvr.set(pv); return true; }
    // ok, this is where we find same name or make one
    String cur_own_path = cur_ptr->owner->GetPath(NULL, ot_fun);
    taBase* pv_own_tab = my_fun->FindFromPath(cur_own_path, md);
    if(!pv_own_tab || !pv_own_tab->InheritsFrom(&TA_ProgVar_List)) {
      LocalVars* pvars = (LocalVars*)my_fun->fun_code.FindType(&TA_LocalVars);
      if(!pvars) {
        taMisc::Warning("Warning: could not find owner for program variable:",
                        var_nm, "in program:", my_prg->name, "on path:",
                        cur_own_path, "setting var to null!");
        pvr.set(NULL);
        return false;
      }
      pv_own_tab = &(pvars->local_vars);
    }
    ProgVar_List* pv_own = (ProgVar_List*)pv_own_tab;
    pv = pv_own->FindName(var_nm);
    if(pv) { pvr.set(pv); return true; }        // got it!
    pv = my_fun->FindVarName(var_nm); // do more global search
    if(pv) { pvr.set(pv); return true; }        // got it!
    // now we need to add a clone of cur_ptr to our local list and use that!!
    pv = (ProgVar*)cur_ptr->Clone();
    pv_own->Add(pv);
    pv->CopyFrom(cur_ptr);      // somehow clone is not copying stuff -- try this
    pv->name = var_nm;          // just to be sure
    pv->SigEmitUpdated();
    pvr.set(pv); // done!!
    taMisc::Info("Note: copied program variable:",
                 var_nm, "from function:", ot_fun->name, "to function:",
                 my_fun->name, "because copied program element refers to it");
    taProject* myproj = GET_OWNER(my_prg, taProject);
    taProject* otproj = GET_OWNER(ot_prg, taProject);
    // update possible var pointers from other project!
    if(myproj && otproj && (myproj != otproj)) {
      pv->UpdatePointers_NewPar(otproj, myproj);
    }
  }
  else {
    if(my_prg == ot_prg) return false;        // same program, no problem
    String cur_path = cur_ptr->GetPath(NULL, ot_prg);
    MemberDef* md;
    ProgVar* pv = (ProgVar*)my_prg->FindFromPath(cur_path, md);
    if(pv && (pv->name == var_nm)) { pvr.set(pv); return true; }
    // ok, this is where we find same name or make one
    String cur_own_path = cur_ptr->owner->GetPath(NULL, ot_prg);
    taBase* pv_own_tab = my_prg->FindFromPath(cur_own_path, md);
    if(!pv_own_tab || !pv_own_tab->InheritsFrom(&TA_ProgVar_List)) {
      taMisc::Warning("Warning: could not find owner for program variable:",
                      var_nm, "in program:", my_prg->name, "on path:",
                      cur_own_path, "setting var to null!");
      pvr.set(NULL);
      return false;
    }
    ProgVar_List* pv_own = (ProgVar_List*)pv_own_tab;
    pv = pv_own->FindName(var_nm);
    if(pv) { pvr.set(pv); return true; }        // got it!
    pv = my_prg->FindVarName(var_nm); // do more global search
    if(pv) { pvr.set(pv); return true; }        // got it!
    // now we need to add a clone of cur_ptr to our local list and use that!!
    if(cur_ptr->objs_ptr && (bool)cur_ptr->object_val) {
      // copy the obj -- if copying var, by defn need to copy obj -- auto makes corresp var!
      taBase* varobj = cur_ptr->object_val.ptr();
      taBase* nwobj = varobj->Clone();
      nwobj->CopyFrom(varobj);  // should not be nec..
      nwobj->SetName(varobj->GetName()); // copy name in this case
      my_prg->objs.Add(nwobj);
      taMisc::Info("Note: copied program object:",
                   varobj->GetName(), "from program:", ot_prg->name, "to program:",
                   my_prg->name, "because copied program element refers to it");
      pv = my_prg->FindVarName(var_nm); // get new var that was just created!
      if(pv) { pvr.set(pv); return true; }      // got it!
    }
    pv = (ProgVar*)cur_ptr->Clone();
    pv_own->Add(pv);
    pv->CopyFrom(cur_ptr);      // somehow clone is not copying stuff -- try this
    pv->name = var_nm;          // just to be sure
    pvr.set(pv); // done!!
    pv->SigEmitUpdated();
    taMisc::Info("Note: copied program variable:",
                 var_nm, "from program:", ot_prg->name, "to program:",
                 my_prg->name, "because copied program element refers to it");
    taProject* myproj = GET_OWNER(my_prg, taProject);
    taProject* otproj = GET_OWNER(ot_prg, taProject);
    // update possible var pointers from other project!
    if(myproj && otproj && (myproj != otproj)) {
      pv->UpdatePointers_NewPar(otproj, myproj);
    }
  }
  return true;
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
  if(!prg) return NULL;

  ProgVar* rval = FindVarNameInScope_impl(var_nm);
  
  if (!rval && this->GetTypeDef()->DerivesFromName("PrintExpr")) {
    return NULL;
  }
  if(!rval && else_make) {
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

String ProgEl::GetToolbarName() const {
  return "<base el>";
}

bool ProgEl::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  return false;
}

bool ProgEl::CvtFmCode(const String& code) {
  // nothing to initialize
  return true;
}

bool ProgEl::CvtFmSavedCode() {
  bool rval = CvtFmCode(orig_prog_code);
  SigEmitUpdated();
  return rval;
}

bool ProgEl::BrowserEditSet(const String& code, int move_after) {
  edit_move_after = 0;
  String cd = CodeGetDesc(code);
  if(CanCvtFmCode(cd, NULL)) {
    bool rval = CvtFmCode(cd);
    UpdateAfterEdit();
    return rval;
  }
  orig_prog_code = cd;
  edit_move_after = move_after;
  tabMisc::DelayedFunCall_gui(this, "RevertToCode"); // do it later..
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
  taProject* proj = GET_OWNER(own, taProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(own, "RevertToCode", own, false, own);
  }
  ProgCode* cvt = new ProgCode;
  cvt->desc = desc;
  cvt->code.expr = orig_prog_code;
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

