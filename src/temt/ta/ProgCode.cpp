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
taTypeDef_Of(Comment);

void ProgCode::Initialize() {
  SetProgExprFlags();
}

void ProgCode::SetProgExprFlags() {
  code.SetExprFlag(ProgExpr::NO_PARSE); // do not parse at all -- often nonsense..
  ClearProgFlag(CAN_REVERT_TO_CODE); // we are code!
}

void ProgCode::CvtCodeCheckType(ProgEl_List& candidates, TypeDef* td,
				const String& code_str) {
  ProgEl* obj = (ProgEl*)tabMisc::root->GetTemplateInstance(td);
  if(obj) {
    if(obj->CanCvtFmCode(code_str, this)) {
      candidates.Link(obj);
    }
  }
  for(int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    CvtCodeCheckType(candidates, chld, code_str);
  }
}

bool ProgCode::CvtCodeToVar(String& code) {
  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return false;

  if(!code.contains(" ")) return false;
  String vtype = code.before(' ');
  TypeDef* td = ProgVar::GetTypeDefFromString(vtype);
  if(!td) return false;
  code = trim(code.after(' '));
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

ProgEl* ProgCode::CvtCodeToProgEl() {
  ProgEl_List candidates;
  String code_mod = code.expr;
  if(code_mod.endsWith(';')) {
    // if we use a ; at the end, it is a guarantee of doing the css expr so
    // we can avoid alternative matches etc.
    candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_CssExpr));
  }
  else if(code_mod.startsWith("//") || code_mod.startsWith("/*")) {
    candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_Comment));
  }
  else {
    bool had_var = CvtCodeToVar(code_mod);
    if(had_var) {
      code.expr = code_mod;     // code was truncated..
      SigEmitUpdated();          // update us
      if(code_mod.empty())
        return NULL;              // that's it, we're done!  avail for something else..
    }
    CvtCodeCheckType(candidates, &TA_ProgEl, code_mod);
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
  if(code_str.startsWith("//") || code_str.startsWith("/*")) {
    code.expr = code_str;
  }
  else {
    code.expr = CodeGetDesc(code_str);
  }
  if(code.expr.nonempty()) {
    tabMisc::DelayedFunCall_gui(this, "ConvertToProgEl"); // do it later..
  }
  return false;
}

void ProgCode::ConvertToProgEl() {
  if(HasBaseFlag(BF_MISC4)) return; // already did the conversion -- going to be nuked!
  ProgEl_List* own = GET_MY_OWNER(ProgEl_List);
  if(!own) return;
  code.expr = trim(code.expr);
  if(code.expr.empty()) return;
  ProgEl* cvt = CvtCodeToProgEl();
  if(!cvt) return;
  taProject* proj = GET_OWNER(own, taProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(own, "ProgCodeCvt", own, false, own); 
  }
  cvt->edit_move_after = edit_move_after; // these are the ones who need it
  edit_move_after = 0;
  cvt->desc = desc;         // transfer description
  cvt->orig_prog_code = code.expr;
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

