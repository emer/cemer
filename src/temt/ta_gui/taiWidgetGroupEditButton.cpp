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

#include "taiWidgetGroupEditButton.h"
#include <taGroup_impl>


taiWidgetGroupEditButton::taiWidgetGroupEditButton
(void* base, TypeDef* tp, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
: taiWidgetEditButton(base, NULL, tp, host_, par, gui_parent_, flags_)
{
}

void taiWidgetGroupEditButton::SetLabel() {
  taGroup_impl* gp = (taGroup_impl*)cur_base;
  if (gp == NULL) {
    taiWidgetEditButton::SetLabel();
    return;
  }
  String nm = " Size: ";
  nm += String(gp->size);
  if(gp->gp.size > 0)
    nm += String(".") + String(gp->gp.size);
  if(gp->leaves != gp->size)
    nm += String(".") + String((int) gp->leaves);
  nm += String(" (") + gp->el_typ->name + ")";
  setRepLabel(nm);
}

