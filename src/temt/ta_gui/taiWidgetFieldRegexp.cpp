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

#include "taiWidgetFieldRegexp.h"
#include <iDialogRegexp>
#include <iLineEdit>
#include <MemberDef>


taiWidgetFieldRegexp::taiWidgetFieldRegexp(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, iDialogRegexpPopulator *re_populator)
  : taiWidgetText(typ_, host_, par, gui_parent_, flags_,
            (re_populator != 0), // Add a "..." button if populator provided.
            "Edit this field using a Regular Expression dialog")
  , m_populator(re_populator)
  , m_fieldOwner(0)
{
  setMinCharWidth(40);
}

void taiWidgetFieldRegexp::SetFieldOwner(const void *fieldOwner)
{
  m_fieldOwner = fieldOwner;
}

void taiWidgetFieldRegexp::btnEdit_clicked(bool)
{
  iDialogRegexp edit_dialog(this, mbr == NULL ? "" : mbr->name, m_populator, m_fieldOwner, readOnly());
  edit_dialog.exec();

  // Unless explicitly overridden, do an autoapply.
  if (edit_dialog.applyClicked() && !HasFlag(flgNoEditDialogAutoApply)) {
    leText->emitReturnPressed();
    applyNow();
  }
}

void taiWidgetFieldRegexp::lookupKeyPressed()
{
  // Open the regexp editor if lookup key pressed.
  btnEdit_clicked(true);
}
