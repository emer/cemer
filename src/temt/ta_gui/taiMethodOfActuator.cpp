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

#include "taiMethodOfActuator.h"
#include <taiWidgetMethodButton>
#include <taiWidgetMethodMenu>
#include <iMenuButton>



int taiMethodOfActuator::BidForMethod(MethodDef* md, TypeDef* td) {
  if (md->HasOption("BUTTON") || (md->HasOption("MENU")) ||
    (md->HasOption("MENU_BUTTON")))
    return (inherited::BidForMethod(md,td) + 1);
  return 0;
}

taiWidgetMethod* taiMethodOfActuator::GetButtonMethodRep_impl(void* base, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_) {
  taiWidgetMethodButton* rval = new taiWidgetMethodButton(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}

taiWidgetMethod* taiMethodOfActuator::GetMenuMethodRep_impl(void* base, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_) {
  taiWidgetMethodMenu* rval = new taiWidgetMethodMenu(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}
