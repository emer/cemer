// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taiWidgetMenuButton.h"
#include <iMenuButton>

#include <taiMisc>



taiWidgetMenuButton::taiWidgetMenuButton(int st, int ft, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                             QWidget* gui_parent_, int flags_, taiWidgetActions* par_menu_)
  : taiWidgetActions(st, ft, typ_, host_, par, gui_parent_, flags_, par_menu_, true, NULL)
{
  init();
}

void taiWidgetMenuButton::init()
{
  iMenuButton* button = new iMenuButton(gui_parent);
  //note: for taiWidgetEditButton, we don't add the menu to ourself if it is in EditOnly mode
  //  because that seems to interfere with normal pushbutton ability
  if (!HasFlag(flgEditOnly)) {
    button->setMenu(menu());
  }
  taiM->FormatButton(button, _nilString, font_spec);
  button->setFixedHeight(taiM->button_height(defSize()));
  SetRep(button);
}

void taiWidgetMenuButton::Delete() {
  if (!HasFlag(flgEditOnly)) {
    delete m_menu;
    m_menu = NULL;
  }

  inherited::Delete();
  //WE ARE DELETED HERE
}

