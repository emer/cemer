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

#include "iAction.h"
#include <taiWidgetMenu>
#include <taiWidgetActions>
#include <iMenuAction>

iAction::iAction(int sel_type_, const String& label_)
: QAction(label_, NULL)
{
  init(sel_type_);
}

iAction::iAction(const QString& label_, const QKeySequence& accel, const char* name_)
: QAction(label_, NULL)
{
  init(taiWidgetMenu::use_default);
  setShortcut(accel);
  setObjectName(name_);
}

iAction::iAction(const QString& label_, QObject* receiver, const char* member, const QKeySequence& accel)
: QAction(label_, NULL)
{
  init(taiWidgetMenu::use_default);
  setShortcut(accel);
  connect(action, receiver, member);
}

iAction::iAction(const Variant& usr_data_, const QString& label_, const QKeySequence& accel,
  const char* name_)
: QAction(label_, NULL)
{
  init(taiWidgetMenu::use_default);
  usr_data = usr_data_;
  setShortcut(accel);
  setObjectName(name_);
}

iAction::~iAction() {
}

void iAction::init(int sel_type_)
{
  sel_type = sel_type_;
  //note: we do this here, but also at AddAction time in case we create default here,
  // and it only gets its true sel_type when added
  if (sel_type & (taiWidgetActions::radio | taiWidgetActions::toggle)) {
    setCheckable(true);
  }
  nref = 0;
  m_changing = 0;
  QObject::connect(this, SIGNAL(triggered(bool)), this, SLOT(this_triggered_toggled(bool)) );
//note: we don't want the toggled signal, because this causes us to signal twice in most cases
// the only thing 'triggered' doesn't signal is programmatic changes, which is ok
//  QObject::connect(this, SIGNAL(toggled(bool)), this, SLOT(this_triggered_toggled(bool)) );
}

bool iAction::canSelect() {
  // an item can be the curSel if it is a global radio item
  return ((sel_type & taiWidgetMenu::radio) && (!isGrouped()) && !isSubMenu());
}

bool iAction::isGrouped() {
  return (actionGroup() != NULL);
}

void iAction::connect(CallbackType ct_, const QObject *receiver, const char* member) {
  // connect callback to given
  if ((ct_ == none) || (receiver == NULL) || (member == NULL)) return;

  switch (ct_) {
  case none:
    return;
  case action:
    QObject::connect(this, SIGNAL(Action()), receiver, member);
    break;
  case men_act:
    QObject::connect(this, SIGNAL(MenuAction(iAction*)), receiver, member);
    break;
  case int_act:
    QObject::connect(this, SIGNAL(IntParamAction(int)), receiver, member);
    break;
  case ptr_act:
    QObject::connect(this, SIGNAL(PtrParamAction(void*)), receiver, member);
    break;
  case var_act:
    QObject::connect(this, SIGNAL(VarParamAction(const Variant&)), receiver, member);
    break;
  }
}

void iAction::connect(const iMenuAction* mact) {
  if (mact == NULL) return;
  connect(men_act, mact->receiver, mact->member);
}

void iAction::emitActions() {
// it is possible an action could end up deleting us, so we guard...
  QPointer<iAction> ths = this;
  emit Action();
  if (!ths) return;
  emit MenuAction(ths);
  if (!ths) return;
  emit IntParamAction(ths->usr_data.toInt());
  if (!ths) return;
  emit PtrParamAction(ths->usr_data.toPtr());
  if (!ths) return;
  emit VarParamAction(ths->usr_data);
}

void iAction::this_triggered_toggled(bool checked) {
  if (m_changing > 0) return;
  ++m_changing;
  emitActions(); // will also cause curSel update, and datachanged
  --m_changing;
}
