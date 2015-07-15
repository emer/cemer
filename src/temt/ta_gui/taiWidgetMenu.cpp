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

#include "taiWidgetMenu.h"
#include <iVec2i>


taiWidgetMenu::taiWidgetMenu(int st, int ft, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                 QWidget* gui_parent_, int flags_, taiWidgetActions* par_menu_)
  : taiWidgetActions(st, ft, typ_, host_, par, gui_parent_, flags_, par_menu_, true, NULL)
{
  init();
}

/*nbg taiWidgetMenu::taiWidgetMenu(int rt, int st, int ft, QWidget* gui_parent_)
: taiWidget(NULL, NULL, NULL, gui_parent_, 0)
{
  init(rt, st, ft, gui_parent_, NULL);
} */

taiWidgetMenu::taiWidgetMenu(QWidget* gui_parent_, int st, int ft, QMenu* exist_menu)
  : taiWidgetActions(st, ft, NULL, NULL, NULL, gui_parent_, 0, NULL, true, exist_menu)
{
  init();
}

void taiWidgetMenu::init()
{
  SetRep(menu());
}

taiWidgetMenu::~taiWidgetMenu() {
}

void taiWidgetMenu::exec(const iVec2i& pos) {
  menu()->exec((QPoint)pos);
}

void taiWidgetMenu::exec(const QPoint& pos) {
  menu()->exec(pos);
}

iAction* taiWidgetMenu::insertItem(const char* val, const QObject *receiver, const char* member, const QKeySequence* accel) {
  iAction* mel = AddItem(val, use_default, iAction::none, receiver, member);
  if (accel != NULL) mel->setShortcut(*accel);
  return mel;
}

