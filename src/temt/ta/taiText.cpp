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

#include "taiText.h"
#include <iLineEdit>
#include <taiClipData>

#include <taMisc>
#include <taiMisc>


#include <QHBoxLayout>
#include <QToolButton>

taiText::taiText(TypeDef* typ_, IWidgetHost* host_, taiData* par, QWidget* gui_parent_, int flags_,
                 bool needs_edit_button, const char *tooltip)
  : taiData(typ_, host_, par, gui_parent_, flags_)
  , leText()
  , btnEdit()
{
  if (needs_edit_button) {
    QWidget* act_par = MakeLayoutWidget(gui_parent_);
    QHBoxLayout* lay = new QHBoxLayout(act_par);
    lay->setMargin(0);
    lay->setSpacing(1);

    leText = new iLineEdit(act_par);
    lay->addWidget(leText, 1);

    btnEdit = new QToolButton(act_par);
    btnEdit->setText("...");
    btnEdit->setToolTip(tooltip);
    btnEdit->setFixedHeight(taiM->text_height(defSize()));
    lay->addWidget(btnEdit);

    SetRep(act_par);
    connect(btnEdit, SIGNAL(clicked(bool)),
      this, SLOT(btnEdit_clicked(bool)) );
  }
  else {
    leText = new iLineEdit(gui_parent_);
    SetRep(leText);
  }

  rep()->setFixedHeight(taiM->text_height(defSize()));
  if (readOnly()) {
    rep()->setReadOnly(true);
  }
  else {
    QObject::connect(rep(), SIGNAL(textChanged(const QString&) ),
          this, SLOT(repChanged() ) );
  }

  // cliphandling connections
  QObject::connect(rep(), SIGNAL(selectionChanged()),
    this, SLOT(selectionChanged() ) );

  QObject::connect(rep(), SIGNAL(lookupKeyPressed()),
                   this, SLOT(lookupKeyPressed()) );
}

iLineEdit* taiText::rep() const { return leText; }

void taiText::GetImage(const String& val) {
  if(!rep()) return;
  rep()->setText(val);
}

String taiText::GetValue() const {
  if(!rep()) return _nilString;
  return rep()->text();
}

void taiText::selectionChanged() {
  emit_UpdateUi();
}

void taiText::setMinCharWidth(int num) {
  rep()->setMinCharWidth(num);
}

void taiText::this_GetEditActionsEnabled(int& ea) {
  if(!rep()) return;
  if (!readOnly())
    ea |= taiClipData::EA_PASTE;
  if (rep()->hasSelectedText()) {
    ea |= (taiClipData::EA_COPY);
    if (!readOnly())
      ea |= (taiClipData::EA_CUT |  taiClipData::EA_DELETE);
  }
}

void taiText::this_EditAction(int ea) {
  if(!rep()) return;
  if (ea & taiClipData::EA_CUT) {
    rep()->cut();
  } else if (ea & taiClipData::EA_COPY) {
    rep()->copy();
  } else if (ea & taiClipData::EA_PASTE) {
    rep()->paste();
  } else if (ea & taiClipData::EA_DELETE) {
    rep()->del(); //note: assumes we already qualified with hasSelectedText, otherwise it is a BS
  }
}

void taiText::this_SetActionsEnabled() {
  //TODO: UNDO/REDO
}

