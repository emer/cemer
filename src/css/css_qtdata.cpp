// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#include "ta_qtdialog.h"
#include "css_qtdata.h"
#include "css_qttype.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
//#include "css_ta.h"
//#include "ta_type.h"

//#include <stdlib.h>

#include <qobject.h>
#include <qpushbutton.h>

/* OBS:
#ifndef CYGWIN
#include <IV-X11/xevent.h>
#endif
*/

//////////////////////////////////////////////////
// 		cssiPolyData			//
//////////////////////////////////////////////////

cssiPolyData::cssiPolyData(cssClassInst* ob, TypeDef* typ_, IDataHost* host_, taiData* par,
    QWidget* gui_parent, int flags_)
: taiData(typ_, host_, par, gui_parent, flags_) {
  obj = ob;
}

cssiPolyData::~cssiPolyData() {
  type_el.Reset();
  data_el.Reset();
}

void cssiPolyData::Constr(QWidget* gui_parent_) {
  /* TODO:
  rep = taiM->layout->hbox();
  ivResource::ref(rep);
  int i;
  for(i=0; i<obj->members->size; i++) {
    cssEl* md = obj->members->FastEl(i);
    if((obj->type_def != NULL) && !obj->type_def->MbrHasOption(i, "SHOW") &&
       (obj->type_def->MbrHasOption(i, "HIDDEN") ||
	obj->type_def->MbrHasOption(i, "HIDDEN_INLINE") ||
	obj->type_def->MbrHasOption(i, "READ_ONLY")))
      continue;
    bool read_only = false;
    if((obj->type_def != NULL) && obj->type_def->MbrHasOption(i, "READ_ONLY"))
      read_only = true;
    cssiType* tv = cssiEditDialog::GetTypeFromEl(md, read_only); // get the actual object..
    if((tv == NULL) || (tv->cur_base == NULL))
      continue;
    type_el.Add(tv);
    taiData* mb_dat = tv->GetDataRep(ih, host_, this);
    data_el.Add(mb_dat);
    String nm = md->GetName();
    rep->append(taiM->layout->center(taiM->wkit->label(nm), 0, 0));
    rep->append(taiM->hfsep);
    rep->append(taiM->layout->vcenter(mb_dat->GetLook(),0));
    if(i != (obj->members->size -1))
      rep->append(taiM->hfsep);
  } */
}

void cssiPolyData::GetImage_impl(const void*) {
  for (int i = 0; i <type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    cit->GetImage(mb_dat);
  }
}

void cssiPolyData::GetValue_impl(void*) const {
  for (int i = 0; i < type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    cit->GetValue(mb_dat);
  }
}


//////////////////////////////////////////////////
// 		cssiMethMenu			//
//////////////////////////////////////////////////

cssiMethMenu::cssiMethMenu(cssClassInst* ob, cssProgSpace* tp, cssMbrScriptFun* cfn,
	TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_)
: taiMethMenu(NULL, (MethodDef*)NULL, typ_, host_, par, gui_parent, flags_)
{
  obj = ob;
  top = tp;
  css_fun = cfn;
  arg_obj = NULL;
  if (css_fun->HasOption("BUTTON")) {
    is_menu_item = false;
    QAbstractButton* but = MakeButton(); //note: connects to CallFun
    but->setText(css_fun->name);
  }
  else {
    is_menu_item = true;
  }
}

void cssiMethMenu::AddToMenu(taiMenu* menu) {
  if (css_fun->HasOption("MENU_SEP_BEFORE"))
    menu->AddSep();
  menu->AddItem(css_fun->name, taiMenu::use_default,
	taiAction::action, this, SLOT(CallFun), (void*)NULL);
  if (css_fun->HasOption("MENU_SEP_AFTER"))
    menu->AddSep();
}

void cssiMethMenu::CallFun() {
  /* TODO:
  ApplyBefore();
  use_argc = css_fun->argc;
  String argc_str = css_fun->OptionAfter("ARGC_");
  if (argc_str != "")
    use_argc = (int)argc_str + 1;

  // get appropriate context to run function in
  cssProg* prg = cssMisc::CallFunProg;
  cssProgSpace* old_top = NULL;
  if (top != NULL)
    old_top = cssMisc::SetCurTop(top);
  else
    old_top = cssMisc::SetCurTop(cssMisc::Top);
  cssProgSpace* old_prg_top = prg->SetTop(cssMisc::cur_top);

  if ((use_argc <= 1) && !css_fun->HasOption("CONFIRM")) {
    prg->Stack()->Push(&cssMisc::Void);	// argstop
    prg->Stack()->Push(obj);
    cssEl::Ref(obj);		// make sure it doesn't get to 0 refcount in arg del..
    css_fun->Do(prg);
    cssEl::unRef(obj);		// undo that
    cssEl* rval = prg->Stack()->Pop();
    cssEl::Ref(rval);
    rval->prog = prg;
    GenerateScript();
    UpdateAfter();
    if(rval != &cssMisc::Void)
      ShowReturnVal(rval);
    cssEl::unRefDone(rval);
    prg->PopTop(old_prg_top);
    cssMisc::PopCurTop(old_top);
    return;
  }
  arg_obj = new cssClassInst("args for: ");
  arg_obj->name = css_fun->name + ": " + css_fun->desc;
  int i;
  for(i=2; i<=use_argc; i++) {
    cssEl* av = css_fun->argv[i].El()->Clone();
    if(av->GetType() == cssEl::T_Class) {
      ((cssClassInst*)av)->ConstructToken();
      arg_obj->members->Push(av); // currently not saving to string..
    }
    else {
      arg_obj->members->Push(av);
      if(!css_fun->arg_vals[i].empty())	// set to saved values
	*av = css_fun->arg_vals[i];
    }
  }

  cssiEditDialog* carg_dlg = new cssiEditDialog(arg_obj);
  carg_dlg->Constr(NULL, true);
  int ok_can = carg_dlg->Edit();	// true = wait for a response
  if(ok_can) {
    prg->Stack()->Push(&cssMisc::Void);	// argstop
    prg->Stack()->Push(obj);	// always the first arg
    int i;
    for(i=0; i<arg_obj->members->size; i++) {
      cssEl* av = arg_obj->members->FastEl(i);
      prg->Stack()->Push(av);
      css_fun->arg_vals[2 + i] = av->GetStr(); // set cur val based on arg val
      if(av->GetType() == cssEl::T_Class)
	((cssClassInst*)av)->DestructToken();
    }
    cssEl::Ref(obj);		// make sure it doesn't get to 0 refcount in arg del..
    css_fun->Do(prg);
    cssEl::unRef(obj);		// undo that
    cssEl* rval = prg->Stack()->Pop();
    cssEl::Ref(rval);
    rval->prog = prg;
    GenerateScript();
    UpdateAfter();
    if(rval != &cssMisc::Void)
      ShowReturnVal(rval);
    cssEl::unRefDone(rval);
  }
  delete carg_dlg;
  delete arg_obj;
  prg->PopTop(old_prg_top);
  cssMisc::PopCurTop(old_top); */
}

void cssiMethMenu::ShowReturnVal(cssEl* rval) {
  if(!css_fun->HasOption("USE_RVAL"))
    return;

  if ((rval->GetType() == cssEl::T_TA) || (rval->GetType() == cssEl::T_Class)) {
    if (host != NULL)
      rval->Edit(host->isModal());
    else
      rval->Edit(false);
    return;
  }
  String val = css_fun->name + " Return Value: ";
  val += rval->PrintStr();
  taiChoiceDialog::ChoiceDialog(NULL, val, "Return Value", true);
}

void cssiMethMenu::ApplyBefore() {
//  if ((host == NULL) || (host->state != IDataHost::ACTIVE))
  if (host == NULL) return;
  if (css_fun->HasOption("NO_APPLY_BEFORE") || !host->HasChanged())
    return;
  if (taMisc::auto_revert == taMisc::CONFIRM_REVERT) {
    int chs = taiChoiceDialog::ChoiceDialog
      (NULL, "Auto Apply/Revert: You have edited the data --apply or revert and lose changes?,&Apply,&Revert");
    if(chs == 0)
      host->GetValue();
  }
  else {
    host->GetValue();
  }
}

void cssiMethMenu::UpdateAfter() {
  if (css_fun->HasOption("NO_REVERT_AFTER"))
     return;
//  if ((host == NULL) || (host->state != IDataHost::ACTIVE))
  if (host == NULL)
    return;
  host->GetImage();		// apply stuff dealt with already
}

void cssiMethMenu::GenerateScript() {
}


