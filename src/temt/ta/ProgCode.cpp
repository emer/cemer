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

#include "ProgCode.h"
#include <Program>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taProject>
#include <ProgElChoiceDlg>

TA_BASEFUNS_CTORS_DEFN(ProgCode);

taTypeDef_Of(CssExpr);
taTypeDef_Of(AssignExpr);

void ProgCode::Initialize() {
  SetProgExprFlags();
}

void ProgCode::SetProgExprFlags() {
  code.SetExprFlag(ProgExpr::NO_PARSE); // do not parse at all -- often nonsense..
  ClearProgFlag(CAN_REVERT_TO_CODE); // we are code!
}

void ProgCode::CvtCodeCheckType(ProgEl_List& candidates, TypeDef* td,
				const String& code_str, ProgEl* scope_el) {
  ProgEl* obj = (ProgEl*)tabMisc::root->GetTemplateInstance(td);
  if(obj) {
    if(obj->CanCvtFmCode(code_str, scope_el)) {
      candidates.Link(obj);
    }
  }
  for(int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    CvtCodeCheckType(candidates, chld, code_str, scope_el);
  }
}

bool ProgCode::CvtCodeToVar(String& code, ProgEl* scope_el) {
  Program* prg = GET_OWNER(scope_el, Program);
  if(!prg) return false;

  if(!code.contains(" ")) return false;
  String vtype = code.before(" ");
  TypeDef* td = taMisc::FindTypeName(vtype);
  if(!td) return false;
  code = trim(code.after(" "));
  String var_nm;
  int pos = 0;
  char c = code[pos];
  while(isalnum(c) || c == '_') {
    var_nm += c;
    if(code.length() > pos)
      c = code[++pos];
    else 
      break;
  }
  code = code.from(pos);
  ProgElChoiceDlg dlg;
  taBase::Ref(dlg);
  int choice = 2;
  ProgVar::VarType var_type = ProgVar::GetTypeFromTypeDef(td);
  int result = dlg.GetLocalGlobalChoice(var_nm, choice, var_type);
  ProgVar* rval = NULL;
  if (result == 1) {
    if(choice == 0) {
      rval = scope_el->MakeLocalVar(var_nm);
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
      // todo: set object type for object, etc..
      rval->UpdateAfterEdit();
    }
  }
  return true;
}

ProgEl* ProgCode::CvtCodeToProgEl(const String& code_str, ProgEl* scope_el) {
  ProgEl_List candidates;
  String code_mod = code_str;
  if(code_str.endsWith(';')) {
    // if we use a ; at the end, it is a guarantee of doing the css expr so
    // we can avoid alternative matches etc.
    candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_CssExpr));
  }
  else {
    bool had_var = CvtCodeToVar(code_mod, scope_el);
    if(had_var && code_mod.empty())
      return NULL;              // that's it
    String check_code = code_mod;
    check_code.downcase();        // check only on lowercase
    CvtCodeCheckType(candidates, &TA_ProgEl, check_code, scope_el);
    if(candidates.size == 0)
      return NULL;
  }
  ProgEl* cvt = candidates[0];
  if(candidates.size > 1) {
    int ctrl_n = 0;
    for(int i=candidates.size-1; i>= 0; i--) {
      ProgEl* pel = candidates[i];
      if(pel->InheritsFrom(&TA_AssignExpr)) { // assign only matches if it is the only one..
        candidates.RemoveIdx(i);
	continue;
      }
      if(pel->IsCtrlProgEl()) {
	cvt = pel;		// take precidence
	ctrl_n++;
      }
    }
    if(ctrl_n != 1) {		// still need to pick -- ctrl_n should never be > 1 but who knows
      cvt = candidates[0];
      if(candidates.size > 1) {
	int chs = taMisc::Choice("Multiple program elements match code string:\n"
                                 + code_mod + "\nPlease choose correct one.",
	 "Cancel",
	 candidates[0]->GetToolbarName(),
	 candidates[1]->GetToolbarName(),
	 (candidates.size > 2 ? candidates[2]->GetToolbarName() : _nilString),
	 (candidates.size > 3 ? candidates[3]->GetToolbarName() : _nilString),
	 (candidates.size > 4 ? candidates[4]->GetToolbarName() : _nilString)
	 );
	if(chs == 0) return NULL;
	cvt = candidates[chs-1];
      }
    }
  }

  ProgEl* rval = (ProgEl*)cvt->MakeToken();
  return rval;
}

bool ProgCode::BrowserEditSet(const String& code_str, int move_after) {
  edit_move_after = move_after;
  code.expr = CodeGetDesc(code_str);
  if(code.expr.nonempty()) {
    tabMisc::DelayedFunCall_gui(this, "ConvertToProgEl"); // do it later..
  }
  return false;
}

void ProgCode::ConvertToProgEl() {
  if(HasBaseFlag(BF_MISC4)) return; // already did the conversion -- going to be nuked!
  ProgEl_List* own = GET_MY_OWNER(ProgEl_List);
  if(!own) return;
  String code_str = trim(code.expr);
  if(code_str.empty()) return;
  ProgEl* cvt = CvtCodeToProgEl(code_str, this);
  if(!cvt) return;
  taProject* proj = GET_OWNER(own, taProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(own, "ProgCodeCvt", own, false, own); 
  }
  cvt->edit_move_after = edit_move_after; // these are the ones who need it
  edit_move_after = 0;
  cvt->desc = desc;         // transfer description
  cvt->orig_prog_code = code_str;
  cvt->SetProgFlag(CAN_REVERT_TO_CODE);
  int myidx = own->FindEl(this);
  own->ReplaceLater(cvt, myidx, "CvtFmSavedCode");
  SetBaseFlag(BF_MISC4); // indicates that we're done..
}

void ProgCode::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
  if(HasBaseFlag(BF_MISC4)) return; // already did the conversion -- going to be nuked!
  if(!taMisc::gui_active) return;
  ProgEl_List* own = GET_MY_OWNER(ProgEl_List);
  if(!own) return;
  String code_str = trim(code.expr);
  if(code_str.empty()) return;
  code.expr = CodeGetDesc(code_str);
  tabMisc::DelayedFunCall_gui(this, "ConvertToProgEl"); // do it later..
}  

String ProgCode::GetDisplayName() const {
  return code.expr;
}

