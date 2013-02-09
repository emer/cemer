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

ProgEl* ProgCode::CvtCodeToProgEl(const String& code_str, ProgEl* scope_el) {
  ProgEl_List candidates;
  if(code_str.endsWith(';')) {
    // if we use a ; at the end, it is a guarantee of doing the css expr so
    // we can avoid alternative matches etc.
    candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_CssExpr));
  }
  else {
    CvtCodeCheckType(candidates, &TA_ProgEl, code_str, scope_el);
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
                                 + code_str + "\nPlease choose correct one.",
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

void ProgCode::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
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
  cvt->desc = desc;         // transfer description
  cvt->orig_prog_code = code_str;
  cvt->SetProgFlag(CAN_REVERT_TO_CODE);
  tabMisc::DelayedFunCall_gui(own, "BrowserSelectMe"); // do this first so later one registers as diff..
  int myidx = own->FindEl(this);
  own->Insert(cvt, myidx); // insert at my position
  bool ok = cvt->CvtFmCode(code_str); // convert once in position -- needs access to scope..
  if(ok) {
    SetBaseFlag(BF_MISC4); // indicates that we're done..
    this->CloseLater();      // kill me later..
    tabMisc::DelayedFunCall_gui(cvt, "BrowserExpandAll");
    tabMisc::DelayedFunCall_gui(cvt, "BrowserSelectMe");
  }
  else {
    own->RemoveIdx(myidx); // remove it!
    tabMisc::DelayedFunCall_gui(this, "BrowserSelectMe");
  }
}

String ProgCode::GetDisplayName() const {
  return code.expr;
}

