// Copyright 2017, Regents of the University of Colorado,
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

#include "taiWidgetTypeHierMenu.h"
#include <taiWidgetMenu>


taiWidgetTypeHierMenu::taiWidgetTypeHierMenu(taiWidgetActions::RepType rt, int ft, TypeDef* typ_, IWidgetHost* host_,
                         taiWidget* par, QWidget* gui_parent_, int flags_)
  : taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  ta_actions = taiWidgetActions::New(rt, taiWidgetMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  enum_mode = false;
}

taiWidgetTypeHierMenu::taiWidgetTypeHierMenu(taiWidgetMenu* existing_menu, TypeDef* typ_,
                         IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
  : taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  ta_actions = existing_menu;
  ownflag = false;
  enum_mode = false;
}

taiWidgetTypeHierMenu::~taiWidgetTypeHierMenu() {
  if(ownflag && (ta_actions != NULL)) delete ta_actions;
  ta_actions = NULL;
}

bool taiWidgetTypeHierMenu::AddType_Class(TypeDef* typ_) {
  if (typ_->HasOption("HIDDEN")) return false;
  if (!typ_->IsActualClass()) // only type classes please..
    return false;
  // no templates (since a template is not itself a type)
  if (typ_->IsTemplInst())
    return false;
  // no nested typedefs TODO: find a better way to identify nested typedefs
  if (typ_->name == "inherited") return false;

  // don't clutter list with these..
  if((typ_->members.size==0) && (typ_->methods.size==0) && !(typ_ == &TA_taBase))
    return false;
  return true;
}

bool taiWidgetTypeHierMenu::AddType_Enum(TypeDef* typ_) {
  if (typ_->HasOption("HIDDEN")) return false;
  if (typ_->enum_vals.size == 0) return false; // not an enum type
  return true;
}

int taiWidgetTypeHierMenu::CountChildren(TypeDef* td) {
  int rval = 0;
  TypeDef* chld;
  for (int i = 0; i < td->children.size; ++i) {
    chld = td->children[i];
    if (chld->IsAnyPtr())
      continue;
    ++rval;
  }
  return rval;
}

int taiWidgetTypeHierMenu::CountEnums(TypeDef* td) {
  int rval = 0;
  TypeDef* chld;
  for (int i = 0; i < td->sub_types.size; ++i) {
    chld = td->sub_types.FastEl(i);
    if (!AddType_Enum(chld))
      continue;
    ++rval;
  }
  return rval;
}

void taiWidgetTypeHierMenu::GetImage(TypeDef* ths) {
  ta_actions->GetImageByData(Variant((void*)ths));
}

void taiWidgetTypeHierMenu::GetMenu(const iMenuAction* acn) {
  if (HasFlag(flgNullOk))
    ta_actions->AddItem(String::con_NULL, taiWidgetMenu::use_default, acn, (void*)NULL);
  if (enum_mode)
    GetMenu_Enum_impl(ta_actions, typ, acn);
  else
    GetMenu_impl(ta_actions, typ, acn);
}

QWidget* taiWidgetTypeHierMenu::GetRep() {
  return ta_actions->GetRep();
}

void taiWidgetTypeHierMenu::GetMenu_Enum_impl(taiWidgetActions* menu, TypeDef* typ_, const iMenuAction* acn) {
  // add Enums of this type
  for (int i = 0; i < typ_->sub_types.size; ++i) {
    TypeDef* chld = typ_->sub_types.FastEl(i);
    if (AddType_Enum(chld)) {
      menu->AddItem(typ_->name + "::" + chld->name, taiWidgetMenu::use_default, acn, (void*)chld);
    }
  }
  menu->AddSep(false);
  // add entries for the subclasses
  for (int i = 0; i < typ_->children.size; ++i) {
    TypeDef* chld = typ_->children.FastEl(i);
    if (chld->IsAnyPtr())
      continue;

    if ((CountChildren(chld) > 0) || (CountEnums(chld) > 0))
    {
      taiWidgetMenu* chld_menu = menu->AddSubMenu((char*)chld->name);
      GetMenu_Enum_impl(chld_menu, chld, acn);
    }
  }
}

void taiWidgetTypeHierMenu::GetMenu_impl(taiWidgetActions* menu, TypeDef* typ_, const iMenuAction* acn) {
  // it is hard to do recursive menus, so we just build optimistically, then delete empties
  if (AddType_Class(typ_)) {
    menu->AddItem((char*)typ_->name, taiWidgetMenu::use_default, acn, (void*)typ_);
    menu->AddSep(false); //don't start new radio group
  }
  for (int i = 0; i < typ_->children.size; ++i) {
    TypeDef* chld = typ_->children.FastEl(i);
    if (chld->IsAnyPtr())
      continue;
    if (!AddType_Class(chld)) {
      if (chld->IsTemplInst() && (chld->children.size == 1)) {
        GetMenu_impl(menu, chld->children.FastEl(0), acn);
      }
      continue;
    }

    if (CountChildren(chld) > 0) {
      taiWidgetMenu* chld_menu = menu->AddSubMenu((char*)chld->name);
      GetMenu_impl(chld_menu, chld, acn);
    }  else
      menu->AddItem((char*)chld->name, taiWidgetMenu::use_default, acn, (void*)chld);
  }
}

TypeDef* taiWidgetTypeHierMenu::GetValue() {
  iAction* cur = ta_actions->curSel();
  if (cur) return (TypeDef*)cur->usr_data.toPtr(); return NULL;
}

void taiWidgetTypeHierMenu::UpdateMenu(const iMenuAction* acn) {
  ta_actions->Reset();
  GetMenu(acn);
}

