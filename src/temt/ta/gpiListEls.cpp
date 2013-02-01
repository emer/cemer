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

#include "gpiListEls.h"
#include <taList_impl>
#include <taiMenu>
#include <taiObjChooser>

#include <taMisc>


gpiListEls::gpiListEls(taiWidgetActions::RepType rt, int ft, taList_impl* lst, TypeDef* typ_, IWidgetHost* host_,
	taiWidget* par, QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ths = lst;
//  ta_actions = new taiMenu(rt, taiMenu::radio_update, ft, typ, (void*)ths, host_, this, gui_parent_);
  ta_actions = taiWidgetActions::New(rt, taiMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  over_max = false;
}

gpiListEls::gpiListEls(taiMenu* existing_menu, taList_impl* lst, TypeDef* typ_,
	IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
: taiElBase(existing_menu, typ_, host_, par, gui_parent_, flags_) {
  ths = lst;
  over_max = false;
}

QWidget* gpiListEls::GetRep() {
  return ta_actions->GetRep();
}

void gpiListEls::GetImage(taList_impl* base_lst, taBase* it) {
  if (ths != base_lst) {
    ths = base_lst;
  }
  UpdateMenu();
  ta_actions->GetImageByData(Variant(it)); // get rid of (void*)!
  setCur_obj(it, false);
}

taBase* gpiListEls::GetValue() {
  taiAction* cur = ta_actions->curSel();
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

void gpiListEls::Edit() {
  taBase* cur_base = GetValue();
  if (cur_base != NULL) {
    cur_base->Edit();
/*obs    if (host != NULL)
      cur_base->Edit(host->modal);
    else
      cur_base->Edit(); */
  }
}

void gpiListEls::Choose() {
  taList_impl* chs_root = ths;
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select>") &&
//      (ta_actions->cur_sel->usr_data != NULL))
//     chs_root = (taList_impl*)ta_actions->cur_sel->usr_data;

  taiObjChooser* chs = taiObjChooser::createInstance(chs_root, "List/Group Objects", true);
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

void gpiListEls::GetMenu(taiWidgetActions* menu, taiMenuAction* actn) {
  if (HasFlag(flgNullOk))
    menu->AddItem("NULL", taiMenu::radio_update, actn, (void*)NULL);
  if (HasFlag(flgEditOk))
    menu->AddItem("Edit", taiMenu::normal,
	taiAction::action, this, SLOT(Edit()));
  if (!HasFlag(flgNoList)) {
    String nm;
    if (ths->GetName() == "")
      nm = "All";
    else
      nm = ths->GetName();
    menu->AddItem(nm, taiMenu::use_default, actn, ths);
  }
  menu->AddSep(); //note: doesn't double add or add at beginning
  GetMenu_impl(ths, menu, actn);
}

void gpiListEls::UpdateMenu(taiMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(ta_actions, actn);
}

void gpiListEls::GetMenu_impl(taList_impl* cur_lst, taiWidgetActions* menu, taiMenuAction* actn) {
  if (cur_lst == NULL)	return;
  if (cur_lst->size >= taMisc::max_menu) 
  {

    taiAction* mnel = menu->AddItem
      ("<Over max, select...>", taiMenu::normal,
	taiAction::action, this, SLOT(Choose()), cur_lst );
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
    menu->AddItem((char*)nm, taiMenu::radio_update, actn, tmp);
  }
}

