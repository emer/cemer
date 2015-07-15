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

#include "taiWidgetPlusToggle.h"
#include <iCheckBox>

#include <taMisc>
#include <taiMisc>



taiWidgetPlusToggle::taiWidgetPlusToggle(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
  : taiWidgetComposite(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(MakeLayoutWidget(gui_parent_));
  but_rep = NULL;
  data = NULL;
}

taiWidgetPlusToggle::~taiWidgetPlusToggle() {
//  rep = NULL;
  data = NULL; //note: will be owned/parented elsewise, so it must delete that way
}

void taiWidgetPlusToggle::applyNow() {
  but_rep->setChecked(true);
  inherited::applyNow();
}

void taiWidgetPlusToggle::InitLayout() {
  inherited::InitLayout();
  but_rep = new iCheckBox(rep());
  AddChildWidget(but_rep, taiM->hsep_c);
  if (HasFlag(flgToggleReadOnly)) {
    but_rep->setReadOnly(true);
  }
  else {
    connect(but_rep, SIGNAL(clicked(bool)),
        this, SLOT(Toggle_Callback()) );
  }
}

int taiWidgetPlusToggle::GetValue() {
  return but_rep->isChecked();
}
void taiWidgetPlusToggle::GetImage(bool chk) {
  but_rep->setChecked(chk);
}
void taiWidgetPlusToggle::Toggle_Callback() {
  if (host != NULL)
    host->Changed();
  // SigEmit(); //note: was already remarked out prior to qt port
}

void taiWidgetPlusToggle::SigEmit_impl(taiWidget* chld) {
  but_rep->setChecked(true);
}

