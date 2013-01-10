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

#include "taiPlusToggle.h"

taiPlusToggle::taiPlusToggle(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
  : taiCompData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(MakeLayoutWidget(gui_parent_));
  but_rep = NULL;
  data = NULL;
}

taiPlusToggle::~taiPlusToggle() {
//  rep = NULL;
  data = NULL; //note: will be owned/parented elsewise, so it must delete that way
}

void taiPlusToggle::applyNow() {
  but_rep->setChecked(true);
  inherited::applyNow();
}

void taiPlusToggle::InitLayout() {
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

int taiPlusToggle::GetValue() {
  return but_rep->isChecked();
}
void taiPlusToggle::GetImage(bool chk) {
  but_rep->setChecked(chk);
}
void taiPlusToggle::Toggle_Callback() {
  if (host != NULL)
    host->Changed();
  // DataChanged(); //note: was already remarked out prior to qt port
}

void taiPlusToggle::DataChanged_impl(taiData* chld) {
  but_rep->setChecked(true);
}

