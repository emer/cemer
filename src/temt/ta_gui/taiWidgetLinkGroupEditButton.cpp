// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taiWidgetLinkGroupEditButton.h"
#include <taiWidgetMethod>
#include <taiMethod>


taiWidgetLinkGroupEditButton::taiWidgetLinkGroupEditButton
(void* base, TypeDef* tp, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
: taiWidgetGroupEditButton(base, tp, host_, par, gui_parent_, flags_)
{
}

void taiWidgetLinkGroupEditButton::GetMethMenus() {
  if(meth_el.size > 0)		// only do this once..
    return;
  String men_nm, lst_men_nm;
  for (int i = 0; i <typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Close"))
      continue;
    String cur_nm = md->OptionAfter("MENU_ON_");
    if(cur_nm != "")
      men_nm = cur_nm;
    // has to be on one of these two menus..
    if ((men_nm != "Object") && (men_nm != "Edit"))
      continue;
    if ((men_nm == "Object") && (md->name != "Edit"))
      continue;
    if((md->name == "DuplicateEl") || (md->name == "Transfer"))
      continue;
    lst_men_nm = men_nm;
    taiWidgetMethod* mth_rep = md->im->GetMenuMethodRep(cur_base, host, this, gui_parent);
    if(mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    mth_rep->AddToMenu(this);
  }
}

