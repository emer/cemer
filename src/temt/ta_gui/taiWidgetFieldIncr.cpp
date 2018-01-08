// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiWidgetFieldIncr.h"
#include <iSpinBox>
#include <iClipData>

#include <taMisc>
#include <taiMisc>



taiWidgetFieldIncr::taiWidgetFieldIncr(TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_)
: taiWidget(typ_, host_, par, gui_parent_, flags_)
{
  SetRep( new iSpinBox(gui_parent_) );
  iSpinBox* rep = this->rep();
  // int ht = taiM->text_height(defSize());
  //  rep->setMaximumSize(5 * ht, ht);
  //  rep->setMinimumSize(2*ht, ht);
//   rep->setFixedHeight(ht);

  //note: the taiType will set the max/min
  if (readOnly()) {
    rep->setReadOnly(true);
  } else {
    QObject::connect(m_rep, SIGNAL(valueChanged(int) ),
          this, SLOT(repChanged() ) );
  }
  // cliphandling connections
  QObject::connect(m_rep, SIGNAL(selectionChanged()),
    this, SLOT(selectionChanged() ) );
}

void taiWidgetFieldIncr::GetImage(const int val) {
  rep()->setValue(val);
}

int taiWidgetFieldIncr::GetValue() const {
  return rep()->value();
}

void taiWidgetFieldIncr::selectionChanged() {
  emit_UpdateUi();
}

void taiWidgetFieldIncr::setMinimum(const Variant& min) {
  rep()->setMinimum(min.toInt());
}

void taiWidgetFieldIncr::setMaximum(const Variant& max) {
  rep()->setMaximum(max.toInt());
}

void taiWidgetFieldIncr::setMinCharWidth(int num) {
  rep()->setMinCharWidth(num);
}

void taiWidgetFieldIncr::setCharWidth(int num) {
  rep()->setCharWidth(num);
}


void taiWidgetFieldIncr::this_GetEditActionsEnabled(int& ea) {
  if(!rep()) return;
  if (!readOnly())
    ea |= iClipData::EA_PASTE;
  if (rep()->hasSelectedText()) {
    ea |= (iClipData::EA_COPY);
    if (!readOnly())
      ea |= (iClipData::EA_CUT |  iClipData::EA_DELETE);
  }
}

void taiWidgetFieldIncr::this_EditAction(int ea) {
  if(!rep()) return;
  if (ea & iClipData::EA_CUT) {
    rep()->cut();
  } else if (ea & iClipData::EA_COPY) {
    rep()->copy();
  } else if (ea & iClipData::EA_PASTE) {
    rep()->paste();
  } else if (ea & iClipData::EA_DELETE) {
    rep()->del(); //note: assumes we already qualified with hasSelectedText, otherwise it is a BS
  }
}

void taiWidgetFieldIncr::this_SetActionsEnabled() {
  //TODO: UNDO/REDO
}
