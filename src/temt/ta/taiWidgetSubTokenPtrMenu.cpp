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

#include "taiWidgetSubTokenPtrMenu.h"
#include <taiWidgetMenu>
#include <MemberDef>
#include <taiEdit>


taiWidgetSubTokenPtrMenu::taiWidgetSubTokenPtrMenu(taiWidgetActions::RepType rt, int ft, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                         QWidget* gui_parent_, int flags_)
  : taiWidgetTaBaseMenu(NULL, typ_, host_, par, gui_parent_, flags_)
{
  menubase = NULL;
  ta_actions = taiWidgetActions::New(rt, taiWidgetMenu::radio_update, ft, typ_, host_, this, gui_parent_); //note: only needs taiWidgetMenu, but this doesn't hurt
  ownflag = true;
}

taiWidgetSubTokenPtrMenu::taiWidgetSubTokenPtrMenu(taiWidgetMenu* existing_menu, TypeDef* typ_, IWidgetHost* host_,
                         taiWidget* par, QWidget* gui_parent_, int flags_)
  : taiWidgetTaBaseMenu(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  menubase = NULL;
}

QWidget* taiWidgetSubTokenPtrMenu::GetRep() {
  return ta_actions ? ta_actions->GetRep() : 0;
}

void* taiWidgetSubTokenPtrMenu::GetValue() {
  iAction* cur = ta_actions->curSel();
  if (cur == NULL)
    return NULL;
  else
    return cur->usr_data.toPtr();
}

void taiWidgetSubTokenPtrMenu::Edit() {
  void* cur_base = GetValue();
  if (cur_base == NULL) return;

  taiEdit* gc;
  if(typ->IsActualTaBase()) {
    gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  }
  else {
    gc = (taiEdit*)typ->ie;
  }

  gc->Edit(cur_base, false);
}

void taiWidgetSubTokenPtrMenu::GetImage(const void* ths, void* sel) {
  if (menubase != ths) {
    menubase = (void*)ths;
    UpdateMenu();
  }
  if (sel == NULL)
    sel = (void*)ths;
  if (!(ta_actions->GetImageByData(Variant(sel))))
    ta_actions->GetImageByData(Variant(ths));
}

void taiWidgetSubTokenPtrMenu::UpdateMenu(iMenuAction* actn){
  ta_actions->Reset();
  GetMenu(actn);
}

void taiWidgetSubTokenPtrMenu::GetMenu(iMenuAction* actn) {
  if (HasFlag(flgNullOk))
    ta_actions->AddItem(String::con_NULL, taiWidgetMenu::use_default, actn, (void*)NULL);
  if (HasFlag(flgEditOk))
    ta_actions->AddItem("Edit", taiWidgetMenu::normal,
      iAction::action, this, SLOT(Edit()) );
  ta_actions->AddSep(); // note: never adds spurious seps

  GetMenuImpl(menubase, actn);
}

void taiWidgetSubTokenPtrMenu::GetMenuImpl(void* base, iMenuAction* actn){
  if (base== NULL) return;
  taBase* rbase = (taBase*) base;
  taBase** memb;
  TypeDef* basetd = rbase->GetTypeDef();

  // if you're the right type, put yourself on the list
  if (basetd->DerivesFrom(typ)) {
    String nm = rbase->GetName();
    ta_actions->AddItem(nm, taiWidgetMenu::use_default, actn, rbase);
  }

  // put your typed members on the list
  MemberDef* md;
  for (int i = 0; i <basetd->members.size; ++i){
    md = basetd->members.FastEl(i);
    if (md->type->DerivesFrom(typ) && !(md->HasOption("NO_SUBTYPE"))) {
      memb = (taBase **) md->GetOff((void*)rbase);
      if ((*memb != NULL) && ((void *) *memb != (void *) rbase)) {
        GetMenuImpl((void *) *memb, actn);
      }
    }
  }
}

