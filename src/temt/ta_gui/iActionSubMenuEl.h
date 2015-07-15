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

#ifndef iActionSubMenuEl_h
#define iActionSubMenuEl_h 1

// parent includes:
#include <iAction>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetMenu;


class TA_API iActionSubMenuEl: public iAction { // an action used exclusively to hold a submenu
  Q_OBJECT
  friend class taiWidgetActions;
public:
  taiWidgetMenu*              sub_menu_data; // the taiWidgetMenu for this submenu

  QMenu*                SubMenu()   override    { return menu(); }
  bool                  isSubMenu() override    { return true; }

protected: // only allowed to be used internally when creating submenus
  void          this_triggered_toggled(bool checked) {} // override submenu items don't signal anything

  iActionSubMenuEl(const String& label_, taiWidgetMenu* sub_menu_data); //
  ~iActionSubMenuEl();

};

#endif // iActionSubMenuEl_h
