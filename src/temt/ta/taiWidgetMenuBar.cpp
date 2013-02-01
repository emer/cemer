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

#include "taiWidgetMenuBar.h"
#include <QMenuBar>

#include <taiMisc>



taiWidgetMenuBar::taiWidgetMenuBar(int ft, TypeDef* typ_, IWidgetHost* host_,
                       taiWidget* par_, QWidget* gui_parent_, int flags_)
  : taiWidgetActions(normal, ft, typ_, host_, par_, gui_parent_, flags_)
{
  init(NULL);
}

taiWidgetMenuBar::taiWidgetMenuBar(QWidget* gui_parent_, int ft, QMenuBar* exist_menu)
  : taiWidgetActions(normal, ft, NULL, NULL, NULL, gui_parent_, 0)
{
  init(exist_menu);
}

taiWidgetMenuBar::~taiWidgetMenuBar() {
}

void taiWidgetMenuBar::init(QMenuBar* exist_menu)
{
  //TODO: would be safer if we used Qt's type system to absolutely confirm that correct type was passed...
  QMenuBar* mrep_bar = (exist_menu) ? exist_menu : new QMenuBar(gui_parent);
  mrep_bar->setFont(taiM->menuFont(font_spec));
  //NOTE: do *not* try to change the height of menubars -- if too small, some platforms
  // turn the menubars into pop-aside menubuttons
  SetRep(mrep_bar);
//  cur_sel = NULL;
}

/* QMenu* taiWidgetMenuBar::NewSubItem(const char* val, QMenu* child, const QKeySequence* accel) {
  QMenu* new_men;
  int itemId;
  itemId = menu()->insertItem(val, child);
  new_men = menu()->findItem(itemId);
// TODO: Font control
  if((font_spec == big) || (font_spec == big_italic)) {
    lbl = new ivLabel(val, taiM->big_menu_font, taiM->font_foreground);
  }
  else {
    lbl = new ivLabel(val, taiM->small_menu_font, taiM->font_foreground);
  }

  return new_men;
}*/

