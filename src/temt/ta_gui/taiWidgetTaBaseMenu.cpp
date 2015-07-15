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

#include "taiWidgetTaBaseMenu.h"
#include <taiWidgetActions>


taiWidgetTaBaseMenu::taiWidgetTaBaseMenu(taiWidgetActions* actions_, TypeDef* tp, IWidgetHost* host_, taiWidget* par,
                     QWidget* gui_parent_, int flags_)
  : taiWidget(tp, host_, par, gui_parent_, flags_)
{
  cur_obj = NULL;
  ta_actions = actions_;
  ownflag = false;
}

taiWidgetTaBaseMenu::~taiWidgetTaBaseMenu() {
  if (ownflag) {
    delete ta_actions;
  }
  ta_actions = NULL;
}

void taiWidgetTaBaseMenu::SigEmit(taiWidget* chld) {
  if (HasFlag(flgAutoApply))
    applyNow();
  else inherited::SigEmit(chld);
}

void taiWidgetTaBaseMenu::setCur_obj(taBase* value, bool do_chng) {
  if (cur_obj == value) return;
  cur_obj = value;
  ta_actions->GetImageByData(Variant(value));
/*TODO Qt4 -- this should happen automatically...
  if (value == NULL)
    ta_actions->setLabel(String::con_NULL);
  else
    ta_actions->setLabel(value->GetName()); */
  if (do_chng)
    SigEmit(NULL);
}

