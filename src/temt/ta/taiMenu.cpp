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

#include "taiMenu.h"

taiMenu::taiMenu(int st, int ft, TypeDef* typ_, IDataHost* host_, taiData* par,
                 QWidget* gui_parent_, int flags_, taiActions* par_menu_)
  : taiActions(st, ft, typ_, host_, par, gui_parent_, flags_, par_menu_, true, NULL)
{
  init();
}

/*nbg taiMenu::taiMenu(int rt, int st, int ft, QWidget* gui_parent_)
: taiData(NULL, NULL, NULL, gui_parent_, 0)
{
  init(rt, st, ft, gui_parent_, NULL);
} */

taiMenu::taiMenu(QWidget* gui_parent_, int st, int ft, QMenu* exist_menu)
  : taiActions(st, ft, NULL, NULL, NULL, gui_parent_, 0, NULL, true, exist_menu)
{
  init();
}

void taiMenu::init()
{
  SetRep(menu());
}

taiMenu::~taiMenu() {
}

void taiMenu::exec(const iPoint& pos) {
  menu()->exec((QPoint)pos);
}

taiAction* taiMenu::insertItem(const char* val, const QObject *receiver, const char* member, const QKeySequence* accel) {
  taiAction* mel = AddItem(val, use_default, taiAction::none, receiver, member);
  if (accel != NULL) mel->setShortcut(*accel);
  return mel;
}

