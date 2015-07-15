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

#include "taiWidgetToggle.h"
#include <iCheckBox>

#include <taiMisc>


taiWidgetToggle::taiWidgetToggle(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_) :
       taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  SetRep( new iCheckBox(gui_parent_) );
  rep()->setFixedHeight(taiM->label_height(defSize()));

//NOTE: use 'clicked' (gui only) not 'toggled' (gui + code)
  QObject::connect(m_rep, SIGNAL(clicked(bool) ),
        this, SLOT(repChanged() ) );
  if (readOnly()) {
    rep()->setReadOnly(true);
  } else {
    // if ApplyNow, just apply on change, else connect changed signal to our slot
    if (mflags & flgAutoApply)
      QObject::connect(m_rep, SIGNAL(clicked(bool) ),
            this, SLOT(applyNow() ) );
    else
      QObject::connect(m_rep, SIGNAL(clicked(bool) ),
            this, SLOT(repChanged() ) );
  }
}

void taiWidgetToggle::GetImage(bool val) {
  rep()->setChecked(val);
}

bool taiWidgetToggle::GetValue() const {
  return rep()->isChecked();
}

