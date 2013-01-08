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

#ifndef taiSubMenuEl_h
#define taiSubMenuEl_h 1

// parent includes:
#include <taiAction>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiSubMenuEl: public taiAction { // an action used exclusively to hold a submenu
  Q_OBJECT
  friend class taiActions;
public:
  taiMenu*              sub_menu_data; // the taiMenu for this submenu

  QMenu*                SubMenu()       { return menu(); } // override
  bool                  isSubMenu()     { return true; } // override

protected: // only allowed to be used internally when creating submenus
  void          this_triggered_toggled(bool checked) {} // override submenu items don't signal anything

  taiSubMenuEl(const String& label_, taiMenu* sub_menu_data); //
  ~taiSubMenuEl();

};

#endif // taiSubMenuEl_h
