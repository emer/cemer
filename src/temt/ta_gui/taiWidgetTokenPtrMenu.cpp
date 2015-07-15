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

#include "taiWidgetTokenPtrMenu.h"
#include <taiWidgetMenu>
#include <iDialogObjChooser>
#include <taiEdit>
#include <iMenuAction>

#include <taMisc>
#include <taiMisc>



taiWidgetTokenPtrMenu::taiWidgetTokenPtrMenu(taiWidgetActions::RepType rt, int ft, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                   QWidget* gui_parent_, int flags_)
  : taiWidgetTaBaseMenu(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ta_actions = taiWidgetActions::New(rt, taiWidgetMenu::radio_update, ft, typ_, host_, this, gui_parent_);
  ownflag = true;
  scope_ref = NULL;
}

void taiWidgetTokenPtrMenu::Chooser() {
/*nn  QWidget* par_window = (host == NULL) ? NULL : host->widget(); */
  iDialogObjChooser* chs = iDialogObjChooser::createInstance(typ, "Tokens of Given Type", scope_ref, NULL);
  chs->setSel_obj(cur_obj); // set initial selection
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj((taBase*)chs->sel_obj());
  }
  delete chs;
  // todo: need to return focus after dialog if possible:
  // rep()->window()->setFocus();
  // rep()->setFocus();
}

void taiWidgetTokenPtrMenu::Edit() {
  void* cur_base = GetValue();
  if(cur_base == NULL) return;

  taiEdit* gc;
  if (typ->IsActualTaBase()) {
    gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  }
  else {
    gc = (taiEdit*)typ->ie;
  }

  gc->Edit(cur_base, false);
}

void taiWidgetTokenPtrMenu::GetImage(taBase* ths) {
  GetUpdateMenu();
  setCur_obj(ths, false);
}

void taiWidgetTokenPtrMenu::GetImage(taBase* ths, TypeDef* new_typ, taBase* new_scope) {
  scope_ref = new_scope;
  typ = new_typ;
  GetImage(ths);
}

void taiWidgetTokenPtrMenu::GetUpdateMenu(const iMenuAction* actn) {
  ta_actions->Reset();
  if (ownflag) {
    if (HasFlag(flgEditOk))
      ta_actions->AddItem("Edit...", taiWidgetMenu::normal, iAction::action, this, SLOT(Edit()) );
    ta_actions->AddSep();
    if (HasFlag(flgNullOk)) {
      iAction* mel = ta_actions->AddItem(String::con_NULL, taiWidgetMenu::radio, actn, (void*)NULL);
      mel->connect(iAction::men_act, this, SLOT(ItemChosen(iAction*)));
    }
  }
  GetMenu_impl(ta_actions, typ, actn);
}

taBase* taiWidgetTokenPtrMenu::GetValue() {
  return cur_obj;
}

void taiWidgetTokenPtrMenu::GetMenu_impl(taiWidgetActions* menu, TypeDef* td, const iMenuAction* actn) {
  if (!td->IsActualTaBase()) return; // sanity check, so we don't crash...

  if (!td->tokens.keep) {
    menu->AddItem("<Sorry, not keeping tokens>", taiWidgetMenu::normal);
  }
  else {
    bool too_many = false;
    if (scope_ref != NULL) {
      int cnt = taBase::NTokensInScope(td, scope_ref);
      if (cnt > taMisc::max_menu)
        too_many = true;
    }
    else if (td->tokens.size > taMisc::max_menu) {
      too_many = true;
    }
    if (too_many) {
      iAction* mnel = menu->AddItem
        ("<Over max, select...>", taiWidgetMenu::normal,
        iAction::action, this, SLOT(Chooser()) );
      if (actn != NULL) {               // also set callback action!
        mnel->connect(actn);
      }
    }
    else {
      iMenuAction ma(this, SLOT(ItemChosen(iAction*)));
      String    nm;
      for (int i = 0; i < td->tokens.size; ++i) {
        taBase* btmp = (taBase*)td->tokens.FastEl(i);
        if ((scope_ref != NULL) && !btmp->SameScope(scope_ref))
          continue;
        //TODO: need to get some kind of less ambiguous name
        nm = btmp->GetDisplayName();
        iAction* mel = menu->AddItem(nm, taiWidgetMenu::radio, actn, Variant(btmp)); //connect caller's callback
        mel->connect(&ma); // connect our own callback
      }
    } // too_many
  } // !td.tokens.keep

  if (td->children.size == 0)
    return;

  if (td->tokens.size > 0)
    menu->AddSep();

  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    if (chld->IsAnyPtr())
      continue;
    if ((chld->tokens.size == 0) && (chld->tokens.sub_tokens == 0))
      continue;
    if (chld->tokens.size != 0) {
      taiWidgetMenu* submenu = menu->AddSubMenu(chld->name);
      GetMenu_impl(submenu, chld, actn);
    }
    else {
      GetMenu_impl(menu, chld, actn);   // if no tokens, don't add a submenu..
    }
  }
}

void taiWidgetTokenPtrMenu::ItemChosen(iAction* menu_el) {
  setCur_obj(menu_el->usr_data.toBase());
}

