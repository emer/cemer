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

#include "taiWidgetListTypeHierMenu.h"


taiWidgetListTypeHierMenu::taiWidgetListTypeHierMenu(taiWidgetActions::RepType rt, int ft, TypeDef* lstd, TypeDef* typ_, IWidgetHost* host_, 
  taiWidget* par, QWidget* gui_parent_, int flags_)
: taiWidgetTypeHierMenu(rt, ft, typ_, host_, par, gui_parent_, flags_)
{
  lst_typd = lstd;
}

taiWidgetListTypeHierMenu::taiWidgetListTypeHierMenu
(taiWidgetMenu* existing_menu, TypeDef* gtd, TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
: taiWidgetTypeHierMenu(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  lst_typd = gtd;
}

void taiWidgetListTypeHierMenu::GetMenu(taiWidgetActions* menu, iMenuAction* nact) {
  GetMenu_impl(menu, typ, nact);
  menu->AddSep(); //note: won't add a spurious separator if not needed
  GetMenu_impl(menu, lst_typd, nact);	// get group types for this type
}

