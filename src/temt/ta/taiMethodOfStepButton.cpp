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

#include "taiMethodOfStepButton.h"
#include <taiProgStepButton>
#include <taiMethMenu>




int taiMethodOfStepButton::BidForMethod(MethodDef* md, TypeDef* td) {
  if (md->HasOption("STEP_BUTTON"))
    return (inherited::BidForMethod(md,td) + 1);
  return 0;
}

taiMethodData* taiMethodOfStepButton::GetButtonMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiProgStepButton* rval = new taiProgStepButton(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}

taiMethodData* taiMethodOfStepButton::GetMenuMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiMethMenu* rval = new taiMethMenu(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}

