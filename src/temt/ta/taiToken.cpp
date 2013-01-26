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

#include "taiToken.h"
#include <taiMenu>
#include <taiObjChooser>
#include <taiEdit>
#include <taiMenuAction>

#include <taMisc>
#include <taiMisc>



taiToken::taiToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host_, taiData* par,
                   QWidget* gui_parent_, int flags_)
  : taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, typ_, host_, this, gui_parent_);
  ownflag = true;
  scope_ref = NULL;
}

void taiToken::Chooser() {
/*nn  QWidget* par_window = (host == NULL) ? NULL : host->widget(); */
  taiObjChooser* chs = taiObjChooser::createInstance(typ, "Tokens of Given Type", scope_ref, NULL);
  chs->setSel_obj(cur_obj); // set initial selection
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj((taBase*)chs->sel_obj()); //TODO: ***DANGEROUS CAST*** -- could possibly be non-taBase type!!!
 /*TODO: can we even do this??? is there ever actions for radio button items???   if ((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select...>") &&
       (ta_actions->cur_sel->men_act != NULL)) {
      ta_actions->cur_sel->usr_data = (void*)chs_obj;
      ta_actions->cur_sel->men_act->Select(ta_actions->cur_sel); // call it!
    }
    else
      ta_actions->setLabel(chs->sel_str());*/
  }
  delete chs;
  // todo: need to return focus after dialog if possible:
  // rep()->window()->setFocus();
  // rep()->setFocus();
}

void taiToken::Edit() {
  void* cur_base = GetValue();
  if(cur_base == NULL) return;

  taiEdit* gc;
  if (typ->IsTaBase()) {
    gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  }
  else {
    gc = (taiEdit*)typ->ie;
  }

  gc->Edit(cur_base, false);
}

void taiToken::GetImage(taBase* ths) {
  GetUpdateMenu();
  setCur_obj(ths, false);
}

void taiToken::GetImage(taBase* ths, TypeDef* new_typ, taBase* new_scope) {
  scope_ref = new_scope;
  typ = new_typ;
  GetImage(ths);
}

void taiToken::GetUpdateMenu(const taiMenuAction* actn) {
  ta_actions->Reset();
  if (ownflag) {
    if (HasFlag(flgEditOk))
      ta_actions->AddItem("Edit...", taiMenu::normal, taiAction::action, this, SLOT(Edit()) );
    ta_actions->AddSep();
    if (HasFlag(flgNullOk)) {
      taiAction* mel = ta_actions->AddItem(String::con_NULL, taiMenu::radio, actn, (void*)NULL);
      mel->connect(taiAction::men_act, this, SLOT(ItemChosen(taiAction*)));
    }
  }
  GetMenu_impl(ta_actions, typ, actn);
}

taBase* taiToken::GetValue() {
  return cur_obj;
}

void taiToken::GetMenu_impl(taiActions* menu, TypeDef* td, const taiMenuAction* actn) {
  if (!td->IsTaBase()) return; // sanity check, so we don't crash...

  if (!td->tokens.keep) {
    menu->AddItem("<Sorry, not keeping tokens>", taiMenu::normal);
  } else {
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
      taiAction* mnel = menu->AddItem
        ("<Over max, select...>", taiMenu::normal,
        taiAction::action, this, SLOT(Chooser()) );
      if (actn != NULL) {               // also set callback action!
        mnel->connect(actn);
      }
    } else {
      taiMenuAction ma(this, SLOT(ItemChosen(taiAction*)));
      String    nm;
      for (int i = 0; i < td->tokens.size; ++i) {
        taBase* btmp = (taBase*)td->tokens.FastEl(i);
        if ((scope_ref != NULL) && !btmp->SameScope(scope_ref))
          continue;
        //TODO: need to get some kind of less ambiguous name
        nm = btmp->GetDisplayName();
        taiAction* mel = menu->AddItem(nm, taiMenu::radio, actn, Variant(btmp)); //connect caller's callback
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
//Qt3      taiMenu* submenu = menu->AddSubMenu(chld->name, (void*)chld);
      taiMenu* submenu = menu->AddSubMenu(chld->name);
//huh?? why??      menu->AddSep();
      GetMenu_impl(submenu, chld, actn);
    } else {
      GetMenu_impl(menu, chld, actn);   // if no tokens, don't add a submenu..
    }
  }
}

void taiToken::ItemChosen(taiAction* menu_el) {
  setCur_obj(menu_el->usr_data.toBase());
}

