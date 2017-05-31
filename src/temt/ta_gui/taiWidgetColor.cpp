// Copyright 2017, Regents of the University of Colorado,
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

#include "taiWidgetColor.h"
#include <iColorButton>

#include <taMisc>
#include <taiMisc>



taiWidgetColor::taiWidgetColor(TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags)
: inherited(typ_, host_, par, gui_parent_, flags)
{
  iColorButton* rep = new iColorButton(gui_parent_);
  rep->setFixedHeight(taiM->button_height(defSize()));
  SetRep(rep);
  //note: using alpha is the default, must add NO_ALPHA to suppress
  rep->setUseAlpha(!(flags & flgNoAlpha));
  if (flags & flgReadOnly) {
    rep->setEnabled(false);
  } else {
    connect(rep, SIGNAL(colorChanged()), this, SLOT(repChanged()));
  }
}

void taiWidgetColor::GetImage(const iColor& val) {
  rep()->setColor(val);
}

iColor taiWidgetColor::GetValue() const {
  iColor rval(rep()->color());
  return rval;
}

