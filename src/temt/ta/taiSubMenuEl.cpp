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

#include "taiSubMenuEl.h"

taiSubMenuEl::taiSubMenuEl(const String& label_, taiMenu* sub_menu_data_)
:taiAction(taiMenu::st_none, label_)
{
  sub_menu_data = sub_menu_data_;
  setMenu(sub_menu_data->menu());
}

taiSubMenuEl::~taiSubMenuEl() {
  if (sub_menu_data != NULL) {
    delete sub_menu_data;
    sub_menu_data = NULL;
  }
}
