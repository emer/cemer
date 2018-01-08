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

#include "taiWidgetPlusToggle.h"
#include <iCheckBox>

#include <taMisc>
#include <taiMisc>



taiWidgetPlusToggle::taiWidgetPlusToggle(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
  : taiWidgetComposite(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(MakeLayoutWidget(gui_parent_));
  but_rep = NULL;
}

taiWidgetPlusToggle::~taiWidgetPlusToggle() {
//  rep = NULL;
}

void taiWidgetPlusToggle::applyNow() {
  inherited::applyNow();
}

void taiWidgetPlusToggle::InitLayout() {
  inherited::InitLayout();
  but_rep = new iCheckBox(rep());
  AddChildWidget(but_rep, 1);
  if (HasFlag(flgToggleReadOnly)) {
    but_rep->setReadOnly(true);
  }
  else {
    QObject::connect(but_rep, SIGNAL(clicked(bool) ),
                     this, SLOT(applyNow() ) );
   }
}

bool taiWidgetPlusToggle::GetValue() {
  return but_rep->isChecked();
}

void taiWidgetPlusToggle::GetImage(bool chk) {
  but_rep->setChecked(chk);
}

void taiWidgetPlusToggle::SigEmit_impl(taiWidget* chld) {
  but_rep->setChecked(true);
}

