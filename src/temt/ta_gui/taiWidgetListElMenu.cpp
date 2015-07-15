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

#include "taiWidgetListElMenu.h"
#include <taList_impl>
#include <taiWidgetMenu>
#include <iDialogObjChooser>

#include <taMisc>


taiWidgetListElMenu::taiWidgetListElMenu(taiWidgetActions::RepType rt, int ft, taList_impl* lst, TypeDef* typ_, IWidgetHost* host_,
	taiWidget* par, QWidget* gui_parent_, int flags_)
: taiWidgetTaBaseMenu(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ths = lst;
//  ta_actions = new taiWidgetMenu(rt, taiWidgetMenu::radio_update, ft, typ, (void*)ths, host_, this, gui_parent_);
  ta_actions = taiWidgetActions::New(rt, taiWidgetMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  over_max = false;
}

taiWidgetListElMenu::taiWidgetListElMenu(taiWidgetMenu* existing_menu, taList_impl* lst, TypeDef* typ_,
	IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
: taiWidgetTaBaseMenu(existing_menu, typ_, host_, par, gui_parent_, flags_) {
  ths = lst;
  over_max = false;
}

QWidget* taiWidgetListElMenu::GetRep() {
  return ta_actions->GetRep();
}

void taiWidgetListElMenu::GetImage(taList_impl* base_lst, taBase* it) {
  if (ths != base_lst) {
    ths = base_lst;
  }
  UpdateMenu();
  ta_actions->GetImageByData(Variant(it)); // get rid of (void*)!
  setCur_obj(it, false);
}

taBase* taiWidgetListElMenu::GetValue() {
  iAction* cur = ta_actions->curSel();
  if (over_max) {
    if ((cur == NULL) || (cur->text() == "<Over max, select...>")
       || (cur->text() == "gp.<Over max, select...>"))
      return cur_obj;
    else
      return cur->usr_data.toBase();
  }
  if(cur != NULL)
    return cur->usr_data.toBase();
  return NULL;
}

void taiWidgetListElMenu::Edit() {
  taBase* cur_base = GetValue();
  if (cur_base != NULL) {
    cur_base->Edit();
/*obs    if (host != NULL)
      cur_base->Edit(host->modal);
    else
      cur_base->Edit(); */
  }
}

void taiWidgetListElMenu::Choose() {
  taList_impl* chs_root = ths;
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select>") &&
//      (ta_actions->cur_sel->usr_data != NULL))
//     chs_root = (taList_impl*)ta_actions->cur_sel->usr_data;

  iDialogObjChooser* chs = iDialogObjChooser::createInstance(chs_root, "List/Group Objects", true);
  //if ((chs_obj != NULL) && !chs_obj->GetName().empty())
    //obs chs->sel_str = chs_obj->GetName();
    chs->setSel_obj(cur_obj);
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj(chs->sel_obj());
/*TODO: even needed???    if((ta_actions->curSel() != NULL) && (ta_actions->curSel()->label == "<Over max, select...>") &&
       (ta_actions->curSel()->men_act != NULL)) {
      ta_actions->curSel()->usr_data = (void*)chs_obj;
      ta_actions->curSel()->men_act->Select(ta_actions->curSel()); // call it!
    }
    else */
   //   ta_actions->setLabel(chs->sel_str());
  }
  delete chs;
}

void taiWidgetListElMenu::GetMenu(taiWidgetActions* menu, iMenuAction* actn) {
  if (HasFlag(flgNullOk))
    menu->AddItem("NULL", taiWidgetMenu::radio_update, actn, (void*)NULL);
  if (HasFlag(flgEditOk))
    menu->AddItem("Edit", taiWidgetMenu::normal,
	iAction::action, this, SLOT(Edit()));
  if (!HasFlag(flgNoList)) {
    String nm;
    if (ths->GetName() == "")
      nm = "All";
    else
      nm = ths->GetName();
    menu->AddItem(nm, taiWidgetMenu::use_default, actn, ths);
  }
  menu->AddSep(); //note: doesn't double add or add at beginning
  GetMenu_impl(ths, menu, actn);
}

void taiWidgetListElMenu::UpdateMenu(iMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(ta_actions, actn);
}

void taiWidgetListElMenu::GetMenu_impl(taList_impl* cur_lst, taiWidgetActions* menu, iMenuAction* actn) {
  if (cur_lst == NULL)	return;
  if (cur_lst->size >= taMisc::max_menu) 
  {

    iAction* mnel = menu->AddItem
      ("<Over max, select...>", taiWidgetMenu::normal,
	iAction::action, this, SLOT(Choose()), cur_lst );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
    return;
  }
  taBase* tmp;
  for (int i = 0; i < cur_lst->size; ++i) {
    tmp = (taBase*)cur_lst->FastEl_(i);
    if (tmp == NULL) continue;
    String nm = tmp->GetName();
    if (nm == "")
      nm = String("[") + String(i) + "]: (" + tmp->GetTypeDef()->name + ")";
    menu->AddItem((char*)nm, taiWidgetMenu::radio_update, actn, tmp);
  }
}

