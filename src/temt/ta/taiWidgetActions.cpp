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

#include "taiWidgetActions.h"
#include <taiWidgetMenu>
#include <taiWidgetMenuButton>
#include <iActionSubMenuEl>
#include <iMenuAction>
#include <QToolBar>
#include <QToolButton>

#include <taMisc>
#include <taiMisc>

#include <QAbstractButton>

taiWidgetActions* taiWidgetActions::New(RepType rt, int sel_type_, int font_spec_, TypeDef* typ_, IWidgetHost* host,
      taiWidget* par, QWidget* gui_parent_, int flags_, taiWidgetActions* par_menu_)
{
  taiWidgetActions* rval = NULL; //note: switch handles all actual cases, NULL=compiler food
  switch (rt) {
  case popupmenu:
    rval = new taiWidgetMenu(sel_type_, font_spec_, typ_, host, par, gui_parent_, flags_, par_menu_);
    break;
  case buttonmenu:
    rval = new taiWidgetMenuButton(sel_type_, font_spec_, typ_, host, par, gui_parent_, flags_, par_menu_);
    break;
  }
  return rval;
}

taiWidgetActions::taiWidgetActions(int sel_type_, int ft, TypeDef* typ_, IWidgetHost* host_,
                       taiWidget* par_, QWidget* gui_parent_, int flags_,
                       taiWidgetActions* par_menu_, bool has_menu, QMenu* exist_menu)
  : taiWidget(typ_, host_, par_, gui_parent_, flags_)
{
  sel_type = (SelType)sel_type_;
  font_spec = ft;
  gui_parent = gui_parent_;
  cur_grp = NULL;
  cur_sel = NULL;
  par_menu = par_menu_;
  par_menu_el = NULL;
  if (has_menu) {
    m_menu = (exist_menu) ? exist_menu : new QMenu(gui_parent);
    m_menu->setFont(taiM->menuFont(font_spec));
  } else {
    m_menu = NULL;
  }
}

taiWidgetActions::~taiWidgetActions() {
  Reset();
  if (m_menu) {
    m_menu->setParent(NULL); // avoid potential issues
    m_menu->deleteLater();
    m_menu = NULL;
  }
}

void taiWidgetActions::ActionAdded(iAction* it) {
  actionsRep()->addAction(it);
}

void taiWidgetActions::ActionRemoving(iAction* it) {
  QWidget* wid = actionsRep(); // avoid destruct issues
  if (wid)
    wid->removeAction(it);
}

QWidget* taiWidgetActions::actionsRep() {
  if (m_menu) return m_menu;
  else                return GetRep();
}

void taiWidgetActions::AddAction(iAction* act) {
  // if it is a radio item
  if (act->sel_type & (radio | toggle)) {
    act->setCheckable(true);
  }
  // if we have an explicit radio group going, add it
  // this is not done much, only when we explicitly create groups, like for the view menu
  if ((act->sel_type & radio) && (cur_grp != NULL)) {
    cur_grp->addAction(act);
  }
  // font compliance
  act->setFont(taiM->menuFont(font_spec));
  if (act->parent())
    taMisc::DebugInfo("WARNING: iAction has non-null parent!");
  items.Add(act);
  ActionAdded(act);
  connect(act, SIGNAL(MenuAction(iAction*)), this, SLOT(child_triggered_toggled(iAction*)) );
}

// Add an item to the list and connect its action.
iAction* taiWidgetActions::AddItem(
  const String& val,
  SelType st,
  iAction::CallbackType ct_,
  const QObject *receiver,
  const char* member, // 'member' is the result of the SLOT() macro
  const Variant& usr
)
{
  if (st == use_default) {
    st = sel_type;
  }

  iAction* rval;
//TODO: this "no duplicates" was causing token items to not appear
//  determine if allowing duplicates causes regression issues
/*  // do not add items of same name -- return it instead of adding it
  for (int i = 0; i < items.size; ++i) {
    rval = items.FastEl(i);
    if (rval->text() == val) {
      return rval;
    }
  } */
  rval = new iAction(st, val);
  rval->usr_data = usr;
  AddAction(rval);
  rval->connect(ct_, receiver, member);

/*TODO: modify for Qt4
  // connect any parent submenu handler
  if (par_menu_el) {
    if (par_menu_el->default_child_action.receiver) {
      menu()->connectItem(rval->id(), par_menu_el->default_child_action.receiver, par_menu_el->default_child_action.member);
    }
  } */
  return rval;
}

// Add item with (global) keyboard shortcut.
iAction* taiWidgetActions::AddItem(
  const String& val,
  iAction::CallbackType ct,
  const QObject *receiver,
  const char* member,
  const Variant& usr,
  const QKeySequence& shortcut
)
{
  iAction* rval = AddItem(val, use_default, ct, receiver, member, usr);
  if (!shortcut.isEmpty()) rval->setShortcut(shortcut);
  return rval;
}

// Add submenu item with numeric accelerator.
iAction* taiWidgetActions::AddItemWithNumericAccel(
  const String& val,
  iAction::CallbackType ct,
  const QObject *receiver,
  const char* member,
  const Variant& usr
)
{
  // Set accelerator keys of 1,2,3...9,0 for the first 10 items.
  String label;
  int idx = count();
  if (idx < 10) {
    label = "&" + String((idx + 1) % 10) + " " + val;
  }
  else {
    label = "  " + val;
  }

  return AddItem(label, ct, receiver, member, usr);
}

// Convenience function that takes a iMenuAction.
iAction* taiWidgetActions::AddItem(
  const String& val,
  SelType st,
  const iMenuAction* mact,
  const Variant& usr
)
{
  if (mact != NULL)
    return AddItem(val, st, iAction::men_act, mact->receiver, mact->member, usr);
  else
    return AddItem(val, st, iAction::none, NULL, NULL, usr);
}

// Minimal convenience function.
iAction* taiWidgetActions::AddItem(
  const String& val,
  const Variant& usr
)
{
  return AddItem(val, sel_type, iAction::none, NULL, NULL, usr);
}

void taiWidgetActions::AddSep(bool new_radio_grp) {
  if (new_radio_grp) NewRadioGroup();
  //don't double add seps or add at beginning (this check simplifies many callers, so they don't need to know
  //  whether a previous operation added items and/or seps, or not)
  QWidget* ar = actionsRep();
  QAction* it = NULL;
  if (ar->actions().count() > 0)
    it = ar->actions().last();
//  iAction* it = items.Peek();
  if ((it == NULL) || it->isSeparator()) return;

  it = new QAction(ar); //TODO: make sure making the rep as parent for hidden seps won't ever accumulate
  it->setSeparator(true);
  actionsRep()->addAction(it);
}

taiWidgetMenu* taiWidgetActions::AddSubMenu(const String& val, TypeDef* typ_) {
  // do not add items of same label -- return it instead of adding it
  for (int i = 0; i < items.size; ++i) {
    iAction* act = items.FastEl(i);
    if (act->isSubMenu() && (act->text() == val)) {
      return ((iActionSubMenuEl*)act)->sub_menu_data;
    }
  }

  taiWidgetMenu* rval = new taiWidgetMenu(sel_type, font_spec, typ_, host, this,
                                          gui_parent, mflags, this);
  iActionSubMenuEl* sme = new iActionSubMenuEl(val, rval);
  rval->par_menu_el = sme;
  AddAction(sme);
  QToolBar* tb = qobject_cast<QToolBar*>(actionsRep());
  if(tb) {
    tb->setIconSize(QSize(4,4)); // keep the arrows small
    // if we went in a toolbar, change from default MenuButtonPopup to InstantPopup!
    if (QToolButton* but = qobject_cast<QToolButton*>(tb->widgetForAction(sme))) {
      //      but->setPopupMode(QToolButton::MenuButtonPopup); 
      // need to move the damn arrow to the RHS to be consistent, but wtf this will work
      but->setPopupMode(QToolButton::InstantPopup); 
      but->setArrowType(Qt::DownArrow);
      but->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
  }
  return rval;
}

void taiWidgetActions::child_triggered_toggled(iAction* act) {
  // if a radio item in global group, these are the cases:
  // * user manually unchecked it -- not an allowed operation, so  just recheck it
  // * user selected a new item -- just request the change
  if (act->canSelect()) {
    if (!act->isChecked()) {
      iAction* cur = curSel();
      if (act == cur) {
        act->setChecked(true);
        return; // skip data change, because nothing actually changed
      }
    } else {
      // just request the change on select
      setCurSel(act); // gets ignored while already setting, ex. when unsetting the other item
    }
  }

  if (act->sel_type & taiWidgetActions::update) {
    SigEmit();              // something was selected..
  }
}

iAction* taiWidgetActions::curSel() const {
  if (par_menu != NULL)
    return par_menu->curSel();
  else  return cur_sel;
}

void taiWidgetActions::DeleteItem(uint idx) {
  if (idx >= (uint)items.size) return;
  iAction* act = items[idx];
  ActionRemoving(act);
  items.RemoveIdx(idx); // deletes if ref==0
}

void taiWidgetActions::emitLabelChanged(const String& val) {
  emit labelChanged(val.chars());
}

iAction* taiWidgetActions::FindActionByData(const Variant& usr) {
  for (int i = 0; i < items.size; ++i) {
    iAction* itm = items.FastEl(i);
    if (itm->usr_data == usr) return itm;
  }
  return NULL;
}

taiWidgetMenu* taiWidgetActions::FindSubMenu(const String& nm) {
  for (int i = 0; i < items.size; ++i) {
    iAction* itm = items.FastEl(i);
    if (!itm->isSubMenu()) continue;
    iActionSubMenuEl* sme = (iActionSubMenuEl*)itm;
    if (sme->text() == nm)
      return sme->sub_menu_data;
  }
  return NULL;
}

bool taiWidgetActions::GetImageByData(const Variant& usr) {
  // first try to find item by iterating through all eligible items and subitems
  if (GetImage_impl(usr))
      return true;
  return false;
}

bool taiWidgetActions::GetImage_impl(const Variant& usr) {
   // set to this usr item, returns false if not found
  // first, look at our items...
  for (int i = 0; i < items.size; ++i) {
    iAction* itm = items.FastEl(i);
    if (!itm->canSelect()) continue;
    if (itm->usr_data == usr) {
//TODO Qt4: make sure this case is automatically handled now
//      if (usr.isPtrType() && (usr.toPtr() == NULL) && (itm->text() != String::con_NULL))
//      continue;
      setCurSel(itm);
      return true;
    }
  }
  // ...otherwise, recursively descend to submenus
  for (int i = 0; i < items.size; ++i) {
    iAction* itm = items.FastEl(i);
    if (!itm->isSubMenu()) continue;
    iActionSubMenuEl* sme = (iActionSubMenuEl*)itm;
    taiWidgetMenu* sub_menu = sme->sub_menu_data;
    if (sub_menu->GetImage_impl(usr))
      return true;
  }
  return false;
}

void taiWidgetActions::GetImageByIndex(int itm) {
  if ((itm < 0) || (itm >= items.size)) return;
  iAction* mel = items.FastEl(itm);
  setCurSel(mel);
//  Update();
}

void taiWidgetActions::NewRadioGroup() {
  cur_grp = new QActionGroup(gui_parent);
}

void taiWidgetActions::Reset() {
  for (int i = count() - 1; i >= 0; --i) {
    DeleteItem(i);
  }
  // also remove the phantom seps
  cur_sel = NULL;
  QWidget* ar = actionsRep();
  if (ar != NULL)
    ar->actions().clear();
}

void taiWidgetActions::setCurSel(iAction* value) {
  //curSel can only be a global radio type, or null
  if ( (value != NULL) && !value->canSelect() ) return;
  if (par_menu != NULL) {
    par_menu->setCurSel(value);
  } else {
    // controlling root needs to unselect existing element
    if (cur_sel == value) return;
    if (cur_sel != NULL) {
      // need to get it out of curSel so unchecking is allowed by iAction item's handler
      iAction* tmp = cur_sel;
      cur_sel = NULL;
      tmp->setChecked(false);
    }
    cur_sel = value; // need to set it to new, so checking action causes an ignore
    if (cur_sel != NULL) {
      cur_sel->setChecked(true);
      setLabel(cur_sel->text());
    } else { //NOTE: special case of going from legal radio item to no item -- set label to NULL
      setLabel(String::con_NULL);
    }
  }
}

String taiWidgetActions::label() const {
  if (par_menu != NULL)
    return par_menu->label();
  else {
    return mlabel;
  }
}

void taiWidgetActions::setLabel(const String& val) {
  if (par_menu != NULL)
    par_menu->setLabel(val);
  else {
    if (mlabel == val) return;
    mlabel = val;
    // we support a limited number of reps...
    QMenu* menu_ = qobject_cast<QMenu*>(GetRep());
    if (menu_ != NULL) {
      menu_->menuAction()->setText(val);
    }
    else {
      QAbstractButton* pb_ = qobject_cast<QAbstractButton*>(GetRep());
      if (pb_ != NULL) {
        pb_->setText(val);
      }
    }
    emitLabelChanged(mlabel);
  }
}

