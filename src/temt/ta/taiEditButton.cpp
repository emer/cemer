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

#include "taiEditButton.h"
#include <taiEdit>
#include <MethodDef>
#include <taiMethod>
#include <taiMethodData>

#include <taiMisc>

#include <QAbstractButton>


taiEditButton* taiEditButton::New(void* base, taiEdit *taie, TypeDef* typ_,
  IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
{
  if (typ_ && (!typ_->IsTaBase() || typ_->HasOption("EDIT_ONLY")))
    flags_ |= flgEditOnly;
  taiEditButton* rval = new taiEditButton(base, taie, typ_,
    host_, par, gui_parent_, flags_);
  return rval;
}

taiEditButton::taiEditButton(void* base, taiEdit *taie, TypeDef* typ_,
                             IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
  : taiButtonMenu(taiMenu::normal_update, taiMisc::fonSmall, typ_, host_, par, gui_parent_, flags_)
{
  cur_base = base;
  ie = taie;    // note: if null, it uses type's ie
  // if true edit-only button, we just wire the action right to the button, and don't make any menu items
  // otherwise,  we create a menu
  if (HasFlag(flgEditOnly)) {
    connect(m_rep, SIGNAL(clicked()),
        this, SLOT(Edit()) );
  }
  SetLabel();
}

taiEditButton::~taiEditButton() {
  delete ie;
  ie = NULL;
  meth_el.Reset();
}

void taiEditButton::SetLabel() {
  String lbl = typ->name;
  if (HasFlag(flgEditOnly)) {
    lbl += ": Edit...";
  }
  else {
    lbl += ": Actions";
  }
  setRepLabel(lbl);
}

void taiEditButton::GetMethMenus() {
  if (meth_el.size > 0)         // only do this once..
    return;
  if (HasFlag(flgEditOnly)) return;

  String men_nm;
  String lst_men_nm;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Close"))
      continue;
    // don't do following unless
    if ((md->name == "CopyFrom") || (md->name == "CopyTo") || (md->name == "DuplicateMe")
       || (md->name == "ChangeMyType") || (md->name == "Help"))
      continue;
    // do menu stuff to stay in sync with directives
    String cur_nm = md->OptionAfter("MENU_ON_");
    if (cur_nm != "")
      men_nm = cur_nm;
    // has to be on one of these two menus..
    if ((men_nm != "Object") && (men_nm != "Edit"))
      continue;
    if (HasFlag(flgReadOnly) && (!md->HasOption("EDIT_READ_ONLY")) ) continue;
    if ((men_nm != lst_men_nm) && (lst_men_nm != ""))
      AddSep();
    lst_men_nm = men_nm;
    taiMethodData* mth_rep = md->im->GetMenuMethodRep(cur_base, host, this, gui_parent);
    if (mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    if ((ie != NULL) && (md->name == "Edit"))
      AddItem("Edit", taiMenu::use_default,
          taiAction::action, this, SLOT(Edit()));
    else
      mth_rep->AddToMenu(this);
  }
}

void taiEditButton::GetImage_impl(const void* base) {
  cur_base = (void*)base; //ok to deconstify
  SetLabel();
  GetMethMenus();
}

void taiEditButton::Edit() {
  if (cur_base == NULL)
    return;
  // note that the target of a pointer is not necessarily readonly just because
  // the pointer itself was readonly... (so we don't propagate ro to target)
  if (ie == NULL) {
    typ->ie->Edit(cur_base, false);
  }
  else {
    ie->typ = typ;
    ie->Edit(cur_base, false);
  }
  GetImage_impl(cur_base);
}

void taiEditButton::setRepLabel(const char* label) {
    rep()->setText(label);
}

